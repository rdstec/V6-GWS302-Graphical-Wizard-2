/*
 * functions.h
 *
 *  Created on: 25 May 2016
 *      Author: cparsons
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

// defines
/*-----------------------------------------------------------*/
//#define M_TWOPI 6.28318530718 - math.h

#define GAUGE_MIN (-M_TWOPI * 0.045) // negative!!
#define GAUGE_MAX (M_TWOPI * 0.340)
#define GAUGE_STEP ((GAUGE_MAX - GAUGE_MIN) / 100.0)

#define CL0CK_ICON_OFF 0
#define CL0CK_ICON_ON 1

#define METRIC 0
#define IMPERIAL 1

#define POWEROFF 1
#define POWERON  0

#define GRAPHICS_SIZE (160 * 16)
#define COLUMNS 20 // 20 columns of 8 bytes for 160 pixel wide display

#define RFS_CLEAR	0
#define RFS_CS1FAIL	1
#define RFS_CS2FAIL 2
#define RFS_SW1FAIL 3
#define RFS_IS1FAIL 4
#define RFS_RV1FAIL 5
#define RFS_SW2FAIL 6
#define RFS_IS2FAIL 7
#define RFS_RV2FAIL 8
#define RFS_CS3FAIL 9
#define RFS_CS4FAIL 10

#define ATODVREF 3.3
#define ATODSCALE (ATODVREF / 4095.0) // 12 bits
#define ATOD_IP_SCALE (ATODSCALE * ((2.2 + 3.9) / 3.9))
#define ATOD_VIN_SCALE (ATODSCALE * ((220.0 + 22.0) / 22.0))
#define ATODVMIN (8.4 / ATOD_VIN_SCALE)

#define ATOD_IP_CURRENT_GAIN (1000000.0 / 220000.0)
#define ATOD_IP_CURRENT_RES 24.0 /* 24.0 Ohm */
#define ATOD_IP_CURRENT_SCALE_MA (ATOD_IP_SCALE * 1000.0 / ATOD_IP_CURRENT_GAIN / ATOD_IP_CURRENT_RES)

#define ATODVREF_HR 5.0
#define ATODSCALE_HR (ATODVREF_HR / 8388607.0) // 23 bits
#define ATODSCALE_CURRENT_HR (ATODSCALE_HR * 1000.0 / ATOD_IP_CURRENT_GAIN / ATOD_IP_CURRENT_RES)

#define ADC_WAKEUP1  0x00
#define ADC_RDATA    0x01
#define ADC_RDATAC   0x03
#define ADC_SDATAC   0x0f
#define ADC_RREG     0x10
#define ADC_WREG     0x50
#define ADC_SELFCAL  0xf0
#define ADC_SELFOCAL 0xf1
#define ADC_SELFGCAL 0xf2
#define ADC_SYSOCAL  0xf3
#define ADC_SYSGCAL  0xf4
#define ADC_SYNC     0xfc
#define ADC_STANDBY  0xfd
#define ADC_RESET    0xfe
#define ADC_WAKEUP2  0xff

#define ADR_STATUS 0x00
#define ADR_MUX    0x01
#define ADR_ADCON  0x02
#define ADR_DRATE  0x03
#define ADR_IO     0x04
#define ADR_OFC0   0x05
#define ADR_OFC1   0x06
#define ADR_OFC2   0x07
#define ADR_FSC0   0x08
#define ADR_FSC1   0x09
#define ADR_FSC2   0x0a

#define ADZ_SEARCH 0x00
#define ADZ_FAULT  0x01
#define ADZ_FOUND  0x02

// ADXL Register Map
#define ACC_DEVID			0x00 // R 	Device ID
#define ACC_THRESH_TAP		0x1D // R/W Tap threshold
#define ACC_OFSX			0x1E // R/W X-axis offset
#define ACC_OFSY			0x1F // R/W Y-axis offset
#define ACC_OFSZ			0x20 // R/W	Z-axis offset
#define ACC_DUR				0x21 // R/W	Tap duration
#define ACC_Latent			0x22 // R/W	Tap latency
#define ACC_Window			0x23 // R/W	Tap window
#define ACC_THRESH_ACT		0x24 //	R/W	Activity threshold
#define ACC_THRESH_INACT	0x25 // R/W	Inactivity threshold
#define ACC_TIME_INACT		0x26 // R/W	Inactivity time
#define ACC_ACT_INACT_CTL	0x27 // R/W	Axis enable control for activity and inactivity detection
#define ACC_THRESH_FF		0x28 // R/W	Free-fall threshold
#define ACC_TIME_FF			0x29 // R/W	Free-fall time
#define ACC_TAP_AXES		0x2A // R/W	Axis control for single tap/double tap
#define ACC_ACT_TAP_STATUS	0x2B // R	Source of single tap/double tap
#define ACC_BW_RATE			0x2C // R/W	Data rate and power mode control
#define ACC_POWER_CTL		0x2D // R/W Power-saving features control
#define ACC_INT_ENABLE		0x2E // R/W	Interrupt enable control
#define ACC_INT_MAP			0x2F // R/W	Interrupt mapping control
#define ACC_INT_SOURCE		0x30 // R	Source of interrupts
#define ACC_DATA_FORMAT		0x31 // R/W	Data format control
#define ACC_DATAX0			0x32 // R	X-Axis Data 0
#define ACC_DATAX1			0x33 // R	X-Axis Data 1
#define ACC_DATAY0			0x34 // R	Y-Axis Data 0
#define ACC_DATAY1			0x35 // R	Y-Axis Data 1
#define ACC_DATAZ0			0x36 // R	Z-Axis Data 0
#define ACC_DATAZ1			0x37 // R	Z-Axis Data 1
#define ACC_FIFO_CTL		0x38 // R/W	FIFO control
#define ACC_FIFO_STATUS		0x39 // R	FIFO status

// Address Bits
#define ACC_READ			(1<<7)
#define ACC_WRITE			(0<<7)
#define ACC_MULTIBYTE 		(1<<6)

// Power Control Register Bits
#define ACC_WU_0			(1<<0)	//Wake Up Mode - Bit 0
#define	ACC_WU_1			(1<<1)	//Wake Up mode - Bit 1
#define ACC_SLEEP			(1<<2)	//Sleep Mode
#define	ACC_MEASURE			(1<<3)	//Measurement Mode
#define ACC_AUTO_SLP		(1<<4)	//Auto Sleep Mode bit
#define ACC_LINK			(1<<5)	//Link bit

