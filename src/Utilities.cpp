#include "Utilities.h"
#include <fstream>

cv::Vec4f utl::groundPlane;
cv::Mat utl::groundImg=Mat::zeros(240,320,CV_8UC3)
        ,utl::nonGroundImg=Mat::zeros(240,320,CV_8UC3);
cv::Mat utl::transformMatrix;

void utl::splitGround(cv::Mat &colorImg,cv::Mat &depth){
    for(int i=0;i<colorImg.cols;i++){
        for(int j=0;j<colorImg.rows;j++){
            if(depth.at<ushort>(j,i)>0){
                Point3f p=getRealPointInWorld(Point(i,j),depth.at<ushort>(j,i));
                if(dToPlane(p,groundPlane)>50){
                    nonGroundImg.at<Vec3b>(j,i)=colorImg.at<Vec3b>(j,i);
                    groundImg.at<Vec3b>(j,i)=Vec3b(0,0,0);
                }
                else {
                    groundImg.at<Vec3b>(j,i)=colorImg.at<Vec3b>(j,i);
                    nonGroundImg.at<Vec3b>(j,i)=Vec3b(0,0,0);
                }
            }else{
                groundImg.at<Vec3b>(j,i)=colorImg.at<Vec3b>(j,i);
                nonGroundImg.at<Vec3b>(j,i)=Vec3b(0,0,0);
            }
        }
    } 
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
cv::Point3f utl::rayCastGroundPlane(cv::Point screenPoint, cv::Vec4f plane){
    cv::Point3f vu;
    vu=utl::getRealPointInWorld(screenPoint,2000);
    float t=-plane[3]/(plane[0]*vu.x+plane[1]*vu.y+plane[2]*vu.z);
    return cv::Point3f(vu.x*t,vu.y*t,vu.z*t);
}
cv::Point2f utl::worldToScreen(cv::Point3f world){
    float px = 2.0 * world.z * tan(60.0 * PI / (180.0 * 2)); //fovH
    float py = 2.0 * world.z * tan(49.5 * PI / (180.0 * 2)); //fovV
    
    return cv::Point2f(320.0/2-world.x*320.0/px,240.0/2-world.y*320.0/py);
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
    res = cv::Vec4f(vn.x, vn.y, vn.z, -(vn.x * _3Point.at(0).x + vn.y * _3Point.at(0).y + vn.z * _3Point.at(0).z))/1000.0;
    return res;
}
//Tim toa do trong the gioi that, goc toa do la camera
cv::Point3f utl::getRealPointInWorld(cv::Point screenPoint, int depth)
{
    cv::Point3f res;
    float px = 2.0 * depth * tan(60.0 * PI / (180.0 * 2)); //fovH
    float py = 2.0 * depth * tan(49.5 * PI / (180.0 * 2)); //fovV
    res.x = ((320.0/2 - screenPoint.x) / 320.0) * px;
    res.y = ((240.0/2 - screenPoint.y) / 240.0) * py;
    res.z = depth;
    return res;
}
void utl::getTransformMatrix(){
    Point3f M,N,P,Q;
    M=utl::rayCastGroundPlane(Point(0,240),groundPlane);
    N=utl::rayCastGroundPlane(Point(320,240),groundPlane);
    Point3f vAB,vAC,vu;
    vAB=N-M;
    vAC=cv::Point3f(groundPlane[0],groundPlane[1],groundPlane[2]);
    vu = cv::Vec3f(vAB.y * vAC.z - vAB.z * vAC.y, vAB.z * vAC.x - vAB.x * vAC.z, vAB.x * vAC.y - vAB.y * vAC.x);
    double dVu=sqrt(vu.x*vu.x+vu.y*vu.y+vu.z*vu.z);
    double dMN=sqrt(vAB.x*vAB.x+vAB.y*vAB.y+vAB.z*vAB.z);
    P=M-vu*((dMN/LANE_SIZE*240)/dVu);
    Q=N-vu*((dMN/LANE_SIZE*240)/dVu);
    // Input Quadilateral or Image plane coordinates
    Point2f inputQuad[4];
    // Output Quadilateral or World plane coordinates
    Point2f outputQuad[4];
    // Lambda Matrix
    //Mat lambda;
    // Set the lambda matrix the same type and size as input
    //lambda = Mat::zeros(input.rows, input.cols, input.type());
    // The 4 points that select quadilateral on the input , from top-left in clockwise order
    // These four pts are the sides of the rect box used as input
    inputQuad[0] = Point2f(0, 240);
    inputQuad[1] = Point2f(320, 240);
    inputQuad[2] = utl::worldToScreen(P);
    inputQuad[3] = utl::worldToScreen(Q);
    // The 4 points where the mapping is to be done , from top-left in clockwise order
    outputQuad[0] = Point2f((320-LANE_SIZE)/2, 240);
    outputQuad[1] = Point2f((320-LANE_SIZE)/2+LANE_SIZE, 240);
    outputQuad[2] = Point2f((320-LANE_SIZE)/2, 0);
    outputQuad[3] = Point2f((320-LANE_SIZE)/2+LANE_SIZE, 0);
    // Get the Perspective Transform Matrix i.e. lambda
    utl::transformMatrix=getPerspectiveTransform(inputQuad, outputQuad);
}
void utl::readGroundPlane()
{
    std::ifstream input(GROUND_PLANE_INPUT);
    if (!input.is_open())
        throw "Ground plane not found! Please run Setup first.";
    for (int i = 0; i < 4; i++)
        input >> groundPlane[i];
    std::cout << "ground plane: " << groundPlane << std::endl;
}