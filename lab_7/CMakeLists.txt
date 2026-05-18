cmake_minimum_required(VERSION 3.14) 
project(CameraApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package(OpenCV REQUIRED)

include_directories(include)
include_directories(${OpenCV_INCLUDE_DIRS})

set(COMMON_SOURCES
    src/CameraProvider.cpp
    src/KeyProcessor.cpp
    src/FrameProcessor.cpp
    src/Display.cpp
    src/FaceDetector.cpp
)

add_executable(camera_app src/main.cpp ${COMMON_SOURCES})
target_link_libraries(camera_app ${OpenCV_LIBS})
set_target_properties(camera_app PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(lab7_tests 
    tests/test_main.cpp 
    ${COMMON_SOURCES}
)

target_link_libraries(lab7_tests PRIVATE Catch2::Catch2WithMain ${OpenCV_LIBS})

# Додаємо підтримку утиліти ctest (потрібно для запуску через CI)
enable_testing()
add_test(NAME RunLab7Tests COMMAND lab7_tests)
