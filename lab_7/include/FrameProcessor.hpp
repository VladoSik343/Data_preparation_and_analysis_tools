#ifndef FRAME_PROCESSOR_HPP
#define FRAME_PROCESSOR_HPP
#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"
#include "FaceDetector.hpp"
#include <chrono>
#include <memory>

class FrameProcessor {
private:
    ProcessingMode mode;
    int blurKernelSize;
    int cannyThreshold1;
    int cannyThreshold2;
    int binaryThreshold;
    double zoomFactor;
    double rotationAngle;
    int brightness;
    
    std::chrono::steady_clock::time_point lastTime;
    int frameCount;
    double fps;
    
    std::shared_ptr<FaceDetector> faceDetector;
    
public:
    FrameProcessor();
    
    void setFaceDetector(std::shared_ptr<FaceDetector> detector);
    
    cv::Mat process(const cv::Mat& input, ProcessingMode currentMode, const MouseState& mouseState);
    void setBrightness(int value);
    void setBlurKernelSize(int size);
    void setCannyThresholds(int t1, int t2);
    void setBinaryThreshold(int thresh);
    
private:
    cv::Mat applyNormal(const cv::Mat& input);
    cv::Mat applyInvert(const cv::Mat& input);
    cv::Mat applyGaussianBlur(const cv::Mat& input);
    cv::Mat applyCanny(const cv::Mat& input);
    cv::Mat applySobel(const cv::Mat& input);
    cv::Mat applyBinary(const cv::Mat& input);
    cv::Mat applyGlitch(const cv::Mat& input);
    cv::Mat applyZoom(const cv::Mat& input, const MouseState& mouseState);
    cv::Mat applyRotate(const cv::Mat& input);
    cv::Mat applyDrawing(const cv::Mat& input, const MouseState& mouseState);
    cv::Mat applyFaceDetection(const cv::Mat& input);
    void drawText(cv::Mat& image);
    void updateFPS();
};

#endif