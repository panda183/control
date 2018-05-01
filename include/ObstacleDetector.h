#ifndef OBSTACLE_DETECTOR_H
#define OBSTACLE_DETECTOR_H

#include <opencv2/opencv.hpp>

class ObstacleDetector
{
  public:
    void detectObstacle(cv::Mat &color, cv::Mat &depth);
    bool obstacle;
    cv::Point center;
};

#endif
