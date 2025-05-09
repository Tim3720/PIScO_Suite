#include "correction.hpp"
#include "reader.hpp"
#include "settings.hpp"
#include "writer.hpp"
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

void runCorrection(const std::vector<std::string>& files, size_t fileIndexOffset)
{
    // first read nBackground images:
    std::vector<cv::Mat> backgroundImages;
    backgroundImages.resize(e_nBackgroundImages);
    for (size_t i = 0; i < e_nBackgroundImages; i++) {
        getImage(backgroundImages[i], i, files).check();
    }

    Writer writer;

    cv::Mat background;
    size_t counter = 0;
    size_t imgIndex = 0;
    size_t insertionIndex = 0;
    size_t fileIndex = e_nBackgroundImages;
    while (counter < files.size()) {
        // compute background:
        e_backgroundCorrectionModel(backgroundImages, background, 0, e_nBackgroundImages);
        // compute corrected image:
        cv::absdiff(background, backgroundImages[imgIndex], background);
        // save result:
        writer.writeImage(background, counter + fileIndexOffset);

        // update indices
        imgIndex = (imgIndex + 1) % e_nBackgroundImages;
        counter++;

        // read next image:
        if (counter > e_nBackgroundImages / 2 &&
            counter < files.size() - e_nBackgroundImages / 2) {
            getImage(backgroundImages[insertionIndex], fileIndex, files).check();
            insertionIndex = (insertionIndex + 1) % e_nBackgroundImages;
            fileIndex++;
        }
    }
}
