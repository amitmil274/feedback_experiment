
#define STEREO_SUPPORT

#include <HD/hd.h>
#include <HDU/hduVector.h>
#include <HDU/hduMatrix.h>
#include <HDU/hduError.h>

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <GL/glut.h>
#include <ctype.h>
#include <string.h>
#include <list>
#include <math.h>
#include <process.h>
#include <mmsystem.h>
#include "HDAPIUtilities.h"
#include "Timer.h"
#include "FileHandle.h"
#include "Physics.h"
#include "Display.h"
#include "Haptic.h"
#include "CriticalSection.h"
#include "Protocol.h"
#include <pthread.h>
#include <fstream>
#include <direct.h>

pthread_mutex_t mutexposition;

pthread_t threads;

extern Protocol protocol;
extern FileHandle filehandle;
extern HapticData hapticData;
extern Timer timer;
extern CriticalSection critical;
extern Point point1;
extern DisplayData displayData;
extern HHD devHandle;
extern std::list<hduVector3Dd>	gForceBuff1;
extern HDSchedulerHandle gHDCallbackHandle;

const double deg2rad(const double deg){ return deg * (PI/180.0); }
const double rad2deg(const double rad){ return rad * (180.0/PI); }
			
double stereo_iod= 0.0450;		
double screen_width= 0.5300;		
double screen_height=  0.300;		
double screen_distance= 0.900;		
double screen_near= 0.100;		
double screen_far= 10.000;		
double cam_fov_v= rad2deg(atan2(screen_height / 2.0, screen_distance) * 2.0);																	// カメラの垂直視野角		[deg]　		モノでのみ使用
double cam_rotation_adjust= 0;
double cam_sides_adjust=0;
double gRampupRate1=0.2;

hduVector3Dd gEmuPos1(-0.100,0,0);
hduVector3Dd gEmuPos2(0.100,0,0);
hduVector3Dd cam_pos(0.0, 0.0, 0.900);

int	cam_last_reshape_width= 1920;
int	cam_last_reshape_height= 1080;

int TimeToStop=0;
int status_keep=0;
int last_status=-1;
int	gMouse_Last_X=0;
int	gMouse_Last_Y=0;

BOOL stereo_enable= true;
bool gMouse_LDOWN	= false;
bool gMouse_MDOWN	= false;
bool gMouse_RDOWN	= false;
bool ActForce=false;
bool count1 = true;
bool count2 = false;
bool cam_mirrored= false;
bool switch_key = false;
bool switch_temperature = false;

int ExpMode[]={0,1,1,2,1,2,1,3};
//int ExpMode[]={0,2,1,2,1,3};
int ExpModeLen=sizeof(ExpMode)/sizeof(*ExpMode);
float DelayMode[]={0,0,0,0,0,4,0,0};
bool isDelay=true;
int status = ExpMode[protocol.ModeInd];
bool NextModeFlag=false; // a flag that says the 'p' keyboard was pressed- when the pong trial will be finished, reaching will appear
int PongStatus=0;
extern int ReachStatus=0;

float DispScaleX=filehandle.DisplayData[0]; // for calculating the horizontal scaling factor according to the definition (2*HorizTrans) and the input (DispScaleInX).
float DispScaleY=filehandle.DisplayData[1]; // for calculating the vertical scaling factor according to the definition (2*VerTrans) and the input (DispScaleInY).
float DispShiftX=filehandle.Calib[0]; 
float DispShiftY=filehandle.Calib[1];

float CenterX=0.0*DispScaleX+DispShiftX;
float CenterY=0.0*DispScaleY+DispShiftY;
float WallHoriz=0.13; // half of the distance between left and right walls [m]
float WallVer=0.09; // half of the distance between up and down walls [m]
float WallWidth=0.01;

// calibration data

bool ServoFlag= false;
float initReachPosX=0.0;
float initReachPosY=-0.1;
int TargNum=2;
float TargDist=0.1; // target distance [m]
float Targ[3][2] = {{initReachPosX-sqrt(pow(TargDist,2)/2), initReachPosY+sqrt(pow(TargDist,2)/2)},{initReachPosX, initReachPosY+TargDist},{initReachPosX+sqrt(pow(TargDist,2)/2), initReachPosY+sqrt(pow(TargDist,2)/2)}};
float TargX;
float TargY;

