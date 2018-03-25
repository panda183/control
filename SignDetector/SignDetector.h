#ifndef SIGN_DETECTOR_H
#define SIGN_DETECTOR_H

#include <opencv2/opencv.hpp>
#include "../LaneDetector/LaneDetector.h"

namespace sd
{
    extern Mat leftSign, rightSign;
    void init();
    double similar(Mat &img1, Mat &img2);
    void equalizeHistBGR(Mat &src, Mat &dst);
    double distance(Point p1, Point p2);
    side DetectSign(Mat &src);
    int recognizeSign(Mat &sign);
}

#endif
