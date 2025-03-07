#pragma once
#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <vector>

/*
 *  This code implements a reader class that is designed to read and handle large amount of data. The data is expected to be in the output format of the SegmenterParallel code.
 */

// SegmenterParallel style Object that holds all possible output data
struct Object {
    std::vector<cv::Point> contour;
    std::string imageFileName;
    cv::Rect boundingBox;
    float area;
};

std::ostream& operator<<(std::ostream& out, const Object& object);

enum Mode {
    OneFile,
    OneFilePerImage,
    OneFilePerObject,
};

struct FileReader {
private:
    std::ifstream _file;
    std::string _header;

    bool _getNextLine(std::string& line);

public:
    FileReader(const std::string& fileName, Mode mode);
    ~FileReader();
    bool getNextObject(Object& object);
    bool getNextObjectStack(std::vector<Object>& objects, const size_t& stackSize);
};
