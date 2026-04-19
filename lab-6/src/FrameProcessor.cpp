#include "FrameProcessor.hpp"
FrameProcessor::FrameProcessor() 
    : mode(ProcessingMode::NORMAL)
    , blurKernelSize(15)
    , cannyThreshold1(50)
    , cannyThreshold2(150)
    , binaryThreshold(127)
    , zoomFactor(1.0)
    , rotationAngle(0.0)
    , brightness(0)
    , frameCount(0)
    , fps(0.0) {
    
    lastTime = std::chrono::steady_clock::now();
}

cv::Mat FrameProcessor::process(const cv::Mat& input, ProcessingMode currentMode, const MouseState& mouseState) {
    if (input.empty()) {
        return input;
    }
    
    mode = currentMode;
    cv::Mat result;
    
    switch(mode) {
        case ProcessingMode::NORMAL:
            result = applyNormal(input);
            break;
        case ProcessingMode::INVERT:
            result = applyInvert(input);
            break;
        case ProcessingMode::GAUSSIAN_BLUR:
            result = applyGaussianBlur(input);
            break;
        case ProcessingMode::CANNY:
            result = applyCanny(input);
            break;
        case ProcessingMode::SOBEL:
            result = applySobel(input);
            break;
        case ProcessingMode::BINARY:
            result = applyBinary(input);
            break;
        case ProcessingMode::GLITCH:
            result = applyGlitch(input);
            break;
        case ProcessingMode::DRAW_MODE:
            result = applyDrawing(input, mouseState);
            break;
        case ProcessingMode::ZOOM_MODE:
            result = applyZoom(input, mouseState);
            break;
        case ProcessingMode::ROTATE_MODE:
            result = applyRotate(input);
            break;
        default:
            result = input.clone();
    }
    
    updateFPS();
    drawText(result);
    
    return result;
}

cv::Mat FrameProcessor::applyNormal(const cv::Mat& input) {
    cv::Mat result = input.clone();
    if (brightness != 0) {
        result.convertTo(result, -1, 1, brightness);
    }
    return result;
}

cv::Mat FrameProcessor::applyInvert(const cv::Mat& input) {
    cv::Mat result;
    cv::bitwise_not(input, result);
    return result;
}

cv::Mat FrameProcessor::applyGaussianBlur(const cv::Mat& input) {
    cv::Mat result;
    int ksize = (blurKernelSize % 2 == 0) ? blurKernelSize + 1 : blurKernelSize;
    cv::GaussianBlur(input, result, cv::Size(ksize, ksize), 0);
    return result;
}

cv::Mat FrameProcessor::applyCanny(const cv::Mat& input) {
    cv::Mat gray, edges, result;
    cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    cv::Canny(gray, edges, cannyThreshold1, cannyThreshold2);
    cv::cvtColor(edges, result, cv::COLOR_GRAY2BGR);
    return result;
}

cv::Mat FrameProcessor::applySobel(const cv::Mat& input) {
    cv::Mat gray, grad_x, grad_y, abs_grad_x, abs_grad_y, grad, result;
    cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    
    cv::Sobel(gray, grad_x, CV_16S, 1, 0, 3);
    cv::Sobel(gray, grad_y, CV_16S, 0, 1, 3);
    
    cv::convertScaleAbs(grad_x, abs_grad_x);
    cv::convertScaleAbs(grad_y, abs_grad_y);
    
    cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
    cv::cvtColor(grad, result, cv::COLOR_GRAY2BGR);
    return result;
}

cv::Mat FrameProcessor::applyBinary(const cv::Mat& input) {
    cv::Mat gray, binary, result;
    cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, binary, binaryThreshold, 255, cv::THRESH_BINARY);
    cv::cvtColor(binary, result, cv::COLOR_GRAY2BGR);
    return result;
}

cv::Mat FrameProcessor::applyGlitch(const cv::Mat& input) {
    cv::Mat result = input.clone();
    std::vector<cv::Mat> channels;
    cv::split(result, channels);
    
    if (channels.size() >= 3) {
        int shift = rand() % 10 - 5;
        cv::Mat temp;
        cv::Mat transMat = (cv::Mat_<double>(2, 3) << 1, 0, shift, 0, 1, 0);
        cv::warpAffine(channels[0], temp, transMat, channels[0].size());
        channels[0] = temp;
        
        shift = rand() % 10 - 5;
        transMat = (cv::Mat_<double>(2, 3) << 1, 0, shift, 0, 1, 0);
        cv::warpAffine(channels[1], temp, transMat, channels[1].size());
        channels[1] = temp;
        
        cv::merge(channels, result);
    }
    return result;
}

