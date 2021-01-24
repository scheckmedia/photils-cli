#ifndef HELPER_H
#define HELPER_H

#include <exiv2/exiv2.hpp>
#include <opencv2/core.hpp>

#if __APPLE__
#    include <mach-o/dyld.h>
#endif

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
fs::path get_execution_path();
fs::path get_data_home();
cv::Mat get_preview_image(const std::string &filepath, const cv::Size2i minSize);
}

#endif // HELPER_H
