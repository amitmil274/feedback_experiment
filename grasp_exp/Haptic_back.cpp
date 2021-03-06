﻿#include <stdio.h>
#include "timer.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <windows.h>
#include <list>
#include <vector>
#include "Haptic.h"
#include "FileHandle.h"
#include "Protocol.h"
#include "dhdc.h"
#include "drdc.h"
#include <Eigen/Geometry>
#include "Eigen/Eigen"
#include "PracticalSocket\PracticalSocket.h"
#include "sigma_comm.h"
#include "ITPteleoperation.h"
#include "..\VS_GUI_SERVER\CommonDS.h"
#include "VisionThread3.h"
using namespace std;
using namespace Eigen;

HapticData hapticData;
extern v_struct ravenData;
extern FrameRateCounter FPS;
extern FileHandle filemaster;
extern FileHandle fileslave;
extern Protocol protocol;
extern Timer timer;
extern int status;
extern int laststatus;
extern double gRampupRate1;
extern int graspStatus;
Matrix4d xfs;
int footpedal, prevpedal;
 bool filterRdy=false;
bool oneOmni=false;
bool leftDevice=false;
bool rightDevice=false;
bool greenArm=false;
bool goldArm=false;
bool firstime=true;
extern int trialnumber;
extern int currentobject;
extern int nextobject;
//double nullPose[DHD_MAX_DOF] = { 0.08, -0.03, 0.0,  // base  (translations)
//                                 0.0, 0.0, 0.0,  // wrist (rotations)
//                                 0.0 };          // gripper

double gDeviceOffset1X=0;// 0.063;//0.07
double gDeviceOffset1Y=0;// 0.06;
double gDeviceOffset1Z=0;// 0.062;
double gMaxForceRatio= 1.0;
const double PI = 3.1415926535897932384626433832795;
double ang=PI/4;
extern double nullPose[DHD_MAX_DOF];
bool WriteToFile=false;
double CurrentTime;
int devHandle;
bool ExpOn = false;
//extern bool ServoFlag;
//extern float initReachPosX;
//extern float initReachPosY;

//extern bool HitFlag;
//extern float VelX;
//extern float VelY;
//extern float VelPreX;
//extern float VelPreY;

extern stMA2UI_DATA Ma2UIdata;
extern stUI2MA_DATA UI2Madata;
double scale_pos;
double scale_grip;
double base_z;
double base_y;
double base_x;
Vector3d zerobase(0,0,0);

