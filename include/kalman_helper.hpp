#ifndef KALMAN_HELPER_HPP
#define KALMAN_HELPER_HPP

#include <memory>

#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>

namespace OT {
  namespace KalmanHelper {
    std::unique_ptr<cv::KalmanFilter> makeKalmanFilter(float x, float y);
    cv::Point predict(const std::unique_ptr<cv::KalmanFilter>& kf);
    cv::Point correct(const std::unique_ptr<cv::KalmanFilter>& kf, float x, float y);
  }
}

#endif
