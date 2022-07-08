#pragma once
#include <opencv2/core.hpp>

#if __APPLE__
#include <mach-o/dyld.h>
#endif

#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
#if __has_include(<filesystem>)
#define GHC_USE_STD_FS
#include <filesystem>
namespace fs = std::filesystem;
#endif
#endif
#ifndef GHC_USE_STD_FS
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif

namespace photils {
fs::path getExecutionPath();
fs::path getDataHome();
void centerCrop(cv::Mat& image, cv::Size2i size = cv::Size2i());
}  // namespace photils
