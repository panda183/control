#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <cmath>
#include "Driver.h"
#include "Utilities.h"
#include "OpenNI.h"
// test push
using namespace std;
using namespace cv;
//Client int
//create a message buffer
char msg[100000];
int clientSd;

bool ConnectToServer(char *serverIp,int port)
{
    //setup a socket and connection tools
    struct hostent* host = gethostbyname(serverIp);
    sockaddr_in sendSockAddr;
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    clientSd = socket(AF_INET, SOCK_STREAM, 0);
    //try to connect...
    int status = connect(clientSd, (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0)
    {
        cout<<"Error connecting to socket!"<<endl; return false;
    }
    cout << "Connected to the server!" << endl;
    return true;
}

Mat GetImageFromServer()
{
    memset(&msg, 0, sizeof(msg));//clear the buffer
    recv(clientSd, (char*)&msg, sizeof(msg), 0);
    vector<char> arr(msg, msg + sizeof(msg) - 1);
    Mat decodedImage= imdecode(arr, CV_LOAD_IMAGE_COLOR);
    return decodedImage;
}

void SendDataToServer(float torque,float angle)
{
    memset(&msg, 0, sizeof(msg));//clear the buffer
    string data = to_string(torque) + "|" + to_string(angle);
    send(clientSd, data.c_str(), data.length(), 0);
}

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        utl::openni2_init();
    }
    else
    {
        char *serverIp = argv[1]; int port = atoi(argv[2]);

        if(!ConnectToServer(serverIp,port))
        {
            return 0;
        }
    }

    Driver driver;

    while (true)
    {
        Mat color, depth;
        if (argc == 3)
            color = GetImageFromServer();
        else
            utl::openni2_getmat(color, depth);
        //xu ly anh img
        if (color.empty() || depth.empty())
            continue;

        Point carPosition(color.cols / 2, color.rows);

        driver.inputImg(color, depth);

        line(color, driver.target, carPosition, Scalar(255, 255, 255));

        Mat adjMap;
        convertScaleAbs(depth, adjMap, 255.0 / 6000);

        imshow("color", color);
        imshow("depth", adjMap);
        if (waitKey(1) == 27)
            break;
        if (argc == 3) SendDataToServer(18, driver.getSteering());
    }

    if (argc == 3)
        close(clientSd);
    else
        utl::openni2_destroy();

    return 0;
}
