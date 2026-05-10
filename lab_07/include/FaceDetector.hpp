#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

class FaceDetector {
public:
    FaceDetector(const std::string& prototxt,
                 const std::string& model);

    ~FaceDetector();

    void updateFrame(const cv::Mat& frame);

    std::vector<cv::Rect> getFaces();

private:
    void detectionLoop();

    cv::dnn::Net net;

    std::thread workerThread;

    std::mutex mtx;

    std::atomic<bool> isRunning;

    cv::Mat currentFrame;

    std::vector<cv::Rect> detectedFaces;

    bool hasNewFrame = false;
};