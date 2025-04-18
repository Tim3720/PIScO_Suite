#include "crop_generator.hpp"
#include "reader.hpp"
#include "utils.hpp"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <threadmanager.hpp>

Info operateOnObjects(std::vector<Object>* objectsPtr, const size_t& taskIdx, cv::Mat& sourceImage, const std::string& imageName)
{
    if (saveMode == oneImagePerObject) {
        Object object;
        for (size_t i = 0; i < objectsPtr[taskIdx].size(); i++) {
            object = objectsPtr[taskIdx][i];
            cv::imwrite(savePath + "/" + imageName + "_" + std::to_string(i) + ".png", sourceImage(object.boundingBox));
        }

        // Object object;
        // cv::Mat result;
        // cv::Mat channel[3] = {};
        // for (size_t i = 0; i < objectsPtr[taskIdx].size(); i++) {
        //     object = objectsPtr[taskIdx][i];
        //     channel[0] = sourceImage(object.boundingBox);
        //
        //     cv::threshold(channel[0], channel[1], object.threshold + 5, 255, cv::THRESH_BINARY);
        //     cv::threshold(channel[0], channel[2], object.threshold - 5, 255, cv::THRESH_BINARY);
        //     cv::threshold(channel[0], channel[0], object.threshold, 255, cv::THRESH_BINARY);
        //
        //     cv::merge(channel, 3, result);
        //     cv::imwrite(savePath + "/" + imageName + "_" + std::to_string(i) + ".png", result);
        // }

        // store object only in blue channel:
        // Object object;
        cv::Mat result;
        cv::Mat channel[3] = {};
        // std::vector<std::vector<cv::Point>> contours;
        // contours.resize(1);
        for (size_t i = 0; i < objectsPtr[taskIdx].size(); i++) {
            object = objectsPtr[taskIdx][i];
            channel[0] = sourceImage(object.boundingBox);
            channel[1] = cv::Mat::zeros(channel[0].size(), CV_8UC1);
            channel[2] = cv::Mat::zeros(channel[0].size(), CV_8UC1);
            // contours[0] = object.contour;

            size_t xOffset = object.boundingBox.x;
            size_t yOffset = object.boundingBox.y;
            uint8_t c1 = 1;
            uint8_t c2 = 0;
            for (const cv::Point& p : object.contour) {
                channel[1].at<uint8_t>(p.y - yOffset, p.x - xOffset) = c1;
                channel[2].at<uint8_t>(p.y - yOffset, p.x - xOffset) = c2;
                if (c1 == 255) {
                    c1 = 0;
                    c2++;
                }
                c1++;
            }
            // / draw contour in green channel
            // cv::drawContours(channel[1], contours, -1, 255, 1, cv::LINE_8, cv::noArray(), 1, cv::Point(-object.boundingBox.x, -object.boundingBox.y));
            cv::merge(channel, 3, result);
            cv::imwrite(savePath + "/" + imageName + "_" + std::to_string(i) + ".png", result);
            result.release();
        }
    } else if (saveMode == drawContours) {
        std::vector<std::vector<cv::Point>> contours;
        Object object;
        for (size_t i = 0; i < objectsPtr[taskIdx].size(); i++) {
            object = objectsPtr[taskIdx][i];
            contours.push_back(object.contour);
        }
        cv::cvtColor(sourceImage, sourceImage, cv::COLOR_GRAY2BGR);
        cv::drawContours(sourceImage, contours, -1, cv::Scalar(0, 255, 0));
        cv::imwrite(savePath + "/" + imageName + ".png", sourceImage);
    } else if (saveMode == cluster) {
        std::cout << makeRed("Not implemented") << std::endl;
        return RuntimeError;
    } else {
        std::cout << makeRed("Not implemented") << std::endl;
        return RuntimeError;
    }
    return Success;
}

