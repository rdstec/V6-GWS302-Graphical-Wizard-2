// Graphic Wizard Display Related File

// Include Standard files
#include "global.h"

#ifdef WINDOWS
 // function not available in borland builder
 float cosf( float x ) { return cos( x ); }
 float acosf( float x ) { return acos( x ); }
 float sinf( float x ) { return sin( x ); }
 float asinf( float x ) { return asin( x ); }
 float atanf( float x ) { return atan( x ); }
 float atoff( const char *s ) { return atof( s ); }
 unsigned char ClockRam[9];
 unsigned char PowerSwitchedOff;
#endif

unsigned char stopPulseAccu;
unsigned char AutoTwineCutButtonPressed, tempIP4Pules;
/*-----------------------------------------------------------*/

// global variables
/*-----------------------------------------------------------*/
// constants
unsigned char const mux[4] = { 0x01, 0x23, 0x45, 0x67 };	
unsigned char const default_time[9] = {
	0x00, // seconds
	0x00, // minutes
	0x12, // hours
	0x03, // day of week
	0x25, // day of month
	0x05, // century & month
	0x16, // year
	0X00, // control
	0x00  // cal
	};
unsigned short int const clk_min[6] = {  0,  0, 0,  1,  1, 2000 };
unsigned short int const clk_max[6] = { 23, 59, 6, 31, 12, 2099 };
unsigned char const * const DayofWeek[8] =
 { "Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat", "---" };
unsigned char const * const MonthofYear[13] =
 { "---", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
unsigned char const * const ip_status[2] = { SWOPEN, SWCLOSED };
unsigned char const * const op_status[2] = { BUTTON04, BUTTON01 };
unsigned char const * const type_string[CAN_MODULE_TYPES][2] = {
	"MCM x %u", "%u. MCM%u on CAN%u",
	"ACM x %u", "%u. ACM%u on CAN%u",
	"HBM x %u", "%u. HBM%u on CAN%u",
	"WCM x %u", "%u. WCM%u on CAN%u",
	"DOP x %u", "%u. DOP%u on CAN%u",
	"DIP x %u", "%u. DIP%u on CAN%u",
	"LMM x %u", "%u. LMM%u on CAN%u",
	"IOM x %u", "%u. IOM%u on CAN%u",
	"IOC x %u", "%u. IOC%u on CAN%u",
	"IOB x %u", "%u. IOB%u on CAN%u"
};

unsigned char const * const BuzzerStatus[5] = {
	"OFF",
	"ON",
	"TEST",
	"POWER ONCE",
	"POWER LOOP" };

unsigned char const UCkey[] = " ÀÁÂÃÄÅÆABCÇDEÈÉÊËFGHIÌÍÎÏÐJKLMNÑOÒÓÔÕÖØPQRSTUÙÚÛÜVWXYÝßZÞ";
unsigned char const LCkey[] = " aàáâãäåæbcçdeèéêëfghiìíîïðjklmnñoòóôõöøpqrstuùúûüvwxyýÿzþ";
unsigned char const NCkey[] = "0123456789.+-±!#¢£$";
/*    
	"!\"#$%&'()*+,-./:;<=>?@[\\]^_`"
	"{|}~ ¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿"
	"0123456789"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß"
	"àáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ"
*/

unsigned int const OpSetIOM[11] = { 0,
	IOM_OP01, IOM_OP02, IOM_OP03, IOM_OP04, IOM_OP05,
	IOM_OP06, IOM_OP07, IOM_OP08, IOM_OP09, IOM_OP10 };

unsigned int const OpSetIOC[25] = { 0,
	IOC_PWM1, IOC_PWM2, IOC_PWM3, IOC_OP01, IOC_PWM4, IOC_OP02, IOC_PWM5, IOC_OP03,
	IOC_PWM6, IOC_OP04, IOC_PWM7, IOC_OP05, IOC_PWM8, IOC_OP06, IOC_OP07, IOC_OP08,
	IOC_OP09, IOC_OP10, IOC_OP11, IOC_OP12, IOC_OP13, IOC_OP14, IOC_OP15, IOC_OP16 };

unsigned int const OpSetIOB[17] = { 0,
	IOB_PWM1, IOB_OP01, IOB_PWM2, IOB_OP02, IOB_PWM8, IOB_OP08, IOB_PWM7, IOB_OP07,
	IOB_PWM6, IOB_OP06, IOB_PWM5, IOB_OP05, IOB_PWM4, IOB_OP04, IOB_PWM3, IOB_OP03 };

#ifndef WINDOWS
/*struct structIoPins {
	GPIO_TypeDef* GPIO;
	uint16_t Pin;
	};*/
struct structIoPins const InputPin[MAX_INPUTS] = {
	GPIOE, GPIO_Pin_9,  // DIP01
	GPIOE, GPIO_Pin_11, // DIP02
	GPIOE, GPIO_Pin_13, // DIP03
	GPIOE, GPIO_Pin_14, // DIP04
	GPIOA, GPIO_Pin_0,  // DIP05
	GPIOA, GPIO_Pin_1   // DIP06
	};

/*struct structIoPins {
	GPIO_TypeDef* GPIO;
	uint16_t Pin;
	};*/
struct structIoPins const OutputPin[MAX_OUT_OUTPUTS] = {
	GPIOC, GPIO_Pin_6,  // OUT1
	GPIOC, GPIO_Pin_7,  // OUT2
	GPIOB, GPIO_Pin_0,  // OUT3
	GPIOB, GPIO_Pin_1,  // OUT4
	GPIOD, GPIO_Pin_12, // OUT5
	GPIOD, GPIO_Pin_13, // OUT6
	GPIOD, GPIO_Pin_14, // OUT7
	GPIOD, GPIO_Pin_15, // OUT8
	GPIOA, GPIO_Pin_8,  // LED_RED
	GPIOD, GPIO_Pin_11, // LED_AMBER
	GPIOE, GPIO_Pin_12, // LED_GEEN
	GPIOE, GPIO_Pin_15, // SHUTDOWN
	GPIOE, GPIO_Pin_3,  // BUZZER
	GPIOE, GPIO_Pin_4,  // IE_AIP01
	GPIOE, GPIO_Pin_7,  // IE_AIP02
	GPIOE, GPIO_Pin_8,  // IE_AIP03
	GPIOE, GPIO_Pin_10  // IE_AIP04
	};

/*struct PWMregisters {
	TIM_TypeDef *timer;
	__IO uint32_t *compare;
 	__IO uint16_t *mode_reg;
	uint16_t mode_bits;
	uint16_t mPWM;
	uint16_t mOFF;
	uint16_t mON;
	};*/
struct PWMregisters const PWMreg[MAX_PWM_OUTPUTS] = {
	TIM9,  &TIM9->CCR1,  &TIM9->CCMR1,  TIM_CCMR1_OC1M, TIM_OCMode_PWM1 << 0, TIM_OCMode_Inactive << 0, TIM_OCMode_Active << 0,
	TIM10, &TIM10->CCR1, &TIM10->CCMR1, TIM_CCMR1_OC1M, TIM_OCMode_PWM1 << 0, TIM_OCMode_Inactive << 0, TIM_OCMode_Active << 0,
	TIM3,  &TIM3->CCR1,  &TIM3->CCMR1,  TIM_CCMR1_OC1M, TIM_OCMode_PWM1 << 0, TIM_OCMode_Inactive << 0, TIM_OCMode_Active << 0,
	TIM3,  &TIM3->CCR2,  &TIM3->CCMR1,  TIM_CCMR1_OC2M, TIM_OCMode_PWM1 << 8, TIM_OCMode_Inactive << 8, TIM_OCMode_Active << 8,
	TIM3,  &TIM3->CCR3,  &TIM3->CCMR2,  TIM_CCMR2_OC3M, TIM_OCMode_PWM1 << 0, TIM_OCMode_Inactive << 0, TIM_OCMode_Active << 0,
	TIM3,  &TIM3->CCR4,  &TIM3->CCMR2,  TIM_CCMR2_OC4M, TIM_OCMode_PWM1 << 8, TIM_OCMode_Inactive << 8, TIM_OCMode_Active << 8,
	TIM4,  &TIM4->CCR1,  &TIM4->CCMR1,  TIM_CCMR1_OC1M, TIM_OCMode_PWM1 << 0, TIM_OCMode_Inactive << 0, TIM_OCMode_Active << 0,
	TIM4,  &TIM4->CCR2,  &TIM4->CCMR1,  TIM_CCMR1_OC2M, TIM_OCMode_PWM1 << 8, TIM_OCMode_Inactive << 8, TIM_OCMode_Active << 8,
	TIM4,  &TIM4->CCR3,  &TIM4->CCMR2,  TIM_CCMR2_OC3M, TIM_OCMode_PWM1 << 0, TIM_OCMode_Inactive << 0, TIM_OCMode_Active << 0,
	TIM4,  &TIM4->CCR4,  &TIM4->CCMR2,  TIM_CCMR2_OC4M, TIM_OCMode_PWM1 << 8, TIM_OCMode_Inactive << 8, TIM_OCMode_Active << 8
	};
#endif

/*struct TimeDef {
  unsigned long int time;
  unsigned char stage;
  unsigned char day;
  };*/
struct TimeDef const TD[13] = {
  HOME_TIME_N-0,  0, 0x1e,
  HOME_TIME_N-1,  1, 0x1e,
  HOME_TIME_N-6,  2, 0x1e,

  HOME_TIME_F-0,  0, 0x20,
  HOME_TIME_F-1,  1, 0x20,
  HOME_TIME_F-6,  2, 0x20,

  LUNCH_TIME-0,   0, 0x3e,
  LUNCH_TIME-1,   1, 0x3e,
  LUNCH_TIME-6,   2, 0x3e,

  MORNING_TIME-0, 0, 0x3e,
  MORNING_TIME-1, 1, 0x3e,
  MORNING_TIME-6, 2, 0x3e,

  0, 0, 0xff
  };


/*struct ModuleInfo {
	unsigned int module_type_id;
	unsigned char SoftwareRequest;
};*/
struct ModuleInfo const module_info_table[CAN_MODULE_TYPES * 4] = {
 (CAN_RXID_MCM | (0 << CAN_MOD_BIT_SHIFT)), 0xff, // MCM module 1
 (CAN_RXID_MCM | (1 << CAN_MOD_BIT_SHIFT)), 0xff, // MCM module 2
 (CAN_RXID_MCM | (2 << CAN_MOD_BIT_SHIFT)), 0xff, // MCM module 3
 (CAN_RXID_MCM | (3 << CAN_MOD_BIT_SHIFT)), 0xff, // MCM module 4
 (CAN_RXID_ACM | (0 << CAN_MOD_BIT_SHIFT)), 0xff, // ACM module 1
 (CAN_RXID_ACM | (1 << CAN_MOD_BIT_SHIFT)), 0xff, // ACM module 2
 (CAN_RXID_ACM | (2 << CAN_MOD_BIT_SHIFT)), 0xff, // ACM module 3
 (CAN_RXID_ACM | (3 << CAN_MOD_BIT_SHIFT)), 0xff, // ACM module 4
 (CAN_RXID_HBM | (0 << CAN_MOD_BIT_SHIFT)), 0xff, // HBM module 1
 (CAN_RXID_HBM | (1 << CAN_MOD_BIT_SHIFT)), 0xff, // HBM module 2
 (CAN_RXID_HBM | (2 << CAN_MOD_BIT_SHIFT)), 0xff, // HBM module 3
 (CAN_RXID_HBM | (3 << CAN_MOD_BIT_SHIFT)), 0xff, // HBM module 4
 (CAN_RXID_WCM | (0 << CAN_MOD_BIT_SHIFT)), 0x82, // WCM module 1
 (CAN_RXID_WCM | (1 << CAN_MOD_BIT_SHIFT)), 0x82, // WCM module 2
 (CAN_RXID_WCM | (2 << CAN_MOD_BIT_SHIFT)), 0x82, // WCM module 3
 (CAN_RXID_WCM | (3 << CAN_MOD_BIT_SHIFT)), 0x82, // WCM module 4
 (CAN_RXID_DOP | (0 << CAN_MOD_BIT_SHIFT)), 0xff, // DOP module 1
 (CAN_RXID_DOP | (1 << CAN_MOD_BIT_SHIFT)), 0xff, // DOP module 2
 (CAN_RXID_DOP | (2 << CAN_MOD_BIT_SHIFT)), 0xff, // DOP module 3
 (CAN_RXID_DOP | (3 << CAN_MOD_BIT_SHIFT)), 0xff, // DOP module 4
 (CAN_RXID_DIP | (0 << CAN_MOD_BIT_SHIFT)), 0xff, // DIP module 1
 (CAN_RXID_DIP | (1 << CAN_MOD_BIT_SHIFT)), 0xff, // DIP module 2
 (CAN_RXID_DIP | (2 << CAN_MOD_BIT_SHIFT)), 0xff, // DIP module 3
 (CAN_RXID_DIP | (3 << CAN_MOD_BIT_SHIFT)), 0xff, // DIP module 4
 (CAN_RXID_LMM | (0 << CAN_MOD_BIT_SHIFT)), 0xff, // LMM module 1
 (CAN_RXID_LMM | (1 << CAN_MOD_BIT_SHIFT)), 0xff, // LMM module 2
 (CAN_RXID_LMM | (2 << CAN_MOD_BIT_SHIFT)), 0xff, // LMM module 3
 (CAN_RXID_LMM | (3 << CAN_MOD_BIT_SHIFT)), 0xff, // LMM module 4
 (CAN_RXID_IOM | (0 << CAN_MOD_BIT_SHIFT)), 0xff, // IOM module 1
 (CAN_RXID_IOM | (1 << CAN_MOD_BIT_SHIFT)), 0xff, // IOM module 2
 (CAN_RXID_IOM | (2 << CAN_MOD_BIT_SHIFT)), 0xff, // IOM module 3
 (CAN_RXID_IOM | (3 << CAN_MOD_BIT_SHIFT)), 0xff, // IOM module 4
 (CAN_RXID_IOC | (0 << CAN_MOD_BIT_SHIFT)), 0xff, // IOC module 1
 (CAN_RXID_IOC | (1 << CAN_MOD_BIT_SHIFT)), 0xff, // IOC module 2
 (CAN_RXID_IOC | (2 << CAN_MOD_BIT_SHIFT)), 0xff, // IOC module 3
 (CAN_RXID_IOC | (3 << CAN_MOD_BIT_SHIFT)), 0xff, // IOC module 4
 (CAN_RXID_IOB | (0 << CAN_MOD_BIT_SHIFT)), 0xff, // IOB module 1
 (CAN_RXID_IOB | (1 << CAN_MOD_BIT_SHIFT)), 0xff, // IOB module 2
 (CAN_RXID_IOB | (2 << CAN_MOD_BIT_SHIFT)), 0xff, // IOB module 3
 (CAN_RXID_IOB | (3 << CAN_MOD_BIT_SHIFT)), 0xff  // IOB module 4
};
/*-----------------------------------------------------------*/

// acosf, normalised 255 = 90 degrees
// 256 values in table 0 to 255, 256 = 0
unsigned char const acosTable[256] = {
 255, 254, 254, 253, 252, 252, 251, 251, 250, 249, 249, 248, 247, 247, 246, 245,
 245, 244, 244, 243, 242, 242, 241, 240, 240, 239, 238, 238, 237, 237, 236, 235,
 235, 234, 233, 233, 232, 231, 231, 230, 230, 229, 228, 228, 227, 226, 226, 225,
 224, 224, 223, 222, 222, 221, 220, 220, 219, 219, 218, 217, 217, 216, 215, 215,
 214, 213, 213, 212, 211, 211, 210, 209, 209, 208, 207, 207, 206, 205, 205, 204,
 203, 203, 202, 201, 201, 200, 199, 199, 198, 197, 197, 196, 195, 195, 194, 193,
 193, 192, 191, 191, 190, 189, 188, 188, 187, 186, 186, 185, 184, 184, 183, 182,
 181, 181, 180, 179, 179, 178, 177, 177, 176, 175, 174, 174, 173, 172, 171, 171,
 170, 169, 169, 168, 167, 166, 166, 165, 164, 163, 163, 162, 161, 160, 160, 159,
 158, 157, 156, 156, 155, 154, 153, 153, 152, 151, 150, 149, 149, 148, 147, 146,
 145, 145, 144, 143, 142, 141, 140, 140, 139, 138, 137, 136, 135, 135, 134, 133,
 132, 131, 130, 129, 128, 128, 127, 126, 125, 124, 123, 122, 121, 120, 119, 118,
 117, 116, 115, 114, 113, 112, 111, 110, 109, 108, 107, 106, 105, 104, 103, 102,
 101, 100,  99,  98,  97,  95,  94,  93,  92,  91,  90,  88,  87,  86,  85,  83,
  82,  81,  79,  78,  77,  75,  74,  72,  71,  69,  68,  66,  65,  63,  61,  59,
  58,  56,  54,  52,  50,  48,  46,  43,  41,  38,  35,  32,  29,  25,  20,  14 };
/*-----------------------------------------------------------*/

// asinf, normalised 255 = 90 degrees
// 256 values in table 0 to 255, 256 = 90
unsigned char const asinTable[256] = {
   0,   1,   1,   2,   3,   3,   4,   4,   5,   6,   6,   7,   8,   8,   9,  10,
  10,  11,  11,  12,  13,  13,  14,  15,  15,  16,  17,  17,  18,  18,  19,  20,
  20,  21,  22,  22,  23,  24,  24,  25,  25,  26,  27,  27,  28,  29,  29,  30,
  31,  31,  32,  33,  33,  34,  35,  35,  36,  36,  37,  38,  38,  39,  40,  40,
  41,  42,  42,  43,  44,  44,  45,  46,  46,  47,  48,  48,  49,  50,  50,  51,
  52,  52,  53,  54,  54,  55,  56,  56,  57,  58,  58,  59,  60,  60,  61,  62,
  62,  63,  64,  64,  65,  66,  67,  67,  68,  69,  69,  70,  71,  71,  72,  73,
  74,  74,  75,  76,  76,  77,  78,  78,  79,  80,  81,  81,  82,  83,  84,  84,
  85,  86,  86,  87,  88,  89,  89,  90,  91,  92,  92,  93,  94,  95,  95,  96,
  97,  98,  99,  99, 100, 101, 102, 102, 103, 104, 105, 106, 106, 107, 108, 109,
 110, 110, 111, 112, 113, 114, 115, 115, 116, 117, 118, 119, 120, 120, 121, 122,
 123, 124, 125, 126, 127, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137,
 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153,
 154, 155, 156, 157, 158, 160, 161, 162, 163, 164, 165, 167, 168, 169, 170, 172,
 173, 174, 176, 177, 178, 180, 181, 183, 184, 186, 187, 189, 190, 192, 194, 196,
 197, 199, 201, 203, 205, 207, 209, 212, 214, 217, 220, 223, 226, 230, 235, 241 };
/*-----------------------------------------------------------*/

// atan table
// 256 values in table 0 to 255 - 256 = 45
unsigned char const atanTable[256] = {
   0,   1,   3,   4,   5,   6,   8,   9,  10,  11,  13,  14,  15,  17,  18,  19,
  20,  22,  23,  24,  25,  27,  28,  29,  30,  32,  33,  34,  36,  37,  38,  39,
  41,  42,  43,  44,  46,  47,  48,  49,  51,  52,  53,  54,  55,  57,  58,  59,
  60,  62,  63,  64,  65,  67,  68,  69,  70,  71,  73,  74,  75,  76,  77,  79,
  80,  81,  82,  83,  85,  86,  87,  88,  89,  91,  92,  93,  94,  95,  96,  98,
  99, 100, 101, 102, 103, 104, 106, 107, 108, 109, 110, 111, 112, 114, 115, 116,
 117, 118, 119, 120, 121, 122, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133,
 134, 135, 136, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150,
 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166,
 167, 168, 169, 170, 171, 172, 173, 174, 175, 175, 176, 177, 178, 179, 180, 181,
 182, 183, 184, 185, 186, 186, 187, 188, 189, 190, 191, 192, 193, 194, 194, 195,
 196, 197, 198, 199, 200, 200, 201, 202, 203, 204, 205, 205, 206, 207, 208, 209,
 210, 210, 211, 212, 213, 214, 214, 215, 216, 217, 218, 218, 219, 220, 221, 221,
 222, 223, 224, 224, 225, 226, 227, 227, 228, 229, 230, 230, 231, 232, 233, 233,
 234, 235, 235, 236, 237, 238, 238, 239, 240, 240, 241, 242, 242, 243, 244, 244,
 245, 246, 247, 247, 248, 249, 249, 250, 250, 251, 252, 252, 253, 254, 254, 255 };
/*-----------------------------------------------------------*/

// square root table
// 256 values in table 0 to 255
unsigned char const sqrtTable[256] = {
   0,  16,  22,  27,  32,  35,  39,  42,  45,  48,  50,  53,  55,  57,  59,  61,
  64,  65,  67,  69,  71,  73,  75,  76,  78,  80,  81,  83,  84,  86,  87,  89,
  90,  91,  93,  94,  96,  97,  98,  99, 101, 102, 103, 104, 106, 107, 108, 109,
 110, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126,
 128, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142,
 143, 144, 144, 145, 146, 147, 148, 149, 150, 150, 151, 152, 153, 154, 155, 155,
 156, 157, 158, 159, 160, 160, 161, 162, 163, 163, 164, 165, 166, 167, 167, 168,
 169, 170, 170, 171, 172, 173, 173, 174, 175, 176, 176, 177, 178, 178, 179, 180,
 181, 181, 182, 183, 183, 184, 185, 185, 186, 187, 187, 188, 189, 189, 190, 191,
 192, 192, 193, 193, 194, 195, 195, 196, 197, 197, 198, 199, 199, 200, 201, 201,
 202, 203, 203, 204, 204, 205, 206, 206, 207, 208, 208, 209, 209, 210, 211, 211,
 212, 212, 213, 214, 214, 215, 215, 216, 217, 217, 218, 218, 219, 219, 220, 221,
 221, 222, 222, 223, 224, 224, 225, 225, 226, 226, 227, 227, 228, 229, 229, 230,
 230, 231, 231, 232, 232, 233, 234, 234, 235, 235, 236, 236, 237, 237, 238, 238,
 239, 240, 240, 241, 241, 242, 242, 243, 243, 244, 244, 245, 245, 246, 246, 247,
 247, 248, 248, 249, 249, 250, 250, 251, 251, 252, 252, 253, 253, 254, 254, 255 };
/*-----------------------------------------------------------*/

// zero initialized
unsigned short int AtoDbufferDMA[MAX_VOLT_IP];

/*struct SmoothBuffer {
	unsigned short int buffer[MAX_BUFFERING_ADC];
	unsigned short int smooth;
	unsigned char ptr;
	};*/
struct SmoothBuffer AtoDmux[MAX_VOLT_IP];
union LongToChar AtoDhp[4];

unsigned char buffer[GRAPHICS_SIZE+128];
unsigned char adm[GRAPHICS_SIZE];
unsigned char startup, selection, escape, leaf, windowdepth, nextwindow;
unsigned char deftime, reverttime, looptime,offtime, half_second, module_id, module_selection;

void (*main_window)( unsigned int );
void (*pin_window)( unsigned int );
void (*default_window)( unsigned int );

struct sdPreviousWindow PreviousWindow[MAX_WINDOWDEPTH];

unsigned int key;
unsigned char savecal, editing, okset, valuechanged, number[16];
unsigned char SavecalDelayWithinMenus;
unsigned char SavecalDelayOnMainDisplay;

unsigned char buzz_count;
unsigned int buzz_length, off_length;

unsigned short active_pin;

unsigned char COM0_message_received, COM1_message_received;
unsigned char COM0_message[COM_MAXRX], COM1_message[COM_MAXRX];

unsigned short int clock_display_time;

unsigned short int ExtIn;

struct CAN_Status CanStat[CAN_MODULE_TYPES][8]; // [type][id]
struct CAN_MCM MCM[8];
struct CAN_ACM ACM[8];
struct CAN_HBM HBM[8];
struct CAN_WCM WCM[8];
struct CAN_DOP DOP[8];
struct CAN_DIP DIP[8];
struct CAN_LMM LMM[8];
struct CAN_IOM IOM[8];
struct CAN_IOC IOC[8];
struct CAN_IOB IOB[8];


/*McHale variables*/

unsigned long int BaleDiaVoltsTotAccumMultSamples;     // V
unsigned long int DensityPresVoltsTotAccumMultSamples; // V
unsigned long int BaleShapeVoltsTotAccumMultSamples;   // V
unsigned long int KnifePresVoltsTotAccumMultSamples;   // V

unsigned long int BeakEngagePulseCount; // Pulses
unsigned long int NetPulseCountPrev; // Pulses
unsigned long int NewNetPulses; // Pulses
unsigned long int NetMetresPulseCount; // Pulses
float NetMetresAppliedForCount; // m
float NetPulseFrequency; // Hz
float NetPulsesPerLayer; // Pulses/layer
unsigned short int TargetNetPulses; // Pulses

// language string pointer
char const * const *ptText;

// Programmable Factors (Array)

// Machine Menu                                                 Step size,      Minimum,      Maximum,      Default
unsigned char const PF_GW302_BaleProfile[PF_DIM]            = {         1,  BP_PRESET_A,  BP_PRESET_E,  BP_PRESET_A }; // Preset A to E
unsigned char const PF_GW302_BaleDiameter[PF_DIM]           = {         1,           60,          168,       PF_cNA }; // cm     -- Preset Default (char) -- Minimum Bale Diameter is limited to Current Core Diameter
unsigned char const PF_GW302_CoreDiameter[PF_DIM]           = {         1,           60,          130,       PF_cNA }; // cm     -- Preset Default (char)
unsigned char const PF_GW302_BaleDensity[PF_DIM]            = {         1,            1,           10,       PF_cNA }; // ID     -- Preset Default (char)
unsigned char const PF_GW302_CoreDensity[PF_DIM]            = {         1,            1,           10,       PF_cNA }; // ID     -- Preset Default (char)
float const         PF_GW302_NetLayers[PF_DIM]              = {       0.1,          1.5,          6.0,       PF_fNA }; // layers -- Preset Default (float)
unsigned long int const PF_GW302_BaleTotal[PF_DIM]          = {      1UL,          0UL,        9999UL,          0UL }; // Bales  -- Bale Totals (in Bale Stores A to J)
unsigned char const PF_GW302_BaleStore[PF_DIM]              = {        1,            0,  MAX_STORES-1,            0 }; // Bale Store ID: A - J
// LubeCounter      // Moved to preserved run-time totals   = PF_GW302_LubeAlarm[PF_DIM] for range of values with the exception that minimum can go negative
unsigned long int const PF_GW302_GrandBaleTotal[PF_DIM]     = {       1UL,          0UL,     999999UL,          0UL }; // Bales  -- Moved to early position in memory since value is to be preserved between successive software versions
float const         PF_GW302_DiaPotZero[PF_DIM]             = {    PF_fNA,       PF_fNA,       PF_fNA,          1.0 }; // V      -- Value fixed -- 'zero' bale diameter voltage when baler door closed & no bale in chamber
signed char const   PF_GW302_DiameterCorrection[PF_DIM]     = {         1,          -20,          +20,            0 }; // cm
unsigned char const PF_GW302_BaleShapeIndicator[PF_DIM]     = {         1,      BSI_OFF,     BSI_BEEP,      BSI_OFF }; // Off/On/Beep
float const         PF_GW302_BaleShapeIndicatorZero[PF_DIM] = {    PF_fNA,          0.0,          9.0,          2.5 }; // V      -- 'zero' deviation from uniform cylinder shape
float const         PF_GW302_TotalNetMetres[PF_DIM]         = {    PF_fNA,          0.0,       9999.0,          0.0 }; // m      -- Incremented by NetMetresAppliedForCount; Display range = 0 - 9999 in increments of 1
unsigned char const PF_GW302_PreWarningBeep[PF_DIM]         = {         1,            1,          100,           96 }; // %
float const         PF_GW302_NetDelay[PF_DIM]               = {       0.1,          0.0,          5.0,          2.0 }; // s
unsigned char const PF_GW302_NetTwineOption[PF_DIM]         = {         1,       TO_NET,     TO_TWINE,        TO_NET}; // Off/On/Beep
float const         PF_GW302_TwineDelay[PF_DIM]               = {       0.1,          0.0,          9.9,          0.0 }; // s


// Technician Menu - Baler Full Page 1/2                        Step size,   Minimum,   Maximum,   Default
unsigned short int const PF_GW302_LubeAlarm[PF_DIM]         = {         1,         0,       999,       300 }; // Bales
float const              PF_GW302_PresDeadband[PF_DIM]      = {      0.01,      0.00,      9.99,      0.00 }; // V     -- Deadband on AnIP2
float const              PF_GW302_NetSpeed[PF_DIM]          = {       0.1,       0.0,      99.0,      50.0 }; // Hz    -- OP2 switches off if frequency on IP4 reaches this setting
float const              PF_GW302_PresSwDelay[PF_DIM]       = {       0.1,       0.0,       9.9,       0.2 }; // s     -- Delay between target signal on AnIP2 and OP8 reacting
float const              PF_GW302_DoorClosed[PF_DIM]        = {       0.1,       0.0,       9.9,       1.5 }; // s     -- Time IP3 must be closed for once the bale has been ejected
unsigned char const      PF_GW302_BaleSizeFactor[PF_DIM]    = {         1,       100,       250,       221 }; //       -- Factor to determine the maximum diameter voltage

// Technician Menu - Baler Full Page 2/2                  Step size,   Minimum,   Maximum,   Default
float const         PF_GW302_FillPotRange[PF_DIM]     = {      0.01,      0.00,      9.99,      1.00 }; // V          -- Range of AnIP3
float const         PF_GW302_FillPotIgnore[PF_DIM]    = {      0.01,      0.00,      9.99,      0.25 }; // V          -- Ignore range of AnIP3
unsigned char const PF_GW302_EngageDiameter[PF_DIM]   = {         1,         0,       100,        95 }; // %          -- % of target bale diameter when OP5 switches on
unsigned char const PF_GW302_BaleIndicator[PF_DIM]    = {         1,  BI_STEER,   BI_FILL,  BI_STEER }; // Steer/Fill -- Inverts the direction of the driving arrows on the main screen
float const         PF_GW302_RollerDelay[PF_DIM]      = {       0.1,       0.0,       9.9,       0.5 }; // s          -- Delay between OP5 and OP2 switching on if 'Net System' is set to 'Roller'
unsigned char const PF_GW302_DiagnosticMode[PF_DIM]      = {         1,   TO_OFF,  	TO_ON, 	  TO_OFF }; //
float const        	PF_GW302_EngageTime[PF_DIM]       = {       0.1,       0.0,       9.9,       3.0 }; // s     -- Delay for how long the OP5 should be on to engage the roller

// Technician Menu - Netting Setup Page 1/2               Step size,   Minimum,   Maximum,   Default
float const         PF_GW302_NetOn[PF_DIM]            = {       0.1,       0.0,       9.9,       3.0 }; // s            -- Duration that OP2 switches on for (t2)
unsigned char const PF_GW302_NetCutPulse[PF_DIM]      = {         1,         0,        99,        40 }; // pulses       -- Used in the calculation of the target net pulses
float const         PF_GW302_NetCutTime[PF_DIM]       = {       0.1,       0.0,       9.9,       6.0 }; // s            -- Duration that OP7 switches on for to cut the net (t5)
float const         PF_GW302_MinDiaPulsPerLay[PF_DIM] = {       0.1,       0.0,     999.9,      38.0 }; // pulses/layer -- Net pulses per layer at minimum diameter setting
float const         PF_GW302_MaxDiaPulsPerLay[PF_DIM] = {       0.1,       0.0,     999.9,     108.0 }; // pulses/layer -- Net pulses per layer at maximum diameter setting
float const         PF_GW302_NetFactor[PF_DIM]        = {       0.1,       0.0,       9.9,       1.0 }; //              -- Used in the calculation of the target net pulses
float const         PF_GW302_NoNetPulse[PF_DIM]       = {       0.1,       0.0,       9.9,       4.0 }; // s            -- No net warning appears if there are no pulses on IP4 within this time
unsigned char const PF_GW302_PreNetPulse[PF_DIM]      = {         1,         0,        99,         2 }; // pulses       -- Target pulses on IP4 to pre-feed the net (n2)

// Technician Menu - Netting Setup Page 2/2               Step size,   Minimum,   Maximum,   Default
unsigned char const PF_GW302_NetSystem[PF_DIM]        = {         1,   NS_BEAK, NS_ROLLER, NS_ROLLER }; // Beak/Roller -- Type of netting system fitted to the machine
unsigned char const PF_GW302_BeakEngage[PF_DIM]       = {         1,         0,        99,         6 }; // pulses      -- Number of pulses on IP4 before OP5 switches on (only if Net System is set to Beak)



// Technician Menu - Twine Setup Page 1/2               Step size,   Minimum,   Maximum,   Default
unsigned char const PF_GW302_TwineOption[PF_DIM]      = {         1,   TO_OFF,  	TO_ON, 	  TO_ON }; // Twine Option-- Type of netting system fitted to the machine
unsigned char const PF_GW302_TwineFeedDiameter[PF_DIM]   = {         1,         0,       100,        100 }; // %
float const         PF_GW302_TwineFeedTime[PF_DIM]       = {       0.5,       1.0,       60.0,       15.0 }; // s
float const         PF_GW302_TwineFeedSpeed[PF_DIM]       = {       0.1,       0.1,       200.0,       17.0 }; // Hz
float const         PF_GW302_TwineTrackDelay[PF_DIM]       = {       0.5,       0.0,       60.0,       0.0 }; // s
float const         PF_GW302_TwineLenghtPulse[PF_DIM]       = {       0.1,       0.1,       200.0,       47.1 }; // mm
float const         PF_GW302_TrackDistancePulse[PF_DIM]       = {       0.01,       0.01,       9.99,       1.63 }; // mm
unsigned short int const         PF_GW302_TrackMaxFwd[PF_DIM]       = {       1,       1,       600,       383 }; // mm

// Technician Menu - Twine Setup Page 2/2               Step size,   Minimum,   Maximum,   Default
unsigned short int const         PF_GW302_TrackMaxRev[PF_DIM]       = {       1,       1,       600,       102 }; // mm
unsigned short int const         PF_GW302_TwineMinimumEdgeDistance[PF_DIM]       = {       10,       10,       600,       80 }; // mm
unsigned short int const         PF_GW302_TwineMinimumCentreDistance[PF_DIM]       = {       1,       1,       600,       40 }; // mm

unsigned short int const         PF_GW302_TwineTrackProtection[PF_DIM]       = {       1,       1,       100,       10 }; // ms

unsigned char         PF_GW302_PreTwinePulses[PF_DIM]       = {       1,       0,       50,       6 }; // pulses

unsigned char         PF_GW302_PreTwineDiam[PF_DIM]       = {       1,       0,       100,       10 }; // %
unsigned char const PF_GW302_TwineEngageDiameter[PF_DIM]   = {         1,         0,       100,        98 }; // %          -- % of target bale diameter when OP5 switches on
unsigned short int const         PF_GW302_PreTwineRev[PF_DIM]       = {       1,      0,       600,       0 }; // mm
unsigned short int const         PF_GW302_PreTwine2Rev[PF_DIM]       = {       1,      0,       50,       4 }; // mm


// Technician Menu - Diameter Setup                                 Step size,   Minimum,   Maximum,   Default
unsigned char const PF_GW302_DiameterMin[PF_DIM]                = {         1,         0,       100,         8 }; // % -- Target diameter at setting 1
unsigned char const PF_GW302_DiameterMax[PF_DIM]                = {         1,         0,       100,       100 }; // % -- Target diameter at setting 109
float const         PF_GW302_DiaCorrection[PF_DIM]              = {      0.01,      0.00,      0.20,      0.01 }; // V -- Amount the target diameter voltage changes for each 'Diameter correction' step
unsigned char const PF_GW302_BaleDiaSmoothingFactor[PF_DIM]     = {         1,         1,       100,        50 }; // % -- Bale Diameter (AnIP1) Smoothing Factor
unsigned char const PF_GW302_DensityPresSmoothingFactor[PF_DIM] = {         1,         1,       100,       100 }; // % -- (Bale) Density Pressure (AnIP2) Smoothing Factor
unsigned char const PF_GW302_BaleShapeSmoothingFactor[PF_DIM]   = {         1,         1,       100,        50 }; // % -- Bale Shape Indicator (AnIP3) Smoothing Factor
unsigned char const PF_GW302_KnifePresSmoothingFactor[PF_DIM]   = {         1,         1,       100,        50 }; // % -- Knife Pressure (AnIP4) Smoothing Factor

// Technician Menu - Density Setup Page 1/2    Step size,   Minimum,   Maximum,   Default
float const PF_GW302_Density1[PF_DIM]      = {      0.01,      0.00,     10.00,      0.00 }; // V -- Target voltage on AnIP2 if target density is 1 -- Equivalent Pressure:   0 Bar
float const PF_GW302_Density2[PF_DIM]      = {      0.01,      0.00,     10.00,      0.80 }; // V -- Target voltage on AnIP2 if target density is 2 -- Equivalent Pressure:  40 Bar
float const PF_GW302_Density3[PF_DIM]      = {      0.01,      0.00,     10.00,      1.20 }; // V -- Target voltage on AnIP2 if target density is 3 -- Equivalent Pressure:  60 Bar
float const PF_GW302_Density4[PF_DIM]      = {      0.01,      0.00,     10.00,      1.60 }; // V -- Target voltage on AnIP2 if target density is 4 -- Equivalent Pressure:  80 Bar
float const PF_GW302_Density5[PF_DIM]      = {      0.01,      0.00,     10.00,      2.00 }; // V -- Target voltage on AnIP2 if target density is 5 -- Equivalent Pressure: 100 Bar
float const PF_GW302_Density6[PF_DIM]      = {      0.01,      0.00,     10.00,      2.40 }; // V -- Target voltage on AnIP2 if target density is 6 -- Equivalent Pressure: 120 Bar

// Technician Menu - Density Setup Page 2/2    Step size,   Minimum,   Maximum,   Default
float const PF_GW302_Density7[PF_DIM]      = {      0.01,      0.00,     10.00,      2.80 }; // V -- Target voltage on AnIP2 if target density is 7  -- Equivalent Pressure: 140 Bar
float const PF_GW302_Density8[PF_DIM]      = {      0.01,      0.00,     10.00,      3.20 }; // V -- Target voltage on AnIP2 if target density is 8  -- Equivalent Pressure: 160 Bar
float const PF_GW302_Density9[PF_DIM]      = {      0.01,      0.00,     10.00,      3.60 }; // V -- Target voltage on AnIP2 if target density is 9  -- Equivalent Pressure: 180 Bar
float const PF_GW302_Density10[PF_DIM]     = {      0.01,      0.00,     10.00,      4.00 }; // V -- Target voltage on AnIP2 if target density is 10 -- Equivalent Pressure: 200 Bar

// Technician Menu - Knives Setup              Step size,   Minimum,   Maximum,   Default
float const PF_GW302_KnifePressure[PF_DIM] = {      0.01,      0.00,      9.99,      0.90 }; // V
float const PF_GW302_KnifePresWarn[PF_DIM] = {      0.01,      0.00,      9.99,      1.30 }; // V -- Knife pressure warning appears if AnIP4 reaches this value
float const PF_GW302_DiverterDelay[PF_DIM] = {       0.1,       0.0,       9.9,       0.2 }; // s -- Delay to expire while key pressed before turning OP on

// Technician Menu - Formation 2                            Step size,   Minimum,   Maximum,   Default
unsigned char const PF_GW302_FormationDensity[PF_DIM]   = {         1,         1,        10,    PF_cNA }; // ID -- Preset Default (char) -- Target density during the 'Formation' stage
unsigned char const PF_GW302_FormationDiameter[PF_DIM]  = {         1,         0,       100,    PF_cNA }; // %  -- Preset Default (char) -- Upper diameter limit of 'Formation' stage
unsigned char const PF_GW302_Formation2Density[PF_DIM]  = {         1,         1,        10,    PF_cNA }; // ID -- Preset Default (char) -- Target density during the 'Formation2' stage
unsigned char const PF_GW302_Formation2Diameter[PF_DIM] = {         1,         0,       100,    PF_cNA }; // %  -- Preset Default (char) -- Upper diameter limit of 'Formation2' stage

// Diagnostics - Digital Inputs                               Step size,      Minimum,     Maximum,      Default
unsigned char const PF_GW302_ChoppingKnivesFitted[PF_DIM] = {         1, CKF_DISABLED, CKF_ENABLED, CKF_DISABLED }; // Disabled/Enabled

/*Twine Factors*/
unsigned char const PF_TwineNumberOfRevAroundEdge[PF_DIM]   = {         1,         1,        10,    4 }; // ID -- Preset Default (char)
unsigned char const PF_TwineDistanceFromEdge[PF_DIM]   = {         1,         4,        22,    4 };
unsigned char const PF_TwineNumberOfStopsBetweenEdgeAndCentre[PF_DIM] = {         1,         1,        10,    3 };
float const PF_TwineStopTime[PF_DIM] = {0.1,      0.1,     30.0,      1.0};
unsigned char const PF_TwineNumberOfRevolutionsAroundCentre[PF_DIM] = {         1,         1,        10,    4 };
unsigned char const PF_DistanceFromCentre[PF_DIM] = {         1,         4,        10,    4 };


float const PF_NoTwineWarningTime[PF_DIM] = {0.1,      0.1,     10.0,      5.0};

// Bale Profile defaults                             Preset =     A    B    C    D    E
unsigned char const PresetCoreDiameter[BP_PRESET_MAX]       = {  90,  90,  90,  90,  90 }; // Core Diameter [cm]
unsigned char const PresetCoreDensity[BP_PRESET_MAX]        = {   9,   8,   7,   3,   1 }; // Core Density [ID]
unsigned char const PresetBaleDiameter[BP_PRESET_MAX]       = { 125, 125, 125, 125, 125 }; // Bale Diameter [cm]
unsigned char const PresetBaleDensity[BP_PRESET_MAX]        = {  10,   9,   8,   4,   2 }; // Bale Density [ID]
unsigned char const PresetFormationDiameter[BP_PRESET_MAX]  = {   0,   0,   0,   0,   0 }; // Formation Diameter [%]
unsigned char const PresetFormationDensity[BP_PRESET_MAX]   = {  10,  10,  10,   5,   5 }; // Formation Density [ID]
unsigned char const PresetFormation2Diameter[BP_PRESET_MAX] = {   0,   0,   0,   0,   0 }; // Formation 2 Diameter [%]
unsigned char const PresetFormation2Density[BP_PRESET_MAX]  = {  10,  10,  10,   5,   5 }; // Formation 2 Density [ID]
float const PresetNetLayers[BP_PRESET_MAX]                  = { 3.5, 3.0, 2.5, 4.2, 4.2 }; // Net Layers [layers]


// Bale/Core Density ID     =    1    2    3    4    5    6    7    8    9    10
unsigned char const Bar[10] = {  0,  40,  60,  80, 100, 120, 140, 160, 180,  200 };

unsigned char lastoutput, nextoutput, OKWarnBox;
unsigned int SeqOpOn, SeqOpOff, KeyOpOn, KeyOpOff, OpOff, OpOn;
unsigned char DisableChoppingKnivesWarnings;
unsigned char Kniv1_IPClosed, AutoMan;
unsigned char ContinueOnKeyRelease;
unsigned char StartAutoNet;

unsigned short int MWarn;

unsigned char Stage, DelayTime, BeepDelay;
unsigned char stagecomp, knivesdelay, cont_beep;
unsigned char target_bale_dia_reached_cont_beep;
unsigned char pre_warning_cont_beep;
unsigned char start_chamber_open_delay_cont_beep;
unsigned char end_chamber_open_delay_cont_beep;

float DiaTargetVolts;    // V

float DiaMinVolts;       // V
float DiaMaxVolts;       // V

float DiaMinExtDisplayPercent;    // 0.0 - 100.0 % of extended display range from PotZero to DiaMax
float DiaTargetExtDisplayPercent; // 0.0 - 100.0 % of extended display range from PotZero to DiaMax
float BaleDiaExtDisplayPercent;   // 0.0 - 100.0 % of extended display range from PotZero to DiaMax

float DiaFormationVolts;                  // V
float DiaFormation2Volts;                 // V
float DiaCoreVolts;                       // V
float DiaEngageVolts;                     // V
float TwineDiaEngageVolts;			// V
float DiaFeedVolts;                     // V
float DiaPreWarnVolts;                    // V

float DensityBaleVolts;                   // V
float DensityCoreVolts;                   // V
float DensityFormation2Volts;             // V
float DensityFormationVolts;              // V

unsigned char DensityPressureTooHigh;     // 0 / 1


unsigned char SequenceInProgress;

unsigned char OP2OnDelayTime, OP2OffDelayTime;
unsigned char OP7OffDelayTime;
unsigned char OP8OnDelayTime, OP8OffDelayTime;
unsigned char NetWarningDelayTime;

unsigned char NetWarningOn;  // Check Net Error (AKA Net Warning) On/Off switch

unsigned char RollEngageTurnedOn; // OP5 Roller Engage Solenoid (AKA Pre Net Position) turned on
unsigned char SavedRollEngageTurnedOn; // Saved when IP3 opens in AUTO mode
unsigned char NetWarningDisabledWhileIP3OpenInAutoMode;
unsigned char NettingStartBeepsDisabledWhileIP3OpenInAutoMode; // Including Pre Warning & Net Delay beep
unsigned char RollerTimer = 255;      // OP5 Roller Engage Solenoid (AKA Pre Net Position) turned on for 3 seconds

unsigned char RollEngageTurnedOnInManMode; // OP5 Roller Engage Solenoid (AKA Pre Net Position) turned on in MAN mode

unsigned char ChoppingKnives1WarningOn; // Chopping knives position 1 Warning On/Off switch

unsigned char ChamberClosedDelayTime; // Door Closed

unsigned char BalerFullDelayTime;     // applies to MAN mode only -- Set to Net Delay

unsigned char ChamberOpenDelayTime;   // applies to MAN mode only

unsigned char ChamberOpenBeepDelay;   // applies to AUTO mode only

unsigned char BaleKickerStatus;                // Bale Kicker Down/Up flags
unsigned char BaleDiameterStatusInManMode;     // Bale Diameter Below/Above Engage Diameter (in MAN mode) flags
unsigned char ChamberStatus;                   // Chamber Closed/Open flags

unsigned char OP8OnCount;             // Count for Density Release Solenoid (OP8) on after Chamber (IP3) closes then opens (MAN/AUTO)

unsigned char IP4PulsesStoppedCount;        // Count for Net Pulses (on IP4) to stop after Target Pulses reached for Net Cut (OP7) to be turned off
unsigned long int NetPulseCountAtLastCheck; // Pulses -- Similar NetPulseCountPrev to but used in auto sequence loop

unsigned char DrivingArrowBeepsDisabled;    // Driving Arrow Beeps turned off/on

unsigned char DensityControlFunctionActive;                 // Density Control Function (based on Target Bale Density) active
unsigned char NetPulseCheckingEnabled;                      // Checking for no pulses on IP4 (for NetWarningDelayTime) enabled
unsigned char OP5TurnedOnAfterBeakEngagePulsesEnabled;      // Turning on Roller Engage Solenoid (OP5) after 'Beak Engage' pulses counted on IP4
unsigned char SavedOP5TurnedOnAfterBeakEngagePulsesEnabled; // Saved when IP3 opens in AUTO mode
unsigned long int NetPulseCount; // Pulses
unsigned char PreTwineSeq, PreTwineSeqActive;

float PreTwineVoltage;

float BaleDiaVolts;
float DensityPresVolts;
float BaleShapeVolts;
float KnifePresVolts;

unsigned char swrel;
unsigned char const Store[10] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };

