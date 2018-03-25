#ifndef DRIVER_H
#define DRIVER_H

#include "../LaneDetector/LaneDetector.h"

class Driver
{
public:
    Driver();
    ~Driver();
    side hug;
    Point getTarget();
    void setHug(side hug);
    void inputImg(Mat img);
private:
    LaneDetector ld;
    Mat img;
    Point lastTarget;
};

#endif
