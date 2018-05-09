#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "OpenNIHelper.h"
#include <unistd.h>
#include <chrono>
#include <stdlib.h>
#include "LaneDetector.h"

using namespace cv;
using namespace std;

Mat depthImg(240, 320, CV_16UC1),
    colorImg(240, 320, CV_8UC3);
bool RunCar();
void Destroy();
void init();
int main(int argc, char *argv[])
{
    init();
    RunCar();
    Destroy();
}
void Destroy()
{
    ni::openni2_destroy();
}
bool RunCar()
{
    float P=0,D=0,angle=0;
    do{
        auto cur_time = std::chrono::system_clock::now();
        ni::openni2_getmat(colorImg, depthImg);
        utl::splitGround(colorImg,depthImg);
        cout << "h" << endl;
        ld::findLane();
        float deltaTime=chrono::duration<double, milli> (std::chrono::system_clock::now()-cur_time).count();
        D=((ld::xCenterLane-160)-P)/deltaTime;
        P=(ld::xCenterLane-160);
        angle=P+D;
        imshow("ground",utl::groundImg);
        imshow("nonGround",utl::nonGroundImg);
    }while(waitKey(1)!=27);
    return true;
}
void init()
{
    ni::openni2_init();
    utl::readGroundPlane();
    utl::getTransformMatrix();
}

