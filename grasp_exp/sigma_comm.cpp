/*****************************************************************************

omni_comm.h/omni_comm.cpp

These files declare and define objects for communicating with the 
robot slave and the Surgical User Interface.

Class created by KoSy
Modified by Hawkeye

*****************************************************************************/

#define _CRT_SECURE_NO_WARNINGS 1
//#include "stdafx.h"
#include <iostream>
#include <assert.h>

#include <cstdio>
#include <cassert>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
//#include <window.h>
#include <windows.h>
#include "PracticalSocket/PracticalSocket.h"
#include "../VS_GUI_SERVER/CommonDS.h"
#include "ITPteleoperation.h"
#include "Haptic.h"
#include "sigma_comm.h"
#include "dhdc.h"
#include "VisionThread2.h"
#define FALSE 0
#define TRUE 1
#define OMNI1 1
#define OMNI2 0
#define TIMEOUT_SECS 0
#define TIMEOUT_USECS 100

#define THOUSAND 1000000
#define MILLION  (THOUSAND*THOUSAND)
#define round(x) (x)

#define ADDR_BUBBLES   "127.0.0.1"
#define ADDR_BUTTERCUP "127.0.0.1"

//#define ADDR_BUBBLES   "132.72.216.156"
//#define ADDR_BUTTERCUP "132.72.216.40"
extern HapticData hapticData;
extern stMA2UI_DATA Ma2UIdata;
extern stUI2MA_DATA UI2Madata;
extern unsigned short c_port_UI;
extern const char* c_addr_UI;
extern Vector3d zerobase;
v_struct ravenData;
// Three camera rotation angles
//extern double th1, th2, th3;
extern int useITP;

//Global variable for world coordinate scale uniform in all 3 axis.
extern double scale_pos;
extern double scale_grip;
extern double base_x;
extern double base_y;
extern double base_z;
extern int status;
extern int laststatus;
///////////////////////////////////////////////////////////////////
// Implementation 
///////////////////////////////////////////////////////////////////
Sigma_Comm::Sigma_Comm(stMA2UI_DATA* pma2ui, stUI2MA_DATA* pui2ma)
{
	pMa2UIdata=pma2ui;
	pUI2Madata=pui2ma;

	//addr_Robot = ADDR_BUBBLES;
	strcpy(addr_Robot, ADDR_BUBBLES);
	port_Robot = 36000;
	iUDPaddr=0;
}


Sigma_Comm::~Sigma_Comm()
{
}


void Sigma_Comm::Set_Flag(int value, int mode)
{
	if(value == TRUE)
		pUI2Madata->flag01 |= mode;
	else if(value == FALSE)
		pUI2Madata->flag01 &= (ALL_ONES - mode);
}


bool Sigma_Comm::Check_Flag(int mode)
{
	return pUI2Madata->flag01&mode ? TRUE : FALSE;
}


int Sigma_Comm::Update_MA2UI(Vector3d pos[2],int rl, unsigned int servo,  int graspStatus, int status, int currentobject, int nextobject, int trialnumber, int fps)
{
	pMa2UIdata->delx[0] = int(round(pos[OMNI1][0]*THOUSAND));
	pMa2UIdata->dely[0] = int(round(pos[OMNI1][1]*THOUSAND));
	pMa2UIdata->delz[0] = int(round(pos[OMNI1][2]*THOUSAND));
	pMa2UIdata->delx[1] = int(round(pos[OMNI2][0]*THOUSAND));
	pMa2UIdata->dely[1] = int(round(pos[OMNI2][1]*THOUSAND));
	pMa2UIdata->delz[1] = int(round(pos[OMNI2][2]*THOUSAND));
	pMa2UIdata->expstatus = status; 
	pMa2UIdata->graspstatus = graspStatus; 
	pMa2UIdata->currentobject = currentobject; 
	pMa2UIdata->nextobject = nextobject; 
	pMa2UIdata->trialnumber = trialnumber; 
	pMa2UIdata->tick = servo; 
	pMa2UIdata->runlevel = rl;
	pMa2UIdata->checksum = pMa2UIdata->delx[0] +  pMa2UIdata->dely[0] +  pMa2UIdata->delz[0] +  pMa2UIdata->runlevel + (int)pMa2UIdata->tick;   
	pMa2UIdata->fps = fps;
	return 1;
}


