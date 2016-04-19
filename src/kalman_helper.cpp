#include <memory>

#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>

#include "kalman_helper.hpp"

namespace OT {
    KalmanHelper::KalmanHelper(float x, float y) {
        this->kf = std::make_unique<cv::KalmanFilter>();
        
        // Initialize filter with 4 dynamic parameters (x, y, x velocity, y
        // velocity), 2 measurement parameters (x, y), and no control parameters.
        this->kf->init(4, 2, 0);
        
        // Set the pre and post states.
        this->kf->statePre.setTo(0);
        this->kf->statePre.at<float>(0, 0) = x;
        this->kf->statePre.at<float>(1, 0) = y;
        
        this->kf->statePost.setTo(0);
        this->kf->statePost.at<float>(0, 0) = x;
        this->kf->statePost.at<float>(1, 0) = y;
        
        // Create the matrices.
        cv::setIdentity(this->kf->transitionMatrix);
        cv::setIdentity(this->kf->measurementMatrix);
        cv::setIdentity(this->kf->processNoiseCov, cv::Scalar::all(0.005));
        cv::setIdentity(this->kf->measurementNoiseCov, cv::Scalar::all(1e-1));
        cv::setIdentity(this->kf->errorCovPost, cv::Scalar::all(0.1));
    }
    
    cv::Point KalmanHelper::correct(float x, float y) {
        cv::Mat_<float> measurement = cv::Mat_<float>::zeros(2, 1);
        measurement(0) = x;
        measurement(1) = y;
        cv::Mat estimated = this->kf->correct(measurement);
        cv::Point statePt(estimated.at<float>(0), estimated.at<float>(1));
        return statePt;
    }
    
    cv::Point KalmanHelper::predict() {
        cv::Mat prediction = this->kf->predict();
        cv::Point predictedPt(prediction.at<float>(0), prediction.at<float>(1));
        this->kf->statePre.copyTo(this->kf->statePost);
        this->kf->errorCovPre.copyTo(this->kf->errorCovPost);
        return predictedPt;
    }
}