int BallState = 0;
int PaddleState = 0;
float BallX0= 0.0;
float BallY0= 0.0;
float VelX=0.0;
float VelY=0.2;
float VelPreX=0.0;
float VelPreY=0.2;
float MaxVelY=0.4;
float MaxVelX=0.2;
bool HitFlag= false;
float paddlePosX;
float paddlePosY;
float paddleVelX;
float paddleVelY;
bool InUpdateVecFlag=false;
int TrialPongNum1=1;
int TrialPongNum2=3;


float paddleX=0.012; // coordinates for paddle horizontal size
float paddleY=0.0025; // coordinates for paddle vertical size

void GlutSpecialFunc(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_F1: break;
	case GLUT_KEY_F2: break;
	case GLUT_KEY_F3: break;
	case GLUT_KEY_F4: break;
	case GLUT_KEY_F5: break;
	case GLUT_KEY_F6: break;
	case GLUT_KEY_F7: break;
	case GLUT_KEY_F8: break;
	case GLUT_KEY_F9: break;
	case GLUT_KEY_F10: break;
	case GLUT_KEY_F11: break;
	case GLUT_KEY_F12: break;
	case GLUT_KEY_LEFT: break;
	case GLUT_KEY_UP: break;
	case GLUT_KEY_RIGHT: break;
	case GLUT_KEY_DOWN: break;
	case GLUT_KEY_PAGE_UP: break;
	case GLUT_KEY_PAGE_DOWN: break;
	case GLUT_KEY_HOME: break;
	case GLUT_KEY_END: break;
	case GLUT_KEY_INSERT: break;
	}
}

void GlutMotionFunc(int x, int y)
{
	int diff_x = x - gMouse_Last_X;
	int diff_y = y - gMouse_Last_Y;

	// エミュレーションモードの時には、マウスで指を移動させる- When in emulation mode, move the finger across the mouse
	if(gMouse_LDOWN && !gMouse_MDOWN)
	{
		gEmuPos1[0] += 0.100 * double(diff_x) * 0.001;
		gEmuPos2[0] += 0.100 * double(diff_x) * 0.001;
		gEmuPos1[2] += 0.100 * double(diff_y) * 0.001;
		gEmuPos2[2] += 0.100 * double(diff_y) * 0.001;
	}
	if(gMouse_MDOWN && !gMouse_LDOWN)
	{
		gEmuPos1[0] += 0.100 * double(diff_x) * 0.001;
		gEmuPos2[0] += 0.100 * double(diff_x) * 0.001;
		gEmuPos1[1] -= 0.100 * double(diff_y) * 0.001;
		gEmuPos2[1] -= 0.100 * double(diff_y) * 0.001;
	}
	if(gMouse_RDOWN)
	{
		gEmuPos1[0] += 0.100 * double(diff_x) * 0.001;
		gEmuPos2[0] -= 0.100 * double(diff_x) * 0.001;
	}

	gMouse_Last_X = x;
	gMouse_Last_Y = y;
}

