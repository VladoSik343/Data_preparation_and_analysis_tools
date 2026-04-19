#!/bin/bash
echo "========================================="
echo "Camera App - Pre-installation Script"
echo "========================================="
echo "Updating package list..."
sudo apt-get update
echo "Installing required packages..."
sudo apt-get install -y libopencv-dev cmake g++ make
if [ $? -eq 0 ]; then
    echo "========================================="
    echo "All packages installed successfully!"
    echo "Installed versions:"
    if command -v cmake &> /dev/null; then
        echo "  CMake: $(cmake --version | head -n1)"
    fi
    if command -v g++ &> /dev/null; then
        echo "  GCC: $(g++ --version | head -n1)"
    fi
    if pkg-config --modversion opencv4 &> /dev/null; then
        echo "  OpenCV: $(pkg-config --modversion opencv4)"
    elif pkg-config --modversion opencv &> /dev/null; then
        echo "  OpenCV: $(pkg-config --modversion opencv)"
    else
        echo "  OpenCV: installed"
    fi
    echo "========================================="
else
    echo "Error: Failed to install packages"
    exit 1
fi