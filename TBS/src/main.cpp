#include "reader.hpp"
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>

int main()
{
    std::cout << "Start of program\n";

    Reader reader("/home/tim/Documents/Arbeit/TestVideos/Julia_1_r.MP4", Video, 100, 400);
    Reader reader2("/home/tim/Documents/Arbeit/TestVideos/Julia_1_r.MP4", Video, 500);

    bool result = true;
    bool result2 = true;
    cv::Mat frame, frame2;
    while (result && result2) {
        result = reader.getNextImage(frame);
        result2 = reader2.getNextImage(frame2);
        if (!result || !result2)
            break;

        cv::resize(frame, frame, cv::Size(500, 500));
        cv::resize(frame2, frame2, cv::Size(500, 500));
        cv::imshow("img", frame);
        cv::imshow("img2", frame2);

        if (cv::waitKey(10) == 'q') {
            result = false;
        }
    }
}
