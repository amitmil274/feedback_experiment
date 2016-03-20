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

void* NetworkOUT(void* pUserData);
void* NetworkIN(void* pUserData);
