#include "multi_object_tracker.hpp"

#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>

#include <opencv2/opencv.hpp>

#include "kalman_helper.hpp"
#include "hungarian.hpp"

#define DISTANCE_THRESHOLD 60.0
#define MAX_FRAMES_WITHOUT_UPDATE 10

namespace OT {
    MultiObjectTracker::MultiObjectTracker(cv::Size frameSize) {
        this->frameSize = frameSize;
        this->kalmanTrackers = std::make_unique<std::vector<OT::KalmanHelper>>();
    }
    
    void MultiObjectTracker::update(const std::vector<cv::Point2f>& massCenters) {
        // If there are no Kalman trackers, make one for each detection.
        if (this->kalmanTrackers->empty()) {
            for (auto massCenter : massCenters) {
                this->kalmanTrackers->push_back(OT::KalmanHelper(massCenter));
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
                this->kalmanTrackers->at(i).noUpdateThisFrame();
            }
        }
        
        // Remove any trackers that haven't been updated in a while.
        for (size_t i = 0; i < this->kalmanTrackers->size(); i++) {
            if (this->kalmanTrackers->at(i).getNumFramesWithoutUpdate() > MAX_FRAMES_WITHOUT_UPDATE) {
                this->kalmanTrackers->erase(this->kalmanTrackers->begin() + i);
                assignment.erase(assignment.begin() + i);
                i--;
            }
        }
        
        // Find unassigned mass centers.
        std::vector<int> centersWithoutKalman;
        std::vector<int>::iterator it;
        for (size_t i = 0; i < massCenters.size(); i++) {
            it = std::find(assignment.begin(), assignment.end(), i);
            if (it == assignment.end()) {
                centersWithoutKalman.push_back(i);
            }
        }
        
        // Create new trackers for the unassigned mass centers.
        for (size_t i = 0; i < centersWithoutKalman.size(); i++) {
            this->kalmanTrackers->push_back(OT::KalmanHelper(massCenters[centersWithoutKalman[i]]));
        }
        
        // Update the Kalman filters.
        for (size_t i = 0; i < assignment.size(); i++) {
            this->kalmanTrackers->at(i).predict();
            if (assignment[i] != -1) {
                this->kalmanTrackers->at(i).correct(massCenters[assignment[i]].x, massCenters[assignment[i]].y);
            } else {
                // Otherwise update this with the previous step's measurement.
                this->kalmanTrackers->at(i).correct();
            }
        }
    }
}