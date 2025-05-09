#include "reader.hpp"
#include "settings.hpp"
#include <filesystem>
#include <iostream>
#include <opencv2/imgcodecs.hpp>

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

Error getImage(cv::Mat& img, size_t fileIndex, const std::vector<std::string>& files)
{
    if (fileIndex >= files.size()) {
        Error error = Error::RuntimeError;
        error.addMessage("File index given to getImage larger than size of files: " +
                         std::to_string(fileIndex) +
                         " >= " + std::to_string(files.size()));
    }
    const std::string filename = files[fileIndex];
    try {
        img = cv::imread(filename, e_colorMode);
    } catch (const cv::Exception& e) {
        Error error = Error::RuntimeError;
        error.addMessage(e.what());
    }

    if (img.empty()) {
        Error error = Error::EmptyImage;
        error.addMessage("Empty image for file '" + filename + "' at index " +
                         std::to_string(fileIndex));
        return error;
    }
    return Error::Success;
}