int Sigma_Comm::Check_UI2MA(int message)
{
	if(pUI2Madata->zero)
	{
		laststatus=status;
		status=10;
	}
	zerobase<< pUI2Madata->zeroX,pUI2Madata->zeroY,pUI2Madata->zeroZ;
	if(pUI2Madata->checksum == (pUI2Madata->scale_pos + pUI2Madata->scale_grip + pUI2Madata->flag01 + pUI2Madata->UDPaddr+ (int)pUI2Madata->tick)) { 
		scale_pos= (double)pUI2Madata->scale_pos/100.0;
		scale_grip= (double)pUI2Madata->scale_grip;

		// Set camera angle 1
		int count=0;
		if (base_x != pUI2Madata->camAngle[0]){
			base_x = pUI2Madata->camAngle[0];
			cout << "Camera angle 1 changed to "<< base_x <<" degrees.\n";
			count++;
		}
		if (base_y != pUI2Madata->camAngle[1]){
			base_y = pUI2Madata->camAngle[1];
			cout << "Camera angle 2 changed to "<< base_y <<" degrees.\n";
			count++;
		}
		if (base_z != pUI2Madata->camAngle[2]){
			base_z = pUI2Madata->camAngle[2];
			cout << "Camera angle 3 changed to "<< base_z <<" degrees.\n";
			count++;
		}
		/*if (count>0)
		hapticData.SetHapticBaseAngle(base_x,base_y,base_z);*/
		if (hapticData.enable_gripper != pUI2Madata->enable_grip){
			hapticData.enable_gripper = pUI2Madata->enable_grip;
			if (hapticData.enable_gripper)
			cout << "enabled gripper"<<endl;
			else
				cout << "disabled gripper"<<endl;
		}
				if (hapticData.enable_position != pUI2Madata->enable_position){
			hapticData.enable_position = pUI2Madata->enable_position;
			if (hapticData.enable_position)
			cout << "enabled position"<<endl;
			else
				cout << "disabled position"<<endl;
		}
						if (hapticData.enable_orientation != pUI2Madata->enable_orientation){
			hapticData.enable_orientation = pUI2Madata->enable_orientation;
			if (hapticData.enable_orientation)
			cout << "enabled orientation"<<endl;
			else
				cout << "disabled orientation"<<endl;
		}
		if (hapticData.fullscreen != pUI2Madata->fullscreen){
			hapticData.fullscreen = pUI2Madata->fullscreen;
			vsScreenSize (hapticData.fullscreen);
				}
		if(pUI2Madata->setCamOff)
		{
			double offx[2]={pUI2Madata->camOffLX,pUI2Madata->camOffRX};
			double offy[2]={pUI2Madata->camOffLY,pUI2Madata->camOffRY};
			vsCamOff(offx,offy);
		}
		// Set camera angle 2
		//if (th2 != pUI2Madata->camAngle[1]){
		//	th2 = pUI2Madata->camAngle[1];
		//	cout << "Camera angle 2 changed to "<< th2 <<" degrees.\n";
		//}
		//// Set camera angle 3
		//if (th3 != pUI2Madata->camAngle[2]){
		//	th3 = pUI2Madata->camAngle[2];
		//	cout << "Camera angle 3 changed to "<< th3 <<" degrees.\n";
		//}
		//if (useITP != pUI2Madata->useITP) {
		//	useITP = pUI2Madata->useITP;
		//	if (useITP)
		//		cout << "Using ITP coordinate reference.\n";
		//	else
		//		cout << "Using Omni native coordinate reference.\n";
		//}

		// Robot's IP address Update
		if(iUDPaddr!=pUI2Madata->UDPaddr) {
			iUDPaddr = pUI2Madata->UDPaddr;
			sprintf(addr_Robot,"%d.%d.%d.%d",(unsigned int)((iUDPaddr&0xff000000)>>24),(unsigned int)((iUDPaddr&0xff0000)>>16), (unsigned int)((iUDPaddr&0xff00)>>8),(iUDPaddr&0x00ff));
			cout << "Surgical Robot's IP is chaged into " << addr_Robot <<endl;
		}
		if(message) {
			cout << "Received Data is fine. " << endl;
		}
		return TRUE;
	} else {
		pUI2Madata->flag01 &= (ALL_ONES - FPEDAL_RIGHT);
		cout << " Data is strange !!" << endl;
		return FALSE;
	}
}


