#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <windows.h>
using namespace cv;
using namespace std;
class Cam_Hand{
	Cam_Hand(int cID,char *winname);
	~Cam_Hand();
	void Off();
	void On();
	void Str();
	void time();
	void settings();
	int camID;
	int show_flag;
	Mat cam_mat;
	VideoCapture cap;
	char name;
}