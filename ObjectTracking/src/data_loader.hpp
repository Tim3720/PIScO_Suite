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
    std::vector<cv::Point> contour;
    int imageId;
    float area;
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;

    ObjectData() : contour({}), imageId(-1), area(0), x(0), y(0), w(0), h(0) {};
};

/////////////////////////////////////////////////////////////////////////////////////////
// Functions:
/////////////////////////////////////////////////////////////////////////////////////////

// Reads all data corresponding to the image with imageId from a given file and stores
// the result in dst.
Error getImageData(std::vector<ObjectData>& dst, size_t imageId, std::ifstream& file);
