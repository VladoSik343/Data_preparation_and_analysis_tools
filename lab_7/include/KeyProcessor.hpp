#ifndef KEY_PROCESSOR_HPP
#define KEY_PROCESSOR_HPP

#include <opencv2/opencv.hpp>
#include <map>
#include <vector>

enum class ProcessingMode {
    NORMAL,
    INVERT,
    GAUSSIAN_BLUR,
    CANNY,
    SOBEL,
    BINARY,
    GLITCH,
    DRAW_MODE,
    ZOOM_MODE,
    ROTATE_MODE,
    FACE_DETECT_MODE 
};

enum class MouseAction {
    NONE,
    LEFT_BUTTON_DOWN,
    LEFT_BUTTON_UP,
    RIGHT_BUTTON_DOWN, 
    RIGHT_BUTTON_UP, 
    MOUSE_MOVE,
    MOUSE_WHEEL
};

struct MouseState {
    MouseAction action;
    cv::Point position;
    int wheelDelta;
    bool isDrawing;
    std::vector<cv::Point> drawingPoints;
};

class KeyProcessor {
private:
    ProcessingMode currentMode;
    std::map<int, ProcessingMode> keyMap;
    MouseState mouseState;
    
public:
    KeyProcessor();
    
    ProcessingMode processKey(int key);
    ProcessingMode getCurrentMode() const;
    
    void setMouseAction(MouseAction action, int x, int y, int delta = 0);
    MouseState getMouseState();
    void clearMouseState();
    
    static void onMouse(int event, int x, int y, int flags, void* userdata);
};

#endif
