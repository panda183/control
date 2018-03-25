#include "SignDetector.h"

Mat sd::leftSign, sd::rightSign;

void sd::init()
{
    leftSign = imread("left.jpg");
    rightSign = imread("right.jpg");
}

side sd::DetectSign(Mat &src)
{
    int cols = src.cols;
    int rows = src.rows;
    Mat hsv, gray;
    Rect roiDetect = Rect(int(cols * 0.3), int(rows * 0.1), int(cols * 0.46), int(rows * 0.3));
    rectangle(src, roiDetect, Scalar(0, 0, 255));
    cvtColor(src(roiDetect), hsv, COLOR_BGR2HSV);
    int minH=100, minS=100, minV= 60, maxH=135, maxS=255, maxV=255;
    Scalar min = Scalar(minH, minS, minV);   //HSV VALUE
    Scalar max = Scalar(maxH, maxS, maxV); //HSV VALUE
    inRange(hsv, min, max, gray);

    erode(gray, gray, Mat(), Point(-1, -1), 2, 1, 1);
    dilate(gray, gray, Mat(), Point(-1, -1), 8, 1, 1);
    //imshow("HSV",gray);
    vector<vector<Point>> contours;
    findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    for (int i = 0; i < contours.size(); i++)
    {
        Rect rect = boundingRect(contours[i]);
        if (abs((rect.width * 1.0 / rect.height) - 1) < 0.1 && rect.width - 6 > int(cols * 0.06) && rect.width - 6 < int(cols * 0.125))
        {
            rect.x += roiDetect.x + 6;
            rect.y += roiDetect.y + 6;
            rect.width -= 6 * 2;
            rect.height -= 6 * 2;
            rectangle(src, rect, Scalar(0, 0, 255));
            Mat matsign = src(rect);

            imshow("src", src);

            if (recognizeSign(matsign))
                return r;
            else 
                return l;
        }
    }
}

int sd::recognizeSign(Mat &sign)
{
    double p1 = similar(sign, leftSign);
    double p2 = similar(sign, rightSign);
    if (p1 > p2)
        return 0;
    return 1;
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
