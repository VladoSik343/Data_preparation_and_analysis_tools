#include "Display.hpp"

Display::Display(const std::string& name) : windowName(name), isInitialized(false) {
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    isInitialized = true;
}

Display::~Display() {
    if (isInitialized) {
        cv::destroyWindow(windowName);
    }
}

void Display::show(const cv::Mat& image) {
    if (!image.empty() && isInitialized) {
        cv::imshow(windowName, image);
    }
}

void Display::createTrackbar(const std::string& trackbarName, int* value, int max) {
    cv::createTrackbar(trackbarName, windowName, value, max);
}

void Display::setMouseCallback(KeyProcessor* processor) {
    cv::setMouseCallback(windowName, KeyProcessor::onMouse, processor);
}

bool Display::isWindowOpen() const {
    return cv::getWindowProperty(windowName, cv::WND_PROP_VISIBLE) > 0;
}