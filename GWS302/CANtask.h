/*
 * CANtask.h - STM32F2 GWiz
 *
 *  Created on: 14th December 2011
 *      Author: Chris Parsons
 */

#ifndef CANTASK_H_
#define CANTRASK_H_

// defines
/*-----------------------------------------------------------*/
#define mainCAN_TASK_PRIORITY (tskIDLE_PRIORITY + 2) // CAN/COM set priority <= watchdog task
#define canRx_STACK_SIZE configSPRINT_STACK_SIZE // uses sprintf

#define CANTX_BLOCK_TIME 0 /* ((portTickType)(10 / portTICK_RATE_MS)) was 10ms */

#define CAN_MAXRX 10
#define CAN_MAXTX 20

#define CANTIMEOUT 30 // 3 seconds at 100ms tick

// prime number used to offset counters so actions don't happen simultaneously
/* Prime Numbers less than 509...
	 2,    3,   5,   7,  11,  13,  17,  19,
	23,   29,  31,  37,  41,  43,  47,  53,
	59,   61,  67,  71,  73,  79,  83,  89,
	97,  101, 103, 107, 109, 113, 127, 131,
	137, 139, 149, 151, 157, 163, 167, 173,
	179, 181, 191, 193, 197, 199, 211, 223,
	227, 229, 233, 239, 241, 251, 257, 263,
	269, 271, 277, 281, 283, 293, 307, 311,
	313, 317, 331, 337, 347, 349, 353, 359,
	367, 373, 379, 383, 389, 397, 401, 409,
	419, 421, 431, 433, 439, 443, 449, 457,
	461, 463, 467, 479, 487, 491, 499, 503
*/
#define DIVOFFO000 5
#define DIVOFFO001 31
#define DIVOFFO002 67
#define DIVOFFO003 103
#define DIVOFFO004 149
#define DIVOFFO005 191
#define DIVOFFO006 233
#define DIVOFFO007 277
#define DIVOFFO008 331
#define DIVOFFO009 379
#define DIVOFFO010 431
#define DIVOFFO011 467

