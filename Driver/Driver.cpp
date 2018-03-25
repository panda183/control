#include "Driver.h"
#include "../SignDetector/SignDetector.h"
#include <iostream>

Driver::Driver() {}

Driver::~Driver() {}

void Driver::setHug(side hug)
{
    this->hug = hug;
}

void Driver::inputImg(Mat img)
{
    this->ld.inputImg(img);
    this->img = img;
    hug = sd::DetectSign(img);
    if (hug == l)
        std::cout << "left" << std::endl;
    else
        std::cout << "right" << std::endl;
    ld.findLane();
}

Point Driver::getTarget()
{
    if (this->lastTarget == Point(0, 0))
        lastTarget = Point(img.cols / 2, img.rows * 2 / 3);
    Point p = ld.findLanePoint(hug, this->lastTarget);
    if (p == Point(0, 0)) 
    {
        if (hug == l)
            hug = r;
        else 
            hug = l;
        p = ld.findLanePoint(hug, this->lastTarget);
    }
    if (p == Point(0, 0))
        return lastTarget;
    int adjust = img.cols / 3;
    if (hug == l)
        p.x += adjust;
    else
        p.x -= adjust;
    lastTarget = p;
    return p;
}

