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
    return (90 - utl::computeAngle(target, carPosition, carPosition + Point(1, 0))) * 0.2 + diff * 0.15;
}
