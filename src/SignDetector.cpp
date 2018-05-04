#include "SignDetector.h"
#include "Utilities.h"
#include <queue>

Mat sd::leftSign, sd::rightSign, sd::stopSign;
int sd::sign;

void sd::init()
{
    leftSign = imread("img/left.jpg");
    rightSign = imread("img/right.jpg");
    stopSign = imread("img/stop.png");
}

bool isBlue(Scalar color)
{
    int minH = 50, minS = 100, minV = 40,
        maxH = 135, maxS = 255, maxV = 255;
    if ((color[0] < minH) || (color[0] > maxH)) return false;
    if ((color[1] < minS) || (color[1] > maxS)) return false;
    if ((color[2] < minV) || (color[2] > maxV)) return false;
    return true;
}

void sd::DetectSign(Mat &color, Mat &depth)
{
    Mat hsv, range1, range2, range3;
    cvtColor(color, hsv, COLOR_BGR2HSV);
    
    int minS = 100, maxS = 255,
        minV = 50, maxV = 255,
        minH_1 = 50, maxH_1 = 135,
        minH_2 = 0, maxH_2 = 10,
        minH_3 = 170, maxH_3 = 180;

    Scalar min1 = Scalar(minH_1, minS, minV);   //HSV VALUE
    Scalar max1 = Scalar(maxH_1, maxS, maxV); //HSV VALUE
    Scalar min2= Scalar(minH_2, minS, minV) ;
    Scalar max2= Scalar(maxH_2, maxS, maxV);
    Scalar min3= Scalar(minH_3, minS, minV);
    Scalar max3= Scalar(maxH_3, maxS, maxV);
    inRange(hsv, min1, max1, range1);
    inRange(hsv, min2, max2, range2);
    inRange(hsv, min3, max3, range3);
    Mat gray = range1 | range2 | range3;
    imshow("HSV",gray);
    erode(gray, gray, Mat(), Point(-1, -1), 2, 1, 1);
    dilate(gray, gray, Mat(), Point(-1, -1), 8, 1, 1);
    imshow("dilate",gray);

    int oo = 9999999;
    int NEIGHBOR_DIFF = 7;
    int RADIUS = 48000;
    int dx[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
    int dy[8] = {-1,  0,  1, -1, 1, -1, 0, 1};

    int d[depth.cols][depth.rows];
    queue<Point> q;

    for (int i = 0; i < depth.cols; i++)
        for (int j = 0; j < depth.rows; j++)
        {
            d[i][j] = oo;
        }

    for (int i = 0; i < depth.cols; i++)
        for (int j = 0; j < depth.rows; j++)
        {
            Point start(i, j);
            if (!isBlue(hsv.at<Vec3b>(start))) d[i][j] = 0;
            if (depth.at<ushort>(start) == 0) d[i][j] = 0;
            if (d[i][j] != oo) continue;
            d[i][j] = 0;
            q.push(start);
            Point p_max(start), p_min(start);

            while (!q.empty())
            {
                Point u = q.front();
                q.pop();
                for (int x = 0; x < 8; x++)
                {
                    Point v = u + Point(dx[x], dy[x]);
                    if ((v.x < 0) || (v.x >= depth.cols) || (v.y < 0) || (v.y >= depth.rows)) continue;
                    if (d[v.x][v.y] != oo) continue;
                    if (abs(depth.at<ushort>(v) - depth.at<ushort>(u)) > NEIGHBOR_DIFF) continue;
                    if (!isBlue(hsv.at<Vec3b>(v))) continue;
                    d[v.x][v.y] = d[u.x][u.y] + 1;
                    q.push(v);
                    p_max.x = max(p_max.x, v.x);
                    p_max.y = max(p_max.y, v.y);
                    p_min.x = min(p_min.x, v.x);
                    p_min.y = min(p_min.y, v.y);
                }
            }
            Point center = Point(p_max.x + p_min.x, p_max.y + p_min.y) / 2;
            int distance = depth.at<ushort>(center);
            int radius = (p_max.x - p_min.x) / 2;

            if (abs(distance * radius - RADIUS) > RADIUS / 10) continue;
            // cout << avg_depth << endl;
            Rect r = Rect(p_min, p_max);
            if (abs(r.height * 1.0 / r.width - 1) > 0.2) continue;
            // Mat sign = color(r);
            // recognizeSign(sign);
            rectangle(color, r, Scalar(0, 0, 255));
        }
}

int sd::recognizeSign(Mat &sign)
{
    double p_left = similar(sign, leftSign);
    double p_right = similar(sign, rightSign);
    double p_stop = similar(sign, stopSign);
    double p_max = max(p_left, max(p_right, p_stop));

    if (p_max < 0.5) return NO_SIGN;
    // cout << p_max << " ";
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
    if (img1.empty())
    {
        // cout << "empty" << endl;
        return 0;
    }
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
