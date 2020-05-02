#ifndef INFERENCE_H
#define INFERENCE_H

//#define BOOST_NETWORK_ENABLE_HTTPS

#define CNN_MODEL_PATH "opencv_model_v2.pb"
#define CNN_INPUT_WIDHT 256
#define CNN_INPUT_HEIGHT 256
#define API_URL "https://api.photils.app/tags"

#include <condition_variable>
#include <memory>
#include <mutex>

#include <thread>
#include <boost/network/protocol/http/client.hpp>
#include <boost/property_tree/ptree.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <queue>

using namespace cv::dnn;
using namespace boost::network;
namespace pt = boost::property_tree;

namespace photils
{
class Inference
{
public:
    ~Inference();
    static Inference &getInstance()
    {
        static Inference instance;
        return instance;
    }

    Inference(Inference const &) = delete;
    void operator=(Inference const &) = delete;

    cv::Mat get_feature(cv::Mat image);
    cv::Mat decode_image(std::string base64_string);
    std::string encode_feature(cv::Mat feature);

private:
    Inference();
    void load_model();
    void run_http_requests();

    cv::dnn::Net m_cnn;
    cv::Scalar m_mean;

    bool m_http_requests_running;
    std::unique_ptr<http::client> m_http_client;
    std::thread m_http_requests;
    std::queue<std::pair<std::string, cv::Mat>> m_request_queue;
    std::mutex m_request_queue_mtx;
    std::condition_variable m_request_queue_cond;
};
} // namespace photils

#endif // INFERENCE_H
