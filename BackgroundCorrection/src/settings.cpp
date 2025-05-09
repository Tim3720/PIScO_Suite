#include "settings.hpp"
#include "background_correction.hpp"

////////////////////////////////////////////////////////////////////////////////
/// External settings:
////////////////////////////////////////////////////////////////////////////////
std::string e_sourcePath;
std::string e_savePath;
cv::ImreadModes e_colorMode;
size_t e_nBackgroundImages;
std::function<void(const std::vector<cv::Mat>&, cv::Mat&, int, int)>
    e_backgroundCorrectionModel;
bool e_useMultiChannelSaveMode;

////////////////////////////////////////////////////////////////////////////////
/// Functions:
////////////////////////////////////////////////////////////////////////////////
void defaultSettings()
{
    e_sourcePath = "/home/tim/Documents/ArbeitTestData/selection/";
    e_savePath = "/home/tim/Documents/ArbeitTestData/TestResults/";
    e_colorMode = cv::IMREAD_GRAYSCALE;
    e_nBackgroundImages = 10;
    e_backgroundCorrectionModel = minMaxMethod;
    e_useMultiChannelSaveMode = true;
}

void loadSettings(char* filename) { }
