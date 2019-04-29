/*
 * timers.c - RDS Type D Ecu Module CAN Task
 *
 *  Created on: 1st February 2011
 *      Author: Chris Parsons
 */

#include "global.h"

// global variables
/*-----------------------------------------------------------*/
unsigned int I2C1timeout;

/*struct IPTimerCapture {
	volatile unsigned long int *CurrentTime;
	volatile unsigned long int *CaptureTime;
	volatile unsigned short int *InterruptEnable;
	volatile unsigned short int *Status;
	unsigned short int CaptureFlag;
	};*/
struct IPTimerCapture const tcIP[MAX_FREQUENCY_MEASUREMENTS] = {
	 &TIM1->CNT,  &TIM1->CCR1,  &TIM1->DIER,  &TIM1->SR,  TIM_IT_CC1, // IP01
	 &TIM1->CNT,  &TIM1->CCR2,  &TIM1->DIER,  &TIM1->SR,  TIM_IT_CC2, // IP02
	 &TIM1->CNT,  &TIM1->CCR3,  &TIM1->DIER,  &TIM1->SR,  TIM_IT_CC3, // IP03
	 &TIM1->CNT,  &TIM1->CCR4,  &TIM1->DIER,  &TIM1->SR,  TIM_IT_CC4, // IP04
	 &TIM2->CNT,  &TIM2->CCR1,  &TIM2->DIER,  &TIM2->SR,  TIM_IT_CC1, // IP05
	 &TIM2->CNT,  &TIM2->CCR2,  &TIM2->DIER,  &TIM2->SR,  TIM_IT_CC2  // IP06
	};

/*struct freqInput {
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
	};*/
struct freqInput fIP[MAX_FREQUENCY_MEASUREMENTS];
/*-----------------------------------------------------------*/

// functions
/*-----------------------------------------------------------
 * Function Name	: TimerCaptureSetup
 * Description		: Timer frequency capture initialisation
 * Input			: not used
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void TimerCaptureSetup( void )
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* SystemInit_GPIO() enables peripherals and timer clocks & maps io pins */

	// Initialise data structure
	TIM_TimeBaseStructInit( &TIM_TimeBaseInitStructure );
	//TIM_TimeBaseInitStructure.TIM_Period = 0xFFFFFFFF;
	TIM_TimeBaseInitStructure.TIM_Period = 0x0000FFFF; // timers 2 & 5 are 32 bit
	//TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x0000;

	// input capture timers
	TIM_TimeBaseInitStructure.TIM_Prescaler = 59; // TIMER_CLOCK_FREQUENCY / 60 for 1us counts
	TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure ); // 16 bit timer
	TIM_TimeBaseInit( TIM2, &TIM_TimeBaseInitStructure ); // 32 bit timer

	// output timer - 1ms
	TIM_TimeBaseInitStructure.TIM_Period = 999;
	TIM_TimeBaseInit( TIM14, &TIM_TimeBaseInitStructure ); // 16 bit timer

	TIM_ICStructInit( &TIM_ICInitStructure );
	//TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	//TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	//TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	//TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	//TIM_ICInitStructure.TIM_ICFilter = 0x00;
	TIM_ICInit(  TIM1, &TIM_ICInitStructure ); // IP01 - TIM1-CH1
	TIM_ICInit(  TIM2, &TIM_ICInitStructure ); // IP05 - TIM2-CH1

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInit(  TIM1, &TIM_ICInitStructure ); // IP02 - TIM1-CH2
	TIM_ICInit(  TIM2, &TIM_ICInitStructure ); // IP06 - TIM2-CH2

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
	TIM_ICInit(  TIM1, &TIM_ICInitStructure ); // IP03 - TIM1-CH3

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInit(  TIM1, &TIM_ICInitStructure ); // IP04 - TIM1-CH4

	// Enable TIM1 capture INT
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn; // TIM1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = timerCAPTURE_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	// Enable TIM1 update INT & TIM10_IRQn
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn; // TIM1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = timerUPDATE_PRIORITY;
	NVIC_Init( &NVIC_InitStructure );

	// TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_Init( &NVIC_InitStructure );

	// TIM14_IRQn;
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_TRG_COM_TIM14_IRQn;
	NVIC_Init( &NVIC_InitStructure );

	TIM_ITConfig( TIM1,  TIM_IT_Update | TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, ENABLE );
	TIM_ITConfig( TIM2,  TIM_IT_Update | TIM_IT_CC1 | TIM_IT_CC2, ENABLE );
	TIM_ITConfig( TIM14, TIM_IT_Update, ENABLE );
}
/*-----------------------------------------------------------*/

