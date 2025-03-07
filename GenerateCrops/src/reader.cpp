#include "reader.hpp"
#include <iostream>
#include <opencv2/core/types.hpp>
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

FileReader::FileReader(const std::string& fileName, Mode mode)
    : _file(fileName)
{
    if (!_file.is_open()) {
        throw std::runtime_error("Error: Unable to open the file " + fileName);
    }
    std::getline(_file, _header); // skip first line (header)
}

FileReader::~FileReader()
{
    _file.close();
}

bool FileReader::_getNextLine(std::string& line)
{
    return !std::getline(_file, line).fail();
}

bool FileReader::getNextObject(Object& object)
{
    std::string line;
    if (_getNextLine(line)) {
        // line layout: id,imgFileName,x,y,width,h,area,[contour] (optional)
        std::vector<std::string> values;
        values.reserve(8);
        split(line, ',', values);

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
        return true;
    }
    return false;
}

bool FileReader::getNextObjectStack(std::vector<Object>& objects, const size_t& stackSize)
{
    objects.resize(stackSize);
    for (size_t i = 0; i < stackSize; i++) {
        if (!getNextObject(objects[i])) {
            objects.resize(i); // if failed, resize object to last index
            return false;
        }
    }
    return true;
}
