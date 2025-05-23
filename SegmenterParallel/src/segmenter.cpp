#include "segmenter.hpp"
#include "reader.hpp"
#include "threadmanager.hpp"
#include "utils.hpp"
#include "writer.hpp"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mutex>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <vector>

int globalCounter = 0;

Info _read(const size_t& threadId, std::mutex& mutex, void* imageBufferIndices, const size_t& taskIdx, const size_t& taskSize,
    Int* frameIndices, Image* imageBuffer, cv::VideoCapture* videoCaptures)
{
    Int* taskPtr = (Int*)imageBufferIndices;
    cv::VideoCapture videoCapture = videoCaptures[threadId];
    size_t numFrames = size_t(videoCapture.get(cv::CAP_PROP_FRAME_COUNT));

    Int frameIdx = frameIndices[threadId];
    Int maxFrameIdx = (threadId + 1) * numFrames / numReaderThreads;
    if (Int(threadId) == numReaderThreads - 1) // last reader thread should handle all remaining files
        maxFrameIdx = numFrames;

    Int stop = frameIdx + Int(stackSize);
    if (stop > maxFrameIdx || maxFrameIdx - stop < Int(stackSize)) { // handle images that are do not fill a stack.
        stop = maxFrameIdx;
    }

    Int imageBufferIndex = taskPtr[taskIdx + 1];
    Int size = 0;
    cv::Mat helper;
    while (frameIdx < stop) {
        bool result = false;
        while (!result && frameIdx < maxFrameIdx) {
            result = videoCapture.read(helper);
            if (helper.empty())
                result = false;
            frameIdx++;
        }
        if (result) {
            cv::cvtColor(helper, imageBuffer[imageBufferIndex].originalImage, cv::COLOR_BGR2GRAY);
            imageBuffer[imageBufferIndex].fileIdx = frameIdx - 1;
            imageBufferIndex++;
            size++;
        }
    }
    taskPtr[taskIdx] = size;

    // this worker is finished:
    if (frameIdx >= maxFrameIdx) {
        return ThreadManagerThisWorkerFinished;
    }

    frameIndices[threadId] = frameIdx;

    return Success;
}

Info _read(const size_t& threadId, std::mutex& mutex, void* imageBufferIndices, const size_t& taskIdx, const size_t& taskSize,
    const std::vector<std::string>& files, Int* fileIndices, Image* imageBuffer)
{
    // cast pointer to right type
    Int* taskPtr = (Int*)imageBufferIndices;

    // get file data
    Int fileIdx = fileIndices[threadId];
    Int filesSize = Int(files.size());
    Int maxFileIdx = (threadId + 1) * filesSize / numReaderThreads;
    if (Int(threadId) == numReaderThreads - 1) // last reader thread should handle all remaining files
        maxFileIdx = filesSize;

    Int stop = fileIdx + Int(stackSize);
    if (stop > maxFileIdx || maxFileIdx - stop < Int(stackSize)) { // handle images that are do not fill a stack.
        stop = maxFileIdx;
    }

    // iterate over files and read images
    Int imageBufferIndex = taskPtr[taskIdx + 1];
    Int size = 0;
    while (fileIdx < stop) {
        Info info = EmptyImage;
        // read files until succes.
        // TODO: The number of indices can be smaller than the stackSize if not enough images where readable or larger than the stackSize if not enough images would remain to fill a stack. Both cases should be handled correctly
        while (info != Success && fileIdx < maxFileIdx) {
            info = readImage(files[fileIdx], imageBuffer[imageBufferIndex].originalImage);
            checkInfo(info, enableDetailedPrinting, enableDetailedPrinting);
            fileIdx++;
        }
        if (info == Success) {
            imageBuffer[imageBufferIndex].fileIdx = fileIdx - 1;
            imageBufferIndex++;
            size++;
        }
    }
    taskPtr[taskIdx] = size;

    // this worker is finished:
    if (fileIdx >= maxFileIdx) {
        return ThreadManagerThisWorkerFinished;
    }

    fileIndices[threadId] = fileIdx;

    return Success;
}

