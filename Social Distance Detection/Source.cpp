#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "person.h"

using namespace std;
using namespace cv;

const Scalar SCALAR_BLACK = Scalar(0.0, 0.0, 0.0);
const Scalar SCALAR_WHITE = Scalar(255.0, 255.0, 255.0);
const Scalar SCALAR_GREEN = Scalar(0.0, 200.0, 0.0);
const Scalar SCALAR_RED = Scalar(0.0, 0.0, 255.0);

int main(void) {
    VideoCapture capVideo;
    Mat imgFrame1;
    Mat imgFrame2;

    capVideo.open("people.avi");

    if (!capVideo.isOpened()) {
        cout << "\nerror reading video file" << endl << endl;
        return(0);
    }

    if (capVideo.get(CAP_PROP_FRAME_COUNT) < 2) {
        cout << "\nerror: video file must have at least two frames";
        return(0);
    }

    capVideo.read(imgFrame1);
    capVideo.read(imgFrame2);

    char chCheckForEscKey = 0;

    while (capVideo.isOpened() && chCheckForEscKey != 27) {

        vector<Person> persons;

        Mat imgFrame1Copy = imgFrame1.clone();
        Mat imgFrame2Copy = imgFrame2.clone();

        Mat imgDifference;
        Mat imgThresh;

        cvtColor(imgFrame1Copy, imgFrame1Copy, COLOR_BGR2GRAY);
        cvtColor(imgFrame2Copy, imgFrame2Copy, COLOR_BGR2GRAY);

        GaussianBlur(imgFrame1Copy, imgFrame1Copy, Size(5, 5), 0);
        GaussianBlur(imgFrame2Copy, imgFrame2Copy, Size(5, 5), 0);

        absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);

        threshold(imgDifference, imgThresh, 10, 255.0, ADAPTIVE_THRESH_MEAN_C);

        //imshow("imgThresh", imgThresh);

        Mat structuringElement5x5 = getStructuringElement(MORPH_RECT, Size(5, 5));
        
        dilate(imgThresh, imgThresh, structuringElement5x5);
        dilate(imgThresh, imgThresh, structuringElement5x5);
        dilate(imgThresh, imgThresh, structuringElement5x5);
        erode(imgThresh, imgThresh, structuringElement5x5);

        Mat imgThreshCopy = imgThresh.clone();

        vector<vector<Point> > contours;

        findContours(imgThreshCopy, contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

        Mat imgContours(imgThresh.size(), CV_8UC3, SCALAR_BLACK);

        drawContours(imgContours, contours, -1, SCALAR_WHITE, -1);

        //imshow("imgContours", imgContours);

        vector<vector<Point> > convexHulls(contours.size());

        for (unsigned int i = 0; i < contours.size(); i++) {
            convexHull(contours[i], convexHulls[i]);
        }

        for (auto& convexHull : convexHulls) {
            Person possibleperson(convexHull);

            if (possibleperson.boundingRect.area() < 6500 &&
                possibleperson.dblAspectRatio >= 0.2 &&
                possibleperson.dblAspectRatio <= 1.2 &&
                possibleperson.boundingRect.width > 30 &&
                possibleperson.boundingRect.height > 40 &&
                possibleperson.dblDiagonalSize > 50.0) {
                persons.push_back(possibleperson);
            }
        }

        Mat imgConvexHulls(imgThresh.size(), CV_8UC3, SCALAR_BLACK);

        convexHulls.clear();

        for (auto& person : persons) {
            convexHulls.push_back(person.contour);
        }

        drawContours(imgConvexHulls, convexHulls, -1, SCALAR_WHITE, -1);

        //imshow("imgConvexHulls", imgConvexHulls);

        imgFrame2Copy = imgFrame2.clone();
        
        for (auto& person : persons) {
            if (persons.size() >= 2) {
                std::vector<Person>personIndex;
                personIndex.push_back(person);
                for (auto& personIndex : persons) {
                if (person.centerPosition.y < imgFrame1.rows / 2){
                    if (sqrt((personIndex.centerPosition.x - person.centerPosition.x) * 
                        (personIndex.centerPosition.x - person.centerPosition.x) +
                        (personIndex.centerPosition.y - person.centerPosition.y) * 
                        (personIndex.centerPosition.y - person.centerPosition.y)) < 85
                        || person.boundingRect.area() > 3500) {
                        rectangle(imgFrame2Copy, person.boundingRect, SCALAR_RED, 2);
                        circle(imgFrame2Copy, person.centerPosition, 3, SCALAR_GREEN, -1);
                    }
                    else
                    {
                        rectangle(imgFrame2Copy, person.boundingRect, SCALAR_GREEN, 2);
                        circle(imgFrame2Copy, person.centerPosition, 3, SCALAR_GREEN, -1);
                    }
                }

                else
                {
                    if (sqrt((personIndex.centerPosition.x - person.centerPosition.x) *
                        (personIndex.centerPosition.x - person.centerPosition.x) +
                        (personIndex.centerPosition.y - person.centerPosition.y) * 
                        (personIndex.centerPosition.y - person.centerPosition.y)) < 145
                        || person.boundingRect.area() > 4000) {
                        rectangle(imgFrame2Copy, person.boundingRect, SCALAR_RED, 2);
                        circle(imgFrame2Copy, person.centerPosition, 3, SCALAR_GREEN, -1);
                    }
                    else
                    {
                        rectangle(imgFrame2Copy, person.boundingRect, SCALAR_GREEN, 2);
                        circle(imgFrame2Copy, person.centerPosition, 3, SCALAR_GREEN, -1);
                    }
                }
                }
            }
        }
        imshow("Distantare sociala", imgFrame2Copy);

        imgFrame1 = imgFrame2.clone();

        if ((capVideo.get(CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CAP_PROP_FRAME_COUNT)) {
            capVideo.read(imgFrame2);
        }
        else {
            cout << "end of video\n";
            break;
        }

        chCheckForEscKey = waitKey(1);

    }

    if (chCheckForEscKey != 27) {
        waitKey(0);
    }
    return(0);
}