// all times must divide into maximum time
#define DIV00020MS	1   /*    20ms - 20ms ticks */
#define DIV00100MS	5   /*   100ms - 20ms ticks */
#define DIV00200MS	10  /*   200ms - 20ms ticks */
#define DIV00500MS	25  /*   500ms - 20ms ticks */
#define DIV01000MS	50  /*  1000ms - 20ms ticks */
#define DIV02000MS	100 /*  2000ms - 20ms ticks */
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 RDS CAN Module Identifier 11bit 83Kbs

 11
 1098 7654 3210 BIT No
 |||| |||| ||||
 0000 0000 0001 - TX bit - set by responding module module
 0000 0111 1000 - Module Number - upto 16 allowed, 4 supported
 nnnn nxxx xnnx - 7 bits for module type, 4 for module number, 1 for TX/RX
 0000 1000 0100 – 084 MCM Motor Control Module
 0010 0000 0100 – 204 ACM Actuator Control Module
 0001 0000 0100 – 104 DIP Digital Input Module
 0001 1000 0100 – 184 DOP Digital Output Module
 0010 1000 0100 – 284 HBM H-Bridge Module
 0110 0000 0100 – 604 WCM Weigh Cell Module - Sulky
 0100 0000 0100 – 404 LMM Loadmaster Module
 0011 0000 0100 – 304 IOM Input Output Module - McHale
 0011 1000 0100 – 384 IOC Input Output Module - TypeC - Evrard
 0011 1000 0010 – 382 IOB Input Output Module - TypeB - Sulky

 The ID has 4 bits reserved for multiple module connection which is the same for all modules…
  ID_MASK  0x078
 The ID has 1 bit reserved to identify master/slave
  MS_MASK  0x001

 6 bits are used for module ID, so a maximum of 64 module types can be defined using this scheme
 No             Mask -    ID
  1 0x0000 0ppp p00m - 0x000
  2 0x0000 0ppp p01m - 0x002
  3 0x0000 0ppp p10m - 0x004
  4 0x0000 0ppp p11m - 0x006
  5 0x0000 1ppp p00m - 0x080
  6 0x0000 1ppp p01m - 0x082
  7 0x0000 1ppp p10m - 0x084 - MCM
  8 0x0000 1ppp p11m - 0x086
  9 0x0001 0ppp p00m - 0x100
 10 0x0001 0ppp p01m - 0x102
 11 0x0001 0ppp p10m - 0x104 - DIP
 12 0x0001 0ppp p11m - 0x106
 13 0x0001 1ppp p00m - 0x180
 14 0x0001 1ppp p01m - 0x182
 15 0x0001 1ppp p10m - 0x184 - DOP
 16 0x0001 1ppp p11m - 0x186
 17 0x0010 0ppp p00m - 0x200
 18 0x0010 0ppp p01m - 0x202
 19 0x0010 0ppp p10m - 0x204 - ACM
 20 0x0010 0ppp p11m - 0x206
 21 0x0010 1ppp p00m - 0x280
 22 0x0010 1ppp p01m - 0x282
 23 0x0010 1ppp p10m - 0x284 - HBM
 24 0x0010 1ppp p11m - 0x286
 25 0x0011 0ppp p00m - 0x300
 26 0x0011 0ppp p01m - 0x302
 27 0x0011 0ppp p10m - 0x304 - IOM
 28 0x0011 0ppp p11m - 0x306
 29 0x0011 1ppp p00m - 0x380
 30 0x0011 1ppp p01m - 0x382 - IOB
 31 0x0011 1ppp p10m - 0x384 - IOC
 32 0x0011 1ppp p11m - 0x386
 33 0x0100 0ppp p00m - 0x400
 34 0x0100 0ppp p01m - 0x402
 35 0x0100 0ppp p10m - 0x404
 36 0x0100 0ppp p11m - 0x406
 37 0x0100 1ppp p00m - 0x480
 38 0x0100 1ppp p01m - 0x482
 39 0x0100 1ppp p10m - 0x484
 40 0x0100 1ppp p11m - 0x486
 41 0x0101 0ppp p00m - 0x500
 42 0x0101 0ppp p01m - 0x502
 43 0x0101 0ppp p10m - 0x504
 44 0x0101 0ppp p11m - 0x506
 45 0x0101 1ppp p00m - 0x580
 46 0x0101 1ppp p01m - 0x582
 47 0x0101 1ppp p10m - 0x584
 48 0x0101 1ppp p11m - 0x586
 49 0x0110 0ppp p00m - 0x600
 50 0x0110 0ppp p01m - 0x602
 51 0x0110 0ppp p10m - 0x604 - WCM
 52 0x0110 0ppp p11m - 0x606
 53 0x0110 1ppp p00m - 0x680
 54 0x0110 1ppp p01m - 0x682
 55 0x0110 1ppp p10m - 0x684
 56 0x0110 1ppp p11m - 0x686
 57 0x0111 0ppp p00m - 0x700
 58 0x0111 0ppp p01m - 0x702
 59 0x0111 0ppp p10m - 0x704
 60 0x0111 0ppp p11m - 0x706
 61 0x0111 1ppp p00m - 0x780
 62 0x0111 1ppp p01m - 0x782
 63 0x0111 1ppp p10m - 0x784
 64 0x0111 1ppp p11m - 0x786

 p = module group id
 m = master slave bit
-----------------------------------------------------------*/
#define CAN_TIME 0xf5
#define CAN_TEST 0xfe

#define IOM_VOLTAGESCALE 800.0 // 12bits for a 10bit value (2^12 / max voltage (4.99V)) = 821
#define IOM_CURRENTSCALE 1200.0 // 12bits for a 10bit value (2^12 / max current (3.3A)) = 1241

#define IOC_VOLTAGESCALE 800.0 // 12bits for a 10bit value (2^12 / max voltage (4.99V)) = 821

#define IOB_VOLTAGESCALE 800.0 // 12bits for a 10bit value (2^12 / max voltage (4.99V)) = 821
#define IOB_CURRENTSCALE 480.0 // 12bits for a 10bit value (2^12 / max current (8.4A)) = 488

#define IOB_ATOD_IP_SCALE (ATODSCALE * ((220.0 + 330.0) / 330.0))
#define IOB_ATOD_VTOI_SCALE (IOB_ATOD_IP_SCALE * (100.0 / (2700.0 * 0.022)))

#define ACN_RUN_CURRENT		0.100
#define ACN_STALL_CURRENT	4.000

#define MAX_FREQ_IP  11
#define MAX_OUTPUTS  24
/*-----------------------------------------------------------*/

// variables
/*-----------------------------------------------------------*/
// variables
extern xQueueHandle RxCAN0, TxCAN0;
extern xQueueHandle RxCAN1, TxCAN1;

#define CAN1_HW 0
#define CAN2_HW 4

#define MCM_ID 0
#define ACM_ID 1
#define HBM_ID 2
#define WCM_ID 3
#define DOP_ID 4
#define DIP_ID 5
#define LMM_ID 6
#define IOM_ID 7
#define IOC_ID 8
#define IOB_ID 9

