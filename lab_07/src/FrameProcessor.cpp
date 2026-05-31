#include "FrameProcessor.hpp"

FrameProcessor::FrameProcessor()
    : filterIntensity(15),
      startTick(cv::getTickCount()),
      fps(0) {}

void FrameProcessor::process(
    cv::Mat& frame,
    ProcessMode mode
) {
    // Застосовуємо фільтр залежно від активного режиму
    switch (mode) {
        case ProcessMode::INVERT:
            // Інверсія кольорів (негатив)
            cv::bitwise_not(frame, frame);
            break;
        case ProcessMode::BLUR: {
            // Розмір ядра GaussianBlur обов'язково має бути непарним
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
            // Детекція контурів: спочатку конвертуємо в сірий (Canny вимагає 1 канал)
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
            // Конвертуємо назад у BGR, щоб UI (зелений текст) малювався коректно
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
    
    // Завжди малюємо інтерфейс поверх кадру
    drawOverlay(frame);
}

void FrameProcessor::applyGlitch(
    cv::Mat& frame
) {
    // Розділяємо кадр на 3 окремі канали: B (0), G (1), R (2)
    std::vector<cv::Mat> channels;
    cv::split(frame, channels);
    
    int offset = 15; // Величина горизонтального зсуву
    
    cv::Mat shifted =
        cv::Mat::zeros(
            channels[2].size(),
            channels[2].type()
        );

    // Копіюємо червоний канал зі зміщенням (ефект хроматичної аберації)
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

    // Збираємо канали назад у єдине зображення
    cv::merge(channels, frame);
}

void FrameProcessor::drawOverlay(
    cv::Mat& frame
) {
    // Динамічний розрахунок частоти кадрів (FPS)
    int64 currentTick =
        cv::getTickCount();
        
    fps =
        cv::getTickFrequency()
        / (currentTick - startTick);
        
    startTick = currentTick;

    // Формування рядка з текстом
    std::string text =
        "FPS: "
        + std::to_string((int)fps)
        + " | Mode: Active";
        
    // Відмальовка тексту (зелений колір)
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