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

void loadSettings(char* filename)
{
    std::unordered_map<std::string, std::string> settings = parseConfigFile(filename);

    readParameterString(settings, e_sourcePath, "sourcePath");
    if (e_sourcePath.back() != '/')
        e_sourcePath += "/";

    readParameterString(settings, e_savePath, "savePath");
    if (e_savePath.back() != '/')
        e_savePath += "/";

    std::string colorModeHelper;
    readParameterString(settings, colorModeHelper, "colorMode");
    if (colorModeHelper == "gray")
        e_colorMode = cv::IMREAD_GRAYSCALE;
    else if (colorModeHelper == "rgb")
        e_colorMode = cv::IMREAD_COLOR;
    else
        throw std::runtime_error(
            makeRed("Invalid value for setting colorMode: " + colorModeHelper));
    // TODO: Implement single/multi channel mode

    readParameterInt(settings, e_nBackgroundImages, "nBackgroundImages");

    std::string bgCorrectionModelHelper;
    readParameterString(settings, bgCorrectionModelHelper, "backgroundCorrectionModel");
    if (bgCorrectionModelHelper == "minMethod")
        e_backgroundCorrectionModel = minMethod;
    else if (bgCorrectionModelHelper == "minMaxMethod")
        e_backgroundCorrectionModel = minMaxMethod;
    else if (bgCorrectionModelHelper == "averageMethod")
        e_backgroundCorrectionModel = averageMethod;
    else if (bgCorrectionModelHelper == "medianMethod")
        e_backgroundCorrectionModel = medianMethod;
    else
        throw std::runtime_error(
            makeRed("Invalid value for setting backgroundCorrectionModel: " +
                    bgCorrectionModelHelper));

    readParameterBool(settings, e_useMultiChannelSaveMode, "useMultiChannelSaveMode");
}
