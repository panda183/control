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
    int threshold = 70; // [0 - 255], >threshold is white, otherwise is black
    int length[img.rows][img.cols];

    for (int i = 0; i < img.rows; i++)
        for (int j = 0; j < img.cols; j++)
            length[i][j] = 0;

    for (int i = 0; i < img.rows; i++)
        for (int j = 0; j < img.cols; j++)
            if (img.at<uchar>(i, j) > threshold)
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

    Vec3b red(0, 0, 255);
    for (int i = 1; i < img.rows; i++)
        for (int j = 0; j < img.cols; j++)
        {
            if (length[i][j] == length[i - 1][j] + 1) src.at<Vec3b>(i, j) = red;
            if ((j - 1 >= 0) && (length[i][j] == length[i - 1][j - 1] + 1)) src.at<Vec3b>(i, j) = red;
            if ((j + 1 < img.cols) && (length[i][j] == length[i - 1][j + 1] + 1)) src.at<Vec3b>(i, j) = red;
            if ((j - 2 >= 0) && (length[i][j] == length[i - 1][j - 2] + 1)) src.at<Vec3b>(i, j) = red;
            if ((j + 2 < img.cols) && (length[i][j] == length[i - 1][j + 2] + 1)) src.at<Vec3b>(i, j) = red;
        }
    imshow("lane", src);
}
