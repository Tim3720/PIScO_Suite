#include "reader.hpp"
#include "info.hpp"
#include <filesystem>
#include <stdexcept>

Reader::Reader(const std::string& path, ReaderMode readerMode, size_t frameOffset, size_t maxFrames, char deviceId)
    : _path(path)
    , _frameId(0)
    , _maxFrames(maxFrames)
    , _readerMode(readerMode)
    , _deviceId(deviceId)
{
    if (_readerMode == Video) {
        bool result = _videoCapture.open(_path);
        if (!result) {
            throw std::runtime_error(makeRed("Could not open Video at path " + _path));
        }
    } else if (_readerMode == Camera) {
        bool result = _videoCapture.open(_deviceId);
        if (!result) {
            throw std::runtime_error(makeRed("Could not open Camera with id " + std::to_string(_deviceId)));
        }
    } else {
        if (std::filesystem::is_directory(_path)) {
            // read filenames:
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.is_directory()) {
                    continue;
                }
                _filenames.push_back(entry.path().string());
            }
            // sort filenames to ensure right order of images
            std::sort(_filenames.begin(), _filenames.end());
        } else {
            throw std::runtime_error(makeRed("Invalid image directory: " + _path));
        }
    }
    skipFrames(frameOffset);
}

Reader::~Reader()
{
    if (_readerMode == Images) {
        _filenames.clear();
    } else {
        _videoCapture.release();
    }
}

void Reader::skipFrames(size_t nFrames)
{
    size_t start = _frameId;
    bool result = true;
    while (result && _frameId - start < nFrames) {
        _frameId++;
        if (_readerMode == Images) {
            if (_frameId >= _filenames.size())
                result = false;
        } else {
            result = _videoCapture.grab();
        }
    }
}

bool Reader::getNextImage(cv::Mat& dest)
{
    if (_maxFrames > 0 && _frameId >= _maxFrames) {
        return false;
    }
    if (_readerMode == Images) {
        if (_frameId < _filenames.size()) {
            dest = cv::imread(_filenames[_frameId]);
            _frameId++;
            return true;
        }
        return false;
    } else {
        _frameId++;
        return _videoCapture.read(dest);
    }
}
