#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <windows.h>
#include "cam_hand.h"
using namespace cv;
using namespace std;
Cam_Hand::Cam_Hand(int cID,char* winname)
{
	if ( !this->cap.isOpened() )  // if not success, exit program
    {
         cout << "Cannot open the web cam" << endl;     //  return -1;
    }
	cvNamedWindow(winname, CV_WINDOW_NORMAL);
	strcpy(&this->name,winname);
	this->camID=cID;
}
Cam_Hand::~Cam_Hand()
{
	this->cap.release();
}
void Cam_Hand::settings()
{
	this->cap.set(CV_CAP_PROP_SETTINGS, 1);
}


 Mat imgTmp;
 cap.read(imgTmp); 

  //Create a black image with the size as the camera output
 Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );;
 

    while (true)
    {
        Mat imgOriginal;

        bool bSuccess = cap.read(imgOriginal); // read a new frame from video



         if (!bSuccess) //if not success, break loop
        {
             cout << "Cannot read a frame from video stream" << endl;
             break;
        }

  //  Mat imgHSV;

  // cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
 
  //Mat imgThresholded;

  // inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
  //    
  ////morphological opening (removes small objects from the foreground)
  //erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
  //dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

  // //morphological closing (removes small holes from the foreground)
  //dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
  //erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

  // //Calculate the moments of the thresholded image
  //Moments oMoments = moments(imgThresholded);

  // double dM01 = oMoments.m01;
  //double dM10 = oMoments.m10;
  //double dArea = oMoments.m00;

  // // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
  //if (dArea > 10000)
  //{
  // //calculate the position of the ball
  // int posX = dM10 / dArea;
  // int posY = dM01 / dArea;        
  //      
  // if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
  // {
  //  //Draw a red line from the previous point to the current point
  //  line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0,0,255), 2);
  // }

  //  iLastX = posX;
  // iLastY = posY;
  //}

  // imshow("Thresholded Image", imgThresholded); //show the thresholded image

  // imgOriginal = imgOriginal + imgLines;
  imshow("Name", imgOriginal); //show the original image

        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
       {
            cout << "esc key is pressed by user" << endl;
            break; 
       }
    }

   return 0;
}