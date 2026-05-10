#!/bin/bash
# Скрипт для підготовки чистої системи до білду
echo "Оновлення списку пакетів та встановлення залежностей..."
sudo apt update
sudo apt install -y libopencv-dev cmake gcc g++ make
echo "Встановлення завершено успішно!"
# Завантаження моделей для детекції облич
wget -nc https://raw.githubusercontent.com/opencv/opencv/master/samples/dnn/face_detector/deploy.prototxt
wget -nc https://raw.githubusercontent.com/opencv/opencv_3rdparty/dnn_samples_face_detector_20170830/res10_300x300_ssd_iter_140000.caffemodel