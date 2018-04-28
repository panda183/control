#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include "Driver.h"
#include "Utilities.h"
#include "OpenNI.h"

using namespace std;
using namespace cv;

#define PI 3.14159265

Mat color, depth;
Vec4f groundPlane;       //phuong trinh mat phang
vector<Point3f> _3Point; //3 diem tren mat phang

void CallBackFunc(int event, int x, int y, int flags, void *userdata)
{
    if (event == EVENT_LBUTTONDOWN && depth.at<ushort>(y, x) > 0)
    {
        Point3f p = utl::getRealPointInWorld(Point(x, y), depth.at<ushort>(y, x));
        if (_3Point.size() < 3)
        {
            _3Point.push_back(p);
            cout << "Point:" << p << endl;
        }
        if (_3Point.size() == 3)
        {
            groundPlane = utl::findPlaneEquation(_3Point);
            cout << "PLane:" << groundPlane << endl;
            _3Point.clear();
        }
    }
    if (event == EVENT_RBUTTONDOWN && depth.at<ushort>(y, x) > 0)
    {
        Point3f p = utl::getRealPointInWorld(Point(x, y), depth.at<ushort>(y, x));
        cout << "Distance:" << utl::dToPlane(p, groundPlane) << endl;
    }
}
int main(int argc, char *argv[])
{
    utl::openni2_init();
    namedWindow("depth");
    setMouseCallback("depth", CallBackFunc, NULL);
    do
    {
        utl::openni2_getmat(color, depth);

        Mat adjMap;
        convertScaleAbs(depth, adjMap, 255.0 / 6000);

        imshow("color", color);
        imshow("depth", adjMap);
    } while (waitKey(20) != 27);
    utl::openni2_destroy();
    return 0;
}
