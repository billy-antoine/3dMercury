#include "opencv2/opencv.hpp"
#include "config.h"
#include <ctime>
#include <iostream>
#include <cstdlib>


using namespace cv;
using namespace std;

std::string getTime(){
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%I:%M", timeinfo);
    std::string str(buffer);
    return str;
}

std::string getDay(){
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m", timeinfo);
    std::string str(buffer);
    return str;
}

int main(int, char**)
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened()){  // check if we succeeded
	std::cout << "Camera not found" << std::endl;
        return -1;
    }

    // Redefine camera resolution
    cap.set(CV_CAP_PROP_FRAME_WIDTH,  1920);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);

    namedWindow("3D Mercury",1);
    Mat splash;
    splash = imread(THIS_SOURCE_DIR+"graphics/splash.png",1);
    imshow("3D Mercury", splash);

    for(;;){
        char c = waitKey(30);
        if(c>=0)
            break;
    }

    // Building folders
    std::string folder_day   = THIS_SOURCE_DIR+"Results/"+getDay();
    std::string folder_time = folder_day+"/"+getTime();

    std::system(("cd "    +THIS_SOURCE_DIR+"Results/").c_str());
    std::system(("mkdir "+folder_day).c_str()); 
    std::system(("mkdir "+folder_time).c_str()); 
    std::system(("mkdir "+folder_time+"/images").c_str());

    int nb_images=0;
    Mat frame, tmp;
    for(;;)
    {
        cap >> frame; // get a new frame from camera

        imshow("3D Mercury", frame);

        char c = waitKey(30);
        if(c==32)
            break;
        if(c>=0){
            bitwise_not(frame, tmp);
            imshow("3D Mercury", tmp);
            bilateralFilter(frame,tmp,5,80,80);
            imwrite(folder_time+"/images/frame"+std::to_string(nb_images++)+".png", tmp);
            waitKey(50);
        }
    }
   
    frame = imread(THIS_SOURCE_DIR+"loader/wait10.png",1);
    imshow("3D Mercury", frame);
    waitKey(20);

    destroyAllWindows();

    std::system(("cd "+THIS_SOURCE_DIR+"Results/ && ./Global_SfM_pipeline.sh " + folder_time + " " + MVG_BUILD_PATH + " " + MVS_BUILD_PATH + " " + MVG_PATH).c_str());

    for(int i=0; i<100; ++i){

        Mat tmp = frame.clone();
        rectangle(tmp, Point(i*(frame.cols/100),0), Point(frame.cols,frame.rows), Scalar(46,200,244), -1);
        imshow("3D Mercury", tmp );
        waitKey(10);
    }
    
    destroyAllWindows();

    std::system(("cd "+THIS_SOURCE_DIR+"Results/ && " + folder_time + "/Viewer scene_dense.mvs").c_str());
    waitKey(0);


    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
