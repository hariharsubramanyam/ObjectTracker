#include "multi_object_tracker.hpp"

#include <memory>
#include <vector>

#include <opencv2/opencv.hpp>

#include "kalman_helper.hpp"

namespace OT {
    MultiObjectTracker::MultiObjectTracker(cv::Size frameSize) {
        this->frameSize = frameSize;
        this->kalmanTrackers = std::make_unique<std::vector<OT::KalmanHelper>>();
    }
}