void GlutReshapeFunc(int width, int height)
{
	cam_last_reshape_width = width;
	cam_last_reshape_height = height;

	static GLfloat lightPosition[4] = {0.25f, 1.0f, 0.25f, 0.0f};
	static GLfloat lightDiffuse[3] = {1.0f, 1.0f, 1.0f};
	static GLfloat lightAmbient[3] = {0.35f, 0.35f, 0.35f};
	static GLfloat lightSpecular[3] = {0.7f, 0.7f, 0.7f};

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(cam_fov_v, (double)width / (double)height, 0.001, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(cam_mirrored) glScaled(1.0, -1.0, 1.0);		
	glTranslatef(-cam_pos[0], -cam_pos[1], -cam_pos[2]);
	glRotatef(cam_rotation_adjust, 1.0, 0.0, 0.0);
	glRotatef(cam_sides_adjust, 0.0, 1.0, 0.0);
	glLineWidth(2.0);
}

void GlutMouseFunc(int button, int state, int x, int y)
{
	switch(button)
	{
	case GLUT_LEFT_BUTTON:
		if(state == GLUT_UP) gMouse_LDOWN = false;
		else gMouse_LDOWN = true;
		break;
	case GLUT_MIDDLE_BUTTON:
		if(state == GLUT_UP) gMouse_MDOWN = false;
		else gMouse_MDOWN = true;
		break;
	case GLUT_RIGHT_BUTTON:
		if(state == GLUT_UP) gMouse_RDOWN = false;
		else gMouse_RDOWN = true;
		break;
	}
	gMouse_Last_X = x;
	gMouse_Last_Y = y;
	
}


void GlutKeyboardFunc(unsigned char key, int x, int y)
{
	const int GLUT_KEY_ESC			= 0x1b;
	const int GLUT_KEY_BACKSPACE	= 0x08;
	const int GLUT_KEY_DELETE		= 0x10;
	
	cout<<"Finishing Pong Block"<<endl;
	switch(key)
	{
	case GLUT_KEY_ESC:	// Esc
		break;
	
	case 'p':	// Pong
		NextModeFlag=true;
		cout<<"NextModeFlag "<<NextModeFlag<<endl;
		break;
	}
}


void DrawaString(char *str, int x, int y)
{
	void* font_face = GLUT_BITMAP_TIMES_ROMAN_24;
	void* font_face2 = GLUT_STROKE_ROMAN;
	int font_height = 16;
	int wy;
	int width, height;

	width = glutGet((GLenum)GLUT_WINDOW_WIDTH);
	height = glutGet((GLenum)GLUT_WINDOW_HEIGHT);

	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0f, (float)width, 0.0f, (float)height);

	glMatrixMode(GL_MODELVIEW);
	glTranslatef(0.0, -50,0.0);
	glScalef(-1.0,1.0,1.0);
	glPushMatrix();
		
	glLoadIdentity();

	wy = height - y - font_height;
	glRasterPos2i(x, wy);
	for(; *str !=0; str++)
	{
		if(*str == '\n')
		{
			wy -= font_height;
			glRasterPos2i(x, wy);
			continue;
		}
	
	/*	glutBitmapCharacter(font_face, *str);*/
		glutStrokeCharacter(font_face2, *str);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
}

void DrawScene()
{
	if(status == 1) //HERE DRAW REACH
	{
		static GLfloat diffuse3 [3] = {1.0f, 0.0f, 0.0f};
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //back ground colar glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse3);//指の色が変わる 赤
	
		if (protocol.ModeInd<=1)
			point1.Draw();

		if (protocol.ModeInd==2)
		{
			if(protocol.DispFeedbackFlag)
				point1.Draw();
		}


		if (ReachStatus==0)
		{
			point1.Draw();
			DrawaString("Stand on the gray zone to start", 200,200);
			glPushMatrix();
			glTranslatef(CenterX+initReachPosX*DispScaleX,CenterY+initReachPosY*DispScaleY,0);
			glColor3f(0.7,0.7,0.7);
			glBegin(GL_POLYGON);
			/* Set the vertices */
			glVertex2d(-0.025*DispScaleX, 0.005*DispScaleY);
			glVertex2d(-0.025*DispScaleX, -0.005*DispScaleY);
			glVertex2d(0.025*DispScaleX, -0.005*DispScaleY);
			glVertex2d(0.025*DispScaleX, 0.005*DispScaleY);
			/* Done making polygon */
			glEnd();
			glPopMatrix();

			
		}
		else if (ReachStatus==1)
		{
			DrawaString("Move with the robot", 200,200);
			glPushMatrix();
			glTranslatef(CenterX+0,CenterY+0,0);
			glColor3f(0.7,0.7,0.7);
			glPopMatrix();
			
		}
		else if (ReachStatus==3)
		{
			

			TargX=Targ[protocol.TargInd][0];
			TargY=Targ[protocol.TargInd][1];
			
			glPushMatrix();
			glTranslatef(CenterX+TargX*DispScaleX,CenterY+TargY*DispScaleY,0);
			glColor3f(0.0,0.0,0.0);
			glBegin(GL_POLYGON);
			/* Set the vertices */
			glVertex2d(-protocol.TargSize*DispScaleX, protocol.TargSize*DispScaleY);
			glVertex2d(-protocol.TargSize*DispScaleX, -protocol.TargSize*DispScaleY);
			glVertex2d(protocol.TargSize*DispScaleX, -protocol.TargSize*DispScaleY);
			glVertex2d(protocol.TargSize*DispScaleX, protocol.TargSize*DispScaleY);
			/* Done making polygon */
			glEnd();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(CenterX+TargX*DispScaleX,CenterY+TargY*DispScaleY,0);
			glColor3f(1.0,1.0,1.0);
			glBegin(GL_POLYGON);
			/* Set the vertices */
			glVertex2d(-(protocol.TargSize+0.003)*DispScaleX, (protocol.TargSize+0.003)*DispScaleY);
			glVertex2d(-(protocol.TargSize+0.003)*DispScaleX, -(protocol.TargSize+0.003)*DispScaleY);
			glVertex2d((protocol.TargSize+0.003)*DispScaleX, -(protocol.TargSize+0.003)*DispScaleY);
			glVertex2d((protocol.TargSize+0.003)*DispScaleX, (protocol.TargSize+0.003)*DispScaleY);
			/* Done making polygon */
			glEnd();
			glPopMatrix();	
				
		}
		

		
	}
	else if(status == 2) //HERE DRAW PONG
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //back ground colar glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		

	// マテリアルパラメータ
	/*static GLfloat diffuse1 [3] = {1.0f, 0.0f, 1.0f};//finger and top ball
	static GLfloat diffuse2 [3] = {0.0f, 0.2f, 1.0f};// all object
	static GLfloat diffuse3 [3] = {1.0f, 0.5f, 0.0f};//
	static GLfloat ambient  [3] = {0.0f, 0.1f, 0.5f};// all object
	static GLfloat specular [3] = {1.0f, 1.0f, 1.0f};
	static GLfloat shininess[1] = {32.0f};//all object
	*/
	static GLfloat diffuse1 [3] = {0.1f, 0.3f, 0.8f};
	static GLfloat diffuse2 [3] = {0.0f, 0.2f, 1.0f};
	static GLfloat diffuse3 [3] = {1.0f, 0.0f, 0.0f};
	static GLfloat diffuse4 [3] = {1.0f, 1.0f, 0.0f};
	static GLfloat ambient  [3] = {0.0f, 0.1f, 0.5f};
	static GLfloat specular [3] = {1.0f, 1.0f, 1.0f};
	static GLfloat shininess[1] = {32.0f};

	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse4);//空中の丸の色変わる 黄色
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	//クリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// カーソル位置
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse3);//指の色が変わる 赤

	//Left Wall
	glPushMatrix();
	//glTranslatef(-0.15,0,0);
	glTranslatef(CenterX-WallHoriz*DispScaleX,CenterY,0);
	//glColor3f(1.0,0.7,0.0);
	glColor3f(0.7,0.7,0.7);
	glBegin(GL_POLYGON);
	/* Set the vertices */
	/*glVertex2d(-0.005, 0.1);
    glVertex2d(-0.005, -0.1);
    glVertex2d(0.005, -0.1);
    glVertex2d(0.005, 0.1);*/
    glVertex2d(-WallWidth/2*DispScaleX, (WallVer+WallWidth/2)*DispScaleY);
    glVertex2d(-WallWidth/2*DispScaleX, (-WallVer-WallWidth/2)*DispScaleY);
    glVertex2d(WallWidth/2*DispScaleX, (-WallVer-WallWidth/2)*DispScaleY);
    glVertex2d(WallWidth/2*DispScaleX, (WallVer+WallWidth/2)*DispScaleY);
	/* Done making polygon */
	glEnd();
	glPopMatrix();

	//Right Wall
	glPushMatrix();
	//glTranslatef(0.15,0,0);
	glTranslatef(CenterX+WallHoriz*DispScaleX,CenterY,0);
	//glColor3f(1.0,0.7,0.0);
	glColor3f(0.7,0.7,0.7);
	glBegin(GL_POLYGON);
	/* Set the vertices */
    glVertex2d(-WallWidth/2*DispScaleX, (WallVer+WallWidth/2)*DispScaleY);
    glVertex2d(-WallWidth/2*DispScaleX, (-WallVer-WallWidth/2)*DispScaleY);
    glVertex2d(WallWidth/2*DispScaleX, (-WallVer-WallWidth/2)*DispScaleY);
    glVertex2d(WallWidth/2*DispScaleX, (WallVer+WallWidth/2)*DispScaleY);
	/* Done making polygon */
	glEnd();
	glPopMatrix();

	// BottomWall
	glPushMatrix();
	glTranslatef(CenterX,CenterY-WallVer*DispScaleY,0);
	//glColor3f(0.24,0.7,0.44);
	glColor3f(0.7,0.7,0.7);
	glBegin(GL_POLYGON);
	/* Set the vertices */
    glVertex2d(-WallHoriz*DispScaleX,-WallWidth/2*DispScaleY);
    glVertex2d(WallHoriz*DispScaleX,-WallWidth/2*DispScaleY);
    glVertex2d(WallHoriz*DispScaleX,WallWidth/2*DispScaleY);
    glVertex2d(-WallHoriz*DispScaleX,WallWidth/2*DispScaleY);
	/* Done making polygon */
	glEnd();
	glPopMatrix();
	
	// TopWall
	glPushMatrix();
	glTranslatef(CenterX,CenterY+WallVer*DispScaleY,0);
	//glColor3f(1.0,0.7,0.0);
	glColor3f(0.7,0.7,0.7);
	glBegin(GL_POLYGON);
	/* Set the vertices */
    glVertex2d(-WallHoriz*DispScaleX,-WallWidth/2*DispScaleY);
    glVertex2d(WallHoriz*DispScaleX,-WallWidth/2*DispScaleY);
    glVertex2d(WallHoriz*DispScaleX,WallWidth/2*DispScaleY);
    glVertex2d(-WallHoriz*DispScaleX,WallWidth/2*DispScaleY);
	/* Done making polygon */
	glEnd();
	glPopMatrix();
	
	// Timer Bar
		//Progressing Bar
	glPushMatrix();
	glTranslatef(CenterX-0.15*DispScaleX,CenterY+0.125*DispScaleY,0);
	glColor3f(0.73,0.33,0.83);
	glBegin(GL_POLYGON);
	/* Set the vertices */
	glVertex2d(-0.05*DispScaleX, 0.01*DispScaleY);
	glVertex2d(-0.05*DispScaleX, -0.01*DispScaleY);
	glVertex2d((-0.05+displayData.TimeBarLength)*DispScaleX, -0.01*DispScaleY);
	glVertex2d((-0.05+displayData.TimeBarLength)*DispScaleX, 0.01*DispScaleY);
	/* Done making polygon */
	glEnd();
	glPopMatrix();

		//Boundaries
	glPushMatrix();
	glTranslatef(CenterX-0.15*DispScaleX,CenterY+0.125*DispScaleY,0);
	glColor3f(0.0,0.0,0.0);
	glBegin(GL_POLYGON);
	/* Set the vertices */
	glVertex2d(-0.05*DispScaleX, 0.01*DispScaleY);
	glVertex2d(-0.05*DispScaleX, -0.01*DispScaleY);
	glVertex2d(0.05*DispScaleX, -0.01*DispScaleY);
	glVertex2d(0.05*DispScaleX, 0.01*DispScaleY);
	/* Done making polygon */
	glEnd();
	glPopMatrix();
		
	glPushMatrix();
	glTranslatef(CenterX-0.15*DispScaleX,CenterY+0.125*DispScaleY,0);
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_POLYGON);
	/* Set the vertices */
	glVertex2d(-0.053*DispScaleX, 0.013*DispScaleY);
	glVertex2d(-0.053*DispScaleX, -0.013*DispScaleY);
	glVertex2d(0.053*DispScaleX, -0.013*DispScaleY);
	glVertex2d(0.053*DispScaleX, 0.013*DispScaleY);
	/* Done making polygon */
	glEnd();
	glPopMatrix();

	// Hit Bar
		//Progressing Bar
	glPushMatrix();
	glTranslatef(CenterX-0.03*DispScaleX,CenterY+0.125*DispScaleY,0);
	glColor3f(0.12,0.56,1.0);
	glBegin(GL_POLYGON);
	/* Set the vertices */
	glVertex2d(-0.05*DispScaleX, 0.01*DispScaleY);
	glVertex2d(-0.05*DispScaleX, -0.01*DispScaleY);
	glVertex2d((-0.05+displayData.HitsBarLength)*DispScaleX, -0.01*DispScaleY);
	glVertex2d((-0.05+displayData.HitsBarLength)*DispScaleX, 0.01*DispScaleY);
	/* Done making polygon */
	glEnd();
	glPopMatrix();

		//Boundaries
	glPushMatrix();
	glTranslatef(CenterX-0.03*DispScaleX,CenterY+0.125*DispScaleY,0);
	glColor3f(0.0,0.0,0.0);
	glBegin(GL_POLYGON);
	/* Set the vertices */
	glVertex2d(-0.05*DispScaleX, 0.01*DispScaleY);
	glVertex2d(-0.05*DispScaleX, -0.01*DispScaleY);
	glVertex2d(0.05*DispScaleX, -0.01*DispScaleY);
	glVertex2d(0.05*DispScaleX, 0.01*DispScaleY);
	/* Done making polygon */
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(CenterX-0.03*DispScaleX,CenterY+0.125*DispScaleY,0);
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_POLYGON);
	/* Set the vertices */
	glVertex2d(-0.053*DispScaleX, 0.013*DispScaleY);
	glVertex2d(-0.053*DispScaleX, -0.013*DispScaleY);
	glVertex2d(0.053*DispScaleX, -0.013*DispScaleY);
	glVertex2d(0.053*DispScaleX, 0.013*DispScaleY);
	/* Done making polygon */
	glEnd();
	glPopMatrix();

	point1.Draw();

	//Background
	glPushMatrix();
	glTranslatef(CenterX,CenterY,0);
	glColor3f(0.2,0.2,0.2);
	//glColor3f(0.4,0.4,0.4);
	glBegin(GL_POLYGON);
	/* Set the vertices */
    glVertex2d(-WallHoriz*DispScaleX, WallVer*DispScaleY);
    glVertex2d(-WallHoriz*DispScaleX, -WallVer*DispScaleY);
    glVertex2d(WallHoriz*DispScaleX, -WallVer*DispScaleY);
    glVertex2d(WallHoriz*DispScaleX, WallVer*DispScaleY);
	/* Done making polygon */
	glEnd();
	glPopMatrix();

	// Restart Pong Zone
	if (PongStatus==0)
	{
		glPushMatrix();
		glTranslatef(CenterX+0.05*DispScaleX,CenterY-0.12*DispScaleY,0);
		glColor3f(0.24,0.7,0.44);
		glBegin(GL_POLYGON);
		/* Set the vertices */
		glVertex2d(-0.02*DispScaleX, 0.005*DispScaleY);
		glVertex2d(-0.02*DispScaleX, -0.005*DispScaleY);
		glVertex2d(0.02*DispScaleX, -0.005*DispScaleY);
		glVertex2d(0.02*DispScaleX, 0.005*DispScaleY);
		/* Done making polygon */
		glEnd();
		glPopMatrix();
	}
	// Ball
	else
	{
		glPushMatrix();
		glTranslatef(CenterX+protocol.BallX*DispScaleX,CenterY+protocol.BallY*DispScaleY,0);
		glColor3f(1.0,1.0,1.0);
		glutSolidSphere(0.004*DispScaleX,20,20);
		glPopMatrix();

	
	}
	

	}
}



