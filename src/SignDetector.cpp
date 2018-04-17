#include "SignDetector.h"
#include "Utilities.h"
#include <queue>

Mat sd::leftSign, sd::rightSign, sd::stopSign;
bool sd::signDetected;
int sd::turn;

void sd::init()
{
    leftSign = imread("left.jpg");
    rightSign = imread("right.jpg");
    stopSign = imread("stop.png");
}

void smoothen(vector<Point> &shape, Point center, int diameter)
{
    vector<int> histogram(diameter);
    for (int i = 1; i < shape.size(); i++)
    {
        int d = (int) utl::distance(center, shape[i]);
        if (d >= diameter) return;
        histogram[d] += 1;
    }

    int max_density = 0;
    for (int i = 0; i < histogram.size(); i++)
        max_density = max(max_density, histogram[i]);

    for (int i = histogram.size() - 1; i >= 0; i--)
        if (histogram[i] < max_density / 2)
            histogram[i] = 0;
        else
            break;

    int remove_count = 0;
    for (int i = 0; i < shape.size(); i++)
        if (histogram[ (int) utl::distance(shape[i], center) ] == 0)
        {
            remove_count++;
            shape[i] = Point(-1, -1);
        }
    if (remove_count > shape.size() / 8)
        shape.clear();

    vector<Point> tmp(shape);
    shape.clear();
    for (int i = 0; i < tmp.size(); i++)
        if (tmp[i] != Point(-1, -1))
            shape.push_back(tmp[i]);
}

void sd::DetectSign(Mat &color, Mat &depth)
{
    GaussianBlur( depth, depth, Size(9, 9), 1, 1 );
    int oo = 9999999;
    int NEIGHBOR_DIFF = 1;
    int MAX_DEPTH = 161, MIN_DIAMETER = 33,
        MIN_DEPTH = 42, MAX_DIAMETER = 120;
    int DIAMETER;
    int dx[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
    int dy[8] = {-1,  0,  1, -1, 1, -1, 0, 1};

    int d[depth.rows][depth.cols];
    queue<Point> q;

    for (int i = 0; i < depth.rows; i++)
        for (int j = 0; j < depth.cols; j++)
        {
            d[i][j] = oo;
        }

    for (int i = 0; i < depth.rows; i++)
        for (int j = 0; j < depth.cols; j++)
        {
            if (d[i][j] != oo) continue;

            d[i][j] = 0;
            int s = 1;
            Point start(i, j), finish(i, j);
            q.push(start);
            vector<Point> currentGroup;
            currentGroup.push_back(start);
            int maxy = -1, miny = oo;
            int perimeter = 0;
            int bottom_border = 0;

            while (!q.empty())
            {
                Point u = q.front();
                maxy = max(maxy, u.y);
                miny = min(miny, u.y);
                finish = u;
                q.pop();
                for (int x = 0; x < 8; x++)
                {
                    Point v = u + Point(dx[x], dy[x]);
                    if ((v.x < 0) || (v.x >= depth.rows) || (v.y < 0) || (v.y >= depth.cols)) continue;
                    if (abs(v.x - start.x) > (maxy - miny))
                    {
                        bottom_border++;
                        continue;
                    }
                    if (d[v.x][v.y] != oo) continue;
                    if (abs(depth.at<uchar>(v.x, v.y) - depth.at<uchar>(u.x, u.y)) > NEIGHBOR_DIFF)
                    {
                        perimeter++;
                        continue;
                    }
                    d[v.x][v.y] = d[u.x][u.y] + 1;
                    q.push(v);
                    currentGroup.push_back(v);
                    s += 1;
                }
            }
            Point center(0, 0);
            int avg_depth = 0;
            for (int i = 0; i < currentGroup.size(); i++)
            {
                center += currentGroup[i];
                avg_depth += depth.at<uchar>(currentGroup[i].x, currentGroup[i].y);
            }
            center = center / (int)currentGroup.size();
            avg_depth = avg_depth / (int)currentGroup.size();

            if ((avg_depth < 20) || (avg_depth > 165)) continue;

            DIAMETER = 5000 / avg_depth;

            if (perimeter > DIAMETER * PI * 5) continue;
            if (bottom_border > DIAMETER * 1.5) continue;

            smoothen(currentGroup, center, DIAMETER);

            for (int i = 0; i < currentGroup.size(); i++)
                if (utl::distance(center, currentGroup[i]) > (DIAMETER * 0.55))
                    s = 0;
            if (abs(s - (DIAMETER * DIAMETER / 4 * PI)) > DIAMETER * DIAMETER / 4 * PI * 0.3) continue;
            // cout << avg_depth << endl;
            for (int i = 0; i < currentGroup.size(); i++)
                currentGroup[i] = Point(currentGroup[i].y, currentGroup[i].x);
            Rect r = boundingRect(currentGroup);
            Mat sign = color(r);
            recognizeSign(sign);
            rectangle(color, r, Scalar(0, 0, 255));
        }
}

int sd::recognizeSign(Mat &sign)
{
    double p_left = similar(sign, leftSign);
    double p_right = similar(sign, rightSign);
    double p_stop = similar(sign, stopSign);
    double Max = max(max(p_left, p_right), p_stop);
    if (Max < 0.5) return 0;
    if (abs(Max - p_left) < 1e-4)
    {
        cout << "left " << p_left << endl;
        return LEFT;
    }
    else
    if (abs(Max - p_right) < 1e-4)
    {
        cout << "right " << p_right << endl;
        return LEFT;
    }
    if (abs(Max - p_stop) < 1e-4)
    {
        cout << "stop " << p_stop << endl;
        return LEFT;
    }
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
