#ifndef INFERENCE_H
#define INFERENCE_H

//#define BOOST_NETWORK_ENABLE_HTTPS

#define CNN_MODEL_PATH "../share/photils/model.pb"
#define CNN_INPUT_WIDHT 224
#define CNN_INPUT_HEIGHT 224
#define API_URL "https://api.photils.app/tags"

#include <condition_variable>
#include <memory>
#include <mutex>

#include <thread>
#include <boost/property_tree/ptree.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <queue>

#include <curl/curl.h>
#include <json/json.h>

using namespace cv::dnn;
namespace pt = boost::property_tree;

namespace photils
{
class Inference
{
public:
    static Inference &getInstance()
    {
        static Inference instance;
        return instance;
    }

    std::string get_tags(std::string filepath);

    Inference(Inference const &) = delete;
    void operator=(Inference const &) = delete;

private:
    Inference();
    void load_model();
    void init_curl();
    static size_t curlWriteFnc(char *ptr, size_t size, size_t nmemb, std::string *stream);

    cv::Mat get_feature(cv::Mat image);
    cv::Mat decode_image(std::string base64_string);
    std::string encode_feature(cv::Mat feature);
    std::string tag_request(cv::Mat feature);

    cv::dnn::Net m_cnn;
};
} // namespace photils

#endif // INFERENCE_H
