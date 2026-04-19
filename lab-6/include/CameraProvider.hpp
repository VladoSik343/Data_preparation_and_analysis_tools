#ifndef CAMERA_PROVIDER_HPP
#define CAMERA_PROVIDER_HPP
#include <opencv2/opencv.hpp>
#include <iostream>

class CameraProvider {
private:
    cv::VideoCapture cap;
    int cameraId;
    bool isOpened;
public:
    CameraProvider(int cameraId = 0);
    ~CameraProvider();
    
    cv::Mat getFrame();
    bool isCameraOpened() const;
    double getFPS() const;
    void setResolution(int width, int height);
};
#endif