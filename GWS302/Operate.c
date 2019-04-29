/*
 * operate.c
 *
 *  Created on: 31 May 2016
 *      Author: cparsons
 */

#include "global.h"

// defines
/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/

// variables
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

unsigned char  tempst;

// functions
/*-----------------------------------------------------------*/
static void vOperateTask( void *pvParameters )
{
	portTickType xLastWakeTime;
	static unsigned char HalfSecondCounter;
	static unsigned char Ms100Timer;
	struct CAN_HBM *ptMod;
	static unsigned char CANModuleTimeout;
	// start timer
	TIM_Cmd( TIM14, ENABLE );

	// start pwm timers - including
	SetupPWMTimers();

	ptMod = &HBM[0];
	/* turn on OP6 - 12V supply on the machine */
	SetOutputOUT(OUT_OP06, OUT_ON);
	SetOutputPWM(PWM_OP06, 0, PWM_DUTY100);


	vTaskDelay( 200 / portTICK_RATE_MS ); // wait 200ms

	// setup atod and start first conversion
	StartAtoD();

	// start IP capture timers
	TIM_Cmd( TIM1, ENABLE );
	TIM_Cmd( TIM2, ENABLE );


	savecal = 0;

	SavecalDelayWithinMenus = 0;
	SavecalDelayOnMainDisplay = 0;


	// Resetting IP4 pulse count for Net applied (in metres)
	NetMetresPulseCount = 0UL;


	// Initialise for calculation of Analogue Input Rolling Averages
	BaleDiaVoltsTotAccumMultSamples     = 0UL;  // Bale Diameter
	DensityPresVoltsTotAccumMultSamples = 0UL;  // (Bale) Density Pressure
	BaleShapeVoltsTotAccumMultSamples   = 0UL;  // Bale Shape Indicator
	KnifePresVoltsTotAccumMultSamples   = 0UL;  // Knife Pressure


	// set current time
	xLastWakeTime = xTaskGetTickCount();

	while( 1 )
	{
		vTaskDelayUntil( &xLastWakeTime, (10 / portTICK_RATE_MS) );



		if( (++Ms100Timer % 10) == 0 ) // timer incremented every 100ms
		{


			// check IP01 to IP06
			CheckInputs();

			// calculate frequencies on IP01 to IP06
			CalculateFrequencies();

			// check CAN0 for errors
			CAN0check();

			// check CAN1 for errors
			CAN1check();

			// reload watch dog count down timer
			IWDG_ReloadCounter();


			tempst = ptMod->status;

			if((CANModuleTimeout > 80) && !(ptMod->status & CAN_MODULE_MESSAGE_RECEIVED))
			{
				ptMod->status &= ~CAN_MODULE_PRESENT;
			}



			if(!(ptMod->status & CAN_MODULE_MESSAGE_RECEIVED))
			{
				CANModuleTimeout++;
			}
			else
			{
				CANModuleTimeout = 0;
			}



			//If module is not present request software message
			if(!(ptMod->status & CAN_MODULE_PRESENT))
				HBMRoutines(ptMod,HBM_GET_SOFTWARE, 0);
			else
			{
				HBMRoutines(ptMod,HBM_GETINPUTS, 0);
				HBMRoutines(ptMod,HBM_GETINPUTS, 1);
				HBMRoutines(ptMod,HBM_GETINPUTS, 2);
				HBMRoutines(ptMod,HBM_GETINPUTS, 3);
				HBMRoutines(ptMod,HBM_SETPARAMETERS, 0);

				HBMRoutines(ptMod,HBM_SETOUTPUTS, 0);

			}


			// every half second
			if( (++HalfSecondCounter % 5) == 0 ) // timer incremented every 100ms
			{

				if( savecal )
				{
					savecal = 0;

					// write factor to flash if needed
					pFac.pf.CheckSum = CheckSumIt( (unsigned char *)&pFac, (unsigned char *)&(pFac.pf.CheckSum) );
					#ifndef WINDOWS
					// write main factors
					SPI_FLASH_EraseThenWrite( (uint8_t *)&pFac, pFac_BASEADDRESS_MAIN, sizeof( pFac ), 0, 0 );

					// write backup factors
					SPI_FLASH_EraseThenWrite( (uint8_t *)&pFac, pFac_BASEADDRESS_BACK, sizeof( pFac ), 0, 0 );
					#endif
				}


				// Every 500 ms

				// Length of Net applied (in metres) for the current IP4 pulse count indicating Net applied (yet to be added to the Total Net Metres (m) factor)
				//NetMetresAppliedForCount = NET_METERS_PER_PULSE * (float)NetMetresPulseCount;
				NetMetresAppliedForCount = NET_METERS_PER_PULSE * (float)NetMetresPulseCount;
				// Calculating Net Pulse Frequency (Hz)
				CalculateNetPulseFrequency();

				HalfSecondCounter = 0;
			}

			Ms100Timer = 0;



		}
		// calculate atod values on AIP01 to AIP04 & IPVOUT
		CheckAnalogueLevels();



	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void vStartOperateTask( unsigned portBASE_TYPE uxPriority )
{
	// setup frequency measurement timers
	TimerCaptureSetup();

	/* Spawn the task. */
	xTaskCreate( vOperateTask, "OPERATE", configOPERATE_STACK_SIZE, (void *)NULL, uxPriority, (xTaskHandle *)NULL );
}
/*-----------------------------------------------------------*/
