/*****************************************************************************
* Common Data Structure 
*******************************************************************************/
#ifndef __COMMON_DS_H__
#define __COMMON_DS_H__

//#define Million 1000
//double scale;

//#define ADDR_BUBBLES	"bubbles.ee.washington.edu"
//#define ADDR_SAME_PC	"132.72.216.156"
#define ADDR_SAME_PC	"127.0.0.1"

//#define BUTTON_ON	1
//#define BUTTON_OFF	0

//#define ALL_ONES	0xffffffff
#define ALL_ONES	0x0fffffff  //in order to use 'int' not 'unsigned int'

//#define BASIC_FLAG	0x0000000f
#define BASIC_PROGRAM 	0x00000001
#define BASIC_START 	0x00000002
#define BASIC_LOGIN		0x00000004
//#define BASIC_PEDAL_SW	0x00000004

//#define DATA_COL_FLAG   0x000000f0
#define DC_NEWFILE      0x00000010
#define DC_STARTSTOP    0x00000020
#define DC_MARKINDEX    0x00000040
#define DC_DICTATE      0x00000080

#define VIEW_FLAG       0x00000f00
#define VIEW_ENDOSCOPE  0x00000100
#define VIEW_ORVIEW     0x00000200
#define VIEW_ORVIEW2    0x00000300

#define TOOL_RIGHT	0x0000f000
#define TOOL_LEFT	0x000f0000
#define MOVEBIT_TOOL_RIGHT	12
#define MOVEBIT_TOOL_LEFT	16

//#define FOOT_PEDAL	0x00f00000
//#define MOVEBIT_FOOTPEDAL	20
#define FPEDAL_RIGHT	0x00100000
#define FPEDAL_MIDDLE	0x00200000
#define FPEDAL_LEFT	0x00400000

// SCALING
#define SCALE_POS_MAX 	100	//0 ~ 100 scale should be changed 0 ~ 1
#define SCALE_POS_INIT_VALUE 40	//5 means 0.05
#define SCALE_GRIP_MAX 	10	//0 ~ 10 scale 
#define SCALE_GRIP_INIT_VALUE 5	
// GRIP FORCE
#define GRIP_FORCE_KP_MAX 	200 	
#define GRIP_FORCE_KD_MAX 	100
#define GRIP_FORCE_KP_INIT_VALUE 10	
#define GRIP_FORCE_KD_INIT_VALUE 1	

typedef struct{
	unsigned int tick;
	int delx[2];
	int dely[2];
	int delz[2];
	int runlevel;
	int checksum;
	int currentobject;
	int nextobject;
	int trialnumber;
	int expstatus;
	int graspstatus;
	int fps_vision;
	int fps_haptic;
}stMA2UI_DATA;

typedef struct{
	unsigned int tick;
	int flag01;
	int UDPaddr;
	// SCALING
	int scale_pos;
	int scale_grip;
	int checksum;
	double camAngle[3];  // Specifies three camera angles
	int camAngle2[3];
	int useITP;          // Specifies coordinate frame mode
	bool zero;
	// FEATURES ENABLED
	bool enable_grip;
	bool enable_orientation;
	bool enable_position;
	bool enable_gripforce;
	double grip_force_Kd, grip_force_Kp;
	int gripType;
	bool fullscreen;
	double camOffLX, camOffLY, camOffRX, camOffRY;
	bool setCamOff;
	double zeroX,zeroY,zeroZ;
}stUI2MA_DATA;

#endif
