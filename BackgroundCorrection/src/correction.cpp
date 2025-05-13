#include "correction.hpp"
#include "reader.hpp"
#include "settings.hpp"
#include "writer.hpp"
#include <chrono>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <thread>

void correction(const std::vector<std::string>& files, size_t startIndex,
    size_t stopIndex)
{
    // first read nBackground images:
    std::vector<cv::Mat> backgroundImages;
    backgroundImages.resize(e_nBackgroundImages);
    for (size_t i = 0; i < e_nBackgroundImages; i++) {
        getImage(backgroundImages[i], i + startIndex, files).check();
    }

    Writer writer;

    cv::Mat background;
    size_t imgIndex = 0;
    size_t insertionIndex = 0;
    size_t fileIndex = e_nBackgroundImages + startIndex;
    for (size_t idx = 0; idx < (stopIndex - startIndex); idx++) {
        // compute background:
        e_backgroundCorrectionModel(backgroundImages, background, 0, e_nBackgroundImages);

        // compute corrected image:
        cv::absdiff(background, backgroundImages[imgIndex], background);

        // save result:
        if (idx == (stopIndex - startIndex) - 1) {  // last image:
            writer.writeImage(background, startIndex + idx, true);
        } else {
            writer.writeImage(background, startIndex + idx);
        }

        // update indices
        imgIndex = (imgIndex + 1) % e_nBackgroundImages;

        // read next image:
        if (idx > e_nBackgroundImages / 2 &&
            idx < stopIndex - startIndex - e_nBackgroundImages / 2) {
            getImage(backgroundImages[insertionIndex], fileIndex, files).check();
            insertionIndex = (insertionIndex + 1) % e_nBackgroundImages;
            fileIndex++;
        }
    }
}

Error runCorrection()
{
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();
    std::vector<std::string> files;
    getFiles(files).check();
    writeFileNameDecoding(files);

    // check if enough files exists for the given number of threads:
    if (files.size() / e_nThreads <= e_nBackgroundImages) {
        Error error = Error::RuntimeError;
        error.addMessage(
            "Not enough images for specified number of backgroundImages and threads");
        return error;
    }

    // start threads on subspaces on the file vector:
    std::vector<std::thread> threads;
    threads.resize(e_nThreads - 1);
    size_t subFileSize = files.size() / e_nThreads;
    for (size_t i = 0; i < e_nThreads - 1; i++) {
        threads[i] = std::thread(correction, std::ref(files), i * subFileSize,
            (i + 1) * subFileSize);
    }
    correction(files, (e_nThreads - 1) * subFileSize, files.size());

    // join threads:
    for (std::thread& t : threads) {
        t.join();
    }

    double duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::high_resolution_clock::now() - start)
                          .count() /
                      1000.;
    std::cout << "Total duration: " << duration << "s\n";
    std::cout << "Avg time per img: " << duration / files.size() << "s\n";

    return Error::Success;
}
