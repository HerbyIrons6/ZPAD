#!/bin/bash
# Скрипт для автоматизації збірки проекту
echo "Створення директорії build..."
mkdir -p build
cd build

echo "Запуск CMake та Make..."
cmake ..
make

echo "Збірка завершена. Файл знаходиться у build/Lab6"