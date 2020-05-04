#include "inference.h"
#include <chrono>
#include <fstream>
#include <numeric>
#include <sstream>
#include <boost/beast/core/detail/base64.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <opencv2/opencv.hpp>

using namespace photils;
using namespace std::chrono_literals;
namespace base64 = boost::beast::detail::base64;

Inference::Inference()
{
    load_model();
}

std::string Inference::get_tags(std::string filepath)
{
    if (!boost::filesystem::exists(filepath))
    {
        std::cout << "File does not exist!\n" << filepath << std::endl;
        return "";
    }

    auto image = cv::imread(filepath);
    auto feature = get_feature(image);
    std::string tags = tag_request(feature);
    return tags;
}

cv::Mat Inference::get_feature(cv::Mat image)
{
    auto blob =
        blobFromImage(image, 1 / 127.5f, cv::Size(CNN_INPUT_WIDHT, CNN_INPUT_HEIGHT), cv::Scalar(1, 1, 1), true, false);
    m_cnn.setInput(blob);

    auto feature = m_cnn.forward();
    return feature;
}

cv::Mat Inference::decode_image(std::string base64_string)
{
    std::vector<uchar> decoded_string;
    decoded_string.resize(base64::decoded_size(base64_string.size()));
    base64::decode(decoded_string.data(), base64_string.data(), base64_string.size());

    cv::Mat image;
    cv::imdecode(decoded_string, 1, &image);
    return image;
}

std::string Inference::encode_feature(cv::Mat feature)
{
    auto total = feature.total() * feature.elemSize();
    std::string feature_encoded;
    feature_encoded.resize(base64::encoded_size(total));
    base64::encode(&feature_encoded[0], reinterpret_cast<char *>(feature.data), total);
    return feature_encoded;
}

void Inference::load_model()
{
    if (!boost::filesystem::exists(CNN_MODEL_PATH))
    {
        std::cout << "File not found: " << CNN_MODEL_PATH;
        return;
    }

    m_cnn = readNetFromTensorflow(CNN_MODEL_PATH);
    m_cnn.setPreferableBackend(DNN_BACKEND_DEFAULT);
}

size_t Inference::curlWriteFnc(char *ptr, size_t size, size_t nmemb, std::string *stream)
{
    int realsize = size * nmemb;
    stream->append(ptr, realsize);
    return realsize;
}

std::string Inference::tag_request(cv::Mat feature)
{
    std::ostringstream out;
    auto encoded_feature = encode_feature(feature);
    std::string url(API_URL);

    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charset: utf-8");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    if (curl)
    {
        long httpCode(0);
        std::string response;
        std::string errBuffer;

        std::string json = "{\"feature\": \"" + encoded_feature + "\"}";
        curl_easy_setopt(curl, CURLOPT_PROXY, "");
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, &errBuffer);
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(json.c_str()));
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFnc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (std::string *)&response);
        CURLcode res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK && httpCode == 200)
        {
            Json::Value data;
            Json::CharReaderBuilder builder;
            Json::String err;
            const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
            if (!reader->parse(response.c_str(), response.c_str() + response.length(), &data, &err))
            {
                // error handling
            }

            auto tags = data["tags"];

            for (int i = 0; i < tags.size(); ++i)
            {
                auto tag = tags.get(i, "").asString();
                out << tag << std::endl;
            }
        }
    }

    curl_global_cleanup();

    return out.str();
}
