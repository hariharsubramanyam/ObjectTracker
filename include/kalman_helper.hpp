#ifndef KALMAN_HELPER_HPP
#define KALMAN_HELPER_HPP

#include <memory>

#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>

namespace OT {
    class KalmanHelper {
    private:
        std::unique_ptr<cv::KalmanFilter> kf;
    public:
        KalmanHelper(float x, float y);
        cv::Point predict();
        cv::Point correct(float x, float y);
    };
}

#endif
