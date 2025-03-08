#pragma once
#include "utils.hpp"
#include <fstream>
#include <info.hpp>
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

struct FileReader {
private:
    std::ifstream _file;
    Object _loadedObject;
    std::vector<std::string> _files;
    size_t _fileIdx;
    size_t _numFiles;
    size_t _numLines;
    size_t _currentLine;
    bool _nextObjectLoaded;

    Info _openFile();
    Info _getNextLine(std::string& line);
    void _getFiles(std::string path, std::vector<std::string>& files, const std::string& startWith = "");

public:
    FileReader(const std::string& fileName, std::string startWith = "");
    ~FileReader();
    Info getNextObject(Object& object);
    Info getNextObjectStack(std::vector<Object>& objects, const size_t& stackSize);
    Info getNextImage(std::vector<Object>& objects);
    size_t getNumFiles();
    void progress();
};