// Interrupt Enable/Interrupt Map/Interrupt Source Register Bits
#define	ACC_OVERRUN			(1<<0)
#define	ACC_WATERMARK		(1<<1)
#define ACC_FREE_FALL		(1<<2)
#define	ACC_INACTIVITY		(1<<3)
#define	ACC_ACTIVITY		(1<<4)
#define ACC_DOUBLE_TAP		(1<<5)
#define	ACC_SINGLE_TAP		(1<<6)
#define	ACC_DATA_READY		(1<<7)

// Data Format Bits
#define ACC_RANGE_0			(1<<0)
#define	ACC_RANGE_1			(1<<1)
#define ACC_JUSTIFY			(1<<2)
#define	ACC_FULL_RES		(1<<3)
#define	ACC_INT_INVERT		(1<<5)
#define	ACC_SPI				(1<<6)
#define	ACC_SELF_TEST		(1<<7)

// FIFO Bits
#define ACC_FIFO_MODE_BYPASS	(0<<6)
#define ACC_FIFO_MODE_PAUSE		(1<<6)
#define ACC_FIFO_MODE_STREAM	(2<<6)
#define ACC_FIFO_MODE_TRIGGER	(3<<6)
#define ACC_FIFO_TRIGGER		(1<<5)

#define QS_QUEUEFULL 0x01
#define QS_QUEUED    0x02

#define COM_MAXRX 256
#define COM_MAXTX 256

#define CLOCK_DEFAULT_TIME 1800

#define START_TIME (( 8*3600UL)+(30 * 60UL)+ 0UL)
#define STOP_TIME    ((17*3600UL)+( 0 * 60UL)+ 0UL)

#define MORNING_TIME (( 8*3600UL)+(30 * 60UL)+ 0UL)
#define LUNCH_TIME   ((12*3600UL)+(30 * 60UL)+ 0UL)
#define HOME_TIME_F  ((14*3600UL)+(30 * 60UL)+ 0UL)
#define HOME_TIME_N  ((17*3600UL)+( 0 * 60UL)+ 0UL)

#define MAX_VOLT_IP 6
#define MAX_BUFFERING_ADC 10

#define PWM_FREQUENCY 200 // Default PWM Frequency
#define PWM_PRESCALE (TIMER_CLOCK_FREQUENCY / 65536 / PWM_FREQUENCY)
#define PWM_PERIOD (TIMER_CLOCK_FREQUENCY / (PWM_PRESCALE + 1) / PWM_FREQUENCY)
#define PWM_DUTY100 10000
#define PWM_DUTY000 0
#define PWM_STEP 1000

#define PWM_OUTMAX 8

enum freqInputs {
	INPUT_IN01,
	INPUT_IN02,
	INPUT_IN03,
	INPUT_IN04,
	INPUT_IN05,
	INPUT_IN06,
	MAX_INPUTS
	};

enum pwmOutputs {
	PWM_BACK,
	PWM_ILLU,
	PWM_OP01,
	PWM_OP02,
	PWM_OP03,
	PWM_OP04,
	PWM_OP05,
	PWM_OP06,
	PWM_OP07,
	PWM_OP08,
	MAX_PWM_OUTPUTS
	};

enum staticOutputs {
	OUT_OP01,
	OUT_OP02,
	OUT_OP03,
	OUT_OP04,
	OUT_OP05,
	OUT_OP06,
	OUT_OP07,
	OUT_OP08,
	OUT_RED,
	OUT_AMBER,
	OUT_GREEN,
	OUT_SHUTDOWN,
	OUT_BUZZER,
	OUT_IEAIP01,
	OUT_IEAIP02,
	OUT_IEAIP03,
	OUT_IEAIP04,
	MAX_OUT_OUTPUTS
	};

enum staticOutputStatus {
	OUT_STATUS,
	OUT_OFF,
	OUT_ON
	};

// Output Definition
#define OUT1      (1 << OUT_OP01)
#define OUT2      (1 << OUT_OP02)
#define OUT3      (1 << OUT_OP03)
#define OUT4      (1 << OUT_OP04)
#define OUT5      (1 << OUT_OP05)
#define OUT6      (1 << OUT_OP06)
#define OUT7      (1 << OUT_OP07)
#define OUT8      (1 << OUT_OP08)
#define LED_RED   (1 << OUT_RED)
#define LED_AMBER (1 << OUT_AMBER)
#define LED_GREEN (1 << OUT_GREEN)
#define SHUTDOWN  (1 << OUT_SHUTDOWN)
#define BUZZER    (1 << OUT_BUZZER)
#define IE_IP01   (1 << OUT_IEAIP01)
#define IE_IP02   (1 << OUT_IEAIP02)
#define IE_IP03   (1 << OUT_IEAIP03)
#define IE_IP04   (1 << OUT_IEAIP04)

#define INPUT_PFAIL          (GPIOB->IDR & GPIO_Pin_2)

#define OUTPUT_LED_RED_ON    (GPIOA->BSRRH = GPIO_Pin_8)
#define OUTPUT_LED_RED_OFF   (GPIOA->BSRRL = GPIO_Pin_8)
#define OUTPUT_LED_AMBER_ON  (GPIOD->BSRRH = GPIO_Pin_11)
#define OUTPUT_LED_AMBER_OFF (GPIOD->BSRRL = GPIO_Pin_11)
#define OUTPUT_LED_GREEN_ON  (GPIOE->BSRRH = GPIO_Pin_12)
#define OUTPUT_LED_GREEN_OFF (GPIOE->BSRRL = GPIO_Pin_12)
#define OUTPUT_BUZZER_ON     (GPIOE->BSRRH = GPIO_Pin_3)
#define OUTPUT_BUZZER_OFF    (GPIOE->BSRRL = GPIO_Pin_3)

#define LARGEARRAYRUN (4096 - 18 - (sizeof( struct PWMsettings ) * PWM_OUTMAX))
#define LARGEARRAYFAC (4096 - 20)

struct Angles {
	float x;
	float y;
	float z;
	};

/*-----------------------------------------------------------*/

// variables
/*-----------------------------------------------------------*/
extern unsigned short int AtoDbufferDMA[MAX_VOLT_IP];

struct SmoothBuffer {
	unsigned short int buffer[MAX_BUFFERING_ADC];
	unsigned short int smooth;
	unsigned char ptr;
	};
extern struct SmoothBuffer AtoDmux[MAX_VOLT_IP];
extern union LongToChar AtoDhp[4];

