#ifndef _PROTOCOL
#define _PROTOCOL

#include <vector>

class Protocol{
public:
	Protocol()
	{
		ModeInd=0;
		HitTime=0.0;
		ChangeTime=0.0;
		FinishActualGrasp=false;
		GraspTrials=0;
		//ReachTargCount=0;
		InitLocFlag=true;
		DispFeedbackFlag=false;
		//extern float DelayMode[];
		//if (DelayMode[5])
		//{
		//	for (int i=1; i<=4; i++) 
		//	{
		//		DelayPosX.push_back(0.0);
		//		DelayPosY.push_back(0.0);
		//		DelayVelX.push_back(0.0);
		//		DelayVelY.push_back(0.0);
		//		TimeVec.push_back(0.0);
		//		/*StartFlag=true;*/
		//	}
		//}
		//for (int i=1; i<=5; i++) 
		//{
		//	HitRefCal.push_back(0.0);
		//}
		UpdateVecTimeIntervel=0.001;
		UpdateDelayVecFlag=true;
		CurrentWriteFreqTime=0.0;

		TargSize=0.0075;

		//CurrentDelay=0;
	}

	void run();

	std::vector<float> DelayPosX;
	std::vector<float> DelayPosY;
	std::vector<float> DelayVelX;
	std::vector<float> DelayVelY;
	std::vector<float> TimeVec;
	std::vector<float> HitRefCal;
	float UpdateDelayVecTime;
	float UpdateVecTimeIntervel;
	bool UpdateDelayVecFlag;

	double CurrentWriteFreqTime;
	

	int ModeInd;
	float ep;
	float Jitter;
	float BallX;
	float BallY;
	float Angle;
	//float VelX;
	//float VelY;
	float HitTime;
	float ChangeTime;
	float TimeEp;
	float TimeBuf;

	int GraspTrials;
	int TargInd;
	float TargSize;
	float TargVelEp; // define the minimum velocity to stop in the target
	float TargTimeEp; // time to be in target
	float StartTargTime; // hold the time the hand reached the target (or zero velocity...)
	bool ZeroVelFlag;
	bool DispFeedbackFlag;
	float StartPosEp;
	float ServoTime;
	bool ServoTimeFlag;
	bool InitLocFlag; // a flag to hold the time the hand reached the init position
	float InitLocTime; // hold the time the hand reached the init position
	float InitPosTimeEp; // time for pausing in init position before the next target appears
	bool FinishActualGrasp;

};
#endif