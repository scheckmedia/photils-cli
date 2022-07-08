#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include "gtest/gtest.h"
#include "helper.h"

TEST(Helper, getExecutionPath) {
  auto path = photils::getExecutionPath();
  ASSERT_TRUE(fs::exists(path));
  //ASSERT_TRUE(fs::exists(path / "photils-cli"));
}

TEST(Helper, getDataHome) {
  auto path = photils::getDataHome();
  ASSERT_TRUE(fs::exists(path));
}

TEST(Helper, centerCrop) {
  auto path = photils::getExecutionPath()
                  .parent_path()
                  .parent_path()
                  .parent_path()
                  .parent_path();
  auto testImgPath = path / "tests" / "data" / "may-kiron105mm-0012.jpg";
  auto img = cv::imread(testImgPath);

  auto sourceImg = img.clone();
  photils::centerCrop(img, {128, 128});
  ASSERT_TRUE(sourceImg.size() != img.size());
  ASSERT_TRUE(sourceImg.elemSize() == img.elemSize());
  ASSERT_TRUE(sourceImg.data != img.data);
}
