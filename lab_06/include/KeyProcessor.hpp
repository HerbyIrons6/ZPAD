#ifndef KEY_PROCESSOR_HPP
#define KEY_PROCESSOR_HPP

// Перерахування режимів обробки кадру
enum class ProcessMode {
    NORMAL,    // Без фільтрів
    INVERT,    // Інверсія кольорів
    BLUR,      // Розмиття Гаусса
    CANNY,     // Детектор меж Кенні
    GLITCH     // Ефект зсуву каналів
};

class KeyProcessor {
public:
    KeyProcessor();
    void handleKey(int key); // Обробка натискання клавіші
    ProcessMode getCurrentMode() const; // Отримання поточного режиму
private:
    ProcessMode currentMode;
};

#endif