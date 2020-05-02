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
    : m_http_requests_running(true)
    , m_request_queue()
{
    m_mean = cv::Scalar(103.939, 116.779, 123.68);
    load_model();

    http::client::options options;
    options.follow_redirects(true).cache_resolved(true).timeout(30).always_verify_peer(false);
    m_http_client = std::unique_ptr<http::client>(new http::client(options));
    // m_http_requests = std::thread(&Inference::run_http_requests, this);
}

Inference::~Inference()
{
    m_http_requests_running = false;

    if (m_http_requests.joinable())
        m_http_requests.join();
}

cv::Mat Inference::get_feature(cv::Mat image)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    auto blob = blobFromImage(image, 1, cv::Size(CNN_INPUT_WIDHT, CNN_INPUT_HEIGHT), m_mean, true, false);
    m_cnn.setInput(blob);

    auto feature = m_cnn.forward();
    return feature;

    /*std::unique_lock<std::mutex> lock(m_request_queue_mtx);
    auto feature = m_cnn.forward();
    m_request_queue.emplace(id, feature.clone());
    lock.unlock();
    m_request_queue_cond.notify_one();*/
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
        return;

    m_cnn = readNetFromTensorflow(CNN_MODEL_PATH);
    m_cnn.setPreferableBackend(DNN_BACKEND_OPENCV);
}

void Inference::run_http_requests()
{
    http::client::options options;
    options.follow_redirects(true).cache_resolved(true).timeout(10).always_verify_peer(false);
    http::client client(options);
    http::client::request request(API_URL);

    while (m_http_requests_running)
    {
        std::unique_lock<std::mutex> lock(m_request_queue_mtx);
        while (m_request_queue.empty())
        {
            m_request_queue_cond.wait(lock);
        }

        auto item = m_request_queue.front();
        m_request_queue.pop();
        auto feature = item.second;
        lock.unlock();

        auto total = feature.total() * feature.elemSize();
        std::string feature_encoded;
        feature_encoded.resize(base64::encoded_size(total));
        base64::encode(&feature_encoded[0], reinterpret_cast<char *>(feature.data), total);

        auto json = "{\"feature\": \"" + feature_encoded + "\"}";
        auto response = client.post(request, json, "application/json");
        std::string out = body(response);
        std::cout << "id: " << item.first << "bval: " << out << std::endl;
    }
}
