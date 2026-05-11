#include "KeyProcessor.hpp"

// Конструктор: за замовчуванням встановлюємо чистий режим без фільтрів
KeyProcessor::KeyProcessor()
    : currentMode(ProcessMode::NORMAL) {}

void KeyProcessor::handleKey(int key) {
    // Мапінг натиснутих клавіш (1-5) на відповідні стани програми
    switch (key) {
        case '1':
            currentMode = ProcessMode::NORMAL;
            break;
        case '2':
            currentMode = ProcessMode::INVERT;
            break;
        case '3':
            currentMode = ProcessMode::BLUR;
            break;
        case '4':
            currentMode = ProcessMode::CANNY;
            break;
        case '5':
            currentMode = ProcessMode::GLITCH;
            break;
    }
}

// Геттер для безпечного отримання поточного стану в головному циклі (main)
ProcessMode KeyProcessor::getCurrentMode() const {
    return currentMode;
}