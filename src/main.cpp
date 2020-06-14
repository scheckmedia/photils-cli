#include <cxxopts.hpp>
#include <fstream>
#include <iostream>
#include <libgen.h>
#include <memory>
#include <sstream>

#include "helper.h"
#include "inference.h"

using namespace std;
using namespace photils;

#define VERSION "0.2b"

int main(int argc, char *argv[]) {
  try {
    auto app_path = get_execution_path();
    std::string desc =
        "Extract meaningful keywords of your images - Version: " +
        std::string(VERSION);

    cxxopts::Options options("photils-cli", desc);
    options.add_options()("h,help", "print this help")(
        "v,version", "print version")("i,image", "Image to predict keywords",
                                      cxxopts::value<std::string>())(
        "o,output_file", "File where to write keywords. Optional",
        cxxopts::value<std::string>()->default_value(""))(
        "c,with_confidence",
        "If flag is used a confidence value for each prediction is printed as "
        "well. Optional",
        cxxopts::value<bool>()->default_value("false"));

    auto result = options.parse(argc, argv);

    // if I call photils-cli -v then argc = 1 and length of argv = 2
    // that's the reason for manual counting
    int num_args = 0;
    while (argv[num_args] != NULL)
      ++num_args;

    if (num_args == 1 || result.count("help")) {
      std::cout << options.help() << std::endl;
      return EXIT_SUCCESS;
    }

    if (result.count("version")) {
      std::cout << VERSION << std::endl;
      return EXIT_SUCCESS;
    }

    if (result.count("image")) {
      auto image = result["image"].as<std::string>();

      std::ostringstream out;
      auto with_confidence = result["with_confidence"].as<bool>();
      Inference::get_instance().set_app_path(app_path);
      int ret =
          Inference::get_instance().get_tags(image, &out, with_confidence);
      auto outputPath = result["output_file"].as<std::string>();

      if (!outputPath.empty()) {
        std::ofstream outFile(outputPath);

        if (!outFile)
          return EXIT_FAILURE;

        outFile << out.str();
      } else {
        std::cout << out.str();
      }

      std::exit(ret);
    }
  } catch (const cxxopts::OptionException &ex) {
    std::cerr << ex.what() << '\n';
    std::exit(EXIT_FAILURE);
  }

  std::exit(EXIT_FAILURE);
}
