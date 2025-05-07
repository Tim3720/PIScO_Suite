#pragma once
#include <sstream>
#include <vector>

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Datatypes:
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cstdint>
#include <string>

enum ImageInputMode : char {
    Video,
    Images,
};

enum ObjectDataInputMode : char {
    OneFile,
    OneFilePerImage,
    OneFilePerObject,
};

/////////////////////////////////////////////////////////////////////////////////////////
// External variables:
/////////////////////////////////////////////////////////////////////////////////////////

extern ImageInputMode e_imageInputMode;
extern ObjectDataInputMode e_objectDataInputMode;

extern bool e_warningsAsError;

/////////////////////////////////////////////////////////////////////////////////////////
// Errors:
/////////////////////////////////////////////////////////////////////////////////////////

// Error struct. Predefined errors are available. An error code of 0 means Success,
// negative values correspond to errors and positve values to warnings. Additionaly,
// messages can be defined which makes debugging easier. Errors print warnings when the
// destructor is called before checking  the error.
struct Error {
   private:
    const std::string m_name;
    std::string m_message;
    const int m_errorCode;
    bool m_checked;

   public:
    Error(int errorCode, std::string name = "")
        : m_name(name), m_errorCode(errorCode), m_checked(false) {};
    Error(const Error& error)
        : m_name(error.m_name), m_message(error.m_message),
          m_errorCode(error.m_errorCode), m_checked(false) {};
    ~Error();

    Error operator=(const Error& other);
    bool operator==(const Error& other);

    static const Error Success;
    static const Error InvalidInput;
    static const Error EmptyImage;

    bool check(std::string message = "");
    void addMessage(std::string message);
};

/////////////////////////////////////////////////////////////////////////////////////////
// Functions:
/////////////////////////////////////////////////////////////////////////////////////////
void defaultSettings();
std::string makeRed(std::string str);
std::string makeYellow(std::string str);
std::string makeBlue(std::string str);
void splitStringOnChar(std::vector<std::string> dst, const std::string& str, char c);
std::string splitAllBeforeLast(const std::string& str, char c);

/////////////////////////////////////////////////////////////////////////////////////////
// Helpers:
/////////////////////////////////////////////////////////////////////////////////////////
#define NCOLS_WITH_CNTS 9