struct PinWin const PinOption = {
       PINTECH1, screen141
};

float SupplyVolts;
/*struct PWMsettings {
	unsigned char enable;
	unsigned char status;
	unsigned int freq;
	unsigned int duty;
	};

 // runtime variables for serial flash save held in this structure
struct PreservedRuntime {
	// general runtime
	unsigned char Brightness;
	unsigned char Power;
	unsigned char Tempcomp;
	unsigned char Bias;
	unsigned char Gain;
	unsigned char GrayScale;
	unsigned char Contrast;
	unsigned char LineRate;
	unsigned char PixelMask;

	unsigned char PipsOn;
	unsigned char BuzzerTest;

	unsigned char memory_status;

	unsigned short int PowerCount;

	struct PWMsettings PWMout[PWM_OUTMAX];

	unsigned long int CheckSum __attribute__((aligned(4)));
	};*/
struct PreservedRuntimePR pRun;
#ifndef WINDOWS
 _Static_assert( sizeof( struct PreservedRuntimePR ) <= 4096, "pRun structure > 4096 bytes, reduce LARGEARRAYRUN value" );
#endif

// factors for serial flash save held in this structure
/*struct PreservedFactors {
  // general factors
  unsigned short int software_type;
  unsigned short int software_issue;
  unsigned short int software_revision;
	
  unsigned char Lang;
  
  unsigned long int COM0Baud; 
  unsigned long int COM1Baud; 
  
  unsigned long int CheckSum __attribute__((aligned(4)));
  };*/
struct PreservedFactorsPF pFac;
#ifndef WINDOWS
 _Static_assert( sizeof( struct  PreservedFactorsPF ) <= 4096, "pFac structure > 4096 bytes, reduce LARGEARRAYFAC value" );
#endif
/*-----------------------------------------------------------*/

// functions
/*-----------------------------------------------------------
 * Function Name  : StartAtoD
 * Description    : enable AtoD and start conversion
 * Input          : None
 * Output         : None
 * Return         : None
 * Note				:
 *-----------------------------------------------------------*/
