#ifndef KALMAN_TRACKER_HPP
#define KALMAN_TRACKER_HPP

#include <memory>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>

namespace OT {
    
    struct TrackingOutput {
        int id;
        cv::Point location;
        cv::Scalar color;
        std::vector<cv::Point> trajectory;
    };
    
    class KalmanTracker {
    private:
        std::unique_ptr<cv::KalmanFilter> kf;
        
        // The number of frames that this Kalman Filter has gone without having an update.
        int numFramesWithoutUpdate;
        
        // The maximum number of points to store in the tracked object's trajectory.
        size_t maxTrajectorySize;
        
        // The trajectory of the moving object.
        std::shared_ptr<std::vector<cv::Point>> trajectory;
        
        // Store the latest prediction.
        cv::Point prediction;
        
        // The number of frames that this Kalman tracker has been alive.
        long lifetime;
        
        // The unique identifier for this tracker.
        int id;
        
        // The unique color associated with this Kalman tracker.
        cv::Scalar color;
        
        // The latest hue histogram for this image.
        cv::Mat histogram;
        
        void addPointToTrajectory(cv::Point pt);
    public:
        KalmanTracker(cv::Point startPt,
                      float dt = 0.2,
                      float magnitudeOfAccelerationNoise = 0.5,
                      size_t maxTrajectorySize = 20);
        
        const int getNumFramesWithoutUpdate();
        
        // Indicate to this Kalman filter that it did not get an update this frame.
        void noUpdateThisFrame();
        
        // Indicate that the Kalman filter was updated this frame.
        void gotUpdate();
        
        // Return the number of frames that this Kalman tracker has been alive.
        long getLifetime();
        
        // Update the histogram for this tracker.
        void updateWithBox(cv::Rect boundingRect,
                           const cv::Mat &frame);
        
        cv::Point predict();
        cv::Point latestPrediction();
        cv::Point correct(cv::Point pt);
        OT::TrackingOutput latestTrackingOutput();
    };
}

#endif
