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

enum LoadMode : char {
    oneFile,
    oneFilePerImage,
    oneFilePerObject,
};

enum SaveMode : char {
    oneImagePerObject, // one image per object
    cluster, // clusters object to on larger images, fills an image before starting the next
    clusterPerImage, // same as cluster, but starts a new cluster image if all objects of one source image are drawn on the cluster image
    drawContours, // draws the contours on the source images and saves them under the save path
};

constexpr Info CouldNotOpenFile = nextError;
constexpr Info EndOfFileNotReached = nextError + 1;
constexpr Info CVRuntimeError = nextError + 2;

constexpr Info EmptyFile = nextWarning;
constexpr Info EmptyTask = nextWarning + 1;

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

extern std::string cropDataLoadModeStr;
extern std::string saveModeStr;

// Other:
extern bool enableDetailedPrinting;
extern Int progressBarWidth;

// Helper:
extern LoadMode cropDataLoadMode;
extern SaveMode saveMode;

//////////////////////////////////////////////////////////////

std::string customInfo(Info errorCode);

void readParameters(int argc, char* argv[]);
void progressBar(Int fileIdx, Int filesSize);
void print(std::string str, bool newLine = true, bool ignoreDetailedPrinting = false);
