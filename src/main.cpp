#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <boost/program_options.hpp>
#include "inference.h"

using namespace std;
using namespace photils;

namespace po = boost::program_options;

int main(int argc, const char *argv[])
{
    try
    {
        po::options_description desc {"Options"};
        desc.add_options()("help,h", "produce help message")(
            "image,i", po::value<std::string>()->required(), "Image to predict keywords")(
            "output_file,o", po::value<std::string>()->default_value(""), "File where to write keywords. Optional");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
        {
            cout << desc << "\n";
            return EXIT_SUCCESS;
        }

        po::notify(vm);

        if (vm.count("image"))
        {
            auto image = vm["image"].as<std::string>();
            std::ostringstream out;
            int ret = Inference::getInstance().get_tags(image, &out);
            auto outputPath = vm["output_file"].as<std::string>();

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
    catch (const po::error &ex)
    {
        std::cerr << ex.what() << '\n';
        std::exit(EXIT_FAILURE);
    }

    std::exit(EXIT_FAILURE);
}
