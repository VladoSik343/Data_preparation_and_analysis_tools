#!/bin/bash
echo "========================================="
echo "Camera App - Pre-installation Script"
echo "========================================="
check_lock() {
    if fuser /var/lib/dpkg/lock-frontend >/dev/null 2>&1; then
        echo "Waiting for dpkg lock to be released..."
        local count=0
        while fuser /var/lib/dpkg/lock-frontend >/dev/null 2>&1 && [ $count -lt 30 ]; do
            echo -n "."
            sleep 1
            ((count++))
        done
        echo ""
        if fuser /var/lib/dpkg/lock-frontend >/dev/null 2>&1; then
            echo "Warning: dpkg lock still held. You may need to wait or run: sudo killall unattended-upgr"
            return 1
        fi
    fi
    return 0
}

echo "Checking for system updates in progress..."
sleep 2

echo "Installing required packages..."
if pgrep unattended-upgr > /dev/null; then
    echo "Detected unattended-upgrades process running..."
    echo "You can either wait or run: sudo killall unattended-upgr"
    echo "Waiting 10 seconds for it to complete..."
    sleep 10
fi

MAX_RETRIES=3
RETRY_COUNT=0

while [ $RETRY_COUNT -lt $MAX_RETRIES ]; do
    echo "Attempt $((RETRY_COUNT + 1))/$MAX_RETRIES"
    
    sudo apt-get install -y libopencv-dev cmake g++ make wget
    
    if [ $? -eq 0 ]; then
        echo "Packages installed successfully!"
        break
    else
        RETRY_COUNT=$((RETRY_COUNT + 1))
        if [ $RETRY_COUNT -lt $MAX_RETRIES ]; then
            echo "Installation failed. Waiting 5 seconds before retry..."
            sleep 5
        fi
    fi
done

if [ $RETRY_COUNT -eq $MAX_RETRIES ]; then
    echo "========================================="
    echo "ERROR: Failed to install packages after $MAX_RETRIES attempts"
    echo ""
    echo "Please try the following commands manually:"
    echo ""
    echo "1. Kill the blocking process:"
    echo "   sudo killall unattended-upgr"
    echo ""
    echo "2. Or wait for updates to complete, then run:"
    echo "   sudo apt-get install -y libopencv-dev cmake g++ make wget"
    echo ""
    echo "3. If still failing, try:"
    echo "   sudo dpkg --configure -a"
    echo "   sudo apt-get install -f"
    echo "========================================="
    exit 1
fi

echo "========================================="
echo "Downloading Face Detection Model..."
echo "========================================="

mkdir -p models
if [ ! -f "models/deploy.prototxt" ]; then
    echo "Downloading deploy.prototxt..."
    wget -q --show-progress -O models/deploy.prototxt \
        https://raw.githubusercontent.com/opencv/opencv/master/samples/dnn/face_detector/deploy.prototxt
    if [ $? -eq 0 ]; then
        echo "deploy.prototxt downloaded successfully"
    else
        echo "Failed to download deploy.prototxt"
    fi
else
    echo "deploy.prototxt already exists"
fi

if [ ! -f "models/res10_300x300_ssd_iter_140000.caffemodel" ]; then
    echo "Downloading res10_300x300_ssd_iter_140000.caffemodel (approx 10MB)..."
    wget -q --show-progress -O models/res10_300x300_ssd_iter_140000.caffemodel \
        https://raw.githubusercontent.com/opencv/opencv_3rdparty/dnn_samples_face_detector_20170830/res10_300x300_ssd_iter_140000.caffemodel
    if [ $? -eq 0 ]; then
        echo "Face detection model downloaded successfully"
    else
        echo "Failed to download face detection model"
    fi
else
    echo "Face detection model already exists"
fi

echo "========================================="
echo "Pre-installation completed!"
echo ""
echo "Installed versions:"
if command -v cmake &> /dev/null; then
    cmake --version | head -n1
fi
if command -v g++ &> /dev/null; then
    g++ --version | head -n1
fi
if command -v wget &> /dev/null; then
    wget --version | head -n1
fi
echo ""
echo "Model files location: ./models/"
ls -lh models/ 2>/dev/null || echo "No model files found"
echo "========================================="