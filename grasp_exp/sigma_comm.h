/*****************************************************************************

omni_comm.h/omni_comm.h

These files declare and define objects for communicating with the 
robot slave and the Surgical User Interface.

Class created by KoSy
Modified by Hawkeye

*****************************************************************************/
//#include <HDU/hduQuaternion.h>
#include <Eigen\Eigen>
#include "..\VS_GUI_SERVER\CommonDS.h"
#include "ITPteleoperation.h"

using namespace Eigen;

class Sigma_Comm
{
public:
	//Omni_Comm(stMA2UI_DATA*, stUI2MA_DATA*,unsigned short, char *);
	Sigma_Comm(stMA2UI_DATA*, stUI2MA_DATA*);
	~Sigma_Comm();

	// ** TCP Commnucation to GUI  **
	int Initialize_TCP_GUI();
	int Update_MA2UI(Vector3d pos[2],int rl, unsigned int servo, int graspStatus, int status, int currentobject, int nextobject, int trialnumber,int fps_vision, int fps_haptic);
	int Check_UI2MA(int message);
	bool Check_Flag(int mode);
	void Set_Flag(int value,int mode);
	void Send_TCP();
	void Recv_TCP();
	int checksumUDPData();

	TCPSocket* tcpsocket;
	stMA2UI_DATA* pMa2UIdata;
	stUI2MA_DATA* pUI2Madata;

	// ** UDP Commnucation to Robot  **
	int Initialize_UDP_Robot(int argc, char*argv[]); 
	void Update_UDP_Data(Vector3d pos[2], Quaterniond qIncr[2],double grasp[2], int bttn[2], int fp, unsigned int servo, Matrix4d xfs[2]);
	//void Update_UDP_Raven(v_struct *rcvdata,v_struct *ravendata);
	void Send_UDP();
	bool Recv_UDP();
	//ComHeader msgHeader;
	u_struct msgHeader;
	v_struct rcvHeader;
	UDPSocket udpsocket;
	unsigned short port_Robot;
	char addr_Robot[16];
	int iUDPaddr;
};