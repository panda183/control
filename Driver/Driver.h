#ifndef DRIVER_H
#define DRIVER_H

#include "../LaneDetector/LaneDetector.h"

class Driver
{
public:
    Driver();
    ~Driver();
    int hug;
    Point getTarget();
    void setHug(int hug);
    void inputImg(Mat img);
private:
    LaneDetector ld;
    Mat img;
    Point lastTarget;
    int signOverride = 0;
};

#endif
