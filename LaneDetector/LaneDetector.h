#include <opencv2/opencv.hpp>
#include <vector>
using namespace std;
using namespace cv;

class LaneDetector
{
public:
    LaneDetector();
    ~LaneDetector();
    void inputImg(const Mat& img);
    void findLane();
    Point laneCenter;
private:
    Mat img;   
};