Info imageTask(const size_t& threadId, std::mutex& mutex, void* taskData, const size_t& taskIdx, const size_t& taskSize)
{
    std::vector<Object>* objectsPtr = ((std::vector<Object>*)taskData);

    if (objectsPtr[taskIdx].empty()) {
        print(makeYellow("Error: empty object vector at index " + std::to_string(taskIdx)));
        return EmptyTask;
    }

    std::string imageFileName;
    std::string imageName;
    try {
        imageFileName = objectsPtr[taskIdx][0].imageFileName;
        imageName = imageFileName.substr(0, imageFileName.size() - 4);

        // cv::Mat img = cv::imread(imageSourcePath + "/" + imageFileName, cv::IMREAD_GRAYSCALE);
        //
        cv::Mat img = cv::imread(imageSourcePath + "/" + imageName + "." + sourceImageFileType, cv::IMREAD_GRAYSCALE);

        if (resizeToImageWidthHeight)
            cv::resize(img, img, cv::Size(imageWidth, imageHeight));

        checkInfo(operateOnObjects(objectsPtr, taskIdx, img, imageName), enableDetailedPrinting, enableDetailedPrinting);

        // clean up
        img.release();
        objectsPtr[taskIdx].clear();
    } catch (cv::Exception& e) {
        std::cout << "OpenCV error: " << e.what() << " in file " << imageSourcePath + "/" + imageName + "." + sourceImageFileType << std::endl;
        return CVRuntimeError;
    } catch (std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return RuntimeError;
    }

    return Success;
}

void generate_crops()
{
    auto start = std::chrono::high_resolution_clock::now();
    FileReader reader(sourcePath, "objects_img");
    // FileReader reader(sourcePath, "objects.");

    std::vector<Object>* taskBuffer = new std::vector<Object>[taskBufferSize];
    taskFunction f = [](const size_t& threadId, std::mutex& mutex, void* taskData, const size_t& taskIdx, const size_t& taskSize) { return imageTask(threadId, mutex, taskData, taskIdx, taskSize); };
    taskFunction functions[] = { f };

    ThreadManager tm(taskBuffer, functions, numThreads, true, taskBufferSize);

    Info info;
    reader.progress();

    for (Int i = 0; i < taskBufferSize; i++) {
        info = reader.getNextImage(taskBuffer[i]);
        checkInfo(info, enableDetailedPrinting, enableDetailedPrinting);

        Task task;
        task.taskIdx = i;
        task.size = 1;
        task.funcIdx = 0;

        checkInfo(tm.addTask(task), enableDetailedPrinting, enableDetailedPrinting);
    }

    Task finishedTask;
    while (true) {
        tm.getFinishedTask(finishedTask);
        checkInfo(finishedTask.threadManagerInfo, enableDetailedPrinting, enableDetailedPrinting);
        checkInfo(finishedTask.taskInfo, enableDetailedPrinting, enableDetailedPrinting);

        info = reader.getNextImage(taskBuffer[finishedTask.taskIdx]);
        checkInfo(info, enableDetailedPrinting, enableDetailedPrinting);

        Task task;
        task.taskIdx = finishedTask.taskIdx;
        task.size = 1;
        task.funcIdx = 0;

        checkInfo(tm.addTask(task), enableDetailedPrinting, enableDetailedPrinting);

        if (info == FinishedAllFiles)
            break;
        reader.progress();
    }

    tm.finishThreads();
    tm.cleanUp();
    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "--------------------------------------------------------------------" << std::endl;
    std::cout << "Metrics: " << std::endl;
    std::cout << "ThreadManager:\n";
    std::cout << "\tAverage wait time: " << tm.getAvgWaitTime() << "ms" << std::endl;
    std::cout << "\tAverage task run time: " << tm.getAvgTaskRunTime() << "ms" << std::endl;

    std::cout << "Duration: " << duration << "ms" << std::endl;
    std::cout << "Avg time per file: " << duration / reader.getNumFiles() << "ms" << std::endl;

    delete[] taskBuffer;
}
