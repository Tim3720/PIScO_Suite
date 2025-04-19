#pragma once
#include <opencv4/opencv2/highgui.hpp>
#include <string>
#include <vector>

enum ReaderMode : char {
    Images,
    Video,
    Camera,
};

// Struct that handles reading of images. It can read the images from a video or read images from a folder.
// Images from a folder are first sorted by filename. The frameOffset can be set to ignore the first frameOffset images/frames.
struct Reader {
private:
    cv::VideoCapture _videoCapture;
    const std::string _path;
    std::vector<std::string> _filenames;
    size_t _frameId;
    const size_t _maxFrames;
    const ReaderMode _readerMode;
    const char _deviceId;

public:
    Reader(const std::string& path, ReaderMode readerMode, size_t frameOffset = 0, size_t maxFrames = 0, char deviceId = 0);
    ~Reader();

    void skipFrames(size_t nFrames);
    bool getNextImage(cv::Mat& dest);
};
