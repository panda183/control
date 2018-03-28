#include "Driver.h"
#include "../SignDetector/SignDetector.h"
#include <iostream>
#define PI 3.14159

Driver::Driver() {}

Driver::~Driver() {}

double computeAngle(Point A, Point O, Point B)
{
    if ((A == O) || (B == O)) return 90;
    Point vOA = A - O;
    Point vOB = B - O;
    double dOA = sqrt(vOA.x * vOA.x + vOA.y * vOA.y);
    double dOB = sqrt(vOB.x * vOB.x + vOB.y * vOB.y);
    double res = acos(vOA.dot(vOB) / (dOA * dOB));
    res = res / PI * 180;
    return res;
}

void Driver::setHug(int hug)
{
    this->hug = hug;
}

void Driver::inputImg(Mat img)
{
    this->ld.inputImg(img);
    this->img = img;
    sd::DetectSign(img);
    if (sd::signDetected)
    {
        hug = sd::turn;
        this->signOverride = 15;
    }
    ld.findLane();
    findTarget();
}

void Driver::findTarget()
{
    diff = 0;
    int targetRow = img.rows * 0.5;
    if (this->lastTarget == Point(0, 0))
        lastTarget = Point(img.cols / 2, targetRow);

    target = ld.findLanePoint(hug, this->lastTarget);
    if (target == Point(0, 0))
    {
        if (signOverride)
        {
            target = Point(img.cols / 2 + hug * 10, targetRow);
            return;
        }
        hug = -hug;
        target = ld.findLanePoint(hug, this->lastTarget);
    }
    else
    {
        int adjust = img.cols / 3.5;
        target.x -= hug * adjust;
        diff = target.x - lastTarget.x;
        lastTarget = target;
    }
    if (signOverride) signOverride -= 1;

    if (target == Point(0, 0))
    {
        target = lastTarget;
        return;
    }
}

double Driver::getSteering()
{
    Point carPosition(img.cols / 2, img.rows);
    return (90 - computeAngle(target, carPosition, carPosition + Point(1, 0))) * 0.2 + diff * 0.15;
}
