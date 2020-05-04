#include <cstdlib>
#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/program_options.hpp>
#include "inference.h"

using namespace std;
using namespace photils;

namespace logging = boost::log;
namespace po = boost::program_options;

void initLog()
{
    logging::add_file_log("sample.log");
    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::trace);
    logging::add_console_log(std::cout, boost::log::keywords::format = ">> %Message%");
}

int main(int argc, const char *argv[])
{
    initLog();
    try
    {
        po::options_description desc {"Options"};
        desc.add_options()("image", po::value<std::string>()->required(), "Image to predict keywords");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("image"))
        {
            auto image = vm["image"].as<std::string>();
            std::cout << Inference::getInstance().get_tags(image);
        }
    }
    catch (const po::error &ex)
    {
        std::cerr << ex.what() << '\n';
        std::exit(EXIT_FAILURE);
    }

    std::exit(EXIT_SUCCESS);
}
