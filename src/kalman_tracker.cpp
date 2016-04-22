#include <memory>

#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>

#include "kalman_tracker.hpp"

namespace OT {
    KalmanTracker::KalmanTracker(cv::Point startPt,
                                 float dt,
                                 float magnitudeOfAccelerationNoise,
                                 size_t maxTrajectorySize) {
        this->maxTrajectorySize = maxTrajectorySize;
        this->kf = std::make_unique<cv::KalmanFilter>();
        this->trajectory = std::make_shared<std::list<cv::Point>>();
        this->numFramesWithoutUpdate = 0;
        this->previousPoint = startPt;
        this->prediction = startPt;
        this->lifetime = 0;
        
        // Initialize filter with 4 dynamic parameters (x, y, x velocity, y
        // velocity), 2 measurement parameters (x, y), and no control parameters.
        this->kf->init(4, 2, 0);
        
        // Set the pre and post states.
        this->kf->statePre.setTo(0);
        this->kf->statePre.at<float>(0, 0) = startPt.x;
        this->kf->statePre.at<float>(1, 0) = startPt.y;
        
        this->kf->statePost.setTo(0);
        this->kf->statePost.at<float>(0, 0) = startPt.x;
        this->kf->statePost.at<float>(1, 0) = startPt.y;
        
        // Create the matrices.
        this->kf->transitionMatrix = (cv::Mat_<float>(4, 4) << 1,0,dt,0,   0,1,0,dt,  0,0,1,0,  0,0,0,1);

        cv::setIdentity(this->kf->measurementMatrix);
        this->kf->processNoiseCov = (cv::Mat_<float>(4, 4) <<
                                 pow(dt,4.0)/4.0, 0, pow(dt,3.0)/2.0, 0,
                                 0, pow(dt,4.0)/4.0 , 0 ,pow(dt,3.0)/2.0,
                                 pow(dt,3.0)/2.0, 0, pow(dt,2.0), 0,
                                 0, pow(dt,3.0)/2.0, 0, pow(dt,2.0));
        this->kf->processNoiseCov *= magnitudeOfAccelerationNoise;
        
        cv::setIdentity(this->kf->measurementNoiseCov, cv::Scalar::all(0.1));
        cv::setIdentity(this->kf->errorCovPost, cv::Scalar::all(0.1));
    }
    
    cv::Point KalmanTracker::correct(cv::Point pt) {
        cv::Mat_<float> measurement = cv::Mat_<float>::zeros(2, 1);
        measurement(0) = pt.x;
        measurement(1) = pt.y;
        this->previousPoint = pt;
        cv::Mat estimated = this->kf->correct(measurement);
        cv::Point statePt(estimated.at<float>(0), estimated.at<float>(1));
        this->prediction.x = statePt.x;
        this->prediction.y = statePt.y;
        return statePt;
    }
    
    cv::Point KalmanTracker::correct() {
        return this->correct(this->previousPoint);
    }
    
    cv::Point KalmanTracker::predict() {
        cv::Mat prediction = this->kf->predict();
        cv::Point predictedPt(prediction.at<float>(0), prediction.at<float>(1));
        this->kf->statePre.copyTo(this->kf->statePost);
        this->kf->errorCovPre.copyTo(this->kf->errorCovPost);
        this->addPointToTrajectory(predictedPt);
        this->prediction = predictedPt;
        return predictedPt;
    }
    
    cv::Point KalmanTracker::latestPrediction() {
        return this->prediction;
    }
    
    void KalmanTracker::addPointToTrajectory(cv::Point pt) {
        if (this->trajectory->size() >= this->maxTrajectorySize) {
            this->trajectory->pop_front();
        }
        this->trajectory->push_back(pt);
    }
    
    void KalmanTracker::getTrajectorySegments(std::list<TrajectorySegment> *segments) {
        segments->clear();
        cv::Point prevPt;
        bool hasPrevPt = false;
        for (auto pt : *this->trajectory) {
            if (hasPrevPt) {
                segments->push_back(TrajectorySegment{prevPt, pt});
            } else {
                hasPrevPt = true;
            }
            prevPt = pt;
        }
    }
    
    long KalmanTracker::getLifetime() {
        return this->lifetime;
    }
    
    void KalmanTracker::noUpdateThisFrame() {
        this->lifetime++;
        this->numFramesWithoutUpdate++;
    }
    
    const int KalmanTracker::getNumFramesWithoutUpdate() {
        return this->numFramesWithoutUpdate;
    }
    
    void KalmanTracker::gotUpdate() {
        this->lifetime++;
        this->numFramesWithoutUpdate = 0;
    }
}
