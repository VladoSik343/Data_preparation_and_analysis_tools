#!/bin/bash
echo "========================================="
echo "Camera App - Run Script"
echo "========================================="
if [ -f "build/bin/camera_app" ]; then
    echo "Starting camera application..."
    ./build/bin/camera_app
elif [ -f "build/camera_app" ]; then
    echo "Starting camera application..."
    ./build/camera_app
elif [ -f "camera_app" ]; then
    echo "Starting camera application..."
    ./camera_app
else
    echo "Error: Executable not found!"
    echo "Please run ./build.sh first"
    exit 1
fi