#pragma once
#include <cstdint>
#include <opencv2/core/mat.hpp>
#include <vector>

struct SegmenterObject {
  std::vector<cv::Point> contour; // Vector of points that enclose the object
  cv::Rect boundingRect; // Bounding rect of the object in the (resized)
  // image. If the image was resized, the bounding box for the original image
  // can be computed with the imageSize that will be saved
  size_t fileIdx;    // File index of the image the object was found on.
  float area;        // contour area of object
  size_t id;         // Unique object id
  uint8_t threshold; // threshold used on background corrected image to
                     // find object
};
