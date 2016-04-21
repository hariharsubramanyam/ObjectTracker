#ifndef multi_object_tracker_h
#define multi_object_tracker_h

#include <memory>
#include <vector>

#include <opencv2/opencv.hpp>

#include "kalman_helper.hpp"

namespace OT {
    class MultiObjectTracker {
    private:
        std::unique_ptr<std::vector<OT::KalmanHelper>> kalmanTrackers;
        cv::Size frameSize;
    public:
        MultiObjectTracker(cv::Size frameSize);
        
        // Update the object tracker with the mass centers of the observed boundings rects.
        void update(const std::vector<cv::Point2f>& massCenters);
    };
}

#endif /* multi_object_tracker_h */
