#include "error.hpp"
#include "reader.hpp"
#include "settings.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    if (argc > 1) {
        // loadSettings(argv[1]);
    } else {
        defaultSettings();
    }

    std::vector<std::string> files;
    getFiles(files).check();
    readImages(files);
}
