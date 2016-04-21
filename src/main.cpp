
#include <iostream>
#include <vector>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

#include "draw_utils.hpp"
#include "kalman_helper.hpp"
#include "multi_object_tracker.hpp"

/**
 * Check if there's another frame in the video capture. We do this by first checking if the user has quit (i.e. pressed
 * the "Q" key) and then trying to retrieve the next frame of the video.
 */
bool hasFrame(cv::VideoCapture& capture) {
    bool hasNotQuit = ((char) cv::waitKey(1)) != 'q';
    bool hasAnotherFrame = capture.grab();
    return hasNotQuit && hasAnotherFrame;
}

/**
 * Extract the center of mass for the given contours.
 */
void getCentersAndBoundingBoxes(std::vector<std::vector<cv::Point>>& contours,
                                std::vector<cv::Point2f>& massCenters,
                                std::vector<cv::Rect>& boundingBoxes) {
    // Empty the vectors.
    massCenters.clear();
    boundingBoxes.clear();
    
    // Create local variables.
    cv::Moments contourMoments;
    std::vector<std::vector<cv::Point> > contourPolygons(contours.size());
    
    // Iterate through every contour.
    for (size_t i = 0; i < contours.size(); i++) {
        // Compute the center of mass.
        contourMoments = cv::moments(contours[i], false);
        massCenters.push_back(cv::Point2f(contourMoments.m10/contourMoments.m00, contourMoments.m01/contourMoments.m00));
        
        // Compute the polygon represented by the contour, and then compute the bounding box around that polygon.
        cv::approxPolyDP(cv::Mat(contours[i]), contourPolygons[i], 3, true);
        boundingBoxes.push_back(cv::boundingRect(cv::Mat(contourPolygons[i])));
    }
}

/**
 * Remove contours if they are are too small.
 */
void filterOutBadContours(std::vector<std::vector<cv::Point>>& contours) {
    const size_t CONTOUR_SIZE_THRESHOLD = 1000;
    auto removeThese = std::remove_if(contours.begin(), contours.end(), [](std::vector<cv::Point> contour) {
        return cv::contourArea(contour) <= CONTOUR_SIZE_THRESHOLD;
    });
    contours.erase(removeThese, contours.end());
}

/**
 * Draw the contours in a new image and show them.
 */
void contourShow(std::string drawingName, const std::vector<std::vector<cv::Point>>& contours, cv::Size imgSize) {
    cv::Mat drawing = cv::Mat::zeros(imgSize, CV_8UC1);
    for (size_t i = 0; i < contours.size(); i++) {
        cv::drawContours(drawing,
                         contours,
                         i,
                         cv::Scalar::all(255),
                         CV_FILLED,
                         8,
                         std::vector<cv::Vec4i>(),
                         0,
                         cv::Point());
    }
    cv::imshow(drawingName, drawing);
}


int main(int argc, char **argv) {
    std::unique_ptr<OT::MultiObjectTracker> tracker = nullptr;

    cv::Mat frame;
    cv::VideoCapture capture;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<std::vector<cv::Point> > contours;
    
    std::list<OT::TrajectorySegment> trajectorySegments;
    cv::Mat fore;
    cv::Ptr<cv::BackgroundSubtractorMOG2> bg = cv::createBackgroundSubtractorMOG2();
    bg->setHistory(1000);
    bg->setNMixtures(3);
    bg->setDetectShadows(false);
    
    cv::Point s;
    cv::Point p;

    if (argc > 1) {
      capture.open(argv[1]);
    } else {
      capture.open(0);
    }
    
    if(!capture.isOpened()) {
        std::cerr << "Problem opening video source" << std::endl;
    }


    // Repeat while the user has not pressed "q" and while there's another frame.
    while(hasFrame(capture)) {
        capture.retrieve(frame);
        if (tracker == nullptr) {
            tracker = std::make_unique<OT::MultiObjectTracker>(cv::Size(frame.rows, frame.cols));
        }
        bg->apply(frame, fore);
        
        // Get rid little specks of noise by doing a median blur.
        // The median blur is good for salt-and-pepper noise, not Gaussian noise.
        cv::medianBlur(fore, fore, 5);
        
        // Dilate the image to make the blobs larger.
        cv::dilate(fore, fore, cv::Mat());
        
        // Display the forground.
        cv::imshow("Foreground", fore);

        // Find the contours.
        cv::findContours(fore, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
        
        // Keep only those contours that are sufficiently large.
        filterOutBadContours(contours);
        contourShow("Contours", contours, fore.size());
        
        
        std::vector<cv::Point2f> mc(contours.size());
        std::vector<cv::Rect> boundRect(contours.size());
        getCentersAndBoundingBoxes(contours, mc, boundRect);
        
        std::vector<cv::Point> predictions;
        tracker->update(mc, predictions);
        
        for (auto pred : predictions) {
            OT::DrawUtils::drawCross(frame, pred, cv::Scalar(255, 255, 255), 5);
        }
        
//        for( size_t i = 0; i < contours.size(); i++ ) {
//            cv::Point center = OT::DrawUtils::drawBoundingRect(frame, boundRect[i]);
//            OT::DrawUtils::drawCross(frame, s, cv::Scalar(255, 255, 255), 5);
//                
//            // Draw the trajectory.
//            KF->getTrajectorySegments(&trajectorySegments);
//            for (OT::TrajectorySegment segment : trajectorySegments) {
//                line(frame, segment.start, segment.end, cv::Scalar(0, 255, 0), 1);
//            }
//        }
        imshow("Video", frame);
    }
    return 0;
}
