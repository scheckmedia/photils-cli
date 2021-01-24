#include "helper.h"
#include <cstdlib>
#include <opencv2/highgui.hpp>

using namespace photils;

fs::path photils::get_execution_path()
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

    return app_path;
}

fs::path photils::get_data_home()
{
    auto data_home = fs::path();
#if __APPLE__
    data_home = fs::path(std::getenv("HOME")) / "Library" / "Application Support" / "photils";
#else
    data_home = fs::path(std::getenv("HOME")) / ".local" / "share" / "photils";
#endif

    if (!fs::exists(data_home))
        fs::create_directories(data_home);

    return data_home;
}

cv::Mat photils::get_preview_image(const std::string &filepath, const cv::Size2i minSize)
{
    try
    {
        auto image = Exiv2::ImageFactory::open(filepath);
        if (image.get() == nullptr || !image.get()->good())
            return cv::Mat();

        image->readMetadata();

        Exiv2::PreviewManager loader(*image);
        Exiv2::PreviewPropertiesList list = loader.getPreviewProperties();

        if (list.empty())
            return cv::Mat();

        cv::Size2i bestSize(-1, -1);
        Exiv2::PreviewProperties previewProperty;
        for (const auto &previewItem : list)
        {
            if (previewItem.width_ < minSize.width || previewItem.height_ < minSize.height)
                continue;

            if (previewItem.width_ < bestSize.width || previewItem.height_ < bestSize.height)
            {
                previewProperty = previewItem;
                bestSize.width = previewItem.width_;
                bestSize.height = previewItem.height_;
            }
        }

        Exiv2::PreviewImage preview = loader.getPreviewImage(previewProperty);
        auto data = cv::Mat(1, preview.size(), CV_8UC1, const_cast<Exiv2::byte *>(preview.pData()));
        return cv::imdecode(data, cv::IMREAD_UNCHANGED);
    }
    catch (Exiv2::Error &err)
    {
        return cv::Mat();
    }
}