#pragma once
#include "utils.hpp"

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

/////////////////////////////////////////////////////////////////////////////////////////
// Datatypes:
/////////////////////////////////////////////////////////////////////////////////////////

// Handles image input for either singular images or videos.
struct ImageInput {
   private:
    cv::VideoCapture m_videoCapture;
    std::vector<std::string> m_imageFileNames;
    const std::string m_filename;

   public:
    ImageInput(const std::string& filename = "");
    // Reads the image corresponding to the imageId and stores it in dst. This function
    // can be used both for multiple image and video input.
    Error getImage(cv::Mat& dst, size_t imageId);
};

// Handles all data describing one object
struct ObjectData {
    std::vector<cv::Point> m_contour;
    int m_imageId;
    float m_area;
    uint16_t m_x;
    uint16_t m_y;
    uint16_t m_w;
    uint16_t m_h;

    ObjectData()
        : m_contour({}), m_imageId(-1), m_area(0), m_x(0), m_y(0), m_w(0), m_h(0) {};
    ObjectData(const ObjectData& other)
        : m_contour(other.m_contour), m_imageId(other.m_imageId), m_area(other.m_area),
          m_x(other.m_x), m_y(other.m_y), m_w(other.m_w), m_h(other.m_h) {};
};

/////////////////////////////////////////////////////////////////////////////////////////
// Functions:
/////////////////////////////////////////////////////////////////////////////////////////

// Reads all data corresponding to the image with imageId from a given file and stores
// the result in dst.
Error getImageData(std::vector<ObjectData>& dst, size_t imageId, std::ifstream& file);
