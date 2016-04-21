#ifndef KALMAN_TRACKER_HPP
#define KALMAN_TRACKER_HPP

#include <memory>
#include <list>

#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>

namespace OT {
    
    // Represents a segment in the trajectory of a moving object. The segment has a start and end point.
    struct TrajectorySegment {
        cv::Point start;
        cv::Point end;
    };
    
    class KalmanTracker {
    private:
        std::unique_ptr<cv::KalmanFilter> kf;
        
        // The number of frames that this Kalman Filter has gone without having an update.
        int numFramesWithoutUpdate;
        
        // The maximum number of points to store in the tracked object's trajectory.
        size_t maxTrajectorySize;
        
        // The trajectory of the moving object.
        std::shared_ptr<std::list<cv::Point>> trajectory;
        
        cv::Point previousPoint;
        
        // Store the latest prediction.
        cv::Point prediction;
        
        void addPointToTrajectory(cv::Point pt);
    public:
        KalmanTracker(cv::Point startPt, size_t maxTrajectorySize = 100);
        
        const int getNumFramesWithoutUpdate();
        
        // Get the trajectory of this object as a list of line segments. Put the resulting segments in the given list.
        void getTrajectorySegments(std::list<TrajectorySegment> *segments);
        
        // Indicate to this Kalman filter that it did not get an update this frame.
        void noUpdateThisFrame();
        
        // Indicate that the Kalman filter was updated this frame.
        void gotUpdate();
        
        cv::Point predict();
        cv::Point latestPrediction();
        cv::Point correct(cv::Point pt);
        cv::Point correct();
    };
}

#endif
