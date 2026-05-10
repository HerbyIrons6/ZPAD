// KeyProcessor.hpp
#ifndef KEY_PROCESSOR_HPP
#define KEY_PROCESSOR_HPP

enum class ProcessMode {
    NORMAL,
    INVERT,
    BLUR,
    CANNY,
    GLITCH
};

class KeyProcessor {
public:
    KeyProcessor();
    void handleKey(int key);
    ProcessMode getCurrentMode() const;

private:
    ProcessMode currentMode;
};

#endif