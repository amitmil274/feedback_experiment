#include "VisionThread3.h"

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
#include "VisionThread3.h"
using namespace FlyCapture2;
using namespace std;
static const unsigned int sk_maxNumTextures = 36;
GLuint m_imageTextures[sk_maxNumTextures];
FrameRateCounter FPS;
Camera** camera = new Camera*[2];
BITMAPINFO        m_bitmapInfo;  
bool glutstarted=false;
extern int status;
extern int graspStatus;
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
/*
 *		This Code Was Created By Jeff Molofee 2000
 *		Modified by Shawn T. to handle (%3.2f, num) parameters.
 *		A HUGE Thanks To Fredric Echols For Cleaning Up
 *		And Optimizing The Base Code, Making It More Flexible!
 *		If You've Found This Code Useful, Please Let Me Know.
 *		Visit My Site At nehe.gamedev.net
 */


GLuint	base;				// Base Display List For The Font Set
GLfloat	cnt1;				// 1st Counter Used To Move Text & For Coloring
GLfloat	cnt2;				// 2nd Counter Used To Move Text & For Coloring

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default

GLvoid BuildFont(GLvoid)								// Build Our Bitmap Font
{
	HFONT	font;										// Windows Font ID
	HFONT	oldfont;									// Used For Good House Keeping
	base = glGenLists(96);								// Storage For 96 Characters
	font = CreateFont(	-24,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						(LPCWSTR)"Courier New");					// Font Name
	
	oldfont = (HFONT)SelectObject(hDC, font);           // Selects The Font We Want
	wglUseFontBitmaps(hDC, 32, 96, base);				// Builds 96 Characters Starting At Character 32
	SelectObject(hDC, oldfont);							// Selects The Font We Want
	DeleteObject(font);									// Delete The Font
}
GLvoid KillFont(GLvoid)									// Delete The Font List
{
	glDeleteLists(base, 96);							// Delete All 96 Characters
}

GLvoid glPrint(int x,int y,const char *fmt)					// Custom GL "Print" Routine
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	//glLoadIdentity();									// Reset The Current Modelview Matrix
	//glTranslatef(0.0f,0.0f,-1.0f);						// Move One Unit Into The Screen
	// Pulsing Colors Based On Text Position
	int font_height=16;
	int wy;
	int width=1600, height=900;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0f, (float)width, 0.0f, (float)height);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	// Position The Text On The Screen
	wy = height - y - font_height;
	//glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1.0f, 0.0f, 0.0f);
	glRasterPos2i(x, wy);
 		// Print GL Text To The Screen
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments
	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing
	va_start(ap, fmt);									// Parses The String For Variables
	vsprintf(text, fmt, ap);						    // And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text
	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base - 32);								// Sets The Base Character to 32
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();		
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();// Pops The Display List Bits
}



void DrawaString(char *str, int x, int y)
{
	void* font_face = GLUT_BITMAP_TIMES_ROMAN_24;
	void* font_face2 = GLUT_STROKE_ROMAN;
	int font_height = 16;
	int wy;
	int width=1600, height=900;

	//width = glutGet((GLenum)GLUT_WINDOW_WIDTH);
	//height = glutGet((GLenum)GLUT_WINDOW_HEIGHT);

	//glDisable(GL_DEPTH_TEST | GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0f, (float)width, 0.0f, (float)height);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	wy = height - y - font_height;
	//glPushAttrib(GL_CURRENT_BIT);
	glColor3f(1.0f, 0.0f, 0.0f);
	glRasterPos2i(x, wy);
		for(; *str !=0; str++)
	{
		if(*str == '\n')
		{
			wy -= font_height;
			glRasterPos2i(x, wy);

			continue;
		}
		glutBitmapCharacter(font_face, *str);
		//glutStrokeCharacter(font_face2, *str);
	}
	//	glPopAttrib();
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

	int i=nCam-1; 
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
}

void GlutDisplayFunc()
{
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
		if (status>0)
		switch (graspStatus)
		{
			case 0:
			DrawaString("Click when you ready",300,400);
			glPrint(1100,400,"Click when you ready");
//			DrawaString("Click when you ready",1100,400);
			break;
			case 1:
			DrawaString("get ready to grasp",300,400);
				glPrint(1100,400,"get ready to grasp");
	//		DrawaString("get ready to grasp",1100,400);
			break;
			case 2:
			DrawaString("SET",300,200);
		glPrint(1100,400,"set %7.2f", cnt1);
		//	DrawaString("SET",1100,200);
			break;
			case 3:
			DrawaString("GO",300,200);
			glPrint(1100,400,"go %7.2f", cnt1);
//			DrawaString("GO",1100,200);
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

		PGRGuid guid;
		Format7ImageSettings f7set ;
		unsigned int packetsize;
		float percent;
		error = busmgr.GetCameraFromIndex( i, &guid );
		if (error != PGRERROR_OK)
		{
			printerror( error );
			return 0;
		}
		error = camera[i]->Connect(  &guid );
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
		// get camera settings
		error = camera[i]->GetFormat7Configuration( &f7set,&packetsize,&percent );
		if ( error != PGRERROR_OK )
		{
			std::cout << "failed to get camera info from camera" << std::endl;     
			return false;
		}
		f7set.height=900;
		f7set.width=1600;
		
		error = camera[i]->SetFormat7Configuration( &f7set,100.0f );
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
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	BuildFont();										// Build The Font
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
		KillFont();
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