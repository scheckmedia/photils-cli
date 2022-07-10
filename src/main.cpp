#include <cxxopts.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "helper.h"
#include "inference.h"
#include "version.h"

using namespace std;
using namespace photils;

int main(int argc, char* argv[]) {
  try {
    std::string desc =
        "Extract meaningful keywords of your images - Version: " +
        std::string(PROJECT_VER);

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
      std::cout << PROJECT_VER << std::endl;
      return EXIT_SUCCESS;
    }

    if (result.count("image")) {
      auto image = result["image"].as<std::string>();

      auto& inference = Inference::getInstance();
      auto with_confidence = result["with_confidence"].as<bool>();
      auto predictions = inference.getTags(image, with_confidence);
      auto outputPath = result["output_file"].as<std::string>();

      std::ostringstream out;
      for (auto pred : predictions) {
        out << pred.label;
        if (with_confidence) {
          out << ":" << std::to_string(pred.confidence);
        }
        out << std::endl;
      }

      if (!outputPath.empty()) {
        std::ofstream outFile(outputPath);

        if (!outFile)
          return EXIT_FAILURE;

        outFile << out.str();
      } else {
        std::cout << out.str();
      }

      std::exit(predictions.size() > 0 ? EXIT_SUCCESS : EXIT_FAILURE);
    }
  } catch (const cxxopts::OptionException& ex) {
    std::cerr << ex.what() << '\n';
    std::exit(EXIT_FAILURE);
  }

  std::exit(EXIT_FAILURE);
}
