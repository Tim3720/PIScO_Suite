#pragma once
#include <string>
#include <unordered_map>

std::unordered_map<std::string, std::string> parseConfigFile(std::string filename);

template <typename T>
void readParameterInt(std::unordered_map<std::string, std::string>& fileConfig,
    T& parameter, std::string name);
void readParameterInt(std::unordered_map<std::string, std::string>& fileConfig,
    size_t& parameter, std::string name);

void readParameterDouble(std::unordered_map<std::string, std::string>& fileConfig,
    double& parameter, std::string name);

void readParameterBool(std::unordered_map<std::string, std::string>& fileConfig,
    bool& parameter, std::string name);

void readParameterString(std::unordered_map<std::string, std::string>& fileConfig,
    std::string& parameter, std::string name);
