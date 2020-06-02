#include "inference.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <sstream>

using namespace photils;

void Inference::set_app_path(fs::path app_path) { this->m_app_path = app_path; }

int Inference::get_tags(std::string filepath, std::ostringstream *out,
                        bool with_confidence) {
  if (!fs::exists(filepath)) {
    std::cout << "File does not exist!\n" << filepath << std::endl;
    return EXIT_FAILURE;
  }

  if (load_labels() != EXIT_SUCCESS)
    return EXIT_FAILURE;

  if (m_interpreter == nullptr && load_model() != EXIT_SUCCESS)
    return EXIT_FAILURE;

  auto image = cv::imread(filepath);
  if (image.empty())
    return EXIT_FAILURE;

  cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
  cv::resize(image, image, cv::Size(CNN_INPUT_WIDHT, CNN_INPUT_HEIGHT), 0, 0,
             cv::InterpolationFlags::INTER_NEAREST);
  image.convertTo(image, CV_32FC3, 1 / 255.0, -1.0);

  std::vector<Prediction> predictions;
  int ret = get_predictions(image, predictions);
  if (ret != kTfLiteOk)
    return EXIT_FAILURE;

  for (auto pred : predictions) {
    *out << pred.label;
    if (with_confidence)
      *out << ":" << std::to_string(pred.confidence);
    *out << std::endl;
  }

  return ret;
}

int Inference::get_predictions(cv::Mat image,
                               std::vector<Prediction> &predictions) {
  auto input = m_interpreter->typed_tensor<float>(0);

  std::memcpy(input, image.data, image.total() * image.elemSize());
  int ret = m_interpreter->Invoke();

  auto output = m_interpreter->outputs()[0];
  auto result = m_interpreter->tensor(output);

  std::vector<float> pred_vector(
      result->data.f, result->data.f + result->bytes / sizeof(float));

  for (int i = 0; i < pred_vector.size(); ++i) {
    auto conf = pred_vector.at(i);
    auto pred = Prediction(m_labels.at(i), conf);

    predictions.push_back(pred);
  }

  std::sort(predictions.begin(), predictions.end(), std::greater<>());
  return ret;
}

int Inference::load_model() {
  auto model_path = m_app_path / fs::path(CNN_MODEL_PATH);

  if (!fs::exists(model_path)) {
    std::cout << "Model file not found: " << model_path.string();
    return EXIT_FAILURE;
  }

  m_model = tflite::FlatBufferModel::BuildFromFile(model_path.string().c_str());
  tflite::ops::builtin::BuiltinOpResolver resolver;
  tflite::InterpreterBuilder builder(*m_model, resolver);
  builder(&m_interpreter);
  if (m_interpreter->AllocateTensors() != kTfLiteOk) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int Inference::load_labels() {
  auto label_path = m_app_path / fs::path(LABELS_PATH);
  auto labels_data = std::ifstream(label_path);

  if (!fs::exists(label_path)) {
    std::cout << "Labels file not found: " << label_path.string();
    return EXIT_FAILURE;
  }

  Json::Value data;
  Json::CharReaderBuilder json_builder;
  std::string err;
  if (!Json::parseFromStream(json_builder, labels_data, &data, &err)) {
    return EXIT_FAILURE;
  }

  for (uint i = 0; i < data.size(); ++i) {
    auto label = data.get(i, "").asString();
    m_labels.push_back(label);
  }

  if (m_labels.size() == 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
