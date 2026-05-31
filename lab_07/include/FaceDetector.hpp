#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

// Асинхронний детектор облич (зберігає високий FPS у головному потоці)
class FaceDetector {
public:
    // Завантажує модель та одразу запускає фоновий потік
    FaceDetector(const std::string& prototxt, 
                 const std::string& model);
                 
    // Коректно зупиняє потік при виході
    ~FaceDetector();

    // Передає кадр у фоновий потік (потокобезпечно)
    void updateFrame(const cv::Mat& frame);
    
    // Повертає останні знайдені координати (потокобезпечно)
    std::vector<cv::Rect> getFaces();

private:
    // Робочий цикл потоку, де виконується інференс нейромережі
    void detectionLoop();

    cv::dnn::Net net;
    std::thread workerThread;
    
    std::mutex mtx;               // Захищає доступ до спільних даних
    std::atomic<bool> isRunning;  // Прапорець для безпечної зупинки циклу

    // Спільні дані (Shared Data)
    cv::Mat currentFrame;
    std::vector<cv::Rect> detectedFaces;
    bool hasNewFrame = false;
};