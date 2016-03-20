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
#include "VisionThread2.h"
using namespace FlyCapture2;
using namespace std;
static const unsigned int sk_maxNumTextures = 36;
GLuint m_imageTextures[sk_maxNumTextures];
FrameRateCounter FPS;
Camera** camera = new Camera*[2];
PGRGuid guid[2];
Format7ImageSettings f7set[2] ;
Format7Info f7info[2];
bool supported[2];
BITMAPINFO        m_bitmapInfo;  
bool glutstarted=false;
extern int status;
extern int graspStatus;
bool pauseCam = false;
bool usingcam = false;
double baseoffx, baseoffy,stepoffx,stepoffy;
void InitBitmapStruct( int cols, int rows )
{
	BITMAPINFOHEADER* pheader = &m_bitmapInfo.bmiHeader;
	// Initialize permanent data in the bitmapinfo header.
	pheader->biSize          = sizeof( BITMAPINFOHEADER );
	pheader->biPlanes        = 1;
	pheader->biCompression   = BI_RGB;
	pheader->biXPelsPerMeter = 100;
	pheader->biYPelsPerMeter = 100;
	pheader->biClrUsed       = 0;
	pheader->biClrImportant  = 0;

	// Set a default window size.
	pheader->biWidth    = cols;
	pheader->biHeight   = -rows;
	pheader->biBitCount = 32;

	m_bitmapInfo.bmiHeader.biSizeImage = 0;
}
void printerror( Error error )
{
	error.PrintErrorTrace();

}
int GetMinimumPowerOfTwo(int in)
{
	int i = 1;
	while ( i < in)
	{
		i *= 2;
	}

	return i;
}
void DrawaString(char *str, int x, int y,float scale_f[3],float color_rec[3],float color_str[3])
{
	void* font_face = GLUT_BITMAP_TIMES_ROMAN_24;
	void* font_face2 = GLUT_STROKE_ROMAN ;
	int font_height = 16;
	int wy;
	int width=1600, height=900;
	int strw = glutStrokeLength(font_face2,(unsigned char*) str)+10;
	int strh=150;
	int recoff=-30;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0f, (float)width, 0.0f, (float)height);
	glTranslatef(x, y,0.0f);
	glScalef(scale_f[0],scale_f[1],scale_f[2]);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(color_rec[0],color_rec[1],color_rec[2]);
	glBegin(GL_QUADS);
	glVertex2f(recoff,recoff);
	glVertex2f(recoff,strh);
	glVertex2f(strw,strh);
	glVertex2f(strw,recoff);
	glEnd();
	//wy = height - y - font_height;
	glColor3f(color_str[0],color_str[1],color_str[2]);
	//glRasterPos2i(x,wy);
	for(; *str !=0; str++)
	{
		if(*str == '\n')
		{
			//	wy-=font_height;
			//	glRasterPos2i(x,wy);
			continue;
		}
		//glutBitmapCharacter(font_face, *str);
		glutStrokeCharacter(font_face2, *str);
	}	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	//glEnable(GL_DEPTH_TEST | GL_LIGHTING);
}
void DrawaString2(char *str, double x, double y)
{
	void* font_face = GLUT_BITMAP_TIMES_ROMAN_24;
	void* font_face2 = GLUT_BITMAP_HELVETICA_18 ;
	int font_height = 16;
	int wy;
	int width, height;

	width = glutGet((GLenum)GLUT_WINDOW_WIDTH);
	height = glutGet((GLenum)GLUT_WINDOW_HEIGHT);

	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	/*wy = height - y - font_height;
	glRasterPos2d(x, wy);*/
	for(; *str !=0; str++)
	{
		if(*str == '\n')
		{
			wy -= font_height;
			glRasterPos2d(x, y);
			continue;
		}

		/*	glutBitmapCharacter(font_face, *str);*/
		glutStrokeCharacter(font_face2, *str);
	}
	glPopMatrix();
	//glMatrixMode(GL_PROJECTION);
	//glPopMatrix();
	//glMatrixMode(GL_MODELVIEW);
	//glPopMatrix();

	glEnable(GL_DEPTH_TEST);
}

