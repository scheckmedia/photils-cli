#include "inference.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <sstream>

using namespace photils;

std::vector<Prediction> Inference::getTags(std::string filepath,
                                           bool with_confidence) {

  cv::Mat image;
  std::vector<Prediction> predictions;

  if (prepareImage(filepath, image) != EXIT_SUCCESS)
    return predictions;

  if (loadLabels() != EXIT_SUCCESS)
    return predictions;

  if (mModel.empty() && loadModel() != EXIT_SUCCESS)
    return predictions;

  loadOverrideLabels();

  getPredictions(image, predictions);
  return predictions;
}

int Inference::getPredictions(const cv::Mat& image,
                              std::vector<Prediction>& predictions) {
  cv::Mat blob;
  cv::dnn::blobFromImage(image, blob);
  mModel.setInput(blob);

  auto result = mModel.forward();
  for (int i = 0; i < result.total(); ++i) {
    auto conf = result.at<float>(i);
    auto label = mLabels.at(i);

    if (mLabelsOverride.count(label)) {
      label = mLabelsOverride[label];
    }

    auto pred = Prediction(label, conf);
    predictions.push_back(pred);
  }

  std::sort(predictions.begin(), predictions.end(), std::greater<>());
  return EXIT_SUCCESS;
}

int Inference::prepareImage(std::string& filepath, cv::Mat& dest) {
  if (!fs::exists(filepath)) {
    std::cout << "File does not exist: " << filepath << std::endl;
    return EXIT_FAILURE;
  }

  auto image = cv::imread(filepath);

  if (image.empty())
    return EXIT_FAILURE;

  // center_crop(image, cv::Size2i(CNN_INPUT_SIZE, CNN_INPUT_SIZE));

  cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
  cv::resize(image, image, {CNN_INPUT_SIZE, CNN_INPUT_SIZE});
  image.convertTo(dest, CV_32FC3, 1 / 255.0);

  dest -= CNN_MEAN;
  dest /= CNN_STD;

  return EXIT_SUCCESS;
}

int Inference::loadModel() {
  auto model_path = mAppPath / fs::path(CNN_MODEL_PATH);

  if (!fs::exists(model_path)) {
    std::cout << "Model file not found: " << model_path.string();
    return EXIT_FAILURE;
  }

  mModel = cv::dnn::readNet(model_path.string());
  mModel.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
  mModel.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

  if (mModel.empty())
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

int Inference::loadLabels() {
  auto label_path = mAppPath / fs::path(LABELS_PATH);
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
    mLabels.push_back(label);
  }

  if (mLabels.size() == 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

void Inference::loadOverrideLabels() {
  auto data_home = getDataHome();
  auto label_path = data_home / "override_labels.json";
  auto labels_data = std::ifstream(label_path);

  if (!fs::exists(label_path)) {
    return;
  }

  Json::Value data;
  Json::CharReaderBuilder json_builder;
  std::string err;
  if (!Json::parseFromStream(json_builder, labels_data, &data, &err)) {
    return;
  }

  for (auto const& id : data.getMemberNames()) {
    auto label = id;
    auto override = data[id].asString();
    mLabelsOverride.insert({id, data[id].asString()});
  }
}
