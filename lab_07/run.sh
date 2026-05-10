#!/bin/bash
# Скрипт для швидкого запуску програми
if [ -f "./build/Lab6" ]; then
    ./build/Lab6
else
    echo "Помилка: Програма не скомпільована. Запустіть ./build.sh спочатку."
fi