#ifndef MY_PERSON
#define MY_PERSON

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
class Person {
public:
    // member variables ///////////////////////////////////////////////////////////////////////////
    std::vector<cv::Point> contour;

    cv::Rect boundingRect;

    cv::Point centerPosition;

    double dblDiagonalSize;

    double dblAspectRatio;

    // function prototypes ////////////////////////////////////////////////////////////////////////
    Person(std::vector<cv::Point> _contour);

};

#endif    // MY_BLOB