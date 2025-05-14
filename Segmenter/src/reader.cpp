#include "reader.hpp"
#include "settings.hpp"
#include <algorithm>
#include <filesystem>

Error getFiles(std::vector<std::string>& files)
{
    try {
        for (const auto& entry : std::filesystem::directory_iterator(e_sourcePath)) {
            if (entry.is_directory()) {
                continue;
            }
            files.push_back(entry.path().string());
        }
    } catch (const std::exception& e) {
        Error error = Error::RuntimeError;
        error.addMessage("Error on getting files from directory '" + e_sourcePath +
                         "'. Error: \n" + e.what());
        return error;
    }

    std::sort(files.begin(), files.end());
    return Error::Success;
}

Error readImage(cv::Mat& image, size_t fileIndex, const std::vector<std::string>& files)
{
    cv::ImreadModes colorMode =
        e_useMultiChannelInputMode ? cv::IMREAD_COLOR : cv::IMREAD_GRAYSCALE;
    try {
        image = cv::imread(files[fileIndex], colorMode);
    } catch (const cv::Exception& e) {
        Error error = Error::RuntimeError;
        error.addMessage(e.what());
        return error;
    }

    if (image.empty()) {
        Error error = Error::EmptyImage;
        error.addMessage("Empty image for file '" + files[fileIndex] + "' at index " +
                         std::to_string(fileIndex));
        return error;
    }

    return Error::Success;
}
