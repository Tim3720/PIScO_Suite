#include "correction.hpp"
#include "error.hpp"
#include "reader.hpp"
#include "settings.hpp"
#include "writer.hpp"
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

int main(int argc, char** argv)
{
#if (DEBUG)
    std::cout << makeRed("Program is compiled in DEBUG mode") << std::endl;
#endif
    if (argc > 1) {
        loadSettings(argv[1]);
    } else {
        std::cout << makeBlue("No input file given, using default settings.")
                  << std::endl;
        defaultSettings();
    }

    std::vector<std::string> files;
    getFiles(files).check();
    writeFileNameDecoding(files);
    runCorrection(files);
}
