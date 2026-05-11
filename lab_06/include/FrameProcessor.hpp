#ifndef FRAME_PROCESSOR_HPP
#define FRAME_PROCESSOR_HPP

#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"

class FrameProcessor {
public:
    FrameProcessor();
    // Основний метод обробки кадру відповідно до обраного режиму
    void process(cv::Mat& frame, ProcessMode mode);
    
    // Поля для налаштувань (використовуються в Trackbar)
    int filterIntensity; 

private:
    // Допоміжні методи для візуалізації інформації
    void drawOverlay(cv::Mat& frame);
    void applyGlitch(cv::Mat& frame);
    
    int64 startTick;
    double fps;
};

#endif