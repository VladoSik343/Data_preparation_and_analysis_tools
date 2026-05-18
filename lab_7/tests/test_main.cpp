#include <catch2/catch_test_macros.hpp>
#include <opencv2/opencv.hpp>
#include <memory>
#include "../include/FaceDetector.hpp"
#include "../include/FrameProcessor.hpp"
#include "../include/KeyProcessor.hpp"

TEST_CASE("FaceDetector - Public Interface & State Management", "[detector]") {
    FaceDetector detector;

    detector.setFaceDetectionEnabled(true);
    REQUIRE(detector.isDetectionEnabled() == true);

    detector.setFaceDetectionEnabled(false);
    REQUIRE(detector.isDetectionEnabled() == false);

    cv::Mat empty_frame;
    REQUIRE_NOTHROW(detector.updateFrame(empty_frame));
    
    auto faces = detector.getFaces();
    REQUIRE(faces.empty() == true);
}

TEST_CASE("FrameProcessor - Processing with Different Modes", "[processor]") {
    FrameProcessor processor;
    
    cv::Mat dummy_frame = cv::Mat::zeros(100, 100, CV_8UC3);
    
    MouseState fake_mouse_state;
    fake_mouse_state.action = MouseAction::NONE;
    fake_mouse_state.position = cv::Point(0, 0);
    fake_mouse_state.wheelDelta = 0;
    fake_mouse_state.isDrawing = false;

    cv::Mat output_frame;
    REQUIRE_NOTHROW(output_frame = processor.process(dummy_frame, ProcessingMode::NORMAL, fake_mouse_state));
    
    REQUIRE_FALSE(output_frame.empty());
    REQUIRE(output_frame.cols == 100);
    REQUIRE(output_frame.rows == 100);
}

TEST_CASE("FrameProcessor - Parameter Setters Verification", "[processor]") {
    FrameProcessor processor;
    
    REQUIRE_NOTHROW(processor.setBrightness(30));
    REQUIRE_NOTHROW(processor.setBlurKernelSize(3));
    REQUIRE_NOTHROW(processor.setCannyThresholds(50, 150));
    REQUIRE_NOTHROW(processor.setBinaryThreshold(128));
}