Info finishSegmenterTask(std::mutex& mutex, Image* imageBuffer, Int startPos, Int size, const std::vector<std::string>& files,
    const std::vector<std::vector<SegmenterObject>>& objects)
{
    try {
        if (saveMode == oneFile) {
            std::unique_lock<std::mutex> lock(mutex);
            for (Int i = 0; i < size; i++) {
                bool newImage = true;
                for (const SegmenterObject& object : objects[i]) {
                    writeSegmenterObject(object, files, newImage);
                    newImage = false;
                }
            }
        } else {
            for (Int i = 0; i < size; i++) {
                bool newImage = true;
                for (const SegmenterObject& object : objects[i]) {
                    writeSegmenterObject(object, files, newImage);
                    newImage = false;
                }
            }
        }
        for (Int i = 0; i < size; i++) {
            cv::Mat img;
            if (saveBackgroundCorrectedImages || saveCrops)
                img = imageBuffer[startPos + i].originalImage;

            if (saveBackgroundCorrectedImages) {
                std::string path = savePath + "/" + splitAllBeforeLast(splitLast(files[imageBuffer[startPos + i].fileIdx], '/'), '.') + ".png";
                cv::imwrite(path, img);
            }

            if (saveCrops) {
                std::string path = savePath + "/crops/" + splitAllBeforeLast(splitLast(files[imageBuffer[startPos + i].fileIdx], '/'), '.');
                for (const SegmenterObject& object : objects[i]) {
                    cv::imwrite(path + "_" + std::to_string(object.id) + ".png", img(object.boundingRect));
                }
            }

            {
                std::unique_lock<std::mutex> lock(mutex);
                globalCounter++;
                progressBar(globalCounter, files.size());
            }
        }
        return Success;
    } catch (cv::Exception& e) {
        // always print exceptions!
        std::cout << "Exception: " << e.what() << std::endl;
        return CVRuntime;
    } catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return RuntimeError;
    }
    return Unknown;
}

