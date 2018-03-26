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
#include "Driver/Driver.h"
#include "SignDetector/SignDetector.h"

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
    // return imread("1.jpg", CV_LOAD_IMAGE_COLOR); 
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
        cerr << "Usage: ip_address port" << endl; exit(0); 
    }
    char *serverIp = argv[1]; int port = atoi(argv[2]); 
    
    if(!ConnectToServer(serverIp,port))
    {
        return 0;
    }
    sd::init();
    Driver driver;
    driver.setHug(LEFT);
    do
    {
        Mat img = GetImageFromServer();
        //xu ly anh img
        if (img.empty())
            continue;

        Point carPosition(img.cols / 2, img.rows);

        driver.inputImg(img);
        double angle = driver.getSteering();
        
        line(img, driver.target, carPosition, Scalar(255, 255, 255));

        if (!img.empty()) imshow("src",img);
        waitKey(1);
        SendDataToServer(10, angle);
    } while (1);

    close(clientSd);
    return 0;    
}
