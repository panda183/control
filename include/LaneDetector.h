#ifndef LANE_DETECTOR_H
#define LANE_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <vector>
using namespace std;
using namespace cv;

#define NO_SIGN 0
#define LEFT 1
#define RIGHT 2
#define STOP 3

class LaneDetector
{
public:
    LaneDetector();
    ~LaneDetector();
    void inputImg(Mat img);
    void findLane();
    Point findLanePoint(int hug, Point start);
private:
    Mat img, src;
};

#endif
