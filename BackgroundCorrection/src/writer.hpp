#pragma once
#include <cstddef>
#include <opencv2/core.hpp>
#include <string>
#include <vector>

void writeFileNameDecoding(const std::vector<std::string>& files);

struct Writer {
    std::vector<cv::Mat> channels;
    cv::Mat color;
    int counter;

    Writer();

    // Writes the given image at the save path under the unqiue name fileIndex.png.
    void writeImage(const cv::Mat& img, size_t fileIndex, bool lastImage = false);
};
