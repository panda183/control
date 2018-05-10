#include "Driver.h"
#include "SignDetector.h"
#include "Utilities.h"
#include <iostream>
#define PI 3.14159

Driver::Driver()
{
    this->setHug(LEFT);
    sd::init();
}

Driver::~Driver() {}

void Driver::setHug(int hug)
{
    this->hug = hug;
}

void Driver::inputImg(Mat color, Mat depth)
{
    this->ld.inputImg(color);
    this->color = color;
    this->depth = depth;
    sd::DetectSign(color, depth);
    // this->od.detectObstacle(color, depth);
    if (sd::sign)
    {
        hug = sd::sign;
        this->signOverride = 15;
    }
    findTarget();
}

void Driver::findTarget()
{
    diff = 0;
    int targetRow = color.rows * 0.5;
    if (this->lastTarget == Point(0, 0))
        lastTarget = Point(color.cols / 2, targetRow);

    target = ld.findLanePoint(hug, this->lastTarget);
    if (target == Point(0, 0))
    {
        if (signOverride)
        {
            target = Point(color.cols / 2 + hug * 10, targetRow);
            return;
        }
        hug = -hug;
        target = ld.findLanePoint(hug, this->lastTarget);
    }
    else
    {
        int adjust = color.cols / 3.5;
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
    circle(this->color, target, 3, Scalar(0, 255, 0), 3);
}

double Driver::getSteering()
{
    Point carPosition(color.cols / 2, color.rows);
    return (90 - utl::computeAngle(target, carPosition, carPosition + Point(1, 0))) * 0.2 + diff * 0.15;
}
