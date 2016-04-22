#ifndef multi_object_tracker_h
#define multi_object_tracker_h

#include <memory>
#include <vector>

#include <opencv2/opencv.hpp>

#include "kalman_tracker.hpp"

namespace OT {
    class MultiObjectTracker {
    private:
        std::unique_ptr<std::vector<OT::KalmanTracker>> kalmanTrackers;
        cv::Size frameSize;
    public:
        MultiObjectTracker(cv::Size frameSize);
        
        // Update the object tracker with the mass centers of the observed boundings rects.
        void update(const std::vector<cv::Point2f>& massCenters,
                    std::vector<cv::Point>& outputPredictions,
                    long lifetimeThreshold = -1);
    };
}

#endif /* multi_object_tracker_h */
