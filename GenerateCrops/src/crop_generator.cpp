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
        // store object only in blue channel:
        cv::multiply(sourceImage, cv::Scalar(1, 0, 0), sourceImage);
        Object object;
        cv::Mat crop;
        std::vector<std::vector<cv::Point>> contours;
        contours.resize(1);
        for (size_t i = 0; i < objectsPtr[taskIdx].size(); i++) {
            object = objectsPtr[taskIdx][i];
            crop = sourceImage(object.boundingBox);
            contours[0] = object.contour;
            // draw contour in green channel
            cv::drawContours(crop, contours, -1, cv::Scalar(0, 255, 0), 1, cv::LINE_8, cv::noArray(), 1, cv::Point(-object.boundingBox.x, -object.boundingBox.y));
            cv::imwrite(savePath + "/" + imageName + "_" + std::to_string(i) + ".png", crop);
        }
    } else if (saveMode == drawContours) {
        std::vector<std::vector<cv::Point>> contours;
        Object object;
        for (size_t i = 0; i < objectsPtr[taskIdx].size(); i++) {
            object = objectsPtr[taskIdx][i];
            contours.push_back(object.contour);
        }
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

    try {
        std::string imageFileName = objectsPtr[taskIdx][0].imageFileName;
        std::string imageName = imageFileName.substr(0, imageFileName.size() - 4);
        cv::Mat img = cv::imread(imageSourcePath + "/" + imageFileName, cv::IMREAD_COLOR);

        if (resizeToImageWidthHeight)
            cv::resize(img, img, cv::Size(imageWidth, imageHeight));

        checkInfo(operateOnObjects(objectsPtr, taskIdx, img, imageName), enableDetailedPrinting, enableDetailedPrinting);

        // clean up
        img.release();
        objectsPtr[taskIdx].clear();
    } catch (cv::Exception& e) {
        std::cout << "OpenCV error: " << e.what() << std::endl;
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
