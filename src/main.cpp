
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

#include "draw_utils.hpp"
#include "kalman_tracker.hpp"
#include "multi_object_tracker.hpp"
#include "contour_finder.hpp"
#include "tracker_log.hpp"


// The mouse callback to allow the user to draw a rectangle on the screen.
bool isDragging = false;
bool hasRectangle = false;
bool triggerCallback = false;
cv::Point point1, point2;

void mouseHandler(int event, int x, int y, int flags, void* param) {
    if (event == CV_EVENT_LBUTTONDOWN && !isDragging) {
        point1 = cv::Point(x, y);
        isDragging = true;
    } else if (event == CV_EVENT_MOUSEMOVE && isDragging) {
        point2 = cv::Point(x, y);
        hasRectangle = true;
    } else if (event == CV_EVENT_LBUTTONUP && isDragging) {
        point2 = cv::Point(x, y);
        isDragging = false;
        triggerCallback = true;
        hasRectangle = false;
    }
}


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
    std::unique_ptr<OT::MultiObjectTracker> tracker = nullptr;

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
    
    // We'll count the frame with this variable.
    long frameNumber = 0;
    
    // This will log all of the tracked objects.
    OT::TrackerLog trackerLog(true);

    // Read the first positional command line argument and use that as the video
    // source. If no argument has been provided, use the webcam.
    if (argc > 1) {
      capture.open(argv[1]);
    } else {
      capture.open(0);
    }
    
    // Read the second positional command line argument and use that as the log
    // for the output file.
    bool hasOutputFile = false;
    std::ofstream outputFile;
    if (argc > 2) {
        hasOutputFile = true;
        outputFile.open(argv[2]);
    }
    
    // Ensure that the video has been opened correctly.
    if(!capture.isOpened()) {
        std::cerr << "Problem opening video source" << std::endl;
    }
    
    // Set the mouse callback.
    cv::namedWindow("Video");
    cv::setMouseCallback("Video", mouseHandler);


    // Repeat while the user has not pressed "q" and while there's another frame.
    while(hasFrame(capture)) {
        // Fetch the next frame.
        capture.retrieve(frame);
        frameNumber++;
        
        // Resize the frame to reduce the time required for computation.
        cv::resize(frame, frame, cv::Size(300, 300));
        
        // Create the tracker if it isn't created yet.
        if (tracker == nullptr) {
            tracker = std::make_unique<OT::MultiObjectTracker>(cv::Size(frame.rows, frame.cols));
        }
        
        // Find the contours.
        std::vector<cv::Point2f> mc(contours.size());
        std::vector<cv::Rect> boundRect(contours.size());
        contourFinder.findContours(frame, hierarchy, contours, mc, boundRect);
        
        contourShow("Contours", contours, boundRect, frame.size());
        
        // Update the predicted locations of the objects based on the observed
        // mass centers.
        std::vector<OT::TrackingOutput> predictions;
        tracker->update(mc, boundRect, predictions);
        
        for (auto pred : predictions) {
            // Draw a cross at the location of the prediction.
            OT::DrawUtils::drawCross(frame, pred.location, pred.color, 5);
            
            // Draw the trajectory for the prediction.
            OT::DrawUtils::drawTrajectory(frame, pred.trajectory, pred.color);
            
            // Update the tracker log.
            if (hasOutputFile) {
                trackerLog.addTrack(pred.id, pred.location.x, pred.location.y, frameNumber);
            }
        }
        
        // Handle mouse callbacks.
        if (hasRectangle || triggerCallback) {
            cv::rectangle(frame, point1, point2, cv::Scalar::all(255));
        }
        
        if (triggerCallback) {
            triggerCallback = false;
            contourFinder.suppressRectangle(cv::Rect(point1, point2));
        }
        
        imshow("Video", frame);
    }
    
    
    // Log the output file if we need to.
    if (hasOutputFile) {
        trackerLog.logToFile(outputFile);
        outputFile.close();
    }
    
    return 0;
}
