#include "ObstacleDetector.h"
#include "Utilities.h"
using namespace cv;
using namespace std;

void ObstacleDetector::detectObstacle(cv::Mat &color, cv::Mat &depth)
{
    Mat hsv, gray;
    cvtColor(color, hsv, COLOR_BGR2HSV);
    int minH = 50, minS = 100, minV = 40,
        maxH = 135, maxS = 255, maxV = 255;
    Scalar min = Scalar(minH, minS, minV);   //HSV VALUE
    Scalar max = Scalar(maxH, maxS, maxV); //HSV VALUE
    inRange(hsv, min, max, gray);
    // imshow("HSV",gray);
    erode(gray, gray, Mat(), Point(-1, -1), 2, 1, 1);
    dilate(gray, gray, Mat(), Point(-1, -1), 8, 1, 1);
    // imshow("dilate",gray);
    vector<vector<Point>> contours;
    findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    ushort minDistance = 30000;
    obstacle = false;
    for (int i = 0; i < contours.size(); i++)
    {
        Rect rect = boundingRect(contours[i]);
        if (abs((rect.width * 1.0 / rect.height) - 1) > 0.4) continue;
        rect.x += 6;
        rect.y += 6;
        rect.width -= 6 * 2;
        rect.height -= 6 * 2;
        Point center(rect.x + rect.width / 2, rect.y + rect.height / 2);
        int radius = rect.height / 2;
        int RADIUS = 49500;
        float HEIGHT = 70;
        ushort distance = depth.at<ushort>(center);
        if (abs(RADIUS - radius * distance) > RADIUS / 7) continue;
        // cout << radius << " : " << distance << endl;

        Point3f p = utl::getRealPointInWorld(center, distance);
        float signHeight = utl::dToPlane(p, utl::groundPlane);
        if (abs(signHeight - HEIGHT) > 30) continue;
        // cout << "height:" << signHeight << " ";

        rectangle(color, rect, Scalar(0, 255, 0));

        if (distance > minDistance) continue;
        obstacle = true;
        this->center = center;
        minDistance = distance;
    }
}
