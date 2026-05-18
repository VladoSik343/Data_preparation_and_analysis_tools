#include <iostream>
#include <memory>
#include "CameraProvider.hpp"
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "Display.hpp"
#include "FaceDetector.hpp"

void onBrightnessChange(int value, void* userdata) {
    FrameProcessor* processor = static_cast<FrameProcessor*>(userdata);
    processor->setBrightness(value);
}

void onBlurChange(int value, void* userdata) {
    FrameProcessor* processor = static_cast<FrameProcessor*>(userdata);
    processor->setBlurKernelSize(value);
}

void onCannyChange(int value, void* userdata) {
    FrameProcessor* processor = static_cast<FrameProcessor*>(userdata);
    processor->setCannyThresholds(value, value * 3);
}

void onBinaryChange(int value, void* userdata) {
    FrameProcessor* processor = static_cast<FrameProcessor*>(userdata);
    processor->setBinaryThreshold(value);
}

int main() {
    std::cout << "=========================================" << std::endl;
    std::cout << "Camera Application Starting..." << std::endl;
    std::cout << "=========================================" << std::endl;
    
    CameraProvider camera(0);
    if (!camera.isCameraOpened()) {
        std::cerr << "Failed to open camera!" << std::endl;
        return -1;
    }
    
    auto faceDetector = std::make_shared<FaceDetector>();
    
    if (!faceDetector->loadModel("models/deploy.prototxt", 
                                  "models/res10_300x300_ssd_iter_140000.caffemodel")) {
        std::cerr << "Warning: Face detection model not loaded!" << std::endl;
    }
    
    faceDetector->start();
    faceDetector->setConfidenceThreshold(0.5f);
    
    KeyProcessor keyProcessor;
    FrameProcessor frameProcessor;
    Display display("Camera App - Interactive Processing with Face Detection");
    
    frameProcessor.setFaceDetector(faceDetector);
    
    int brightnessVal = 128;
    int blurVal = 15;
    int cannyVal = 50;
    int binaryVal = 127;
    
    display.createTrackbar("Brightness", &brightnessVal, 255);
    display.createTrackbar("Blur Size", &blurVal, 50);
    display.createTrackbar("Canny Threshold", &cannyVal, 255);
    display.createTrackbar("Binary Threshold", &binaryVal, 255);
    
    display.setMouseCallback(&keyProcessor);
    
    std::cout << "Application ready!" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  1 - Normal mode" << std::endl;
    std::cout << "  2 - Invert colors" << std::endl;
    std::cout << "  3 - Gaussian blur" << std::endl;
    std::cout << "  4 - Canny edge detection" << std::endl;
    std::cout << "  5 - Sobel filter" << std::endl;
    std::cout << "  6 - Binary threshold" << std::endl;
    std::cout << "  7 - Glitch effect" << std::endl;
    std::cout << "  d - Drawing mode (click and drag mouse)" << std::endl;
    std::cout << "  z - Zoom mode (use mouse wheel)" << std::endl;
    std::cout << "  r - Rotate mode" << std::endl;
    std::cout << "  f - Face detection mode (multi-threaded)" << std::endl;
    std::cout << "  ESC - Exit" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "\nNOTE: Face detection runs in separate thread!" << std::endl;
    std::cout << "Video remains smooth even during face detection." << std::endl;
    std::cout << "=========================================" << std::endl;

    while (true) {
        cv::Mat frame = camera.getFrame();
        if (frame.empty()) {
            std::cerr << "Failed to get frame from camera!" << std::endl;
            break;
        }
        
        MouseState mouseState = keyProcessor.getMouseState();
        cv::Mat processedFrame = frameProcessor.process(frame, keyProcessor.getCurrentMode(), mouseState);
        
        display.show(processedFrame);
        
        frameProcessor.setCannyThresholds(cannyVal, cannyVal * 3);
        frameProcessor.setBinaryThreshold(binaryVal);
        
        int key = cv::waitKey(1);
        if (key == 27) { 
            std::cout << "Exiting application..." << std::endl;
            break;
        }
        
        keyProcessor.processKey(key);
        if (key == 's' || key == 'S') {
            static bool simEnabled = false;
            simEnabled = !simEnabled;
            faceDetector->setSimulateDelay(simEnabled);
            std::cout << "Simulation delay: " << (simEnabled ? "ON (blocking mode)" : "OFF (normal mode)") << std::endl;
            if (simEnabled) {
                std::cout << "Notice: Video may become choppy due to blocking detection!" << std::endl;
            } else {
                std::cout << "Video remains smooth with multi-threaded detection!" << std::endl;
            }
        }
    }
    faceDetector->stop();

    std::cout << "Application terminated successfully!" << std::endl;
    return 0;
}