cv::Mat FrameProcessor::applyZoom(const cv::Mat& input, const MouseState& mouseState) {
    cv::Mat result = input.clone();
    
    if (mouseState.action == MouseAction::MOUSE_WHEEL) {
        zoomFactor += mouseState.wheelDelta / 120.0 * 0.1;
        zoomFactor = std::max(0.5, std::min(zoomFactor, 3.0));
    }
    
    if (zoomFactor != 1.0) {
        cv::resize(result, result, cv::Size(), zoomFactor, zoomFactor);
        
        if (zoomFactor > 1.0) {
            int x = (result.cols - input.cols) / 2;
            int y = (result.rows - input.rows) / 2;
            result = result(cv::Rect(x, y, input.cols, input.rows));
        } else if (zoomFactor < 1.0) {
            cv::Mat padded = cv::Mat::zeros(input.size(), input.type());
            int x = (input.cols - result.cols) / 2;
            int y = (input.rows - result.rows) / 2;
            result.copyTo(padded(cv::Rect(x, y, result.cols, result.rows)));
            result = padded;
        }
    }
    
    return result;
}

cv::Mat FrameProcessor::applyRotate(const cv::Mat& input) {
    cv::Mat result;
    rotationAngle += 1.0;
    if (rotationAngle >= 360) rotationAngle = 0;
    
    cv::Point2f center(input.cols/2.0, input.rows/2.0);
    cv::Mat rotMat = cv::getRotationMatrix2D(center, rotationAngle, 1.0);
    cv::warpAffine(input, result, rotMat, input.size());
    return result;
}

cv::Mat FrameProcessor::applyDrawing(const cv::Mat& input, const MouseState& mouseState) {
    cv::Mat result = input.clone();
    
    if (!mouseState.drawingPoints.empty()) {
        for (size_t i = 1; i < mouseState.drawingPoints.size(); i++) {
            cv::line(result, mouseState.drawingPoints[i-1], mouseState.drawingPoints[i], 
                     cv::Scalar(0, 255, 0), 3);
        }
    }
    
    return result;
}

void FrameProcessor::drawText(cv::Mat& image) {
    int y = 30;
    int lineHeight = 25;
    
    std::string modeText;
    switch(mode) {
        case ProcessingMode::NORMAL: modeText = "NORMAL"; break;
        case ProcessingMode::INVERT: modeText = "INVERT"; break;
        case ProcessingMode::GAUSSIAN_BLUR: modeText = "GAUSSIAN BLUR"; break;
        case ProcessingMode::CANNY: modeText = "CANNY EDGE"; break;
        case ProcessingMode::SOBEL: modeText = "SOBEL"; break;
        case ProcessingMode::BINARY: modeText = "BINARY"; break;
        case ProcessingMode::GLITCH: modeText = "GLITCH"; break;
        case ProcessingMode::DRAW_MODE: modeText = "DRAW MODE"; break;
        case ProcessingMode::ZOOM_MODE: modeText = "ZOOM MODE (Mouse wheel)"; break;
        case ProcessingMode::ROTATE_MODE: modeText = "ROTATE MODE"; break;
    }
    
    cv::putText(image, "Mode: " + modeText, cv::Point(10, y), 
                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
    y += lineHeight;
    
    cv::putText(image, "FPS: " + std::to_string(static_cast<int>(fps)), 
                cv::Point(10, y), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
    y += lineHeight;
    
    cv::putText(image, "Controls: 1-Normal 2-Invert 3-Blur 4-Canny 5-Sobel 6-Binary 7-Glitch", 
                cv::Point(10, y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0), 1);
    y += 20;
    cv::putText(image, "d-Draw z-Zoom r-Rotate ESC-Exit", 
                cv::Point(10, y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0), 1);
}

void FrameProcessor::updateFPS() {
    frameCount++;
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
    
    if (elapsed >= 1000) {
        fps = frameCount * 1000.0 / elapsed;
        frameCount = 0;
        lastTime = currentTime;
    }
}

void FrameProcessor::setBrightness(int value) {
    brightness = value - 128;
}

void FrameProcessor::setBlurKernelSize(int size) {
    blurKernelSize = size;
}

void FrameProcessor::setCannyThresholds(int t1, int t2) {
    cannyThreshold1 = t1;
    cannyThreshold2 = t2;
}

void FrameProcessor::setBinaryThreshold(int thresh) {
    binaryThreshold = thresh;
}