#ifndef FRAME_PROCESSOR_HPP
#define FRAME_PROCESSOR_HPP

#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"

// Клас для візуальної обробки кадрів (фільтри, ефекти, накладання UI)
class FrameProcessor {
public:
    FrameProcessor();

    // Головний метод: застосовує ефекти залежно від обраного режиму
    void process(cv::Mat& frame, ProcessMode mode);
    
    // Інтенсивність ефектів (прив'язана до повзунка/Trackbar у головному вікні)
    int filterIntensity;

private:
    // Накладає текстовий інтерфейс (значення FPS, поточний режим)
    void drawOverlay(cv::Mat& frame);
    
    // Кастомний візуальний ефект зсуву кольорових каналів
    void applyGlitch(cv::Mat& frame);

    // Змінні для обчислення частоти кадрів (FPS)
    int64 startTick;
    double fps;
};

#endif