void GlutDisplayFunc()
{
	// デバイスが見つからない場合は、ここでシミュレーションを進める
	if(devHandle == HD_INVALID_HANDLE)
	{
		if(ActForce)
		{
			for(int i=0; i<17; i++)
			{
				hduMatrix transform1 = hduMatrix::createTranslation(gEmuPos1);
				hduMatrix transform2 = hduMatrix::createTranslation(gEmuPos2);

				// デバイス情報のセット
				point1.SetPosition(gEmuPos1);
			
			}
		}
	}
	glutPostRedisplay();

#ifdef STEREO_SUPPORT
	// ステレオ３Ｄ表示用描画処理
	if(stereo_enable)
	{
		// パラメータを準備
		double half_iod = stereo_iod / 2.0;
		double half_width = screen_width / 2.0;
		double half_height = screen_height / 2.0;
		double rate		= screen_near / screen_distance;

		// 右目用描画コード
		{
			// 描画先として、右目用バックバッファを選択
			glDrawBuffer(GL_BACK_RIGHT);

			// 視点からのスクリーン位置算出
			double left		= (-half_width - half_iod) * rate;
			double right	= (half_width - half_iod) * rate;
			double bottom	= (-half_height) * rate;
			double top		= (half_height) * rate;

			// 視錐台の設定
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glFrustum(left, right, bottom, top, screen_near, screen_far);

			// カメラの設定
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			// 上下反転
			if(cam_mirrored) glScaled(1.0, -1.0, 1.0);
			// 視点位置設定
			glTranslatef(-cam_pos[0] - half_iod, -cam_pos[1], -cam_pos[2]);
			// 回転
			glRotatef(cam_rotation_adjust, 1.0, 0.0, 0.0);
			glRotatef(cam_sides_adjust, 0.0, 1.0, 0.0);

			// モデルの描画
			glPushMatrix();
			DrawScene();
			glPopMatrix();
		}

		// 左目用描画コード
		{
			// 描画先として、左目用バックバッファを選択
			glDrawBuffer(GL_BACK_LEFT);

			// 視点からのスクリーン位置算出
			double left		= (-half_width + half_iod) * rate;
			double right	= (half_width + half_iod) * rate;
			double bottom	= (-half_height) * rate;
			double top		= (half_height) * rate;

			// 視錐台の設定
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glFrustum(left, right, bottom, top, screen_near, screen_far);

			// カメラの設定
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			// 上下反転
			if(cam_mirrored) glScaled(1.0, -1.0, 1.0);
			// 視点位置設定
			glTranslatef(-cam_pos[0] + half_iod, -cam_pos[1], -cam_pos[2]);
			// 回転
			glRotatef(cam_rotation_adjust, 1.0, 0.0, 0.0);
			glRotatef(cam_sides_adjust, 0.0, 1.0, 0.0);

			// モデルの描画
			glPushMatrix();
				DrawScene();
			glPopMatrix();
		}
	}
	// モノ２Ｄ表示用描画処理
	else
#endif
	{
		glDrawBuffer(GL_BACK);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		DrawScene();
		glPopMatrix();
	}

	// バッファをスワップ
	glutSwapBuffers();
}

