#pragma once

#include "helper.h"

#include <json/json.h>
#include <opencv2/core/core.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>

#define CNN_INPUT_SIZE 224
#define CNN_MODEL_PATH "../share/photils/photils.onnx"
#define LABELS_PATH "../share/photils/labels.json"
#define CNN_MEAN cv::Scalar(0.485f, 0.456f, 0.406f);
#define CNN_STD cv::Scalar(0.229f, 0.224f, 0.225f);

namespace photils {
struct Prediction {
  Prediction(std::string lbl, float conf) : label(lbl), confidence(conf) {}

  std::string label;
  float confidence;

  bool operator<(const Prediction& other) const {
    return confidence < other.confidence;
  }

  bool operator>(const Prediction& other) const {
    return confidence > other.confidence;
  }
};
class Inference {
 public:
  static Inference& getInstance() {
    static Inference instance;
    instance.mAppPath = photils::getExecutionPath();

    return instance;
  }

  std::vector<Prediction> getTags(std::string filepath, bool with_confidence);

  Inference(Inference const&) = delete;
  void operator=(Inference const&) = delete;

 private:
  Inference() = default;

  int loadModel();
  int loadLabels();
  int getPredictions(const cv::Mat& image,
                     std::vector<Prediction>& predictions);
  int prepareImage(std::string& filepath, cv::Mat& dest);
  void loadOverrideLabels();

  cv::dnn::Net mModel;
  std::vector<std::string> mLabels;
  std::map<std::string, std::string> mLabelsOverride;
  fs::path mAppPath;
};
}  // namespace photils
