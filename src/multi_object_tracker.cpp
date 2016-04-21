#include "multi_object_tracker.hpp"

#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>

#include <opencv2/opencv.hpp>

#include "kalman_helper.hpp"
#include "hungarian.hpp"

#define DISTANCE_THRESHOLD 60.0

namespace OT {
    MultiObjectTracker::MultiObjectTracker(cv::Size frameSize) {
        this->frameSize = frameSize;
        this->kalmanTrackers = std::make_unique<std::vector<OT::KalmanHelper>>();
    }
    
    void MultiObjectTracker::update(const std::vector<cv::Point2f>& massCenters) {
        // If there are no Kalman trackers, make one for each detection.
        if (this->kalmanTrackers->empty()) {
            for (auto massCenter : massCenters) {
                this->kalmanTrackers->push_back(OT::KalmanHelper(massCenter.x, massCenter.y));
            }
        }
        
        // Create our cost matrix.
        size_t numKalmans = this->kalmanTrackers->size();
        size_t numCenters = massCenters.size();
        
        std::vector<std::vector<double>> costMatrix(numKalmans, std::vector<double>(numCenters));
        std::vector<int> assignment;
        
        
        // Run the predict step for the Kalman filters.
        std::vector<cv::Point2f> predictions(this->kalmanTrackers->size());
        for (size_t i = 0; i < this->kalmanTrackers->size(); i++) {
            predictions[i] = (*this->kalmanTrackers)[i].predict();
        }
        
        // We need to associate each of the mass centers to their corresponding Kalman filter. First,
        // let's find the pairwise distances.
        for (size_t i = 0; i < predictions.size(); i++) {
            for (size_t j = 0; j < massCenters.size(); j++) {
                costMatrix[i][j] = cv::norm(predictions[i] - massCenters[j]);
            }
        }
        
        // Assign Kalman trackers to mass centers with the Hungarian algorithm.
        AssignmentProblemSolver solver;
        solver.Solve(costMatrix, assignment, AssignmentProblemSolver::optimal);
        
        // Unassign any Kalman trackers whose distance to their assignment is too large.
        std::vector<int> kalmansWithoutCenters;
        for (size_t i = 0; i < assignment.size(); i++) {
            if (assignment[i] != -1) {
                if (costMatrix[i][assignment[i]] > DISTANCE_THRESHOLD) {
                    assignment[i] = -1;
                    kalmansWithoutCenters.push_back(i);
                }
            } else {
                (*this->kalmanTrackers)[i].noUpdateThisFrame();
            }
        }
        
        // Remove any trackers that haven't been updated in a while.
    }
}