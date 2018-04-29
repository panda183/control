#include "Utilities.h"
#include <fstream>

openni::Device utl::device;
openni::VideoStream utl::color, utl::depth;
openni::VideoFrameRef utl::colorFrame, utl::depthFrame;
cv::Vec4f utl::groundPlane;

void utl::openni2_init()
{
    openni::OpenNI::initialize();
    device.open(openni::ANY_DEVICE);

    color.create(device, openni::SENSOR_COLOR);
    color.start();

    depth.create(device, openni::SENSOR_DEPTH);
    depth.start();

    color.setMirroringEnabled(false);
    depth.setMirroringEnabled(false);

    device.setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);
    device.setDepthColorSyncEnabled(true);
}

void utl::openni2_getmat(cv::Mat &mat_color, cv::Mat &mat_depth)
{
    color.readFrame(&colorFrame);
    const openni::RGB888Pixel *colorImageBuffer = (const openni::RGB888Pixel *)colorFrame.getData();

    mat_color.create(colorFrame.getHeight(), colorFrame.getWidth(), CV_8UC3);
    memcpy(mat_color.data, colorImageBuffer, 3 * colorFrame.getHeight() * colorFrame.getWidth() * sizeof(uint8_t));

    cv::cvtColor(mat_color, mat_color, CV_BGR2RGB);

    depth.readFrame(&depthFrame);
    const openni::DepthPixel *depthImageBuffer = (const openni::DepthPixel *)depthFrame.getData();

    mat_depth.create(depthFrame.getHeight(), depthFrame.getWidth(), CV_16UC1);
    memcpy(mat_depth.data, depthImageBuffer, depthFrame.getHeight() * depthFrame.getWidth() * sizeof(uint16_t));
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
    if ((A == O) || (B == O))
        return 90;
    cv::Point vOA = A - O;
    cv::Point vOB = B - O;
    double dOA = sqrt(vOA.x * vOA.x + vOA.y * vOA.y);
    double dOB = sqrt(vOB.x * vOB.x + vOB.y * vOB.y);
    double res = acos(vOA.dot(vOB) / (dOA * dOB));
    res = res / PI * 180;
    return res;
}
//Khoang cach tu diem toi mat phang
float utl::dToPlane(cv::Point3f p, cv::Vec4f plane)
{
    if (plane[0] == 0 && plane[1] == 0 && plane[2] == 0)
        return 0;
    return abs(plane[0] * p.x + plane[1] * p.y + plane[2] * p.z + plane[3]) / sqrt(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
}
//Tim 4 he so phuong trinh mat phang dua vao 3 diem tren mat phang
cv::Vec4f utl::findPlaneEquation(std::vector<cv::Point3f> _3Point)
{
    cv::Vec4f res;
    cv::Point3f vAB, vAC, vn;
    vAB = cv::Vec3f(_3Point.at(1).x - _3Point.at(0).x, _3Point.at(1).y - _3Point.at(0).y, _3Point.at(1).z - _3Point.at(0).z);
    vAC = cv::Vec3f(_3Point.at(2).x - _3Point.at(0).x, _3Point.at(2).y - _3Point.at(0).y, _3Point.at(2).z - _3Point.at(0).z);
    vn = cv::Vec3f(vAB.y * vAC.z - vAB.z * vAC.y, vAB.z * vAC.x - vAB.x * vAC.z, vAB.x * vAC.y - vAB.y * vAC.x);
    res = cv::Vec4f(vn.x, vn.y, vn.z, -(vn.x * _3Point.at(0).x + vn.y * _3Point.at(0).y + vn.z * _3Point.at(0).z));
    return res;
}
//Tim toa do trong the gioi that, goc toa do la camera
cv::Point3f utl::getRealPointInWorld(cv::Point screenPoint, int depth)
{
    cv::Point3f res;
    float px = 2.0 * depth * tan(60.0 * PI / (180.0 * 2)); //fovH
    float py = 2.0 * depth * tan(49.5 * PI / (180.0 * 2)); //fovV
    res.x = ((320.0 - screenPoint.x) / 640.0) * px;
    res.y = ((240.0 - screenPoint.y) / 480.0) * py;
    res.z = depth;
    return res;
}

void utl::readGroundPlane()
{
    std::ifstream input(GROUND_PLANE_INPUT);
    for (int i = 0; i < 4; i++)
        input >> groundPlane[i];
    std::cout << "ground plane: " << groundPlane << std::endl;
}