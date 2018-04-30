#include <opencv2/opencv.hpp>

class ObstacleDetector
{
  public:
    void detectObstacle(cv::Mat &color, cv::Mat &depth);
    bool obstacle;
    cv::Point center;
};