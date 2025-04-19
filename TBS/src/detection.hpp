#pragma once
#include <opencv4/opencv2/core.hpp>

double getContours(std::vector<std::vector<cv::Point>>& contours, const cv::Mat& frame, const cv::Mat& background);