extern unsigned char const mux[4];
extern unsigned char const default_time[9];
extern unsigned short int const clk_min[6];
extern unsigned short int const clk_max[6];
extern unsigned char const * const DayofWeek[8];
extern unsigned char const * const MonthofYear[13];
extern unsigned char const * const ip_status[2];
extern unsigned char const * const op_status[2];
extern unsigned char const * const type_string[CAN_MODULE_TYPES][2]; // [type][option]

extern unsigned char const * const BuzzerStatus[5];

extern unsigned char const UCkey[];
extern unsigned char const LCkey[];
extern unsigned char const NCkey[];

extern unsigned int const OpSetIOM[11];
extern unsigned int const OpSetIOC[25];
extern unsigned int const OpSetIOB[17];

#ifndef WINDOWS
struct PWMregisters {
	TIM_TypeDef *timer;
	__IO uint32_t *compare;
 	__IO uint16_t *mode_reg;
	uint16_t mode_bits;
	uint16_t mPWM;
	uint16_t mOFF;
	uint16_t mON;
	};
extern struct PWMregisters const PWMreg[MAX_PWM_OUTPUTS];

struct structIoPins {
	GPIO_TypeDef* GPIO;
	uint16_t Pin;
	};
extern struct structIoPins const InputPin[MAX_INPUTS];
extern struct structIoPins const OutputPin[MAX_OUT_OUTPUTS];
#endif

extern unsigned char buffer[GRAPHICS_SIZE+128];
extern unsigned char adm[GRAPHICS_SIZE];
extern unsigned char startup, selection, escape, leaf, windowdepth, nextwindow;
extern unsigned char deftime, reverttime, looptime, offtime, half_second, module_id, module_selection, keytime;

extern void (*main_window)( unsigned int );
extern void (*pin_window)( unsigned int );
extern void (*default_window)( unsigned int );

#define MAX_WINDOWDEPTH 16
struct sdPreviousWindow {
		void (*window)( unsigned int );
		unsigned char selection;
};
extern struct sdPreviousWindow PreviousWindow[MAX_WINDOWDEPTH];

extern unsigned int key;
extern unsigned char savecal, editing, okset, valuechanged, number[16];
extern unsigned char SavecalDelayWithinMenus;
extern unsigned char SavecalDelayOnMainDisplay;

extern unsigned char ClockRam[9];

extern unsigned char buzz_count;
extern unsigned int buzz_length, off_length;

extern unsigned short active_pin;

extern unsigned char COM0_message_received, COM1_message_received;
extern unsigned char COM0_message[COM_MAXRX], COM1_message[COM_MAXRX];

extern unsigned short int clock_display_time;

extern unsigned short int ExtIn;

extern unsigned char const acosTable[256];
extern unsigned char const asinTable[256];
extern unsigned char const atanTable[256];
extern unsigned char const sqrtTable[256];


// Bale Totals (in Bale Stores A to J)
#define MAX_STORES  10

// Programmable Factors (Array)
#define PF_STEP   0  // Step size
#define PF_MIN    1  // Minimum
#define PF_MAX    2  // Maximum
#define PF_DEF    3  // Default
#define PF_DIM    4  // Array dimension

// Programmable Factors (Array Elements)
#define PF_cNA        0 // Not Applicable (Char type)
#define PF_fNA      0.0 // Not Applicable (Float type)

// Bale Profile
#define BP_PRESET_A    0
#define BP_PRESET_B    1
#define BP_PRESET_C    2
#define BP_PRESET_D    3
#define BP_PRESET_E    4
#define BP_PRESET_MAX  5

// Bale Shape Indicator
#define BSI_OFF    0
#define BSI_ON     1
#define BSI_BEEP   2

/* Bale Indicator */
#define BI_STEER   0
#define BI_FILL    1

/* Net System */
#define NS_BEAK    0
#define NS_ROLLER  1

/* Twine Option*/
#define TO_OFF    0
#define TO_ON  	  1

/* Twine or Net*/
#define TO_NET 	  0
#define TO_TWINE  1
#define OP_ROLL_ENGAGE_TIMER 30 /*3 seconds*/

/* Chopping Knives Fitted */
#define CKF_DISABLED  0
#define CKF_ENABLED   1


#define DIA_FIXED_MULTIPLIER 0.013 // Bale diameter fixed multiplier

#define NET_METERS_PER_PULSE 0.047 // m/pulse

// Digital Outputs
#define OP_MASTER_DIV   OUT1 // Master Diverter
#define OP_NET_ON       OUT2 // Net Clutch (AKA Net On)
#define OP_KNIV_DIV     OUT3 // Knife Diverter (AKA Knives Diverter)
#define OP_FLOOR_DIV    OUT4 // Floor Diverter
#define OP_ROLL_ENGAGE  OUT5 // Roller Engage Solenoid (AKA Pre Net Position)
// OUT6 is 12V Sensor Power
#define OP_NET_CUT      OUT7 // Net Cut Motor (AKA Net Cut Trip )
#define OP_DENSITY_REL  OUT8 // Density Release Solenoid (AKA Density Control)

// Digital Inputs
#define FLOOR_WORKING_POS       (ExtIn & (1 << 0))  // IP1: Floor (Position) Sensor closed
#define CHOPPING_KNIVES_ON      (ExtIn & (1 << 1))  // IP2 open: 11 Knife Sensor (AKA Chopping Knives position / Knives Position Sensor 1) in the ‘ON’ position
#define CHAMBER_CLOSED          (ExtIn & (1 << 2))  // IP3: Chamber Locked Sensor closed
#define CHAMBER_OPEN          	!(ExtIn & (1 << 2))  // IP3: Chamber Locked Sensor open
#define BALE_KICKER_UP          (ExtIn & (1 << 4))  // IP5: Bale Kicker Sensor closed
#define BALE_KICKER_DOWN       !(ExtIn & (1 << 4))// IP5: Bale Kicker Sensor open
#define NET_CUT_SENSOR_CLOSED   (ExtIn & (1 << 5))  // IP6: Net Cut Sensor closed


#define KNIVESCHEV 0x01

#define NETFEEDTIME 30

