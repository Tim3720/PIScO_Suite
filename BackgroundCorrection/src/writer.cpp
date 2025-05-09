#include "writer.hpp"
#include "settings.hpp"
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

void writeFileNameDecoding(const std::vector<std::string>& files)
{
    std::ofstream file(e_savePath + "fileNameDecoding.dat");
    file << "Index\tFileName\n";
    for (size_t i = 0; i < files.size(); i++) {
        file << i << "\t" << files[i] << std::endl;
    }
}

Writer::Writer()
{
    channels.resize(3);
    counter = 0;
}

void Writer::writeImage(const cv::Mat& img, size_t fileIndex, bool lastImage)
{
    if (e_useMultiChannelSaveMode) {
        channels[counter] = img.clone();
        counter++;
        if (counter == 3) {
            counter = 0;
            cv::merge(channels, color);
            cv::imwrite(e_savePath + std::to_string(fileIndex - 2) + "_" +
                            std::to_string(fileIndex - 1) + "_" +
                            std::to_string(fileIndex) + ".png",
                color);
        } else if (lastImage) {
            cv::merge(channels, color);
            if (counter == 0) {
                cv::imwrite(e_savePath + std::to_string(fileIndex) + ".png", color);
            } else {
                cv::imwrite(e_savePath + std::to_string(fileIndex - 1) + "_" +
                                std::to_string(fileIndex) + ".png",
                    color);
            }
        }
    } else {
        cv::imwrite(e_savePath + std::to_string(fileIndex) + ".png", img);
    }
}
