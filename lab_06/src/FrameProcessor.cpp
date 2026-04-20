#include "FrameProcessor.hpp"
#include <vector>

FrameProcessor::FrameProcessor() : filterIntensity(15), startTick(cv::getTickCount()), fps(0) {}

void FrameProcessor::process(cv::Mat& frame, ProcessMode mode) {
    // Реалізація різних фільтрів OpenCV
    switch (mode) {
        case ProcessMode::INVERT:
            cv::bitwise_not(frame, frame);
            break;
        case ProcessMode::BLUR:
            // Перевіряємо, щоб розмір ядра був непарним
            {
                int kSize = (filterIntensity % 2 == 0) ? filterIntensity + 1 : filterIntensity;
                cv::GaussianBlur(frame, frame, cv::Size(kSize, kSize), 0);
            }
            break;
        case ProcessMode::CANNY:
            cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
            cv::Canny(frame, frame, 50, 150);
            cv::cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
            break;
        case ProcessMode::GLITCH:
            applyGlitch(frame);
            break;
        default: break;
    }
    drawOverlay(frame); // Малюємо FPS та текст
}

void FrameProcessor::applyGlitch(cv::Mat& frame) {
    std::vector<cv::Mat> channels;
    cv::split(frame, channels);   

    // Створюємо порожню (чорну) матрицю такого ж розміру
    cv::Mat shiftR = cv::Mat::zeros(frame.size(), channels[0].type());
   
    // Вказуємо, що копіюємо (оригінальний канал без останніх 15 пікселів справа)
    cv::Rect srcRect(0, 0, frame.cols - 15, frame.rows);  
   
    // Вказуємо, куди вставляємо (відступаємо 15 пікселів зліва)
    cv::Rect dstRect(15, 0, frame.cols - 15, frame.rows);
  
    // Копіюємо вирізаний шматок у зсунуту позицію
    channels[2](srcRect).copyTo(shiftR(dstRect));
    channels[2] = shiftR;
    
    cv::merge(channels, frame);
}

void FrameProcessor::drawOverlay(cv::Mat& frame) {
    // Розрахунок FPS
    int64 currentTick = cv::getTickCount();
    fps = cv::getTickFrequency() / (currentTick - startTick);
    startTick = currentTick;

    std::string text = "FPS: " + std::to_string((int)fps) + " | Mode: Active";
    cv::putText(frame, text, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
}