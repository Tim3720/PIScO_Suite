#pragma once
#include "error.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <string>
#include <vector>

Error getFiles(std::vector<std::string>& files);
Error readImage(cv::Mat& image, size_t fileIndex, const std::vector<std::string>& files);
