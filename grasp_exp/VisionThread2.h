#pragma once
#include "flycapture2.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <direct.h>
#include "GL/gl.h"
#include "GL/glu.h"
#include <GL/glut.h>
#include "FrameRateCounter.h"
using namespace FlyCapture2;
void InitBitmapStruct( int cols, int rows );
void printerror( Error error );
int GetMinimumPowerOfTwo(int in);
void DrawaString(char *str, int x, int y,float scale_f[3],float color_rec[3],float color_str[3]);
void DrawScene(int nCam);
void GlutDisplayFunc();
void* visionrun2(void*);
void vsScreenSize (bool fullscreen);
void DrawaString2(char *str, double x, double y);
void vsCamOff(double offX[2],double offY[2]);
