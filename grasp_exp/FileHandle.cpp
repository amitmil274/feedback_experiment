#include "FileHandle.h"
#include "Haptic.h"
#include "Protocol.h"


FileHandle filemaster;

extern int TargNum;

extern int status;

extern HapticData hapticData;
extern Protocol protocol;
extern int TargNum;

void FileHandle::CreateDataFolder()
{	
	SYSTEMTIME st;
	GetLocalTime(&st);
	
	sprintf(foldername, ".\\DataLogs\\%s_%02d_%02d_%02d", SubjectName, st.wYear, st.wMonth, st.wDay);
	_mkdir(foldername);
}

void FileHandle::CreateExpInfoFile()
{	
	SYSTEMTIME st;
	GetLocalTime(&st);
	
	sprintf(infofilename, "./DataLogs/%s_%02d_%02d_%02d/%s_%02d_%02d_%02d_ExpInfo.txt", SubjectName, st.wYear, st.wMonth, st.wDay, SubjectName, st.wYear, st.wMonth, st.wDay);
	ExpInfo_file = fopen(infofilename, "wt");

	if(ExpInfo_file)
	{
		fprintf(ExpInfo_file, "TargNum:\t%d\n",TargNum);

		int i;
		fprintf(ExpInfo_file, "]\n");
		
		fprintf(ExpInfo_file, "UpdateVecTimeIntervel:\t%f sec\n",protocol.UpdateVecTimeIntervel);


	}
}

void FileHandle::OpenDataFile()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	
	sprintf(filename_master, "./DataLogs/%s_%02d_%02d_%02d/%s_%02d_%02d_%02d_Trial%d.txt", SubjectName, st.wYear, st.wMonth, st.wDay, SubjectName, st.wYear, st.wMonth, st.wDay, TrialNum);
	sprintf(filename_slave, "./DataLogs/%s_%02d_%02d_%02d/%s_%02d_%02d_%02d_Trial%d_slave.txt", SubjectName, st.wYear, st.wMonth, st.wDay, SubjectName, st.wYear, st.wMonth, st.wDay, TrialNum);
	Data_file_master = fopen(filename_master, "wt");
	Data_file_slave = fopen(filename_slave, "wt");

	if(Data_file_master)
	{
		fprintf(Data_file_master, "Time Px Py Pz Vx Vy Vz GripRad VGripRad DiffFilt VFilt Ox Oy Oz\n");
	}
	if(Data_file_slave)
	{
		fprintf(Data_file_slave, "Time Px Py Pz Pxd Pyd Pzd Grip Gripd Qw Qx Qy Qz Qwd Qxd Qyd Qzd\n");
	}
}

void FileHandle::CloseOpenDataFile()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	fclose(Data_file_master);
	fclose(Data_file_slave);
	
}


void FileHandle::SaveDataFile(double time)
{
	SaveVal_master[0][0] = hapticData.position[0][0]; // The order here corresponds to the display Coordinate system- Z in the robot is Y in the display
	SaveVal_master[0][1] = hapticData.position[0][1];
	SaveVal_master[0][2] = hapticData.position[0][2];
	SaveVal_master[0][3] = hapticData.velocity[0][0];
	SaveVal_master[0][4] = hapticData.velocity[0][1];  
	SaveVal_master[0][5] = hapticData.velocity[0][2];
	SaveVal_master[0][6] = hapticData.gripper[0];
	SaveVal_master[0][7] = hapticData.vgrip[0];
	SaveVal_master[0][8] = hapticData.grip_diff_filt[0];
	SaveVal_master[0][9] = hapticData.vgrip_filt[0];
	SaveVal_master[0][10] = hapticData.orientation[0][0];
	SaveVal_master[0][11] = hapticData.orientation[0][1];  
	SaveVal_master[0][12] = hapticData.orientation[0][2];
	SaveVal_slave[0][0] = ravenData.px[0]; // The order here corresponds to the display Coordinate system- Z in the robot is Y in the display
	SaveVal_slave[0][1] = ravenData.py[0];
	SaveVal_slave[0][2] = ravenData.pz[0];
	SaveVal_slave[0][3] = ravenData.pxd[0]; // The order here corresponds to the display Coordinate system- Z in the robot is Y in the display
	SaveVal_slave[0][4] = ravenData.pyd[0];
	SaveVal_slave[0][5] = ravenData.pzd[0];
	SaveVal_slave[0][6] = ravenData.grasp[0];
	SaveVal_slave[0][7] = ravenData.graspd[0];
	SaveVal_slave[0][8] = ravenData.Qw[0];
	SaveVal_slave[0][9] = ravenData.Qx[0];
	SaveVal_slave[0][10] = ravenData.Qy[0];
	SaveVal_slave[0][11] = ravenData.Qz[0];
	SaveVal_slave[0][12] = ravenData.Qwd[0];
	SaveVal_slave[0][13] = ravenData.Qxd[0];
	SaveVal_slave[0][14] = ravenData.Qyd[0];
	SaveVal_slave[0][15] = ravenData.Qzd[0];
	SaveVal_master.appendToFile(Data_file_master,time);
	SaveVal_slave.appendToFile(Data_file_slave,time);
	
}

void FileHandle::ReadTargFile()
{
	TargFile.open("Targ5.txt");
	int i=0;
	while(!TargFile.eof()) 
	{
		TargFile>>TargInd[i];
		i++;
	}
}



