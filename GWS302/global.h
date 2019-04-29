/*
 * global.h
 *
 *  Created on: 16 Nov 2011
 *      Author: Chris Parsons
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

// defines
/*-----------------------------------------------------------*/

// includes
/*-----------------------------------------------------------*/
// Standard Library includes
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#ifndef WINDOWS
// STM32F2 includes
#include "stm32f2xx.h"

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// Test Includes
#include "BlockQ.h"
#include "GenQTest.h"
#include "integer.h"
#include "PollQ.h"
#include "QPeek.h"
#include "recmutex.h"
#include "semtest.h"
#endif

// other projects
#include "..\GWS302\common.h"

/*-----------------------------------------------------------*/

/* main.c */
/*-----------------------------------------------------------*/
/* defines */
/* The time between cycles of the 'check' functionality (defined within the tick hook. */
#define mainCHECK_DELAY					((portTickType)2000 / portTICK_RATE_MS)

/* Task priorities. */
// Default Task Priorities (configMAX_PRIORITIES = 5)
#define mainQUEUE_POLL_PRIORITY			(tskIDLE_PRIORITY + 2)
#define mainSEM_TEST_PRIORITY			(tskIDLE_PRIORITY + 1)
#define mainBLOCK_Q_PRIORITY			(tskIDLE_PRIORITY + 2)
#define mainUIP_TASK_PRIORITY			(tskIDLE_PRIORITY + 3)
#define mainFLASH_TASK_PRIORITY			(tskIDLE_PRIORITY + 2)
#define mainCHECK_TASK_PRIORITY			(tskIDLE_PRIORITY + 3)
#define mainINTEGER_TASK_PRIORITY		(tskIDLE_PRIORITY)
#define mainGEN_QUEUE_TASK_PRIORITY		(tskIDLE_PRIORITY)

#define configSPRINT_STACK_SIZE		(configMINIMAL_STACK_SIZE * 3)

/* The length of the queue used to send messages to the LCD task. */
#define mainQUEUE_SIZE					(3)

/* The period of the system clock in nano seconds.  This is used to calculate the jitter time in nano seconds. */
#define mainNS_PER_CLOCK	((unsigned long)((1.0 / (double)TIMER_CLOCK_FREQUENCY) * 1000000000.0))

// variables

// function prototypes
extern void debug_message( unsigned char *pt );

/*-----------------------------------------------------------*/

#ifndef WINDOWS
/* timertest.c */
#include "timertest.h"
/*-----------------------------------------------------------*/
/* variables */
//extern volatile unsigned short usMaxJitter;

/* function prototypes */
//extern void vSetupTimerTest( void );
/*-----------------------------------------------------------*/
#endif

// bm_sym.c
/*-----------------------------------------------------------*/
#include "bm_sym.h"
/*-----------------------------------------------------------*/

// timers.c
/*-----------------------------------------------------------*/
#include "timers.h"
/*-----------------------------------------------------------*/

// functions.c
/*-----------------------------------------------------------*/
#include "functions.h"
/*-----------------------------------------------------------*/

#ifndef WINDOWS
// Power.c
/*-----------------------------------------------------------*/
#include "Power.h"
/*-----------------------------------------------------------*/
#else
#define M_TWOPI         (M_PI * 2.0)

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

#define CAN1_HW 0
#define CAN2_HW 4

#define CAN_MAXRX 10
#define CAN_MAXTX 20

#define CANTIMEOUT 30 // 3 seconds at 100ms tick

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
	float Duty[2];
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

struct CAN_Status {
	unsigned char tx_timeout;
	unsigned char tx_status;
	unsigned char rx_status;
};
extern struct CAN_Status CanStat[CAN_MODULE_TYPES][8];


extern unsigned char RunFactorStatus;
extern unsigned char PowerSwitchedOff;

extern void generate_crc_table( void );
extern unsigned int calculate_crc( unsigned int crc_accum, char *pt, unsigned int size );

#endif

// display.c
/*-----------------------------------------------------------*/
#include "display.h"
/*-----------------------------------------------------------*/

#ifndef WINDOWS
/* CAN1task.c */
/*-----------------------------------------------------------*/
#include "CANtask.h"
/*-----------------------------------------------------------*/

/* TWItask.c */
/*-----------------------------------------------------------*/
#include "TWItask.h"
/*-----------------------------------------------------------*/
#endif

/* screens.c */
/*-----------------------------------------------------------*/
#include "screens.h"
/*-----------------------------------------------------------*/

#ifndef WINDOWS
/* operate.c */
/*-----------------------------------------------------------*/
#include "operate.h"
/*-----------------------------------------------------------*/

/* COMtask.c */
/*-----------------------------------------------------------*/
#include "COMtask.h"
/*-----------------------------------------------------------*/

/* interrupts.c */
/*-----------------------------------------------------------*/
#include "interrupts.h"
/*-----------------------------------------------------------*/
#endif

/* Language file */

/*
" ÀÁÂÃÄÅÆABCÇDEÈÉÊËFGHIÌÍÎÏÐJKLMNÑOÒÓÔÕÖØPQRSTUÙÚÛÜVWXYÝßZÞ";
" aàáâãäåæbcçdeèéêëfghiìíîïðjklmnñoòóôõöøpqrstuùúûüvwxyýÿzþ";
"0123456789.+-±!#¢£$";

"!\"#$%&'()*+,-./:;<=>?@[\\]^_`"
"{|}~ ¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿"
"0123456789"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß"
"àáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ"
*/

#endif /* GLOBAL_H_ */

