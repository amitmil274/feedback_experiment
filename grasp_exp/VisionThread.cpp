#include "FileHandle.h"
#include "FlyCapture2.h"

#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
//extern bool WriteToFile;
#include "protocol.h"
extern int status;
extern int graspStatus;
extern bool SimulationOn;
using namespace FlyCapture2;
using namespace cv;
void PrintError( Error error )
{
	error.PrintErrorTrace();
}
void* visionrun(void* pUserData)
{
	Error error;
	const int k_numImages = 100;
	int flag_full=0;

	BusManager busMgr;
	unsigned int numCameras;
	error = busMgr.GetNumOfCameras(&numCameras);
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return false;
	}
	cvNamedWindow("heyimage",CV_NORMAL);//,WINDOW_OPENGL);//CV_NORMAL);
    //cvSetWindowProperty("heyimage", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	std::cout << "Number of cameras detected: " << numCameras << std::endl; 

	if ( numCameras < 1 )
	{
		std::cout << "Insufficient number of cameras... press Enter to exit." << std::endl; ;
		std::cin.ignore();
		return false;
	}
	CameraInfo camInfo;
	Camera** camera = new Camera*[2];
	// Connect the camera
	for (int i=0;i<2;i++)
	{
		camera[i] = new Camera();

		PGRGuid guid;
		error = busMgr.GetCameraFromIndex( i, &guid );
		if (error != PGRERROR_OK)
		{
			PrintError( error );
			return false;
		}
		error = camera[i]->Connect(  &guid );
		if ( error != PGRERROR_OK )
		{
			std::cout << "Failed to connect to camera" << std::endl;     
			return false;
		}

		// Get the camera info and print it out
		error = camera[i]->GetCameraInfo( &camInfo );
		if ( error != PGRERROR_OK )
		{
			std::cout << "Failed to get camera info from camera" << std::endl;     
			return false;
		}
		std::cout << camInfo.vendorName << " "
			<< camInfo.modelName << " " 
			<< camInfo.serialNumber << std::endl;

		error = camera[i]->StartCapture();
		if ( error == PGRERROR_ISOCH_BANDWIDTH_EXCEEDED )
		{
			std::cout << "Bandwidth exceeded" << std::endl;     
			return false;
		}
		else if ( error != PGRERROR_OK )
		{
			std::cout << "Failed to start image capture" << std::endl;     
			return false;
		} 
	}
	// capture loop
	char key = 0;
	int rows;
	int cols;
	cv::Size s;
		int fontFace = cv::FONT_HERSHEY_COMPLEX;
			double fontScale = 3;
			int thickness = 2;  

	for(int i=0;i<2;i++)
	{		
		Image rawImage;
		Error error = camera[i]->RetrieveBuffer( &rawImage );
		if ( error != PGRERROR_OK )
		{
			std::cout << "capture error" << std::endl;
			continue;
		}
		Image rgbImage;
		rawImage.Convert( FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage );
		// convert to OpenCV Mat
		unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize()/(double)rgbImage.GetRows();       
		rows=rgbImage.GetRows();
		cols=rgbImage.GetCols();
		cv::Mat  image = cv::Mat(rows, cols , CV_8UC3, rgbImage.GetData(),rowBytes);
		s = image.size();		
	}
	cv::Point textOrg(cols/2.5,100);
	int flag_text=0;
	while(SimulationOn)
	{
		cv::Mat blah;
		// Get the image
		switch (status)
		{
		case 0:

			for(int i=0;i<2;i++)
			{		

				Image rawImage;
				Error error = camera[i]->RetrieveBuffer( &rawImage );
				if ( error != PGRERROR_OK )
				{
					std::cout << "capture error" << std::endl;
					continue;
				}

				// convert to rgb	    
				Image rgbImage;
				rawImage.Convert( FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage );
				// convert to OpenCV Mat
				unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize()/(double)rgbImage.GetRows();       
				//rows=rgbImage.GetRows();
				//cols=rgbImage.GetCols();
				cv::Mat  image = cv::Mat(rows, cols , CV_8UC3, rgbImage.GetData(),rowBytes);
				//s = image.size();		
				blah.push_back(image);
			}
			cv::resize(blah, blah, s);
			//	blah= cv::Mat::ones(rows,cols,CV_8U);;
			cv::imshow("heyimage", blah);
			key = cv::waitKey(1);     
			if (key==92)
			{
				if(flag_full==0)
				{
				 cvSetWindowProperty("heyimage", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
				flag_full=1;
				}
				else
				{
					cvSetWindowProperty("heyimage", CV_WND_PROP_AUTOSIZE, CV_WINDOW_AUTOSIZE);
					flag_full=0;
				}
			}
            break;
		case 1:
		case 2:
			cv::Mat img(rows,cols,CV_8UC3,cv::Scalar(0,0,0));
			cv::Mat img2(rows,cols,CV_8UC3,cv::Scalar(0,0,0));
			Size textSize;
			int baseline=0;
			string text;
			switch (graspStatus)
			{
			case 0:
				text = "Reset Position";
				textSize = getTextSize(text, fontFace,fontScale, thickness, &baseline);
				baseline += thickness;
				cv::rectangle(img, textOrg + Point(0, thickness),textOrg + Point(textSize.width, -textSize.height),	Scalar(0,0,255),-1);
				cv::rectangle(img2, textOrg + Point(0, thickness),textOrg + Point(textSize.width, -textSize.height),	Scalar(0,0,255),-1);
				cv::putText(img, text, textOrg, fontFace, fontScale, cv::Scalar::all(255), thickness,8);
				cv::putText(img2, text, textOrg, fontFace, fontScale, cv::Scalar::all(255), thickness,8);
				img.push_back(img2);
				cv::resize(img, img, s);
				cv::imshow("heyimage", img);
				key = cv::waitKey(1);        
				break;
			case 1:
				text = "Click when Ready";
				textSize = getTextSize(text, fontFace,fontScale, thickness, &baseline);
				baseline += thickness;
				cv::rectangle(img, textOrg + Point(0, thickness),textOrg + Point(textSize.width, -textSize.height),	Scalar(0,0,255),-1);
				cv::rectangle(img2, textOrg + Point(0, thickness),textOrg + Point(textSize.width, -textSize.height),	Scalar(0,0,255),-1);
				cv::putText(img, text, textOrg, fontFace, fontScale, cv::Scalar::all(255), thickness,8);
				cv::putText(img2, text, textOrg, fontFace, fontScale, cv::Scalar::all(255), thickness,8);
				img.push_back(img2);
				cv::resize(img, img, s);
				cv::imshow("heyimage", img);
				key = cv::waitKey(1);        
				break;
			case 2: case 3:
				for(int i=0;i<2;i++)
				{		
					Image rawImage;
					Error error = camera[i]->RetrieveBuffer( &rawImage );
					if ( error != PGRERROR_OK )
					{
						std::cout << "capture error" << std::endl;
						continue;
					}   
					Image rgbImage;
					rawImage.Convert( FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage );
					unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize()/(double)rgbImage.GetRows();            
					cv::Mat  image = cv::Mat(rows, cols , CV_8UC3, rgbImage.GetData(),rowBytes);
					blah.push_back(image);
				}
				cv::resize(blah, blah, s);
				img = blah;
				Scalar rec_color;
								if(graspStatus==2)
								{
									rec_color=Scalar (0,0,255);
									text = "Set";
								}
								else
								{
									rec_color=Scalar(0,255,0);
									text = "Go";
								}
				textSize = getTextSize(text, fontFace,fontScale, thickness, &baseline);
				baseline += thickness;
				cv::rectangle(img, textOrg + Point(0, thickness),textOrg + Point(textSize.width, -textSize.height),	rec_color,-1);
				cv::putText(blah, text, textOrg, fontFace, fontScale, cv::Scalar::all(255), thickness,8);
				cv::imshow("heyimage", blah);
				key = cv::waitKey(1);        
				break;
			}
		}

	}	

	for (int i=0;i<2;i++)
	{
		//error = camera[i]->StopCapture();
		 camera[i]->StopCapture();
		//if ( error != PGRERROR_OK )
		//{
			//std::cout<<"hello";// This may fail when the camera was removed, so don't show 
			// an error message
		//}  

		camera[i]->Disconnect();
	} 
	return 0;
}
//int main(int argc, char *argv[])
//{
//    help();
//
//    if (argc != 4)
//    {
//        cout << "Not enough parameters" << endl;
//        return -1;
//    }
//
//    const string source      = argv[1];           // the source file name
//    const bool askOutputType = argv[3][0] =='Y';  // If false it will use the inputs codec type
//
//    VideoCapture inputVideo(source);              // Open input
//    if (!inputVideo.isOpened())
//    {
//        cout  << "Could not open the input video: " << source << endl;
//        return -1;
//    }
//
//    string::size_type pAt = source.find_last_of('.');                  // Find extension point
//    const string NAME = source.substr(0, pAt) + argv[2][0] + ".avi";   // Form the new name with container
//    int ex = static_cast<int>(inputVideo.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form
//
//    // Transform from int to char via Bitwise operators
//    char EXT[] = {s(char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0};
//
//    Size S = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
//                  (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));
//
//    VideoWriter outputVideo;                                        // Open the output
//    if (askOutputType)
//        outputVideo.open(NAME, ex=-1, inputVideo.get(CV_CAP_PROP_FPS), S, true);
//    else
//        outputVideo.open(NAME, ex, inputVideo.get(CV_CAP_PROP_FPS), S, true);
//
//    if (!outputVideo.isOpened())
//    {
//        cout  << "Could not open the output video for write: " << source << endl;
//        return -1;
//    }
//
//    if(WriteToFile) //Show the image captured in the window and repeat
//    {
//		outputVideo.write(blah); //save or
//      // outputVideo << res;
//    }
//
//    cout << "Finished writing" << endl;
//    return 0;
//}