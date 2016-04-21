#include "multi_object_tracker.hpp"

#include <memory>
#include <vector>
#include <algorithm>

#include <opencv2/opencv.hpp>

#include "kalman_helper.hpp"

namespace OT {
    MultiObjectTracker::MultiObjectTracker(cv::Size frameSize) {
        this->frameSize = frameSize;
        this->kalmanTrackers = std::make_unique<std::vector<OT::KalmanHelper>>();
    }
    
    struct PairwiseDistance {
        size_t massCenterIndex;
        size_t predictionIndex;
        double distance;
    };
    
    void MultiObjectTracker::update(const std::vector<cv::Point2f>& massCenters) {
        // Run the predict step for the Kalman filters.
        std::vector<cv::Point2f> predictions(this->kalmanTrackers->size());
        for (size_t i = 0; i < this->kalmanTrackers->size(); i++) {
            predictions[i] = (*this->kalmanTrackers)[i].predict();
        }
        
        // We need to associate each of the mass centers to their corresponding Kalman filter. First,
        // let's find the pairwise distances.
        std::vector<PairwiseDistance> pairwiseDistances;
        for (size_t i = 0; i < massCenters.size(); i++) {
            for (size_t j = 0; j < predictions.size(); j++) {
                pairwiseDistances.push_back(PairwiseDistance{i, j, cv::norm(massCenters[i] - predictions[j])});
            }
        }
        
        // TODO: Feed this into the Hungarian Algorithm.
    }
}