void DrawScene(int nCam)
{
	double validTextureWidth = 1.0;
	double validTextureHeight = 1.0;
	GLenum errorno;
	int width = m_bitmapInfo.bmiHeader.biWidth;
	int height = ::abs( m_bitmapInfo.bmiHeader.biHeight );
	glEnable( GL_TEXTURE_2D );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glBindTexture( GL_TEXTURE_2D, m_imageTextures[0] );

	if (!pauseCam)
	{
		int i=nCam-1; 
		usingcam=true;
		Image rawimage;
		Error error = camera[i]->RetrieveBuffer( &rawimage );
		if ( error != PGRERROR_OK )
		{
			std::cout << "capture error" << std::endl;
		}
		Image rgbimage;
		rawimage.Convert( FlyCapture2::PIXEL_FORMAT_BGR, &rgbimage );	

		const int textureWidth = GetMinimumPowerOfTwo(width);
		const int textureHeight = GetMinimumPowerOfTwo(height);
		validTextureWidth = (double)width / textureWidth;
		validTextureHeight = (double)height / textureHeight;			
		glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			GL_RGB, 
			textureWidth, 
			textureHeight, 
			0, 
			GL_BGR_EXT, 
			GL_UNSIGNED_BYTE,
			NULL );
		errorno = glGetError();

		glTexSubImage2D( 
			GL_TEXTURE_2D, 
			0, 
			0, 
			0,
			width, 
			height,
			GL_BGR_EXT, 
			GL_UNSIGNED_BYTE, 
			rgbimage.GetData());
		errorno = glGetError();
		if (graspStatus<2  && status!=0)
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();
		}
		else
		{
			glBegin( GL_QUADS );
			glTexCoord2d( 0.0, validTextureHeight );
			glVertex2d( 0.0+i*0.5, 0.0 );

			glTexCoord2d( validTextureWidth, validTextureHeight );
			glVertex2d( 0.5+i*0.5, 0.0 );

			glTexCoord2d( validTextureWidth, 0.0 );
			glVertex2d( 0.5+i*0.5, 1.0 );

			glTexCoord2d( 0.0, 0.0 );
			glVertex2d( 0.0+i*0.5, 1.0 );
			glEnd();
		}
		glDisable( GL_TEXTURE_2D );
		usingcam=false;
	}
	}


void GlutDisplayFunc()
{
	float scale_f[3]={0.35,0.35,0.35};
	float color_str[3]={0,0,0};
	float color_rec[3]={0,0,0};
	glutPostRedisplay();
	{
		glDrawBuffer(GL_BACK);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		DrawScene(1);
		glPopMatrix();
		glPushMatrix();
		DrawScene(2);
		glPopMatrix();
		glPushMatrix();
		switch (status)
		{
		case 0:
			color_rec[0]=1;
			DrawaString("Calibrate and Click when done",50,400,scale_f,color_rec,color_str);
			DrawaString("Calibrate and Click when done",850,400,scale_f,color_rec,color_str);
			break;
		case 1:
			switch (graspStatus)
			{
			case 0:
				color_rec[0]=1;
				DrawaString("Click when you ready",50,400,scale_f,color_rec,color_str);
				DrawaString("Click when you ready",850,400,scale_f,color_rec,color_str);
				break;
			case 1:
				color_rec[0]=1;
				DrawaString("get ready to grasp",50,400,scale_f,color_rec,color_str);
				DrawaString("get ready to grasp",850,400,scale_f,color_rec,color_str);
				break;
			case 2:
				color_rec[0]=1;
				DrawaString("SET",300,800,scale_f,color_rec,color_str);
				DrawaString("SET",1100,800,scale_f,color_rec,color_str);
				break;
			case 3:
				color_rec[1]=1;
				DrawaString("GO",300,800,scale_f,color_rec,color_str);
				DrawaString("GO",1100,800,scale_f,color_rec,color_str);
				break;
			}
			break;
		case 2:
			switch (graspStatus)
			{
			case 0:
				color_rec[0]=1;
				DrawaString("Click when you ready",50,400,scale_f,color_rec,color_str);
				DrawaString("Click when you ready",850,400,scale_f,color_rec,color_str);
				break;
			case 1:
				color_rec[0]=1;
				DrawaString("get ready",50,400,scale_f,color_rec,color_str);
				DrawaString("get ready",850,400,scale_f,color_rec,color_str);
				break;
			case 2:
				color_rec[0]=1;
				DrawaString("Show Grasp and press Space",50,800,scale_f,color_rec,color_str);
				DrawaString("Show Grasp and press Space",850,800,scale_f,color_rec,color_str);
				break;
			case 3:
				color_rec[1]=1;
				DrawaString("GO",300,800,scale_f,color_rec,color_str);
				DrawaString("GO",1100,800,scale_f,color_rec,color_str);
				break;
			}
			break;
		case 10:
			color_rec[0]=1;
			DrawaString("HOLD ON CALIBRATING",100,400,scale_f,color_rec,color_str);
			break;
		}
		glPopMatrix();
	}
	glutSwapBuffers();
	FPS.NewFrame();
}
void* visionrun2(void*)
{
	Error error;
	const int k_numimages = 100;
	BusManager busmgr;
	unsigned int numcameras;
	error = busmgr.GetNumOfCameras(&numcameras);
	if (error != PGRERROR_OK)
	{
		printerror( error );
		return 0;
	}
	std::cout << "number of cameras detected: " << numcameras << std::endl; 
	if ( numcameras < 1 )
	{
		std::cout << "insufficient number of cameras... press enter to exit." << std::endl; ;
		std::cin.ignore();
		return 0;
	}
	CameraInfo caminfo;
	// connect the camera
	for (int i=0;i<2;i++)
	{
		camera[i] = new Camera();

		

		unsigned int packetsize;
		float percent;
		error = busmgr.GetCameraFromIndex( i, &guid[i] );
		if (error != PGRERROR_OK)
		{
			printerror( error );
			return 0;
		}
		error = camera[i]->Connect(  &guid[i]  );
		if ( error != PGRERROR_OK )
		{
			std::cout << "failed to connect to camera" << std::endl;     
			return false;
		}
		// get the camera info and print it out
		error = camera[i]->GetCameraInfo( &caminfo );
		if ( error != PGRERROR_OK )
		{
			std::cout << "failed to get camera info from camera" << std::endl;     
			return false;
		}
		error = camera[i]->GetFormat7Info( &f7info[i], &supported[i] );
		// get camera settings
		error = camera[i]->GetFormat7Configuration( &f7set[i],&packetsize,&percent );
		if ( error != PGRERROR_OK )
		{
			std::cout << "failed to get camera info from camera" << std::endl;     
			return false;
		}
		f7set[i].height=900;
		f7set[i].width=1600;
		baseoffx=f7set[i].offsetX;
		baseoffy=f7set[i].offsetY;

		
		stepoffx=f7info[i].imageHStepSize;
		stepoffy=f7info[i].imageVStepSize;
		//stepoffx=f7info[i].offsetHStepSize;
		//stepoffy=f7info[i].offsetVStepSize;
		error = camera[i]->SetFormat7Configuration( &f7set[i],100.0f );
		if ( error != PGRERROR_OK )
		{
			std::cout << "failed to set camera settings" << std::endl;     
			return false;
		}

		std::cout << caminfo.vendorName << " "
			<< caminfo.modelName << " " 
			<< caminfo.serialNumber << std::endl;
		error = camera[i]->StartCapture();
		if ( error == PGRERROR_ISOCH_BANDWIDTH_EXCEEDED )
		{
			std::cout << "bandwidth exceeded" << std::endl;     
			return false;
		}
		else if ( error != PGRERROR_OK )
		{
			std::cout << "failed to start image capture" << std::endl;     
			return false;
		} 
	}
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1600, 900);
	InitBitmapStruct(1600, 900);
	glutCreateWindow("Grasp Exp");
	glutDisplayFunc(GlutDisplayFunc);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	// Create Rendering Context