// Stage (switch case IDs)
#define STAGE_START                        0x00
#define STAGE_FORMATION                    0x01
#define STAGE_FORMATION2                   0x02
#define STAGE_CORE                         0x03
#define STAGE_MAIN                         0x04
#define STAGE_NETTING_START                0x05
#define STAGE_NETTING_NET_DELAY            0x06
#define STAGE_NETTING_NET_ON               0x07
#define STAGE_NETTING_WAIT_REQ_NET_PULSES  0x08
#define STAGE_NETTING_NET_CUT_ON           0x09
#define STAGE_NETTING_END                  0x0A
#define STAGE_UNLOADING                    0x0B
#define STAGE_UNLOADING_WAIT               0x0C
#define STAGE_END                          0x0D
#define STAGE_PRE_NET_PULSES               0x0E

//Twine Stages
#define TWINE_START 					0x0F
#define TWINE_OP5 						0x10
#define TWINE_STAGE_3					0x11
#define TWINE_STAGE_4					0x12
#define TWINE_STAGE_5					0x13
#define TWINE_STAGE_6					0x14
#define TWINE_STAGE_7					0x15
#define TWINE_STAGE_8					0x16
#define TWINE_STAGE_9					0x17
#define TWINE_STAGE_10					0x18
#define TWINE_STAGE_11					0x19
#define TWINE_STAGE_12					0x20

// stagecomp (flags)
#define SC_CLEAR        0x00
#define NETFEEDON       0x01
#define NETCUTON        0x02
#define CHAMBOP         0x04
#define CHAMBCLO        0x08

#define SEQ_ALL     0xff
#define SEQ_INIT    0x01
#define SEQ_NET     0x02
#define SEQ_UNLOAD  0x04

#define ON  0
#define OFF 1

// DensityPresVolts
#define DPV_DENSITY_PRESSURE_TOO_HIGH_VOLTS            4.8 // V
#define DPV_DENSITY_PRESSURE_TOO_HIGH_VOLTS_HYSTERESIS 0.2 // V

// ChamberOpenDelayTime (applies to MAN mode only)
#define CODT_CHAMBER_OPEN_TIME          20 // 2s

// ChamberOpenBeepDelay (applies to AUTO mode only)
#define COBD_CHAMBER_OPEN_BEEP_DELAY    10 // 1s

// BaleKickerStatus (flags)
#define BKS_CLEAR               0x00
#define BKS_BALE_KICKER_DOWN    0x01
#define BKS_BALE_KICKER_UP      0x02

// BaleDiameterStatusInManMode (flags)
#define BDSIMM_CLEAR               0x00
#define BDSIMM_BELOW_ENGAGE_DIA    0x01
#define BDSIMM_ABOVE_ENGAGE_DIA    0x02

// ChamberStatus (flags)
#define CS_CLEAR                0x00
#define CS_CLOSED               0x01
#define CS_OPEN                 0x02

// OP8OnCount -- Count for Density Release Solenoid (OP8) on after Chamber (IP3) closes then opens (MAN/AUTO)
#define OP8_ON_COUNT_DURATION   60 // 6s

// IP4PulsesStoppedCount -- Count for Net Pulses (on IP4) to stop after Target Pulses reached for Net Cut (OP7) to be turned off
#define IP4_PULSES_STOPPED_COUNT_DURATION   15 // 1.5 s

// MWarn
#define MW_MASK         0x3F
#define MW_CLEAR        0x00
#define MW_DENSITYPRESS 0x01    // Density Pressure Too High
#define MW_CHECKNET     0x02    // Check Net Error (AKA Net Warning)
#define MW_DROPFLOOR    0x04    // Drop Floor Warning (AKA Floor Position)
#define MW_KNIFEPRES    0x08    // Knife maximum pressure
#define MW_CHOPKNIVPOS1 0x10    // Chopping knives position for bank of 11 knives
#define MW_LOWBAT       0x20    // Low battery warning (AKA Low voltage)
#define MW_NO_CAN_COM       0x40    // No CAN communication
#define MW_NO_IP4_PULSES      0x80    // No HBM-IP4 Pulses
#define MW_TRACK_POSITION 0x100
#define MW_SINGLE_NO_TWINE 0x200
#define MW_DOUBLE_NO_TWINE 0x400

// OKWarnBox
#define WARNMASK    0x03
#define OK          0x00
#define EXCL        0x01
#define LUBE        0x02


#define HBM_OUT_ON 100
#define HBM_OUT_OFF 0

#define FEED_MOTOR_OUT 0
#define TRACK_MOTOR_FORWARD_OUT 1
#define TRACK_MOTOR_REVERSE_OUT 2

// buzz_length -- function related
#define MAN_START_END_CHAMBER_OPEN_DELAY_BEEP  250  // 0.25s beep at start & end of chamber open delay (in MAN mode)
#define AUTO_CHAMBER_OPEN_BEEP                 250  // 0.25s beep while chamber open (in AUTO mode)
#define PROMPT_TO_OPEN_DOOR_BEEP               500  // 0.5 s beep to prompt operator to open the chamber door & unload the bale
#define AUTO_CONFIRM_DOOR_OPENED_BEEP          250  // 0.25s to confirm chamber door has been opened (in AUTO mode)
#define AUTO_CONFIRM_BALE_UNLOADED_BEEP        250  // 0.25s to confirm bale has been unloaded (in AUTO mode)
#define AUTO_CONFIRM_DOOR_CLOSED_BEEP          250  // 0.25s to confirm chamber door has been closed (in AUTO mode)
#define ALL_STAGES_COMPLETE_BEEP              1000  // 1 s beep on completion of all stages
#define NETTING_START_BEEP                    3000  // 3 s beep when netting starts in AUTO mode / can be started in MAN mode
#define PRE_WARNING_BEEP                      1000  // 1 s beep when Pre-target bale diameter reached (in MAN/AUTO mode)

// buzz_length & delay -- for beeps accompanying driving arrows
#define DRIVING_ARROWS_X4_BEEP        50  // 50ms beep length (for each of 4 beeps)
#define DRIVING_ARROWS_X4_BEEP_DELAY  15  // 1.5 s delay (before first beep of 4 starts)
#define DRIVING_ARROWS_X3_BEEP       100  // 100ms beep length (for each of 3 beeps)
#define DRIVING_ARROWS_X3_BEEP_DELAY  20  // 2.0 s delay (before first beep of 3 starts)
#define DRIVING_ARROWS_X2_BEEP       200  // 200ms beep length (for each of 2 beeps)
#define DRIVING_ARROWS_X2_BEEP_DELAY  25  // 2.5 s delay (before first beep of 2 starts)
#define DRIVING_ARROWS_X1_BEEP       400  // 400ms beep length
#define DRIVING_ARROWS_X1_BEEP_DELAY  30  // 3.0 s delay (before beep starts)