unsigned long int tempL, ctime2;

unsigned short countT;
unsigned int f6time, f6count;
unsigned char fnow;
/*-----------------------------------------------------------
 * Function Name	: fnTimerUpdate
 * Description		: overflow interrupt common function.
 * Input			: input number
 * Output			: None
 * Return			: None
 * Note				: uses IPTimerCapture Structure for lookup
 *-----------------------------------------------------------*/
void fnTimerUpdate( unsigned int ipX )
{
	struct freqInput *ip;
	const struct IPTimerCapture *ptTC;
	unsigned long int ctime, ltime;
	static unsigned int stime;

	ip = &fIP[ipX]; // point to interrupt variables

	ptTC = &tcIP[ipX]; // point to Timer Variables

	ip->time++;
	ctime = (ip->time * 0x10000UL) + (*ptTC->CurrentTime & 0x0ffff);
	ltime = ctime - ip->stime;

	if( ltime >= TC_ZERO )
	{

		// if any task using values then wait until next overflow
		if( ip->lockFreq == 0 )
		{
			ip->ftime = ltime;
			ip->fcount = ip->count;
			ip->count = 0;
			ip->newFreq = 1;
			ip->stime = ctime;

			ip->fnow = TC_IGNORE_EDGE;
		}
	}
	else if( ltime >= TC_SAMPLE )
	{
		if( ip->count && (ip->fnow == TC_CAPTURE_EDGE) )
		{
			ip->fnow = TC_SAMPLE_EDGE;
		}


	}

	// for frequency error to occur, overflow has to be serviced after
	// capture event, but before capture interrupt has been serviced, so
	// capture interrupt pending flag should be set
	// save overflow status for selected input if capture is pending
	ip->status = (*ptTC->Status & ptTC->CaptureFlag) ? *ptTC->Status : 0;
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: fnTimerCapture
 * Description		: overflow interrupt common function.
 * Input			: input number
 * Output			: None
 * Return			: None
 * Note				: uses IPTimerCapture Structure for lookup
 *-----------------------------------------------------------*/
void fnTimerCapture( unsigned int ipX )
{
	struct freqInput *ip;
	const struct IPTimerCapture *ptTC;
	unsigned long int ctime;

	ip = &fIP[ipX]; // point to interrupt variables

	ptTC = &tcIP[ipX]; // point to Timer Variables

	ctime = (ip->time * 0x10000UL) + (*ptTC->CaptureTime & 0x0ffff);

	// if overflow occurred after capture then time will need reducing
	// this time window is up to 32.768ms in the past from overflow
	// uses saved status, as actual bit is cleared by overflow interrupt
	if( (ip->status & TIM_IT_Update) && ((ctime & 0x0ffff) > 0x8000) )
	{
		ctime -= 0x10000;
	}

	if(ipX == 3)
		tempL = ctime - ip->edge;

	// check debounce time
	if( (ctime - ip->edge) > TC_DEBOUNCE ) //
	{

		// ignore first edge after zero time
		if( ip->fnow == TC_IGNORE_EDGE )
		{
			ip->fnow = TC_CAPTURE_EDGE;

			// set start time
			ip->stime = ctime;
		}
		else
		{
			ip->count++;

			if( ip->fnow == TC_SAMPLE_EDGE )
			{
				// if any task using values then wait until next capture
				if( ip->lockFreq == 0 )
				{
					ip->fnow = TC_CAPTURE_EDGE;

					ip->ftime = ctime - ip->stime;
					ip->stime = ctime;
					ip->fcount = ip->count;
					ip->count = 0;

					ip->newFreq = 1;

				}
			}

		} // ip->fnow

		// input pulse count
		if( ip->lockCount == 0 )
		{

			if(ipX == 3)
				IP4PulsesCount++; //temp counter for diagnostics

			ip->pcount++;
			if(ipX == 3)
			{
				/*
				NetMetresPulseCount = ip->pcount;
				NetPulseCount = ip->pcount;
				BeakEngagePulseCount = ip->pcount;
				*/
				NetMetresPulseCount++;
				NetPulseCount++;
				BeakEngagePulseCount++;
			}


			if( ip->pstore )
			{
				ip->pcount += ip->pstore;
				ip->pstore = 0;
			}
		}
		else
		{
			ip->pstore++;
		}
	}

	ip->edge = ctime;

	// clear saved overflow status for selected input
	ip->status = 0;
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: TIM1_CC_IRQHandler
 * Description		: capture interrupt for IP01/02/03/04
 * Input			: not used
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void TIM1_CC_IRQHandler( void )
{
	// capture
	if( TIM1->SR & TIM_IT_CC1 )
	{
		fnTimerCapture( IP01 );

		// clear interrupt bit - reading capture register also does this!
		TIM1->SR = ~TIM_IT_CC1;
	}

	if( TIM1->SR & TIM_IT_CC2 )
	{
		fnTimerCapture( IP02 );

		// clear interrupt bit - reading capture register also does this!
		TIM1->SR = ~TIM_IT_CC2;
	}

	if( TIM1->SR & TIM_IT_CC3 )
	{
		fnTimerCapture( IP03 );

		// clear interrupt bit - reading capture register also does this!
		TIM1->SR = ~TIM_IT_CC3;
	}

	if( TIM1->SR & TIM_IT_CC4 )
	{
		fnTimerCapture( IP04 );

		// clear interrupt bit - reading capture register also does this!
		TIM1->SR = ~TIM_IT_CC4;
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: TIM1_UP_TIM10_IRQHandler
 * Description		: overflow interrupt for IP01/02/03/04
 * 					: capture/overflow interrupt for IP11
 * Input			: not used
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void TIM1_UP_TIM10_IRQHandler( void )
{
	// timer overflow
	if( TIM1->SR & TIM_IT_Update )
	{
		fnTimerUpdate( IP01 );
		fnTimerUpdate( IP02 );
		fnTimerUpdate( IP03 );
		fnTimerUpdate( IP04 );

		// clear interrupt bit
		TIM1->SR = ~TIM_IT_Update;
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: TIM2_IRQHandler
 * Description		: capture/overflow interrupt for IP05/06/07/08
 * Input			: not used
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void TIM2_IRQHandler( void )
{
	// timer overflow
	if( TIM2->SR & TIM_IT_Update )
	{
		fnTimerUpdate( IP05 );
		fnTimerUpdate( IP06 );

		// clear interrupt bit
		TIM2->SR = ~TIM_IT_Update;
	}

	// capture
	if( TIM2->SR & TIM_IT_CC1 )
	{
		fnTimerCapture( IP05 );

		// clear interrupt bit - reading capture register also does this!
		TIM2->SR = ~TIM_IT_CC1;
	}

	// capture
	if( TIM2->SR & TIM_IT_CC2 )
	{
		fnTimerCapture( IP06 );

		// clear interrupt bit - reading capture register also does this!
		TIM2->SR = ~TIM_IT_CC2;
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: ShortDelay
 * Description		:
 * Input			: delay in microseconds
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void ShortDelay( unsigned short int delay )
{
	unsigned short int start, dur;

	start = (unsigned short int)TIM14->CNT;

	delay = delay % 1000;

	do
	{
		dur = (unsigned short int)TIM14->CNT;

		if( dur >= start )
		{
			dur = dur - start;
		}
		else
		{
			dur = 1000 + dur - start;
		}
	}
	while( dur < delay );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: TIM14duration
 * Description		: gets TIM14 elapsed time
 * Input			: start time
 * Output			:
 * Return			: duration in microseconds
 * Note				:
 *-----------------------------------------------------------*/
unsigned short int TIM14duration( unsigned short int start )
{
	unsigned short int dur;

	dur = (unsigned short int)TIM14->CNT;
	if( dur >= start )
	{
		dur = dur - start;
	}
	else
	{
		dur = 1000 + dur - start;
	}

	return( dur );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: TIM8_TRG_COM_TIM14_IRQHandler
 * Description		: capture/overflow interrupt for IP09/10
 * Input			: not used
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void TIM8_TRG_COM_TIM14_IRQHandler( void )
{
	// timer overflow every 1ms
	if( TIM14->SR & TIM_IT_Update )
	{
		if( buzz_length )
		{
			buzz_length--;
			// buzzer on
			OUTPUT_BUZZER_ON;
		}
		else if( off_length )
		{
			off_length--;
			// buzzer off
			OUTPUT_BUZZER_OFF;
		}
		else if( buzz_count )
		{
			buzz_count--;
			buzz_length = BEEP_ON;
			off_length = BEEP_OFF;
		}
		else
		{
			// buzzer off
			OUTPUT_BUZZER_OFF;
		}

		if( I2C1timeout > 0 )
		{
			I2C1timeout--;
		}

		// clear interrupt bit
		TIM14->SR = ~TIM_IT_Update;
	}
}
/*-----------------------------------------------------------*/


