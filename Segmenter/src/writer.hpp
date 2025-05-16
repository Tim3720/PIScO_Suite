#pragma once
#include "error.hpp"
#include <fstream>
#include <opencv2/core.hpp>
#include <vector>

struct SegmenterObject {
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