// Pin code
#define PINTECH1 3473    // Released version
#define POTDIV   1.568  // Potential divider scaling factor applied to analogue input voltages


struct PWMsettings {
	unsigned char enable;
	unsigned char status;
	unsigned short freq;
	unsigned short duty;
	};

// runtime variables for serial flash save held in this structure
struct PreservedRuntime {
	// general runtime
	unsigned char Brightness[2];
	unsigned char Power;
	unsigned char Tempcomp;
	unsigned char Bias;
	unsigned char Gain;
	unsigned char GrayScale;
	unsigned char Contrast[2];
	unsigned char LineRate;
	unsigned char PixelMask;

	unsigned char PipsOn;
	unsigned char BuzzerTest;
	unsigned char DayNight;
	unsigned char memory_status;

	unsigned short int PowerCount;
	signed short int LubeCounter;
	struct PWMsettings PWMout[PWM_OUTMAX];


	unsigned long int CheckSum;
	};

//This is done to match the structure for factors in original GWIZ
struct PreservedRuntimePR {
	struct PreservedRuntime pr;

};

extern struct PreservedRuntimePR pRun;

// factors for serial flash save held in this structure
struct PreservedFactors {
	// general factors
	unsigned short int software_type;
	unsigned short int software_issue;
	unsigned short int software_revision;

	unsigned char Lang;

	unsigned long int COM0Baud;
	unsigned long int COM1Baud;



	unsigned long int GrandBaleTotal;        // Default=0 Bales; Increment=1 Bales; Range=0-99999 Bales


	unsigned long int BaleTotal[MAX_STORES]; // Bale Totals (in Bale Stores A to J)
	unsigned char BaleStore;                 // Bale Store ID: A - J

	unsigned char BaudRate;                  // Not set/used

	// Machine Menu
	unsigned char GW302_BaleProfile;        // Preset A to E
	unsigned char GW302_BaleDiameter[BP_PRESET_MAX]; // cm
	unsigned char GW302_CoreDiameter[BP_PRESET_MAX]; // cm
	unsigned char GW302_BaleDensity[BP_PRESET_MAX];  // ID
	unsigned char GW302_CoreDensity[BP_PRESET_MAX];  // ID
	float GW302_NetLayers[BP_PRESET_MAX];            // layers
	// LubeCounter                          // Moved to preserved run-time totals
	// GrandBaleTotal                       // Moved to early position in memory since value is to be preserved between successive software versions
	float GW302_DiaPotZero;                 // V
	signed char GW302_DiameterCorrection;   // cm
	unsigned char GW302_BaleShapeIndicator; // On/Off/Beep
	float GW302_BaleShapeIndicatorZero;     // V
	float GW302_TotalNetMetres;             // m
	unsigned char GW302_PreWarningBeep;     // %
	float GW302_NetDelay;                   // s
	unsigned char GW302_NetTwineOption; 		  // Net/Twine
	float GW302_TwineDelay;

	// Technician Menu - Baler Full Page 1/2
	unsigned short int GW302_LubeAlarm;     // bales
	float GW302_PresDeadband;               // V
	float GW302_NetSpeed;                   // Hz
	float GW302_PresSwDelay;                // s
	float GW302_DoorClosed;                 // s
	unsigned char GW302_BaleSizeFactor;

	// Technician Menu - Twine Setup 1/2
	unsigned char GW302_TwineOption;          // Twine Option
	unsigned char GW302_TwineFeedDiameter;          // Feed Diameter
	float GW302_TwineFeedTime; // Feed Time
	float GW302_TwineFeedSpeed; // Feed Speed
	float GW302_TwineTrackDelay;// Track Delay
	float GW302_TwineLenghtPulse; //Twine Length Pulse
	float GW302_TrackDistancePulse; //Track Distance Pulse
	unsigned short int GW302_TrackMaxFwd; //Track Max Fwd
	unsigned short int GW302_TrackMaxRev;
	unsigned short int GW302_TwineMinimumEdgeDistance;
	unsigned short int GW302_TwineMinimumCentreDistance;
	unsigned short int GW302_TwineTrackProtection;

	unsigned char GW302_PreTwinePulses;
	unsigned char GW302_PreTwineDiam;

	// Technician Menu - Baler Full Page 2/2
	float GW302_FillPotRange;               // V
	float GW302_FillPotIgnore;              // V
	unsigned char GW302_EngageDiameter;     // %
	unsigned char GW302_BaleIndicator;      // Steer/Fill
	float GW302_RollerDelay;                // s
	unsigned char GW302_DiagnosticMode;
	float GW302_EngageTime;
	// Technician Menu - Netting Setup Page 1/2
	float GW302_NetOn;                      // s
	unsigned char GW302_NetCutPulse;        // pulses
	float GW302_NetCutTime;                 // s
	float GW302_MinDiaPulsPerLay;           // pulses/layer
	float GW302_MaxDiaPulsPerLay;           // pulses/layer
	float GW302_NetFactor;
	float GW302_NoNetPulse;                 // s
	unsigned char GW302_PreNetPulse;        // pulses

	// Technician Menu - Netting Setup Page 2/2
	unsigned char GW302_NetSystem;          // Beak/Roller
	unsigned char GW302_BeakEngage;         // pulses

	// Technician Menu - Diameter Setup
	unsigned char GW302_DiameterMin;        // %
	unsigned char GW302_DiameterMax;        // %
	float GW302_DiaCorrection;              // V
	unsigned char GW302_BaleDiaSmoothingFactor;     // %
	unsigned char GW302_DensityPresSmoothingFactor; // %
	unsigned char GW302_BaleShapeSmoothingFactor;   // %
	unsigned char GW302_KnifePresSmoothingFactor;   // %

	// Technician Menu - Density Setup Page 1/2
	float GW302_Density1;                   // V
	float GW302_Density2;                   // V
	float GW302_Density3;                   // V
	float GW302_Density4;                   // V
	float GW302_Density5;                   // V
	float GW302_Density6;                   // V

	// Technician Menu - Density Setup Page 2/2
	float GW302_Density7;                   // V
	float GW302_Density8;                   // V
	float GW302_Density9;                   // V
	float GW302_Density10;                  // V

	// Technician Menu - Knives Setup
	float GW302_KnifePressure;              // V
	float GW302_KnifePresWarn;              // V
	float GW302_DiverterDelay;              // s

