#include "inference.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "base64.h"

using namespace photils;
using namespace std::chrono_literals;

Inference::Inference()
{
    load_model();
}

int Inference::get_tags(std::string filepath, std::ostringstream *out)
{
    if (!std::filesystem::exists(filepath))
    {
        std::cout << "File does not exist!\n" << filepath << std::endl;
        return EXIT_FAILURE;
    }

    auto image = cv::imread(filepath);
    cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
    cv::resize(image, image, cv::Size(CNN_INPUT_WIDHT, CNN_INPUT_HEIGHT), 0, 0, cv::InterpolationFlags::INTER_NEAREST);
    image.convertTo(image, CV_32FC3, 1 / 255.0, -1.0);

    cv::Mat feature;
    int ret = get_feature(image, feature);
    if (ret != kTfLiteOk)
        return EXIT_FAILURE;

    return tag_request(feature, out);
}

int Inference::get_feature(cv::Mat image, cv::Mat &feature_out)
{
    auto input = m_interpreter->typed_tensor<float>(0);

    std::memcpy(input, image.data, image.total() * image.elemSize());
    int ret = m_interpreter->Invoke();

    auto output = m_interpreter->outputs()[0];
    auto result = m_interpreter->tensor(output);

    feature_out.create(result->dims->data[0], result->dims->data[1], CV_32F);
    std::memcpy(feature_out.data, result->data.data, result->bytes);
    return ret;
}

std::string Inference::encode_feature(cv::Mat feature)
{
    auto total = feature.total() * feature.elemSize();
    auto in = std::string(reinterpret_cast<char *>(feature.data), total);
    auto feature_encoded = macaron::Base64::Encode(in);
    return feature_encoded;
}

void Inference::load_model()
{
    if (!std::filesystem::exists(CNN_MODEL_PATH))
    {
        std::cout << "File not found: " << CNN_MODEL_PATH;
        return;
    }

    m_model = tflite::FlatBufferModel::BuildFromFile(CNN_MODEL_PATH);
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder builder(*m_model, resolver);
    builder(&m_interpreter);
    if (m_interpreter->AllocateTensors() != kTfLiteOk)
    {
        // error
    }
}

size_t Inference::curlWriteFnc(char *ptr, size_t size, size_t nmemb, std::string *stream)
{
    int realsize = size * nmemb;
    stream->append(ptr, realsize);
    return realsize;
}

int Inference::tag_request(cv::Mat feature, std::ostringstream *out)
{
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

    if (!curl)
        return EXIT_FAILURE;

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

    if (res != CURLE_OK || httpCode != 200)
        return EXIT_FAILURE;

    Json::Value data;
    Json::CharReaderBuilder builder;
    std::string err;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(response.c_str(), response.c_str() + response.length(), &data, &err))
    {
        return EXIT_FAILURE;
    }

    auto tags = data["tags"];

    for (uint i = 0; i < tags.size(); ++i)
    {
        auto tag = tags.get(i, "").asString();
        *out << tag << std::endl;
    }

    curl_global_cleanup();

    return EXIT_SUCCESS;
}
