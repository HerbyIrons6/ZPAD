#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include <iostream>

int main() {
    cv::VideoCapture cap(0); // Відкриваємо камеру за замовчуванням
    if (!cap.isOpened()) {
        std::cerr << "Помилка: Не вдалося відкрити камеру!" << std::endl;
        return -1;
    }

    KeyProcessor keyProc;
    FrameProcessor frameProc;
    const std::string winName = "Lab 6: OpenCV Processing";
    
    cv::namedWindow(winName);
    // Створюємо слайдер для регулювання інтенсивності фільтрів
    cv::createTrackbar("Intensity", winName, &frameProc.filterIntensity, 50);

    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // Обробляємо кадр відповідно до поточного режиму
        frameProc.process(frame, keyProc.getCurrentMode());

        cv::imshow(winName, frame);

        // Чекаємо 30мс на натискання клавіші
        int key = cv::waitKey(30);
        if (key == 27) break; // ESC для виходу
        keyProc.handleKey(key);
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}