#define CS_CLEAR		0x00
#define CS_QUEUED		0x01
#define CS_QUEUEFULL	0x02
#define CS_RECEIVED 	0x04
#define CS_WASRECEIVED	0x08
#define CS_TIMEDOUT 	0x10

struct CAN_Status {
	unsigned char tx_timeout;
	unsigned char tx_status;
	unsigned char rx_status;
};
extern struct CAN_Status CanStat[CAN_MODULE_TYPES][8];

#define MCM_fPI 960
#define MCM_fPD 400
#define MCM_fPWM 15974

#define MCM_sRUN	0x01
#define MCM_sMODE	0x02

#define MCM_RESET 1 // reset module
#define MCM_SET_MOTOR_SPEED 2 // set motor speed and mode
#define MCM_GET_FORWARD_SPEED 3 // get speed sensor information
#define MCM_AUTOCAL_RESET 4 // reset autocal
#define MCM_AUTOCAL_PROGRESS 5 // auto cal progress
#define MCM_AUTOCAL_STOP 6 // transmitted to module - stop auto cal
#define MCM_GET_AUX_INPUT 7 // get auxillary inputs
#define MCM_GET_AUX_TIME 8 // get auxillary inputs time
#define MCM_SET_PID_DATA 9 // send PID configuration
#define MCM_GET_AUX_FREQ 10 // get aux frequency
#define MCM_GET_SOFTWARE 0xff // get software info

#define METER_AUTO 0
#define METER_MANUAL 1
#define METER_CALIBRATION 2

#define CAN_MODULE_MESSAGE_RECEIVED		0x01
#define CAN_MODULE_PRESENT				0x02

struct CAN_MCM {
  unsigned short int software_type;
  unsigned short int software_issue;

  unsigned long int motor_pulse_accumulator;
  unsigned long int speed_pulse_accumulator;
  unsigned long int calibration_motor_pulse_accumulator;

  unsigned short int motor_frequency;
  unsigned short int speed_frequency;

  unsigned char dip_time_since_last_pulse[6];
  unsigned char dip_status;
  unsigned char dil_setting;
  unsigned char motor_mode;
  unsigned char motor_current;
  unsigned char controller_temperature;
  unsigned char controller_voltage;
  unsigned char controller_pwm;
  unsigned char reset_fault;

  signed char motor_frequency_error;
  signed char speed_frequency_error;
  };
extern struct CAN_MCM MCM[8];

#define ACM_RESET 1
#define ACM_GET_SOFTWARE 0xff // get software info
struct CAN_ACM {
  unsigned short int software_type;
  unsigned short int software_issue;
};
extern struct CAN_ACM ACM[8];

#define HBM_RESET 1
#define HBM_SETPARAMETERS 2
#define HBM_SETOUTPUTS 3
#define HBM_GETINPUTS 4
#define HBM_GETANALOG 5
#define HBM_GET_SOFTWARE 0xff // get software info
struct HBM_Output_struct
{
	unsigned char Stalled;
	unsigned char Stall_Timer;
	float Stall_Current;
	float Limit_Current;	// not used in original HBM software
	unsigned short int Duty[2];
	float Current;
};

struct HBM_Input_struct
{
	unsigned long int Pulse_Accumulator;
	unsigned short int Frequency;
	unsigned char Level_Now;

};

struct CAN_HBM {
  unsigned short int software_type;
  unsigned short int software_issue;
  unsigned char status;
  unsigned char UnscaledVoltage[2];

  struct HBM_Input_struct HBM_Input[4];
  struct HBM_Output_struct HBM_Output[2];
};
extern struct CAN_HBM HBM[8];

#define WCM_GET_ALL_VOLTAGES 0x81 // get all voltages
#define WCM_GET_SOFTWARE 0x82 // get software info
#define WCM_GET_ANGLE_VOLTAGES 0x83 // get angle voltages
struct CAN_WCM {
  unsigned short int software_type;
  unsigned short int software_issue;

  signed long int WeightVoltage;
  unsigned char WeighFilter;
  unsigned char RollVoltage;
  unsigned char PitchVoltage;
};
extern struct CAN_WCM WCM[8];

#define DOP_RESET 1 // get software info
#define DOP_SET_OUTPUT 2 // get software info
#define DOP_GET_SOFTWARE 0xff // get software info
struct CAN_DOP {
  unsigned short int software_type;
  unsigned short int software_issue;