extern Sigma_Comm comm;
extern int g_footpedal;
extern bool SimulationOn;
unsigned int servo=0;
bool indexing= false;
bool IsCalibrated = false;
extern double nullbase[3];
extern double nullrot[3];
double dr;
void* HapticsLoop(void* pUserData)
{

	static double t0 = dhdGetTime ();
	double        t  = t0 + 0.001;
	double        dt,grasp=0;
	double newGrip=0;
	double x,y,z;
	double vx,vy,vz;
	double rx,ry,rz;
	bool          initialized = false;

	int bttn=0;
	int rl2sui;
	double   r[3][3];
	Vector3d Position[2], oldPosition[2], dPosition[2], ravenPosition[2],deltaPosition[2];
	bool firstraven=false;
	Vector3d Velocity[2];
	Vector3d Orientation[2];
	double Gripper[2];
	double Vgrip[2];
	//static Vector3d oldPosistion[2];
	static Vector3d oldVelocity[2];
	Quaterniond qIncr[2],qCurr[2],qPrev[2];
	Matrix3d xform[2];
	Vector3d radius[2];
	radius[0]<< 0.0, -0.02, 0.0;
	int servo=0;
	int K=50;
	double d[3]={0,0,0};
	// start haptic simulation
	SimulationOn       = true;
	//	SimulationFinished = false;

	// start with no force
	dhdEnableForce (DHD_ON, devHandle);
	dhdSetDevice(devHandle);
	double oldGrip[3]={0,0,0};
	double oldFiltGrip[3]={0,0,0};
	double filtGrip=0;
	    float B[] = {0.0002196,  0.0006588,  0.0006588,  0.0002196};
    float A[] = {1.0000,   2.7488, -2.5282,  0.7776};
	while(SimulationOn)
	{
		servo++;
		//if(servo%100 == 1) cout<<servo<<endl;
		prevpedal= footpedal;
		footpedal = comm.Check_Flag(FPEDAL_RIGHT) ? TRUE : FALSE;
		t  = dhdGetTime ();
		dt = t - t0;
		t0 = t;
		// POSITION AND VELOCITY OF END EFFECTOR
		dhdGetPosition(&x,&y,&z);
		oldPosition[0]=Position[0];
		Position[0] << x,y,z;
		dhdGetLinearVelocity(&vx,&vy,&vz);
		Velocity[0] << vx,vy,vz;		
		//Position *=0.001;	// mm->m
		//	Velocity *=0.001;	// mm/s->m/s
		// GRIPPER POS and VELOCITY
		dhdGetGripperAngleRad(&Gripper[0]);
		dhdGetGripperGap(&Gripper[1]);
		//cout<<Gripper[1]<<endl;
	
		d[0]=x-zerobase.x();
		d[1]=y-zerobase.y();
		d[2]=z-zerobase.z();
	
		dr=sqrt(d[1]*d[1]+d[2]*d[2]+d[0]*d[0]);
		if ((graspStatus<4 && graspStatus>1 && status!=10))
		{
			K=0;
		}
		else
		{
			K=4/dr;
			if (K>200)
				K=200;			
		}
		double KG;
		// DAVINCI LIKE GRIPPER FORCE FEEDBACK
		int gripFB=2;
		if (gripFB==1)
		{
		
		if(Gripper[0]<0.1)
			KG=40;
		else if(Gripper[0]<0.2)
			KG=4/Gripper[0];
		else
			KG=20;
			if (dhdSetForceAndGripperForce (K*-d[0], K*-d[1], K*-d[2],(0.5-Gripper[0])*KG) < DHD_NO_ERROR) 
				printf ("error: cannot set force (%s)\n", dhdErrorGetLastStr());
			// END OF DAVINCI LIKE GRIPPER FORCE FEEDBACK
	}
	else
	{

			// POSITION EXCHANGE GRIPPER FORCE FEEDBACK
			double tmpGrip=Gripper[0];
			tmpGrip-=3*M_PI/180;
			tmpGrip*=scale_grip;
			double gripDiff=-(tmpGrip-ravenData.grasp[0]);
			
			 if (!filterRdy)

    {
        oldGrip[2] = gripDiff;
        oldGrip[1] = gripDiff;
        oldGrip[0] = gripDiff;
        oldFiltGrip[2] = gripDiff;
        oldFiltGrip[1] = gripDiff;
        oldFiltGrip[0] = gripDiff;
		filterRdy=true;
    }

    //Compute filtered motor angle
    filtGrip =
        B[0] * gripDiff  +
        B[1] * oldGrip[0] +
        B[2] * oldGrip[1] +
        B[3] * oldGrip[2] +
        A[1] * oldFiltGrip[0] +
        A[2] * oldFiltGrip[1] +
        A[3] * oldFiltGrip[2];
	 oldGrip[2] = oldGrip[1];
    oldGrip[1] = oldGrip[0];
    oldGrip[0] = gripDiff;
    oldFiltGrip[2] = oldFiltGrip[1];
    oldFiltGrip[1] = oldFiltGrip[0];
    oldFiltGrip[0] = filtGrip;
	KG=1;
	if (filtGrip>0)
		if (dhdSetForceAndGripperForce (K*-d[0], K*-d[1], K*-d[2],filtGrip*KG) < DHD_NO_ERROR) 
				printf ("error: cannot set force (%s)\n", dhdErrorGetLastStr());

	cout.setf(ios::fixed,ios::floatfield);
	cout.precision(3);
	cout << '\r' << "GF: " <<std::setw(8)<< filtGrip*KG << ' : '<<std::setw(8) << tmpGrip << ' : '<<std::setw(8)<< ravenData.grasp[0] << flush;
	}
		double gripForce=0;
			// END OF POSITION EXCHANGE GRIPPER FORCE FEEDBACK
		if (dhdSetForceAndGripperForce (K*-d[0], K*-d[1], K*-d[2],gripForce) < DHD_NO_ERROR) 
				printf ("error: cannot set force (%s)\n", dhdErrorGetLastStr());
		dhdGetGripperAngularVelocityRad(&Vgrip[0]);
		dhdGetGripperLinearVelocity(&Vgrip[1]);
				
		// ORIENTATION
		dhdGetOrientationFrame (r);
		dhdGetOrientationRad(&rx,&ry,&rz);
		Orientation[0] << rx,ry,rz;
		xform[0]  << r[0][0], r[0][1], r[0][2],
			r[1][0], r[1][1], r[1][2],
			r[2][0], r[2][1], r[2][2];
		Matrix3d xinv;
		if (hapticData.enable_orientation)
			xinv<< -1 , 0 , 0 ,
			0 ,1 , 0,
			0, 0, -1;
		else
			xinv<< 0 , 0 , 0 ,
			0 ,0 , 0,
			0, 0, 0;
		qPrev[0]=qCurr[0];
		qCurr[0]=xinv*Quaterniond(xform[0]);


		hapticData.position[0]=Position[0];
		hapticData.velocity[0]=Velocity[0];
		hapticData.gripper[0]=Gripper[0];
		hapticData.gripper[1]=Gripper[1];
		hapticData.vgrip[0]=Vgrip[0];
		hapticData.vgrip[1]=Vgrip[1];
		hapticData.orientation[0]=Orientation[0];		
		
		if (WriteToFile)
		{
			filemaster.position=Position[0];
			filemaster.velocity=Velocity[0];
			filemaster.gripper=Gripper[0];
			filemaster.vgrip=Vgrip[0];
			filemaster.orientation=Orientation[0];
		}
		// COMMUNICATION
		if( comm.Check_Flag(FPEDAL_RIGHT) && comm.Check_Flag(BASIC_START) )
		{
			if (prevpedal!=footpedal)
			{
				firstraven=false;
				ravenPosition[0].x()=(-ravenData.py[0]);
				ravenPosition[0].y()=(-ravenData.pz[0]);
				ravenPosition[0].z()=ravenData.px[0];
				deltaPosition[0]= (Position[0]-zerobase)-ravenPosition[0];
			}
				
			// get position increment
			if (hapticData.enable_position)
				//dPosition[0] = Position[0] - oldPosition[0];
				dPosition[0] = scale_pos*(Position[0]-zerobase) - deltaPosition[0];
			else
				dPosition[0] << scale_pos*(Position[0]-zerobase); //0,0,0;
			qIncr[0]=qCurr[0]*qPrev[0].inverse();
			//qIncr[0]=qCurr[0]; //TRYING TO IMPLEMENT ABSOLUTE
			if (hapticData.enable_gripper)
			{
				Gripper[0]-=3*M_PI/180;
				Gripper[0]*=1000*scale_grip;
			}
			else
				Gripper[0]=0;
		} 
		else 
		{
			ravenPosition[0].x()=(-ravenData.py[0]);
			ravenPosition[0].y()=(-ravenData.pz[0]);
			ravenPosition[0].z()=ravenData.px[0];
			indexing=true;
			dPosition[0] << ravenPosition[0];
			//dPosition[0]<< 0 , 0 ,0 ;
			qIncr[0]=qCurr[0]*qPrev[0].inverse();
			Gripper[0]-=3*M_PI/180;
			Gripper[0]*=1000*scale_grip;

		}

		rl2sui = 3-comm.Check_Flag(FPEDAL_RIGHT);

		// compute projected force on each gripper finger
		int grip2[2];
		grip2[0]=(int)Gripper[0];
		bool rcvudp;
		int fps = FPS.GetFrameRate();
		if(servo%100 == 1) {
			comm.Update_MA2UI(dPosition, rl2sui, servo, graspStatus, status, currentobject, nextobject, trialnumber,fps);			// Update SUI data
			comm.Send_TCP();
			//cout<<rl2sui;
		}
		if(comm.Check_Flag(BASIC_PROGRAM)) { // when UI allows 'Server' run
			comm.Update_UDP_Data(dPosition, qIncr, Gripper, grip2, footpedal, servo, &xfs );  // Update robot command packet
			comm.Send_UDP();
			rcvudp=comm.Recv_UDP();
			if(prevpedal>footpedal && !firstime)
				firstime=true;
			if (!firstraven && firstime && rcvudp)
			{
				firstime=false;
				firstraven=true;
			}
			


			//comm.Update_UDP_Raven(&rcvHeader);
		}
		//if (servo%200==1 && comm.Check_Flag(BASIC_PROGRAM))
		//{
		//	
		//	//protocol.run();
		//}
		

	}
	//SimulationFinished = true;
	// return
	return NULL;
}
//void HapticData::moveto()
//{
//				 drdRegulatePos  (true);
//          drdRegulateRot  (true);
//          drdRegulateGrip (true);
//    //      drdStart();
//          drdMoveTo(nullPose);
//      //    drdStop(true);
//		  			 drdRegulatePos  (false);
//          drdRegulateRot  (false);
//          drdRegulateGrip (false);
// 
//}
void HapticData::InitHapticDevice()
{
	int done = 0;
	int deviceCount;
	// get device count
	deviceCount = dhdGetDeviceCount ();
	if (deviceCount < 1) {
		printf ("error: %s\n", dhdErrorGetLastStr ());
		return;
	}
	oneOmni=true;
	greenArm=true;
	goldArm=false;
	leftDevice=false;
	// open the first available device
	if ((devHandle = dhdOpenID (0)) < 0){
		printf ("error: %s\n", dhdErrorGetLastStr ());
		printf("\nPress any key to quit.\n");
		getchar();
		return;
	}
	//drdRegulatePos  (true);
	//drdRegulateRot  (true);
	//drdRegulateGrip (true);
	//drdStart();
	//drdMoveTo(nullPose);
	//drdMoveToPos(nullbase[0],nullbase[1],nullbase[2]);
	//	drdMoveToRot(nullrot[0],nullrot[1],nullrot[2]);
	//drdStop(true);
	//SetHapticBaseAngle(0,0,0);
	//double newbase;
	//std::cin>>newbase;
	//dhdSetBaseAngleZDeg(newbase);*/
}
void HapticData::SetHapticBaseAngle(double x,double y, double z)
{
	double baseang[3];
	dhdSetBaseAngleXDeg(x);
	dhdSetDeviceAngleDeg(y);
	dhdSetBaseAngleZDeg(z);
	dhdGetBaseAngleXDeg(&baseang[0]);
	dhdGetDeviceAngleDeg(&baseang[1]);
	dhdGetBaseAngleZDeg(&baseang[2]);
	std::cout<<"new base angles:"<<std::endl;
	for(int i=0;i<3;i++)
		std::cout<<baseang[i]<<std::endl;
	double r = sqrt(nullbase[0]*nullbase[0]+nullbase[1]*nullbase[1]);
	//nullbase[0]=r*sin(baseang[2]/180*M_PI);
	//nullbase[1]=r*cos(baseang[2]/180*M_PI);
	//nullPose[0]=nullbase[0];
	//nullPose[1]=nullbase[1];

}
void Ori_feedback(Eigen::Vector3d ori)
{
	//if (graspStatus>2)
	//{
	double a[4]={*ravenData.Qw,*ravenData.Qx,*ravenData.Qy,*ravenData.Qz};
	double w=a[0];
	double x=a[1];
	double y=a[2];
	double z=a[3];
	Quaterniond newCurr(a);
	AngleAxisd theta(newCurr);
	Vector3d raven=theta.axis();
	//Vector3d raven(3,1);
	//raven << ravenData.roll[0], ravenData.pitch[0], ravenData.yaw[0]; 
	Vector3d force=-0.02*(ori-raven);
	double roll= atan2(2*w*y-2*x*z,1-2*y*y-2*z*z);
	double pitch= atan2(2*x*w-2*y*z,1-2*x*x-2*z*z);
	double yaw = asin(2*x*y+2*w*z);
	cout.setf(ios::fixed,ios::floatfield);
	cout.precision(3);
	cout << '\r' << "Q: " <<std::setw(4)<< raven.x() << ':'<<std::setw(4) << raven.y() << ':'<<std::setw(4)<< raven.z() << " A: " <<std::setw(4)<< roll << ':'<<std::setw(4) << pitch << ':'<<std::setw(4)<< yaw
		<< "  Master: "<<std::setw(4) <<ori.x()<<':'<<std::setw(4)<<ori.y()<<':'<<std::setw(4)<<ori.z()<< flush;

	//	dhdSetForceAndTorque(0,0,0,force.x(),force.y(),force.z());
	//}
}

