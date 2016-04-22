
#include <iostream>
#include <vector>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

#include "draw_utils.hpp"
#include "multi_object_tracker.hpp"
#include "contour_finder.hpp"

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
 * Draw the contours in a new image and show them.
 */
void contourShow(std::string drawingName,
                 const std::vector<std::vector<cv::Point>>& contours,
                 const std::vector<cv::Rect>& boundingRect,
                 cv::Size imgSize) {
    cv::Mat drawing = cv::Mat::zeros(imgSize, CV_32FC3);
    for (size_t i = 0; i < contours.size(); i++) {
        cv::drawContours(drawing,
                         contours,
                         i,
                         cv::Scalar::all(127),
                         CV_FILLED,
                         8,
                         std::vector<cv::Vec4i>(),
                         0,
                         cv::Point());
        OT::DrawUtils::drawBoundingRect(drawing, boundingRect[i]);
    }
    cv::imshow(drawingName, drawing);
}


int main(int argc, char **argv) {
    // This does the actual tracking of the objects. We can't initialize it now because
    // it needs to know the size of the frame. So, we set it equal to nullptr and initialize
    // it after we get the first frame.
    OT::MultiObjectTracker tracker;

    // We'll use this variable to store the current frame captured from the video.
    cv::Mat frame;
    
    // This object represents the video or image sequence that we are reading from.
    cv::VideoCapture capture;
    
    // These two variables store the contours and contour hierarchy for the current frame.
    // We won't use the hierarchy, but we need it in order to be able to call the cv::findContours
    // function.
    std::vector<cv::Vec4i> hierarchy;
    std::vector<std::vector<cv::Point> > contours;
    
    // We'll use a ContourFinder to do the actual extraction of contours from the image.
    OT::ContourFinder contourFinder;

    // Read the first positional command line argument and use that as the video
    // source. If no argument has been provided, use the webcam.
    if (argc > 1) {
      capture.open(argv[1]);
    } else {
      capture.open(0);
    }
    
    // Ensure that the video has been opened correctly.
    if(!capture.isOpened()) {
        std::cerr << "Problem opening video source" << std::endl;
    }


    // Repeat while the user has not pressed "q" and while there's another frame.
    while(hasFrame(capture)) {
        // Fetch the next frame.
        capture.retrieve(frame);
        
        // Find the contours.
        contourFinder.findContours(frame, hierarchy, contours);
        
        // Find the bounding boxes for the contours and also find the center of mass for each contour.
        std::vector<cv::Point2f> mc(contours.size());
        std::vector<cv::Rect> boundRect(contours.size());
        contourFinder.getCentersAndBoundingBoxes(contours, mc, boundRect);
        
        contourShow("Contours", contours, boundRect, frame.size());
        
        // Update the predicted locations of the objects based on the observed
        // mass centers.
        std::vector<cv::Point> predictions;
        tracker.update(mc, boundRect, predictions);
        
        // Draw a cross for each predicted location.
        for (auto pred : predictions) {
            OT::DrawUtils::drawCross(frame, pred, cv::Scalar(255, 255, 255), 5);
        }
        imshow("Video", frame);
    }
    return 0;
}
