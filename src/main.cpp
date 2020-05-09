#include <cxxopts.hpp>
#include <fstream>
#include <iostream>
#include <libgen.h>
#include <memory>
#include <sstream>
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

#include "inference.h"

using namespace std;
using namespace photils;

int main(int argc, char *argv[])
{
    try
    {
        auto app_path = fs::path();
#if __APPLE__
        char buff[1024];
        uint32_t size = sizeof(buff);
        if (_NSGetExecutablePath(buff, &size) == 0)
        {
            app_path.assign(buff).remove_filename();
        }
#else
        app_path = fs::read_symlink("/proc/self/exe").remove_filename();
#endif
        cxxopts::Options options("photils-cli", "Extract meaningful keywords of your images");
        options.add_options()("h,help",
                              "print this help")("i,image", "Image to predict keywords", cxxopts::value<std::string>())(
            "o,output_file", "File where to write keywords. Optional", cxxopts::value<std::string>()->default_value(""));

        auto result = options.parse(argc, argv);

        if (result.count("help"))
        {
            std::cout << options.help() << std::endl;
            return EXIT_SUCCESS;
        }

        if (result.count("image"))
        {
            auto image = result["image"].as<std::string>();

            std::ostringstream out;
            Inference::get_instance().set_app_path(app_path);
            int ret = Inference::get_instance().get_tags(image, &out);
            auto outputPath = result["output_file"].as<std::string>();

            if (!outputPath.empty())
            {
                std::ofstream outFile(outputPath);

                if (!outFile)
                    return EXIT_FAILURE;

                outFile << out.str();
            }
            else
            {
                std::cout << out.str();
            }

            std::exit(ret);
        }
    }
    catch (const cxxopts::OptionException &ex)
    {
        std::cerr << ex.what() << '\n';
        std::exit(EXIT_FAILURE);
    }

    std::exit(EXIT_FAILURE);
}
