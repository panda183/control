#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include "Utilities.h"
#include "OpenNIHelper.h"
#include <fstream>

using namespace std;
using namespace cv;

Mat depth(480, 640, CV_16UC1),
    color(480, 640, CV_8UC3);
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
            cout << "Plane:" << groundPlane << endl;
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
    //ni::openni2_init();
    namedWindow("depth");
    setMouseCallback("depth", CallBackFunc, NULL);
    cout << "three left-click to choose ground plane" << endl;
    cout << "ENTER to confirm, ESC to cancel" << endl;
    int k,i=0;
    while (1)
    {
        //ni::openni2_getmat(color, depth);
        color=imread("dataset/Sample01/rgb/"+to_string(i)+".png");
        depth=imread("dataset/Sample01/depth/"+to_string(i)+".png",CV_LOAD_IMAGE_ANYDEPTH);
        i++;
        Mat adjMap;
        convertScaleAbs(depth, adjMap, 255.0 / 6000);
        imshow("color", color);
        imshow("depth", adjMap);
        k = waitKey(100);
        if (k != -1) break;
    }
    if (k == 27)
        cout << "exiting" << endl;
    else
    {
        ofstream output(GROUND_PLANE_INPUT);
        for (int i = 0; i < 4; i++)
            output << groundPlane[i] << " ";
        output.close();
        cout << "ground plane writen to " << GROUND_PLANE_INPUT << endl;
        cout << "exiting" << endl;
    }
    //ni::openni2_destroy();
    return 0;
}
