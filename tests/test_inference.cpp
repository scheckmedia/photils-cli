#include <cstdio>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include "gtest/gtest.h"
#include "helper.h"
#include "inference.h"

TEST(Inference, getTags) {
  auto path = photils::getExecutionPath()
                  .parent_path()
                  .parent_path()
                  .parent_path()
                  .parent_path();
  auto testImgPath = path / "tests" / "data" / "may-kiron105mm-0012.jpg";
  auto& instance = photils::Inference::getInstance();

  auto tags = instance.getTags("no_file", false);
  ASSERT_EQ(tags.size(), 0);

  tags = instance.getTags(testImgPath, false);
  ASSERT_TRUE(tags.size() > 0);

  auto cmp = [](photils::Prediction p) {
    return p.label == "ladybug";
  };
  auto hasLadyBug = std::any_of(tags.begin(), tags.end(), cmp);
  ASSERT_TRUE(hasLadyBug);
}

TEST(Inference, getTagsFromNEF) {
  auto overrideLoc = photils::getDataHome() / "override_labels.json";
  auto path = photils::getExecutionPath()
                  .parent_path()
                  .parent_path()
                  .parent_path()
                  .parent_path();
  auto testImgPath = path / "tests" / "data" / "_DSC2555.NEF";
  auto& instance = photils::Inference::getInstance();

  auto tags = instance.getTags(testImgPath, false);
  ASSERT_TRUE(tags.size() > 0);

  auto cmp = [](photils::Prediction p) {
    return p.label == "flower";
  };
  auto hasFlower = std::any_of(tags.begin(), tags.end(), cmp);
  ASSERT_TRUE(hasFlower);
}

TEST(Inference, labelOverride) {
  auto overrideLoc = photils::getDataHome() / "override_labels.json";
  std::ofstream out(overrideLoc);
  out << std::string("{\"flower\":\"wtf\"}");
  out.close();

  auto path = photils::getExecutionPath()
                  .parent_path()
                  .parent_path()
                  .parent_path()
                  .parent_path();

  auto testImgPath = path / "tests" / "data" / "_DSC2555.NEF";
  auto& instance = photils::Inference::getInstance();

  auto tags = instance.getTags(testImgPath, false);
  ASSERT_TRUE(tags.size() > 0);

  auto cmp = [](photils::Prediction p) {
    return p.label == "wtf";
  };
  auto hasOverrideLabel = std::any_of(tags.begin(), tags.end(), cmp);
  ASSERT_TRUE(hasOverrideLabel);

  if (fs::exists(overrideLoc)) {
    std::remove(overrideLoc.c_str());
  }
}
