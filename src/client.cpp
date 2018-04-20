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

//Khoang cach tu diem toi mat phang
float dToPlane(Point3f p, Vec4f plane)
{
    if (plane[0] == 0 && plane[1] == 0 && plane[2] == 0)
        return 0;
    return abs(plane[0] * p.x + plane[1] * p.y + plane[2] * p.z + plane[3]) / sqrt(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
}
//Tim 4 he so phuong trinh mat phang dua vao 3 diem tren mat phang
Vec4f findPlaneEquation(vector<Point3f> _3Point)
{
    Vec4f res;
    Point3f vAB, vAC, vn;
    vAB = Vec3f(_3Point.at(1).x - _3Point.at(0).x, _3Point.at(1).y - _3Point.at(0).y, _3Point.at(1).z - _3Point.at(0).z);
    vAC = Vec3f(_3Point.at(2).x - _3Point.at(0).x, _3Point.at(2).y - _3Point.at(0).y, _3Point.at(2).z - _3Point.at(0).z);
    vn = Vec3f(vAB.y * vAC.z - vAB.z * vAC.y, vAB.z * vAC.x - vAB.x * vAC.z, vAB.x * vAC.y - vAB.y * vAC.x);
    res = Vec4f(vn.x, vn.y, vn.z, -(vn.x * _3Point.at(0).x + vn.y * _3Point.at(0).y + vn.z * _3Point.at(0).z));
    return res;
}
//Tim toa do trong the gioi that, goc toa do la camera
Point3f getRealPointInWorld(Point screenPoint, int depth)
{
    Point3f res;
    float px = 2.0 * depth * tan(60.0 * PI / (180.0 * 2)); //fovH
    float py = 2.0 * depth * tan(49.5 * PI / (180.0 * 2)); //fovV
    res.x = ((320.0 - screenPoint.x) / 640.0) * px;
    res.y = ((240.0 - screenPoint.y) / 480.0) * py;
    res.z = depth;
    return res;
}
void CallBackFunc(int event, int x, int y, int flags, void *userdata)
{
    if (event == EVENT_LBUTTONDOWN && depth.at<ushort>(y, x) > 0)
    {
        Point3f p = getRealPointInWorld(Point(x, y), depth.at<ushort>(y, x));
        if (_3Point.size() < 3)
        {
            _3Point.push_back(p);
            cout << "Point:" << p << endl;
        }
        if (_3Point.size() == 3)
        {
            groundPlane = findPlaneEquation(_3Point);
            cout << "PLane:" << groundPlane << endl;
            _3Point.clear();
        }
    }
    if (event == EVENT_RBUTTONDOWN && depth.at<ushort>(y, x) > 0)
    {
        Point3f p = getRealPointInWorld(Point(x, y), depth.at<ushort>(y, x));
        cout << "Distance:" << dToPlane(p, groundPlane) << endl;
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
