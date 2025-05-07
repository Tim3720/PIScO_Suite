#include "data_loader.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <opencv2/highgui.hpp>

int main(int argc, char** argv)
{
    if (argc > 1) {
    } else {
        defaultSettings();
    }

    std::vector<ObjectData> test;
    std::ifstream file("/home/tim/Documents/ArbeitTestData/TestResults/"
                       "objects_img_SO308_1-5-1_PISCO2_0012.81dbar-20.10S-036.10E-26.08C_"
                       "20241106-15340569.dat");
    getImageData(test, 0, file).check();

    return 0;
}