#define GL_CLAMP_TO_EDGE 0x812F
	glGenTextures( sk_maxNumTextures, m_imageTextures );
	bool useClampToEdge = true;
	if ( atof( (const char*)glGetString(GL_VERSION)) < 1.15)
	{
		useClampToEdge = false;
	}

	for ( int i = 0; i < sk_maxNumTextures; i++)
	{
		glBindTexture( GL_TEXTURE_2D, m_imageTextures[ i] );

		if ( useClampToEdge)
		{
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		}
		else
		{
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
		}
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	}
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	glShadeModel( GL_FLAT );
	glEnable( GL_BLEND );
	glEnableClientState(GL_VERTEX_ARRAY);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// initialize matrices
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0, 1, 0, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glutstarted=true;
	glutMainLoop();
	for (int i=0;i<2;i++)


	{
		error = camera[i]->StopCapture();
		if ( error != PGRERROR_OK )
		{
			// this may fail when the camera was removed, so don't show 
			// an error message
		}  

		camera[i]->Disconnect();
	} 
	return 0;
	return 0;
}
void vsScreenSize (bool fullscreen)
{
	if (glutstarted)
		if (fullscreen)
			glutFullScreen();
		else
			glutReshapeWindow(1600, 900);
	else
		return;
}
void vsCamOff (double offX[2], double offY[2])
{
	Error error;
	pauseCam = true;
	bool valid[2];
	//while(usingcam);
	Sleep(1000);
	if (!usingcam)
	{
		for (int i=0;i<2;i++)
		{
			error = camera[i]->StopCapture();
				if ( error != PGRERROR_OK )
			{
				std::cout << "failed to stop" << std::endl;     
				break;
			}
			error = camera[i]->Disconnect();
			if ( error != PGRERROR_OK )
			{
				std::cout << "failed to disconnect" << std::endl;     
				break;
			}
			error = camera[i]->Connect(  &guid[i]  );
			if ( error != PGRERROR_OK )
			{
				std::cout << "failed to connect" << std::endl;     
				break;
			}
			
			camera[i]->GetFormat7Configuration(&f7set[i],NULL,NULL);			
			f7set[i].offsetX=baseoffx-offX[i]*stepoffx;
			f7set[i].offsetY=baseoffy-offY[i]*stepoffy;
			Format7PacketInfo pPacketInfo;
			error = camera[i]->ValidateFormat7Settings( &f7set[i],  &valid[i],&pPacketInfo);
			if ( error != PGRERROR_OK )
			{
				std::cout << "failed to validate" << std::endl;     
				break;
			}
			error = camera[i]->SetFormat7Configuration( &f7set[i],100.0f );
			if ( error != PGRERROR_OK )
			{
				std::cout << "failed to set camera settings" << std::endl;     
				break;
			}
			
			error = camera[i]->StartCapture();
				if ( error != PGRERROR_OK )
			{
				std::cout << "failed to start" << std::endl;     
				break;
			}
		}
	}
	pauseCam = false;
	return;
}
