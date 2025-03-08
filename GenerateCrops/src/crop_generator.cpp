#include "crop_generator.hpp"
#include "reader.hpp"
#include "utils.hpp"

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <threadmanager.hpp>

Info imageTask(const size_t& threadId, std::mutex& mutex, void* taskData, const size_t& taskIdx, const size_t& taskSize)
{
    std::vector<Object>* objectsPtr = ((std::vector<Object>*)taskData);

    if (objectsPtr[taskIdx].empty()) {
        std::cout << makeRed("Error: empty object vector at index " + std::to_string(taskIdx)) << std::endl;
        return RuntimeError;
    }

    try {
        std::string imageFileName = objectsPtr[taskIdx][0].imageFileName;
        std::string imageName = imageFileName.substr(0, imageFileName.size() - 4);
        cv::Mat img = cv::imread(imageSourcePath + "/" + imageFileName, cv::IMREAD_COLOR);

        if (resizeToImageWidthHeight)
            cv::resize(img, img, cv::Size(imageWidth, imageHeight));

        std::vector<std::vector<cv::Point>> contours;
        Object object;
        for (size_t i = 0; i < objectsPtr[taskIdx].size(); i++) {
            object = objectsPtr[taskIdx][i];
            contours.push_back(object.contour);
            // cv::imwrite(savePath + "/" + imageName + "_" + std::to_string(i) + ".png", img(object.boundingBox));
        }
        // std::cout << imageFileName << std::endl;
        cv::drawContours(img, contours, -1, cv::Scalar(0, 255, 0));
        cv::imwrite(savePath + "/" + imageFileName, img);

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
