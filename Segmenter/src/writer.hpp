#pragma once
#include "error.hpp"
#include <H5Cpp.h>
#include <fstream>
#include <opencv2/core.hpp>
#include <vector>

struct SegmenterObject {
    cv::Mat m_crop;
    std::vector<cv::Point> m_contour;
    cv::Rect m_boundingBox;
    size_t m_imageId;
    float m_threshold;
    float m_area;
};

struct Writer {
    std::ofstream m_saveFile;
    std::mutex m_writerLock;

    Writer();

    Error writeObjects(const std::vector<SegmenterObject>& objects);
};

struct H5Writer {
    std::mutex m_writerLock;
    H5::H5File m_saveFile;
    std::string m_filename;

    H5Writer();

    Error writeObjects(const std::vector<SegmenterObject>& objects, size_t imageId,
        const cv::Mat& image);
};
