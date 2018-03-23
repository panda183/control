#include "LaneDetector.h"

LaneDetector::LaneDetector() {}
LaneDetector::~LaneDetector() {}

void LaneDetector::inputImg(const Mat& img)
{
    if (img.empty())
    {
        printf("Error: input is empty");
        return;
    }
    cvtColor(img, this->img, COLOR_RGB2GRAY);
}

void LaneDetector::findLane()
{
    // int diff[img.rows][img.cols];
    // for (int i = img.rows - 2; i >= 0; i--)
    // {
    //     for (int j = 0; j < img.cols - 1; j++)
    //     {

    //     }
    // }
    if (img.empty())
    {
        printf("Error: input is empty");
        return;
    }
    int threshold = 15,
        row = img.rows * 2 / 3;
    for (int i = img.cols - 3; i >= 0; i--)
    {
        if (abs(img.at<uchar>(row, i) - img.at<uchar>(row, i + 2)) > threshold)
        {
            laneCenter = Point(i - 200, row);
            return;
        }
    }
}
