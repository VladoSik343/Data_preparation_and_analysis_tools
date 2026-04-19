#include "KeyProcessor.hpp"
KeyProcessor::KeyProcessor() : currentMode(ProcessingMode::NORMAL) {
    keyMap['1'] = ProcessingMode::NORMAL;
    keyMap['2'] = ProcessingMode::INVERT;
    keyMap['3'] = ProcessingMode::GAUSSIAN_BLUR;
    keyMap['4'] = ProcessingMode::CANNY;
    keyMap['5'] = ProcessingMode::SOBEL;
    keyMap['6'] = ProcessingMode::BINARY;
    keyMap['7'] = ProcessingMode::GLITCH;
    keyMap['d'] = ProcessingMode::DRAW_MODE;
    keyMap['z'] = ProcessingMode::ZOOM_MODE;
    keyMap['r'] = ProcessingMode::ROTATE_MODE;
    
    mouseState.action = MouseAction::NONE;
    mouseState.isDrawing = false;
    mouseState.wheelDelta = 0;
}

ProcessingMode KeyProcessor::processKey(int key) {
    if (key == 27) { 
        return currentMode; 
    }
    
    auto it = keyMap.find(key);
    if (it != keyMap.end()) {
        currentMode = it->second;
        std::cout << "Mode changed to: " << static_cast<int>(currentMode) << std::endl;
    }
    return currentMode;
}

ProcessingMode KeyProcessor::getCurrentMode() const {
    return currentMode;
}

void KeyProcessor::setMouseAction(MouseAction action, int x, int y, int delta) {
    mouseState.action = action;
    mouseState.position = cv::Point(x, y);
    mouseState.wheelDelta = delta;
    if (action == MouseAction::LEFT_BUTTON_DOWN) {
        mouseState.isDrawing = true;
        mouseState.drawingPoints.clear();
    } else if (action == MouseAction::LEFT_BUTTON_UP) {
        mouseState.isDrawing = false;
    }
    if (action == MouseAction::MOUSE_MOVE && mouseState.isDrawing) {
        mouseState.drawingPoints.push_back(cv::Point(x, y));
    }
}

MouseState KeyProcessor::getMouseState() {
    MouseState state = mouseState;
    mouseState.action = MouseAction::NONE;
    mouseState.wheelDelta = 0;
    return state;
}

void KeyProcessor::clearMouseState() {
    mouseState.action = MouseAction::NONE;
    mouseState.wheelDelta = 0;
    mouseState.drawingPoints.clear();
}

void KeyProcessor::onMouse(int event, int x, int y, int flags, void* userdata) {
    KeyProcessor* processor = static_cast<KeyProcessor*>(userdata);
    switch(event) {
        case cv::EVENT_LBUTTONDOWN:
            processor->setMouseAction(MouseAction::LEFT_BUTTON_DOWN, x, y);
            break;
        case cv::EVENT_LBUTTONUP:
            processor->setMouseAction(MouseAction::LEFT_BUTTON_UP, x, y);
            break;
        case cv::EVENT_RBUTTONDOWN:
            processor->setMouseAction(MouseAction::RIGHT_BUTTON_DOWN, x, y);
            break;
        case cv::EVENT_MOUSEMOVE:
            processor->setMouseAction(MouseAction::MOUSE_MOVE, x, y);
            break;
        case cv::EVENT_MOUSEWHEEL:
            processor->setMouseAction(MouseAction::MOUSE_WHEEL, x, y, cv::getMouseWheelDelta(flags));
            break;
    }
}