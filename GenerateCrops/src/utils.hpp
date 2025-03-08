#pragma once
#include "info.hpp"
#include <functional>
#include <opencv2/core/mat.hpp>
#include <string>
#include <strings.h>

//////////////////////////////////////////////////////////////
// Type definitions:
//////////////////////////////////////////////////////////////
typedef size_t Int;

enum SaveMode : char {
    oneFile,
    oneFilePerImage,
    oneFilePerObject,
};

constexpr Info CouldNotOpenFile = nextError;
constexpr Info EndOfFileNotReached = nextError + 1;
constexpr Info CVRuntimeError = nextError + 2;

constexpr Info EmptyFile = nextWarning;

constexpr Info EndOfFile = nextInfo;
constexpr Info FinishedAllFiles = nextInfo + 1;

//////////////////////////////////////////////////////////////
// External variables:
//////////////////////////////////////////////////////////////
// Paths:
extern std::string sourcePath;
extern std::string imageSourcePath;
extern std::string savePath;

// image manipulation settings:
extern Int imageWidth;
extern Int imageHeight;
extern bool resizeToImageWidthHeight;

extern Int numThreads;
extern Int taskBufferSize;

extern std::string saveModeStr;

// Other:
extern bool enableDetailedPrinting;
extern Int progressBarWidth;

// Helper:
extern SaveMode saveMode;

//////////////////////////////////////////////////////////////

std::string customInfo(Info errorCode);

void readParameters(int argc, char* argv[]);
void progressBar(Int fileIdx, Int filesSize);
void print(std::string str, bool newLine = true, bool ignoreDetailedPrinting = false);
