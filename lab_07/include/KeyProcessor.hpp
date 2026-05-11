#ifndef KEY_PROCESSOR_HPP
#define KEY_PROCESSOR_HPP

// Перелік доступних режимів візуальної обробки
enum class ProcessMode {
    NORMAL,
    INVERT,
    BLUR,
    CANNY,
    GLITCH
};

// Клас для обробки клавіатури та керування станом програми
class KeyProcessor {
public:
    KeyProcessor();
    
    // Перемикає режим залежно від натиснутої клавіші (наприклад, 1-5)
    void handleKey(int key);
    
    // Повертає активний режим (використовується у головному циклі)
    ProcessMode getCurrentMode() const;

private:
    // Зберігає поточний обраний фільтр
    ProcessMode currentMode;
};
#endif