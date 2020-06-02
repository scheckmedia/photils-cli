#ifndef INFERENCE_H
#define INFERENCE_H

#include "helper.h"

#include <json/json.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/model.h>

#define CNN_MODEL_PATH "../share/photils/model.tflite"
#define CNN_INPUT_WIDHT 224
#define CNN_INPUT_HEIGHT 224
#define LABELS_PATH "../share/photils/labels.json"

namespace photils {
class Inference {
public:
  struct Prediction {
    Prediction(std::string lbl, float conf) : label(lbl), confidence(conf) {}

    std::string label;
    float confidence;

    bool operator<(const Prediction &other) const {
      return confidence < other.confidence;
    }

    bool operator>(const Prediction &other) const {
      return confidence > other.confidence;
    }
  };

  static Inference &get_instance() {
    static Inference instance;
    return instance;
  }

  void set_app_path(fs::path app_path);
  int get_tags(std::string filepath, std::ostringstream *out,
               bool with_confidence);

  Inference(Inference const &) = delete;
  void operator=(Inference const &) = delete;

private:
  Inference() = default;

  int load_model();
  int load_labels();
  int get_predictions(cv::Mat image, std::vector<Prediction> &predictions);

  std::unique_ptr<tflite::FlatBufferModel> m_model;
  std::unique_ptr<tflite::Interpreter> m_interpreter;
  std::vector<std::string> m_labels;
  fs::path m_app_path;
};
} // namespace photils

#endif // INFERENCE_H
