#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"

class Display {
private:
    std::string windowName;
    bool isInitialized;
public:
    Display(const std::string& name = "Camera App");
    ~Display();
    void show(const cv::Mat& image);
    void createTrackbar(const std::string& trackbarName, int* value, int max);
    void setMouseCallback(KeyProcessor* processor);
    bool isWindowOpen() const;
};
#endif