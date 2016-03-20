
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

#define ADDR_BUBBLES   "127.0.0.1"
#define ADDR_BUTTERCUP "127.0.0.1"

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
hduVector3Dd temp_delta;


// Communication object for communication with SUI and Slave robot
Sigma_Comm comm(&Ma2UIdata, &UI2Madata);

void*
	HapticsLoop (void* pUserData)
{
	static double t0 = dhdGetTime ();
	double        t  = t0 + 0.001;
	double        dt,grasp[2]={0,0};
	double newGrip[2]={0,0},oldGrip[2];
	bool          initialized = false;
	int footpedal;
	int bttn[2] = {0,0};
	int      i,d;
	int rl2sui;
	double   r[3][3];
	Vector3d newPosGlobal[2][3];
	static Vector3d oldPosGlobal[2][3];
	Quaterniond qIncr[2],qCurr[2],qPrev[2];
	Matrix3d xform;

	Vector3d radius[2];
	radius[0] << 0.0, -0.02, 0.0;
	radius[1] << 0.0,  0.02, 0.0;

	// start haptic simulation
	SimulationOn       = true;
	SimulationFinished = false;

	// start with no force
	ForceGlobal[0].setZero ();
	ForceGlobal[1].setZero ();

	// enable force
	for (d=0; d<NumDevice; d++) dhdEnableForce (DHD_ON, devHandle[d]);
	cout<<1<<endl;
	// main haptic simulation loop
	while (SimulationOn) {
		servo++;
		//if(servo%100 == 1) cout<<servo<<endl;
		footpedal = comm.Check_Flag(FPEDAL_RIGHT) ? TRUE : FALSE;
		t  = dhdGetTime ();
		dt = t - t0;
		t0 = t;
		for (d=0; d<NumDevice; d++) {
			bttn[d]=0;
			//	if (dhdGetButtonMask(devHandle[d]))
			//	bttn[d] = 1;
			//else
			//bttn[d] = -1;
			double    x,y,z;

			// select active device
			dhdSetDevice (devHandle[d]);
			if (dhdSetForceAndTorqueAndGripperForce (0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0) < DHD_NO_ERROR) {
				printf ("error: cannot set force (%s)\n", dhdErrorGetLastStr());
			}
			// retrieve orientation frame (identity for 3-dof devices)
			dhdGetOrientationFrame (r);
			//			DeviceRotGlobal[d]  << r[0][0], r[0][1], r[0][2],
			//			r[1][0], r[1][1], r[1][2],
			//		r[2][0], r[2][1], r[2][2];
			xform  << r[0][0], r[0][1], r[0][2],
				r[1][0], r[1][1], r[1][2],
				r[2][0], r[2][1], r[2][2];
			Matrix3d xinv;
			xinv<< -1 , 0 , 0 ,
				0 ,1 , 0,
				0, 0, -1;
			qPrev[d]=qCurr[d];
			qCurr[d]=xinv*Quaterniond(xform);


			for(i=0;i<3;i++)
				oldPosGlobal[d][i]=newPosGlobal[d][i];
			dhdGetGripperThumbPos (&x,&y,&z);
			newPosGlobal[d][0] << x,y,z;
			dhdGetGripperFingerPos (&x,&y,&z);
			newPosGlobal[d][1] << x,y,z;
			dhdGetPosition (&x, &y, &z);
			newPosGlobal[d][2] << x,y,z;
			oldGrip[d]=newGrip[d];
			dhdGetGripperAngleRad(&newGrip[d]);
			if( comm.Check_Flag(FPEDAL_RIGHT) && comm.Check_Flag(BASIC_START) )
			{
				// get position increment
				pos_delta[d] = newPosGlobal[d][2] - oldPosGlobal[d][2];
				//qIncr[d]=qPrev[d].inverse()*qCurr[d];
				qIncr[d]=qCurr[d]*qPrev[d].inverse();
				//grasp[d]=(newGrip[d]-oldGrip[d])*5000;
				grasp[d]=(newGrip[d])*2*3000;

				//	if (!dhdIsLeftHanded())
				//grasp[d]=-grasp[d];
				bttn[d]=grasp[d];
				if(servo%1000 == 1)
					cout<<grasp[d]<<endl;
				//cout<<pos_delta[d]<<endl;
			} 
			else 
			{
				indexing=true;
				// Set zero increments
				pos_delta[d]<< 0 , 0 ,0 ;
				//	bttn[d] = 0;
				qIncr[d]=qPrev[d].Identity();
			}
			rl2sui = 3-comm.Check_Flag(FPEDAL_RIGHT);

			// compute projected force on each gripper finger
			Vector3d force;
			double    gripperForce = 0.0;
			force = ForceGlobal[0] + ForceGlobal[1];
			Vector3d gripdir = newPosGlobal[d][1] - newPosGlobal[d][0];
			Vector3d oldgripdir = newPosGlobal[d][1] - newPosGlobal[d][0];


			// if force is not null
			if (gripdir.norm() > 0.00001) {

				// project force on mobile gripper finger (forceGlobal[1]) onto gripper opening vector (gripdir)
				gripdir.normalize ();
				Vector3d gripper = (ForceGlobal[1].dot (gripdir) / (gripdir.squaredNorm())) * gripdir;
				gripperForce = gripper.norm();

				// compute the direction of the force based on the angle between
				// the gripper force vector (gripper) and the gripper opening vector (gripdir)
				if (force.norm() > 0.001) {
					double cosangle = gripdir.dot (gripper) / (gripdir.norm()*gripper.norm());
					if      (cosangle >  1.0) cosangle =  1.0;
					else if (cosangle < -1.0) cosangle = -1.0;
					double angle = acos(cosangle);
					if ((angle > M_PI/2.0) || (angle < -M_PI/2.0)) gripperForce = -gripperForce;
				}
			}

			// invert if necessary for left-handed devices
			if (dhdIsLeftHanded()) gripperForce = -gripperForce;

		}
		if(comm.Check_Flag(BASIC_PROGRAM)) { // when UI allows 'Server' run
			comm.Update_UDP_Data(pos_delta, qIncr, grasp, bttn, footpedal, servo, xfs );  // Update robot command packet
			comm.Send_UDP();
		}

		if(servo%100 == 1) {
			comm.Update_MA2UI(pos_delta, rl2sui, servo);			// Update SUI data
			comm.Send_TCP();
		}

		// apply all forces at once
		//   dhdSetForceAndGripperForce (force(0), force(1), force(2), gripperForce);

	}
	// close connection with haptic device
	for (d=0; d<NumDevice; d++) dhdClose (devHandle[d]);
	// simulation is now exiting
	SimulationFinished = true;
	// return
	return NULL;
}


void StartSimulation ()
{
	// create a high priority haptic thread
#if defined(WIN32) || defined(WIN64)
	DWORD ThreadId;
	CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)(HapticsLoop), NULL, NULL, &ThreadId);
	SetThreadPriority(&ThreadId, THREAD_PRIORITY_ABOVE_NORMAL);
	cout<<1<<endl;
#else
	pthread_t handle;
	pthread_create (&handle, NULL, HapticsLoop, NULL);
	struct sched_param sp;
	memset (&sp, 0, sizeof(struct sched_param));
	sp.sched_priority = 10;
	pthread_setschedparam (handle, SCHED_RR, &sp);
#endif
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