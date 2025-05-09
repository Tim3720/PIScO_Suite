#pragma once
#include "error.hpp"
#include <opencv2/core.hpp>
#include <vector>

Error getFiles(std::vector<std::string>& files);

// Reads the image corresponding to the filename at the given index and stores it in
// img.image.
Error getImage(cv::Mat& img, size_t fileIndex, const std::vector<std::string>& files);
