#ifndef HELPER_H
#define HELPER_H

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
fs::path get_execution_path();
}

#endif // HELPER_H
