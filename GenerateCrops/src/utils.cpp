#include "utils.hpp"
#include <csignal>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

//////////////////////////////////////////////////////////////
// External variables:
//////////////////////////////////////////////////////////////
// Paths:
std::string sourcePath;
std::string savePath;
std::string imageSourcePath;

// image manipulation settings:
Int imageWidth;
Int imageHeight;
bool resizeToImageWidthHeight;

std::string cropDataLoadModeStr;
std::string saveModeStr;
Int numThreads;
Int taskBufferSize;

// Other:
bool enableDetailedPrinting;
Int progressBarWidth;

// Helper:
LoadMode cropDataLoadMode;
SaveMode saveMode;
//////////////////////////////////////////////////////////////

void print(std::string str, bool newLine, bool ignoreDetailedPrinting)
{
    if (ignoreDetailedPrinting || enableDetailedPrinting) {
        std::cout << str;
        if (newLine) {
            std::cout << std::endl;
        }
    }
}

void error(std::string errorMessage)
{
    throw std::runtime_error(makeRed("Error: " + errorMessage));
}

void warning(std::string warningMessage)
{
    std::cout << makeYellow("Warning: " + warningMessage) << std::endl;
}

void progressBar(Int fileIdx, Int filesSize)
{
    if (progressBarWidth > 0) {
        double progress = double(fileIdx) / filesSize;
        Int pos = Int(std::round(progress * progressBarWidth));
        std::cout << "[\033[34m";
        for (Int i = 0; i < progressBarWidth; i++) {
            if (i < pos)
                std::cout << "#";
            else if (i == pos)
                std::cout << ">\033[0m\033[90m";
            else
                std::cout << "#";
        }
        std::cout << "\033[0m] " << int(progress * 100) << "%\r";
        std::cout.flush();
        if (progress == 1)
            std::cout << std::endl;
    }
}

std::string customInfo(Info errorCode)
{
    const std::unordered_map<Info, std::string> errorMap {
        { CouldNotOpenFile, "Could not open file" },
        { EndOfFileNotReached, "End of file not reached" },
        { EmptyFile, "File is empty" },
        { EndOfFile, "Reached end of file" },
        { FinishedAllFiles, "Finished all files" },
        { CVRuntimeError, "OpenCV runtime error" },
        { EmptyTask, "Task function received empty task" },
    };
    try {
        return errorMap.at(errorCode);
    } catch (const std::out_of_range& e) {
        return info(errorCode);
    }
}

std::unordered_map<std::string, std::string> parseConfigFile(std::string filename)
{
    std::unordered_map<std::string, std::string> settings;

    std::ifstream configFile(filename);
    if (!configFile.is_open()) {
        throw std::runtime_error("Error: Unable to open the configuration file " + filename);
    }

    std::string line, subStr;
    while (std::getline(configFile, line)) {
        subStr = line.substr(0, 2);
        if (subStr == "//" || subStr == "#") {
            continue;
        }
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            // Trim leading and trailing whitespaces from key and value
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            settings[key] = value;
        }
    }
    configFile.close();
    return settings;
}

void printHelp()
{
    std::cout << "Help" << std::endl;
}

// Function to parse command-line arguments
std::unordered_map<std::string, std::string> parseCommandLineArgs(int argc, char* argv[])
{
    std::unordered_map<std::string, std::string> params;
    std::string key;
    std::string str;
    // find config file path as first argument
    if (argc < 2) {
        // throw std::runtime_error("Error: Input file not found. Specify as first argument, i.e. ./SegmenterParallel <input_file_path>");
        params["config"] = "input.ini";
    } else {
        params["config"] = std::string(argv[1]);
    }

    for (int i = 2; i < argc; i++) {
        str = argv[i];
        if (str.rfind("--", 0) == 0) { // found key
            key = str.substr(2);

            // this key is without value
            if (i + 1 < argc && std::string(argv[i + 1]).rfind("--", 0) == 0) {
                if (key == "help")
                    printHelp();
            }
        } else { // found value
            params[key] = str;
        }
    }
    return params;
}

void readParameterInt(std::unordered_map<std::string, std::string>& fileConfig, std::unordered_map<std::string, std::string>& commandLineConfig, Int& parameter, std::string name)
{
    auto foundKey = commandLineConfig.find(name);
    if (foundKey == commandLineConfig.end()) {
        foundKey = fileConfig.find(name);
        if (foundKey == fileConfig.end()) {
            throw std::runtime_error(makeRed("Error: " + name + " not found in the config file."));
        }
        parameter = std::stoi(fileConfig[name]);
    } else {
        parameter = std::stoi(commandLineConfig[name]);
    }
    print("Found Parameter " + name + " of type int with value " + std::to_string(parameter), true, true);
}

