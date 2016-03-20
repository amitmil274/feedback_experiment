#ifndef _FILEHANDLE
#define _FILEHANDLE
#include <stdio.h>
#include "timer.h"
#include <iostream>
#include <fstream>
#include <direct.h>
#include "nr.h"
#include <Eigen\Eigen>
#include "ITPteleoperation.h"
using namespace std;
using namespace Eigen;
extern v_struct ravenData;

class FileHandle
{
public:
	FileHandle():SaveVal_master(0.0,1,13),SaveVal_slave(0.0,1,16)
	{
		std::cout<<"Subject's Name: ";
		std::cin>>SubjectName;
		std::cout<<endl;
		TrialNum=1;
		TrialEndActual=5;
		TrialEnd=10;
		ReadTargFile();
		SamplingRate=1000.0;
		limit=1.0/SamplingRate;
	}
	void CreateDataFolder();
	void CreateExpInfoFile();
	void OpenDataFile();
	void SaveDataFile(double time);
	void CloseOpenDataFile();
	void ReadTargFile();
	FILE* Data_file_master;
	FILE* Data_file_slave;
	FILE* ExpInfo_file;
	ifstream TargFile;
	int TargInd[220];
	char filename_master[256];
	char filename_slave[256];
	char infofilename[256];
	char foldername[256];
	char fileanswer[256];
	char SubjectName[20];
	int TrialNum;
	int TrialEnd;
	int TrialEndActual;
	float SamplingRate;
	float limit;
	Mat_DP	SaveVal_master;
	Mat_DP	SaveVal_slave;
	Vector3d position;
	Vector3d velocity;
	double gripper;
	double vgrip;
	Vector3d orientation;
};

#endif