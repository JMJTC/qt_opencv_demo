cmake_minimum_required(VERSION 3.10)
project(QtOpenCVDemo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)   # 自动处理 Qt MOC
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(qt_opencv_demo src/main.cpp)
target_link_libraries(qt_opencv_demo Qt5::Widgets ${OpenCV_LIBS})