  unsigned char Op[8];
};
extern struct CAN_DOP DOP[8];

#define DIP_RESET 1
#define DIP_SETPULLUP 2
#define DIP_GETFREQ 3
#define DIP_GETSTATUS 4
#define DIP_GET_SOFTWARE 0xff // get software info
struct CAN_DIP {
  unsigned short int software_type;
  unsigned short int software_issue;

  unsigned char pullup;
  unsigned char ipstatus;
  unsigned short int frequency[8];
};
extern struct CAN_DIP DIP[8];

#define LMM_RESET 1
#define LMM_GET_SOFTWARE 0xff // get software info
struct CAN_LMM {
  unsigned short int software_type;
  unsigned short int software_issue;
};
extern struct CAN_LMM LMM[8];

#define IOM_IP01 (1 << 0)
#define IOM_IP02 (1 << 1)
#define IOM_IP03 (1 << 2)
#define IOM_IP04 (1 << 3)
#define IOM_IP05 (1 << 4)
#define IOM_IP06 (1 << 5)
#define IOM_IP07 (1 << 6)
#define IOM_IP08 (1 << 7)
#define IOM_OP_STATUS (1 << 8)

#define IOM_OP01 (1 << 0)
#define IOM_OP02 (1 << 1)
#define IOM_OP03 (1 << 2)
#define IOM_OP04 (1 << 3)
#define IOM_OP05 (1 << 4)
#define IOM_OP06 (1 << 5)
#define IOM_OP07 (1 << 6)
#define IOM_OP08 (1 << 7)
#define IOM_OP09 (1 << 8)
#define IOM_OP10 (1 << 9)

#define IOM_GET_SOFTWARE 0xff // get software info
struct CAN_IOM {
  unsigned short int software_type;
  unsigned short int software_issue;

  unsigned long int Outputs;
  unsigned long int Inputs;
  unsigned long int pulsecount[8];
  unsigned short int Voltage[4];
  unsigned short int Current[4];
  unsigned short int frequency[8];
  unsigned char IPstatus;
  unsigned char OutputCount;
  unsigned char Supply;
  unsigned char VoltageThreshold;
  unsigned char PulseCountZero;
};
extern struct CAN_IOM IOM[8];

#define IOC_IP01 (1 << 0)
#define IOC_IP02 (1 << 1)
#define IOC_IP03 (1 << 2)
#define IOC_IP04 (1 << 3)
#define IOC_IP05 (1 << 4)
#define IOC_IP06 (1 << 5)
#define IOC_IP07 (1 << 6)
#define IOC_IP08 (1 << 7)
#define IOC_OP_STATUS (1 << 8)

#define IOC_OP01 (1 << 0)
#define IOC_OP02 (1 << 1)
#define IOC_OP03 (1 << 2)
#define IOC_OP04 (1 << 3)
#define IOC_OP05 (1 << 4)
#define IOC_OP06 (1 << 5)
#define IOC_OP07 (1 << 6)
#define IOC_OP08 (1 << 7)
#define IOC_OP09 (1 << 8)
#define IOC_OP10 (1 << 9)
#define IOC_OP11 (1 << 10)
#define IOC_OP12 (1 << 11)
#define IOC_OP13 (1 << 12)
#define IOC_OP14 (1 << 13)
#define IOC_OP15 (1 << 14)
#define IOC_OP16 (1 << 15)
#define IOC_PWM1 (1 << 16)
#define IOC_PWM2 (1 << 17)
#define IOC_PWM3 (1 << 18)
#define IOC_PWM4 (1 << 19)
#define IOC_PWM5 (1 << 20)
#define IOC_PWM6 (1 << 21)
#define IOC_PWM7 (1 << 22)
#define IOC_PWM8 (1 << 23)

#define IOC_GET_SOFTWARE 0xff // get software info
struct CAN_IOC {
  unsigned short int software_type;
  unsigned short int software_issue;

  unsigned long int Outputs;
  unsigned long int Inputs;
  unsigned long int pulsecount[8];
  unsigned short int Voltage[4];
  unsigned short int frequency[8];
  unsigned char IPstatus;
  unsigned char OutputCount;
  unsigned char Supply;
  unsigned char VoltageThreshold;
  unsigned char PulseCountZero;
  unsigned char LastInputTime;
  unsigned char DipSwitchSetting;
};
extern struct CAN_IOC IOC[8];

/* Actuator directions */
#define MOVING 	0xC0
#define STOPPED	0x00
#define EXTEND	0x40
#define RETRACT	0x80

