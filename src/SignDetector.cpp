#include "SignDetector.h"

Mat sd::leftSign, sd::rightSign, sd::stopSign;
int sd::sign;

void sd::init()
{
    leftSign = imread("img/left.jpg");
    rightSign = imread("img/right.jpg");
    stopSign = imread("img/stop.png");
}

void sd::DetectSign(Mat &color, Mat &depth)
{
    int cols = color.cols;
    int rows = color.rows;
    Mat hsv, gray;
    Rect roiDetect = Rect(int(cols * 0.3), int(rows * 0.1), int(cols * 0.46), int(rows * 0.3));
    rectangle(color, roiDetect, Scalar(0, 0, 255));
    cvtColor(color(roiDetect), hsv, COLOR_BGR2HSV);
    int minH = 50, minS = 100, minV = 40,
        maxH = 135, maxS = 255, maxV = 255;
    Scalar min = Scalar(minH, minS, minV);   //HSV VALUE
    Scalar max = Scalar(maxH, maxS, maxV); //HSV VALUE
    inRange(hsv, min, max, gray);
    // imshow("HSV",gray);
    erode(gray, gray, Mat(), Point(-1, -1), 2, 1, 1);
    dilate(gray, gray, Mat(), Point(-1, -1), 8, 1, 1);
    // imshow("dilate",gray);
    vector<vector<Point>> contours;
    findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    ushort minDistance = 30000;
    sign = NO_SIGN;
    for (int i = 0; i < contours.size(); i++)
    {
        Rect rect = boundingRect(contours[i]);
        if (abs((rect.width * 1.0 / rect.height) - 1) > 0.2) continue;
        rect.x += roiDetect.x + 6;
        rect.y += roiDetect.y + 6;
        rect.width -= 6 * 2;
        rect.height -= 6 * 2;
        Point center(rect.x + rect.width / 2, rect.y + rect.height / 2);
        int radius = rect.height / 2;
        int RADIUS = 48000;
        float HEIGHT = 265;
        ushort distance = depth.at<ushort>(center);
        if (abs(RADIUS - radius * distance) > RADIUS / 10) continue;
        // cout << radius << " : " << distance << endl;

        Point3f p = utl::getRealPointInWorld(center, distance);
        float signHeight = utl::dToPlane(p, utl::groundPlane);
        if (abs(signHeight - HEIGHT) > 30) continue;
        cout << "height:" << signHeight << " ";

        rectangle(color, rect, Scalar(0, 0, 255));
        Mat matsign = color(rect);
        resize(matsign, matsign, Size(rect.height, rect.height));
        imshow("matsign", matsign);

        int tmp = recognizeSign(matsign);
        if (tmp == NO_SIGN) continue;
        if (distance > minDistance) continue;
        sign = tmp;
        minDistance = distance;
    }
    if (sign == LEFT) cout << "left" << endl;
    else if (sign == RIGHT) cout << "right" << endl;
    else if (sign == STOP) cout << "stop" << endl;
}

int sd::recognizeSign(Mat &sign)
{
    double p_left = similar(sign, leftSign);
    double p_right = similar(sign, rightSign);
    double p_stop = similar(sign, stopSign);
    double p_max = max(p_left, max(p_right, p_stop));

    if (p_max < 0.5) return NO_SIGN;
    cout << p_max << " ";
    if (p_max == p_left) return LEFT;
    if (p_max == p_right) return RIGHT;
    if (p_max == p_stop) return STOP;
}

double sd::distance(Point p1, Point p2)
{
    return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

void sd::equalizeHistBGR(Mat &src, Mat &dst)
{
    if (src.channels() >= 3)
    {
        Mat ycrcb;

        cvtColor(src, ycrcb, CV_BGR2YUV);

        vector<Mat> channels;
        split(ycrcb, channels);

        equalizeHist(channels[0], channels[0]);

        Mat result;
        merge(channels, ycrcb);

        cvtColor(ycrcb, dst, CV_YUV2BGR);
    }
}

double sd::similar(Mat &img1, Mat &img2)
{
    Mat hsv1, hsv2;
    resize(img1, hsv1, img1.size());
    equalizeHistBGR(hsv1, hsv1);
    cvtColor(hsv1, hsv1, COLOR_BGR2HSV);
    resize(img2, hsv2, hsv1.size());
    cvtColor(hsv2, hsv2, COLOR_BGR2HSV);
    int d = 0, s = 0;
    for (size_t i = 0; i < hsv1.cols; i++)
    {
        for (size_t j = 0; j < hsv1.rows; j++)
        {
            if (distance(Point(i, j), Point(hsv1.cols / 2, hsv1.rows / 2)) <= hsv1.rows / 2)
            {
                s++;
                if (hsv1.at<Vec3b>(j, i)[1] > 70 && hsv1.at<Vec3b>(j, i)[2] > 30)
                {
                    if (abs(hsv2.at<Vec3b>(j, i)[0] - hsv1.at<Vec3b>(j, i)[0]) < 15)
                    {
                        d++;
                    }
                }
                else
                {
                    if (abs(hsv2.at<Vec3b>(j, i)[2] - hsv1.at<Vec3b>(j, i)[2]) < 10)
                    {
                        d++;
                    }
                }
            }
        }
    }
    return (d * 1.0 / s);
}
