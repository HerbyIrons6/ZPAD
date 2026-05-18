#!/bin/bash
# Скрипт для швидкого запуску програми

if [ ! -f "./build/Lab7" ]; then
    echo "Помилка: Програма не скомпільована. Запустіть ./build.sh спочатку."
    exit 1
fi

./build/Lab7