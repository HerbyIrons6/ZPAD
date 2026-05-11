#include "FaceDetector.hpp"

FaceDetector::FaceDetector(
    const std::string& prototxt,
    const std::string& model
) {
    // Завантажуємо попередньо натреновану мережу ResNet
    net = cv::dnn::readNetFromCaffe(
        prototxt,
        model
    );

    isRunning = true;

    // Запускаємо метод detectionLoop у окремому фоновому потоці
    workerThread = std::thread(
        &FaceDetector::detectionLoop,
        this
    );
}

FaceDetector::~FaceDetector() {
    // Сигналізуємо фоновому потоку про завершення роботи
    isRunning = false;

    // Чекаємо, поки фоновий потік коректно завершить свій цикл
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void FaceDetector::updateFrame(
    const cv::Mat& frame
) {
    // Блокуємо м'ютекс для безпечного запису спільного ресурсу
    std::lock_guard<std::mutex> lock(mtx);
    currentFrame = frame.clone(); // Робимо глибоку копію кадру
    hasNewFrame = true;
}

std::vector<cv::Rect> FaceDetector::getFaces() {
    // Блокуємо м'ютекс для безпечного читання
    std::lock_guard<std::mutex> lock(mtx);
    return detectedFaces;
}

void FaceDetector::detectionLoop() {

    while (isRunning) {
        cv::Mat frameToProcess;
        
        // 1. Блок отримання кадру
        {
            std::lock_guard<std::mutex> lock(mtx);

            if (!hasNewFrame ||
                currentFrame.empty()) {
                
                // Якщо нового кадру немає, засинаємо на 1мс, щоб не навантажувати процесор на 100%
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(1)
                );

                continue;
            }
            frameToProcess =
                currentFrame.clone();

            hasNewFrame = false;
        }

        // 2. Підготовка зображення для нейромережі (формат Blob)
        // Масштабуємо до 300x300 та віднімаємо середні значення каналів (Mean Subtraction)
        cv::Mat blob =
            cv::dnn::blobFromImage(
                frameToProcess,
                1.0,
                cv::Size(300, 300),
                cv::Scalar(104, 177, 123)
            );

        net.setInput(blob);

        // 3. Прямий прохід нейромережі (Інференс)
        cv::Mat detection = net.forward();

        // 4. Парсинг результату (вихід мережі - це 4D тензор)
        cv::Mat detectionMat(
            detection.size[2],
            detection.size[3],
            CV_32F,
            detection.ptr<float>()
        );

        std::vector<cv::Rect> faces;

        for (int i = 0;
             i < detectionMat.rows;
             i++) {

            // Отримуємо ймовірність (впевненість мережі), що це обличчя
            float confidence =
                detectionMat.at<float>(i, 2);

            // Відсіюємо помилкові спрацьовування (поріг 50%)
            if (confidence > 0.5f) {

                int x1 = static_cast<int>(
                    detectionMat.at<float>(i, 3)
                    * frameToProcess.cols
                );
                int y1 = static_cast<int>(
                    detectionMat.at<float>(i, 4)
                    * frameToProcess.rows
                );
                int x2 = static_cast<int>(
                    detectionMat.at<float>(i, 5)
                    * frameToProcess.cols
                );
                int y2 = static_cast<int>(
                    detectionMat.at<float>(i, 6)
                    * frameToProcess.rows
                );
        
                // Зберігаємо координати знайденого обличчя
                faces.push_back(
                    cv::Rect(
                        cv::Point(x1, y1),
                        cv::Point(x2, y2)
                    )
                );
            }
        }

        // 5. Безпечний запис результатів для головного потоку
        {
            std::lock_guard<std::mutex> lock(mtx);
            detectedFaces = faces;
        }
        // НЕ 500мс
        // Оптимальна затримка для зниження навантаження на CPU під час фонової роботи
        std::this_thread::sleep_for(
            std::chrono::milliseconds(30)
        );
    }
}