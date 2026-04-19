#include "CameraProvider.hpp"
CameraProvider::CameraProvider(int id) : cameraId(id), isOpened(false) {
    cap.open(cameraId);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera " << cameraId << std::endl;
        isOpened = false;
    } else {
        isOpened = true;
        setResolution(640, 480);
        std::cout << "Camera opened successfully!" << std::endl;
    }
}

CameraProvider::~CameraProvider() {
    if (cap.isOpened()) {
        cap.release();
    }
}

cv::Mat CameraProvider::getFrame() {
    cv::Mat frame;
    if (cap.isOpened()) {
        cap >> frame;
    }
    return frame;
}

bool CameraProvider::isCameraOpened() const {
    return isOpened;
}

double CameraProvider::getFPS() const {
    if (cap.isOpened()) {
        return cap.get(cv::CAP_PROP_FPS);
    }
    return 0.0;
}

void CameraProvider::setResolution(int width, int height) {
    if (cap.isOpened()) {
        cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    }
}