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
#include "Haptic.h"

using namespace std;
using namespace Eigen;

extern Sigma_Comm commOUT,commIN;
extern bool SimulationOn;
extern Timer timer;
double limit=1/1000;
double CurrentSendFreqTime=0.0;
int servo=0;
void* NetworkOUT(void* pUserData)
{
	int servolast=servo;
	while(SimulationOn)
	{
		commOUT.msgHeader.sequence=servo++;
		// COMMUNICATION
		if(commOUT.Check_Flag(BASIC_PROGRAM)) { // when UI allows 'Server' run
				if ((timer.time-CurrentSendFreqTime)>=limit)
			{
				CurrentSendFreqTime=timer.time;
			commOUT.Send_UDP();
				}
		}
		if(servo%100 == 1) {
			// Update SUI data
			commOUT.Send_TCP();
		}
		
	}
	return NULL;
}
void* NetworkIN(void* pUserData)
{
	if(commOUT.Check_Flag(BASIC_PROGRAM)) { // when UI allows 'Server' run
			commIN.Send_UDP();
		}
	while(SimulationOn)
	{
		// COMMUNICATION
		if(commIN.Check_Flag(BASIC_PROGRAM)) { // when UI allows 'Server' run
			commIN.Recv_UDP();
		}
	}
	return NULL;
}


