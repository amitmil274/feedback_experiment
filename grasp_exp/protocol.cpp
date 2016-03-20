
#include "Protocol.h"
#include "FileHandle.h"
#include "Timer.h"
#include "Haptic.h"
#include <vector>
#include "dhdc.h"
#include "drdc.h"
#include <Eigen/Geometry>
#include "Eigen/Eigen"
#include <stdio.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <Windows.h>
#include <windows.h>
#include "..\VS_GUI_SERVER\CommonDS.h"
#include "ITPteleoperation.h"
#include <conio.h>
extern stMA2UI_DATA Ma2UIdata;
extern stUI2MA_DATA UI2Madata;
using namespace std;
using namespace Eigen;
Protocol protocol;
extern double dr;
extern FileHandle filemaster;
extern Timer timer;

extern HapticData hapticData;
int trialnumber;
int currentobject;
int nextobject;
extern bool count1;
//extern int last_status;
extern double CurrentTime;

extern bool WriteToFile;

extern int TimeToStop;

int temperature_no = 0;
double safety_temperature = 0.80;
double OVER_temperature = 0.98;

float currentTime;


extern int status;
extern int laststatus;

extern int graspStatus;
extern int TargNum;
extern int devHandle;
bool trialstart;

int buttonflag;
bool teleop = true;
bool vision = false;
double nullPose[DHD_MAX_DOF] = { 0.08, -0.03, 0.0,  // base  (translations)
	0.0, 0.0, M_PI/4,  // wrist (rotations)
	0.1 };          // gripper
