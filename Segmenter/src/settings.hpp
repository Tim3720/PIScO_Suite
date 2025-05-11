#pragma once
#include "parser.hpp"
#include <functional>
#include <opencv2/imgcodecs.hpp>
#include <string>

////////////////////////////////////////////////////////////////////////////////
/// External settings:
////////////////////////////////////////////////////////////////////////////////
extern std::string e_sourcePath;
extern std::string e_savePath;
// extern cv::ImreadModes e_colorMode;
// extern size_t e_nBackgroundImages;  // This has to be an even number!
// extern std::function<void(const std::vector<cv::Mat>&, cv::Mat&, int, int)>
//     e_backgroundCorrectionModel;
// extern bool e_useMultiChannelSaveMode;
// extern size_t e_nThreads;

////////////////////////////////////////////////////////////////////////////////
/// Functions:
////////////////////////////////////////////////////////////////////////////////
void loadSettings(char* filename);
void defaultSettings();
