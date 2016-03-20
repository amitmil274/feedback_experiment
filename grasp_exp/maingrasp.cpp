#include "PracticalSocket/PracticalSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <Windows.h>
#include <conio.h>
#include <GL/glut.h>
#include <ctype.h>
#include <string.h>
#include <list>
#include <math.h>
#include <process.h>
#include <mmsystem.h>
#include "Timer.h"
#include "FileHandle.h"
#include "Protocol.h"
#include <fstream>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <direct.h>
#include "dhdc.h"
#include "Haptic.h"
#include "sigma_comm.h"
#include "VisionThread2.h"

using namespace std;
using namespace Eigen;

double stereo_iod= 0.0450;		
double screen_width= 0.5300;		
double screen_height=  0.300;		
double screen_distance= 0.900;		
double screen_near= 0.100;		
double screen_far= 10.000;	
//pthread_mutex_t mutexposition;

//pthread_t threads;
const double PI = 3.1415926535897932384626433832795;
#define TIMEOUT_SECS 0
#define TIMEOUT_USECS 100
extern Protocol protocol;
extern FileHandle filemaster;
extern HapticData hapticData;
extern Timer timer;
extern int devHandle;

float CenterX=0;
float CenterY=0;
//extern std::list<hduVector3Dd>	gForceBuff1;
//extern HDSchedulerHandle gHDCallbackHandle;

const double deg2rad(const double deg){ return deg * (PI/180.0); }
const double rad2deg(const double rad){ return rad * (180.0/PI); }
			
	bool SimulationOn;
	int g_footpedal=0;
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
bool switch_key = false;
bool switch_temperature = false;

//int ExpMode[]={1,2};
//int ExpMode[]={0,2,1,2,1,3};
//int ExpModeLen=sizeof(ExpMode)/sizeof(*ExpMode);
//int status = ExpMode[protocol.ModeInd];
int status = 0;
int laststatus;
bool NextModeFlag=false; // a flag that says the 'p' keyboard was pressed- when the pong trial will be finished, reaching will appear
//int PongStatus=0;
extern int graspStatus=0;

// calibration data

bool ServoFlag= false;

int TargNum=140;

bool InUpdateVecFlag=false;

#define ADDR_BUBBLES   "127.0.0.1"
#define ADDR_BUTTERCUP "127.0.0.1"
extern stMA2UI_DATA Ma2UIdata;
extern stUI2MA_DATA UI2Madata;
extern int footpedal;
Sigma_Comm comm(&Ma2UIdata, &UI2Madata);
void ExitHandler()
{
	Sleep(2000);
	dhdClose(devHandle);
	fclose(filemaster.Data_file_master);
	fclose(filemaster.Data_file_slave);
}
void * protocolrun(void *pdata)
{
	while(SimulationOn)
	{
		timer.time=timer.get();
		protocol.run();
	}
	return NULL;
}
void StartExperiment ()
{
	DWORD ThreadId1;
	DWORD ThreadId2;
	DWORD ThreadIdCameras;

	// haptics thread
	CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)(visionrun2), NULL, NULL, &ThreadIdCameras);
		cout<<1<<endl;
	SetThreadPriority(&ThreadId1, THREAD_PRIORITY_HIGHEST);
	CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)(HapticsLoop), NULL, NULL, &ThreadId1);
		cout<<1<<endl;
	SetThreadPriority(&ThreadId1, THREAD_PRIORITY_ABOVE_NORMAL);
	// communication/teleoperation thread
	//CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)(RunComm), NULL, NULL, &ThreadId[2]);
	// camera thread
	//CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)(RunCamera), NULL, NULL, &ThreadId[3]);
	// protocol thread
	CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)(protocolrun), NULL, NULL, &ThreadId2);
		cout<<2<<endl;
	// file thread
	//CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)(RunFile), NULL, NULL, &ThreadId[5]);

}
int main(int argc, char* argv[])
{
		glutInit(&argc, (char**)argv);
		char *input="testing";
	//pthread_mutex_init(&mutexposition, NULL);
	filemaster.CreateDataFolder();
	filemaster.CreateExpInfoFile();
	//initializing haptic device and starting haptic thread
	hapticData.InitHapticDevice();
	timer.reset();
	char ch = 'r';
	int hstdin = (int) GetStdHandle(STD_INPUT_HANDLE);
	fd_set readSet;
	TIMEVAL timeout = {TIMEOUT_SECS, TIMEOUT_USECS};
	int nfound;
	int the_tcpsock;

	// initialize communication with the robot //
	comm.Initialize_UDP_Robot(argc, argv); 

	while(!comm.Initialize_TCP_GUI()) {
		cout << "TCPIP initialization failed retry in 5 sec." << endl;
		Sleep(500);
		//return 1;
	}
	cout << "KS : TCPIP initialized " << endl;
	
	// Initialize FD_SET //
	FD_ZERO(&readSet);
	the_tcpsock = comm.tcpsocket->sockDesc;
	FD_SET(the_tcpsock, &readSet);

	status=0;
	StartExperiment();
	
	cout<<1<<endl;
	//for (int q=1;q<100;q++)
	while ( ch != 'q' && !TimeToStop)
	{// 	ENABLE ME   && comm.Check_Flag(BASIC_PROGRAM) ) {
		timeout.tv_sec = TIMEOUT_SECS;
		timeout.tv_usec = TIMEOUT_USECS;
		nfound = select( 1 , &readSet, NULL, NULL, &timeout );		// check the TCP Socket
		if ( nfound < 0 ) {						// Socket error
			cerr << "Socket error "<<nfound<<" : "<<WSAGetLastError()<<" \n";
			cout << "sockedesc:"<< comm.tcpsocket->sockDesc<<endl;
			//			Sleep(10);
			//			break;
		}
		if ( nfound > 0 && FD_ISSET(the_tcpsock, &readSet)) {			// TCP socket ready to read
			//cout << "recv TCP...\n";
			comm.Recv_TCP();
			comm.Check_UI2MA(1);// 0 means not to dispay the message
		} 
		FD_CLR(the_tcpsock,&readSet);					// Reset fd_set
		FD_SET(the_tcpsock,&readSet);					//  ""

		if( _kbhit() )							// check for keyboard input`
		{
			ch = _getch();
			switch(ch)
			{
			case 'd':
				cout << "set pedal down\n";
				g_footpedal = 1;
				break;
			case 'e':
				cout << "set pedal up\n";
				g_footpedal = 0;
				break;
			case 'q':
				SimulationOn=false;
				break;
			}
		}
		if(!SimulationOn){
			fprintf(stderr, "\nThe main scheduler callback has exited\n");
			fprintf(stderr, "\nPress any key to quit.\n");
			_getch();
			break;
		}
		if (footpedal==1)
		{
		//timer.time=timer.get();
		//protocol.run();
		}
	}
	//while (!TimeToStop)
 //   {
	//	// the only place readTime is called. It is a heavy function.
	//	timer.time=timer.get();
	//	protocol.run();
 //   }

	ExitHandler();
	// メインループを開始
	
}
