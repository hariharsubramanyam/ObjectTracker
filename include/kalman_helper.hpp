#ifndef KALMAN_HELPER_HPP
#define KALMAN_HELPER_HPP

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
    
    class KalmanHelper {
    private:
        std::unique_ptr<cv::KalmanFilter> kf;
        
        // The number of frames that this Kalman Filter has gone without having an update.
        int numFramesWithoutUpdate;
        
        // The maximum number of points to store in the tracked object's trajectory.
        size_t maxTrajectorySize;
        
        // The trajectory of the moving object.
        std::shared_ptr<std::list<cv::Point>> trajectory;
        
        cv::Point previousPoint;
        
        void addPointToTrajectory(cv::Point pt);
    public:
        KalmanHelper(cv::Point startPt, size_t maxTrajectorySize = 100);
        
        const int getNumFramesWithoutUpdate();
        
        // Get the trajectory of this object as a list of line segments. Put the resulting segments in the given list.
        void getTrajectorySegments(std::list<TrajectorySegment> *segments);
        
        // Indicate to this Kalman filter that it did not get an update this frame.
        void noUpdateThisFrame();
        
        cv::Point predict();
        cv::Point correct(float x, float y);
        cv::Point correct();
    };
}

#endif
