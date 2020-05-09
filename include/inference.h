#ifndef INFERENCE_H
#define INFERENCE_H

#define CNN_MODEL_PATH "../share/photils/model.tflite"
#define CNN_INPUT_WIDHT 224
#define CNN_INPUT_HEIGHT 224
#define API_URL "https://api.photils.app/tags"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/model.h>

#include <curl/curl.h>
#include <json/json.h>

#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
#    if __has_include(<filesystem>)
#        define GHC_USE_STD_FS
#        include <filesystem>
namespace fs = std::filesystem;
#    endif
#endif
#ifndef GHC_USE_STD_FS
#    include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif

namespace photils
{
class Inference
{
public:
    static Inference &get_instance()
    {
        static Inference instance;
        return instance;
    }

    void set_app_path(fs::path app_path);
    int get_tags(std::string filepath, std::ostringstream *out);

    Inference(Inference const &) = delete;
    void operator=(Inference const &) = delete;

private:
    Inference() = default;
    static size_t curlWriteFnc(char *ptr, size_t size, size_t nmemb, std::string *stream);
    int load_model();
    int get_feature(cv::Mat image, cv::Mat &feature_out);
    std::string encode_feature(cv::Mat feature);
    int tag_request(cv::Mat feature, std::ostringstream *out);

    std::unique_ptr<tflite::FlatBufferModel> m_model;
    std::unique_ptr<tflite::Interpreter> m_interpreter;
    fs::path m_app_path;
};
} // namespace photils

#endif // INFERENCE_H
