/*
 * timers.h - RDS Type D Ecu Module CAN Task
 *
 *  Created on: 1st February 2011
 *      Author: Chris Parsons
 */

#ifndef TIMERS_H_
#define TIMERS_H_

// defines
/*-----------------------------------------------------------*/
#define timerUPDATE_PRIORITY	4
#define timerCAPTURE_PRIORITY	3

#define IP01	0
#define IP02	1
#define IP03	2
#define IP04	3
#define IP05	4
#define IP06	5
#define IP07	6
#define IP08	7
#define IP09	8
#define IP10	9
#define IP11	9
#define IP12	9

#define TC_IGNORE_EDGE  0x00
#define TC_SAMPLE_EDGE  0x01
#define TC_CAPTURE_EDGE 0x02

// with timers running at 1us
#define TC_ZERO_TIME		120 // ms
#define TC_SAMPLE_TIME		 40 // ms
#define TC_FREQUENCY_LIMIT 5000 // Hz

#define TC_ZERO           (TC_ZERO_TIME * 1000)
#define TC_SAMPLE         (TC_SAMPLE_TIME * 1000)
#define TC_DEBOUNCE       (1000000UL / (unsigned long int)TC_FREQUENCY_LIMIT)

#define MAX_FREQUENCY_MEASUREMENTS 6
/*-----------------------------------------------------------*/

// variables
/*-----------------------------------------------------------*/
extern unsigned int I2C1timeout;

struct IPTimerCapture {
	volatile unsigned long int *CurrentTime;
	volatile unsigned long int *CaptureTime;
	volatile unsigned short int *InterruptEnable;
	volatile unsigned short int *Status;
	unsigned short int CaptureFlag;
};
extern const struct IPTimerCapture tcIP[MAX_FREQUENCY_MEASUREMENTS];

struct freqInput {
	unsigned int zero_time;
	unsigned int sample_time;
	unsigned int debounce_time;

	unsigned int time;
	unsigned int stime;

	unsigned int edge;

	unsigned int pcount;
	unsigned int pstore;
	unsigned int ftime;
	unsigned int dtime;

	unsigned short int count;
	unsigned short int fcount;

	float duty;
	float frequency;

	unsigned char fnow;
	unsigned char status;
	unsigned char newFreq;
	unsigned char lockFreq;
	unsigned char lockCount;
	};
extern struct freqInput fIP[MAX_FREQUENCY_MEASUREMENTS];
/*-----------------------------------------------------------*/

// function prototypes
/*-----------------------------------------------------------*/
extern void TimerCaptureSetup( void );
extern void ShortDelay( unsigned short int delay );
extern unsigned short int TIM14duration( unsigned short int start );

extern unsigned short int PulseCounter;
extern unsigned long int tempL;
extern unsigned short countT;
extern unsigned int IP4PulsesCount;
/*-----------------------------------------------------------*/

#endif /* TIMERS_H_ */
