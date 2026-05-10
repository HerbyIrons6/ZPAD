// FrameProcessor.cpp

#include "FrameProcessor.hpp"

FrameProcessor::FrameProcessor()
    : filterIntensity(15),
      startTick(cv::getTickCount()),
      fps(0) {}

void FrameProcessor::process(
    cv::Mat& frame,
    ProcessMode mode
) {

    switch (mode) {

        case ProcessMode::INVERT:

            cv::bitwise_not(frame, frame);

            break;

        case ProcessMode::BLUR: {

            int kSize =
                (filterIntensity % 2 == 0)
                ? filterIntensity + 1
                : filterIntensity;

            cv::GaussianBlur(
                frame,
                frame,
                cv::Size(kSize, kSize),
                0
            );

            break;
        }

        case ProcessMode::CANNY:

            cv::cvtColor(
                frame,
                frame,
                cv::COLOR_BGR2GRAY
            );

            cv::Canny(
                frame,
                frame,
                50,
                150
            );

            cv::cvtColor(
                frame,
                frame,
                cv::COLOR_GRAY2BGR
            );

            break;

        case ProcessMode::GLITCH:

            applyGlitch(frame);

            break;

        default:
            break;
    }

    drawOverlay(frame);
}

void FrameProcessor::applyGlitch(
    cv::Mat& frame
) {

    std::vector<cv::Mat> channels;

    cv::split(frame, channels);

    int offset = 15;

    cv::Mat shifted =
        cv::Mat::zeros(
            channels[2].size(),
            channels[2].type()
        );

    channels[2](
        cv::Rect(
            0,
            0,
            channels[2].cols - offset,
            channels[2].rows
        )
    ).copyTo(
        shifted(
            cv::Rect(
                offset,
                0,
                channels[2].cols - offset,
                channels[2].rows
            )
        )
    );

    channels[2] = shifted;

    cv::merge(channels, frame);
}

void FrameProcessor::drawOverlay(
    cv::Mat& frame
) {

    int64 currentTick =
        cv::getTickCount();

    fps =
        cv::getTickFrequency()
        / (currentTick - startTick);

    startTick = currentTick;

    std::string text =
        "FPS: "
        + std::to_string((int)fps)
        + " | Mode: Active";

    cv::putText(
        frame,
        text,
        cv::Point(10, 30),
        cv::FONT_HERSHEY_SIMPLEX,
        0.8,
        cv::Scalar(0, 255, 0),
        2
    );
}