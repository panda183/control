#include "Driver.h"
#include "../SignDetector/SignDetector.h"
#include <iostream>

Driver::Driver() {}

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
        this->signOverride = 30;
    }
    ld.findLane();
}

Point Driver::getTarget()
{
    if (this->lastTarget == Point(0, 0))
        lastTarget = Point(img.cols / 2, img.rows * 2 / 3);

    Point p = ld.findLanePoint(hug, this->lastTarget);
    if (p == Point(0, 0))
    {
        if (signOverride)
        {
            signOverride -= 1;
            return Point(img.cols / 2 + hug * 5, img.rows * 2 / 3);
        }
        hug = -hug;
        p = ld.findLanePoint(hug, this->lastTarget);
    }
    if (p == Point(0, 0))
        return lastTarget;
    int adjust = img.cols / 3;
    p.x -= hug * adjust;
    lastTarget = p;
    return p;
}

