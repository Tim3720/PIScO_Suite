#include "segmenter.hpp"
#include "parser.hpp"
#include "reader.hpp"
#include "settings.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

// Runs the segmentation algorithm on the given grayscale image.
Error getObjects(std::vector<SegmenterObject>& dst, cv::Mat& image, size_t imageIndex)
{
    std::vector<std::vector<cv::Point>> contours;
    double threshold;
    try {
        threshold = cv::threshold(image, image, 0, 255, cv::THRESH_TRIANGLE);
        cv::findContours(image, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    } catch (const cv::Exception& e) {
        Error error = Error::RuntimeError;
        error.addMessage("Error in segment: " + std::string(e.what()));
        return error;
    }

    dst.reserve(contours.size());
    for (size_t i = 0; i < contours.size(); i++) {
        SegmenterObject object;
        object.m_area = cv::contourArea(contours[i]);

        // filter objects:
        if (object.m_area < e_minObjectArea) {
            continue;
        }

        object.m_threshold = threshold;
        object.m_contour = contours[i];
        object.m_boundingBox = cv::boundingRect(contours[i]);
        object.m_imageId = imageIndex;
        dst.push_back(object);
    }
    dst.shrink_to_fit();

    return Error::Success;
}

std::string splitLast(const std::string& s, char delimiter)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delimiter)) { }
    return item;
}

Error getImageIds(std::vector<int>& dst, std::string filename)
{
    filename = splitLast(filename, '.');

    size_t start = 0;
    while (start < filename.size()) {
        auto end = filename.find('_', start);
        if (end == std::string_view::npos)
            end = filename.size();

        std::string token = filename.substr(start, end - start);

        try {
            int value = std::stoi(token);
            dst.push_back(value);
        } catch (const std::exception& e) {
            Error error = Error::RuntimeError;
            error.addMessage("Error in getImageIds: " + std::string(e.what()));
            return error;
        }
        start = end + 1;
    }
    return Error::Success;
}

Error segment(size_t fileIndex, const std::vector<std::string>& files)
{
    std::string filename = files[fileIndex];
    cv::Mat image;
    readImage(image, fileIndex, files).check();

    std::vector<SegmenterObject> objects;
    if (e_useMultiChannelInputMode) {
        std::vector<int> imageIds;
        getImageIds(imageIds, filename).check();

        for (size_t i = 0; i < imageIds.size(); i++) { }

    } else {
        // split
        try {
            int imageId = std::stoi(splitLast(filename, '.'));
        } catch (const std::exception& e) {
            Error error = Error::RuntimeError;
            error.addMessage("Error in segment while converting filename to imageId: " +
                             std::string(e.what()));
            return error;
        }

        getObjects(objects, image, imageId).check();
    }

    return Error::Success;
}
