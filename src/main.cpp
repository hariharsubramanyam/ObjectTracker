
#include <iostream>
#include <vector>

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

int main(int argc, char **argv) {
    // Create the Kalman Filter with the point starting at (0, 0) and with a 20 sample trajectory.
    std::unique_ptr<OT::KalmanHelper> KF = std::make_unique<OT::KalmanHelper>(0, 0, 20);

    cv::Mat frame;
    cv::Mat thresh_frame;
    std::vector<cv::Mat> channels;
    cv::VideoCapture capture;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<std::vector<cv::Point> > contours;
    
    std::unique_ptr<std::list<OT::TrajectorySegment>> trajectorySegments;

    cv::Mat back;
    cv::Mat fore;
    cv::Ptr<cv::BackgroundSubtractorMOG2> bg = cv::createBackgroundSubtractorMOG2();
    bg->setHistory(500);
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
        bg->getBackgroundImage(back);
        cv::medianBlur(fore, fore, 5);
        cv::dilate(fore, fore, cv::Mat());
        imshow("Threshold", fore);

        cv::normalize(fore, fore, 0, 1., cv::NORM_MINMAX);
        cv::threshold(fore, fore, .3, 1., CV_THRESH_BINARY);
        
        cv::split(frame, channels);
        cv::add(channels[0], channels[1], channels[1]);
        cv::subtract(channels[2], channels[1], channels[2]);
        cv::threshold(channels[2], thresh_frame, 50, 255, CV_THRESH_BINARY);
        cv::medianBlur(thresh_frame, thresh_frame, 5);

        cv::findContours(fore, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
        std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
        std::vector<cv::Rect> boundRect( contours.size() );

        cv::Mat drawing = cv::Mat::zeros(thresh_frame.size(), CV_8UC1);
        for(size_t i = 0; i < contours.size(); i++) {
            if(contourArea(contours[i]) > 3000) {
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
        }
        thresh_frame = drawing;
        
        // Get the moments
        std::vector<cv::Moments> mu(contours.size() );
        
        for( size_t i = 0; i < contours.size(); i++ ) {
            mu[i] = moments( contours[i], false );
        }
        
        //  Get the mass centers:
        std::vector<cv::Point2f> mc( contours.size() );
        for( size_t i = 0; i < contours.size(); i++ ) {
            mc[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
        }


        for( size_t i = 0; i < contours.size(); i++ ) {
            approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
            boundRect[i] = boundingRect( cv::Mat(contours_poly[i]) );
        }
        
        p = KF->predict();
        
        for( size_t i = 0; i < contours.size(); i++ ) {
            if(contourArea(contours[i]) > 1000) {
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
        }
        imshow("Video", frame);
    }
    return 0;
}