	// Technician Menu - Formation 2
	unsigned char GW302_FormationDensity[BP_PRESET_MAX];   // ID
	unsigned char GW302_FormationDiameter[BP_PRESET_MAX];  // %
	unsigned char GW302_Formation2Density[BP_PRESET_MAX];  // ID
	unsigned char GW302_Formation2Diameter[BP_PRESET_MAX]; // %

	// Diagnostics - Digital Inputs
	unsigned char GW302_ChoppingKnivesFitted; // Disabled/Enabled


	//Twine Factors
	unsigned char TwineNumberOfRevAroundEdge;
	unsigned char TwineDistanceFromEdge;
	unsigned char TwineNumberOfStopsBetweenEdgeAndCentre;
	float TwineStopTime;
	unsigned char TwineNumberOfRevolutionsAroundCentre;
	unsigned char DistanceFromCentre;
	float NoTwineWarningTime;
	unsigned char GW302_TwineEngageDiameter;
	unsigned short int GW302_PreTwineRev;
	unsigned short int GW302_PreTwine2Rev;

	unsigned long int CheckSum;
	};
//This is done to match the structure for factors in original GWIZ
struct PreservedFactorsPF {
	struct PreservedFactors pf;

};

extern struct PreservedFactorsPF pFac;
/*-----------------------------------------------------------*/



/*McHale Variables*/

extern unsigned long int BaleDiaVoltsTotAccumMultSamples;     // V
extern unsigned long int DensityPresVoltsTotAccumMultSamples; // V
extern unsigned long int BaleShapeVoltsTotAccumMultSamples;   // V
extern unsigned long int KnifePresVoltsTotAccumMultSamples;   // V

extern unsigned long int BeakEngagePulseCount; // Pulses
extern unsigned long int NetPulseCountPrev; // Pulses -- Similar to NetPulseCountAtLastCheck but used in display loop
extern unsigned long int NewNetPulses; // Pulses
extern unsigned long int NetMetresPulseCount; // Pulses
extern float NetMetresAppliedForCount; // m
extern float NetPulseFrequency; // Hz
extern float NetPulsesPerLayer; // Pulses/layer
extern unsigned short int TargetNetPulses; // Pulses

// language string pointer
extern char const * const *ptText;
// variables - constant
extern char const * const * const TextString[2];

// Programmable Factors (Array)

// Machine Menu -- Step size, Minimum, Maximum, Default
extern unsigned char const PF_GW302_BaleProfile[PF_DIM];
extern unsigned char const PF_GW302_BaleDiameter[PF_DIM];
extern unsigned char const PF_GW302_CoreDiameter[PF_DIM];
extern unsigned char const PF_GW302_BaleDensity[PF_DIM];
extern unsigned char const PF_GW302_CoreDensity[PF_DIM];
extern float const         PF_GW302_NetLayers[PF_DIM];
extern unsigned long int const PF_GW302_BaleTotal[PF_DIM];
extern unsigned char const PF_GW302_BaleStore[PF_DIM];
// LubeCounter             // Moved to preserved run-time totals
extern unsigned long int const PF_GW302_GrandBaleTotal[PF_DIM];
extern float const         PF_GW302_DiaPotZero[PF_DIM];
extern signed char const   PF_GW302_DiameterCorrection[PF_DIM];
extern unsigned char const PF_GW302_BaleShapeIndicator[PF_DIM];
extern float const         PF_GW302_BaleShapeIndicatorZero[PF_DIM];
extern float const         PF_GW302_TotalNetMetres[PF_DIM];
extern unsigned char const PF_GW302_PreWarningBeep[PF_DIM];
extern float const         PF_GW302_NetDelay[PF_DIM];
extern unsigned char const PF_GW302_NetTwineOption[PF_DIM];
extern float const PF_GW302_TwineDelay[PF_DIM];

// Technician Menu - Baler Full Page 1/2 -- Step size, Minimum, Maximum, Default
extern unsigned short int const PF_GW302_LubeAlarm[PF_DIM];
extern float const              PF_GW302_PresDeadband[PF_DIM];
extern float const              PF_GW302_NetSpeed[PF_DIM];
extern float const              PF_GW302_PresSwDelay[PF_DIM];
extern float const              PF_GW302_DoorClosed[PF_DIM];
extern unsigned char const      PF_GW302_BaleSizeFactor[PF_DIM];

// Technician Menu - Baler Full Page 2/2 -- Step size, Minimum, Maximum, Default
extern float const         PF_GW302_FillPotRange[PF_DIM];
extern float const         PF_GW302_FillPotIgnore[PF_DIM];
extern unsigned char const PF_GW302_EngageDiameter[PF_DIM];
extern unsigned char const PF_GW302_BaleIndicator[PF_DIM];
extern float const         PF_GW302_RollerDelay[PF_DIM];
extern float const PF_GW302_EngageTime[PF_DIM];

// Technician Menu - Netting Setup Page 1/2 -- Step size, Minimum, Maximum, Default
extern float const         PF_GW302_NetOn[PF_DIM];
extern unsigned char const PF_GW302_NetCutPulse[PF_DIM];
extern float const         PF_GW302_NetCutTime[PF_DIM];
extern float const         PF_GW302_MinDiaPulsPerLay[PF_DIM];
extern float const         PF_GW302_MaxDiaPulsPerLay[PF_DIM];
extern float const         PF_GW302_NetFactor[PF_DIM];
extern float const         PF_GW302_NoNetPulse[PF_DIM];
extern unsigned char const PF_GW302_PreNetPulse[PF_DIM];

// Technician Menu - Netting Setup Page 2/2 -- Step size, Minimum, Maximum, Default
extern unsigned char const PF_GW302_NetSystem[PF_DIM];
extern unsigned char const PF_GW302_BeakEngage[PF_DIM];


// Technician Menu - Twine Setup Page 1/2
extern unsigned char const PF_GW302_TwineOption[PF_DIM];          // Twine Option
extern unsigned char const PF_GW302_TwineFeedDiameter[PF_DIM]; // Feed Diameter
extern float const         PF_GW302_TwineFeedTime[PF_DIM]; // Feed Time
extern float const         PF_GW302_TwineFeedSpeed[PF_DIM]; // Feed Speed
extern float const PF_GW302_TwineTrackDelay[PF_DIM]; // Track Delay
extern float const PF_GW302_TwineLenghtPulse[PF_DIM]; // Twine Length Pulse
extern float const PF_GW302_TrackDistancePulse[PF_DIM]; // Track Distance Pulse
extern unsigned short int const PF_GW302_TrackMaxFwd[PF_DIM]; // Track Max Fwd
extern unsigned short int const PF_GW302_TrackMaxRev[PF_DIM]; // Track Max Rev
extern unsigned short int const PF_GW302_TwineMinimumEdgeDistance[PF_DIM]; // Minimum Edge Distance
extern unsigned short int const PF_GW302_TwineMinimumCentreDistance[PF_DIM]; // Minimum Centre Distance
extern unsigned short int const PF_GW302_TwineTrackProtection[PF_DIM]; // ms

