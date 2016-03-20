#include "windows.h"
#include "Eigen/Eigen"
#include "dhdc.h"
#include "drdc.h"
#include <Eigen/Geometry>
using namespace Eigen;
class Sigma_Hand
{
	//constructurs
	Sigma_Hand(int *devHandle,int devId);
	Sigma_Hand(int *devHandle,int devId,Vector3d DevicePos,Vector3d DeviceRot);
	~Sigma_Hand();
public:
		int devHandle;
		Vector3d *DevicePosGlobal;
		Matrix3d  *DeviceRotGlobal;
		int      DeviceID;
		Vector3d **FingerPosGlobal;
		void Sigma_Hand::getROT();
		void Sigma_Hand::getPos();
		void Sigma_Hand::getGrip();
		void Sigma_Hand::setPos();
		void Sigma_Hand::setRot();
		void Sigma_Hand::setGrip();
		void Sigma_Hand::close();
}