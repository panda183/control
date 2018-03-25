#ifndef LANE_DETECTOR_H
#define LANE_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <vector>
using namespace std;
using namespace cv;

enum side
{
    l = -1, r = 1
};

class LaneDetector
{
public:
    LaneDetector();
    ~LaneDetector();
    void inputImg(Mat img);
    void findLane();
    Point findLanePoint(side hug, Point start);
private:
    Mat img, src;
};

#endif
