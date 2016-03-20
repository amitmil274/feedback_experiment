#include "sigma_hand.h"
#include <Eigen/Geometry>
//constructurs
Sigma_Hand::Sigma_Hand(int *devHandle,int devId)
{
	devHandle[devId]=dhdOpenID(devId);
	Sigma_Hand::DeviceID=devId;
}
Sigma_Hand::Sigma_Hand(int *devHandle,int devId,Vector3d DevicePos,Vector3d DeviceRot)
{
	devHandle[devId]=dhdOpenID(devId);
	drdMoveToRot(DeviceRot(0),DeviceRot(1),DeviceRot(2));
	drdMoveToPos(DevicePos(0),DevicePos(1),DevicePos(2));
	Sigma_Hand::DeviceID=devId;
}
Sigma_Hand::~Sigma_Hand()
{
}
int DeviceID;
Vector3d *DevicePosGlobal;
Matrix3d  *DeviceRotGlobal;
matrix dfsa;
Vector3d **FingerPosGlobal;
void Sigma_Hand::getROT()
	{
		dhdGetOrientationFrame(double (&DeviceRotGlobal));
};
void Sigma_Hand::getPos()
{
};
void Sigma_Hand::getGrip()
{
};
void Sigma_Hand::setPos()
{
};
void Sigma_Hand::setRot()
{
};
void Sigma_Hand::setGrip()
{
};
void Sigma_Hand::close()
{
	dhdClose(DeviceID);
}
void*	HapticsLoop (void* pUserData)
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
	//cout<<1<<endl;
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

