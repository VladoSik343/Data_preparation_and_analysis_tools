#include "FaceDetector.hpp"
#include <iostream>

FaceDetector::FaceDetector() 
    : isRunning(false)
    , hasNewFrame(false)
    , confidenceThreshold(0.5f)
    , faceDetectionEnabled(false)
    , simulateDelay(false) {
}

FaceDetector::~FaceDetector() {
    stop();
}

bool FaceDetector::loadModel(const std::string& prototxtPath, const std::string& caffemodelPath) {
    try {
        net = cv::dnn::readNetFromCaffe(prototxtPath, caffemodelPath);
        
        if (net.empty()) {
            std::cerr << "Error: Could not load face detection model!" << std::endl;
            return false;
        }
        
        std::cout << "Face detection model loaded successfully!" << std::endl;
        return true;
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV Exception: " << e.what() << std::endl;
        return false;
    }
}

void FaceDetector::start() {
    if (isRunning) return;
    
    isRunning = true;
    workerThread = std::thread(&FaceDetector::workerFunction, this);
    std::cout << "Face detector thread started" << std::endl;
}

void FaceDetector::stop() {
    if (!isRunning) return;
    
    isRunning = false;
    cv_.notify_all();
    
    if (workerThread.joinable()) {
        workerThread.join();
    }
    
    std::cout << "Face detector thread stopped" << std::endl;
}

void FaceDetector::updateFrame(const cv::Mat& frame) {
    if (!faceDetectionEnabled || frame.empty()) return;
    
    std::lock_guard<std::mutex> lock(frameMutex);
    currentFrame = frame.clone();
    hasNewFrame = true;
    cv_.notify_one();
}

std::vector<Face> FaceDetector::getFaces() {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (faceDetectionEnabled && !detectedFaces.empty()) {
        return detectedFaces;
    }
    
    return std::vector<Face>();
}

void FaceDetector::setConfidenceThreshold(float threshold) {
    confidenceThreshold = threshold;
}

void FaceDetector::setFaceDetectionEnabled(bool enabled) {
    faceDetectionEnabled = enabled;
    
    if (!enabled) {
        std::lock_guard<std::mutex> lock(dataMutex);
        detectedFaces.clear();
    }
}

void FaceDetector::setSimulateDelay(bool enabled) {
    simulateDelay = enabled;
}

bool FaceDetector::isDetectionEnabled() const {
    return faceDetectionEnabled;
}

void FaceDetector::workerFunction() {
    cv::Mat frameToProcess;
    
    while (isRunning) {
        {
            std::unique_lock<std::mutex> lock(frameMutex);
            cv_.wait(lock, [this] { return hasNewFrame || !isRunning; });
            
            if (!isRunning) break;
            
            if (!currentFrame.empty()) {
                frameToProcess = currentFrame.clone();
                hasNewFrame = false;
            }
        }
        
        if (faceDetectionEnabled && !frameToProcess.empty()) {
            if (simulateDelay) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            
            std::vector<Face> faces = detectFaces(frameToProcess);
            
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                detectedFaces = faces;
            }
        }
    }
}

std::vector<Face> FaceDetector::detectFaces(const cv::Mat& frame) {
    std::vector<Face> faces;
    
    if (frame.empty() || net.empty()) {
        return faces;
    }
    
    try {
        cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0, 
                                               cv::Size(inputWidth, inputHeight),
                                               meanValues, false, false);
        
        net.setInput(blob);
        
        cv::Mat detections = net.forward();
        
        int numDetections = detections.size[2];
        
        for (int i = 0; i < numDetections; i++) {
            const float* detection = detections.ptr<float>(0, 0, i);
            
            if (!detection) continue;
            
            float confidence = detection[2];
            
            if (confidence > confidenceThreshold) {
                float x1 = detection[3] * frame.cols;
                float y1 = detection[4] * frame.rows;
                float x2 = detection[5] * frame.cols;
                float y2 = detection[6] * frame.rows;
                
                cv::Rect rect(static_cast<int>(x1), static_cast<int>(y1),
                             static_cast<int>(x2 - x1), static_cast<int>(y2 - y1));
                
                rect = rect & cv::Rect(0, 0, frame.cols, frame.rows);
                
                if (rect.width > 0 && rect.height > 0) {
                    faces.push_back(Face(rect, confidence));
                }
            }
        }
        
    } catch (const cv::Exception& e) {
        std::cerr << "Face detection error: " << e.what() << std::endl;
    }
    
    return faces;
}