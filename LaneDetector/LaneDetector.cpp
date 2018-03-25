#include "LaneDetector.h"
#include <algorithm>
#include <cstring>
#include <queue>

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

Point LaneDetector::findLane(side hug, Point start)
{
    int THRESHOLD_GREY = 120; // [0 - 255], >threshold is white, otherwise is black
    int THRESHOLD_LANE_LENGTH = img.rows / 4;
    int yLimit = img.rows / 4;
    int oo = 9999999;
    int dx[8] = {-1, 0, 1, -1, 1, -1 ,0, 1};
    int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};

    int d[img.rows][img.cols];
    int fullLength[1000];
    int index[img.rows][img.cols];
    int ind = 0;
    queue<Point> q;

    for (int i = 0; i < img.rows; i++)
        for (int j = 0; j < img.cols; j++)
        {
            d[i][j] = index[i][j] = oo;
        }

    for (int i = yLimit; i < img.rows; i++)
        for (int j = 0; j < img.cols; j++)
        {
            if (img.at<uchar>(i, j) < THRESHOLD_GREY) continue;
            if (d[i][j] != oo) continue;

            d[i][j] = 0;
            index[i][j] = ind;
            fullLength[ind] = 0;
            q.push(Point(i, j));

            while (!q.empty())
            {
                Point u = q.front();
                for (int x = 0; x < 8; x++)
                {
                    Point v = u + Point(dx[x], dy[x]);
                    if ((v.x < yLimit) || (v.x >= img.rows) || (v.y < 0) || (v.y >= img.cols)) continue;
                    if (img.at<uchar>(v.x, v.y) < THRESHOLD_GREY) continue;
                    if (d[v.x][v.y] != oo) continue;
                    d[v.x][v.y] = d[u.x][u.y] + 1;
                    index[v.x][v.y] = ind;
                    fullLength[ind] = max(fullLength[ind], d[v.x][v.y]);
                    q.push(v);
                }
                q.pop();
            }
            ind += 1;
        }
    
    Vec3b red(0, 0, 255);
    for (int i = 0; i < img.rows; i++)
        for (int j = 0; j < img.cols; j++)
        {
            if (index[i][j] == oo) continue;
            if (fullLength[index[i][j]] > THRESHOLD_LANE_LENGTH) src.at<Vec3b>(i, j) = red;
        }

    Point move(1, 0);
    if (hug == l) move.x = -1;
    Point p = start;
    printf("%i %i \n", p.x, p.y);
    while (src.at<Vec3b>(p) != red)
        p += move;
    printf("%i %i \n", p.x, p.y);
    return p;
}
