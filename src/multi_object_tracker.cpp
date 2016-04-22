#include "multi_object_tracker.hpp"

#include <memory>
#include <vector>
#include <algorithm>

#include <opencv2/opencv.hpp>

#include "kalman_tracker.hpp"
#include "hungarian.hpp"

#define DISTANCE_THRESHOLD 60.0
#define MAX_FRAMES_WITHOUT_UPDATE 30

namespace OT {
    MultiObjectTracker::MultiObjectTracker() {
        this->kalmanTrackers = std::vector<OT::KalmanTracker>();
    }
    
    void MultiObjectTracker::update(const std::vector<cv::Point2f>& massCenters,
                                    std::vector<cv::Point>& outputPredictions,
                                    long lifetimeThreshold) {
        outputPredictions.clear();
        
        // If we haven't found any mass centers, just update all the Kalman filters and return their predictions.
        if (massCenters.empty()) {
            for (int i = 0; i < this->kalmanTrackers.size(); i++) {
                // Indicate that the tracker didn't get an update this frame.
                this->kalmanTrackers[i].noUpdateThisFrame();
                
                // Remove the tracker if it is dead.
                if (this->kalmanTrackers[i].getNumFramesWithoutUpdate() > MAX_FRAMES_WITHOUT_UPDATE) {
                    this->kalmanTrackers.erase(this->kalmanTrackers.begin() + i);
                    i--;
                }
            }
            // Update the remaining trackers.
            for (size_t i = 0; i < this->kalmanTrackers.size(); i++) {
                if (this->kalmanTrackers[i].getLifetime() > lifetimeThreshold) {
                    outputPredictions.push_back(this->kalmanTrackers[i].predict());
                }
            }
            return;
        }
        
        // If there are no Kalman trackers, make one for each detection.
        if (this->kalmanTrackers.empty()) {
            for (auto massCenter : massCenters) {
                this->kalmanTrackers.push_back(OT::KalmanTracker(massCenter));
            }
        }
        
        // Create our cost matrix.
        size_t numKalmans = this->kalmanTrackers.size();
        size_t numCenters = massCenters.size();
        
        std::vector<std::vector<double>> costMatrix(numKalmans, std::vector<double>(numCenters));
    
        std::vector<int> assignment;
        
        
        // Get the latest prediction for the Kalman filters.
        std::vector<cv::Point2f> predictions(this->kalmanTrackers.size());
        for (size_t i = 0; i < this->kalmanTrackers.size(); i++) {
            predictions[i] = this->kalmanTrackers[i].latestPrediction();
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
                this->kalmanTrackers[i].noUpdateThisFrame();
            }
        }
        
        // Remove any trackers that haven't been updated in a while.
        for (int i = 0; i < this->kalmanTrackers.size(); i++) {
            if (this->kalmanTrackers[i].getNumFramesWithoutUpdate() > MAX_FRAMES_WITHOUT_UPDATE) {
                this->kalmanTrackers.erase(this->kalmanTrackers.begin() + i);
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
            this->kalmanTrackers.push_back(OT::KalmanTracker(massCenters[centersWithoutKalman[i]]));
        }
        
        // Update the Kalman filters.
        for (size_t i = 0; i < assignment.size(); i++) {
            this->kalmanTrackers[i].predict();
            if (assignment[i] != -1) {
                this->kalmanTrackers[i].correct(massCenters[assignment[i]]);
                this->kalmanTrackers[i].gotUpdate();
            } else {
                // Otherwise update this with the previous step's measurement.
                this->kalmanTrackers[i].correct();
            }
        }
        
        // Now update the predictions.
        for (size_t i = 0; i < this->kalmanTrackers.size(); i++) {
            if (this->kalmanTrackers[i].getLifetime() > lifetimeThreshold) {
                outputPredictions.push_back(this->kalmanTrackers[i].latestPrediction());
            }
        }
    }
}