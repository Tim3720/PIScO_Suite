#include "detection.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

double getContours(std::vector<std::vector<cv::Point>>& contours, const cv::Mat& frame, const cv::Mat& background)
{
    cv::Mat helper;
    cv::absdiff(frame, background, helper);
    double threshold = cv::threshold(helper, helper, 0, 255, cv::THRESH_TRIANGLE);
    cv::findContours(helper, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    return threshold;
}
