#include "FaceDetector.hpp"

FaceDetector::FaceDetector(const std::string& prototxt, const std::string& model) {
    net = cv::dnn::readNetFromCaffe(prototxt, model);
    isRunning = true;
    workerThread = std::thread(&FaceDetector::detectionLoop, this);
}

FaceDetector::~FaceDetector() {
    isRunning = false;
    if (workerThread.joinable()) workerThread.join();
}

void FaceDetector::updateFrame(const cv::Mat& frame) {
    std::lock_guard<std::mutex> lock(mtx);
    frame.copyTo(currentFrame);
    hasNewFrame = true;
}

std::vector<cv::Rect> FaceDetector::getFaces() {
    std::lock_guard<std::mutex> lock(mtx);
    return detectedFaces;
}

void FaceDetector::detectionLoop() {
    while (isRunning) {
        cv::Mat frameToProcess;
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!hasNewFrame || currentFrame.empty()) continue;
            currentFrame.copyTo(frameToProcess);
            hasNewFrame = false;
        }

        cv::Mat blob = cv::dnn::blobFromImage(frameToProcess, 1.0, cv::Size(300, 300), cv::Scalar(104, 177, 123));
        net.setInput(blob);
        cv::Mat detection = net.forward();
        
        cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
        std::vector<cv::Rect> faces;

        for (int i = 0; i < detectionMat.rows; i++) {
            float confidence = detectionMat.at<float>(i, 2);
            if (confidence > 0.5) { 
                int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frameToProcess.cols);
                int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frameToProcess.rows);
                int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frameToProcess.cols);
                int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frameToProcess.rows);
                faces.push_back(cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2)));
            }
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            detectedFaces = faces;
        }
        // Навантаження для демонстрації плавності
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}