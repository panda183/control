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
    int i=0;
    do{
        //ni::openni2_getmat(colorImg, depthImg);
        colorImg=imread("dataset/Sample01/rgb/"+to_string(i)+".png");
        depthImg=imread("dataset/Sample01/depth/"+to_string(i)+".png",CV_LOAD_IMAGE_ANYDEPTH);
        i++;
        utl::splitGround(colorImg,depthImg);
        ld::findLane();
        imshow("ground",utl::groundImg);
        imshow("nonGround",utl::nonGroundImg);
    }while(waitKey(10)!=27);
    return true;
}
void init()
{
    //ni::openni2_init();
    utl::readGroundPlane();
    utl::getTransformMatrix();
}

