#ifndef DRIVER_H
#define DRIVER_H

#include "../LaneDetector/LaneDetector.h"

class Driver
{
public:
    Driver();
    ~Driver();
    int hug;
    void findTarget();
    void setHug(int hug);
    void inputImg(Mat img);
    double getSteering();
    Point target;
private:
    LaneDetector ld;
    Mat img;
    Point lastTarget;
    int signOverride = 0;
    int diff;
};

#endif