void readParameterDouble(std::unordered_map<std::string, std::string>& fileConfig, std::unordered_map<std::string, std::string>& commandLineConfig, double& parameter, std::string name)
{
    auto foundKey = commandLineConfig.find(name);
    if (foundKey == commandLineConfig.end()) {
        foundKey = fileConfig.find(name);
        if (foundKey == fileConfig.end()) {
            throw std::runtime_error(makeRed("Error: " + name + " not found in the config file."));
        }
        parameter = std::stod(fileConfig[name]);
    } else {
        parameter = std::stod(commandLineConfig[name]);
    }
    print("Found Parameter " + name + " of type double with value " + std::to_string(parameter), true, true);
}

void getLoadMode(const std::string& loadModeStr, LoadMode& parameter, const std::string& parameterName)
{
    if (loadModeStr == "oneFile") {
        parameter = oneFile;
    } else if (loadModeStr == "oneFilePerImage") {
        parameter = oneFilePerImage;
    } else if (loadModeStr == "oneFilePerObject") {
        parameter = oneFilePerObject;
    } else {
        throw std::runtime_error(makeRed("Error: Invalid value for parameter " + parameterName + ": " + loadModeStr));
    }
}

void getSaveMode(const std::string& saveModeStr, SaveMode& parameter, const std::string& parameterName)
{
    if (saveModeStr == "oneImagePerObject") {
        parameter = oneImagePerObject;
    } else if (saveModeStr == "cluster") {
        parameter = cluster;
    } else if (saveModeStr == "clusterPerImage") {
        parameter = clusterPerImage;
    } else if (saveModeStr == "drawContours") {
        parameter = drawContours;
    } else {
        throw std::runtime_error(makeRed("Error: Invalid value for parameter " + parameterName + ": " + saveModeStr));
    }
}

void readParameterBool(std::unordered_map<std::string, std::string>& fileConfig, std::unordered_map<std::string, std::string>& commandLineConfig, bool& parameter, std::string name)
{
    auto foundKey = commandLineConfig.find(name);
    std::string value;
    if (foundKey == commandLineConfig.end()) {
        foundKey = fileConfig.find(name);
        if (foundKey == fileConfig.end()) {
            throw std::runtime_error(makeRed("Error: " + name + " not found in the config file."));
        }
        value = fileConfig[name];
    } else {
        value = commandLineConfig[name];
    }
    std::transform(value.begin(), value.end(), value.begin(),
        [](unsigned char c) { return std::tolower(c); });
    if (value == "true")
        parameter = true;
    else if (value == "false")
        parameter = false;
    else
        throw std::runtime_error(makeRed("Error: Invalid value for " + name));
    print("Found Parameter " + name + " of type bool with value " + std::to_string(parameter), true, true);
}

void readParameterString(std::unordered_map<std::string, std::string>& fileConfig, std::unordered_map<std::string, std::string>& commandLineConfig, std::string& parameter, std::string name)
{
    auto foundKey = commandLineConfig.find(name);
    if (foundKey == commandLineConfig.end()) {
        foundKey = fileConfig.find(name);
        if (foundKey == fileConfig.end()) {
            throw std::runtime_error(makeRed("Error: " + name + " not found in the config file."));
        }
        parameter = fileConfig[name];
    } else {
        parameter = commandLineConfig[name];
    }
    print("Found Parameter " + name + " of type string with value " + parameter, true, true);
}

void readParameters(int argc, char* argv[])
{
    // read parameters from command line and input file
    std::unordered_map<std::string, std::string> commandLineConfig = parseCommandLineArgs(argc, argv);
    std::unordered_map<std::string, std::string> fileConfig = parseConfigFile(commandLineConfig["config"]);

    print("--------------------------------------------------------------------", true, true);
    readParameterBool(fileConfig, commandLineConfig, enableDetailedPrinting, "enableDetailedPrinting");
    readParameterString(fileConfig, commandLineConfig, sourcePath, "sourcePath");
    readParameterString(fileConfig, commandLineConfig, imageSourcePath, "imageSourcePath");
    readParameterString(fileConfig, commandLineConfig, savePath, "savePath");
    readParameterInt(fileConfig, commandLineConfig, progressBarWidth, "progressBarWidth");
    readParameterInt(fileConfig, commandLineConfig, numThreads, "numThreads");
    readParameterInt(fileConfig, commandLineConfig, taskBufferSize, "taskBufferSize");
    readParameterInt(fileConfig, commandLineConfig, imageWidth, "imageWidth");
    readParameterInt(fileConfig, commandLineConfig, imageHeight, "imageHeight");
    readParameterBool(fileConfig, commandLineConfig, resizeToImageWidthHeight, "resizeToImageWidthHeight");
    readParameterString(fileConfig, commandLineConfig, cropDataLoadModeStr, "cropDataLoadMode");
    readParameterString(fileConfig, commandLineConfig, saveModeStr, "saveMode");

    getSaveMode(saveModeStr, saveMode, "saveMode");
    getLoadMode(cropDataLoadModeStr, cropDataLoadMode, "cropDataLoadMode");

    print("--------------------------------------------------------------------\n", true, true);
}