int Sigma_Comm::Initialize_TCP_GUI()
{
	Sleep(1);

	cout << "Initializing TCP server... " << endl;
	tcpsocket = new TCPSocket;
	try {
		tcpsocket->connect(c_addr_UI, c_port_UI);
	}
	catch( SocketException SE ) {
		cout << "Failed to connect TCP server.\n";
		return FALSE;
	}
	cout << "TCP connection started. ^^" << endl;

	Vector3d zero_pos[2];
	zero_pos[0] << 0,0,0;
	zero_pos[1] << 0,0,0;

	Update_MA2UI(zero_pos, 0, 0,0,0,0,0,0,0);

	Send_TCP();
	Recv_TCP();


	return (Check_UI2MA(1) ? TRUE : FALSE);
}


void Sigma_Comm::Send_TCP()
{
	try{
		tcpsocket->send(&Ma2UIdata, sizeof(Ma2UIdata));
	}
	catch (SocketException SE){
		cerr << "failed to send_tcp\n";
	}
}


void Sigma_Comm::Recv_TCP()
{
	try {
		tcpsocket->recv(&UI2Madata, sizeof(UI2Madata));
	}
	catch (SocketException SE){
		cerr << "failed to recv_tcp\n";
	}
}

int Sigma_Comm::Initialize_UDP_Robot(int argc, char*argv[])
{
	strcpy(addr_Robot,ADDR_BUTTERCUP);
	cout << "Server= " << addr_Robot << endl;
	cout << "Scale Position factor= " << scale_pos << endl;
	cout << "Scale Gripper factor= " << scale_grip << endl;

	return 1;
}


void Sigma_Comm::Send_UDP()
{

	if(Check_Flag(BASIC_START)) {
		try {
				udpsocket.sendTo(&msgHeader,sizeof(u_struct),addr_Robot,port_Robot);
				//cout << "out" << endl;
			}
		
		catch (SocketException SE){
			cerr << "failed to send_UDP\n";
		}
	}
}

bool Sigma_Comm::Recv_UDP()
{
		fd_set ravenSet;
	TIMEVAL timeout = {TIMEOUT_SECS, TIMEOUT_USECS};
	int nfound;
	int the_udpsock;
	// Initialize FD_SET //
	FD_ZERO(&ravenSet);
	the_udpsock = udpsocket.sockDesc;
	FD_SET(the_udpsock, &ravenSet);
	if(Check_Flag(BASIC_START)) {
		try {
			//udpsocket.sendTo(&msgHeader,sizeof(u_struct),addr_Robot,port_Robot);
			timeout.tv_sec = TIMEOUT_SECS;
			timeout.tv_usec = TIMEOUT_USECS;
			if(select( 1 , &ravenSet, NULL, NULL, &timeout ))
			{
			memset(&rcvHeader,0,sizeof(v_struct));
			udpsocket.recv(&rcvHeader,sizeof(v_struct));
			//cout<<ravenData.px[0]<<"\t"<<rcvHeader.px[0]<<"\t"<<ravenData.grasp[0]<<"\t"<<rcvHeader.grasp[0]<<endl;

			ravenData=rcvHeader;
			//cout << "in" << endl;
			}
//			else
				//cout << "in2" << endl;
		}
		catch (SocketException SE){
			cerr << "failed to get_UDP\n";
			return false;
		}
	}
	return true;
}