void* InitGlut(void*)
{
	//glutInit(&argc, (char**)argv);
#ifdef STEREO_SUPPORT
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STEREO);
//#else
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
#endif
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Pong Game");
	glutDisplayFunc(GlutDisplayFunc);
	glutSpecialFunc(GlutSpecialFunc);
	glutMouseFunc(GlutMouseFunc);
	glutMotionFunc(GlutMotionFunc);
	glutReshapeFunc(GlutReshapeFunc);
	//glutIdleFunc(GlutIdleFunc);
	glutKeyboardFunc(GlutKeyboardFunc);
	glutMainLoop();
	return 0;
}






void InitSimulation()
{
	gForceBuff1.push_back(hduVector3Dd());
	
}

//-------------------------------------------------------------------------------------------------




// 終了処理
void ExitHandler()
{
	// スケジューラを止める
	hdStopScheduler();
	hdUnschedule(gHDCallbackHandle);
	hdDisableDevice(devHandle);
	
	fclose(filehandle.Data_file);
}







//void InitDAQmx()
//{
	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	//DAQmxErrChk (DAQmxCreateTask("",&DAQmx_taskHandle));
	//DAQmxErrChk (DAQmxCreateDOChan(DAQmx_taskHandle,"Dev1/port1","",DAQmx_Val_ChanForAllLines));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	//DAQmxErrChk (DAQmxStartTask(DAQmx_taskHandle));

