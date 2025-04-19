#pragma once
#include <opencv2/core.hpp>

// Reads the image corresponding to the imageId and stores it in dst. This
// function can be used both for multiple image and video input.
void getImage(cv::Mat& dst, int imageId);

struct ObjectData {
    std::vector<cv::Point> contour;
    int imageId;
    float area;
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
};

void getData();
