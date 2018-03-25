#include "Driver.h"

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
    ld.findLane();
}

Point Driver::getTarget()
{
    if (this->lastTarget == Point(0, 0))
        lastTarget = Point(img.cols / 2, img.rows * 2 / 3);
    Point p = ld.findLanePoint(hug, this->lastTarget);
    int adjust = img.cols / 3;
    if (hug == l)
        p.x += adjust;
    else
        p.x -= adjust;
    lastTarget = p;
    return p;
}