void Sigma_Comm::Update_UDP_Data(Vector3d pos[2], Quaterniond qIncr[2],double grasp[2], int bttn[2], int fp, unsigned int servo, Matrix4d xfs[2])
{

	static int i;
	msgHeader.delx[0]		= -int(round(pos[OMNI1][0]*THOUSAND));
	msgHeader.dely[0]		= int(round(pos[OMNI1][1]*THOUSAND));
	msgHeader.delz[0]		= -int(round(pos[OMNI1][2]*THOUSAND));
	//msgHeader.Qx[0]			= int(round( qIncr[OMNI1].v()[0]*1e9 ));
	//msgHeader.Qy[0]			= int(round( qIncr[OMNI1].v()[1]*1e9));
	//msgHeader.Qz[0]			= int(round( qIncr[OMNI1].v()[2]*1e9));
	//msgHeader.Qw[0]			= int(round( qIncr[OMNI1].s()*1e9 ));
	msgHeader.Qx[0]			=  qIncr[OMNI1].x();
	msgHeader.Qy[0]			=  qIncr[OMNI1].y();
	msgHeader.Qz[0]			=  qIncr[OMNI1].z();
	msgHeader.Qw[0]			=  qIncr[OMNI1].w();
	//msgHeader.Qx[0]			=  0;
	//msgHeader.Qy[0]			=  0;
	//msgHeader.Qz[0]			=  0;
	//msgHeader.Qw[0]			=  0;
	//msgHeader.transform[0]			=  xfs[0];
	//msgHeader.transform[1]			=  xfs[1];

	msgHeader.delx[1]		= -int(round(pos[OMNI2][0]*THOUSAND));
	msgHeader.dely[1]		= int(round(pos[OMNI2][1]*THOUSAND));
	msgHeader.delz[1]		= -int(round(pos[OMNI2][2]*THOUSAND));
	msgHeader.Qx[1]			=  qIncr[OMNI2].x();
	msgHeader.Qy[1]			=  qIncr[OMNI2].y();
	msgHeader.Qz[1]			=  qIncr[OMNI2].z();
	msgHeader.Qw[1]			=  qIncr[OMNI2].w();
	//msgHeader.Qx[1]			= int(round( qIncr[OMNI2].v()[0]*1e9 ));
	//msgHeader.Qy[1]			= int(round( qIncr[OMNI2].v()[1]*1e9 ));
	//msgHeader.Qz[1]			= int(round( qIncr[OMNI2].v()[2]*1e9 ));
	//msgHeader.Qw[1]			= int(round( qIncr[OMNI2].s()*1e9 ));
	//msgHeader.Qx[1]			=  qIncr[OMNI2].v()[0];
	//msgHeader.Qy[1]			=  qIncr[OMNI2].v()[1];
	//msgHeader.Qz[1]			=  qIncr[OMNI2].v()[2];
	//msgHeader.Qw[1]			=  qIncr[OMNI2].s();
	//msgHeader.Qx[1]			=  0;
	//msgHeader.Qy[2]			=  0;
	//msgHeader.Qz[3]			=  0;
	//msgHeader.Qw[4]			=  0;

	msgHeader.buttonstate[OMNI1]= bttn[OMNI2];
	msgHeader.buttonstate[OMNI2]= bttn[OMNI2];
	msgHeader.grasp[OMNI1]= grasp[OMNI2];
	msgHeader.grasp[OMNI2]= grasp[OMNI2];
	msgHeader.sequence=servo;
	msgHeader.surgeon_mode=fp;
	msgHeader.checksum=checksumUDPData();
}
//void Update_UDP_Raven(v_struct *rcvdata,v_struct *ravendata)
//{
//
//}

int Sigma_Comm::checksumUDPData(){
	int chk=0;
	chk =  (msgHeader.surgeon_mode);
	chk += (msgHeader.delx[0])+(msgHeader.dely[0])+(msgHeader.delz[0]);
	chk += (msgHeader.delx[1])+(msgHeader.dely[1])+(msgHeader.delz[1]);
	chk += (msgHeader.buttonstate[0]);
	chk += (msgHeader.buttonstate[1]);
	chk += (int)(msgHeader.sequence);
	return chk;
}