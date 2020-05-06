#ifndef INFERENCE_H
#define INFERENCE_H

#define CNN_MODEL_PATH "../share/photils/model.tflite"
#define CNN_INPUT_WIDHT 224
#define CNN_INPUT_HEIGHT 224
#define API_URL "https://api.photils.app/tags"

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <opencv2/core/core.hpp>
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/model.h>

#include <curl/curl.h>
#include <json/json.h>

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

    int get_tags(std::string filepath, std::ostringstream *out);

    Inference(Inference const &) = delete;
    void operator=(Inference const &) = delete;

private:
    Inference();
    void load_model();
    void init_curl();
    static size_t curlWriteFnc(char *ptr, size_t size, size_t nmemb, std::string *stream);

    int get_feature(cv::Mat image, cv::Mat &feature_out);
    std::string encode_feature(cv::Mat feature);
    int tag_request(cv::Mat feature, std::ostringstream *out);

    std::unique_ptr<tflite::FlatBufferModel> m_model;
    std::unique_ptr<tflite::Interpreter> m_interpreter;
};
} // namespace photils

#endif // INFERENCE_H