double nullbase[3] = { 0.0, 0.0, 0.0} ; // base  (translations)
double nullrot[3] = {	0.0, 0.0, M_PI/4} ; // wrist (rotations)
//0.1 };          // gripper
extern bool IsCalibrated;
bool calibflag=false;
void Protocol::run()
{
	//double baseang[3]={0,0,0};
	double Pos0[8]={0,0,0,0,0,0,0,0};

	Vector3d Pos=hapticData.position[0];
	Vector3d Vel=hapticData.velocity[0];
	double Grip=hapticData.gripper[0];

	switch(status)
	{
	case 0:
		//calibration
		if(!IsCalibrated)
		{
			if(!calibflag)
			{
				cout<<"please calibrate the starting position of the raven"<<endl;
				cout<<"When Calibrated Please press the button"<<endl;
				calibflag = true;
			}
			InitPosTimeEp=0.5;
			dhdEmulateButton(DHD_ON);
			buttonflag=dhdGetButton(0);
			if(buttonflag==DHD_ON)
			{
				if (InitLocFlag)
				{
					InitLocTime=timer.time;
					InitLocFlag=false;
				}
			}
			else
				InitLocFlag=true;
			if (timer.time-InitLocTime>InitPosTimeEp && buttonflag==DHD_ON)
			{
				InitLocFlag=true;
				IsCalibrated = true;
			}
		}
		else
		{
			timer.reset();
			timer.StartTime=timer.time;
			status = 1;
			cout<<"Trial: "<<filemaster.TrialNum<<endl;
		}
		break;

	case 1: // actual grasping
		trialnumber=filemaster.TrialNum;
		if (trialnumber<filemaster.TrialEndActual) 
			nextobject=filemaster.TargInd[trialnumber];
		else
			nextobject=(filemaster.TargInd[trialnumber]);
		currentobject=filemaster.TargInd[trialnumber-1];

		switch (graspStatus) // reset position
		{
		case 0: // reset position 
			filemaster.OpenDataFile();
			teleop = false;
			Sleep(500);
			graspStatus++;
			buttonflag=DHD_OFF;
			break;
		case 1: // command and 	
			teleop = false;
			ZeroVelFlag=true;
			StartPosEp=0.01;
			InitPosTimeEp=2;
			dhdEmulateButton(DHD_ON);
			buttonflag=dhdGetButton(0);
			if(buttonflag==DHD_ON)
			{
				if (InitLocFlag)
				{
					InitLocTime=timer.time;
					InitLocFlag=false;
				}
			}
			else
				InitLocFlag=true;
			if (timer.time-InitLocTime>InitPosTimeEp && buttonflag==DHD_ON)
			{
				graspStatus=2;
				InitLocFlag=true;
				InitLocTime=timer.time; 
				buttonflag=DHD_OFF;
			}
			break;
		case 2: // command and 
			WriteToFile = true;
			trialstart = true;
			if ((timer.time-CurrentWriteFreqTime)>=filemaster.limit)
			{
				CurrentWriteFreqTime=timer.time;
				filemaster.SaveDataFile(timer.time);
			}
			teleop = false;
			ZeroVelFlag=true;
			StartPosEp=0.01;
			InitPosTimeEp=0.5;
			if (timer.time-InitLocTime>InitPosTimeEp)
			{
				graspStatus=3;
				dhdEmulateButton(DHD_OFF);
				buttonflag=DHD_OFF;
			}
			break;
		case 3:

			if (ZeroVelFlag)
			{
				StartTargTime=timer.time;
				ZeroVelFlag=false;
			}
			if ((timer.time-CurrentWriteFreqTime)>=filemaster.limit)
			{
				CurrentWriteFreqTime=timer.time;
				filemaster.SaveDataFile(timer.time);
			}
			TargVelEp=1;
			TargTimeEp=4; // the time required to be in zero vel on the target, until the servo starts
			if (timer.time-StartTargTime>TargTimeEp)// && dr<0.02)
			{
				if( _kbhit() )							// check for keyboard input`
				{
					char ch = _getch();
					if (ch==32)
					{
						graspStatus=4;
						GraspTrials++;
						cout<<"GraspTrials: "<<GraspTrials<<endl;
						filemaster.TrialNum++;
						cout<<"Trial: "<<filemaster.TrialNum<<endl;
						WriteToFile = false;
						filemaster.CloseOpenDataFile();
						buttonflag=DHD_OFF;
					}
				}		
			}			
			break;
		case 4: // checking number of graspings finished
			if (GraspTrials<filemaster.TrialEndActual)
			{	
				graspStatus=0;	
				ZeroVelFlag = true;
				buttonflag=DHD_OFF;
			}
			else
			{
				status++;
				graspStatus=0;
				GraspTrials=0; // initialize the number of reach trials in block
				FinishActualGrasp=true;
				buttonflag=DHD_OFF;
			}
			break;
		}
		break;
	case 2: // percept grasping
		trialnumber=filemaster.TrialNum;
		if (trialnumber<filemaster.TrialEnd) 
			nextobject=filemaster.TargInd[trialnumber];
		else
			nextobject=-100;
		currentobject=filemaster.TargInd[trialnumber-1];
		switch (graspStatus) // reset position
		{
		case 0: // reset position 
			filemaster.OpenDataFile();
			teleop = false;
			Sleep(500);
			graspStatus++;
			buttonflag=DHD_OFF;
			break;
		case 1: // command and 
			teleop = false; 
			ZeroVelFlag=true;
			StartPosEp=0.01;
			InitPosTimeEp=1;
			dhdEmulateButton(DHD_ON);
			buttonflag=dhdGetButton(0);
			if(buttonflag==DHD_ON)
			{
				if (InitLocFlag)
				{
					InitLocTime=timer.time;
					InitLocFlag=false;
				}
			}
			else
				InitLocFlag=true;
			if (timer.time-InitLocTime>InitPosTimeEp && buttonflag==DHD_ON)
			{
				graspStatus=2;
				InitLocFlag=true;
				WriteToFile = true;
				trialstart = true;
				buttonflag=DHD_OFF;
			}
			break;
		case 2: // command and 
			if ((timer.time-CurrentWriteFreqTime)>=filemaster.limit)
			{
				CurrentWriteFreqTime=timer.time;
				filemaster.SaveDataFile(timer.time);
			}
			teleop = false;
			ZeroVelFlag=true;
			InitLocFlag=true;
			if( _kbhit() )							// check for keyboard input`
			{
				char ch = _getch();
				if (ch==32)
				{
					graspStatus=3;
					filemaster.SaveDataFile(-1.11111);
					filemaster.SaveDataFile(-1.11112);
					filemaster.SaveDataFile(-1.11113);
					dhdEmulateButton(DHD_OFF);
					buttonflag=DHD_OFF;
				}
			}
			break;
		case 3:
			if ((timer.time-CurrentWriteFreqTime)>=filemaster.limit)
			{
				CurrentWriteFreqTime=timer.time;
				filemaster.SaveDataFile(timer.time);
			}
			TargVelEp=1;
			TargTimeEp=3; // the time required to be in zero vel on the target, until the servo starts
			if (timer.time-StartTargTime>TargTimeEp )//&& dr<0.02)
			{
				if( _kbhit() )							// check for keyboard input`
				{
					char ch = _getch();
					if (ch==32)
					{
						graspStatus=4;
						GraspTrials++;
						cout<<"GraspTrials: "<<GraspTrials<<endl;
						filemaster.TrialNum++;
						cout<<"Trial: "<<filemaster.TrialNum<<endl;
						WriteToFile = false;
						filemaster.CloseOpenDataFile();
						buttonflag=DHD_OFF;
					}
				}
			}
			break;
		case 4: // checking number of graspings finished
			if (GraspTrials<filemaster.TrialEndActual+1)
			{	
				graspStatus=0;	
				ZeroVelFlag = true;
			}
			else
				status++;
			break;
		}
		break;
	case 3:
		count1 = false;
		WriteToFile=false;
		status = 10;
		printf("\n\n   Experiment Done!\n\n");
		TimeToStop=1;
		break;
	case 4:
		count1 = false;
		WriteToFile=false;
		status = 1000;
		printf("\n\n   Experiment Done!\n\n");
		TimeToStop=1;
		break;
	case 10: // ZERO THE POSITION
		drdRegulatePos  (true);
		drdStart();
		drdMoveToPos(0,0,0);
		drdStop(true);
		status=laststatus;
		break;
	}
}
