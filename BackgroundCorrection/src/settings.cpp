#include "settings.hpp"
#include "background_correction.hpp"
#include "error.hpp"
#include "parser.hpp"

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
size_t e_nThreads;

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
    e_nThreads = 6;
}

void loadSettings(char* filename)
{
    std::unordered_map<std::string, std::string> config = parseConfigFile(filename);

    readParameterString(config, e_sourcePath, "sourcePath");
    readParameterString(config, e_savePath, "savePath");
    std::string colormode;
    readParameterString(config, colormode, "colorMode");
    if (colormode == "gray")
        e_colorMode = cv::IMREAD_GRAYSCALE;
    else if (colormode == "rgb")
        e_colorMode = cv::IMREAD_COLOR;
    else
        throw std::runtime_error(makeRed("Invalid colormode: " + colormode));
    readParameterInt(config, e_nBackgroundImages, "nBackgroundImages");
    std::string backgroundCorrectionModel;
    readParameterString(config, backgroundCorrectionModel, "backgroundCorrectionModel");
    if (backgroundCorrectionModel == "minMaxMethod")
        e_backgroundCorrectionModel = minMaxMethod;
    else if (backgroundCorrectionModel == "minMethod")
        e_backgroundCorrectionModel = minMethod;
    else if (backgroundCorrectionModel == "averageMethod")
        e_backgroundCorrectionModel = averageMethod;
    else if (backgroundCorrectionModel == "medianMethod")
        e_backgroundCorrectionModel = medianMethod;
    else
        throw std::runtime_error(
            makeRed("Invalid background correction mode: " + backgroundCorrectionModel));
    readParameterBool(config, e_useMultiChannelSaveMode, "useMultiChannelSaveMode");
    readParameterInt(config, e_nThreads, "nThreads");
}