extern unsigned char PF_GW302_PreTwinePulses[PF_DIM];
extern unsigned char PF_GW302_PreTwineDiam[PF_DIM];
extern unsigned char const PF_GW302_TwineEngageDiameter[PF_DIM];
extern unsigned short int const         PF_GW302_PreTwineRev[PF_DIM];
extern unsigned short int const         PF_GW302_PreTwine2Rev[PF_DIM];

// Technician Menu - Diameter Setup -- Step size, Minimum, Maximum, Default
extern unsigned char const PF_GW302_DiameterMin[PF_DIM];
extern unsigned char const PF_GW302_DiameterMax[PF_DIM];
extern float const         PF_GW302_DiaCorrection[PF_DIM];
extern unsigned char const PF_GW302_BaleDiaSmoothingFactor[PF_DIM];
extern unsigned char const PF_GW302_DensityPresSmoothingFactor[PF_DIM];
extern unsigned char const PF_GW302_BaleShapeSmoothingFactor[PF_DIM];
extern unsigned char const PF_GW302_KnifePresSmoothingFactor[PF_DIM];

// Technician Menu - Density Setup Page 1/2 -- Step size, Minimum, Maximum, Default
extern float const PF_GW302_Density1[PF_DIM];
extern float const PF_GW302_Density2[PF_DIM];
extern float const PF_GW302_Density3[PF_DIM];
extern float const PF_GW302_Density4[PF_DIM];
extern float const PF_GW302_Density5[PF_DIM];
extern float const PF_GW302_Density6[PF_DIM];

// Technician Menu - Density Setup Page 2/2 -- Step size, Minimum, Maximum, Default
extern float const PF_GW302_Density7[PF_DIM];
extern float const PF_GW302_Density8[PF_DIM];
extern float const PF_GW302_Density9[PF_DIM];
extern float const PF_GW302_Density10[PF_DIM];

// Technician Menu - Knives Setup -- Step size, Minimum, Maximum, Default
extern float const PF_GW302_KnifePressure[PF_DIM];
extern float const PF_GW302_KnifePresWarn[PF_DIM];
extern float const PF_GW302_DiverterDelay[PF_DIM];

// Technician Menu - Formation 2 -- Step size, Minimum, Maximum, Default
extern unsigned char const PF_GW302_FormationDensity[PF_DIM];
extern unsigned char const PF_GW302_FormationDiameter[PF_DIM];
extern unsigned char const PF_GW302_Formation2Density[PF_DIM];
extern unsigned char const PF_GW302_Formation2Diameter[PF_DIM];

// Diagnostics - Digital Inputs -- Step size, Minimum, Maximum, Default
extern unsigned char const PF_GW302_ChoppingKnivesFitted[PF_DIM];

//Twine Variables
extern unsigned char const PF_TwineNumberOfRevAroundEdge[PF_DIM];
extern unsigned char const PF_TwineDistanceFromEdge[PF_DIM];
extern unsigned char const PF_TwineNumberOfStopsBetweenEdgeAndCentre[PF_DIM];
extern float const PF_TwineStopTime[PF_DIM];
extern unsigned char const PF_TwineNumberOfRevolutionsAroundCentre[PF_DIM];
extern unsigned char const PF_DistanceFromCentre[PF_DIM];
extern float const PF_NoTwineWarningTime[PF_DIM];

// Bale Profile defaults for Presets A - E
extern unsigned char const PresetCoreDiameter[BP_PRESET_MAX];
extern unsigned char const PresetCoreDensity[BP_PRESET_MAX];
extern unsigned char const PresetBaleDiameter[BP_PRESET_MAX];
extern unsigned char const PresetBaleDensity[BP_PRESET_MAX];
extern unsigned char const PresetFormationDiameter[BP_PRESET_MAX];
extern unsigned char const PresetFormationDensity[BP_PRESET_MAX];
extern unsigned char const PresetFormation2Diameter[BP_PRESET_MAX];
extern unsigned char const PresetFormation2Density[BP_PRESET_MAX];
extern float const PresetNetLayers[BP_PRESET_MAX];

extern unsigned char const Bar[10];

extern unsigned char lastoutput, nextoutput, OKWarnBox, KnifePresWasHigh, SecondScreen;
extern unsigned int SeqOpOn, SeqOpOff, KeyOpOn, KeyOpOff, OpOff, OpOn;
extern unsigned char DisableChoppingKnivesWarnings;
extern unsigned char Kniv1_IPClosed, AutoMan;
extern unsigned char ContinueOnKeyRelease;
extern unsigned char StartAutoNet;
extern unsigned short int MWarn;

extern unsigned char Stage, DelayTime, BeepDelay;
extern unsigned char stagecomp, knivesdelay, cont_beep;
extern unsigned char target_bale_dia_reached_cont_beep;
extern unsigned char pre_warning_cont_beep;
extern unsigned char start_chamber_open_delay_cont_beep;
extern unsigned char end_chamber_open_delay_cont_beep;

extern float DiaTargetVolts;    // V

extern float DiaMinVolts;       // V
extern float DiaMaxVolts;       // V

extern float DiaMinExtDisplayPercent;    // 0.0 - 100.0 % of extended display range from PotZero to DiaMax
extern float DiaTargetExtDisplayPercent; // 0.0 - 100.0 % of extended display range from PotZero to DiaMax
extern float BaleDiaExtDisplayPercent;   // 0.0 - 100.0 % of extended display range from PotZero to DiaMax

extern float DiaFormationVolts;
extern float DiaFormation2Volts;
extern float DiaCoreVolts;
extern float DiaEngageVolts;
extern float TwineDiaEngageVolts;
extern float DiaFeedVolts;                     // V
extern float DiaPreWarnVolts;

extern float DensityBaleVolts;
extern float DensityCoreVolts;
extern float DensityFormation2Volts;
extern float DensityFormationVolts;

extern unsigned char DensityPressureTooHigh;

