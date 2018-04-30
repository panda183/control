#ifndef DRIVER_H
#define DRIVER_H

#include "LaneDetector.h"
#include "ObstacleDetector.h"

class Driver
{
  public:
    Driver();
    ~Driver();
    int hug;
    void findTarget();
    void setHug(int hug);
    void inputImg(Mat color, Mat depth);
    double getSteering();
    Point target;

  private:
    LaneDetector ld;
    ObstacleDetector od;
    Mat color, depth;
    Point lastTarget;
    int signOverride = 0;
    int diff;
};

#endif