void StartAtoD( void )
{
#ifndef WINDOWS
	ADC_InitTypeDef  ADC_InitStructure;
	ADC_CommonInitTypeDef  ADC_CommonInitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	ADC_CommonStructInit( &ADC_CommonInitStructure );
	// ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent; // Dual/Triple
	// ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2; // _Div2/4/6/8
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; // _Div2/4/6/8
	// ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; // _1/2/3
	// ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; // 5-20
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_12Cycles; // 5-20
	ADC_CommonInit( &ADC_CommonInitStructure );

	//ADC_TempSensorVrefintCmd( ENABLE );
	//ADC_VBATCmd( ENABLE );

	ADC_StructInit( &ADC_InitStructure );
	// ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; //_10b/8b/6b
	// ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	// ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	// ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // _Rizing/Falling/Both
	// ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1; // 0 option
	// ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // or _Left
	// ADC_InitStructure.ADC_NbrOfConversion = 1; // 1-16
	ADC_InitStructure.ADC_NbrOfConversion = 6;
	ADC_Init( ADC1, &ADC_InitStructure );

	// Rank - 1 to 16
	// ADC_SampleTime_3Cycles - 3/15/28/56/84/112/144/480
	ADC_RegularChannelConfig( ADC1, ADC_Channel_10, 1, ADC_SampleTime_480Cycles );
	ADC_RegularChannelConfig( ADC1, ADC_Channel_11, 2, ADC_SampleTime_480Cycles );
	ADC_RegularChannelConfig( ADC1, ADC_Channel_12, 3, ADC_SampleTime_480Cycles );
	ADC_RegularChannelConfig( ADC1, ADC_Channel_13, 4, ADC_SampleTime_480Cycles );
	ADC_RegularChannelConfig( ADC1, ADC_Channel_14, 5, ADC_SampleTime_480Cycles );
	ADC_RegularChannelConfig( ADC1, ADC_Channel_15, 6, ADC_SampleTime_480Cycles );

	DMA_StructInit( &DMA_InitStructure );
	// DMA_InitStructure.DMA_Channel = 0;
	// DMA_InitStructure.DMA_PeripheralBaseAddr = 0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned int)&ADC1->DR;
	// DMA_InitStructure.DMA_Memory0BaseAddr = 0;
	DMA_InitStructure.DMA_Memory0BaseAddr = (unsigned int)&AtoDbufferDMA;
	// DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	// DMA_InitStructure.DMA_BufferSize = 0;
	DMA_InitStructure.DMA_BufferSize = MAX_VOLT_IP;
	// DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	// DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //DMA_PeripheralDataSize_Word;
	// DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // DMA_MemoryDataSize_Word;
	// DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	// DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	// DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	// DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	// DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	// DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	// ADC1 is mapped to DMA2, Channel0, Stream0
	DMA_Init( DMA2_Stream0, &DMA_InitStructure );

	DMA_Cmd( DMA2_Stream0, ENABLE );

	ADC_DMARequestAfterLastTransferCmd( ADC1, ENABLE );
	ADC_DMACmd( ADC1, ENABLE );

	ADC_Cmd( ADC1, ENABLE );

	// start first conversions
	ADC_SoftwareStartConv( ADC1 );
#endif
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name  : CheckAnalogueLevels
 * Description    : check analogue thresholds analogue inputs
 * Input          : None
 * Output         : None
 * Return         : None
 * Note				:
 *-----------------------------------------------------------*/
void CheckAnalogueLevels( void )
{
	struct SmoothBuffer *ptADC;
	unsigned int total, count;
	unsigned int ix;

	ptADC = &AtoDmux[0];

	for( ix = 0; ix < MAX_VOLT_IP; ix++ )
	{
		// copy data
		ptADC->buffer[ptADC->ptr++] = AtoDbufferDMA[ix];
		if( ptADC->ptr >= MAX_BUFFERING_ADC )
		{
			ptADC->ptr = 0;
		}

		total = 0;
		for( count = 0; count < MAX_BUFFERING_ADC; count++ )
		{
			total += ptADC->buffer[count];
		}
		ptADC->smooth = total / count;

		ptADC++;
	}
#ifndef WINDOWS
	// restart conversions
	ADC_SoftwareStartConv( ADC1 );
#endif    
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name  : CheckInputs
 * Description    : read inputs
 * Input          : None
 * Output         : None
 * Return         : None
 * Note				:
 *-----------------------------------------------------------*/
void CheckInputs( void )
{
#ifndef WINDOWS
	struct structIoPins const *ptIO;
	unsigned int ix;
	unsigned short int us, mask;

	ptIO = &InputPin[0];

	us = 0;
	mask = 1;

	// get current input status
	for( ix = 0; ix < MAX_INPUTS; ix++ )
	{
		if( ptIO->GPIO->IDR & ptIO->Pin )
		{
			us |= mask;
		}

		mask <<= 1;

		ptIO++;
	}

	//us |= (1 << 2);
	ExtIn = us;
#endif    
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name  : CalculateFrequencies
 * Description    : calculates frequencies on capture inputs
 * Input          :
 * Output         :
 * Return         :
 * Note           :
 *-----------------------------------------------------------*/
void CalculateFrequencies( void )
{
#ifndef WINDOWS
	struct freqInput *ip;

	unsigned int ix;

	static unsigned short int count;

	// frequency
	for( ix = 0; ix < MAX_FREQUENCY_MEASUREMENTS; ix++ )
	{
		ip = &fIP[ix];

		ip->lockFreq = 1;

		if( ip->newFreq )
		{
			ip->newFreq = 0;

			if( ip->ftime && ip->fcount )
			{
				// all frequency timers are running with 1us counts
				ip->frequency = (float)ip->fcount * ((float)TIMER_CLOCK_FREQUENCY / 60.0) / (float)ip->ftime;

				ip->duty = (float)(ip->dtime * 1000 * 100) / (float)ip->ftime;




			}
			else
			{
				ip->duty = 0.0;
				ip->frequency = 0.0;
			}
		}

		ip->lockFreq = 0;
	}
#endif
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: SetupPWMTimers
 * Description		: set PWM output timers running
 * Input			: None
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void SetupPWMTimers( void )
{
#ifndef WINDOWS
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	//TIM_BDTRInitTypeDef TIM_BDTRInitStructure;

	/* SystemInit_GPIO() enables peripherals and timer clocks & maps io pins */


	// there is only one prescaler/counter for each of the 4 channels on each timer
	// so the frequency will be same on each group of channels

	// timers are clocked by 60MHz clock
	// prescaler is calculated to give maximum resolution to frequency reload value
	// ... (TIMER_CLOCK_FREQUENCY / prescale / freq) < 65536
	// ... prescale > (TIMER_CLOCK_FREQUENCY / 65536 / freq)


	// Timer configuration used for timer 3,4,9 & 10
	TIM_TimeBaseStructInit( &TIM_TimeBaseStructure );
	TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD;
	TIM_TimeBaseStructure.TIM_Prescaler = PWM_PRESCALE;
	//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;

	// configuration for timer 3
	TIM_TimeBaseInit( TIM3, &TIM_TimeBaseStructure );
	TIM_ARRPreloadConfig( TIM3, ENABLE );

	// configuration for timer 4
	TIM_TimeBaseInit( TIM4, &TIM_TimeBaseStructure );
	TIM_ARRPreloadConfig( TIM4, ENABLE );

	// configuration for timer 9
	TIM_TimeBaseInit( TIM9, &TIM_TimeBaseStructure );
	TIM_ARRPreloadConfig( TIM9, ENABLE );

	// configuration for timer 10
	TIM_TimeBaseInit( TIM10, &TIM_TimeBaseStructure );
	TIM_ARRPreloadConfig( TIM10, ENABLE );

	// channel configuration used for channel 1 to 4
	TIM_OCStructInit( &TIM_OCInitStructure );

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	//TIM_OCInitStructure.TIM_Pulse = 0x00000000;
	//TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
	//TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	//TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

	TIM_OC1Init( TIM3, &TIM_OCInitStructure );
    TIM_OC1PreloadConfig( TIM3, TIM_OCPreload_Enable );

	TIM_OC2Init( TIM3, &TIM_OCInitStructure );
    TIM_OC2PreloadConfig( TIM3, TIM_OCPreload_Enable );

    TIM_OC3Init( TIM3, &TIM_OCInitStructure );
    TIM_OC3PreloadConfig( TIM3, TIM_OCPreload_Enable );

    TIM_OC4Init( TIM3, &TIM_OCInitStructure );
    TIM_OC4PreloadConfig( TIM3, TIM_OCPreload_Enable );

    //TIM_GenerateEvent( TIM3, TIM_EventSource_Update );

	TIM_OC1Init( TIM4, &TIM_OCInitStructure );
    TIM_OC1PreloadConfig( TIM4, TIM_OCPreload_Enable );

	TIM_OC2Init( TIM4, &TIM_OCInitStructure );
    TIM_OC2PreloadConfig( TIM4, TIM_OCPreload_Enable );

	TIM_OC3Init( TIM4, &TIM_OCInitStructure );
    TIM_OC3PreloadConfig( TIM4, TIM_OCPreload_Enable );

	TIM_OC4Init( TIM4, &TIM_OCInitStructure );
    TIM_OC4PreloadConfig( TIM4, TIM_OCPreload_Enable );

	TIM_OC1Init( TIM9, &TIM_OCInitStructure );
    TIM_OC1PreloadConfig( TIM9, TIM_OCPreload_Enable );

	TIM_OC1Init( TIM10, &TIM_OCInitStructure );
    TIM_OC1PreloadConfig( TIM10, TIM_OCPreload_Enable );

    //TIM_GenerateEvent( TIM4, TIM_EventSource_Update );

    //TIM_BDTRStructInit( &TIM_BDTRInitStructure );
    //TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Disable;
    //TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Disable;
    //TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
    //TIM_BDTRInitStructure.TIM_DeadTime = 0x00;
    //TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
    //TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;
    //TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
    //TIM_BDTRConfig( TIMn, &TIM_BDTRInitStructure );

	// finally, enable timers
	TIM_Cmd( TIM3, ENABLE );
	TIM_Cmd( TIM4, ENABLE );
	TIM_Cmd( TIM9, ENABLE );
	TIM_Cmd( TIM10, ENABLE );
#endif
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: SetOutputPWM
 * Description		: set required output PWM frequency/duty
 * Input			: PWM output 1 - 8
 * 					: frequency, 0 = no change
 * 					: duty PWM_DUTY000 to PWM_DUTY100 = 0 to 100%
 * Output			: None
 * Return			: None
 * Note				: timers are in blocks of 4, sharing one frequency per block
 * 					: duty is copied to timer in SetOutputs()
 *-----------------------------------------------------------*/
void SetOutputPWM( enum pwmOutputs output, unsigned int freq, unsigned int duty )
{
#ifndef WINDOWS
	struct PWMregisters const *ptPWM;
	uint16_t cmr;

	// there is only one prescaler/counter for each of the 4 channels on each timer
	// so the frequency will be same on each group of channels

	// timers are clocked by 60MHz clock
	// prescaler is calculated to give maximum resolution to frequency reload value
	// ... (TIMER_CLOCK_FREQUENCY / prescale / freq) < 65536
	// ... prescale > (TIMER_CLOCK_FREQUENCY / 65536 / freq)

	if( output < MAX_PWM_OUTPUTS )
	{
		ptPWM = &PWMreg[output];

		if( freq > 0 )
		{
			ptPWM->timer->PSC = (unsigned short)((TIMER_CLOCK_FREQUENCY / 65536) / freq);
			ptPWM->timer->ARR = (unsigned short) (TIMER_CLOCK_FREQUENCY / (ptPWM->timer->PSC + 1) / freq);
		}

		*ptPWM->compare = (ptPWM->timer->ARR * duty) / PWM_DUTY100;

		cmr = *ptPWM->mode_reg & ~ptPWM->mode_bits;
		if( duty == PWM_DUTY000 )
		{
			cmr |= ptPWM->mOFF;
		}
		else if( duty == PWM_DUTY100 )
		{
			cmr |= ptPWM->mON;
		}
		else
		{
			cmr |= ptPWM->mPWM;
		}
		*ptPWM->mode_reg = cmr;

		// TIM_ForcedAction == TIM_ForcedAction_Active/TIM_ForcedAction_InActive
		// uint16_t tmpccmr1 = 0;
		// tmpccmr1 = TIMx->CCMR1;
		// tmpccmr1 &= (uint16_t)~TIM_CCMR1_OC1M;
		// tmpccmr1 |= TIM_ForcedAction;
		// TIMx->CCMR1 = tmpccmr1;
	}
#endif
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: SetOutputOUT
 * Description		: set or read required output
 * Input			: enum staticOutputs output
 * 					: enum staticOutputStatus status
 * 					:
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
enum staticOutputStatus SetOutputOUT( enum staticOutputs output, enum staticOutputStatus status )
{
#ifndef WINDOWS
	struct structIoPins const *ptOut;

	ptOut = &OutputPin[output];

	switch( status )
	{
	case OUT_STATUS:
		status = (ptOut->GPIO->ODR & ptOut->Pin) ? OUT_ON : OUT_OFF;
		break;

	case OUT_OFF:
		ptOut->GPIO->BSRRH = ptOut->Pin;
		break;

	case OUT_ON:
		ptOut->GPIO->BSRRL = ptOut->Pin;
		break;
	}
#endif
	return( status );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
unsigned long int CheckSumIt( unsigned char *data, unsigned char *end )
{
	unsigned int crc32;

#ifndef WINDOWS
	// turn on CRC hardware clock
	RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;

	// reset crc generator to 0xffffffff (-1)
	CRC->CR = CRC_CR_RESET;

	// sum multiples of 4
	while( data < end )
	{
		CRC->DR = *(unsigned int *)(unsigned int)data;
		data += 4;
	}

	// save crc before turning power off
	crc32 = CRC->DR;

	// turn off CRC hardware clock
	RCC->AHB1ENR &= ~RCC_AHB1ENR_CRCEN;
#else
	unsigned long int ul;

	for( ul = 0; data < end; data++ )
	{
		ul += *data;
	}

	ul = ~ul + 1;

	return( ul );
#endif
	return( crc32 );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
// Function Name       : SetDefaultFactors
// Object              : reset power on defaults & save to flash
// Output Parameters   :
/*-----------------------------------------------------------*/
void SetDefaultFactors( void )
{
	char c;

	ptText = TextString[pFac.pf.Lang];


	// set default Programmable Factors

	if( pFac.pf.software_type != SOFTWARE ) // Value preserved between successive software versions
		pFac.pf.GrandBaleTotal = PF_GW302_GrandBaleTotal[PF_DEF]; // bales

	pFac.pf.software_type = SOFTWARE;
	pFac.pf.software_issue = ISSUE;
	pFac.pf.software_revision = REVISION;

	pFac.pf.Lang = 0;                  // Language set to English

	for( c = 0; c < MAX_STORES; c++ )  // Bale Totals (in Bale Stores A to J) set to 0
		pFac.pf.BaleTotal[c] = PF_GW302_BaleTotal[PF_DEF]; // bales

	pFac.pf.BaleStore = PF_GW302_BaleStore[PF_DEF]; // Bale Store ID set to A

	// pFac.pf.BaudRate -- Not set/used

	// Machine Menu
	pFac.pf.GW302_BaleProfile = PF_GW302_BaleProfile[PF_DEF]; // Preset A to E
	for( c = 0; c < BP_PRESET_MAX; c++ )  // Bale Profile defaults set for Presets A to E
	{
		pFac.pf.GW302_BaleDiameter[c] =  PresetBaleDiameter[c]; // cm
		pFac.pf.GW302_CoreDiameter[c] =  PresetCoreDiameter[c]; // cm
		pFac.pf.GW302_BaleDensity[c]  =  PresetBaleDensity[c]; // ID
		pFac.pf.GW302_CoreDensity[c]  =  PresetCoreDensity[c]; // ID
		pFac.pf.GW302_NetLayers[c]    =  PresetNetLayers[c]; // layers
	}
	// LubeCounter -- Moved to preserved run-time totals
	// GrandBaleTotal -- Moved to early position in memory since value is to be preserved between successive software versions
	pFac.pf.GW302_DiaPotZero = PF_GW302_DiaPotZero[PF_DEF]; // V
	pFac.pf.GW302_DiameterCorrection = PF_GW302_DiameterCorrection[PF_DEF]; // cm
	pFac.pf.GW302_BaleShapeIndicator = PF_GW302_BaleShapeIndicator[PF_DEF]; // Off/On/Beep
	pFac.pf.GW302_BaleShapeIndicatorZero = PF_GW302_BaleShapeIndicatorZero[PF_DEF]; // V
	pFac.pf.GW302_TotalNetMetres = PF_GW302_TotalNetMetres[PF_DEF]; // m
	pFac.pf.GW302_PreWarningBeep = PF_GW302_PreWarningBeep[PF_DEF]; // %
	pFac.pf.GW302_NetDelay = PF_GW302_NetDelay[PF_DEF]; // s

	// Technician Menu - Baler Full
	pFac.pf.GW302_LubeAlarm = PF_GW302_LubeAlarm[PF_DEF]; // bales
	pFac.pf.GW302_PresDeadband = PF_GW302_PresDeadband[PF_DEF]; // V
	pFac.pf.GW302_NetSpeed = PF_GW302_NetSpeed[PF_DEF]; // Hz
	pFac.pf.GW302_PresSwDelay = PF_GW302_PresSwDelay[PF_DEF]; // s
	pFac.pf.GW302_DoorClosed = PF_GW302_DoorClosed[PF_DEF]; // s
	pFac.pf.GW302_BaleSizeFactor = PF_GW302_BaleSizeFactor[PF_DEF];
	pFac.pf.GW302_FillPotRange = PF_GW302_FillPotRange[PF_DEF]; // V
	pFac.pf.GW302_FillPotIgnore = PF_GW302_FillPotIgnore[PF_DEF]; // V
	pFac.pf.GW302_EngageDiameter = PF_GW302_EngageDiameter[PF_DEF]; // %
	pFac.pf.GW302_BaleIndicator = PF_GW302_BaleIndicator[PF_DEF]; // Steer/Fill
	pFac.pf.GW302_RollerDelay = PF_GW302_RollerDelay[PF_DEF]; // s
	pFac.pf.GW302_EngageTime =PF_GW302_EngageTime[PF_DEF]; // s
	pFac.pf.GW302_DiagnosticMode = PF_GW302_DiagnosticMode[PF_DEF];
	// Technician Menu - Netting Setup
	pFac.pf.GW302_NetOn = PF_GW302_NetOn[PF_DEF]; // s
	pFac.pf.GW302_NetCutPulse = PF_GW302_NetCutPulse[PF_DEF]; // pulses
	pFac.pf.GW302_NetCutTime = PF_GW302_NetCutTime[PF_DEF]; // s
	pFac.pf.GW302_MinDiaPulsPerLay = PF_GW302_MinDiaPulsPerLay[PF_DEF]; // pulses/layer
	pFac.pf.GW302_MaxDiaPulsPerLay = PF_GW302_MaxDiaPulsPerLay[PF_DEF]; // pulses/layer
	pFac.pf.GW302_NetFactor = PF_GW302_NetFactor[PF_DEF];
	pFac.pf.GW302_NoNetPulse = PF_GW302_NoNetPulse[PF_DEF]; // s
	pFac.pf.GW302_PreNetPulse = PF_GW302_PreNetPulse[PF_DEF]; // pulses
	pFac.pf.GW302_NetSystem = PF_GW302_NetSystem[PF_DEF]; // Beak/Roller
	pFac.pf.GW302_BeakEngage = PF_GW302_BeakEngage[PF_DEF]; // pulses
	pFac.pf.GW302_NetTwineOption = PF_GW302_NetTwineOption[PF_DEF]; // Net or twine
	pFac.pf.GW302_TwineDelay = PF_GW302_TwineDelay[PF_DEF]; //Twine Delay

	// Technician Menu - Twine Setup
	pFac.pf.GW302_TwineOption = PF_GW302_TwineOption[PF_DEF]; // Net option on or off
	pFac.pf.GW302_TwineFeedDiameter = PF_GW302_TwineFeedDiameter[PF_DEF]; // Feed Diameter
	pFac.pf.GW302_TwineFeedTime = PF_GW302_TwineFeedTime[PF_DEF]; // Feed Time
	pFac.pf.GW302_TwineFeedSpeed = PF_GW302_TwineFeedSpeed[PF_DEF]; // Feed Speed
	pFac.pf.GW302_TwineTrackDelay = PF_GW302_TwineTrackDelay[PF_DEF]; // Track Delay
	pFac.pf.GW302_TwineLenghtPulse = PF_GW302_TwineLenghtPulse[PF_DEF]; // Twine Length Pulse
	pFac.pf.GW302_TrackDistancePulse = PF_GW302_TrackDistancePulse[PF_DEF]; // Track Distance Pulse
	pFac.pf.GW302_TrackMaxFwd = PF_GW302_TrackMaxFwd[PF_DEF]; // Track Max Fwd
	pFac.pf.GW302_TrackMaxRev = PF_GW302_TrackMaxRev[PF_DEF]; // Track Max Rev
	pFac.pf.GW302_TwineMinimumEdgeDistance = PF_GW302_TwineMinimumEdgeDistance[PF_DEF]; //Minimum Edge Distance
	pFac.pf.GW302_TwineMinimumCentreDistance = PF_GW302_TwineMinimumCentreDistance[PF_DEF]; //Minimum Centre Distance
	pFac.pf.GW302_TwineTrackProtection = PF_GW302_TwineTrackProtection[PF_DEF];
	pFac.pf.GW302_PreTwinePulses = PF_GW302_PreTwinePulses[PF_DEF];
	pFac.pf.GW302_PreTwineDiam = PF_GW302_PreTwineDiam[PF_DEF];
	pFac.pf.GW302_TwineEngageDiameter = PF_GW302_TwineEngageDiameter[PF_DEF];
	pFac.pf.GW302_PreTwineRev = PF_GW302_PreTwineRev[PF_DEF];
	pFac.pf.GW302_PreTwine2Rev = PF_GW302_PreTwine2Rev[PF_DEF];

	// Technician Menu - Diameter Setup
	pFac.pf.GW302_DiameterMin = PF_GW302_DiameterMin[PF_DEF]; // %
	pFac.pf.GW302_DiameterMax = PF_GW302_DiameterMax[PF_DEF]; // %
	pFac.pf.GW302_DiaCorrection = PF_GW302_DiaCorrection[PF_DEF]; // V
	pFac.pf.GW302_BaleDiaSmoothingFactor = PF_GW302_BaleDiaSmoothingFactor[PF_DEF]; // %
	pFac.pf.GW302_DensityPresSmoothingFactor = PF_GW302_DensityPresSmoothingFactor[PF_DEF]; // %
	pFac.pf.GW302_BaleShapeSmoothingFactor = PF_GW302_BaleShapeSmoothingFactor[PF_DEF]; // %
	pFac.pf.GW302_KnifePresSmoothingFactor = PF_GW302_KnifePresSmoothingFactor[PF_DEF]; // %

	// Technician Menu - Density Setup
	pFac.pf.GW302_Density1 = PF_GW302_Density1[PF_DEF]; // V
	pFac.pf.GW302_Density2 = PF_GW302_Density2[PF_DEF]; // V
	pFac.pf.GW302_Density3 = PF_GW302_Density3[PF_DEF]; // V
	pFac.pf.GW302_Density4 = PF_GW302_Density4[PF_DEF]; // V
	pFac.pf.GW302_Density5 = PF_GW302_Density5[PF_DEF]; // V
	pFac.pf.GW302_Density6 = PF_GW302_Density6[PF_DEF]; // V
	pFac.pf.GW302_Density7 = PF_GW302_Density7[PF_DEF]; // V
	pFac.pf.GW302_Density8 = PF_GW302_Density8[PF_DEF]; // V
	pFac.pf.GW302_Density9 = PF_GW302_Density9[PF_DEF]; // V
	pFac.pf.GW302_Density10 = PF_GW302_Density10[PF_DEF]; // V

	// Technician Menu - Knives Setup
	pFac.pf.GW302_KnifePressure = PF_GW302_KnifePressure[PF_DEF]; // V
	pFac.pf.GW302_KnifePresWarn = PF_GW302_KnifePresWarn[PF_DEF]; // V
	pFac.pf.GW302_DiverterDelay = PF_GW302_DiverterDelay[PF_DEF]; // s

	// Technician Menu - Formation 2
	for( c = 0; c < BP_PRESET_MAX; c++ )   // Bale Profile defaults set for Presets A to E
	{
		pFac.pf.GW302_FormationDensity[c]   = PresetFormationDensity[c]; // ID
		pFac.pf.GW302_FormationDiameter[c]  = PresetFormationDiameter[c]; // %
		pFac.pf.GW302_Formation2Density[c]  = PresetFormation2Density[c]; // ID
		pFac.pf.GW302_Formation2Diameter[c] = PresetFormation2Diameter[c]; // %
	}

	// Diagnostics - Digital Inputs
	pFac.pf.GW302_ChoppingKnivesFitted = PF_GW302_ChoppingKnivesFitted[PF_DEF]; // Disabled/Enabled

	// Twine Factors
	pFac.pf.TwineNumberOfRevAroundEdge = PF_TwineNumberOfRevAroundEdge[PF_DEF];
	pFac.pf.TwineDistanceFromEdge = (pFac.pf.GW302_TwineMinimumEdgeDistance/10);
	pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre = PF_TwineNumberOfStopsBetweenEdgeAndCentre[PF_DEF];
	pFac.pf.TwineStopTime = PF_TwineStopTime[PF_DEF];
	pFac.pf.TwineNumberOfRevolutionsAroundCentre = PF_TwineNumberOfRevolutionsAroundCentre[PF_DEF];
	pFac.pf.DistanceFromCentre = PF_DistanceFromCentre[PF_DEF];
	pFac.pf.NoTwineWarningTime = PF_NoTwineWarningTime[PF_DEF];

	pFac.pf.CheckSum = CheckSumIt( (unsigned char *)&(pFac), (unsigned char *)&(pFac.pf.CheckSum) );
	
#ifndef WINDOWS
	// write main factors
	SPI_FLASH_EraseThenWrite( (uint8_t *)&pFac, pFac_BASEADDRESS_MAIN, sizeof( pFac ), 0, 0 );

	// write backup factors
	SPI_FLASH_EraseThenWrite( (uint8_t *)&pFac, pFac_BASEADDRESS_BACK, sizeof( pFac ), 0, 0 );
#endif
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
// Function Name       : SetDefaultTotals
// Object              : reset power on defaults
// Output Parameters   :
/*-----------------------------------------------------------*/
void SetDefaultTotals( void )
{
	unsigned short int const freqPWM[PWM_OUTMAX] = { 2000, 2000, 2000, 2000, 100, 100, 100, 100 };

	struct PWMsettings *ptPWM;
	unsigned char c;
	
	pRun.pr.Brightness[0] = DISPLAY_DEFAULT_DAY_BRIGHTNESS;
	pRun.pr.Brightness[1] = DISPLAY_DEFAULT_BRIGHTNESS;
	pRun.pr.Power = DISPLAY_DEFAULT_PANEL_LOADING;
	pRun.pr.Tempcomp = DISPLAY_DEFAULT_TEMPCOMP;
	pRun.pr.Bias = DISPLAY_DEFAULT_BIAS_RATIO;
	pRun.pr.Gain = DISPLAY_DEFAULT_GAIN;
	pRun.pr.GrayScale = DISPLAY_DEFAULT_GRAY_SCALE;
	pRun.pr.Contrast[0] = DISPLAY_DEFAULT_DAY_CONTRAST;
	pRun.pr.Contrast[1] = DISPLAY_DEFAULT_CONTRAST;
	pRun.pr.LineRate = DISPLAY_DEFAULT_LINE_RATE;
	pRun.pr.PixelMask = DISPLAY_DEFAULT_MASK;

	pRun.pr.LubeCounter = PF_GW302_LubeAlarm[PF_DEF]; // bales

	ptPWM = &pRun.pr.PWMout[0];
	for( c = 0; c < PWM_OUTMAX; c++ )
	{
		ptPWM->enable = 0;
		ptPWM->status = 0;
		ptPWM->duty = PWM_DUTY100 / 2;
		ptPWM->freq = freqPWM[c];

		ptPWM++;
	}
	

	pRun.pr.PipsOn = 0;
	pRun.pr.BuzzerTest = 0;
        #ifndef WINDOWS
	pRun.pr.memory_status = RunFactorStatus;
        #endif
	pRun.pr.PowerCount = 0;
	
	pRun.pr.CheckSum = CheckSumIt( (unsigned char *)&pRun, (unsigned char *)&(pRun.pr.CheckSum) );

#ifndef WINDOWS
	// write main factors
	SPI_FLASH_EraseThenWrite( (uint8_t *)&pRun, pRun_BASEADDRESS_MAIN, sizeof( pRun ), 0, 0 );

	// write backup factors
	SPI_FLASH_EraseThenWrite( (uint8_t *)&pRun, pRun_BASEADDRESS_BACK, sizeof( pRun ), 0, 0 );
#endif

}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
// Function Name       : SetStartupDefaults
// Object              : set global varialbles to power on values
// Output Parameters   :
/*-----------------------------------------------------------*/
void SetStartupDefaults( void )
{
	ptText = TextString[pFac.pf.Lang];
#ifndef WINDOWS
	key = ~GPIOD->IDR & SW_MASK;

		#endif
	if( (key & ~(SW_NEW|SW_REP)) == SW_CU )
		main_window = screen147;
	else
		main_window = screen00;

	looptime = 0;

	AutoMan =0;
	SecondScreen = 0;


	if(pFac.pf.GW302_PreTwinePulses)
	{
		PreTwineSeq = 1;
		PreTwineSeqActive = 1;
	}
	else
		PreTwineSeq = 0;

}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
unsigned char btobcd( unsigned char bin )
{

return( ((bin / 10) * 0x10) + (bin % 10) );	
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
unsigned char bcdtob( unsigned char bcd )
{

return( ((bcd / 0x10) * 10) + (bcd % 0x10) );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void SetCurrentTime( unsigned char *time )
{
#ifndef WINDOWS
	I2CMode = I2C_CLOCK_HOLD;

	ClockRam[0] = *time++ | IIC_CLOCK_START; // seconds
	ClockRam[1] = *time++; // minutes
	ClockRam[2] = *time++; // hours
	ClockRam[3] = *time++ | IIC_CLOCK_BATEN; // day of week
	ClockRam[4] = *time++; // day of month
	ClockRam[5] = *time++; // month
	ClockRam[6] = *time; // year
	ClockRam[7] = 0; // control
	ClockRam[8] = 0; // cal

	I2CMode = I2C_CLOCK_SET;
#endif    
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void SetCanTime( struct CanMessage *CanM_Rx )
{
	unsigned char tstring[8];
	
	tstring[0] = CanM_Rx->Data.uc[1]; // seconds
	tstring[1] = CanM_Rx->Data.uc[2]; // minutes
	tstring[2] = CanM_Rx->Data.uc[3]; // hours
	tstring[3] = CanM_Rx->Data.uc[4]; // day
	tstring[4] = CanM_Rx->Data.uc[5]; // date
	tstring[5] = CanM_Rx->Data.uc[6]; // century & month
	tstring[6] = CanM_Rx->Data.uc[7]; // year
	SetCurrentTime( &tstring[0] );

}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void PipsGen( void )
{
	static unsigned int last_time;

	unsigned int current_time;
	unsigned char uc;

	// seconds - low
	current_time = (unsigned int)(ClockRam[0] % 0x10);

	if( last_time != current_time )
	{
		last_time = current_time;

		// seconds - high
		current_time += (unsigned int)(ClockRam[0] / 0x10) * 10UL;
		// minutes - low
		current_time += (unsigned int)(ClockRam[1] % 0x10) * 60UL;
		// minutes - high
		current_time += (unsigned int)(ClockRam[1] / 0x10) * 600UL;
		// hours - low
		current_time += (unsigned int)(ClockRam[2] % 0x10) * 3600UL;
		// hours - high
		current_time += (unsigned int)(ClockRam[2] / 0x10) * 36000UL;


		// turn output on to power external device during the day time
		SetOutputPWM( PWM_OP01, 0, ((current_time < STOP_TIME) && (current_time > START_TIME)) ? PWM_DUTY100 : PWM_DUTY000 );

		uc = 0;

		while( 1 )
		{
			while( !(TD[uc].day & (0x01 << ClockRam[3])) ) // day
			{
				uc++;
			}
			if( !TD[uc].time ) // last entry then exit
			{
				break;
			}

			if( current_time >= TD[uc].time )
			{
				switch( TD[uc].stage )
				{
				case 0:
					buzz_length = 0;
					break;

				case 1:
					off_length = 0;
					buzz_length = 1000;
					break;

				case 2:
					if( !off_length )
					{
						off_length = 500;
						buzz_length = 500;
					}
					break;
				}
				break; // exit while loop at >= time match
			}
			uc++;
		}
	}
	
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
// http://www.azillionmonkeys.com/qed/sqroot.html (integer square root search on google)
// Integer Square Root function
// Contributors include Arne Steinarson for the basic approximation idea,
// Dann Corbit and Mathew Hendry for the first cut at the algorithm,
// Lawrence Kirby for the rearrangement, improvments and range optimization
// and Paul Hsieh for the round-then-adjust idea.
unsigned int integer_sqrt( unsigned int x )
{
	unsigned int xn;

	if( x >= 0x10000 )
	{
		if( x >= 0x1000000 )
		{
			if( x >= 0x10000000 )
			{
				if( x >= 0x40000000 )
				{
					if( x >= (65535UL * 65535UL) )
					{
						return 65535;
					}
					xn = sqrtTable[x >> 24] << 8;
				}
				else
				{
					xn = sqrtTable[x >> 22] << 7;
				}
			}
			else
			{
				if( x >= 0x4000000 )
				{
					xn = sqrtTable[x >> 20] << 6;
				}
				else
				{
					xn = sqrtTable[x >> 18] << 5;
				}
			}
		}
		else
		{
			if( x >= 0x100000 )
			{
				if( x >= 0x400000 )
				{
					xn = sqrtTable[x >> 16] << 4;
				}
				else
				{
					xn = sqrtTable[x >> 14] << 3;
				}
			}
			else
			{
				if( x >= 0x40000 )
				{
					xn = sqrtTable[x >> 12] << 2;
				}
				else
				{
					xn = sqrtTable[x >> 10] << 1;
				}
			}
			goto nr1;
		}
	}
	else
	{
		if( x >= 0x100 )
		{
			if( x >= 0x1000 )
			{
				if( x >= 0x4000 )
				{
					xn = (sqrtTable[x >> 8] >> 0) + 1;
				}
				else
				{
					xn = (sqrtTable[x >> 6] >> 1) + 1;
				}
			}
			else
			{
				if( x >= 0x400 )
				{
					xn = (sqrtTable[x >> 4] >> 2) + 1;
				}
				else
				{
					xn = (sqrtTable[x >> 2] >> 3) + 1;
				}
			}
			goto adj;
		}
		else
		{
			return sqrtTable[x] >> 4;
		}
	}

	// Run two iterations of the standard convergence formula
	xn = (xn + 1 + x / xn) / 2;

nr1:
	xn = (xn + 1 + x / xn) / 2;

adj:
	if( (xn * xn) > x ) // Correct rounding if necessary
	{
		xn--;
	}

	return( xn );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void CalculateAngleSingle( struct Angles *ptXYZ, signed short int x, signed short int y, signed short int z )
{
	float tempf;

	// calculate value
	tempf = (float)x / 256.0;
	if( tempf < -1.0 )
	{
		tempf = -1.0;
	}
	else if( tempf > 1.0 )
	{
		tempf = 1.0;
	}
	ptXYZ->x = asinf( tempf ) * (180.0 / (M_TWOPI / 2.0));


	// calculate value
	tempf = (float)y / 256.0;
	if( tempf < -1.0 )
	{
		tempf = -1.0;
	}
	else if( tempf > 1.0 )
	{
		tempf = 1.0;
	}
	ptXYZ->y = asinf( tempf ) * (180.0 / (M_TWOPI / 2.0));

	// calculate value
	tempf = (float)z / 256.0;
	if( tempf < -1.0 )
	{
		tempf = -1.0;
	}
	else if( tempf > 1.0 )
	{
		tempf = 1.0;
	}
	ptXYZ->z = acosf( tempf ) * (180.0 / (M_TWOPI / 2.0));
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void LookupAngleSingle( struct Angles *ptXYZ, signed short int x, signed short int y, signed short int z )
{
	signed int res;

	// lookup value
	if( x > 255 )
	{
		res = 256;
	}
	else if( x < -255 )
	{
		res = -256;
	}
	else
	{
		res = asinTable[abs( x )];
		if( x < 0 )
		{
			res = -res;
		}
	}
	ptXYZ->x = (float)(res * 90) / 256.0;

	// lookup value
	if( y > 255 )
	{
		res = 256;
	}
	else if( y < -255 )
	{
		res = -256;
	}
	else
	{
		res = asinTable[abs( y )];
		if( y < 0 )
		{
			res = -res;
		}
	}
	ptXYZ->y = (float)(res * 90) / 256.0;

	// lookup value
	if( z > 255 )
	{
		res = 0;
	}
	else if( z < -255 )
	{
		res = 0;
	}
	else
	{
		res = acosTable[abs( z )];
		if( z < 0 )
		{
			res = -res;
		}
	}
	ptXYZ->z = (float)(res * 90) / 256.0;
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void CalculateAngleTriple( struct Angles *ptXYZ, signed short int x, signed short int y, signed short int z )
{
	float tempf;

	tempf = (float)((y * y) + (z * z));
	tempf = sqrt( tempf );
	if( tempf == 0.0 )
	{
		ptXYZ->x = 90.0;
	}
	else
	{
		tempf = (float)x / tempf;
		ptXYZ->x = atanf( tempf ) * (180.0 / (M_TWOPI / 2.0));
	}

	tempf = (float)((x * x) + (z * z));
	tempf = sqrt( tempf );
	if( tempf == 0.0 )
	{
		ptXYZ->y = 90.0;
	}
	else
	{
		tempf = (float)y / tempf;
		ptXYZ->y = atanf( tempf ) * (180.0 / (M_TWOPI / 2.0));
	}

	if( z == 0 )
	{
		ptXYZ->z = 90.0;
	}
	else
	{
		tempf = (float)((x * x) + (y * y));
		tempf = sqrt( tempf );
		tempf = tempf / (float)z;
		ptXYZ->z = atanf( tempf ) * (180.0 / (M_TWOPI / 2.0));
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
float inv_tan( unsigned int ui )
{
	float res;
	
	if( ui > 256 ) // > 45
	{
		ui = (256 * 256) / ui;
		res = 90.0 - (atanTable[ui] * (45.0 / 256.0));
	}
	else if( ui == 256 ) // 45
	{
		res = 45.0;
	}
	else // 0 - 45
	{
		res = atanTable[ui] * (45.0 / 256.0);
	}
	
	return( res );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void LookupAngleTriple( struct Angles *ptXYZ, signed short int x, signed short int y, signed short int z )
{
	unsigned int ui;

	// arctan-function has to cover the range of 0...45 degrees e.a values of x between 0 and 1, first to
	// avoid overflow errors or a division by zero problem at angles around 90 degrees.
	// If the arctan has to be found for an angle alpha between 45 and 90 degrees (e.a. values of x above 1),
	// you'll search for the arctan(1/x) and extract alpha=90-arctan(1/x).

	if( x == 0 )
	{
		ptXYZ->x = 0.0;
	}
	else
	{
		ui = (y * y) + (z * z);
		ui = integer_sqrt( ui );
		if( ui == 0 )
		{
			ptXYZ->x = 90.0;
		}
		else
		{
			ui = (abs( x ) * 256) / ui;
			ptXYZ->x = inv_tan( ui );
		}
		if( x < 0 )
		{
			ptXYZ->x = -ptXYZ->x;
		}
	}

	if( y == 0 )
	{
		ptXYZ->y = 0;
	}
	else
	{
		ui = (x * x) + (z * z);
		ui = integer_sqrt( ui );
		if( ui == 0 )
		{
			ptXYZ->y = 90.0;
		}
		else
		{
			ui = (abs( y ) * 256) / ui;
			ptXYZ->y = inv_tan( ui );
		}
		if( y < 0 )
		{
			ptXYZ->y = -ptXYZ->y;
		}
	}

	if( z == 0 )
	{
		ptXYZ->z = 90.0;
	}
	else
	{
		ui = (x * x) + (y * y);
		ui = integer_sqrt( ui );
		ui = (ui * 256) / abs( z );
		ptXYZ->z = inv_tan( ui );
		if( z < 0 )
		{
			ptXYZ->z = -ptXYZ->z;
		}
	}
	if( z < 0 )
	{
		ptXYZ->z = -ptXYZ->z;
	}
}
/*-----------------------------------------------------------*/

/*McHale Functions*/
// -----------------------------------------------------------
// Function Name       : CalculateNetPulseFrequency
// Object              : Calculates net pulse frequency (Hz)
// Output Parameters   : NetPulseFrequency
// -----------------------------------------------------------
void CalculateNetPulseFrequency( void )
{
       #ifndef WINDOWS
	NetPulseFrequency = fIP[3].frequency;
        #endif

}
// -----------------------------------------------------------

// -----------------------------------------------------------
// Function Name       : ResetSequenceFactors
// -----------------------------------------------------------
void ResetSequenceFactors( unsigned char seq )
{
	SeqOpOff = SeqOpOn;
	OpOff = SeqOpOff;

	//SeqOpOff = 0; /* Nothing off */
	SeqOpOn = 0;  /* Nothing on */

	ContinueOnKeyRelease = 0;
	StartAutoNet = 0;

	if(TwineAutoManMode && (pFac.pf.GW302_NetTwineOption == TO_TWINE))
		Stage = TWINE_START;
	else
		Stage = STAGE_START;

	stagecomp = SC_CLEAR;

	cont_beep = 0;
	target_bale_dia_reached_cont_beep = 0;
	pre_warning_cont_beep = 0;
	start_chamber_open_delay_cont_beep = 0;
	end_chamber_open_delay_cont_beep = 0;

	DensityPressureTooHigh = 0; // Density Pressure Too High Warning

	SequenceInProgress = 0;

	OP2OnDelayTime  = 0;        // Net Clutch (AKA Net On)
	OP2OffDelayTime = 0;
	OP7OffDelayTime = 0;        // Net Cut Motor (AKA Net Cut Trip)
	OP8OnDelayTime  = 0;        // Density Release Solenoid (AKA Density Control)
	OP8OffDelayTime = 0;

	NetPulseCount = 0UL;        // Resetting IP4 net pulse count
	BeakEngagePulseCount = 0UL; // Resetting IP4 'Beak Engage' pulse count
	NewNetPulses = 0UL;         // Resetting new IP4 pulses

#ifdef DEVELOP // Testing pulse counting
	//Saved_NetPulseCount = 0UL;        // Resetting
	//SavedBeakEngagePulseCount = 0UL;  // Resetting
#endif

	NetWarningDelayTime = 0;    // Check Net Error (AKA Net Warning)
	NetWarningOn = 0;           // Check Net Error (AKA Net Warning) turned off

	RollEngageTurnedOn = 0;       // OP5 Roller Engage Solenoid (AKA Pre Net Position) turned off
	SavedRollEngageTurnedOn = 0;  // Saved when IP3 opens in AUTO mode
	NetWarningDisabledWhileIP3OpenInAutoMode = 0;
	NettingStartBeepsDisabledWhileIP3OpenInAutoMode = 0; // Including Pre Warning & Net Delay beep

	ChamberClosedDelayTime = 0; // Door Closed

	ChamberOpenDelayTime = 0;   // applies to MAN mode only

	ChamberOpenBeepDelay = COBD_CHAMBER_OPEN_BEEP_DELAY; // to ensure beeping starts after 1 s delay -- applies to AUTO mode only

	BaleKickerStatus = BKS_CLEAR;               // Reset Bale Kicker Down/Up flags
	BaleDiameterStatusInManMode = BDSIMM_CLEAR; // Reset Bale Diameter Below/Above Engage Diameter (in MAN mode) flags
	ChamberStatus = CS_CLEAR;                   // Reset Chamber Closed/Open flags

	OP8OnCount = 0;                 // Reset count for Density Release Solenoid (OP8) on after Chamber (IP3) closes then opens (MAN/AUTO)

	DrivingArrowBeepsDisabled = 0;  // Driving Arrow Beeps turned on

	DensityControlFunctionActive = 0;                 // Disabling (de-activating) Density Control Function (based on Target Bale Density)
	NetPulseCheckingEnabled = 0;                      // Disabling checking for no pulses on IP4 (for NetWarningDelayTime)
	OP5TurnedOnAfterBeakEngagePulsesEnabled = 0;      // Disabling turning on Roller Engage Solenoid (OP5) after 'Beak Engage' pulses counted on IP4
	SavedOP5TurnedOnAfterBeakEngagePulsesEnabled = 0; // Saved when IP3 opens in AUTO mode

	/*	pFac.pf.GW302_TwineOption = TO_OFF;
		pFac.pf.GW302_NetTwineOption = TO_NET;*/
}
// -----------------------------------------------------------
// -----------------------------------------------------------
// Function Name       : AllSeqOpOff
// -----------------------------------------------------------
void AllSeqOpOff( void )
{
	SeqOpOff = SeqOpOn;
	OpOff = SeqOpOff;
	//SeqOpOff = 0; /* Nothing off */
	SeqOpOn = 0;  /* Nothing on */
}
// ----------------------------------------------------

// -----------------------------------------------------------
// Function Name       : ResetLubeCount
// -----------------------------------------------------------
void ResetLubeCount( void )
{

	if( pRun.pr.LubeCounter < 0 )
	{
		pRun.pr.LubeCounter += pFac.pf.GW302_LubeAlarm; // Reset to Lube Alarm (in bales) minus the number of bales since alarm first appeared
	}
	else // No extra bales since alarm first appeared
	{
		pRun.pr.LubeCounter = pFac.pf.GW302_LubeAlarm; // Reset to Lube Alarm (in bales)
	}

}
// -----------------------------------------------------------

// -----------------------------------------------------------
// Function Name       : CalculateAnalogInputRollingAverages
// Object              : Calculates analogue input rolling averages
// Output Parameters   : BaleDiaVolts     - Bale Diameter (AnIP1)
//                       DensityPresVolts - (Bale) Density Pressure (AnIP2)
//                       BaleShapeVolts   - Bale Shape Indicator (AnIP3)
//                       KnifePresVolts   - Knife Pressure (AnIP4)
// -----------------------------------------------------------
void CalculateAnalogInputRollingAverages( void )
{
	// Average Sample
	float BaleDiaVoltsAvSample;     // V
	float DensityPresVoltsAvSample; // V
	float BaleShapeVoltsAvSample;   // V
	float KnifePresVoltsAvSample;   // V

	// Rolling Average Sample
	static float BaleDiaVoltsRollAvSample;     // V
	static float DensityPresVoltsRollAvSample; // V
	static float BaleShapeVoltsRollAvSample;   // V
	static float KnifePresVoltsRollAvSample;   // V

	// RollingAverageSample = ( [SmoothingFactor]% x AverageSample ) +
	//                        ( [100-SmoothingFactor]% x Previous_RollingAverageSample )
	// where AverageSample = Average of total NUM_ACCUMULATED_SAMPLES

	// AnIP1: Bale Diameter
	//BaleDiaVoltsTotAccumMultSamples = (unsigned long int)( (float)BaleDiaVoltsTotAccumMultSamples * POTDIV * ATODSCALE * (float)INPUT_ACCURACY_MULTIPLIER );
	BaleDiaVoltsAvSample = (float)AtoDmux[0].smooth * ATOD_IP_SCALE;
	BaleDiaVoltsRollAvSample = ( ((float)pFac.pf.GW302_BaleDiaSmoothingFactor/100.0) * BaleDiaVoltsAvSample ) +
			( ((100.0 - (float)pFac.pf.GW302_BaleDiaSmoothingFactor)/100.0) * BaleDiaVoltsRollAvSample);
	BaleDiaVolts = BaleDiaVoltsRollAvSample;

	// AnIP2: (Bale) Density Pressure
	//DensityPresVoltsTotAccumMultSamples = (unsigned long int)( (float)DensityPresVoltsTotAccumMultSamples * POTDIV * ATODSCALE * (float)INPUT_ACCURACY_MULTIPLIER );
	DensityPresVoltsAvSample = (float)AtoDmux[1].smooth * ATOD_IP_SCALE;
	DensityPresVoltsRollAvSample = ( ((float)pFac.pf.GW302_DensityPresSmoothingFactor/100.0) * DensityPresVoltsAvSample ) +
			( ((100.0 - (float)pFac.pf.GW302_DensityPresSmoothingFactor)/100.0) * DensityPresVoltsRollAvSample);
	DensityPresVolts = DensityPresVoltsRollAvSample;

	// AnIP3: Bale Shape Indicator
	//BaleShapeVoltsTotAccumMultSamples = (unsigned long int)( (float)BaleShapeVoltsTotAccumMultSamples * POTDIV * ATODSCALE * (float)INPUT_ACCURACY_MULTIPLIER );
	BaleShapeVoltsAvSample = (float)AtoDmux[2].smooth * ATOD_IP_SCALE;
	BaleShapeVoltsRollAvSample = ( ((float)pFac.pf.GW302_BaleShapeSmoothingFactor/100.0) * BaleShapeVoltsAvSample ) +
			( ((100.0 - (float)pFac.pf.GW302_BaleShapeSmoothingFactor)/100.0) * BaleShapeVoltsRollAvSample);
	BaleShapeVolts = BaleShapeVoltsRollAvSample;

	// AnIP4: Knife Pressure
	//KnifePresVoltsTotAccumMultSamples = (unsigned long int)( (float)KnifePresVoltsTotAccumMultSamples * POTDIV * ATODSCALE * (float)INPUT_ACCURACY_MULTIPLIER );
	KnifePresVoltsAvSample = (float)AtoDmux[3].smooth * ATOD_IP_SCALE;
	KnifePresVoltsRollAvSample = ( ((float)pFac.pf.GW302_KnifePresSmoothingFactor/100.0) * KnifePresVoltsAvSample ) +
			( ((100.0 - (float)pFac.pf.GW302_KnifePresSmoothingFactor)/100.0) * KnifePresVoltsRollAvSample);
	KnifePresVolts = KnifePresVoltsRollAvSample;
}

// -----------------------------------------------------------
// Function Name       : Inputs
// Object              : Calculates inputs
// Output Parameters   : DiaTargetVolts
//                       DiaMinExtDisplayPercent     (0.0 - 100.0 % of extended display range from PotZero to DiaMax)
//                       DiaTargetExtDisplayPercent  (0.0 - 100.0 % of extended display range from PotZero to DiaMax)
//                       BaleDiaExtDisplayPercent    (0.0 - 100.0 % of extended display range from PotZero to DiaMax)
//                       DiaMinVolts
//                       DiaMaxVolts
// -----------------------------------------------------------
void Inputs( void )
{
	float DiaVoltsPer1PercentFullRange;       // V
	float DiaCorrectionNudgeVolts;            // V
	float DiaZeroVolts;                       // V

	float DiaVoltsPerSetting;                 // V where Setting = 1 to 109 (which corresponds to 60 to 168cm)

#ifdef DEVELOP
	float DiaVoltsPer1PercentDisplayRange;    // V where DisplayRange = 0 to 100 % (which corresponds to 60 to 168cm)

	unsigned char tstring[32];                // used in printing out values
#endif

	// Volts per 1% of full range = Voltage Range / 100  where Voltage Range = Bale Size Factor x Fixed Multiplier
	DiaVoltsPer1PercentFullRange = ((float)pFac.pf.GW302_BaleSizeFactor * DIA_FIXED_MULTIPLIER) / 100.0;

	// Correction nudge voltage = Units nudged x Size of unit
	DiaCorrectionNudgeVolts = (float)pFac.pf.GW302_DiameterCorrection * pFac.pf.GW302_DiaCorrection;

	// Diameter zero voltage corresponding to 0cm = Pot Zero Voltage + Correction nudge voltage
	DiaZeroVolts = pFac.pf.GW302_DiaPotZero + DiaCorrectionNudgeVolts;

	// Minimum voltage = (% diameter at setting 1 x Volts per 1%) + Diameter zero voltage corresponding to 0cm
	DiaMinVolts = ((float)pFac.pf.GW302_DiameterMin * DiaVoltsPer1PercentFullRange) + DiaZeroVolts;

	// Maximum voltage = (% diameter at setting 109 x Volts per 1%) + Diameter zero voltage corresponding to 0cm
	DiaMaxVolts = ((float)pFac.pf.GW302_DiameterMax * DiaVoltsPer1PercentFullRange) + DiaZeroVolts;

	// Volts per setting division = (Maximum voltage - Minimum voltage) / (109 - 1) where setting is 1 to 109 corresponding to 60cm to 168cm
	DiaVoltsPerSetting = (DiaMaxVolts - DiaMinVolts) / 108.0;

	// Target bale diameter voltage = ( (Target setting in cm - 60cm) x Volts per setting division  ) + Minimum voltage
	// Note: Target bale diameter voltage = Minimum voltage when Target setting is 60cm
	DiaTargetVolts = ((float)(pFac.pf.GW302_BaleDiameter[pFac.pf.GW302_BaleProfile] - 60) * DiaVoltsPerSetting) + DiaMinVolts;

#ifdef DEVELOP
	// Volts per 1% of display range (from 0 to 100 % corresponding to 60 to 168cm and setting 1 to 109)
	// Not used in any calculations
	DiaVoltsPer1PercentDisplayRange = (DiaMaxVolts - DiaMinVolts) / 100.0;
#endif

	// Current bale diameter -- shown on scale with target & minimum bale diameters indicated

	// Minimum bale diameter extended display percentage (0 - 100 % of extended display range from PotZero to DiaMax)
	if(DiaMinVolts > DiaZeroVolts)
		DiaMinExtDisplayPercent = ( (DiaMinVolts - DiaZeroVolts) / (DiaMaxVolts - DiaZeroVolts) ) * 100.0;
	else
		DiaMinExtDisplayPercent = 0.0; // Limiting to 0.0%
	if( DiaMinExtDisplayPercent > 100.0)
		DiaMinExtDisplayPercent = 100.0; // Limiting to 100.0%

	// Target bale diameter extended display percentage (0 - 100 % of extended display range from PotZero to DiaMax)
	if(DiaTargetVolts > DiaZeroVolts)
		DiaTargetExtDisplayPercent = ( (DiaTargetVolts - DiaZeroVolts) / (DiaMaxVolts - DiaZeroVolts) ) * 100.0;
	else
		DiaTargetExtDisplayPercent = 0.0; // Limiting to 0.0%
	if( DiaTargetExtDisplayPercent > 100.0)
		DiaTargetExtDisplayPercent = 100.0; // Limiting to 100.0%

	// Current bale diameter extended display percentage (0 - 100 %) = (Current bale diameter voltage minus Diameter zero voltage / Extended display voltage range) x 100 to convert to % value
	if(BaleDiaVolts > DiaZeroVolts)
		// Multiplying (BaleDiaVolts - DiaZeroVolts) by 100.0 before division for accuracy
		BaleDiaExtDisplayPercent = ( (BaleDiaVolts - DiaZeroVolts) * 100.0 ) / ( DiaMaxVolts - DiaZeroVolts );
	else
		BaleDiaExtDisplayPercent = 0.0; // Limiting to 0.0%
	if( BaleDiaExtDisplayPercent > 100.0)
		BaleDiaExtDisplayPercent = 100.0; // Limiting to 100.0%


	// Formation diameter voltage = % Upper diameter limit of 'Formation' stage * 100 % range from 0cm (zero voltage) to 168cm diameter (maximum voltage)
	DiaFormationVolts = ((float)pFac.pf.GW302_FormationDiameter[pFac.pf.GW302_BaleProfile] * ((DiaMaxVolts - DiaZeroVolts) / 100.0)) + DiaZeroVolts;

	// Formation2 diameter voltage = % Upper diameter limit of 'Formation2' stage * 100 % range from 0cm (zero voltage) to 168cm diameter (maximum voltage)
	DiaFormation2Volts = ((float)pFac.pf.GW302_Formation2Diameter[pFac.pf.GW302_BaleProfile] * ((DiaMaxVolts - DiaZeroVolts) / 100.0)) + DiaZeroVolts;

	// Core diameter voltage = ( (Core setting in cm - 60cm) x Volts per setting division  ) + Minimum voltage corresponding to 60cm
	DiaCoreVolts = ((float)(pFac.pf.GW302_CoreDiameter[pFac.pf.GW302_BaleProfile] - 60) * DiaVoltsPerSetting) + DiaMinVolts;

	// Engage diameter voltage = Engage % setting x Target diameter voltage (from Zero pot voltage)
	DiaEngageVolts = ((float)pFac.pf.GW302_EngageDiameter * ((DiaTargetVolts - DiaZeroVolts) / 100.0)) + DiaZeroVolts;

	//Twine Engage diameter voltage = Engage % setting x Target diameter voltage (from Zero pot voltage)
	TwineDiaEngageVolts = ((float)pFac.pf.GW302_TwineEngageDiameter * ((DiaTargetVolts - DiaZeroVolts) / 100.0)) + DiaZeroVolts;
	// Engage diameter voltage = Feed % setting x Target diameter voltage (from Zero pot voltage)
	DiaFeedVolts = ((float)pFac.pf.GW302_TwineFeedDiameter * ((DiaTargetVolts - DiaZeroVolts) / 100.0)) + DiaZeroVolts;

	// Pre warning beep diameter voltage = Pre Warning Beep % setting x Target diameter voltage (from Zero pot voltage)
	DiaPreWarnVolts = ((float)pFac.pf.GW302_PreWarningBeep * ((DiaTargetVolts - DiaZeroVolts) / 100.0)) + DiaZeroVolts;

	//Pre Twine voltage
	PreTwineVoltage = ((float)pFac.pf.GW302_PreTwineDiam * ((DiaTargetVolts - DiaZeroVolts) / 100.0)) + DiaZeroVolts;


	// Target density voltage during Main stage
	switch( pFac.pf.GW302_BaleDensity[pFac.pf.GW302_BaleProfile] )
	{
	case 1:
		DensityBaleVolts = pFac.pf.GW302_Density1;
		break;
	case 2:
		DensityBaleVolts = pFac.pf.GW302_Density2;
		break;
	case 3:
		DensityBaleVolts = pFac.pf.GW302_Density3;
		break;
	case 4:
		DensityBaleVolts = pFac.pf.GW302_Density4;
		break;
	case 5:
		DensityBaleVolts = pFac.pf.GW302_Density5;
		break;
	case 6:
		DensityBaleVolts = pFac.pf.GW302_Density6;
		break;
	case 7:
		DensityBaleVolts = pFac.pf.GW302_Density7;
		break;
	case 8:
		DensityBaleVolts = pFac.pf.GW302_Density8;
		break;
	case 9:
		DensityBaleVolts = pFac.pf.GW302_Density9;
		break;
	default: // 10
	DensityBaleVolts = pFac.pf.GW302_Density10;
	break;
	}

	// Target density voltage during Core stage
	switch( pFac.pf.GW302_CoreDensity[pFac.pf.GW302_BaleProfile] )
	{
	case 1:
		DensityCoreVolts = pFac.pf.GW302_Density1;
		break;
	case 2:
		DensityCoreVolts = pFac.pf.GW302_Density2;
		break;
	case 3:
		DensityCoreVolts = pFac.pf.GW302_Density3;
		break;
	case 4:
		DensityCoreVolts = pFac.pf.GW302_Density4;
		break;
	case 5:
		DensityCoreVolts = pFac.pf.GW302_Density5;
		break;
	case 6:
		DensityCoreVolts = pFac.pf.GW302_Density6;
		break;
	case 7:
		DensityCoreVolts = pFac.pf.GW302_Density7;
		break;
	case 8:
		DensityCoreVolts = pFac.pf.GW302_Density8;
		break;
	case 9:
		DensityCoreVolts = pFac.pf.GW302_Density9;
		break;
	default: // 10
	DensityCoreVolts = pFac.pf.GW302_Density10;
	break;
	}

	// Target density voltage during Formation2 stage
	switch( pFac.pf.GW302_Formation2Density[pFac.pf.GW302_BaleProfile] )
	{
	case 1:
		DensityFormation2Volts = pFac.pf.GW302_Density1;
		break;
	case 2:
		DensityFormation2Volts = pFac.pf.GW302_Density2;
		break;
	case 3:
		DensityFormation2Volts = pFac.pf.GW302_Density3;
		break;
	case 4:
		DensityFormation2Volts = pFac.pf.GW302_Density4;
		break;
	case 5:
		DensityFormation2Volts = pFac.pf.GW302_Density5;
		break;
	case 6:
		DensityFormation2Volts = pFac.pf.GW302_Density6;
		break;
	case 7:
		DensityFormation2Volts = pFac.pf.GW302_Density7;
		break;
	case 8:
		DensityFormation2Volts = pFac.pf.GW302_Density8;
		break;
	case 9:
		DensityFormation2Volts = pFac.pf.GW302_Density9;
		break;
	default: // 10
	DensityFormation2Volts = pFac.pf.GW302_Density10;
	break;
	}

	// Target density voltage during Formation stage
	switch( pFac.pf.GW302_FormationDensity[pFac.pf.GW302_BaleProfile] )
	{
	case 1:
		DensityFormationVolts = pFac.pf.GW302_Density1;
		break;
	case 2:
		DensityFormationVolts = pFac.pf.GW302_Density2;
		break;
	case 3:
		DensityFormationVolts = pFac.pf.GW302_Density3;
		break;
	case 4:
		DensityFormationVolts = pFac.pf.GW302_Density4;
		break;
	case 5:
		DensityFormationVolts = pFac.pf.GW302_Density5;
		break;
	case 6:
		DensityFormationVolts = pFac.pf.GW302_Density6;
		break;
	case 7:
		DensityFormationVolts = pFac.pf.GW302_Density7;
		break;
	case 8:
		DensityFormationVolts = pFac.pf.GW302_Density8;
		break;
	case 9:
		DensityFormationVolts = pFac.pf.GW302_Density9;
		break;
	default: // 10
	DensityFormationVolts = pFac.pf.GW302_Density10;
	break;
	}


#if 0 // Testing - intermediate inputs & outputs

	sprintf( tstring, "VP=%6.5fV", DiaVoltsPer1PercentFullRange ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 8, FIXED );
	sprintf( tstring, "CN=%6.5fV", DiaCorrectionNudgeVolts ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 18, FIXED );
	sprintf( tstring, "MI=%6.5fV", DiaMinVolts ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 28, FIXED );
	sprintf( tstring, "MA=%6.5fV", DiaMaxVolts ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 38, FIXED );
	sprintf( tstring, "PS=%6.5fV", DiaVoltsPerSetting ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 48, FIXED );
	sprintf( tstring, "PP=%6.5fV", DiaVoltsPer1PercentDisplayRange ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 58, FIXED );
#endif

#if 0 // Testing - intermediate inputs & outputs

	sprintf( tstring, "ZV=%6.5fV", DiaZeroVolts ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 8, FIXED );
	sprintf( tstring, "FV=%6.5fV", DiaFormationVolts ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 18, FIXED );
	sprintf( tstring, "F2=%6.5fV", DiaFormation2Volts ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 28, FIXED );
	sprintf( tstring, "CV=%6.5fV", DiaCoreVolts ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 38, FIXED );
	sprintf( tstring, "EV=%6.5fV", DiaEngageVolts ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 48, FIXED );
	sprintf( tstring, "PW=%6.5fV", DiaPreWarnVolts ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 58, FIXED );

#endif

#if 0 // Testing - intermediate inputs & outputs

	sprintf( tstring, "BD=%6.5fV", DensityBaleVolts ); // Set
	display_f1_text( tstring, TSMAX, 2, 8, FIXED );
	sprintf( tstring, "CD=%6.5fV", DensityCoreVolts ); // Set
	display_f1_text( tstring, TSMAX, 2, 18, FIXED );
	sprintf( tstring, "F2=%6.5fV", DensityFormation2Volts ); // Set
	display_f1_text( tstring, TSMAX, 2, 28, FIXED );
	sprintf( tstring, "FD=%6.5fV", DensityFormationVolts ); // Set
	display_f1_text( tstring, TSMAX, 2, 38, FIXED );
	sprintf( tstring, "On8=%02u Off8=%02u On2=%02u", OP8OnDelayTime, OP8OffDelayTime, OP2OnDelayTime); // Calculated
	display_f1_text( tstring, TSMAX, 2, 48, FIXED );
	sprintf( tstring, "DV=%6.5fV", DensityPresVolts ); // Input
	display_f1_text( tstring, TSMAX, 2, 58, FIXED );
#endif

#if 0 // Testing - main inputs & outputs

	//sprintf( tstring, "ST=%02X SC=%02X", Stage, stagecomp );
	//display_f1_text( tstring, TSMAX, 2, 68, FIXED );

	sprintf( tstring, "DiMaV=%3.2fV", DiaMaxVolts ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 8, FIXED );

	sprintf( tstring, "DTarV=%3.2fV", DiaTargetVolts );     // Calculated
	display_f1_text( tstring, TSMAX, 2, 18, FIXED );

	sprintf( tstring, "DiMiV=%3.2fV", DiaMinVolts ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 38, FIXED );

	sprintf( tstring, "MiEx%%=%3.2f%%", DiaMinExtDisplayPercent ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 48, FIXED );

	sprintf( tstring, "TaEx%%=%3.2f%%", DiaTargetExtDisplayPercent ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 58, FIXED );

	sprintf( tstring, "BDiE%%=%3.2f%%", BaleDiaExtDisplayPercent ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 68, FIXED );

	sprintf( tstring, "ZV=%6.5fV", DiaZeroVolts ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 88, FIXED );

	sprintf( tstring, "PP=%6.5fV", DiaVoltsPer1PercentDisplayRange ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 98, FIXED );

	sprintf( tstring, "PS=%6.5fV", DiaVoltsPerSetting ); // Calculated
	display_f1_text( tstring, TSMAX, 2, 108, FIXED );

#endif

#if 0 // Testing Analogue inputs
	sprintf( tstring, "KnifePresV=%4.3f", KnifePresVolts );
	display_f1_text( tstring, TSMAX, 2, 48, FIXED );

	sprintf( tstring, "BaleDiaV=%4.3f", BaleDiaVolts );
	display_f1_text( tstring, TSMAX, 2, 58, FIXED );

	sprintf( tstring, "DensityPresV=%4.3f", DensityPresVolts );
	display_f1_text( tstring, TSMAX, 2, 68, FIXED );

	sprintf( tstring, "BaleShapeV=%4.3f", BaleShapeVolts );
	display_f1_text( tstring, TSMAX, 2, 78, FIXED );
#endif

#if 0 // Testing - inputs & outputs
	sprintf( tstring, "%2.1fV", KnifePresVolts ); // Input Top - left
	display_f1_text( tstring, TSMAX, 42, 8, FIXED );

	sprintf( tstring, "%2.1fV", BaleShapeVolts ); // Input Top - right
	display_f1_text( tstring, TSMAX, 66, 8, FIXED );

	sprintf( tstring, "%2.1fV", DensityPresVolts ); // Input Bot - left
	display_f1_text( tstring, TSMAX, 42, 18, FIXED );

	sprintf( tstring, "%2.1fV", BaleDiaVolts ); // Input Bot - right
	display_f1_text( tstring, TSMAX, 66, 18, FIXED );
#endif

#if 0 // Testing
	sprintf( tstring, "%2.1fHz", NetPulseFrequency );
	display_f1_text( tstring, TSMAX, 2, 78, FIXED );

	sprintf( tstring, "%4u", TargetNetPulses ); // Input Bot - far right
	display_f1_text( tstring, TSMAX, 50, 98, FIXED );

	sprintf( tstring, "%4u", NetPulseCount ); // Input Bot - farthest far right
	display_f1_text( tstring, TSMAX, 84, 98, FIXED );

	//sprintf( tstring, "%4u", fIP[3].pcountfIP[3].pcount ); // Input Bot - farthest far right
	//display_f1_text( tstring, TSMAX, 118, 98, FIXED );
#endif

#if 0 // Testing pulse counting
	sprintf( tstring, "%2.1fHz", NetPulseFrequency );
	display_f1_text( tstring, TSMAX, 2, 78, FIXED );

	sprintf( tstring, "%4u", TargetNetPulses ); // Input Bot - far right
	display_f1_text( tstring, TSMAX, 50, 98, FIXED );

	sprintf( tstring, "%4u", NetPulseCount ); // Input Bot - farthest far right
	display_f1_text( tstring, TSMAX, 84, 98, FIXED );

	//sprintf( tstring, "%4u", NetMetresPulseCount ); // Input Bot - farthest far right
	//display_f1_text( tstring, TSMAX, 118, 98, FIXED );

	sprintf( tstring, "%4u", NewNetPulses ); // Input Bot - far right
	display_f1_text( tstring, TSMAX, 50, 88, FIXED );

	sprintf( tstring, "%4u", NetPulseCountPrev ); // Input Bot - farthest far right
	display_f1_text( tstring, TSMAX, 84, 88, FIXED );

	sprintf( tstring, "%4u", NetMetresPulseCount ); // Input Bot - farthest far right
	display_f1_text( tstring, TSMAX, 118, 88, FIXED );

#endif

#if 0 // Testing net metres, pulse count, target pulses
	sprintf( tstring, "MI=%2.1fV", DiaMinVolts ); // Calculated
	display_f1_text( tstring, TSMAX, 42, 18, FIXED );

	sprintf( tstring, "MA=%2.1fV", DiaMaxVolts ); // Calculated
	display_f1_text( tstring, TSMAX, 42, 28, FIXED );

	sprintf( tstring, "T=%2.1fV", DiaTargetVolts );     // Calculated
	display_f1_text( tstring, TSMAX, 42, 38, FIXED );

	sprintf( tstring, "ST=%02X", Stage );
	display_f1_text( tstring, TSMAX, 2, 58, FIXED );

	sprintf( tstring, "SC=%02X", stagecomp );
	display_f1_text( tstring, TSMAX, 2, 68, FIXED );

	/*sprintf( tstring, "%6u", TotalNetMetresLastSaved ); // NOT USED NOW
    display_f1_text( tstring, TSMAX, 2, 78, FIXED );*/

	sprintf( tstring, "%2.1fHz", NetPulseFrequency );
	display_f1_text( tstring, TSMAX, 2, 78, FIXED );

	sprintf( tstring, "%2.1fV", KnifePresVolts ); // Input Top - left
	display_f1_text( tstring, TSMAX, 2, 88, FIXED );

	sprintf( tstring, "%2.1fV", BaleShapeVolts ); // Input Top - right
	display_f1_text( tstring, TSMAX, 26, 88, FIXED );

	sprintf( tstring, "%2.1fp/l", NetPulsesPerLayer ); // Input Top - far right
	display_f1_text( tstring, TSMAX, 50, 88, FIXED );

	sprintf( tstring, "%2.1fV", DensityPresVolts ); // Input Bot - left
	display_f1_text( tstring, TSMAX, 2, 98, FIXED );

	sprintf( tstring, "%2.1fV", BaleDiaVolts ); // Input Bot - right
	display_f1_text( tstring, TSMAX, 26, 98, FIXED );

	sprintf( tstring, "%u p", TargetNetPulses ); // Input Bot - far right
	display_f1_text( tstring, TSMAX, 50, 98, FIXED );

	sprintf( tstring, "%u p", NetPulseCount ); // Input Bot - farthest far right
	display_f1_text( tstring, TSMAX, 74, 98, FIXED );
#endif

#if 0 // Testing Count for Net Pulses (on IP4) to stop after Target Pulses reached for Net Cut (OP7) to be turned off
	sprintf( tstring, "NPC=%u", NetPulseCount );
	display_f1_text( tstring, TSMAX, 69,  8, FIXED );

	sprintf( tstring, "LST=%u", NetPulseCountAtLastCheck );
	display_f1_text( tstring, TSMAX, 69,  18, FIXED );

	sprintf( tstring, "OP7=%u", OP7OffDelayTime );
	display_f1_text( tstring, TSMAX, 69,  28, FIXED );

	sprintf( tstring, "STP=%u", IP4PulsesStoppedCount );
	display_f1_text( tstring, TSMAX, 69,  38, FIXED );
#endif

#if 0 // Testing Analogue inputs
	sprintf( tstring, "D1=%4.3f", BaleDiaVolts );
	display_f1_text( tstring, TSMAX, 69, 28, FIXED );

	sprintf( tstring, "P2=%4.3f", DensityPresVolts );
	display_f1_text( tstring, TSMAX, 69, 78, FIXED );

	sprintf( tstring, "S3=%4.3f", BaleShapeVolts );
	display_f1_text( tstring, TSMAX, 69, 88, FIXED );

	sprintf( tstring, "K4=%4.3f", KnifePresVolts );
	display_f1_text( tstring, TSMAX, 69, 98, FIXED );
#endif

#if 0 // Testing memory
	sprintf( tstring, "F=%3u", sizeof(pFac.pf) );
	display_f1_text( tstring, TSMAX, 127, 16, FIXED );

	sprintf( tstring, "R=%3u", sizeof(pRun.pr) );
	display_f1_text( tstring, TSMAX, 127, 26, FIXED );

	sprintf( tstring, "RF=%1u", TestOutputReadMemAllocFail);
	display_f1_text( tstring, TSMAX, 127, 48, FIXED );

	sprintf( tstring, "WF=%1u", TestOutputWriteMemAllocFail);
	display_f1_text( tstring, TSMAX, 127, 58, FIXED );
#endif

#if 0 // Testing BaleKickerStatus & BaleDiameterStatusInManMode & ChamberStatus
	sprintf( tstring, "BKS=%02X", BaleKickerStatus );
	display_f1_text( tstring, TSMAX, 3, 78, FIXED );

	sprintf( tstring, "BDSIMM=%02X", BaleDiameterStatusInManMode );
	display_f1_text( tstring, TSMAX, 3, 88, FIXED );

	sprintf( tstring, "CS=%02X", ChamberStatus );
	display_f1_text( tstring, TSMAX, 3, 98, FIXED );
#endif

#if 0 // Testing Net Pulses (on IP4)
	sprintf( tstring, "NPC=%3lu", NetPulseCount ); // added l
	display_f1_text( tstring, TSMAX, 69,  8, FIXED );

	sprintf( tstring, "TNP=%3u", TargetNetPulses );
	display_f1_text( tstring, TSMAX, 69,  18, FIXED );

	sprintf( tstring, "LST=%3u", NetPulseCountAtLastCheck );
	display_f1_text( tstring, TSMAX, 69,  28, FIXED );

	sprintf( tstring, "OP7=%3u", OP7OffDelayTime );
	display_f1_text( tstring, TSMAX, 69,  38, FIXED );

	sprintf( tstring, "STP=%3u", IP4PulsesStoppedCount );
	display_f1_text( tstring, TSMAX, 69,  48, FIXED );

	sprintf( tstring, "BEP=%3lu", BeakEngagePulseCount ); // added l
	display_f1_text( tstring, TSMAX, 69,  58, FIXED );
#endif

#if 0 //#ifdef DEVELOP // Testing pulse counting -- Incl. testing Pre Net Pulses & Beak Engage Pulses (on IP4)
	sprintf( tstring, "  NPC=%4lu", NetPulseCount );
	display_f1_text( tstring, TSMAX, 49,  8, FIXED );

	sprintf( tstring, " SNPC=%4lu", Saved_NetPulseCount );
	display_f1_text( tstring, TSMAX, 49,  18, FIXED );

	sprintf( tstring, " TNPC=%4u", TargetNetPulses );
	display_f1_text( tstring, TSMAX, 49,  28, FIXED );

	sprintf( tstring, "SPNPC=%4lu", SavedPreNetPulseCount );
	display_f1_text( tstring, TSMAX, 49,  38, FIXED );

	sprintf( tstring, "TPNPC=%4lu", TargetPreNetPulseCount );
	display_f1_text( tstring, TSMAX, 49,  48, FIXED );

	sprintf( tstring, " BEPC=%4lu", BeakEngagePulseCount );
	display_f1_text( tstring, TSMAX, 49,  58, FIXED );

	sprintf( tstring, "SBEPC=%4lu", SavedBeakEngagePulseCount );
	display_f1_text( tstring, TSMAX, 49,  68, FIXED );

	sprintf( tstring, "TBEPC=%4lu", (unsigned long int)pFac.pf.GW302_BeakEngage );
	display_f1_text( tstring, TSMAX, 49,  78, FIXED );
#endif

#ifdef DEVELOP // Testing state machine
	sprintf( tstring, "ST=%02X", Stage );
	display_f1_text( tstring, TSMAX, 3, 58, FIXED );

	sprintf( tstring, "SC=%02X", stagecomp );
	display_f1_text( tstring, TSMAX, 3, 68, FIXED );
#endif

}
// -----------------------------------------------------------

unsigned char TwineAlarmAck;
// -----------------------------------------------------------
// Function Name       : CheckAlarms
// -----------------------------------------------------------
void CheckAlarms( void )
{

	static unsigned char tempCountTime;
	//CAN module offline
	if(pFac.pf.GW302_NetTwineOption == TO_TWINE)
	{
                  #ifndef WINDOWS
		if(!(HBM[0].status & CAN_MODULE_PRESENT))
		{
			if( !(MWarn & MW_NO_CAN_COM))
			{
				MWarn |= MW_NO_CAN_COM;
				OKWarnBox = EXCL;

				// Sounding 1 beep when message first appears
				buzz_count = 1;
				buzz_length = 250;
			}

		}
		else
		{
			if((MWarn & MW_NO_CAN_COM))
			{
				MWarn &= ~MW_NO_CAN_COM;
				OKWarnBox &= ~ EXCL;


			}
		}
                #endif
	}


	//If during an automatic twine sequence there are no pulses from one of the twine feed sensors (HBM-IP1 or HBM-IP2) for ‘No Twine Warning ’ time then display the ‘Single Twine Warning’ (2103).

	if(( (Stage == TWINE_STAGE_4) || (Stage == TWINE_STAGE_5) || (Stage == TWINE_STAGE_6) || (Stage == TWINE_STAGE_7) || (Stage == TWINE_STAGE_8) || (Stage == TWINE_STAGE_9) || (Stage == TWINE_STAGE_10) || (Stage == TWINE_STAGE_11)) && !AutoTwineCutButtonPressed)
	{

		if(!HBM[0].HBM_Input[1].Frequency && !HBM[0].HBM_Input[0].Frequency)
		{

			tempCountTime++;



			if(tempCountTime >= pFac.pf.NoTwineWarningTime*10)
			{

				if( (MWarn & MW_SINGLE_NO_TWINE))
				{
					//cancel single twine warning
					MWarn &= ~MW_SINGLE_NO_TWINE;
					OKWarnBox &= ~ EXCL;

					TwineAlarmAck = 0;
				}
				else if((!(MWarn & MW_SINGLE_NO_TWINE) && TwineAlarmAck && !AutoTwineCutButtonPressed && HBM[0].HBM_Input[2].Level_Now) || ((Stage == TWINE_STAGE_3) && HBM[0].HBM_Output[0].Duty[0] == HBM_OUT_ON))
				{
					TwineAlarmAck = 0;
				}


				if( !(MWarn & MW_DOUBLE_NO_TWINE) && !TwineAlarmAck)
				{

					MWarn |= MW_DOUBLE_NO_TWINE;
					OKWarnBox = EXCL;

					// Sounding 1 beep when message first appears
					buzz_count = 1;
					buzz_length = 250;

					if(TwineAutoManMode)
					{

						AutoTwineCutButtonPressed = 1;
						FwdOvershootPulses = 0;
						Stage = TWINE_START;
					}

				}
			}

		}
		else if(!HBM[0].HBM_Input[1].Frequency || !HBM[0].HBM_Input[0].Frequency)
		{

			tempCountTime++;



			if(tempCountTime >= pFac.pf.NoTwineWarningTime*10)
			{

				if( !(MWarn & MW_SINGLE_NO_TWINE) && !TwineAlarmAck)
				{
					MWarn |= MW_SINGLE_NO_TWINE;
					OKWarnBox = EXCL;

					// Sounding 1 beep when message first appears
					buzz_count = 1;
					buzz_length = 250;
				}

				if( (MWarn & MW_DOUBLE_NO_TWINE))
				{
					//cancel double twine warning
					MWarn &= ~MW_DOUBLE_NO_TWINE;
					OKWarnBox &= ~ EXCL;
					tempCountTime = 0;
				}
			}
		}

	}
	else
	{
		TwineAlarmAck = 0;
		tempCountTime = 0;
	}

	// -----------------------------------------------------------
	// Density Pressure Too High Warning
	if( DensityPresVolts > DPV_DENSITY_PRESSURE_TOO_HIGH_VOLTS )
	{
		if( !(MWarn & MW_DENSITYPRESS) )
		{
			MWarn |= MW_DENSITYPRESS;
			OKWarnBox = EXCL;

			// Sounding 1 beep when message first appears
			buzz_count = 1;
			buzz_length = 250;

			DensityPressureTooHigh = 1;
		}

		// Sounding warning every second
		if( !half_second )
		{
			buzz_count = 1;
			buzz_length = 250;
		}
	}
	else if( DensityPresVolts < (DPV_DENSITY_PRESSURE_TOO_HIGH_VOLTS - DPV_DENSITY_PRESSURE_TOO_HIGH_VOLTS_HYSTERESIS) )
	{
		if( MWarn & MW_DENSITYPRESS )
		{
			MWarn &= ~MW_DENSITYPRESS;
			OKWarnBox &= ~EXCL;

			DensityPressureTooHigh = 0;
		}
	}


	// -----------------------------------------------------------
	// Check Net Error (AKA Net Warning)

	if(pFac.pf.GW302_NetTwineOption == TO_NET) /**/
	{
		if( NetWarningOn )
		{
			if( !(MWarn & MW_CHECKNET) )
			{
				MWarn |= MW_CHECKNET;
				OKWarnBox = EXCL;

				// Sounding 1 beep when message first appears
				buzz_count = 1;
				buzz_length = 250;
			}
		}
		else // !NetWarningOn
		{
			if( MWarn & MW_CHECKNET )
			{
				MWarn &= ~MW_CHECKNET;
				OKWarnBox &= ~EXCL;
			}
		}
	}

	// -----------------------------------------------------------
	// Drop Floor (AKA Floor Position) Warning

	if( !FLOOR_WORKING_POS )
	{
		if( !(MWarn & MW_DROPFLOOR) )
		{
			MWarn |= MW_DROPFLOOR;
			OKWarnBox = EXCL;

			// Sounding 1 beep when message first appears
			buzz_count = 1;
			buzz_length = 250;

		}
	}
	else  // Floor (Position) Sensor closed
	{
		if( MWarn & MW_DROPFLOOR )
		{
			MWarn &= ~MW_DROPFLOOR;
			OKWarnBox &= ~EXCL;
		}
	}


	// -----------------------------------------------------------
	// Knife Maximum Pressure Warning
	// Inhibited when:
	// - 'Chopping Knives Fitted' set to disabled -- Chopping Knives not fitted
	// - Outside Right (OR) button pressed while first screen displayed

	if( pFac.pf.GW302_ChoppingKnivesFitted == CKF_ENABLED ) // 'Chopping Knives Fitted' set to enabled
	{
		if( (KnifePresVolts > pFac.pf.GW302_KnifePresWarn) &&
				( ((key & ~(SW_NEW | SW_REP)) != SW_OR) || (SecondScreen == 1) ) )
		{
			if( !(MWarn & MW_KNIFEPRES) )
			{
				//buzz_count = 1;
				swrel = 0; /* search for if( looptime > 3 ) in screen.c */
				MWarn |= MW_KNIFEPRES;
				OKWarnBox = EXCL;
			}

			if( !(swrel % 30) )
			{
				//buzz_count = 1;  // 100ms beep every 3 seconds.
				if( !DelayTime )  // When other beeps are going on
				{
					buzz_length = 250;
				}
			}
		}
		else // (KnifePresVolts <= pFac.pf.GW302_KnifePresWarn) or OR button pressed
		{
			if( MWarn & MW_KNIFEPRES )
			{
				MWarn &= ~MW_KNIFEPRES;
				OKWarnBox &= ~EXCL;
			}
		}
	}


	// -----------------------------------------------------------
	// Chopping knives position Warning

	if( DisableChoppingKnivesWarnings ) // Disabling prevents chopping knives position warning from coming on
	{
		if( MWarn & MW_CHOPKNIVPOS1 )
		{
			// Both warnings disabled
			MWarn &= ~(MW_CHOPKNIVPOS1);
			OKWarnBox &= ~EXCL;
		}

		// Chopping knives position Warning turned off
		ChoppingKnives1WarningOn = 0;
	}
	else if( ChoppingKnives1WarningOn ) // Chopping knives position Warning on
	{
		if( ChoppingKnives1WarningOn )
		{
			if( !(MWarn & MW_CHOPKNIVPOS1) )
			{
				MWarn |= MW_CHOPKNIVPOS1;
				OKWarnBox = EXCL;

				// Sounding 1 beep when message first appears
				buzz_count = 1;
				buzz_length = 250;
			}
		}
	}


	// -----------------------------------------------------------
	// Low Battery (AKA Low voltage) Warning

	if( SupplyVolts < 10.5 )
	{
		if( !(MWarn & MW_LOWBAT) )
		{
			MWarn |= MW_LOWBAT;
			OKWarnBox = EXCL;

			// Sounding 1 beep when message first appears
			buzz_count = 1;
			buzz_length = 250;
		}
	}
	else // SupplyVolts >= 10.5
	{
		if( MWarn & MW_LOWBAT )
		{
			MWarn &= ~MW_LOWBAT;
			OKWarnBox &= ~EXCL;
		}
	}


	// -----------------------------------------------------------
	// Lube Count Alarm

	if( pRun.pr.LubeCounter <= 0 )
	{
		if( !MWarn && !(OKWarnBox & LUBE) ) // Lube count warning has lowest priority
		{
			// Oil can icon shown instead of showing total net applied in metres in screens.c

			OKWarnBox = LUBE;

			// Sounding 1 beep when message first appears
			buzz_count = 1;
			buzz_length = 250;

			BeepDelay = 0;
		}

		// Beeping sounds continuously
		if( BeepDelay )
		{
			BeepDelay--;
		}
		else
		{
			BeepDelay = 29;
			buzz_count = 1;
		}
	}
	else // pRun.pr.LubeCounter > 0
	{
		if( OKWarnBox == LUBE )
		{
			OKWarnBox &= ~LUBE;
			draw_box( adm, 2+2, 11+2, 30+2, 32+2, CLEAR );
		}
	}

}
// -----------------------------------------------------------
#define START_AUTO_TWINE 0
#define OP5_AUTO_TWINE 1


#define OP05 0
#define HBM_OP01 1
#define TWINE_DELAY 2

unsigned char TwineStage;

unsigned short int FordwardPulses, TwinePulses, ReversePulses, ReversePulsesBetweenStops, TargetEdgePulses, TargetCentrePulses;

// -----------------------------------------------------------
// Function Name       : Twine Sequence
// -----------------------------------------------------------
void TwineSequence(void)
{


	static unsigned char HBMInputsCounter, TwineStopTime, NumberOfStops;
	unsigned int i;


	static unsigned char TwineCounters[4], TrackDelay;
	static unsigned short int TempIP4PulseCouner;

	static unsigned short int IP04PulsesReminder;



	struct CAN_HBM *ptMod;
	struct HBM_Input_struct *ptInput;

	ptMod = &HBM[0];


	if(TwineAutoManMode)
	{

		if(  ( !NettingStartBeepsDisabledWhileIP3OpenInAutoMode )
					  && ( BaleDiaVolts > DiaPreWarnVolts ) // Pre warning beep bale diameter exceeded
					  && ( pFac.pf.GW302_PreWarningBeep > 0 ) // Pre Warning beep on (set to non-zero value)
					  && ( !pre_warning_cont_beep ) )
		{
			buzz_length = PRE_WARNING_BEEP; // Pre Warning beep
			pre_warning_cont_beep = 1;      // Only repeated after a sequence is completed

		}

		switch(TwineStage)
		{
			case START_AUTO_TWINE:
			case 12:

				TwineCounters[OP05] = 0;
				TwineCounters[HBM_OP01] = 0;
				HBMInputsCounter = 0;
				NumberOfStops = 0;
				TempIP4PulseCouner = 0;
				HBM[0].HBM_Input[3].Pulse_Accumulator = 0;


				if((TwineStage == START_AUTO_TWINE) && ((pFac.pf.GW302_TwineOption == TO_ON) && (pFac.pf.GW302_NetTwineOption == TO_TWINE)))
				{
					if(!HBM[0].HBM_Input[2].Level_Now)
					{
						TwineStage = 1;
					}
				}
				else if(TwineStage == 12)// run the sequence when SW_IL was pressed on the main screen
				{
					TwineStage = 13;
					HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_OFF; //Feed Motor
					HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF; //Track Motor Forward
					HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF; //Track Motor Reverse


				}

			break;

			case OP5_AUTO_TWINE:
			case 13:


				if(TwineStage == OP5_AUTO_TWINE)
				{
					if( BaleDiaVolts > TwineDiaEngageVolts) //When the bale size is greater than ‘Engage Diameter’ switch on Gwiz-OP5 for 3 seconds to engage the roller.
					{
						if(!TwineCounters[OP05])
						{
							TwineCounters[OP05] = 30; // 30 x 100 ms = 3 seconds
							SeqOpOn |= OP_ROLL_ENGAGE; // OP5 ON: Roller Engage Solenoid (AKA Pre Net Position)
							SeqOpOff &= ~OP_ROLL_ENGAGE;
							TwineStage = 2; // now check the HBM_OP1
						}

					}
				}
				else if(TwineStage == 13)
				{
					if(!TwineCounters[OP05])
					{
						TwineCounters[OP05] = 30; // 30 x 100 ms = 3 seconds
						SeqOpOn |= OP_ROLL_ENGAGE; // OP5 ON: Roller Engage Solenoid (AKA Pre Net Position)
						SeqOpOff &= ~OP_ROLL_ENGAGE;
						TwineStage = 14; // now check the HBM_OP1
					}
				}
			break;

			case 2: //in this state HBM-OP1 is on
			case 14:


				if(TwineStage == 2)
				{
					if( BaleDiaVolts > DiaFeedVolts) //When the bale size is greater than ‘Feed Diameter’ switch on HBM-OP1 to feed twine into the baler. HBM-OP1 stays on for twine feed time and switches off.
					{

						if(!TwineCounters[HBM_OP01])
						{
							TwineCounters[HBM_OP01] = (pFac.pf.GW302_TwineFeedTime*10);
							TwineStage = 3;
						}

					}
				}
				else if(TwineStage == 14)
				{
					if(!TwineCounters[OP05]) //As soon as OP5 switches off then switch on HBM-OP1 to feed in the twine.
					{
						if(!TwineCounters[HBM_OP01])
						{
							TwineCounters[HBM_OP01] = (pFac.pf.GW302_TwineFeedTime*10);
							TwineStage = 3;
						}
					}
				}
			break;


			case 3:

					TwineCounters[TWINE_DELAY] = (unsigned char)(pFac.pf.GW302_NetDelay * 10.0); //As soon as HBM-OP1 switches on then start ‘Twine Delay’ timer. When ‘Twine Delay’ timer elapses sound the bale full beeper.
					TwineStage = 4;

			break;

			case 4: // As soon as HBM-OP1 switches on then monitor HBM-IP1 and HBM-IP2. When the speeds of both HBM-IP1 and HBM-IP2 are greater than ‘Twine Feed Speed’ for 1 second then start the ‘Track Delay’ timer.

				if(((float)HBM[0].HBM_Input[0].Frequency > pFac.pf.GW302_TwineFeedSpeed) &&((float)HBM[0].HBM_Input[1].Frequency> pFac.pf.GW302_TwineFeedSpeed))
				{
					if(HBMInputsCounter>9) //1 second
					{
						TwineStage = 5;
						TrackDelay = (pFac.pf.GW302_TwineTrackDelay*10);
					}
					else
						HBMInputsCounter++;
				}

			break;

			case 5:

				if(TrackDelay)
					TrackDelay--;
				else
					TwineStage = 6;

				break;

			case 6: //When ‘Track-Delay’ timer elapses then switch on HBM-OP2 to move the track forward.

				HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_ON;
				HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_ON;

				//If HBM-OP3 switches on to reverse the track and there are no pulses seen on HBM-IP4 for 1s then switch off HBM-OP3 and cancel the twine auto sequence.
				if(HBM[0].HBM_Input[3].Frequency == 0)
				{
					CheckIP4Pulses++;
				}
				else
				{
					CheckIP4Pulses = 0;
				}

				//1 second
				if(CheckIP4Pulses == 10)
				{
					TwineStage = 11;
					CheckIP4Pulses = 0;
					HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF;
				}

				//Forward Pulses - (Track Max Fwd + Min Edge Dist - Operator edge Dist)/Track Dist/Puls
				FordwardPulses = (pFac.pf.GW302_TrackMaxFwd + pFac.pf.GW302_TwineMinimumEdgeDistance - (pFac.pf.TwineDistanceFromEdge*10))/pFac.pf.GW302_TrackDistancePulse;

				if(HBM[0].HBM_Input[3].Pulse_Accumulator >= FordwardPulses)
				{

					HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF;
					TwineStage = 7;

					//Reset Pule counters for HBM-IP1 and HBM-IP2
					HBM[0].HBM_Input[0].Pulse_Accumulator = 0;
					HBM[0].HBM_Input[1].Pulse_Accumulator = 0;
				}



				break;

			case 7: //As soon as HBM-OP2 switches off start a new counter on both HBM-IP1 and HBM-IP2. Theses counters will be used to determine how much twine is going around the edge of the bale.

				/*TwinePulses = (Bale Diameter x PI x Operator Set No of Revolutions)/Twine Leng/Puls*/

				TwinePulses = (pFac.pf.GW302_BaleDiameter[pFac.pf.GW302_BaleProfile] * 3.14 * pFac.pf.TwineNumberOfRevAroundEdge)/(pFac.pf.GW302_TwineLenghtPulse/10);

				TargetEdgePulses = TwinePulses;

				if((HBM[0].HBM_Input[0].Pulse_Accumulator>=TwinePulses) && (HBM[0].HBM_Input[1].Pulse_Accumulator>=TwinePulses))
				{
					TwineStage = 8;
					HBM[0].HBM_Input[3].Pulse_Accumulator = 0;
					IP04PulsesReminder = 0;
				}


				break;

			case 8: //When the required number of revolutions of twine around the edge of the bale is counted (twine pulses) then switch on HBM-OP3 to reverse the track.


				ptInput = &(ptMod->HBM_Input[3]);


				ReversePulses = ((pFac.pf.GW302_TrackMaxFwd + pFac.pf.GW302_TwineMinimumEdgeDistance - (pFac.pf.TwineDistanceFromEdge*10))+ pFac.pf.GW302_TrackMaxRev)/(pFac.pf.GW302_TrackDistancePulse);

				ReversePulsesBetweenStops = ReversePulses/(pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre+1);

				if((NumberOfStops <= (pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre))) // && (TempIP4PulseCouner<ReversePulses)
				{

					if((NumberOfStops == (pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre)) && !IP04PulsesReminder)
					{
						IP04PulsesReminder = ReversePulses - HBM[0].HBM_Input[3].Pulse_Accumulator;

						//The reminder is bigger than the the rest previous stops
						if(IP04PulsesReminder !=ReversePulsesBetweenStops)
						{
							ReversePulsesBetweenStops = IP04PulsesReminder;
						}
					}


					if((HBM[0].HBM_Input[3].Pulse_Accumulator< (ReversePulsesBetweenStops*(NumberOfStops+1))))
					{
						HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_ON;

						//If HBM-OP3 switches on to reverse the track and there are no pulses seen on HBM-IP4 for 1s then switch off HBM-OP3 and cancel the twine auto sequence.
						if(HBM[0].HBM_Input[3].Frequency == 0)
						{
							CheckIP4Pulses++;
						}
						else
						{
							CheckIP4Pulses = 0;
						}


						//1 second
						if(CheckIP4Pulses == 10)
						{
							TwineStage = 11;
							CheckIP4Pulses = 0;
							HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF;
						}

						stopPulseAccu = 0;

					} //First stop
					else if(((HBM[0].HBM_Input[3].Pulse_Accumulator >= (ReversePulsesBetweenStops*(NumberOfStops+1))) && (NumberOfStops < (pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre))) || ((NumberOfStops == pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre) && (HBM[0].HBM_Input[3].Pulse_Accumulator >=ReversePulses)))
					{

						stopPulseAccu = 1;
						if(!TwineStopTime)
							TwineStopTime = (pFac.pf.TwineStopTime * 10);

						if(TwineStopTime)
						{
							HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF;
							TwineStopTime--;

							if(!TwineStopTime) //reset counter
							{
								NumberOfStops++;
								TempIP4PulseCouner = TempIP4PulseCouner + ptInput->Pulse_Accumulator;

							}
						}
					}

				}
				else
				{
					TwineStage = 9;
					HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF;
					//Reset Pule counters for HBM-IP1 and HBM-IP2
					HBM[0].HBM_Input[0].Pulse_Accumulator = 0;
					HBM[0].HBM_Input[1].Pulse_Accumulator = 0;
					stopPulseAccu = 0;
				}
				break;


				case 9: //The target twine pulses are counted on HBM-IP1 and HBM-IP2 (The counter on both inputs has to be reach the target for the correct amount of twine to be applied to both sides of the bale).

					//here we use the RevoultionsAroundCentre value to calculate Twine Pulses
					TwinePulses = (pFac.pf.GW302_BaleDiameter[pFac.pf.GW302_BaleProfile] * 3.14 * pFac.pf.TwineNumberOfRevolutionsAroundCentre)/(pFac.pf.GW302_TwineLenghtPulse/10);
					TargetCentrePulses = TwinePulses;
					if((HBM[0].HBM_Input[0].Pulse_Accumulator>=TwinePulses) && (HBM[0].HBM_Input[1].Pulse_Accumulator>=TwinePulses))
					{
						TwineStage = 10;

					}
				break;

				case 10: //	HBM-OP2 switches on to drive the track forwards towards the home position. HBM-OP2 switches off when HBM-IP3 closes.

					if((HBM[0].HBM_Input[2].Level_Now))
					{
						HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_ON; //switch HBM_OP2 On
					}
					else
					{
						HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF; //switch HBM_OP2 Off
						buzz_length = 250;
						TwineStage = 11;

					}
				break;

				case 11: //do nothing
					break;




		}






		//Check if any counters are set in order to turn any of the outputs
		for(i=0; i<4;i++)
		{
			if(TwineCounters[i])
			{
				if(i == HBM_OP01)
					HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_ON;


				if((i == TWINE_DELAY)) //When ‘Twine Delay’ timer elapses sound the bale full beeper.
				{

					// Intermittent beep for duration of Net Delay time (t1)
					if(TwineCounters[TWINE_DELAY] != 1)
						buzz_count = 1;


					if(TwineCounters[TWINE_DELAY] == 1)
					{

							buzz_length = NETTING_START_BEEP; // Long continuous beep when netting starts in AUTO mode

					}
				}


				TwineCounters[i]--;

			}
			else
			{

				if(!TwineCounters[OP05])
				{
					SeqOpOn &= ~OP_ROLL_ENGAGE; // OP5 OFF: Roller Engage Solenoid (AKA Pre Net Position)
					SeqOpOff |= OP_ROLL_ENGAGE;
				}
				if(!TwineCounters[HBM_OP01])
				{
					HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_OFF;
				}


			}
		}


	}
}
unsigned short int FwdOvershootPulses;
unsigned char TwineSeqStart;
// -----------------------------------------------------------
// Function Name       : Sequence
// -----------------------------------------------------------
void Sequence( void )
{

	static unsigned char Kniv1_IPOpenTime;
	static unsigned long int LastNetPulseCount;

	static unsigned char OP5ManTurnOff;


	int i;
	unsigned char tempCh;


	static unsigned char HBMInputsCounter, TwineStopTime, NumberOfStops;

	static unsigned char TwineCounters[4], TrackDelay;
	static unsigned short int TempIP4PulseCouner;

	static unsigned short int IP04PulsesReminder;

	static unsigned char CheckIP4Pulses, tempVar2, PreTwineStage, PreTwineSeqRunning;

	struct CAN_HBM *ptMod;
	struct HBM_Input_struct *ptInput;
	static float LastAnIP1Voltage;

	static unsigned int k, HBMOP1Beep;

	unsigned short int PreTwineRevPulses;

	ptMod = &HBM[0];



	//if( AllTime ) // Incremented every 100 msec in TC1_irq_handler; so this routine is executed once every 100msec
	//{
		//AllTime--;

		if( AutoMan || TwineAutoManMode)
		{

			// -----------------------------------------------------------
			// State machine:
			// - only runs in AUTO mode -- this mode cannot be selected if chamber closed

			if(TwineAutoManMode && (!Stage) && (pFac.pf.GW302_NetTwineOption == TO_TWINE) )
				Stage = TWINE_STAGE_12;



			switch( Stage )
			{

			// -----------------------------------------------------------
			// State: 0

			case STAGE_START:

				if( StartAutoNet ) // Start Netting button pressed
				{
					Stage = STAGE_NETTING_START; // Go straight to Netting stage
				}
				else // Go to Formation stage
				{
					SequenceInProgress = 1;


					SeqOpOff = 0; //this will prevent OP2 switching off

					OP8OnDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
					OP8OffDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
					Stage = STAGE_FORMATION; // Go to Formation stage


				}
				break;


			// -----------------------------------------------------------
			// State: 1

			case STAGE_FORMATION:

				if( StartAutoNet ) // Start Netting button pressed
				{
					Stage = STAGE_NETTING_START; // Go straight to Netting stage
				}
				else // Continuing Formation stage
				{
					SequenceInProgress = 1;

					if( BaleDiaVolts > DiaFormationVolts )  // Formation diameter exceeded
					{
						OP8OnDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
						OP8OffDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
						Stage = STAGE_FORMATION2; // Go to Formation2 stage
					}
					else  // Formation diameter not yet reached
					{
						// -----------------------------------------------------------
						// Density Control Function -- based on Formation Density

						if( DensityPresVolts >= DensityFormationVolts ) // Target Formation density reached/exceeded
						{
							if( OP8OnDelayTime )
								OP8OnDelayTime--;
							else // Pres. Sw. delay has expired
							{
								SeqOpOn |= OP_DENSITY_REL; // OP8 ON: Density Release Solenoid (AKA Density Control)
								OP8OffDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
							}
						}
						else if( DensityPresVolts < (DensityFormationVolts - pFac.pf.GW302_PresDeadband) ) // Fallen below deadband below target Formation density
						{
							if( OP8OffDelayTime )
								OP8OffDelayTime--;
							else // Pres. Sw. delay has expired
							{
								AllSeqOpOff(); // All OPs OFF
								OP8OnDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
							}
						}
					}
				}
				break;


			// -----------------------------------------------------------
			// State: 2

			case STAGE_FORMATION2:

				if( StartAutoNet ) // Start Netting button pressed
				{
					Stage = STAGE_NETTING_START; // Go straight to Netting stage
				}
				else // Continuing Formation2 stage
				{
					SequenceInProgress = 1;

					if( BaleDiaVolts > DiaFormation2Volts )  // Formation2 diameter exceeded
					{
						OP8OnDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
						OP8OffDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
						Stage = STAGE_CORE; // Go to Core stage
					}
					else  // Formation2 diameter not yet reached
					{
						// -----------------------------------------------------------
						// Density Control Function -- based on Formation2 Density

						if( DensityPresVolts >= DensityFormation2Volts ) // Target Formation2 density reached/exceeded
						{
							if( OP8OnDelayTime )
								OP8OnDelayTime--;
							else // Pres. Sw. delay has expired
							{
								SeqOpOn |= OP_DENSITY_REL; // OP8 ON: Density Release Solenoid (AKA Density Control)
								OP8OffDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
							}
						}
						else if( DensityPresVolts < (DensityFormation2Volts - pFac.pf.GW302_PresDeadband) ) // Fallen below deadband below target Formation2 density
						{
							if( OP8OffDelayTime )
								OP8OffDelayTime--;
							else // Pres. Sw. delay has expired
							{
								AllSeqOpOff(); // All OPs OFF
								OP8OnDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
							}
						}
					}
				}
				break;


			// -----------------------------------------------------------
			// State: 3

			case STAGE_CORE:

				if( StartAutoNet ) // Start Netting button pressed
				{
					Stage = STAGE_NETTING_START; // Go straight to Netting stage
				}
				else // Continuing Core stage
				{
					SequenceInProgress = 1;

					if( BaleDiaVolts > DiaCoreVolts )  // Core diameter exceeded
					{
						OP8OnDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
						OP8OffDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
						Stage = STAGE_MAIN; // Go to Main stage
					}
					else  // Core diameter not yet reached
					{
						// -----------------------------------------------------------
						// Density Control Function -- based on Bale Core Density

						if( DensityPresVolts >= DensityCoreVolts ) // Target core density reached/exceeded
						{
							if( OP8OnDelayTime )
								OP8OnDelayTime--;
							else // Pres. Sw. delay has expired
							{
								SeqOpOn |= OP_DENSITY_REL; // OP8 ON: Density Release Solenoid (AKA Density Control)
								OP8OffDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
							}
						}
						else if( DensityPresVolts < (DensityCoreVolts - pFac.pf.GW302_PresDeadband) ) // Fallen below deadband below target core density
						{
							if( OP8OffDelayTime )
								OP8OffDelayTime--;
							else // Pres. Sw. delay has expired
							{
								AllSeqOpOff(); // All OPs OFF
								OP8OnDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
							}
						}
					}
				}

				OP5ManTurnOff = 1;
				break;


			// -----------------------------------------------------------
			// State: 4

			case STAGE_MAIN:

				if( StartAutoNet ) // Start Netting button pressed
				{
					Stage = STAGE_NETTING_START; // Go straight to Netting stage
				}
				else // Continuing Main stage
				{
					SequenceInProgress = 1;



					if(CHAMBER_CLOSED)
					{

						if( BaleDiaVolts > DiaTargetVolts )  // Target bale diameter exceeded
						{
							Stage = STAGE_NETTING_START; // Go to Netting stage
						}
						else  // Target bale diameter not yet reached
						{
						// Density Control Function (based on Target Bale Density) activated -- turned on
							DensityControlFunctionActive = 1;

						}


						// Roller Engage Solenoid (OP5) turned on now/later when Engage diameter reached
						if( BaleDiaVolts >= DiaEngageVolts ) // Engage diameter reached
						{
							if( pFac.pf.GW302_NetSystem == NS_ROLLER )
							{
								if(OP5ManTurnOff)
								{
									RollEngageTurnedOn = 1; // OP5 ON: Roller Engage Solenoid (AKA Pre Net Position)
									OP5ManTurnOff = 0;
								}
							}
							else //  pFac.pf.GW302_NetSystem == NS_BEAK
							{
								// Roller Engage Solenoid (OP5) turned on after 'Beak Engage' pulses counted on IP4
								// - Counting starts from now but checking only starts after OP2 (Net On) has come on
								BeakEngagePulseCount = 0UL; // Resetting IP4 'Beak Engage' pulse count
								OP5TurnedOnAfterBeakEngagePulsesEnabled = 1;
							}
						}

					}
				}
				break;


			// -----------------------------------------------------------
			// State: 5

			case STAGE_NETTING_START:

				if( StartAutoNet ) // Start Netting button pressed
				{
					// Pausing at start of Netting stage while button held

					// -----------------------------------------------------------
					// Roller Engage Solenoid (OP5) turned on now/later while Net On button 2 pressed

					if( pFac.pf.GW302_NetSystem == NS_ROLLER )
					{
						RollEngageTurnedOn = 1; // OP5 ON: Roller Engage Solenoid (AKA Pre Net Position)
					}
					else //  pFac.pf.GW302_NetSystem == NS_BEAK
					{
						// Roller Engage Solenoid (OP5) turned on after 'Beak Engage' pulses counted on IP4
						// - Counting starts from now but checking only starts after OP2 (Net On) has come on
						OP5TurnedOnAfterBeakEngagePulsesEnabled = 1;
					}
				}
				else // Continuing with this stage
				{
					SequenceInProgress = 1;

					// Density Control Function active/not according to Target Bale Density ID
					if( pFac.pf.GW302_BaleDensity[pFac.pf.GW302_BaleProfile] <= 5 ) // Target Bale Density ID is 1 - 5
					{
						DensityControlFunctionActive = 1; // Density Control Function (based on Target Bale Density) active / remains active
					}
					else // Target Bale Density ID is 6 - 10
					{
						DensityControlFunctionActive = 0; // Density Control Function (based on Target Bale Density) not active -- turned off

						// Ensuring OP8 is turned off
						SeqOpOn &= ~OP_DENSITY_REL; // OP8 OFF: Density Release Solenoid (AKA Density Control)
						SeqOpOff |= OP_DENSITY_REL;
					}

					// Next stage
					OP2OnDelayTime = (unsigned char)(pFac.pf.GW302_NetDelay * 10.0);
					Stage = STAGE_NETTING_NET_DELAY; // Go to Net Delay stage
				}
				break;


			// -----------------------------------------------------------
			// State: 6

			case STAGE_NETTING_NET_DELAY:

				if( StartAutoNet ) // Start Netting button pressed
				{
					// Pausing while button held
					OP2OnDelayTime = 0; // Net Delay (t1) set to 0; hence, on key release go to Netting stage -- Net On
				}
				else // Continuing with this stage
				{
					SequenceInProgress = 1;

					// Intermittent beep for duration of Net Delay time (t1) -- unless disabled by IP3 opening
					if( !NettingStartBeepsDisabledWhileIP3OpenInAutoMode )
					{
						// Intermittent beep for duration of Net Delay time (t1)
						buzz_count = 1;
					}

					if( OP2OnDelayTime )
						OP2OnDelayTime--;
					else // Net Delay (t1) has expired
					{
						SeqOpOn |= OP_NET_ON;   // OP2 ON: Net Clutch (AKA Net On)
						stagecomp |= NETFEEDON; // Set NET FEED ON stage completed

						// Roller Engage Solenoid (OP5) turned on after 'Beak Engage' pulses counted on IP4
						// - Counting starts from now but checking only starts after OP2 (Net On) has come on
						BeakEngagePulseCount = 0UL; // Resetting IP4 'Beak Engage' pulse count

#ifdef DEVELOP // Testing pulse counting
						SavedBeakEngagePulseCount = 0UL;  // Resetting
#endif

						// Continuous beep for 3s (when OP2 turns on) -- unless disabled by IP3 opening
						if( !NettingStartBeepsDisabledWhileIP3OpenInAutoMode )
						{
							if( !cont_beep )
							{
								buzz_length = NETTING_START_BEEP; // Long continuous beep when netting starts in AUTO mode
								cont_beep = 1;
							}
						}

						// Calculating Target Net Pulses for current bale diameter (expected to be just above Target Bale Diameter Voltage)
						CalculateTargetNetPulses();

						// Resetting IP4 net pulse count (when OP2 turns on)
						NetPulseCount = 0UL;

						// Start checking for no pulses detected on IP4 for NetWarningDelayTime since
						// OP2 turned on or since the last pulse counted
						if (pFac.pf.GW302_NetTwineOption == TO_NET)
							NetPulseCheckingEnabled = 1;
						LastNetPulseCount = NetPulseCount;
						NetWarningDelayTime = (unsigned char)(pFac.pf.GW302_NoNetPulse * 10.0); // Delay before Check Net Error (AKA Net Warning) appears

						// Next stage
						OP2OffDelayTime = (unsigned char)(pFac.pf.GW302_NetOn * 10.0); // Total amount of time OP2 is to remain on
						Stage = STAGE_NETTING_NET_ON; // Go to Netting stage -- Net On
					}
				}
				break;


			// -----------------------------------------------------------
			// State: 7

			case STAGE_NETTING_NET_ON:

				if( StartAutoNet ) // Start Netting button pressed
				{
					// Pausing while button held
					OP2OffDelayTime = 0;  // Net On (t2) set to 0; hence on key release turn off OP2 & go to STAGE_NETTING_WAIT_REQ_NET_PULSES
					NetWarningOn = 0;     // Check Net Error (AKA Net Warning) turned off -- if on
				}
				else // Continuing with this stage
				{
					SequenceInProgress = 1;

					if( OP2OffDelayTime )
					{
						if( NetPulseFrequency > pFac.pf.GW302_NetSpeed ) // Net Pulse Frequency exceeds Net Speed
						{
							SeqOpOn &= ~OP_NET_ON; // OP2 OFF: Net Clutch (AKA Net On)
							SeqOpOff |= OP_NET_ON;
							OP2OffDelayTime = 0; // Net On (t2) expires
						}
						else // Net Pulse Frequency at/below Net Speed
						{
							OP2OffDelayTime--;
						}
					}
					else // Net On (t2) has expired
					{
						SeqOpOn &= ~OP_NET_ON; // OP2 OFF: Net Clutch (AKA Net On)
						SeqOpOff |= OP_NET_ON;

						Stage = STAGE_NETTING_WAIT_REQ_NET_PULSES; // Go to Netting stage -- waiting for Target Net Pulses
					}
				}
				break;


			// -----------------------------------------------------------
			// State: 8

			case STAGE_NETTING_WAIT_REQ_NET_PULSES:

				if( StartAutoNet ) // Start Netting button pressed
				{
					// Pausing while button held
					// OP2 is turned on & kept on while key held down
					NetWarningOn = 0;     // Check Net Error (AKA Net Warning) turned off -- if on
				}
				else // Continuing with this stage
				{
					SequenceInProgress = 1;

					if(( NetPulseCount > (unsigned long int)TargetNetPulses ) || (pFac.pf.GW302_NetTwineOption == TO_TWINE)) // Target Net Pulses counted on IP4
					{
						// Density Control Function active/not according to Target Bale Density ID
						if( pFac.pf.GW302_BaleDensity[pFac.pf.GW302_BaleProfile] <= 5 ) // Target Bale Density ID is 1 - 5
						{
							DensityControlFunctionActive = 0; // Density Control Function (based on Target Bale Density) not active -- turned off

							// Ensuring OP8 is turned off
							SeqOpOn &= ~OP_DENSITY_REL; // OP8 OFF: Density Release Solenoid (AKA Density Control)
							SeqOpOff |= OP_DENSITY_REL;
						}

						if(pFac.pf.GW302_NetTwineOption == TO_NET)
							SeqOpOn |= OP_NET_CUT; // OP7 ON: Net Cut Motor (AKA Net Cut Trip)

						stagecomp |= NETCUTON; // Set NET CUT ON stage completed

#ifdef DEVELOP // Testing pulse counting
						Saved_NetPulseCount = NetPulseCount;
						SavedPreNetPulseCount = 0UL;   // Resetting
						TargetPreNetPulseCount = (unsigned long int)pFac.pf.GW302_PreNetPulse;
#endif

						// Next stage
						OP7OffDelayTime = (unsigned char)(pFac.pf.GW302_NetCutTime * 10.0);
						IP4PulsesStoppedCount = IP4_PULSES_STOPPED_COUNT_DURATION; // 1.5 s
						NetPulseCountAtLastCheck = NetPulseCount; // Pulse count saved
						Stage = STAGE_NETTING_NET_CUT_ON; // Go to Netting stage -- Net Cut On

						// Stop checking for no pulses detected on IP4 for NetWarningDelayTime
						NetPulseCheckingEnabled = 0;

						// Stop checking for 'Beak Engage' Pulses counted on IP4
						OP5TurnedOnAfterBeakEngagePulsesEnabled = 0;
					}
				}
				break;


			// -----------------------------------------------------------
			// State: 9

			case STAGE_NETTING_NET_CUT_ON:

				SequenceInProgress = 1;

				if( OP7OffDelayTime || IP4PulsesStoppedCount )
				{
					if( OP7OffDelayTime )
					{
						OP7OffDelayTime--;
					}

					if( IP4PulsesStoppedCount )
					{
						if( NetPulseCount == NetPulseCountAtLastCheck ) // Net Pulses cease to be detected on IP4 after Target Net Pulses reached
								{
							IP4PulsesStoppedCount--;
								}
						else // Net Pulses continue to be detected on IP4 after Target Net Pulses reached
						{
							IP4PulsesStoppedCount = IP4_PULSES_STOPPED_COUNT_DURATION; // Resetting to 1.5 s
						}
						NetPulseCountAtLastCheck = NetPulseCount; // Net Pulse count (for what will be the last check) saved
					}
				}

				// Net Cut Time (t5) has expired OR No Net Pulses detected on IP4 for > 1 s OR Net Cut Sensor closed (if fitted)
				if( !OP7OffDelayTime || !IP4PulsesStoppedCount || NET_CUT_SENSOR_CLOSED || (pFac.pf.GW302_NetTwineOption == TO_TWINE))
				{
					RollEngageTurnedOn = 0;     // OP5 Roller Engage Solenoid (AKA Pre Net Position) turned off
					AllSeqOpOff(); // All OPs OFF

					buzz_length = PROMPT_TO_OPEN_DOOR_BEEP; // Beep to prompt operator to open the chamber door & unload the bale

					Stage = STAGE_NETTING_END; // Go to end of Netting stage
				}
				break;


			// -----------------------------------------------------------
			// State: 10 (Hex A)

			case STAGE_NETTING_END:

				SequenceInProgress = 1;

				if(PreTwineSeq && AutoTwineCutButtonPressed)
				{
					if(BaleDiaVolts > PreTwineVoltage)
					{
						Stage = TWINE_START;
						PreTwineSeqActive = 1;
						AutoTwineCutButtonPressed = 0;
					}
				}
				else
					AutoTwineCutButtonPressed = 0;



				if( CHAMBER_OPEN ) // Chamber Locked Sensor open
				{
					stagecomp |= CHAMBOP;      // Set CHAMBER OPEN stage completed

					buzz_length = AUTO_CONFIRM_DOOR_OPENED_BEEP; // Beep to confirm chamber door has been opened

					Stage = STAGE_UNLOADING;   // Go to Unloading stage
				}


				break;


			// -----------------------------------------------------------
			// State: 11 (Hex B)

			case STAGE_UNLOADING:

				SequenceInProgress = 1;

				if( CHAMBER_CLOSED ) // Chamber Locked Sensor closed
				{
					stagecomp |= CHAMBCLO;  // Set CHAMBER CLOSED stage completed

					buzz_length = AUTO_CONFIRM_DOOR_CLOSED_BEEP; // Beep to confirm chamber door has been closed

					// Next stage
					ChamberClosedDelayTime = (unsigned char)(pFac.pf.GW302_DoorClosed * 10.0);
					Stage = STAGE_UNLOADING_WAIT;      // Go to Unloading Wait stage
				}
				else // Chamber Locked Sensor still open
				{
					// Looking for Bale Kicker to go down then up
					if( BALE_KICKER_DOWN ) // IP5: Bale Kicker Sensor open
					{
						BaleKickerStatus |= BKS_BALE_KICKER_DOWN; // Set BKS_BALE_KICKER_DOWN flag
						BaleKickerStatus &= ~(BKS_BALE_KICKER_UP); // Flag to be set next
					}
					else if( BALE_KICKER_UP ) // IP5: Bale Kicker Sensor closed
					{
						if( BaleKickerStatus == BKS_BALE_KICKER_DOWN ) // BKS_BALE_KICKER_DOWN flag already set
						{
							BaleKickerStatus |= BKS_BALE_KICKER_UP; // Set BKS_BALE_KICKER_UP flag

							buzz_length = AUTO_CONFIRM_BALE_UNLOADED_BEEP; // Beep to confirm bale has been unloaded
							// Image of bale shown on the display disappears
						}
					}
				}
				break;


			// -----------------------------------------------------------
			// State: 12 (Hex C)

			case STAGE_UNLOADING_WAIT:

				SequenceInProgress = 1;

				if( ChamberClosedDelayTime )
					ChamberClosedDelayTime--;
				else // Chamber/Door closed delay has expired
				{
					AllSeqOpOff();          // All OPs OFF

					Stage = STAGE_END;      // Go to End stage
				}
				break;


			// -----------------------------------------------------------
			// State: 13 (Hex D)

			case STAGE_END:

				SequenceInProgress = 1;

				if( stagecomp == (NETFEEDON | NETCUTON | CHAMBOP | CHAMBCLO) || TwineAutoManMode) // all stages completed
				{
					stagecomp = SC_CLEAR; // Reset

					UpdatingAndSavingTotalsToMemory();

					buzz_length = ALL_STAGES_COMPLETE_BEEP; // Beep on completion of all stages


					if(TwineAutoManMode)
						Stage = TWINE_START;
				}

				if( (pFac.pf.GW302_PreNetPulse > 0) && (pFac.pf.GW302_NetTwineOption == TO_NET))
				{
					SeqOpOn |= OP_NET_ON;         // OP2 ON: Net Clutch (AKA Net On)

					// Resetting IP4 net pulse count (to count Pre Net Pulses)
					NetPulseCount = 0UL;

					// Start checking for no pulses detected on IP4 for NetWarningDelayTime since
					// start of checking or since the last pulse counted
					NetPulseCheckingEnabled = 1;
					LastNetPulseCount = NetPulseCount;
					NetWarningDelayTime = (unsigned char)(pFac.pf.GW302_NoNetPulse * 10.0); // Delay before Check Net Error (AKA Net Warning) appears

					// Next stage
					Stage = STAGE_PRE_NET_PULSES; // Go to Pre Net Pulses stage
				}
				else // No Pre Net Pulses
				{
					// Finish sequence
					ResetSequenceFactors( SEQ_ALL ); // Turns off all OPs & goes to start of sequence
				}


				if(PreTwineSeq)
				{
					PreTwineSeqActive = 1;
				}
				break;


			// -----------------------------------------------------------
			// State: 14 (Hex E)

			case STAGE_PRE_NET_PULSES:

				SequenceInProgress = 1;

				if( NetPulseCount >= (unsigned long int)pFac.pf.GW302_PreNetPulse ) // Sufficient Pre Net Pulses counted on IP4
				{
#ifdef DEVELOP // Testing pulse counting
					SavedPreNetPulseCount = NetPulseCount;
#endif
					// Stop checking for no pulses detected on IP4 for NetWarningDelayTime
					NetPulseCheckingEnabled = 0;

					// Finish sequence
					ResetSequenceFactors( SEQ_ALL );  // Turns off all OPs & goes to start of sequence

				}



				break;

				//Twine Mode

			case TWINE_START:

				TwineCounters[OP05] = 0;
				TwineCounters[HBM_OP01] = 0;
				HBMInputsCounter = 0;
				NumberOfStops = 0;
				TempIP4PulseCouner = 0;
				HBM[0].HBM_Input[3].Pulse_Accumulator = 0;
				FwdOvershootPulses = 0;

				DensityControlFunctionActive = 1;

				HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_OFF; //Feed Motor
				HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF; //Track Motor Forward
				HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF; //Track Motor Reverse
				HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_OFF; //Track Motor Forward
				CheckIP4Pulses = 0;

				if(AutoTwineCutButtonPressed)
				{
					Stage = TWINE_STAGE_9;
					LastAnIP1Voltage = BaleDiaVolts;

					SeqOpOn &= ~OP_ROLL_ENGAGE; // OP5 OFF: Roller Engage Solenoid (AKA Pre Net Position)
					SeqOpOff |= OP_ROLL_ENGAGE;

					HBM[0].HBM_Input[0].Pulse_Accumulator = 0;

					HBM[0].HBM_Input[1].Pulse_Accumulator = 0;
					HBM[0].HBM_Input[3].Pulse_Accumulator = 0;

				}
				else if((Stage == TWINE_START) && ((pFac.pf.GW302_TwineOption == TO_ON) && (pFac.pf.GW302_NetTwineOption == TO_TWINE)))
				{
					if(!HBM[0].HBM_Input[2].Level_Now)
					{

						//this will prevent the normal twine sequence running when the twine cut sequence finished and AnIP1>100%
						if(LastAnIP1Voltage<DiaEngageVolts)
							Stage = TWINE_OP5;
						else
							Stage = TWINE_STAGE_12;


						LastAnIP1Voltage = 0;

						if(PreTwineSeqActive)
						{
							HBM[0].HBM_Input[0].Pulse_Accumulator = 0;
							HBM[0].HBM_Input[1].Pulse_Accumulator = 0;
							PreTwineStage = 0;
						}
					}
				}

			break;

			case TWINE_OP5:

				if((PreTwineSeq && PreTwineSeqActive && (BaleDiaVolts>=1.00)) || PreTwineSeqRunning)
				{
					//When AnIP1>’Pre Twine Diam’ then HBM-OP1 switches on to feed in twine. When ‘Pre Twine Pulses’ are counted on either HBM-IP1 or HBM-IP2 (whichever happens first) then switch off HBM-OP1.
					if((BaleDiaVolts > PreTwineVoltage) || PreTwineSeqRunning)
					{



						switch(PreTwineStage)
						{
							//When AnIP1>’Pre Twine Diam’ then switch on HBM-OP3 to reverse the track.
							case 0:

									if(pFac.pf.GW302_PreTwineRev)
									{
										HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_ON;
										PreTwineStage = 1;
									}
									else
									{
										PreTwineStage = 2;
										HBM[0].HBM_Input[0].Pulse_Accumulator = 0;
										HBM[0].HBM_Input[1].Pulse_Accumulator = 0;
									}
									HBM[0].HBM_Input[3].Pulse_Accumulator = 0;
									PreTwineSeqRunning = 1;

							break;

							case 1:

								if(HBM[0].HBM_Input[3].Frequency == 0)
								{
									CheckIP4Pulses++;
								}
								else
								{
									CheckIP4Pulses = 0;
								}


								PreTwineRevPulses = pFac.pf.GW302_PreTwineRev/pFac.pf.GW302_TrackDistancePulse;


								//When no pulses are seen HBM-IP4 for ‘Track Protection’ time then switch off HBM-OP3.
								if(CheckIP4Pulses == pFac.pf.GW302_TwineTrackProtection)
								{
									HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF;
									Stage = TWINE_STAGE_12;
									CheckIP4Pulses = 0;
									MWarn |= MW_NO_IP4_PULSES;
									OKWarnBox = EXCL;
									PreTwineSeqRunning = 0;
									// Sounding 1 beep when message first appears
									buzz_count = 1;
									buzz_length = 250;


								}
								//HBM-OP3 only switches on and reverses the track for a set distance
								if(HBM[0].HBM_Input[3].Pulse_Accumulator >= PreTwineRevPulses)
								{
									HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF;
									PreTwineStage = 2;
								}

							break;

							case 2:
								//When ‘Pre Twine Pulses’ are counted on either HBM-IP1 or HBM-IP2 (whichever happens first) then switch off HBM-OP1.
								if((HBM[0].HBM_Input[0].Pulse_Accumulator >= pFac.pf.GW302_PreTwinePulses) || (HBM[0].HBM_Input[1].Pulse_Accumulator >= pFac.pf.GW302_PreTwinePulses) )
								{
									//When HBM-OP1 switches off then switch on HBM-OP2 to move track forward.
									HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_OFF;
									if(pFac.pf.GW302_PreTwineRev)
									{
										HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_ON;
										HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_ON; //Track Motor Forward
										CheckIP4Pulses = 0;
										PreTwineStage = 3;
									}
									else
									{
										PreTwineSeqActive = 0;
										PreTwineStage = 0;
										PreTwineSeqRunning = 0;
										CheckIP4Pulses = 0;
									}


								}
								else
								{
									HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_ON;
								}
							break;

							case 3:
								//When HBM-IP3 closes then switch off HBM-OP2. This completes the Pre Twine Sequence.

								if(HBM[0].HBM_Input[3].Frequency == 0)
								{
									CheckIP4Pulses++;
								}
								else
								{
									CheckIP4Pulses = 0;
								}

								//When no pulses are seen HBM-IP4 for ‘Track Protection’ time then switch off HBM-OP2.
								if(CheckIP4Pulses == pFac.pf.GW302_TwineTrackProtection)
								{
									Stage = TWINE_STAGE_12;
									CheckIP4Pulses = 0;
									HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF;
									HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_OFF;
									MWarn |= MW_NO_IP4_PULSES;
									OKWarnBox = EXCL;
									PreTwineSeqRunning = 0;

									// Sounding 1 beep when message first appears
									buzz_count = 1;
									buzz_length = 250;
								}


								if(!(HBM[0].HBM_Input[2].Level_Now))
								{
									HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF;
									HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_OFF;
									PreTwineSeqActive = 0;
									PreTwineStage = 0;
									PreTwineSeqRunning = 0;
									CheckIP4Pulses = 0;
								}

							break;
						}





					}
					/*
					else if((BaleDiaVolts > DiaFeedVolts))
					{
						PreTwineSeqActive = 0;
						HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_OFF;
						PreTwineStage = 0;
					}
					*/
				}
				else
				{
					if(CHAMBER_CLOSED && !(key & SW_IL))
					{
						if((BaleDiaVolts > TwineDiaEngageVolts) || TwineSeqStart) //When the bale size is greater than ‘Engage Diameter’ switch on Gwiz-OP5 for 3 seconds to engage the roller.
						{
							if(!TwineCounters[OP05])
							{
								TwineCounters[OP05] = 30; // 30 x 100 ms = 3 seconds
								SeqOpOn |= OP_ROLL_ENGAGE; // OP5 ON: Roller Engage Solenoid (AKA Pre Net Position)
								SeqOpOff &= ~OP_ROLL_ENGAGE;
								Stage = TWINE_STAGE_3; // now check the HBM_OP1
								CheckIP4Pulses = 0;

								//Reset counters
								HBM[0].HBM_Input[0].Pulse_Accumulator = 0;
								HBM[0].HBM_Input[1].Pulse_Accumulator = 0;
							}

						}
					}


					if(HBM[0].HBM_Input[3].Frequency == 0)
					{
						CheckIP4Pulses++;
					}
					else
					{
						CheckIP4Pulses = 0;
					}

					//When no pulses are seen HBM-IP4 for ‘Track Protection’ time then switch off HBM-OP2.
					if(CheckIP4Pulses == pFac.pf.GW302_TwineTrackProtection)
					{
						if((HBM[0].HBM_Output[0].Duty[1] == HBM_OUT_ON) ||(HBM[0].HBM_Output[1].Duty[0] == HBM_OUT_ON))
						{
							HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF;
							HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF;
							HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_OFF;

							Stage = TWINE_STAGE_12;
							CheckIP4Pulses = 0;
							MWarn |= MW_NO_IP4_PULSES;
							OKWarnBox = EXCL;

							// Sounding 1 beep when message first appears
							buzz_count = 1;
							buzz_length = 250;

						}
					}


				}

			break;

			case TWINE_STAGE_3: //in this state HBM-OP1 is on

				if(CHAMBER_CLOSED && !(key & SW_IL))
				{

					if(pFac.pf.GW302_PreTwine2Rev) //pre-twine 2 sequence enabled
					{
						if(!TwineCounters[OP05])
						{

							if((HBM[0].HBM_Input[0].Pulse_Accumulator >= pFac.pf.GW302_PreTwine2Rev) || (HBM[0].HBM_Input[1].Pulse_Accumulator >= pFac.pf.GW302_PreTwine2Rev) )
							{
								HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_OFF; //The second pre twine sequence should start as soon as the ‘Engage Time’ elapses and OP2 switches off. When ‘Engage Time’ elapsed and OP2 switches off then HBM-OP1 should switch on to start the second pre feed sequence.

								if((BaleDiaVolts > DiaFeedVolts) || TwineSeqStart) //When the bale size is greater than ‘Feed Diameter’ switch on HBM-OP1 to feed twine into the baler. HBM-OP1 stays on for twine feed time and switches off.
								{

									if(!TwineCounters[HBM_OP01] && !TwineCounters[OP05])
									{
										TwineCounters[HBM_OP01] = (pFac.pf.GW302_TwineFeedTime*10);
										Stage = TWINE_STAGE_4;
										TwineSeqStart = 0;
									}

								}

							}
							else
							{
								HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_ON;
							}
						}
					}
					else //pre-twine 2 sequence disabled
					{
						if((BaleDiaVolts > DiaFeedVolts) || TwineSeqStart) //When the bale size is greater than ‘Feed Diameter’ switch on HBM-OP1 to feed twine into the baler. HBM-OP1 stays on for twine feed time and switches off.
						{

							if(!TwineCounters[HBM_OP01] && !TwineCounters[OP05])
							{
								TwineCounters[HBM_OP01] = (pFac.pf.GW302_TwineFeedTime*10);
								Stage = TWINE_STAGE_4;
								TwineSeqStart = 0;
							}

						}
					}

				}

			break;


			case TWINE_STAGE_4:

					TwineCounters[TWINE_DELAY] = (unsigned char)(pFac.pf.GW302_TwineDelay * 10.0); //As soon as HBM-OP1 switches on then start ‘Twine Delay’ timer. When ‘Twine Delay’ timer elapses sound the bale full beeper.
					HBMOP1Beep = 3;
					Stage = TWINE_STAGE_5;

			break;

			case TWINE_STAGE_5: // As soon as HBM-OP1 switches on then monitor HBM-IP1 and HBM-IP2. When the speeds of both HBM-IP1 and HBM-IP2 are greater than ‘Twine Feed Speed’ for 1 second then start the ‘Track Delay’ timer. •	Monitoring when HBM-IP1 or HBM-IP2  are above ‘Feed Speed’ if a single twine warning is shown

				if((((float)HBM[0].HBM_Input[0].Frequency > pFac.pf.GW302_TwineFeedSpeed) && ((float)HBM[0].HBM_Input[1].Frequency> pFac.pf.GW302_TwineFeedSpeed)) ||  ((MWarn & MW_SINGLE_NO_TWINE)  &&  (((float)HBM[0].HBM_Input[0].Frequency > pFac.pf.GW302_TwineFeedSpeed) || ((float)HBM[0].HBM_Input[1].Frequency> pFac.pf.GW302_TwineFeedSpeed) )))
				{
					if(HBMInputsCounter>9) //1 second
					{
						Stage = TWINE_STAGE_6;
						TrackDelay = (pFac.pf.GW302_TwineTrackDelay*10);
					}
					else
						HBMInputsCounter++;
				}

			break;

			case TWINE_STAGE_6:

				if(TrackDelay)
					TrackDelay--;
				else
				{
					Stage = TWINE_STAGE_7;
					HBM[0].HBM_Input[3].Pulse_Accumulator = 0;
				}

				break;

			case TWINE_STAGE_7: //When ‘Track-Delay’ timer elapses then switch on HBM-OP2 to move the track forward.

				HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_ON;
				HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_ON;

				//If HBM-OP3 switches on to reverse the track and there are no pulses seen on HBM-IP4 for 1s then switch off HBM-OP3 and cancel the twine auto sequence.
				if(HBM[0].HBM_Input[3].Frequency == 0)
				{
					CheckIP4Pulses++;
				}
				else
				{
					CheckIP4Pulses = 0;
				}


				if(CheckIP4Pulses == pFac.pf.GW302_TwineTrackProtection)
				{
					Stage = TWINE_STAGE_12;
					CheckIP4Pulses = 0;
					HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF;
					HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_OFF;
					MWarn |= MW_NO_IP4_PULSES;
					OKWarnBox = EXCL;

					// Sounding 1 beep when message first appears
					buzz_count = 1;
					buzz_length = 250;
				}

				//Forward Pulses - (Track Max Fwd + Min Edge Dist - Operator edge Dist)/Track Dist/Puls
				FordwardPulses = (pFac.pf.GW302_TrackMaxFwd + pFac.pf.GW302_TwineMinimumEdgeDistance - (pFac.pf.TwineDistanceFromEdge*10))/pFac.pf.GW302_TrackDistancePulse;

				if(HBM[0].HBM_Input[3].Pulse_Accumulator >= FordwardPulses)
				{

					HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF;
					HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_OFF;
					Stage = TWINE_STAGE_8;
					CheckIP4Pulses = 0;
					//Reset Pule counters for HBM-IP1 and HBM-IP2
					HBM[0].HBM_Input[0].Pulse_Accumulator = 0;
					HBM[0].HBM_Input[1].Pulse_Accumulator = 0;
				}



				break;

			case TWINE_STAGE_8: //As soon as HBM-OP2 switches off start a new counter on both HBM-IP1 and HBM-IP2. Theses counters will be used to determine how much twine is going around the edge of the bale.

				/*TwinePulses = (Bale Diameter x PI x Operator Set No of Revolutions)/Twine Leng/Puls*/

				TwinePulses = (pFac.pf.GW302_BaleDiameter[pFac.pf.GW302_BaleProfile] * 3.14 * pFac.pf.TwineNumberOfRevAroundEdge)/(pFac.pf.GW302_TwineLenghtPulse/10);

				TargetEdgePulses = TwinePulses;

				if((HBM[0].HBM_Input[0].Pulse_Accumulator>=TwinePulses) || (HBM[0].HBM_Input[1].Pulse_Accumulator>=TwinePulses))
				{

					FwdOvershootPulses = HBM[0].HBM_Input[3].Pulse_Accumulator - FordwardPulses;

					Stage = TWINE_STAGE_9;
					HBM[0].HBM_Input[3].Pulse_Accumulator = 0;
					IP04PulsesReminder = 0;
				}


				break;

			case TWINE_STAGE_9: //When the required number of revolutions of twine around the edge of the bale is counted (twine pulses) then switch on HBM-OP3 to reverse the track.


				ptInput = &(ptMod->HBM_Input[3]);


				//When 'Auto Twine Cut' button is pressed
				if(AutoTwineCutButtonPressed)
				{


					HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_ON;

					//If HBM-OP3 switches on to reverse the track and there are no pulses seen on HBM-IP4 for 1s then switch off HBM-OP3 and cancel the twine auto sequence.
					if(HBM[0].HBM_Input[3].Frequency == 0)
					{
						CheckIP4Pulses++;
					}
					else
					{
						CheckIP4Pulses = 0;
					}


					//1 second
					if(CheckIP4Pulses == pFac.pf.GW302_TwineTrackProtection)
					{
						Stage = TWINE_STAGE_11;

						CheckIP4Pulses = 0;
						HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF;

					}
				}
				else
				{

					ReversePulses = (((pFac.pf.GW302_TrackMaxFwd + pFac.pf.GW302_TwineMinimumEdgeDistance - (pFac.pf.TwineDistanceFromEdge*10))+ pFac.pf.GW302_TrackMaxRev)/(pFac.pf.GW302_TrackDistancePulse))  + FwdOvershootPulses;

					ReversePulsesBetweenStops = ReversePulses/(pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre+1);

					if((NumberOfStops <= (pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre))) // && (TempIP4PulseCouner<ReversePulses)
					{

						if((NumberOfStops == (pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre)) && !IP04PulsesReminder)
						{
							IP04PulsesReminder = ReversePulses - HBM[0].HBM_Input[3].Pulse_Accumulator;

							//The reminder is bigger than the the rest previous stops
							if(IP04PulsesReminder !=ReversePulsesBetweenStops)
							{
								ReversePulsesBetweenStops = IP04PulsesReminder;
							}
						}


						if((HBM[0].HBM_Input[3].Pulse_Accumulator< (ReversePulsesBetweenStops*(NumberOfStops+1))))
						{
							HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_ON;


							//If HBM-OP3 switches on to reverse the track and there are no pulses seen on HBM-IP4 for 1s then switch off HBM-OP3 and cancel the twine auto sequence.
							if(HBM[0].HBM_Input[3].Frequency == 0)
							{
								CheckIP4Pulses++;
							}
							else
							{
								CheckIP4Pulses = 0;
							}


							//1 second
							if(CheckIP4Pulses == pFac.pf.GW302_TwineTrackProtection)
							{
								Stage = TWINE_STAGE_12;
								CheckIP4Pulses = 0;
								HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF;
								MWarn |= MW_NO_IP4_PULSES;
								OKWarnBox = EXCL;

								// Sounding 1 beep when message first appears
								buzz_count = 1;
								buzz_length = 250;
							}

							stopPulseAccu = 0;

						} //First stop
						else if(((HBM[0].HBM_Input[3].Pulse_Accumulator >= (ReversePulsesBetweenStops*(NumberOfStops+1))) && (NumberOfStops < (pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre))) || ((NumberOfStops == pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre) && (HBM[0].HBM_Input[3].Pulse_Accumulator >=ReversePulses)))
						{

							stopPulseAccu = 1;
							if(!TwineStopTime)
								TwineStopTime = (pFac.pf.TwineStopTime * 10);

							if(TwineStopTime)
							{
								HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF;
								TwineStopTime--;

								if(!TwineStopTime) //reset counter
								{
									NumberOfStops++;
									TempIP4PulseCouner = TempIP4PulseCouner + ptInput->Pulse_Accumulator;

								}
							}
						}

					}
					else
					{
						Stage = TWINE_STAGE_10;
						HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF;
						//Reset Pule counters for HBM-IP1 and HBM-IP2
						HBM[0].HBM_Input[0].Pulse_Accumulator = 0;
						HBM[0].HBM_Input[1].Pulse_Accumulator = 0;
						stopPulseAccu = 0;
						CheckIP4Pulses = 0;
					}
				}
				break;


				case TWINE_STAGE_10: //The target twine pulses are counted on HBM-IP1 and HBM-IP2 (The counter on both inputs has to be reach the target for the correct amount of twine to be applied to both sides of the bale).

					//here we use the RevoultionsAroundCentre value to calculate Twine Pulses
					TwinePulses = (pFac.pf.GW302_BaleDiameter[pFac.pf.GW302_BaleProfile] * 3.14 * pFac.pf.TwineNumberOfRevolutionsAroundCentre)/(pFac.pf.GW302_TwineLenghtPulse/10);
					TargetCentrePulses = TwinePulses;
					if((HBM[0].HBM_Input[0].Pulse_Accumulator>=TwinePulses) || (HBM[0].HBM_Input[1].Pulse_Accumulator>=TwinePulses))
					{
						Stage = TWINE_STAGE_11;

					}
				break;

				case TWINE_STAGE_11: //	HBM-OP2 switches on to drive the track forwards towards the home position. HBM-OP2 switches off when HBM-IP3 closes.

					if((HBM[0].HBM_Input[2].Level_Now))
					{
						HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_ON; //switch HBM_OP2 On
						HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_ON;

						if(AutoTwineCutButtonPressed)
						{
							//If HBM-OP2 switches on to reverse the track and there are no pulses seen on HBM-IP4 for 1s then switch off HBM-OP3 and cancel the twine auto sequence.
							if(HBM[0].HBM_Input[3].Frequency == 0)
							{
								CheckIP4Pulses++;
							}
							else
							{
								CheckIP4Pulses = 0;
							}


							//1 second
							if(CheckIP4Pulses == pFac.pf.GW302_TwineTrackProtection)
							{
								Stage = TWINE_STAGE_12;
								CheckIP4Pulses = 0;
								HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF;
								MWarn |= MW_NO_IP4_PULSES;
								OKWarnBox = EXCL;

								// Sounding 1 beep when message first appears
								buzz_count = 1;
								buzz_length = 250;

								TwineCounters[OP05] = 0;
								TwineCounters[HBM_OP01] = 0;
								HBMInputsCounter = 0;
								NumberOfStops = 0;
								TempIP4PulseCouner = 0;
								HBM[0].HBM_Input[3].Pulse_Accumulator = 0;
								FwdOvershootPulses = 0;

								HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_OFF; //Feed Motor
								HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF; //Track Motor Forward
								HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF; //Track Motor Reverse
								HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_OFF;
							}

						}

					}
					else
					{
						HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF; //switch HBM_OP2 Off
						HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_OFF;
						buzz_length = 250;
						Stage = STAGE_NETTING_END;

						if(AutoTwineCutButtonPressed)
						{

							//Stage = TWINE_START;
							Stage = STAGE_NETTING_END;

						}

					}
				break;

				case TWINE_STAGE_12: //do nothing

					if(pFac.pf.GW302_NetTwineOption == TO_NET)
					{
						TwineCounters[OP05] = 0;
						TwineCounters[HBM_OP01] = 0;
						TwineCounters[TWINE_DELAY] = 0;
						MWarn &= ~MW_TRACK_POSITION;
						MWarn &= ~MW_NO_IP4_PULSES;
						MWarn &= ~MW_DOUBLE_NO_TWINE;
						MWarn &= ~MW_SINGLE_NO_TWINE;


					}

					AutoTwineCutButtonPressed = 0;
					break;



			} // end switch( Stage )

			if(TwineAutoManMode)
			{

				if((Stage != TWINE_STAGE_9) && (Stage != TWINE_STAGE_7) && (Stage != TWINE_STAGE_11)&&
						(Stage != TWINE_OP5) && (Stage != TWINE_STAGE_12))
				{
					if((HBM[0].HBM_Output[0].Duty[1] == HBM_OUT_ON) ||(HBM[0].HBM_Output[1].Duty[0] == HBM_OUT_ON))
					{
						if(HBM[0].HBM_Input[3].Frequency == 0)
						{
							CheckIP4Pulses++;
						}
						else
						{
							CheckIP4Pulses = 0;
						}

						//When no pulses are seen HBM-IP4 for ‘Track Protection’ time then switch off HBM-OP2.
						if(CheckIP4Pulses == pFac.pf.GW302_TwineTrackProtection)
						{
							Stage = TWINE_STAGE_12;
							CheckIP4Pulses = 0;
							HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF;
							HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF;
							HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_OFF;
							MWarn |= MW_NO_IP4_PULSES;
							OKWarnBox = EXCL;

							// Sounding 1 beep when message first appears
							buzz_count = 1;
							buzz_length = 250;
						}

					}
				}

				//Check if any counters are set in order to turn any of the outputs
				for(i=0; i<4;i++)
				{
					if(TwineCounters[i])
					{
						if(i == HBM_OP01)
							HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_ON;


						if((i == TWINE_DELAY)) //When ‘Twine Delay’ timer elapses sound the bale full beeper.
						{

							// Intermittent beep for duration of Net Delay time (t1)
							//if(TwineCounters[TWINE_DELAY] != 1)
							//	buzz_count = 1;


							if(TwineCounters[TWINE_DELAY] == 1)
							{



								//HBMOP1Beep = 1;



									//buzz_length = NETTING_START_BEEP; // Long continuous beep when netting starts in AUTO mode

							}
						}


						TwineCounters[i]--;

					}
					else
					{

						if(!TwineCounters[OP05])
						{
							SeqOpOn &= ~OP_ROLL_ENGAGE; // OP5 OFF: Roller Engage Solenoid (AKA Pre Net Position)
							SeqOpOff |= OP_ROLL_ENGAGE;
						}
						if(!TwineCounters[HBM_OP01] && (Stage != TWINE_OP5) && (Stage != TWINE_STAGE_3))
						{
							HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_OFF;
						}
						if(!TwineCounters[TWINE_DELAY] && HBMOP1Beep)
						{
							switch(HBMOP1Beep)
							{
								case 3:
									buzz_count = 5;
									buzz_length = 250;
									HBMOP1Beep = 2;
								break;

								case 2:
									if(!buzz_count)
									{
										buzz_length = NETTING_START_BEEP;
										HBMOP1Beep = 1;
									}
									break;
								case 1:
									if(!buzz_length)
									{
										buzz_count = 1;
										buzz_length = 250;
										HBMOP1Beep = 0;
									}
								break;
							}

						}

					}
				}

			}


			tempIP4Pules = CheckIP4Pulses;

			// -----------------------------------------------------------
			// Density Control Function -- based on Target Bale Density (AUTO)

			if( DensityControlFunctionActive )
			{

				if(TwineAutoManMode && (pFac.pf.GW302_NetTwineOption == TO_TWINE))
				{

					if( BaleDiaVolts > DiaCoreVolts )
					{
						if( DensityPresVolts >= DensityBaleVolts ) // Target bale density reached/exceeded
						{
							if( OP8OnDelayTime )
								OP8OnDelayTime--;
							else // Pres. Sw. delay has expired
							{
								SeqOpOn |= OP_DENSITY_REL; // OP8 ON: Density Release Solenoid (AKA Density Control)
								SeqOpOff &= ~OP_DENSITY_REL;
								OP8OffDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
							}
						}
						else if( DensityPresVolts < (DensityBaleVolts - pFac.pf.GW302_PresDeadband) ) // Fallen below deadband below target bale density
						{
							if( OP8OffDelayTime )
								OP8OffDelayTime--;
							else // Pres. Sw. delay has expired
							{
								SeqOpOn &= ~OP_DENSITY_REL; // OP8 OFF: Density Release Solenoid (AKA Density Control)
								SeqOpOff |= OP_DENSITY_REL;
								OP8OnDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
							}
						}
					}
					else
					{
						if( DensityPresVolts >= DensityCoreVolts ) // Target core density reached/exceeded
						{
							if( OP8OnDelayTime )
								OP8OnDelayTime--;
							else // Pres. Sw. delay has expired
							{
								SeqOpOn |= OP_DENSITY_REL; // OP8 ON: Density Release Solenoid (AKA Density Control)
								SeqOpOff &= ~OP_DENSITY_REL;
								OP8OffDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
							}
						}
						else if( DensityPresVolts < (DensityCoreVolts - pFac.pf.GW302_PresDeadband) ) // Fallen below deadband below target core density
						{
							if( OP8OffDelayTime )
								OP8OffDelayTime--;
							else // Pres. Sw. delay has expired
							{
								SeqOpOn &= ~OP_DENSITY_REL; // OP8 OFF: Density Release Solenoid (AKA Density Control)
								SeqOpOff |= OP_DENSITY_REL;
								OP8OnDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
							}
						}
					}

				}
				else
				{
					if( DensityPresVolts >= DensityBaleVolts ) // Target bale density reached/exceeded
					{
						if( OP8OnDelayTime )
							OP8OnDelayTime--;
						else // Pres. Sw. delay has expired
						{
							SeqOpOn |= OP_DENSITY_REL; // OP8 ON: Density Release Solenoid (AKA Density Control)
							SeqOpOff &= ~OP_DENSITY_REL;
							OP8OffDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
						}
					}
					else if( DensityPresVolts < (DensityBaleVolts - pFac.pf.GW302_PresDeadband) ) // Fallen below deadband below target bale density
					{
						if( OP8OffDelayTime )
							OP8OffDelayTime--;
						else // Pres. Sw. delay has expired
						{
							SeqOpOn &= ~OP_DENSITY_REL; // OP8 OFF: Density Release Solenoid (AKA Density Control)
							SeqOpOff |= OP_DENSITY_REL;
							OP8OnDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
						}
					}
				}
			} // end if( DensityControlFunctionActive )


			// -----------------------------------------------------------
			// Checking for no pulses detected on IP4 for NetWarningDelayTime (AUTO)

			if( (NetPulseCheckingEnabled) && (pFac.pf.GW302_NetTwineOption == TO_NET) ) // Flag set from when OP2 (Net Clutch) turned on up to when OP7 (Net Cut Motor) turned on
			{
				if( (NetPulseCount == 0UL) || (NetPulseCount == LastNetPulseCount) ) // No pulses detected on IP4 for NetWarningDelayTime
				{
					if( NetWarningDelayTime )
					{
						NetWarningDelayTime--;
					}
					else // NetWarningDelayTime expired
					{
						if(pFac.pf.GW302_NetTwineOption == TO_NET) /**/
							NetWarningOn = 1; // Check Net Error (AKA Net Warning) turned on
					}
				}
				else // Pulses detected on IP4
				{
					NetWarningOn = 0; // Check Net Error (AKA Net Warning) turned off
					LastNetPulseCount = NetPulseCount;
					NetWarningDelayTime = (unsigned char)(pFac.pf.GW302_NoNetPulse * 10.0); // Delay before Check Net Error (AKA Net Warning) appears
				}

				// Turning Net Warning off while IP3 is open
				if( NetWarningDisabledWhileIP3OpenInAutoMode )
				{
					NetWarningOn = 0; // Check Net Error (AKA Net Warning) turned off
				}

			} // end if( NetPulseCheckingEnabled )


			// -----------------------------------------------------------
			// Roller Engage Solenoid (OP5) turned on after 'Beak Engage' Pulses counted on IP4 -- Only applies if 'Net System' set to 'Beak' (AUTO)

			if( OP5TurnedOnAfterBeakEngagePulsesEnabled ) // Flag set from when Engage diameter reached (or Net On button 2 pressed) up to when OP7 (Net Cut Motor) turned on
			{
				if( !NetWarningOn ) // Disabled while Check Net Error (AKA Net Warning) active
				{
					if( pFac.pf.GW302_NetSystem == NS_BEAK ) // Only applies if 'Net System' set to 'Beak' (Double checking)
					{
						if( stagecomp == NETFEEDON ) // Only checks for 'Beak Engage' pulses after OP2 (Net On) has come on (Double checking -- Pulses not expected before)
						{
							if( BeakEngagePulseCount >= (unsigned long int)pFac.pf.GW302_BeakEngage ) // 'Beak Engage' Pulses counted on IP4
							{
#ifdef DEVELOP // Testing pulse counting
								if( !RollEngageTurnedOn )
									SavedBeakEngagePulseCount = BeakEngagePulseCount;
#endif
								RollEngageTurnedOn = 1; // OP5 ON: Roller Engage Solenoid (AKA Pre Net Position)
							}
						}
					}
				}
			} // end if( OP5TurnedOnAfterBeakEngagePulsesEnabled )


			// -----------------------------------------------------------
			// Roller Engage Solenoid (OP5) turned on/off (AUTO)

			if( NetWarningOn ) // Check Net Error (AKA Net Warning) active (AUTO)
			{
				SeqOpOn &= ~OP_ROLL_ENGAGE; // OP5 OFF: Roller Engage Solenoid (AKA Pre Net Position)
				SeqOpOff |= OP_ROLL_ENGAGE;
				RollerTimer = 0;
			}
			else if( RollEngageTurnedOn)
			{
				/*if(pFac.pf.GW302_NetTwineOption == TO_TWINE)
				{*/
				if(RollerTimer == 255)
					RollerTimer = OP_ROLL_ENGAGE_TIMER; /*3 seconds*/
				/*}
				else
				{
					SeqOpOn |= OP_ROLL_ENGAGE; // OP5 ON: Roller Engage Solenoid (AKA Pre Net Position)
					SeqOpOff &= ~OP_ROLL_ENGAGE;
				}*/
			}


			// -----------------------------------------------------------
			// Pre Warning beep (AUTO) -- unless disabled by IP3 opening

			if(  ( !NettingStartBeepsDisabledWhileIP3OpenInAutoMode )
						  && ( BaleDiaVolts > DiaPreWarnVolts ) // Pre warning beep bale diameter exceeded
						  && ( pFac.pf.GW302_PreWarningBeep > 0 ) // Pre Warning beep on (set to non-zero value)
						  && ( !pre_warning_cont_beep ) )
			{
				buzz_length = PRE_WARNING_BEEP; // Pre Warning beep
				pre_warning_cont_beep = 1;      // Only repeated after a sequence is completed

			}
			// -----------------------------------------------------------
			// Beeping if chamber open > 1 s outside unloading (IP3 opens & closes) (AUTO)

			if( (Stage != STAGE_NETTING_END)    &&    // Ignoring chamber opening between the start & end of unloading
					(Stage != STAGE_UNLOADING)      &&
					(Stage != TWINE_START)      &&
					(Stage != TWINE_OP5)      &&
					(Stage != TWINE_STAGE_3)      &&
					(Stage != TWINE_STAGE_4)      &&
					(Stage != TWINE_STAGE_5)      &&
					(Stage != TWINE_STAGE_6)      &&
					(Stage != TWINE_STAGE_7)      &&
					(Stage != TWINE_STAGE_8)      &&
					(Stage != TWINE_STAGE_9)      &&
					(Stage != TWINE_STAGE_10)      &&
					(Stage != TWINE_STAGE_11)      &&
					(Stage != TWINE_STAGE_12)      &&
					(Stage != STAGE_UNLOADING_WAIT)    )
			{
				if( CHAMBER_OPEN ) // Chamber Locked Sensor open
				{
					// Beeping
					if( ChamberOpenBeepDelay )
					{
						ChamberOpenBeepDelay--;
					}
					else
					{
						// Intermittent beeping while chamber open
						buzz_length = AUTO_CHAMBER_OPEN_BEEP;
						ChamberOpenBeepDelay = COBD_CHAMBER_OPEN_BEEP_DELAY;  // Reset chamber open beep delay to beep at 1 s intervals
					}
				}
				else // Chamber closed
				{
					ChamberOpenBeepDelay = COBD_CHAMBER_OPEN_BEEP_DELAY; // to ensure beeping starts after 1 s delay
				}
			}


			// -----------------------------------------------------------
			// Turning off OP2 & OP5 if chamber opens outside of unloading (AUTO)

			if( (Stage != STAGE_NETTING_END)    &&    // Ignoring chamber opening between the start & end of unloading
					(Stage != STAGE_UNLOADING)      &&
					(Stage != STAGE_UNLOADING_WAIT)    )
			{
				if( CHAMBER_OPEN ) // Chamber Locked Sensor open
				{
					// Saving OP5 flags if not yet saved
					if( !SavedRollEngageTurnedOn )
					{
						SavedRollEngageTurnedOn = RollEngageTurnedOn;
					}
					if( !SavedOP5TurnedOnAfterBeakEngagePulsesEnabled )
					{
						SavedOP5TurnedOnAfterBeakEngagePulsesEnabled = OP5TurnedOnAfterBeakEngagePulsesEnabled;
					}

					// Ensuring OP5 Roller Engage Solenoid is off while Chamber Locked Sensor open
					RollEngageTurnedOn = 0;                      // OP5 Roller Engage Solenoid (AKA Pre Net Position) turned off
					OP5TurnedOnAfterBeakEngagePulsesEnabled = 0; // Turning on Roller Engage Solenoid (OP5) after 'Beak Engage' pulses counted on IP4
					SeqOpOn &= ~OP_ROLL_ENGAGE; // OP5 OFF: Roller Engage Solenoid (AKA Pre Net Position)
					SeqOpOff |= OP_ROLL_ENGAGE;

					// Ensuring OP2 Net Clutch is off while Chamber Locked Sensor open
					SeqOpOn &= ~OP_NET_ON; // OP2 OFF: Net Clutch (AKA Net On)
					SeqOpOff |= OP_NET_ON;
				}
				else // Chamber closed
				{
					// Restoring OP5 flags if saved
					if( SavedRollEngageTurnedOn )
					{
						RollEngageTurnedOn = SavedRollEngageTurnedOn;
						SavedRollEngageTurnedOn = 0; // Reset
					}
					if( SavedOP5TurnedOnAfterBeakEngagePulsesEnabled )
					{
						OP5TurnedOnAfterBeakEngagePulsesEnabled = SavedOP5TurnedOnAfterBeakEngagePulsesEnabled;
						SavedOP5TurnedOnAfterBeakEngagePulsesEnabled = 0; // Reset
					}
				}
			}
			else // Between start & end of unloading -- Netting finished before unloading OR waiting for sufficient pre net pulses to be counted after unloading
			{
				RollEngageTurnedOn = 0; // Reset
				SavedRollEngageTurnedOn = 0; // Reset

				OP5TurnedOnAfterBeakEngagePulsesEnabled = 0; // Reset
				SavedOP5TurnedOnAfterBeakEngagePulsesEnabled = 0; // Reset
			}


			// -----------------------------------------------------------
			// Disabling Net Warning & Netting Start Beeps (incl. Pre Warning & Net Delay Beeps) if chamber opens outside of unloading (AUTO)

			if( (Stage != STAGE_NETTING_END)    &&    // Ignoring chamber opening between the start & end of unloading
					(Stage != STAGE_UNLOADING)      &&
					(Stage != STAGE_UNLOADING_WAIT)    )
			{
				if( CHAMBER_OPEN ) // Chamber Locked Sensor open
				{
					NetWarningDisabledWhileIP3OpenInAutoMode = 1;
					NettingStartBeepsDisabledWhileIP3OpenInAutoMode = 1; // Including Pre Warning & Net Delay beep
				}
				else // Chamber closed
				{
					NetWarningDisabledWhileIP3OpenInAutoMode = 0; // Reset
					NettingStartBeepsDisabledWhileIP3OpenInAutoMode = 0; // Reset
				}
			}
			else // Between start & end of unloading -- Netting finished before unloading OR waiting for sufficient pre net pulses to be counted after unloading
			{
				NetWarningDisabledWhileIP3OpenInAutoMode = 0; // Reset
				NettingStartBeepsDisabledWhileIP3OpenInAutoMode = 0; // Reset
			}

		}
		else if(!TwineAutoManMode && !AutoMan) // MAN mode
		{
			// -----------------------------------------------------------
			// Monitoring bale density (MAN)

			if( BaleDiaVolts > DiaCoreVolts )  // Core diameter exceeded
			{
				// -----------------------------------------------------------
				// Density Control Function -- based on Target Bale Density

				if( DensityPresVolts >= DensityBaleVolts ) // Target bale density reached/exceeded
				{
					if( OP8OnDelayTime )
						OP8OnDelayTime--;
					else // Pres. Sw. delay has expired
					{
						SeqOpOn |= OP_DENSITY_REL; // OP8 ON: Density Release Solenoid (AKA Density Control)
						SeqOpOff &= ~OP_DENSITY_REL;
						OP8OffDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
					}
				}
				else if( DensityPresVolts < (DensityBaleVolts - pFac.pf.GW302_PresDeadband) ) // Fallen below deadband below target bale density
				{
					if( OP8OffDelayTime )
						OP8OffDelayTime--;
					else // Pres. Sw. delay has expired
					{
						SeqOpOn &= ~OP_DENSITY_REL; // OP8 OFF: Density Release Solenoid (AKA Density Control)
						SeqOpOff |= OP_DENSITY_REL;
						OP8OnDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
					}
				}
			}
			else  // Core diameter not yet reached
			{
				// -----------------------------------------------------------
				// Density Control Function -- based on Bale Core Density

				if( DensityPresVolts >= DensityCoreVolts ) // Target core density reached/exceeded
				{
					if( OP8OnDelayTime )
						OP8OnDelayTime--;
					else // Pres. Sw. delay has expired
					{
						SeqOpOn |= OP_DENSITY_REL; // OP8 ON: Density Release Solenoid (AKA Density Control)
						SeqOpOff &= ~OP_DENSITY_REL;
						OP8OffDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
					}
				}
				else if( DensityPresVolts < (DensityCoreVolts - pFac.pf.GW302_PresDeadband) ) // Fallen below deadband below target core density
				{
					if( OP8OffDelayTime )
						OP8OffDelayTime--;
					else // Pres. Sw. delay has expired
					{
						SeqOpOn &= ~OP_DENSITY_REL; // OP8 OFF: Density Release Solenoid (AKA Density Control)
						SeqOpOff |= OP_DENSITY_REL;
						OP8OnDelayTime = (unsigned char)(pFac.pf.GW302_PresSwDelay * 10.0);
					}
				}
			}


			// -----------------------------------------------------------
			// Monitoring bale size (MAN)

			if( BaleDiaVolts > DiaTargetVolts )  // Target bale diameter exceeded
			{

				if(OP5ManTurnOff==30)
				{

					OP5ManTurnOff++;
				}
				else if(OP5ManTurnOff < 30)
				{
					OP5ManTurnOff++;
				}


				if( BalerFullDelayTime )
				{
					BalerFullDelayTime--;

					// Intermittent beep for duration of Net Delay time (t1)
					buzz_count = 1;

					// Resetting to enable target beeps
					target_bale_dia_reached_cont_beep = 0;
				}
				else // Baler Full / Net Delay (t1) has expired
				{
					// Continuous beep for 3s (when OP2 turns on)
					if( !target_bale_dia_reached_cont_beep )
					{
						buzz_length = NETTING_START_BEEP; // Long continuous beep when netting can be started in MAN mode
						target_bale_dia_reached_cont_beep = 1;




						//add counter of 3s to make sure that OP5 turns off

					}



				}
			}
			else // Target bale diameter has not yet been reached
			{
				if( BaleDiaVolts > DiaPreWarnVolts ) // Pre warning beep bale diameter exceeded
				{
					if( pFac.pf.GW302_PreWarningBeep > 0 ) // Pre Warning beep on (set to non-zero value)
					{
						if( !pre_warning_cont_beep )
						{
							buzz_length = PRE_WARNING_BEEP; // Pre Warning beep
							pre_warning_cont_beep = 1;      // Only repeated after a sequence is completed
						}
					}
				}

				// Resetting to allow baler full timer to be restarted & enable target beeps
				BalerFullDelayTime = (unsigned char)(pFac.pf.GW302_NetDelay * 10.0);
				target_bale_dia_reached_cont_beep = 0;
			}


			// -----------------------------------------------------------
			// Stage completion (MAN)

			// First 2 stages completed
			if( stagecomp == (NETFEEDON | NETCUTON) )
			{
				if( CHAMBER_OPEN ) // Chamber Locked Sensor open
				{
					if( ChamberOpenDelayTime )
					{
						ChamberOpenDelayTime--;

						if( !start_chamber_open_delay_cont_beep )
						{
							buzz_length = MAN_START_END_CHAMBER_OPEN_DELAY_BEEP; // Beep at start of chamber open delay
							start_chamber_open_delay_cont_beep = 1;
						}
					}
					else // Chamber open delay has expired
					{
						stagecomp |= CHAMBOP;     // Set CHAMBER OPEN stage completed
						stagecomp &= ~(CHAMBCLO); // This stage is to be completed next

						if( !end_chamber_open_delay_cont_beep )
						{
							buzz_length = MAN_START_END_CHAMBER_OPEN_DELAY_BEEP; // Beep at end of chamber open delay
							end_chamber_open_delay_cont_beep = 1;
						}
					}
				}

				if( CHAMBER_CLOSED) // Chamber Locked Sensor closed
				{
					ChamberOpenDelayTime = CODT_CHAMBER_OPEN_TIME;  // Reset Chamber open delay
					start_chamber_open_delay_cont_beep = 0;
				}
			}

			// First 3 stages completed
			if( stagecomp == (NETFEEDON | NETCUTON | CHAMBOP) )
			{
				if( CHAMBER_CLOSED) // Chamber Locked Sensor closed
				{
					stagecomp |= CHAMBCLO;  // Set CHAMBER CLOSED stage completed
				}
			}

			// all stages completed (in the order indicated left to right)
			if( stagecomp == (NETFEEDON | NETCUTON | CHAMBOP | CHAMBCLO) )
			{
				stagecomp = SC_CLEAR; // Reset

				UpdatingAndSavingTotalsToMemory();

				buzz_length = ALL_STAGES_COMPLETE_BEEP; // Beep on completion of all stages

				ResetSequenceFactors( SEQ_ALL );
			}


			// -----------------------------------------------------------
			// Roller Engage Solenoid (OP5) turned on/off (MAN)

			if( pFac.pf.GW302_NetSystem == NS_ROLLER ) // Roller Engage Solenoid (OP5) only turned on/off if 'Net System' set to 'Roller'
			{
				// Turning Roller Engage Solenoid (OP5) on if Bale Diameter goes from below to above Engage diameter
				if( (BaleDiaVolts < DiaEngageVolts)) // below Engage diameter
				{
					BaleDiameterStatusInManMode |= BDSIMM_BELOW_ENGAGE_DIA; // Set BDSIMM_BELOW_ENGAGE_DIA flag
					BaleDiameterStatusInManMode &= ~(BDSIMM_ABOVE_ENGAGE_DIA); // Flag to be set next
					RollEngageTurnedOnInManMode = 0; // OP5 OFF: Roller Engage Solenoid (AKA Pre Net Position) -- turned off in MAN mode
				}
				else // above Engage diameter
				{
					if(CHAMBER_CLOSED)
					{
						if( BaleDiameterStatusInManMode == BDSIMM_BELOW_ENGAGE_DIA ) // BDSIMM_BELOW_ENGAGE_DIA flag already set
						{
							BaleDiameterStatusInManMode |= BDSIMM_ABOVE_ENGAGE_DIA; // Set BDSIMM_ABOVE_ENGAGE_DIA flag

							if ((RollerTimer == 255) && (RollEngageTurnedOnInManMode == 0) )
							{
								RollEngageTurnedOnInManMode = 1; // OP5 ON: Roller Engage Solenoid (AKA Pre Net Position) -- turned on in MAN mode
								RollerTimer = OP_ROLL_ENGAGE_TIMER; /*3 seconds*/
							}

						}
					}
				}

				// Turning Roller Engage Solenoid (OP5) off if OP7 (Net Cut) comes on -- at any time
				// Done in Button 2 code

				// Turning Roller Engage Solenoid (OP5) off if chamber closes and opens -- at any time
				if( ChamberStatus == (CS_CLOSED | CS_OPEN) ) // Chamber was closed and now opened
				{
					RollEngageTurnedOnInManMode = 0; // OP5 OFF: Roller Engage Solenoid (AKA Pre Net Position) -- turned off in MAN mode
				}
				//else if(OP5ManTurnOff==30)
				//{
				//	RollEngageTurnedOnInManMode = 0;
				//}

				// Turning Roller Engage Solenoid (OP5) on/off
/*				if( RollEngageTurnedOnInManMode )
				{
					KeyOpOn |=  OP_ROLL_ENGAGE; // OP5 ON: Roller Engage Solenoid (AKA Pre Net Position)
				}*/
			} // end if( pFac.pf.GW302_NetSystem == NS_ROLLER )

		} // end MAN mode

		/*if(pFac.pf.GW302_NetTwineOption == TO_TWINE)
		{*/
			if (RollerTimer && !(RollerTimer == 255))
			{
				RollEngageTurnedOn = 0;
				SeqOpOn |= OP_ROLL_ENGAGE; // OP5 ON: Roller Engage Solenoid (AKA Pre Net Position)
				SeqOpOff &= ~OP_ROLL_ENGAGE;
				RollerTimer--;
			}
			else if(!RollerTimer)
			{
				SeqOpOn &= ~OP_ROLL_ENGAGE; // OP5 OFF: Roller Engage Solenoid (AKA Pre Net Position)
				SeqOpOff |= OP_ROLL_ENGAGE;
				RollerTimer = 255;
			}
		/* } */

		// -----------------------------------------------------------
		// Check Chopping knives position (MAN/AUTO)

		if( CHOPPING_KNIVES_ON && // IP2 open: 11 Knife Sensor (AKA Chopping Knives position / Knives Position Sensor 1) in the ‘ON’ position
				( (((key & ~(SW_NEW | SW_REP)) != SW_IR) && ((key & ~(SW_NEW | SW_REP)) != SW_OR)) || (SecondScreen == 1) ) ) // Inhibited when IR or OR button pressed while first screen displayed
		{
			Kniv1_IPClosed = 1;
			Kniv1_IPOpenTime = 0;
		}
		else // IP2 closed: 11 Knife Sensor in the ‘OFF’ position
		{
			if( Kniv1_IPClosed )
			{
				if( Kniv1_IPOpenTime < 250 )
					Kniv1_IPOpenTime++;

				if( Kniv1_IPOpenTime > 10 )
					ChoppingKnives1WarningOn = 1; // Chopping knives position Warning on
			}
		}


		// -----------------------------------------------------------
		// Check Chamber Locked Sensor (MAN/AUTO)

		// Looking for Chamber to close then open
		if( CHAMBER_CLOSED ) // IP3: Chamber Locked Sensor closed
		{
			ChamberStatus |= CS_CLOSED; // Set CS_CLOSED flag
			ChamberStatus &= ~(CS_OPEN); // Flag to be set next
		}
		else // CHAMBER_OPEN -- IP3: Chamber Locked Sensor open
		{
			if( ChamberStatus == CS_CLOSED ) // CS_CLOSED flag already set
			{
				ChamberStatus |= CS_OPEN; // Set CS_OPEN flag
				OP8OnCount = OP8_ON_COUNT_DURATION; // 6s -- Set count (or reset, if count down already underway)
			}
		}

		// After Chamber closes then opens, turn on Density Release Solenoid for the count
		if( OP8OnCount )
		{
			KeyOpOn |= OP_DENSITY_REL; // OP8 ON: Density Release Solenoid (AKA Density Control)

			OP8OnCount--;
		}


		// -----------------------------------------------------------
		// Check Bale Diameter (MAN/AUTO)

		// Looking for whether Pre warning beep bale diameter exceeded OR Engage diameter reached
		if( (BaleDiaVolts > DiaPreWarnVolts) || (BaleDiaVolts >= DiaEngageVolts) )
		{
			DrivingArrowBeepsDisabled = 1; // Driving Arrow Beeps turned off
		}


		// -----------------------------------------------------------
		// Outputs (MAN/AUTO)

		SeqOpOn = SeqOpOn & ~SeqOpOff; /* If an Op is to be turned off, dont turn it on */



		OpOff = KeyOpOff | SeqOpOff;
		/*AT91C_BASE_PIOA->PIO_SODR = OpOff;*/

		//turn all outputs off
		if(OpOff == 0)
		{
			//don't turn OP6 off
			OpOff = 0xdf;
		}


		tempCh = 1;

		//outputs to turn off
		for(i=0; i< 8; i++)
		{
			if(OpOff & tempCh)
			{
				SetOutputOUT(i, OUT_OFF);
				SetOutputPWM(i+2, 0, PWM_DUTY000);
			}

			tempCh= tempCh << 1;
		}

		OpOn = KeyOpOn | SeqOpOn;
		//outputs to turn on

		tempCh = 1;

		for(i=0; i< 8; i++)
		{

			if(OpOn & tempCh)
			{
				if(i == 1)
				{
					tempVar2 = 5;
				}


				SetOutputOUT(i, OUT_ON);
				SetOutputPWM(i+2, 0, PWM_DUTY100);
			}

			tempCh= tempCh << 1;
		}

		if(tempVar2 == 5)
		{
			tempVar2 = 5;
		}


	//} // end if( AllTime )

}
// -----------------------------------------------------------

// -----------------------------------------------------------
// Function Name       : CalculateNetPulsesPerLayer
// Object              : Calculates net pulses per layer
// Output Parameters   : NetPulsesPerLayer
// -----------------------------------------------------------
void CalculateNetPulsesPerLayer( void )
{
	float LinearCharacteristicGradient; // [m]
	float LinearCharacteristicOffset; // [c]

	// Net Pulses Per Layer [y] = function( Bale Diameter Voltage minus Minimum Bale Diameter Voltage [x] )
	// where function has a linear characteristic of the general form: y = mx + c

	LinearCharacteristicGradient = (pFac.pf.GW302_MaxDiaPulsPerLay - pFac.pf.GW302_MinDiaPulsPerLay) / (DiaMaxVolts - DiaMinVolts);
	LinearCharacteristicOffset = pFac.pf.GW302_MinDiaPulsPerLay;

	NetPulsesPerLayer = ( LinearCharacteristicGradient * (BaleDiaVolts - DiaMinVolts) ) + LinearCharacteristicOffset;
}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : CalculateTargetNetPulses
// Object              : Calculates target net pulses
// Output Parameters   : TargetNetPulses
// -----------------------------------------------------------
void CalculateTargetNetPulses( void )
{
	// Calculating Net Pulses Per Layer = function( Bale Diameter Voltage minus Minimum Bale Diameter Voltage )
	CalculateNetPulsesPerLayer();

	// Calculating Target Net Pulses = function( Net Pulses Per Layer )
	TargetNetPulses = (unsigned short int)(NetPulsesPerLayer * pFac.pf.GW302_NetLayers[pFac.pf.GW302_BaleProfile] * pFac.pf.GW302_NetFactor) - (unsigned short int)pFac.pf.GW302_NetCutPulse;
}

// -----------------------------------------------------------
// Function Name       : UpdatingAndSavingTotalsToMemory
// -----------------------------------------------------------
void UpdatingAndSavingTotalsToMemory( void )
{
	pFac.pf.BaleTotal[pFac.pf.BaleStore] += PF_GW302_BaleTotal[PF_STEP];
	if( pFac.pf.BaleTotal[pFac.pf.BaleStore] > PF_GW302_BaleTotal[PF_MAX] ) // Rolling over at max number of digits displayed
	{
		pFac.pf.BaleTotal[pFac.pf.BaleStore] = PF_GW302_BaleTotal[PF_MIN]; // 0UL
	}

	pFac.pf.GrandBaleTotal += PF_GW302_GrandBaleTotal[PF_STEP];
	if( pFac.pf.GrandBaleTotal > PF_GW302_GrandBaleTotal[PF_MAX] ) // Rolling over at max number of digits displayed
	{
		pFac.pf.GrandBaleTotal = PF_GW302_GrandBaleTotal[PF_MIN]; // 0UL
	}

	// Saving Net applied indicated by pulses counted for Netting stage of current sequence
	// and unless instrument was powered up at the start of the sequence,
	// any pulses counted since last save from previous sequence (including any pulses counted during Pre-Net stage)
	pFac.pf.GW302_TotalNetMetres += NetMetresAppliedForCount;
	if( pFac.pf.GW302_TotalNetMetres > PF_GW302_TotalNetMetres[PF_MAX] ) // Freezing Total Net Metres at a maximum (when reached)
	{
		pFac.pf.GW302_TotalNetMetres = PF_GW302_TotalNetMetres[PF_MAX];
	}
	NetMetresPulseCount = 0UL; // Resetting IP4 pulse count for Net applied (in metres)

	pRun.pr.LubeCounter--; // pRun.pr.LubeCounter allowed to go negative

	SavecalDelayOnMainDisplay = 1;
}
// -----------------------------------------------------------

