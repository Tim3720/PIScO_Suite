#include "reader.hpp"
#include "settings.hpp"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <opencv2/core.hpp>
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

void readImages(const std::vector<std::string>& files)
{
    cv::Mat img;
    // cv::Mat channels[3] = {};
    // std::chrono::time_point start = std::chrono::high_resolution_clock::now();
    // for (const std::string& file : files) {
    //     img = cv::imread(file, cv::IMREAD_COLOR);
    //     cv::split(img, channels);
    // }
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();
    for (const std::string& file : files) {
        img = cv::imread(file, cv::IMREAD_GRAYSCALE);
    }

    double duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::high_resolution_clock::now() - start)
                          .count() /
                      1000.;
    std::cout << "Total duration: " << duration << "s\n";
    std::cout << "Avg time per img: " << duration / files.size() << "s\n";
}
