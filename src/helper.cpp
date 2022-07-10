#define NOMINMAX
#include "helper.h"
#include <cstdlib>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace photils;

fs::path photils::getExecutionPath() {
  auto app_path = fs::path();
#if __APPLE__
  char buff[1024];
  uint32_t size = sizeof(buff);
  if (_NSGetExecutablePath(buff, &size) == 0) {
    app_path.assign(buff).remove_filename();
  }
#elif _WIN32
  TCHAR path[MAX_PATH];
  GetModuleFileName(NULL, path, MAX_PATH);
  app_path.assign(path).remove_filename();
#else
  app_path = fs::read_symlink("/proc/self/exe").remove_filename();
#endif

  return app_path;
}

fs::path photils::getDataHome() {
  auto data_home = fs::path();
#if __APPLE__
  data_home = fs::path(std::getenv("HOME")) / "Library" /
              "Application Support" / "photils";
#elif _WIN32
  data_home = fs::path("USERPROFILE") / ".local" / "share" / "photils";
#else
  data_home = fs::path(std::getenv("HOME")) / ".local" / "share" / "photils";
#endif

  if (!fs::exists(data_home))
    fs::create_directories(data_home);

  return data_home;
}

void photils::centerCrop(cv::Mat& image, cv::Size2i size) {
  int left;
  int top;
  int right;
  int bottom;

  int cx = image.cols / 2;
  int cy = image.rows / 2;

  if (image.cols > image.rows) {
    left = cx - cy;
    right = cx + cy;
    top = 0;
    bottom = image.rows;
  } else {
    left = 0;
    right = image.cols;
    top = cy - cx;
    bottom = cy + cx;
  }

  auto roi = cv::Rect(left, top, right - left, bottom - top);

  if (!size.empty())
    cv::resize(image(roi), image, size);
  else
    image(roi).copyTo(image);
}
