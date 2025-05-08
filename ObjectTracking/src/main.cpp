#include "data_loader.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

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
    std::cout << "Finished reading object data\n";

    cv::Mat img = cv::imread(
        "/home/tim/Documents/ArbeitTestData/selection/"
        "SO308_1-5-1_PISCO2_0012.81dbar-20.10S-036.10E-26.08C_20241106-15340569.png");

    std::vector<std::vector<cv::Point>> contours;
    contours.reserve(test.size());
    for (ObjectData& data : test) {
        contours.push_back(data.m_contour);
    }
    cv::drawContours(img, contours, -1, cv::Scalar(255, 0, 0), 1);
    cv::resize(img, img, {}, 0.5, 0.5);
    cv::imshow("img", img);
    cv::waitKey();

    return 0;
}
