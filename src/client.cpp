#include <chrono>
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "OpenNIHelper.h"
#include <unistd.h>
#include <chrono>
#include <stdlib.h>
#include "LaneDetector.h"
#include "SignDetector.h"

using namespace cv;
using namespace std;

Mat depthImg(480, 640, CV_16UC1),
    colorImg(480, 640, CV_8UC3);
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
    do{
        ni::openni2_getmat(colorImg, depthImg);
        utl::splitGround(colorImg,depthImg);
        sd::DetectSign(depthImg);
        imshow("ground",utl::groundImg);
        imshow("nonGround",utl::nonGroundImg);
    }while(waitKey(1)!=27);
    return true;
}
void init()
{
    ni::openni2_init();
    sd::init();
    utl::readGroundPlane();
}