Info _detection(Image* imageBuffer, Int startPos, Int size, const std::vector<cv::Mat>& backgrounds, std::vector<std::vector<SegmenterObject>>& objects)
{
    try {
        std::vector<std::vector<cv::Point>> contours;
        cv::Mat background;
        Int backgroundsSize = backgrounds.size();
        for (Int i = 0; i < size; i++) {
            background = backgrounds[i % backgroundsSize];
            cv::absdiff(background, imageBuffer[startPos + i].workingImage, imageBuffer[startPos + i].workingImage);
            std::vector<SegmenterObject> objectsOnImage;

            if (saveBackgroundCorrectedImages || saveCrops)
                imageBuffer[startPos + i].originalImage = imageBuffer[startPos + i].workingImage.clone();

            double threshold = cv::threshold(imageBuffer[startPos + i].workingImage, imageBuffer[startPos + i].workingImage, 0, 255, cv::THRESH_TRIANGLE);
            cv::findContours(imageBuffer[startPos + i].workingImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            int id = 0;
            for (Int j = 0; j < Int(contours.size()); j++) {
                // check if object is large enough to be saved
                double area = cv::contourArea(contours[j]);
                if (area < minObjectArea)
                    continue;
                SegmenterObject object;
                object.fileIdx = imageBuffer[startPos + i].fileIdx;
                object.contour = contours[j];
                object.boundingRect = cv::boundingRect(contours[j]);
                object.area = area;
                object.id = id;
                object.threshold = (uint8_t)threshold;
                id++;
                objectsOnImage.push_back(object);
            }
            objects.push_back(objectsOnImage);
            contours.clear();
        }
        return Success;
    } catch (cv::Exception& e) {
        std::cout << "Exception in taskSegmenterFunction: " << e.what() << std::endl;
        return CVRuntime;
    } catch (std::exception& e) {
        std::cout << "Exception in taskSegmenterFunction: " << e.what() << std::endl;
        return RuntimeError;
    }
    return Unknown;
}

Info _segment(const size_t& threadId, std::mutex& mutex, void* imageBufferIndices, const size_t& taskIdx, const size_t& taskSize,
    Image* imageBuffer, const std::vector<std::string>& files)
{
    Int* taskPtr = (Int*)imageBufferIndices;
    Int size = taskPtr[taskIdx];
    Int startPos = taskPtr[taskIdx + 1];

    std::vector<cv::Mat> workingImages;
    for (Int i = 0; i < size; i++) {
        imageBuffer[startPos + i].workingImage = imageBuffer[startPos + i].originalImage.clone();
        if (resizeToImageWidthHeight) {
            cv::resize(imageBuffer[startPos + i].workingImage, imageBuffer[startPos + i].workingImage, cv::Size(imageWidth, imageHeight));
        }
        if (invertImages) {
            cv::bitwise_not(imageBuffer[startPos + i].workingImage, imageBuffer[startPos + i].workingImage);
        }
        workingImages.push_back(imageBuffer[startPos + i].workingImage);
    }

    std::vector<cv::Mat> backgrounds;
    if (backgroundCorrectionModelStr != "noBackgroundCorrection") {
        int start, end;
        if (numBackgroundImages == size) { // background only needs to be computed once
            backgrounds.resize(1);
            backgroundCorrectionModel(workingImages, backgrounds[0], 0, size);
        } else {
            backgrounds.resize(size);
            for (Int i = 0; i < size; i++) {
                // compute background for every image. The background consits of the numBufferedStacks / 2 images to the left and right, if possible
                if (numBackgroundImages < size) {
                    if (i > numBackgroundImages / 2) {
                        if (i < size - numBackgroundImages / 2) {
                            start = i - numBackgroundImages / 2;
                            end = start + numBackgroundImages;
                        } else {
                            end = size;
                            start = end - numBackgroundImages;
                        }
                    } else {
                        start = 0;
                        end = numBackgroundImages;
                    }
                    // print("index " + std::to_string(i) + "start " + std::to_string(start) + " end " + std::to_string(end), true, true);
                    backgroundCorrectionModel(workingImages, backgrounds[i], start, end);
                }
            }
        }
    } else {
        for (size_t i = 0; i < size; i++) {
            backgrounds.push_back(cv::Mat::zeros(workingImages[i].rows, workingImages[i].cols, CV_8U));
        }
    }

    std::vector<std::vector<SegmenterObject>> objects;
    Info result = _detection(imageBuffer, startPos, size, backgrounds, objects);
    checkInfo(result, enableDetailedPrinting, enableDetailedPrinting);

    // finish task
    result = finishSegmenterTask(mutex, imageBuffer, startPos, size, files, objects);
    checkInfo(result, enableDetailedPrinting, enableDetailedPrinting);
    objects.clear();

    // clean up
    backgrounds.clear();
    workingImages.clear();

    return Success;
}

void skipFrames(cv::VideoCapture& capture, size_t nFrames)
{
    for (size_t i = 0; i < nFrames; i++) {
        bool result = capture.grab();
        if (!result)
            break;
    }
}

void _taskLoop(const std::vector<std::string>& files)
{
    Int* fileIndices = new Int[numReaderThreads];
    cv::VideoCapture* videoCaptures = new cv::VideoCapture[numReaderThreads];
    Int* bufferIndices = new Int[numBufferedStacks * 2]; // memory layout: { size1, start1, size2, start2, ... }
                                                         //
    // allocate memory for image buffer. Double the size to allow for stack overflows. This is wastefull of memory, but should be fine as only pointers are stored. The images are not allocated before reading.
    Image* imageBuffer = new Image[2 * bufferSize];

    // allocate memory for buffer and prepare first tasks
    for (Int i = 0; i < numBufferedStacks; i++) {
        bufferIndices[i * 2] = 0;
        bufferIndices[i * 2 + 1] = i * 2 * stackSize;
        // bufferIndices[i * (2 * stackSize + 1)] = 0;
        // for (int j = 0; j < 2 * stackSize; j++) {
        //     bufferIndices[i * (2 * stackSize + 1) + j + 1] = i * 2 * stackSize + j;
        // }
    }

    taskFunction readerFunction;
    if (inputIsVideo) {
        for (Int i = 0; i < numReaderThreads; i++) {
            videoCaptures[i].open(sourcePath);
            fileIndices[i] = size_t(videoCaptures[i].get(cv::CAP_PROP_FRAME_COUNT)) / numReaderThreads * i;
            skipFrames(videoCaptures[i], fileIndices[i]);
        }

        readerFunction = [imageBuffer, fileIndices, videoCaptures](const size_t& threadId,
                             std::mutex& mutex, void* indices, const size_t& taskIdx,
                             const size_t& taskSize) { return _read(threadId, mutex, indices, taskIdx, taskSize, fileIndices, imageBuffer, videoCaptures); };
    } else {
        readerFunction = [files, imageBuffer, fileIndices](const size_t& threadId, std::mutex& mutex,
                             void* indices, const size_t& taskIdx, const size_t& taskSize) { return _read(threadId,
                                                                                                 mutex, indices, taskIdx, taskSize, files, fileIndices, imageBuffer); };
        for (Int i = 0; i < numReaderThreads; i++) {
            fileIndices[i] = files.size() / numReaderThreads * i;
        }
    }
    taskFunction segmenterFunction = [imageBuffer, files](const size_t& threadId, std::mutex& mutex, void* indices, const size_t& taskIdx, const size_t& taskSize) { return _segment(threadId, mutex, indices, taskIdx, taskSize, imageBuffer, files); };

    taskFunction functions[] = { readerFunction, segmenterFunction };

    ThreadManager tmReader(bufferIndices, functions, numReaderThreads, true, numBufferedStacks);
    ThreadManager tmSegmenter(bufferIndices, functions, numSegmenterThreads, true, numBufferedStacks);

    // initialize reader tasks
    Info info;
    for (Int i = 0; i < numBufferedStacks; i++) {
        Task task;
        task.taskIdx = 2 * i;
        task.size = 1;
        task.funcIdx = 0;

        info = tmReader.addTask(task);
        checkInfo(info, enableDetailedPrinting, enableDetailedPrinting);
    }

    Task finishedTask;
    Int counter = 0;
    while (counter < numReaderThreads) {
        tmReader.getFinishedTask(finishedTask);
        checkInfo(finishedTask.threadManagerInfo, enableDetailedPrinting, enableDetailedPrinting);
        checkInfo(finishedTask.taskInfo, enableDetailedPrinting, enableDetailedPrinting);

        if (finishedTask.taskInfo == ThreadManagerThisWorkerFinished)
            counter++;

        Task segmenterTask;
        segmenterTask.taskIdx = finishedTask.taskIdx;
        segmenterTask.size = 1;
        segmenterTask.funcIdx = 1;

        info = tmSegmenter.addTask(segmenterTask);
        checkInfo(info, enableDetailedPrinting, enableDetailedPrinting);

        //////////////////////////////////////////7
        tmSegmenter.getFinishedTask(finishedTask);
        checkInfo(finishedTask.threadManagerInfo, enableDetailedPrinting, enableDetailedPrinting);
        checkInfo(finishedTask.taskInfo, enableDetailedPrinting, enableDetailedPrinting);

        Task readerTask;
        readerTask.taskIdx = finishedTask.taskIdx;
        readerTask.size = 1;
        readerTask.funcIdx = 0;

        info = tmReader.addTask(readerTask);
        checkInfo(info, enableDetailedPrinting, enableDetailedPrinting);
    }

    tmReader.finishThreads();
    tmSegmenter.finishThreads();
    do {
        tmSegmenter.getFinishedTask(finishedTask);
        checkInfo(finishedTask.threadManagerInfo, enableDetailedPrinting, enableDetailedPrinting);
        checkInfo(finishedTask.taskInfo, enableDetailedPrinting, enableDetailedPrinting);
    } while (finishedTask.threadManagerInfo == Success);

    std::cout << "--------------------------------------------------------------------" << std::endl;
    std::cout << "Metrics: " << std::endl;
    std::cout << "Reader:\n";
    std::cout << "\tAverage wait time: " << tmReader.getAvgWaitTime() << "ms" << std::endl;
    std::cout << "\tAverage task run time: " << tmReader.getAvgTaskRunTime() << "ms" << std::endl;
    std::cout << "Segmenter:\n";
    std::cout << "\tAverage wait time: " << tmSegmenter.getAvgWaitTime() << "ms" << std::endl;
    std::cout << "\tAverage task run time: " << tmSegmenter.getAvgTaskRunTime() << "ms" << std::endl;

    tmReader.cleanUp();
    tmSegmenter.cleanUp();

    // clean up
    delete[] bufferIndices;
    delete[] fileIndices;
    // for (int i = 0; i < 2 * bufferSize; i++) {
    //     imageBuffer[i].workingImage.release();
    //     imageBuffer[i].originalImage.release();
    // }
    delete[] imageBuffer;
    delete[] videoCaptures;
}

void runSegmenter()
{
    auto start = std::chrono::high_resolution_clock::now();
    if (saveMode == oneFile)
        checkInfo(initWriter(), enableDetailedPrinting, enableDetailedPrinting);

    std::vector<std::string> files;
    if (inputIsVideo) {
        cv::VideoCapture videoCapture;
        videoCapture.open(sourcePath);
        size_t nFrames = videoCapture.get(cv::CAP_PROP_FRAME_COUNT);
        for (size_t i = 0; i < nFrames; i++) {
            files.push_back(splitLast(sourcePath, '/') + "_frame_" + std::to_string(i) + ".png");
        }

    } else {
        getFiles(sourcePath, files);
        print("Found " + std::to_string(files.size()) + " files");

        if (numReaderThreads * stackSize > Int(files.size())) {
            print(makeYellow("Not enough images to fill all reader threads. Decrease the number of reader threads."), true, true);
        }
    }

    _taskLoop(files);

    // clean up
    objectSaveFile.close();

    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    duration /= 1000;
    print("Runtime of segmenter: " + std::to_string(duration) + "s", true, true);
    print("Avg. time per image: " + std::to_string(duration / files.size()) + "s", true, true);
    std::cout << "--------------------------------------------------------------------\n"
              << std::endl;
}
