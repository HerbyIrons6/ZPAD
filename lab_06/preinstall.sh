#!/bin/bash
# Скрипт для підготовки чистої системи до білду
echo "Оновлення списку пакетів та встановлення залежностей..."
sudo apt update
sudo apt install -y libopencv-dev cmake gcc g++ make
echo "Встановлення завершено успішно!"