#define IOB_IP01 (1 << 0)
#define IOB_IP02 (1 << 1)
#define IOB_IP03 (1 << 2)
#define IOB_IP04 (1 << 3)
#define IOB_IP05 (1 << 4)
#define IOB_IP06 (1 << 5)
#define IOB_IP07 (1 << 6)
#define IOB_IP08 (1 << 7)
#define IOB_IP09 (1 << 8)
#define IOB_IP10 (1 << 9)
#define IOB_IP11 (1 << 10)
#define IOB_IP12 (1 << 11)
#define IOB_OP_STATUS (1 << 12)

#define IOB_OP01 (1 << 0)
#define IOB_OP02 (1 << 1)
#define IOB_OP03 (1 << 2)
#define IOB_OP04 (1 << 3)
#define IOB_OP05 (1 << 4)
#define IOB_OP06 (1 << 5)
#define IOB_OP07 (1 << 6)
#define IOB_OP08 (1 << 7)
#define IOB_PWM1 (1 << 8)
#define IOB_PWM2 (1 << 9)
#define IOB_PWM3 (1 << 10)
#define IOB_PWM4 (1 << 11)
#define IOB_PWM5 (1 << 12)
#define IOB_PWM6 (1 << 13)
#define IOB_PWM7 (1 << 14)
#define IOB_PWM8 (1 << 15)

#define IOB_GET_SOFTWARE 0xff // get software info

union sutoc {
  signed short int s;
  unsigned short int u;
  char c[2];
  };

union fsltoc {
  float f;
  signed long s;
  unsigned long l;
  char c[4];
  };

struct CAN_IOB {
  unsigned short int software_type;
  unsigned short int software_issue;

  unsigned char StallPulse;
  unsigned char StallNone;
  unsigned long int Outputs;
  unsigned long int Inputs;
  unsigned long int pulsecount[8];
  unsigned short int Voltage[2];
  unsigned short int Current[2];
  unsigned short int frequency[8];
  union fsltoc WeighVoltage, CellWeight;
  struct Angles AccPos;
  union sutoc CurPosX, CurPosY, CurPosZ;
  union sutoc ZerPosX, ZerPosY, ZerPosZ;
  union fsltoc COSangle, RESangle;
  unsigned char IPstatus;
  unsigned char OutputCount;
  unsigned char Supply;
  unsigned char VoltageThreshold;
  unsigned char PulseCountZero;
  unsigned char LastInputTime;
};
extern struct CAN_IOB IOB[8];

struct TimeDef {
  unsigned long int time;
  unsigned char stage;
  unsigned char day;
  };

extern void (* const can_test_screens[CAN_MODULE_TYPES])( unsigned int );

struct ModuleInfo {
	unsigned int module_type_id;
	unsigned char SoftwareRequest;
};
extern struct ModuleInfo const module_info_table[CAN_MODULE_TYPES * 4];
/*-----------------------------------------------------------*/

// function prototypes
/*-----------------------------------------------------------*/
extern void InitialiseCAN0( void );
extern void InitialiseCAN1( void );
extern void Boot_SetRestartStatus( void );
extern void ProcessGWSmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx );
extern void SendTestMessage( unsigned int ModuleType, unsigned int ModuleNumber );
extern void ProcessMCMmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx );
extern void ProcessACMmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx );
extern void ProcessHBMmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx );
extern void ProcessWCMmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx );
extern void ProcessDOPmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx );
extern void ProcessDIPmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx );
extern void ProcessLMMmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx );
extern void ProcessIOMmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx );
extern void ProcessIOCmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx );
extern void ProcessIOBmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx );
extern void CAN0check( void );
//static void vCAN0RxTask( void *pvParameters )
extern void CAN1check( void );
//static void vCAN1RxTask( void *pvParameters )
extern void CAN0Tx( struct CanMessage *ptCanM );
extern void CAN1Tx( struct CanMessage *ptCanM );

extern void CAN1_TX_IRQHandler( void );
extern void CAN1_RX0_IRQHandler( void );
extern void CAN1_RX1_IRQHandler( void );
extern void CAN1_SCE_IRQHandler( void );
extern void CAN2_TX_IRQHandler( void );
extern void CAN2_RX0_IRQHandler( void );
extern void CAN2_RX1_IRQHandler( void );
extern void CAN2_SCE_IRQHandler( void );
extern void vStartCANTasks( unsigned portBASE_TYPE uxPriority );

extern void HBMRoutines(struct CAN_HBM *ptMod, unsigned char message, unsigned char index);


/*-----------------------------------------------------------*/

#endif /* CANTASK_H_ */
