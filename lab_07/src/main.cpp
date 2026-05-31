#include <opencv2/opencv.hpp>

#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "FaceDetector.hpp"

#include <iostream>

int main() {

    // Явно вказуємо бекенд V4L2 (Video for Linux)
    // для стабільної роботи камери у віртуальній машині
    cv::VideoCapture cap(0, cv::CAP_V4L2);

    // Зменшуємо буфер камери до 1 кадру,
    // щоб мінімізувати затримку відео (input lag)
    cap.set(cv::CAP_PROP_BUFFERSIZE, 1);

    if (!cap.isOpened()) {

        std::cerr
            << "Помилка: Не вдалося відкрити камеру!"
            << std::endl;

        return -1;
    }

    KeyProcessor keyProc;

    FrameProcessor frameProc;

    // Ініціалізація нейромережі та запуск фонового потоку
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

        if (!cap.read(frame)) {
            break;
        }

        if (frame.empty()) {
            break;
        }

        // Робимо чисту копію кадру для нейромережі.
        // Це необхідно, щоб фільтри (наприклад, Blur або Invert)
        // не спотворили дані і не зламали детекцію обличчя.
        cv::Mat cleanFrame = frame.clone();

        if (faceMode) {
            // Відправляємо оригінальний кадр у фоновий потік (асинхронно)
            detector.updateFrame(cleanFrame);
        }

        // Накладаємо візуальні ефекти на поточний кадр
        // Це відбувається в основному потоці (UI Thread)
        frameProc.process(
            frame,
            keyProc.getCurrentMode()
        );

        if (faceMode) {

            // Отримуємо останні розраховані координати з фонового потоку.
            // М'ютекс всередині гарантує потокобезпечне читання.
            std::vector<cv::Rect> faces =
                detector.getFaces();

            // Малюємо рамки поверх кадру (вже після накладання фільтрів)
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
        
        // Використовуємо 1мс замість 30мс, щоб головний
        // потік (UI) працював на максимально можливому FPS.
        // Важкі обчислення нас більше не блокують.
        int key = cv::waitKey(1);

        if (key == 27) {
            break; // ESC
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