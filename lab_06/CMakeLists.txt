cmake_minimum_required(VERSION 3.10)
project(OpenCV_Lab6)

# Використовуємо стандарт C++17 для підтримки сучасних конструкцій
set(CMAKE_CXX_STANDARD 17)

# Пошук пакету OpenCV у системі (встановлюється через preinstall.sh)
find_package(OpenCV REQUIRED)

# Додаємо папку include для пошуку заголовочних файлів (.hpp)
include_directories(include ${OpenCV_INCLUDE_DIRS})

# Збираємо всі файли з папки src для компіляції
file(GLOB SOURCES "src/*.cpp")

# Створюємо виконуваний файл
add_executable(Lab6 ${SOURCES})

# Лінкуємо (зв'язуємо) нашу програму з бібліотеками OpenCV
target_link_libraries(Lab6 ${OpenCV_LIBS})