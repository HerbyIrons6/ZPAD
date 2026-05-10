// FrameProcessor.hpp
#ifndef FRAME_PROCESSOR_HPP
#define FRAME_PROCESSOR_HPP
#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"

class FrameProcessor {
public:
    FrameProcessor();
    void process(cv::Mat& frame,
                 ProcessMode mode);
    int filterIntensity;

private:
    void drawOverlay(cv::Mat& frame);
    void applyGlitch(cv::Mat& frame);
    int64 startTick;
    double fps;
};

#endif