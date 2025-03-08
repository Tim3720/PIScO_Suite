#include "reader.hpp"
#include "info.hpp"
#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

std::ostream& operator<<(std::ostream& out, const Object& object)
{
    out << object.imageFileName << " " << object.boundingBox << " " << object.area << " {";
    for (const cv::Point& point : object.contour) {
        out << point << " ";
    }
    out << "}";
    return out;
}

void split(const std::string& string, char delimiter, std::vector<std::string>& parts)
{
    std::istringstream iss(string);
    std::string value;
    while (std::getline(iss, value, delimiter)) {
        parts.push_back(value);
    }
};

FileReader::FileReader(const std::string& filePath, std::string startWith)
    : _fileIdx(0)
    , _numLines(0)
    , _nextObjectLoaded(false)
{
    _getFiles(filePath, _files, startWith);
    _numFiles = _files.size();

    // for progress:
    if (saveMode == oneFile) {
        checkInfo(_openFile(), enableDetailedPrinting, enableDetailedPrinting);
        std::string line;
        while (_getNextLine(line) == Success) {
            _numLines++;
        }
        _fileIdx--;
    }

    checkInfo(_openFile(), enableDetailedPrinting, enableDetailedPrinting);
}

FileReader::~FileReader()
{
    _file.close();
}

Info FileReader::_openFile()
{
    if (_fileIdx >= _files.size()) {
        return FinishedAllFiles;
    }
    std::string fileName = _files[_fileIdx];
    _fileIdx++;

    // open file needs to be closed before new file is opened
    if (_file.is_open()) {
        _file.close();
    }

    _file.open(fileName);
    // reset line counter
    _currentLine = 0;

    if (!_file.is_open()) {
        std::cout << "Error: Unable to open the file " + fileName << std::endl;
        return CouldNotOpenFile;
    }

    std::string header;
    if (std::getline(_file, header).fail()) { // check if file is empty and skip first line (header)
        std::cout << header << std::endl;
        print(makeYellow("EmptyFile: " + fileName), true, enableDetailedPrinting);
        return EmptyFile;
    }
    return Success;
}

void FileReader::_getFiles(std::string path, std::vector<std::string>& files, const std::string& startWith)
{
    // int c = 0;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            continue;
        }
        if (startWith == "" || entry.path().filename().string().rfind(startWith, 0) == 0) {
            files.push_back(entry.path().string());
            // c++;
        }
        // if (c >= 5)
        //     break;
    }
    print("Found " + std::to_string(files.size()) + " files", true, enableDetailedPrinting);
}

Info FileReader::_getNextLine(std::string& line)
{
    if (!std::getline(_file, line).fail()) {
        _currentLine++;
        return Success;
    }
    return EndOfFile;
}

Info FileReader::getNextObject(Object& object)
{
    if (_nextObjectLoaded) {
        object = _loadedObject;
        _nextObjectLoaded = false;
        return Success;
    }

    std::string line;
    Info info = _getNextLine(line);
    if (info == Success) {
        // line layout: id,imgFileName,x,y,width,h,area,[contour] (optional)
        std::vector<std::string> values;
        values.reserve(8);
        split(line, ',', values);

        try {
            object.imageFileName = values[1];
            object.boundingBox = cv::Rect(std::stoi(values[2]), std::stoi(values[3]), std::stoi(values[4]), std::stoi(values[5]));
            object.area = std::stof(values[6]);

            // check if contours where saved:
            if (values.size() > 7) {
                std::string contoursString = values[7].substr(1, values[7].size() - 2); // remove {}
                std::vector<std::string> points;
                split(contoursString, '|', points);
                size_t delimiterIdx;
                int x, y;
                for (const std::string& point : points) {
                    delimiterIdx = point.find(';');
                    x = std::stoi(point.substr(0, delimiterIdx));
                    y = std::stoi(point.substr(delimiterIdx + 1, point.size()));
                    object.contour.push_back(cv::Point(x, y));
                }
            }
        } catch (std::exception& e) {
            std::cout << makeRed("InvalidFileFormat: " + line) << std::endl;
            return RuntimeError;
        }
        return Success;
    } else
        return info;
}

Info FileReader::getNextObjectStack(std::vector<Object>& objects, const size_t& stackSize)
{
    objects.resize(stackSize);
    Info info;
    for (size_t i = 0; i < stackSize; i++) {
        info = getNextObject(objects[i]);
        if (info != Success) {
            objects.resize(i); // if failed, resize object to last index
            return info;
        }
    }
    return Success;
}

Info FileReader::getNextImage(std::vector<Object>& objects)
{
    Info info;
    if (saveMode == oneFile) {
        Object object;
        // get next object to find filename of the new image
        info = getNextObject(object);
        if (info == EndOfFile) {
            return FinishedAllFiles;
        } else if (info != Success) {
            return info;
        }

        std::string currentFileName = object.imageFileName;
        objects.push_back(object);
        while (true) {
            info = getNextObject(object);
            checkInfo(info, enableDetailedPrinting, enableDetailedPrinting);
            if (info != Success) {
                break;
            }
            if (object.imageFileName == currentFileName) {
                objects.push_back(object);
            } else {
                _nextObjectLoaded = true;
                _loadedObject = object;
                break;
            }
        }
        return Success;
    } else if (saveMode == oneFilePerImage) {
        while (true) {
            Object object;
            info = getNextObject(object);
            checkInfo(info, enableDetailedPrinting, enableDetailedPrinting);
            if (info != Success)
                break;
            objects.push_back(object);
        }
        if (info == EndOfFile) {
            info = _openFile();
        } else {
            info = EndOfFileNotReached;
        }
        return info;
    } else {
        std::cout << makeRed("Not implemented") << std::endl;
        return RuntimeError;
    }

    return Success;
}

void FileReader::progress()
{
    if (saveMode == oneFile) {
        progressBar(_currentLine, _numLines);
    } else {
        progressBar(_fileIdx, _numFiles);
    }
}

size_t FileReader::getNumFiles()
{
    return _numFiles;
}
