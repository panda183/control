#include "ObstacleDetector.h"
#include "Utilities.h"

void ObstacleDetector::detectObstacle(cv::Mat &color, cv::Mat &depth)
{
    cv::Mat height(depth.rows, depth.cols, CV_16UC1);
    for (int i = 0; i < height.rows; i++)
        for (int j = 0; j < height.cols; j++)
        {
            cv::Point p(j, i);
            height.at<ushort>(p) = 0;
            if (depth.at<ushort>(p) == 0) continue;

            cv::Point3f _3p = utl::getRealPointInWorld(p, depth.at<ushort>(p));
            height.at<ushort>(p) = (ushort)utl::dToPlane(_3p, utl::groundPlane);
        }
    cv::threshold(height, height, 30, 64000, CV_THRESH_BINARY);
    cv::imshow("height", height);
}