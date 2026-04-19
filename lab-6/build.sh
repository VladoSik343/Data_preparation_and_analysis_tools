#!/bin/bash
echo "========================================="
echo "Camera App - Build Script"
echo "========================================="
if [ ! -d "build" ]; then
    mkdir -p build
fi
cd build
echo "Configuring with CMake..."
cmake .. || { echo "CMake configuration failed"; exit 1; }

echo "Building the project..."
make -j$(nproc) || { echo "Build failed"; exit 1; }
if [ $? -eq 0 ]; then
    echo "========================================="
    echo "Build completed successfully!"
    if [ -f "bin/camera_app" ]; then
        echo "Executable is located at: build/bin/camera_app"
    elif [ -f "camera_app" ]; then
        echo "Executable is located at: build/camera_app"
    fi
    echo "========================================="
else
    echo "Error: Build failed"
    exit 1
fi

cd ..