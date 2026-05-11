#include "KeyProcessor.hpp"

KeyProcessor::KeyProcessor() : currentMode(ProcessMode::NORMAL) {}

void KeyProcessor::handleKey(int key) {
    // Логіка перемикання режимів залежно від клавіш 1-5
    switch (key) {
        case '1': currentMode = ProcessMode::NORMAL; break;
        case '2': currentMode = ProcessMode::INVERT; break;
        case '3': currentMode = ProcessMode::BLUR; break;
        case '4': currentMode = ProcessMode::CANNY; break;
        case '5': currentMode = ProcessMode::GLITCH; break;
    }
}

ProcessMode KeyProcessor::getCurrentMode() const { return currentMode; }