#pragma once
#include "error.hpp"
#include <opencv2/core.hpp>

struct SegmenterObject {
    std::vector<cv::Point> m_contour;
    cv::Rect m_boundingBox;
    size_t m_imageId;
    float m_threshold;
    float m_area;
};

Error segment(SegmenterObject& dst, cv::Mat& image, size_t fileIndex);
