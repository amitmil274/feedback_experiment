#include "PracticalSocket/PracticalSocket.h"
//#include "../VS_GUI_SERVER/CommonDS.h"
//#include "ITPteleoperation.h"
#include "sigma_comm.h"
//#include "coordinateSystems.h"
#include "sigma_hand.h"
#include <stdio.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <Windows.h>
//#include "CMacrosGL.h"

using namespace std;
using namespace Eigen;

#include "dhdc.h"

#define FALSE 0
#define TRUE 1
#define OMNI1 1
#define OMNI2 0
#define TIMEOUT_SECS 0
#define TIMEOUT_USECS 100

#define THOUSAND 1000
#define MILLION  (THOUSAND*THOUSAND)
#define round(x) (x)

// status flags
bool SimulationOn;
bool SimulationFinished;

// device-specific globals
int        NumDevice;
char      *DeviceID;
int       *FingerCount;
Vector3d **FingerPosGlobal;
Matrix3d  *DeviceRotGlobal;
bool      *HasRot;
bool      *HasGrip;
// global force
Vector3d ForceGlobal[2];
// text overlay globals
double LastTime = dhdGetTime();
double Freq;
char   Perf[50];
bool   ShowRate = true;

// object properties
const double Stiffness = 1000.0;
const double Mass      =  100.0;
const double Viscosity =    1.0;


// Three camera rotation angles
extern double th1, th2, th3;

int supportedCalibrationStyles;
int calibrationStyle;

//Global variable for reading device button

Vector3d pos_delta[2];
Vector3d rot_delta[2];
Vector3d pos_offset[2];
Vector3d rot_offset[2];
Matrix4d xfs[2];

extern const hduMatrix rotXMx;
extern const hduMatrix rotYMx;
extern const hduMatrix rotZMx;

int g_footpedal = 0;
unsigned int servo=0;
bool indexing= false;

//#define ADDR_BUBBLES   "132.72.216.40"
//#define ADDR_BUTTERCUP "132.72.216.156"
extern stMA2UI_DATA Ma2UIdata;
extern stUI2MA_DATA UI2Madata;

// Global Haptic device identifier

//HHD devHandle[OMNI1],devHandle[OMNI2]; /*Dual Master device */
int devHandle[2]; /*Dual Master device */
Sigma_Hand rHand;
// Global variable for testing.
bool oneOmni=false;
bool leftDevice=false;
bool rightDevice=false;
bool greenArm=false;
bool goldArm=false;
// Communication object for communication with SUI and Slave robot
Sigma_Comm comm(&Ma2UIdata, &UI2Madata);
void StartExperiment ()
{
	DWORD ThreadId[5];
	// haptics thread
	CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)(HapticsLoop), NULL, NULL, &ThreadId[0]);
	SetThreadPriority(&ThreadId, THREAD_PRIORITY_ABOVE_NORMAL);
	// communication/teleoperation thread
	CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)(RunComm), NULL, NULL, &ThreadId[1]);
	// camera thread
	CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)(RunCamera), NULL, NULL, &ThreadId[2]);
	// protocol thread
	CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)(RunProtocol), NULL, NULL, &ThreadId[3]);
	// file thread
	CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)(RunFile), NULL, NULL, &ThreadId[4]);

}
/******************************************************************************
main function
Initialize the device, create a callback to handle sphere forces, terminate
upon key press.
******************************************************************************/
int main(int argc, char* argv[])
{
	char *input="testing";
	int done = 0;
	int deviceCount;

	// get device count
	deviceCount = dhdGetDeviceCount ();
	NumDevice=deviceCount;
	if (deviceCount < 0) {
		printf ("error: %s\n", dhdErrorGetLastStr ());
		return 0;
	}
	else 
	{
		if (deviceCount < 1) 
			printf ("error: no device detected\n");
		else
			if (deviceCount < 2) 
			{
				oneOmni=true;
				greenArm=true;
				goldArm=false;
				printf("You Have Selected Green  Arm to Control \n");
			}
			else
			{
				printf("The Master is Selected to Work in Normal Mode\n");
				oneOmni=false;
				goldArm=true;
				greenArm=true;
			}
	}
	// open the first available device
	if ((devHandle[0] = dhdOpenID (0)) < 0){
		printf ("error: %s\n", dhdErrorGetLastStr ());
		printf("\nPress any key to quit.\n");
		_getch();
		return -1;
	}
	// open the second available device
	if ((devHandle[1] = dhdOpenID (1)) < 0) 
		printf ("error: %s\n", dhdErrorGetLastStr ());


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

	if(oneOmni && rightDevice && leftDevice){
		printf("You Are Trying to Run Testing With Both Omni's ON.\n We Are Defualting To right Omni\n");
		leftDevice=false;
	}
	// Debug for variables
	if(oneOmni)
		printf("One Omni mode is enabled\n");
	if(greenArm)
		printf("Green Arm is Controlled\n");
	if(goldArm)
		printf("Gold Arm is Controlled\n");

	StartSimulation ();
	//for (int q=1;q<100;q++)
	while ( ch != 'q')
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
	}

	try {
		comm.tcpsocket->cleanUp();
	} 
	catch (SocketException SE) {
		cerr<< "ERROR: Socket cleanUp failed.\n";
	}
	// close the connection to the first device
	if (dhdClose (0) < 0) {
		printf ("error: %s\n", dhdErrorGetLastStr ());
	}

	// close the connection to the second device
	if (dhdClose (1) < 0) {
		printf ("error: %s\n", dhdErrorGetLastStr ());
	}

	cout << "Ended application loop\n Press any key and exit." << endl;
	_getch();
	return 0;
} 		
hThread[0]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)&inThreadClient,(LPVOID) s,0,&dwThreadId[0]);
	hThread[1]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)&outThreadClient,(LPVOID) t,0,&dwThreadId[1]);
	hThread[1]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)&mainClient,NULL,0,&dwThreadId[2]);
	WaitForMultipleObjects(3,hThread,TRUE,INFINITE);

	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	CloseHandle(hThread[2]);