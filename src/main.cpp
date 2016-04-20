
#include <iostream>
#include <vector>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

#include "draw_utils.hpp"
#include "kalman_helper.hpp"

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
    // Create the Kalman Filter with the point starting at (0, 0) and with a 20 sample trajectory.
    std::unique_ptr<OT::KalmanHelper> KF = std::make_unique<OT::KalmanHelper>(0, 0, 20);
    
    const size_t CONTOUR_SIZE_THRESHOLD = 500;

    cv::Mat frame;
    cv::VideoCapture capture;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<std::vector<cv::Point> > contoursBuffer;
    
    std::unique_ptr<std::list<OT::TrajectorySegment>> trajectorySegments;
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
        bg->apply(frame, fore);
        
        // Get rid little specks of noise by doing a median blur.
        // The median blur is good for salt-and-pepper noise, not Gaussian noise.
        cv::medianBlur(fore, fore, 5);
        
        // Dilate the image to make the blobs larger.
        cv::dilate(fore, fore, cv::Mat());
        
        cv::imshow("Threshold", fore);

        cv::findContours(fore, contoursBuffer, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
        contours.clear();
        
        for (auto contour : contoursBuffer) {
            if (cv::contourArea(contour) > CONTOUR_SIZE_THRESHOLD) {
                contours.push_back(contour);
            }
        }
        
        
        contourShow("Contours", contours, fore.size());
        
        std::vector<cv::Point2f> mc(contours.size());
        std::vector<cv::Rect> boundRect(contours.size());
        getCentersAndBoundingBoxes(contours, mc, boundRect);
        
        p = KF->predict();
        
        for( size_t i = 0; i < contours.size(); i++ ) {
            rectangle( frame, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(0, 255, 0), 2, 8, 0 );
            cv::Point center = cv::Point(boundRect[i].x + (boundRect[i].width /2),
                                         boundRect[i].y + (boundRect[i].height/2));
            cv::circle(frame,center, 8, cv::Scalar(0, 0, 255), -1, 1,0);
            s = KF->correct(center.x, center.y);
            OT::DrawUtils::drawCross(frame, s, cv::Scalar(255, 255, 255), 5);
                
                // Draw the trajectory.
            trajectorySegments = KF->getTrajectorySegments();
            for (OT::TrajectorySegment segment : *trajectorySegments) {
                line(frame, segment.start, segment.end, cv::Scalar(0, 255, 0), 1);
            }
        }
        imshow("Video", frame);
    }
    return 0;
}
