#include "LaneDetector.h"
#include <algorithm>
#include <cstring>

LaneDetector::LaneDetector() {}
LaneDetector::~LaneDetector() {}

void LaneDetector::inputImg(Mat img)
{
    if (img.empty())
    {
        printf("Error: input is empty\n");
        return;
    }
    src = img;
    cvtColor(img, this->img, COLOR_RGB2GRAY);
}

void LaneDetector::findLane()
{
    if (img.empty())
    {
        printf("Error: input is empty\n");
        return;
    }
    int THRESHOLD_GREY = 100; // [0 - 255], >threshold is white, otherwise is black
    int THRESHOLD_LANE_LENGTH = 200;

    int length[img.rows][img.cols];
    int fullLength[img.rows][img.cols];

    for (int i = 0; i < img.rows; i++)
        for (int j = 0; j < img.cols; j++)
            fullLength[i][j] = length[i][j] = 0;

    for (int i = 0; i < img.rows; i++)
        for (int j = 0; j < img.cols; j++)
            if (img.at<uchar>(i, j) > THRESHOLD_GREY)
            {
                if (i == 0)
                {
                    length[i][j] = 1;
                    continue;
                }
                length[i][j] = max(length[i][j], length[i - 1][j]);
                if (j - 1 >= 0) length[i][j] = max(length[i][j], length[i - 1][j - 1]);
                if (j + 1 < img.cols) length[i][j] = max(length[i][j], length[i - 1][j + 1]);
                if (j - 2 >= 0) length[i][j] = max(length[i][j], length[i - 1][j - 2]);
                if (j + 2 < img.cols) length[i][j] = max(length[i][j], length[i - 1][j + 2]);
                length[i][j] += 1;
            }

    for (int i = img.rows - 2; i >= 0; i--)
        for (int j = 0; j < img.cols; j++)
        {
            fullLength[i][j] = length[i][j];
            if (length[i][j] == length[i + 1][j] - 1) fullLength[i][j] = max(fullLength[i][j], fullLength[i + 1][j]);
            if ((j - 1 >= 0) && (length[i][j] == length[i + 1][j - 1] - 1)) fullLength[i][j] = max(fullLength[i + 1][j - 1], fullLength[i][j]);
            if ((j + 1 < img.cols) && (length[i][j] == length[i + 1][j + 1] - 1)) fullLength[i][j] = max(fullLength[i + 1][j + 1], fullLength[i][j]);
            if ((j - 2 >= 0) && (length[i][j] == length[i + 1][j - 2] - 1)) fullLength[i][j] = max(fullLength[i + 1][j - 2], fullLength[i][j]);
            if ((j + 2 < img.cols) && (length[i][j] == length[i + 1][j + 2] - 1)) fullLength[i][j] = max(fullLength[i + 1][j + 2], fullLength[i][j]);
        }

    Vec3b red(0, 0, 255);

    for (int i = 0; i < img.rows; i++)
        for (int j = 0; j < img.cols; j++)
            if (fullLength[i][j] > THRESHOLD_LANE_LENGTH) src.at<Vec3b>(i, j) = red;

    for (int i = img.cols - 1; i >= 0; i--)
        if (fullLength[img.rows * 2 / 3][i] > THRESHOLD_LANE_LENGTH)
        {
            laneCenter = Point(i - 250, img.rows * 2 / 3);
            return;
        }
}
