#include "utils.hpp"
#include <iostream>

/////////////////////////////////////////////////////////////////////////////////////////
// External variables:
/////////////////////////////////////////////////////////////////////////////////////////

ImageInputMode e_imageInputMode;
ObjectDataInputMode e_objectDataInputMode;

bool e_warningsAsError;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions:
//////////////////////////////////////////////////////////////////////////////////////////////////////

void defaultSettings()
{
    e_imageInputMode = Video;
    e_objectDataInputMode = OneFilePerImage;
    e_warningsAsError = true;
}

std::string makeRed(std::string str)
{
    std::string helper = "\033[31m" + str + "\033[0m";
    return helper;
}

std::string makeYellow(std::string str)
{
    std::string helper = "\033[33m" + str + "\033[0m";
    return helper;
}

std::string makeBlue(std::string str)
{
    std::string helper = "\033[34m" + str + "\033[0m";
    return helper;
}

void splitStringOnChar(std::vector<std::string> dst, const std::string& str, char c)
{
    std::stringstream stream(str);
    std::string segment;
    while (std::getline(stream, segment, c)) {
        dst.push_back(segment);
    }
}

std::string splitAllBeforeLast(const std::string& str, char c)
{
    std::vector<std::string> seglist;
    splitStringOnChar(seglist, str, c);
    std::string res = "";
    for (size_t i = 0; i < seglist.size() - 1; i++) {
        res += seglist[i] + ".";
    }
    return res.substr(0, res.size() - 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Errors:
//////////////////////////////////////////////////////////////////////////////////////////////////////

Error::~Error()
{
    // if (!m_checked) {
    //     std::cout << makeYellow("Error with Code " + std::to_string(m_errorCode) +
    //                             " and message '" + m_name +
    //                             "' not checked before destruction!")
    //               << std::endl;
    // }
}

Error Error::operator=(const Error& other)
{
    Error error(other);
    return other;
}

bool Error::operator==(const Error& other)
{
    return other.m_errorCode == m_errorCode;
}

void Error::addMessage(std::string message)
{
    if (message != "") {
        if (m_message != "")
            m_message += " ";
        m_message += message;
    }
}

bool Error::check(std::string message)
{
    addMessage(message);
    if (m_errorCode == 0) {
        return true;
    } else if (m_errorCode < 0) {
        throw std::runtime_error(
            makeRed("Error: Code=" + std::to_string(m_errorCode) + ", Name=" + m_name +
                    "\nMessage:" + m_message + "\n"));
    } else {
        if (e_warningsAsError) {
            throw std::runtime_error(
                makeYellow("Warning: Code=" + std::to_string(m_errorCode) +
                           ", Name=" + m_name + "\nMessage:" + m_message + "\n"));
        } else {
            std::cout << makeYellow("Warning: Code=" + std::to_string(m_errorCode) +
                                    ", Name=" + m_name + "\nMessage:" + m_message + "\n")
                      << std::endl;
            return true;
        }
    }
}

const Error Error::Success(0, "Success");
const Error Error::InvalidInput(-1, "InvalidInput");
const Error Error::EmptyImage(1, "EmptyImage");
