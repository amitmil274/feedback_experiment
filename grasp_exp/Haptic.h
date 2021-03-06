//#ifndef _HAPTIC
//#define _HAPTIC
#include <vector>
#include <Eigen/Geometry>
#include "Eigen/Eigen"

using namespace Eigen;
class HapticData
{
public:
	HapticData()
	{
		InitHandPos=0.05;
		ForceCount=-1;
		IncreaseForceMaxTime=2.0;
		gripType=0;
		enable_gripforce=false;
	};
	void moveto();
	void InitHapticDevice();
	void SetHapticBaseAngle(double x,double y, double z);
	void getGripForce();
	Vector3d position[2];
	Vector3d velocity[2];
	double gripper[2];
	double vgrip[2];
	double grip_diff_filt[2];
	double vgrip_filt[2];
	double gripForce;
	Vector3d orientation[2];
	float InitHandPos;
	int ForceCount;
	float IncreaseForceMaxTime;
	bool enable_gripper;
	bool enable_orientation;
	bool enable_position;
	bool enable_gripforce;

	int gripType;
	bool fullscreen;
};
void *HapticsLoop(void* pUserData);
void Ori_feedback(Vector3d ori);

//#endif