#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "FaceDetector.hpp" // Підключили наш новий клас
#include <iostream>

int main() {
    cv::VideoCapture cap(0); // Відкриваємо камеру за замовчуванням
    if (!cap.isOpened()) {
        std::cerr << "Помилка: Не вдалося відкрити камеру!" << std::endl;
        return -1;
    }

    KeyProcessor keyProc;
    FrameProcessor frameProc;
    
    // Ініціалізуємо багатопотоковий детектор облич
    FaceDetector detector("deploy.prototxt", "res10_300x300_ssd_iter_140000.caffemodel");
    bool faceMode = false; // Прапорець: чи увімкнений пошук облич

    const std::string winName = "Lab 7: OpenCV Multithreading"; // Змінили назву
    
    cv::namedWindow(winName);
    // Створюємо слайдер для регулювання інтенсивності фільтрів
    cv::createTrackbar("Intensity", winName, &frameProc.filterIntensity, 50);

    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // 1. Спочатку накладаємо старі фільтри (якщо вони увімкнені)
        frameProc.process(frame, keyProc.getCurrentMode());

        // 2. ЛОГІКА ДЛЯ 7 ЛАБОРАТОРНОЇ:
        if (faceMode) {
            // Передаємо копію кадру у фоновий потік (це не гальмує цикл!)
            detector.updateFrame(frame);

            // Забираємо координати (ті, що готові на даний момент)
            std::vector<cv::Rect> faces = detector.getFaces();
            
            // Малюємо рамки поверх поточного кадру
            for (const auto& face : faces) {
                cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
                cv::putText(frame, "Face", cv::Point(face.x, face.y - 10),
                            cv::FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(0, 255, 0), 2);
            }
        }

        cv::imshow(winName, frame);

        // Чекаємо 30мс на натискання клавіші
        int key = cv::waitKey(30);
        if (key == 27) break; // ESC для виходу
        
        // Обробка клавіші
        if (key == 'f' || key == 'F') {
            faceMode = !faceMode; // Вмикаємо/вимикаємо рамки обличчя
        } else if (key != -1) {
            keyProc.handleKey(key); // Інші клавіші передаємо в старий обробник
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}