#include "parser.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

std::unordered_map<std::string, std::string>
parseConfigFile(std::string filename) {
  std::unordered_map<std::string, std::string> settings;

  std::ifstream configFile(filename);
  if (!configFile.is_open()) {
    throw std::runtime_error("Error: Unable to open the configuration file " +
                             filename);
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

template <typename T>
void readParameterInt(std::unordered_map<std::string, std::string> &fileConfig,
                      T &parameter, std::string name) {
  auto foundKey = fileConfig.find(name);
  if (foundKey == fileConfig.end()) {
    throw std::runtime_error("Error: " + name +
                             " not found in the config file.");
  }
  parameter = T(std::stoi(fileConfig[name]));
  std::cout << "Found Parameter " + name + " of type int with value " +
                   std::to_string(parameter)
            << std::endl;
}

void readParameterInt(std::unordered_map<std::string, std::string> &fileConfig,
                      size_t &parameter, std::string name) {
  auto foundKey = fileConfig.find(name);
  if (foundKey == fileConfig.end()) {
    throw std::runtime_error("Error: " + name +
                             " not found in the config file.");
  }
  parameter = size_t(std::stoi(fileConfig[name]));
  std::cout << "Found Parameter " + name + " of type int with value " +
                   std::to_string(parameter)
            << std::endl;
}

void readParameterDouble(
    std::unordered_map<std::string, std::string> &fileConfig, double &parameter,
    std::string name) {
  auto foundKey = fileConfig.find(name);
  if (foundKey == fileConfig.end()) {
    throw std::runtime_error("Error: " + name +
                             " not found in the config file.");
  }
  parameter = std::stod(fileConfig[name]);
  std::cout << "Found Parameter " + name + " of type double with value " +
                   std::to_string(parameter)
            << std::endl;
}

void readParameterBool(std::unordered_map<std::string, std::string> &fileConfig,
                       bool &parameter, std::string name) {
  auto foundKey = fileConfig.find(name);
  if (foundKey == fileConfig.end()) {
    throw std::runtime_error("Error: " + name +
                             " not found in the config file.");
  }
  std::string value = fileConfig[name];

  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (value == "true")
    parameter = true;
  else if (value == "false")
    parameter = false;
  else
    throw std::runtime_error("Error: Invalid value for " + name);
  std::cout << "Found Parameter " + name + " of type bool with value " +
                   std::to_string(parameter)
            << std::endl;
}

void readParameterString(
    std::unordered_map<std::string, std::string> &fileConfig,
    std::string &parameter, std::string name) {
  auto foundKey = fileConfig.find(name);
  if (foundKey == fileConfig.end()) {
    throw std::runtime_error("Error: " + name +
                             " not found in the config file.");
  }
  parameter = fileConfig[name];

  std::cout << "Found Parameter " + name + " of type string with value " +
                   parameter
            << std::endl;
}
