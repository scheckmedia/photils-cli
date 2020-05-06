#include <cxxopts.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include "inference.h"

using namespace std;
using namespace photils;

int main(int argc, char *argv[])
{
    try
    {
        cxxopts::Options options("photils-cli", "Extract meaningful keywords of your images");
        options.add_options()("i,image", "Image to predict keywords", cxxopts::value<std::string>())(
            "o,output_file", "File where to write keywords. Optional", cxxopts::value<std::string>()->default_value(""));

        auto result = options.parse(argc, argv);

        if (result.count("help"))
        {
            std::cout << options.help({"", "Group"}) << std::endl;
            return EXIT_SUCCESS;
        }

        if (result.count("image"))
        {
            auto image = result["image"].as<std::string>();

            std::ostringstream out;
            int ret = Inference::getInstance().get_tags(image, &out);
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
