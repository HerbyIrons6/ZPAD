// main.cpp
#include <opencv2/opencv.hpp>

#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "FaceDetector.hpp"

#include <iostream>

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {

        std::cerr
            << "Помилка: Не вдалося відкрити камеру!"
            << std::endl;
        return -1;
    }

    KeyProcessor keyProc;

    FrameProcessor frameProc;

    FaceDetector detector(
        "deploy.prototxt",
        "res10_300x300_ssd_iter_140000.caffemodel"
    );

    bool faceMode = false;

    const std::string winName =
        "Lab 7: OpenCV Multithreading";

    cv::namedWindow(winName);

    cv::createTrackbar(
        "Intensity",
        winName,
        &frameProc.filterIntensity,
        50
    );

    cv::Mat frame;

    while (true) {

        cap >> frame;

        if (frame.empty()) {
            break;
        }

        // СНАЧАЛА отправляем нормальный кадр
        // в нейронку

        if (faceMode) {
            detector.updateFrame(frame);
        }

        // ПОТОМ применяем фильтры

        frameProc.process(
            frame,
            keyProc.getCurrentMode()
        );

        // Рисуем рамки

        if (faceMode) {

            std::vector<cv::Rect> faces =
                detector.getFaces();

            for (const auto& face : faces) {

                cv::rectangle(
                    frame,
                    face,
                    cv::Scalar(0, 255, 0),
                    2
                );

                cv::putText(
                    frame,
                    "Face",
                    cv::Point(face.x, face.y - 10),
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.9,
                    cv::Scalar(0, 255, 0),
                    2
                );
            }
        }

        cv::imshow(winName, frame);

        int key = cv::waitKey(30);

        if (key == 27) {
            break;
        }

        if (key == 'f' || key == 'F') {

            faceMode = !faceMode;

        } else if (key != -1) {

            keyProc.handleKey(key);
        }
    }

    cap.release();

    cv::destroyAllWindows();

    return 0;
}