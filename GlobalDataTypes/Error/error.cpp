#include "error.hpp"
#include <iostream>

bool e_warningsAsError = true;

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

Error Error::operator=(Error& other)
{
    Error error(other);
    other.m_checked = true;
    return error;
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
const Error Error::RuntimeError(-1, "Runtime error");
const Error Error::InvalidInput(-2, "InvalidInput");
const Error Error::EmptyImage(1, "EmptyImage");