extern unsigned char SequenceInProgress;
extern unsigned long int NetPulseCount; // Pulses
extern unsigned char OP2OnDelayTime, OP2OffDelayTime;
extern unsigned char OP7OffDelayTime;
extern unsigned char OP8OnDelayTime, OP8OffDelayTime;
extern unsigned char NetWarningDelayTime;

extern unsigned char NetWarningOn; // Check Net Error (AKA Net Warning) On/Off switch

extern unsigned char RollEngageTurnedOn;      // OP5 Roller Engage Solenoid (AKA Pre Net Position) turned on
extern unsigned char SavedRollEngageTurnedOn; // Saved when IP3 opens in AUTO mode
extern unsigned char NetWarningDisabledWhileIP3OpenInAutoMode;
extern unsigned char NettingStartBeepsDisabledWhileIP3OpenInAutoMode; // Including Pre Warning & Net Delay beep
extern unsigned char RollerTimer;      // OP5 Roller Engage Solenoid (AKA Pre Net Position) turned on for 3 seconds

extern unsigned char RollEngageTurnedOnInManMode; // OP5 Roller Engage Solenoid (AKA Pre Net Position) turned on in MAN mode

extern unsigned char ChoppingKnives1WarningOn; // Chopping knives position 1 Warning On/Off switch

extern unsigned char ChamberClosedDelayTime; // Door Closed

extern unsigned char BalerFullDelayTime;     // applies to MAN mode only -- Set to Net Delay

extern unsigned char ChamberOpenDelayTime;   // applies to MAN mode only

extern unsigned char ChamberOpenBeepDelay;   // applies to AUTO mode only

extern unsigned char BaleKickerStatus;                // Bale Kicker Down/Up flags
extern unsigned char BaleDiameterStatusInManMode;     // Bale Diameter Below/Above Engage Diameter (in MAN mode) flags
extern unsigned char ChamberStatus;                   // Chamber Closed/Open flags

extern unsigned char OP8OnCount;             // Count for Density Release Solenoid (OP8) on after Chamber (IP3) closes then opens (MAN/AUTO)

extern unsigned char IP4PulsesStoppedCount;        // Count for Net Pulses (on IP4) to stop after Target Pulses reached for Net Cut (OP7) to be turned off
extern unsigned long int NetPulseCountAtLastCheck; // Pulses -- Similar NetPulseCountPrev to but used in auto sequence loop

extern unsigned char DrivingArrowBeepsDisabled;    // Driving Arrow Beeps turned off/on

extern unsigned char DensityControlFunctionActive;                 // Density Control Function (based on Target Bale Density) active
extern unsigned char NetPulseCheckingEnabled;                      // Checking for no pulses on IP4 (for NetWarningDelayTime) enabled
extern unsigned char OP5TurnedOnAfterBeakEngagePulsesEnabled;      // Turning on Roller Engage Solenoid (OP5) after 'Beak Engage' pulses counted on IP4
extern unsigned char SavedOP5TurnedOnAfterBeakEngagePulsesEnabled; // Saved when IP3 opens in AUTO mode
extern float PreTwineVoltage;
extern float BaleDiaVolts;
extern float DensityPresVolts;
extern float BaleShapeVolts;
extern float KnifePresVolts;

extern unsigned char Screen100Page;
extern unsigned char SelectedBaleStore;

extern unsigned char swrel;
extern unsigned char const Store[10];

extern unsigned char TwineAutoManMode;
extern unsigned short int FordwardPulses, TwinePulses, ReversePulses, ReversePulsesBetweenStops, TargetEdgePulses, TargetCentrePulses;
extern unsigned short int FwdOvershootPulses;
extern unsigned char AutoTwineCutButtonPressed;
extern unsigned char PreTwineSeq, PreTwineSeqActive;

struct PinWin {
  unsigned short int PinNumber;
  void (*PinWindow)(unsigned int);

  };
extern struct PinWin const PinOption;
extern float SupplyVolts;
// function prototypes
/*-----------------------------------------------------------*/
// function prototypes
#ifdef WINDOWS
 // function not available in borland builder
 extern float cosf( float x );
 extern float acosf( float x );
 extern float sinf( float x );
 extern float asinf( float x );
 extern float atoff( const char *s );
#endif


// function prototypes
extern void StartAtoD( void );
extern void CheckAnalogueLevels( void );
extern void CheckInputs( void );
extern void CalculateFrequencies( void );
extern void SetupPWMTimers( void );
extern void SetOutputPWM( enum pwmOutputs output, unsigned int freq, unsigned int duty );
extern enum staticOutputStatus SetOutputOUT( enum staticOutputs output, enum staticOutputStatus status );
extern unsigned long int CheckSumIt( unsigned char *data, unsigned char *end );
extern void SetDefaultFactors( void );
extern void SetDefaultTotals( void );
extern void SetStartupDefaults( void );
extern unsigned char btobcd( unsigned char bin );
extern unsigned char bcdtob( unsigned char bcd );
extern void SetCurrentTime( unsigned char *time );
extern void SetCanTime( struct CanMessage *CanM_Rx );
extern void PipsGen( void );
extern unsigned int integer_sqrt( unsigned int x );
extern void CalculateAngleSingle( struct Angles *ptXYZ, signed short int x, signed short int y, signed short int z );
extern void LookupAngleSingle( struct Angles *ptXYZ, signed short int x, signed short int y, signed short int z );
extern void CalculateAngleTriple( struct Angles *ptXYZ, signed short int x, signed short int y, signed short int z );
extern float inv_tan( unsigned int ui );
extern void LookupAngleTriple( struct Angles *ptXYZ, signed short int x, signed short int y, signed short int z );
/*-----------------------------------------------------------*/

/*McHale Function prototypes*/
extern void ResetSequenceFactors( unsigned char seq );
extern void AllSeqOpOff( void );
extern void ResetLubeCount( void );
extern void CalculateAnalogInputRollingAverages(void);
extern void Sequence( void );
extern void TwineSequence(void);
extern void Inputs( void );
extern void CheckAlarms( void );
extern void CalculateNetPulseFrequency( void );
extern void CalculateNetPulsesPerLayer( void );
extern void CalculateTargetNetPulses( void );
extern void UpdatingAndSavingTotalsToMemory(void);
extern unsigned char TwineAlarmAck;

extern unsigned char stopPulseAccu;
extern unsigned char TwineStage;
extern unsigned char TwineSeqStart;
extern unsigned char CheckIP4Pulses, tempIP4Pules;
#endif /* FUNCTIONS_H_ */
