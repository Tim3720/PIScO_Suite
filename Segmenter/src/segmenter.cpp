#include "segmenter.hpp"
#include "reader.hpp"
#include "settings.hpp"
#include "writer.hpp"
#include <chrono>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <thread>

// Runs the segmentation algorithm on the given grayscale image.
Error getObjects(std::vector<SegmenterObject>& dst, const cv::Mat& image,
    size_t imageIndex)
{
    std::vector<std::vector<cv::Point>> contours;
    double threshold;
    try {
        cv::Mat thresh;
        threshold = cv::threshold(image, thresh, 0, 255, cv::THRESH_TRIANGLE);
        cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
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
        object.m_crop = image(object.m_boundingBox).clone();
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

std::string splitFirst(const std::string& s, char delimiter)
{
    std::stringstream ss(s);
    std::string item;
    std::getline(ss, item, delimiter);
    return item;
}

Error getImageIds(std::vector<int>& dst, std::string filename)
{
    filename = splitFirst(splitLast(filename, '/'), '.');

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
            error.addMessage(
                "Error in getImageIds: " + token + "." + std::string(e.what()));
            return error;
        }
        start = end + 1;
    }
    return Error::Success;
}

Error segment(size_t fileIndex, H5Writer& writer, const std::vector<std::string>& files)
{
    std::string filename = files[fileIndex];
    cv::Mat image;
    readImage(image, fileIndex, files).check();

    if (e_useMultiChannelInputMode) {  // image is given in multi-channel mode, i.e. one
                                       // image per channel
        std::vector<int> imageIds;
        getImageIds(imageIds, filename).check();

        // split image into channels
        cv::Mat channels[3];
        cv::split(image, channels);
        // find objects in each valid channel
        for (size_t i = 0; i < imageIds.size(); i++) {
            std::vector<SegmenterObject> objects;
            getObjects(objects, channels[i], imageIds[i]).check();
            // write objects
            writer.writeObjects(objects, imageIds[i], image).check();
        }

    } else {  // image is given in grayscale
        int imageId;
        try {
            imageId = std::stoi(splitFirst(splitLast(filename, '/'), '.'));
        } catch (const std::exception& e) {
            Error error = Error::RuntimeError;
            error.addMessage("Error in segment while converting filename to imageId: " +
                             std::string(e.what()));
            return error;
        }
        // search objects on image:
        std::vector<SegmenterObject> objects;
        getObjects(objects, image, imageId).check();
        // write objects
        writer.writeObjects(objects, imageId, image).check();
    }

    return Error::Success;
}

void segmentThread(size_t startIndex, size_t stopIndex, H5Writer& writer,
    const std::vector<std::string>& files)
{
    size_t fileIndex = startIndex;
    while (fileIndex < stopIndex) {
        segment(fileIndex, writer, files).check();
        fileIndex++;
    }
}

Error runSegmenter()
{
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();
    std::vector<std::string> files;
    getFiles(files).check();

    H5Writer writer;

    std::vector<std::thread> threads;
    threads.resize(e_nThreads - 1);
    size_t fileIndexStep = files.size() / e_nThreads;
    for (size_t i = 0; i < e_nThreads - 1; i++) {
        threads[i] = std::thread(segmentThread, i * fileIndexStep,
            (i + 1) * fileIndexStep, std::ref(writer), files);
    }
    segmentThread(fileIndexStep * (e_nThreads - 1), files.size(), writer, files);

    for (std::thread& t : threads)
        t.join();

    // close save file
    writer.m_saveFile.close();

    double duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::high_resolution_clock::now() - start)
                          .count() /
                      1000.;
    std::cout << "Total runtime: " << duration << "s\n";
    std::cout << "Avg. runtime per image: " << duration / files.size() << "s\n";

    return Error::Success;
}
