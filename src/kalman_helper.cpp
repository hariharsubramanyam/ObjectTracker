#include <memory>

#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>

namespace OT {
  namespace KalmanHelper {
    std::unique_ptr<cv::KalmanFilter> makeKalmanFilter(float x, float y) {

      // Create the filter.
      std::unique_ptr<cv::KalmanFilter> kf =
        std::make_unique<cv::KalmanFilter>();

      // Initialize filter with 4 dynamic parameters (x, y, x velocity, y
      // velocity), 2 measurement parameters (x, y), and no control parameters.
      kf->init(4, 2, 0);

      // Set the pre and post states.
      kf->statePre.setTo(0);
      kf->statePre.at<float>(0, 0) = x;
      kf->statePre.at<float>(1, 0) = y;

      kf->statePost.setTo(0);
      kf->statePost.at<float>(0, 0) = x;
      kf->statePost.at<float>(1, 0) = y;

      cv::setIdentity(kf->transitionMatrix);
      cv::setIdentity(kf->measurementMatrix);
      cv::setIdentity(kf->processNoiseCov, cv::Scalar::all(0.005));
      cv::setIdentity(kf->measurementNoiseCov, cv::Scalar::all(1e-1));
      cv::setIdentity(kf->errorCovPost, cv::Scalar::all(0.1));

      return kf;
    }

    cv::Point predict(const std::unique_ptr<cv::KalmanFilter>& kf) {
      cv::Mat prediction = kf->predict();
      cv::Point predictedPt(prediction.at<float>(0), prediction.at<float>(1));

      kf->statePre.copyTo(kf->statePost);
      kf->errorCovPre.copyTo(kf->errorCovPost);

      return predictedPt;
    }

    cv::Point correct(const std::unique_ptr<cv::KalmanFilter>& kf, float x, float y) {
      cv::Mat_<float> measurement = cv::Mat_<float>::zeros(2, 1);
      measurement(0) = x;
      measurement(1) = y;
      cv::Mat estimated = kf->correct(measurement);
      cv::Point statePt(estimated.at<float>(0), estimated.at<float>(1));
      return statePt;
    }
  }
}
