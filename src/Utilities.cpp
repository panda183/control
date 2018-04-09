#include "Utilities.h"
#define PI 3.14159

openni::Device utl::device;
openni::VideoStream  utl::color, utl::depth;
openni::VideoFrameRef utl::colorFrame, utl::depthFrame;

void utl::openni2_init()
{
    openni::OpenNI::initialize();
    device.open(openni::ANY_DEVICE);

    color.create(device, openni::SENSOR_COLOR);
    color.start();

    depth.create(device, openni::SENSOR_DEPTH);
    depth.start();
}

void utl::openni2_getmat(cv::Mat& mat_color, cv::Mat& mat_depth)
{
    color.readFrame(&colorFrame);
    const openni::RGB888Pixel* colorImageBuffer = (const openni::RGB888Pixel*)colorFrame.getData();

    mat_color.create(colorFrame.getHeight(), colorFrame.getWidth(), CV_8UC3);
    memcpy( mat_color.data, colorImageBuffer, 3 * colorFrame.getHeight() * colorFrame.getWidth() * sizeof(uint8_t) );

    cv::cvtColor(mat_color, mat_color, CV_BGR2RGB);

    depth.readFrame(&depthFrame);
    const openni::DepthPixel* depthImageBuffer = (const openni::DepthPixel*)depthFrame.getData();

    mat_depth.create(depthFrame.getHeight(), depthFrame.getWidth(), CV_16UC1);
    memcpy( mat_depth.data, depthImageBuffer, depthFrame.getHeight() * depthFrame.getWidth() * sizeof(uint16_t) );

    convertScaleAbs(mat_depth, mat_depth, 0.05);
}

void utl::openni2_destroy()
{
    color.stop();
    color.destroy();
    depth.stop();
    depth.destroy();
    device.close();
}

double utl::computeAngle(cv::Point A, cv::Point O, cv::Point B)
{
    if ((A == O) || (B == O)) return 90;
    cv::Point vOA = A - O;
    cv::Point vOB = B - O;
    double dOA = sqrt(vOA.x * vOA.x + vOA.y * vOA.y);
    double dOB = sqrt(vOB.x * vOB.x + vOB.y * vOB.y);
    double res = acos(vOA.dot(vOB) / (dOA * dOB));
    res = res / PI * 180;
    return res;
}
