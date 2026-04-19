cmake_minimum_required(VERSION 3.10)
project(CameraApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV package
find_package(OpenCV REQUIRED)

include_directories(include)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SOURCES
    src/main.cpp
    src/CameraProvider.cpp
    src/KeyProcessor.cpp
    src/FrameProcessor.cpp
    src/Display.cpp
    src/FaceDetector.cpp
)

add_executable(camera_app ${SOURCES})

target_link_libraries(camera_app ${OpenCV_LIBS})

set_target_properties(camera_app PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")