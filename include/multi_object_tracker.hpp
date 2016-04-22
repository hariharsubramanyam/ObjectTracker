#ifndef multi_object_tracker_h
#define multi_object_tracker_h

#include <vector>

#include <opencv2/opencv.hpp>

#include "kalman_tracker.hpp"

namespace OT {
    class MultiObjectTracker {
    private:
        // The actual object trackers.
        std::vector<OT::KalmanTracker> kalmanTrackers;
        
        // We only care about trackers who have been alive for the
        // given lifetimeThreshold number of frames.
        long lifetimeThreshold;
        
        // We won't associate a tracker with a mass center if the
        // distance between them exceeds this given threshold.
        double distanceThreshold;
        
        // Kill a tracker if it has gone missedFramesThreshold frames
        // without receiving a measurement.
        long missedFramesThreshold;
        
        // Delta time, used to set up matrices for Kalman trackers.
        float dt;
        
        // Magnitude of acceleration noise. Used to set up Kalman trackers.
        float magnitudeOfAccelerationNoise;
    public:
        MultiObjectTracker(long lifetimeThreshold = 20,
                           double distanceThreshold = 60.0,
                           long missedFramesThreshold = 10,
                           float dt = 0.2,
                           float magnitudeOfAccelerationNoise = 0.5);
        
        // Update the object tracker with the mass centers of the observed boundings rects.
        void update(const std::vector<cv::Point2f>& massCenters,
                    std::vector<cv::Point>& outputPredictions);
    };
}

#endif /* multi_object_tracker_h */
