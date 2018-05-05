#include "OpenNIHelper.h"

openni::Device ni::device;
openni::VideoStream ni::color, ni::depth;
openni::VideoFrameRef ni::colorFrame, ni::depthFrame;

void ni::openni2_init()
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

void ni::openni2_getmat(cv::Mat &mat_color, cv::Mat &mat_depth)
{
    color.readFrame(&colorFrame);
    const openni::RGB888Pixel *colorImageBuffer = (const openni::RGB888Pixel *)colorFrame.getData();

    memcpy(mat_color.data, colorImageBuffer, 3 * colorFrame.getHeight() * colorFrame.getWidth() * sizeof(uint8_t));

    cv::cvtColor(mat_color, mat_color, CV_BGR2RGB);

    depth.readFrame(&depthFrame);
    const openni::DepthPixel *depthImageBuffer = (const openni::DepthPixel *)depthFrame.getData();

    memcpy(mat_depth.data, depthImageBuffer, depthFrame.getHeight() * depthFrame.getWidth() * sizeof(uint16_t));
}

void ni::openni2_destroy()
{
    color.stop();
    color.destroy();
    depth.stop();
    depth.destroy();
    device.close();
}
