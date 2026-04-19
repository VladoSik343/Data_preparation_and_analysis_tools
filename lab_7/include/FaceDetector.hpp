#ifndef FACE_DETECTOR_HPP
#define FACE_DETECTOR_HPP
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <vector>

struct Face {
    cv::Rect boundingBox;
    float confidence;
    bool isValid;
    
    Face() : confidence(0.0f), isValid(false) {}
    Face(const cv::Rect& rect, float conf) : boundingBox(rect), confidence(conf), isValid(true) {}
};

class FaceDetector {
private:
    cv::dnn::Net net;
    std::thread workerThread;
    std::mutex dataMutex;
    std::mutex frameMutex;
    std::condition_variable cv_;
    
    std::atomic<bool> isRunning;
    std::atomic<bool> hasNewFrame;
    
    cv::Mat currentFrame;
    std::vector<Face> detectedFaces;
    std::vector<Face> lastDetectedFaces;
    
    float confidenceThreshold;
    bool faceDetectionEnabled;
    bool simulateDelay;
    
    const int inputWidth = 300;
    const int inputHeight = 300;
    const cv::Scalar meanValues = cv::Scalar(104.0, 177.0, 123.0);
    
public:
    FaceDetector();
    ~FaceDetector();
    
    bool loadModel(const std::string& prototxtPath, const std::string& caffemodelPath);
    void start();
    void stop();
    
    void updateFrame(const cv::Mat& frame);
    std::vector<Face> getFaces();
    
    void setConfidenceThreshold(float threshold);
    void setFaceDetectionEnabled(bool enabled);
    void setSimulateDelay(bool enabled);
    bool isDetectionEnabled() const;
    
private:
    void workerFunction();
    std::vector<Face> detectFaces(const cv::Mat& frame);
};
#endif