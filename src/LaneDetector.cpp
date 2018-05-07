#include "LaneDetector.h"

int ld::xCenterLane=320;
Vec3f ld::laneCurve;

Vec3f ld::CurveEstimation(vector<Point2f> lanePoints) {
    int i, j, k;
    double X[2 * 2 + 1];                        //Array that will store the values of sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n)
    for (i = 0;i < 2 * 2 + 1;i++)
    {
        X[i] = 0;
        for (j = 0;j < lanePoints.size();j++)
            X[i] = X[i] + pow(lanePoints[j].x, i);        //consecutive positions of the array will store N,sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n)
    }
    double B[2 + 1][2 + 2], a[2 + 1];            //B is the Normal matrix(augmented) that will store the equations, 'a' is for value of the final coefficients
    for (i = 0;i <= 2;i++)
        for (j = 0;j <= 2;j++)
            B[i][j] = X[i + j];            //Build the Normal matrix by storing the corresponding coefficients at the right positions except the last column of the matrix
    double Y[2 + 1];                    //Array to store the values of sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi)
    for (i = 0;i < 2 + 1;i++)
    {
        Y[i] = 0;
        for (j = 0;j < lanePoints.size();j++)
            Y[i] = Y[i] + pow(lanePoints[j].x, i)*lanePoints[j].y;        //consecutive positions will store sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi)
    }
    for (i = 0;i <= 2;i++)
        B[i][2 + 1] = Y[i];                //load the values of Y as the last column of B(Normal Matrix but augmented)

    for (i = 0;i < 3;i++)                    //From now Gaussian Elimination starts(can be ignored) to solve the set of linear equations (Pivotisation)
        for (k = i + 1;k < 3;k++)
            if (B[i][i] < B[k][i])
                for (j = 0;j <= 3;j++)
                {
                    double temp = B[i][j];
                    B[i][j] = B[k][j];
                    B[k][j] = temp;
                }
    for (i = 0;i < 3 - 1;i++)            //loop to perform the gauss elimination
        for (k = i + 1;k < 3;k++)
        {
            double t = B[k][i] / B[i][i];
            for (j = 0;j <= 3;j++)
                B[k][j] = B[k][j] - t * B[i][j];    //make the elements below the pivot elements equal to zero or elimnate the variables
        }
    for (i = 3 - 1;i >= 0;i--)                //back-substitution
    {                        //x is an array whose values correspond to the values of x,y,z..
        a[i] = B[i][3];                //make the variable to be calculated equal to the rhs of the last equation
        for (j = 0;j < 3;j++)
            if (j != i)            //then subtract all the lhs values except the coefficient of the variable whose value                                   is being calculated
                a[i] = a[i] - B[i][j] * a[j];
        a[i] = a[i] / B[i][i];            //now finally divide the rhs by the coefficient of the variable to be calculated
    }
    return Vec3f(a[0], a[1], a[2]);
}
Mat ld::birdView(Mat &input)
{
    Mat output;
    // Input Quadilateral or Image plane coordinates
    Point2f inputQuad[4];
    // Output Quadilateral or World plane coordinates
    Point2f outputQuad[4];

    // Lambda Matrix
    Mat lambda;

    // Set the lambda matrix the same type and size as input
    lambda = Mat::zeros(input.rows, input.cols, input.type());
    // The 4 points that select quadilateral on the input , from top-left in clockwise order
    // These four pts are the sides of the rect box used as input


    inputQuad[0] = Point2f(290, 120);
    inputQuad[1] = Point2f(350, 120);
    inputQuad[2] = Point2f(640, 480);
    inputQuad[3] = Point2f(0, 480);
    // The 4 points where the mapping is to be done , from top-left in clockwise order
    outputQuad[0] = Point2f(290, 50);
    outputQuad[1] = Point2f(350, 50);
    outputQuad[2] = Point2f(370, 480);
    outputQuad[3] = Point2f(270, 480);

    // Get the Perspective Transform Matrix i.e. lambda
    lambda = getPerspectiveTransform(inputQuad, outputQuad);
    warpPerspective(input, output, lambda, output.size());
    cvtColor(output, output, COLOR_BGR2GRAY);
    threshold(output, output, 200, 255, CV_THRESH_BINARY);
    return output;  
}
int ld::avgX(Mat &window,int whitePixel){
    int s=0;
    for (int i = 0; i < window.cols; ++i)
    {
        for (int j = 0; j < window.rows; ++j)
        {
            if(window.at<uchar>(j,i)==255){
                s+=i;
            }
        }
    }
    return s/whitePixel;
}
void ld::drawCurve(Mat &InputMat,Vec3f &curve){
    for (int i = 1; i < InputMat.rows; ++i)
    {
        line(InputMat,Point(curve[0]+curve[1]*(i-1)+curve[2]*(i-1)*(i-1),i-1),Point(curve[0]+curve[1]*i+curve[2]*i*i,i),Scalar(255,255),3);
    }
}
void ld::findLane(Mat &lane,int laneFollow){
    Mat display;
    cvtColor(lane,display,COLOR_GRAY2BGR);
    int laneSize=150;
    Rect windowSlide=Rect(0,0,80,20);
    int tempXLane=xCenterLane+laneFollow*laneSize/2;
    int diff=0;
    vector<Point2f> lanePoints;
    int footLane=0;
    for(int i=0;i<10;i++){
        Rect curWindow=Rect(tempXLane-windowSlide.width/2,lane.rows-(windowSlide.height*(i+1)),windowSlide.width,windowSlide.height);
        Mat windowMat=lane(curWindow);
        int whitePixel=countNonZero(windowMat);
        if(whitePixel*1.0/(windowSlide.width*windowSlide.height)>0.02&&whitePixel*1.0/(windowSlide.width*windowSlide.height)<0.2){
            diff=(windowSlide.width/2-avgX(windowMat,whitePixel));
            circle(display,Point2f(curWindow.x-diff+windowSlide.width/2,lane.rows-(windowSlide.height*(i+1))+windowSlide.height/2),4,Scalar(0,0,255),-1);
            lanePoints.push_back(Point2f(lane.rows-(windowSlide.height*(i+1))+windowSlide.height/2,curWindow.x-diff+windowSlide.width/2-laneFollow*laneSize/2));
            if(footLane==0) footLane=curWindow.x-diff+windowSlide.width/2;
        }
        curWindow.x-=diff;
        tempXLane-=diff;
        rectangle(display,curWindow,Scalar(255));
        curWindow.x-=laneSize*laneFollow;
        rectangle(display,curWindow,Scalar(255));
    }
    if(footLane!=0) xCenterLane=footLane-laneFollow*laneSize/2;    
    circle(display,Point2f(xCenterLane,lane.rows),10,Scalar(255),-1);
    if(lanePoints.size()>2){
        laneCurve=CurveEstimation(lanePoints);
    }
    drawCurve(display,laneCurve);
    imshow("display",display);
}

