#pragma once
#include "error.hpp"
#include <string>
#include <vector>

Error getFiles(std::vector<std::string>& files);
void readImages(const std::vector<std::string>& files);