//Error:
	//if( DAQmxFailed(DAQmx_error) )
	//	DAQmxGetExtendedErrorInfo(DAQmx_errBuff,2048);
	//if( DAQmx_taskHandle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		//DAQmxStopTask(DAQmx_taskHandle);
		//DAQmxClearTask(DAQmx_taskHandle);
	//}
//}


void Help()
{
	printf("\n\n(S)→change stereo　([.])→adjust rotation　(M)→UP side DOWN\n(Space)→demo　(Z)→reset　(W)→reset{change angle of object}\n(+)→START　(R)→STOP　(T)→RESTART\n(1 or ３)→ANSWER　(X)→ANSWER ON/OFF\n\n");
}


//-------------------------------------------------------------------------------------------------

// メイン関数
int main()
{
	pthread_mutex_init(&mutexposition, NULL);

	// open display thread
	double CurrentPosition[3];
	pthread_create(&threads, NULL, InitGlut, (void *)CurrentPosition);
	
	filehandle.CreateDataFolder();
	filehandle.CreateExpInfoFile();
	filehandle.OpenDataFile();	//区切る場合に非表示
	
	// シミュレーション初期化
	InitSimulation();


	//initializing haptic device and starting haptic thread
	hapticData.InitHapticDevice();


	//USB-6009の初期化
	//InitDAQmx();

	Help();
	timer.reset();
	while (!TimeToStop)
    {
		// the only place readTime is called. It is a heavy function.
		timer.time=timer.get();
		protocol.run();
    }

	ExitHandler();
	// メインループを開始
	
}
 