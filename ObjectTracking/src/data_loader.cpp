#include "data_loader.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <sstream>
#include <string>

ImageInput::ImageInput(const std::string& filename) : m_filename(filename)
{
    if (e_imageInputMode == Video) {
        if (!m_videoCapture.open(m_filename))
            throw std::runtime_error(makeRed("Could not open video at " + filename));
    }
}

Error ImageInput::getImage(cv::Mat& dst, size_t imageId)
{
    if (e_imageInputMode == Video) {
        if (!m_videoCapture.isOpened()) {
            Error error = Error::InvalidInput;
            error.addMessage("ImageInput::getImage: VideoCapture not opened.");
            return error;
        }

        if (!m_videoCapture.read(dst)) {
            Error error = Error::InvalidInput;
            error.addMessage(
                "ImageInput::getImage: Could not read frame from videCapture.");
            return error;
        }
        return Error::Success;

    } else if (e_imageInputMode == Images) {
        if (imageId >= m_imageFileNames.size()) {
            Error error = Error::InvalidInput;
            error.addMessage(
                "ImageInput::getImage: Could not read image, ID out of bounds.");
            return error;
        }
        dst = cv::imread(m_imageFileNames[imageId]);
        return Error::Success;

    } else {
        Error error = Error::InvalidInput;
        error.addMessage("ImageInput::getImage: Invalid input mode for ImageInput.");
        return error;
    }
}

void decodeObjectLine(ObjectData& dst, std::string element, size_t idx)
{
    switch (idx) {
        case 0:  // ID
            break;
        case 1:  // Image filename
            break;
        case 2:  // x
            dst.x = std::stoi(element);
            break;
        case 3:  // y
            dst.y = std::stoi(element);
            break;
        case 4:  // width
            dst.w = std::stoi(element);
            break;
        case 5:  // height
            dst.h = std::stoi(element);
            break;
        case 6:  // contour area
            dst.area = std::stof(element);
            break;
        case 7:  // threshold
            break;
        case 8:  // contour
            std::vector<std::string> contours;
            // TODO: remove leading { and trailing }, then split at |, followed by a split
            // at ;. Then convert the result to cv::Point and add it to the dst.contour
            // vector
            break;
    }
}

Error getImageData(std::vector<ObjectData>& dst, size_t imageId, std::ifstream& file)
{
    if (!file.is_open()) {
        Error error = Error::InvalidInput;
        error.addMessage("getImageData: Could not open file.");
        return error;
    }

    if (e_objectDataInputMode == OneFile) {
    } else if (e_objectDataInputMode == OneFilePerImage) {
        // new file was opened, start on top:
        std::string line;

        // get header line to check if contours where stored:
        if (!std::getline(file, line)) {
            Error error = Error::InvalidInput;
            error.addMessage("getImageData: Could not read header line.");
            return error;
        }
        std::vector<std::string> segments;
        splitStringOnChar(segments, line, ',');
        size_t nCols = segments.size();

        size_t idx = 0;
        ObjectData objectData;
        while (std::getline(file, line, ',')) {
            idx %= nCols;

            decodeObjectLine(objectData, line, idx);

            idx++;
            // TODO:
            if (idx == nCols) {  // add new object:
            }
        }
    } else {
        throw std::runtime_error(makeRed("OneFilePerObject input not implemented"));
    }

    return Error::Success;
}
