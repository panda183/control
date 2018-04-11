#ifndef UTILITIES_H
#define UTILITIES_H

#include "opencv2/opencv.hpp"
#include "OpenNI.h"
#define PI 3.14159

namespace utl
{
    extern openni::Device device;
    extern openni::VideoStream  color, depth;
    extern openni::VideoFrameRef colorFrame, depthFrame;
    extern std::map<int, int> depth_map;

    void openni2_init();
    void openni2_getmat(cv::Mat& color, cv::Mat& depth);
    void openni2_destroy();

    double computeAngle(cv::Point A, cv::Point O, cv::Point B);
    double distance(cv::Point A, cv::Point B);
    void read_config();
}

#endif
