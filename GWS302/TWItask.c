/*
 * TWItask.c
 *
 *  Created on: 30 Sep 2013
 *      Author: Chris
 */

#include "global.h"

// global variables
/*-----------------------------------------------------------*/
unsigned char I2CMode;
unsigned char ClockRam[9];

unsigned int iic_get_error;
unsigned int iic_error_count;

/*-----------------------------------------------------------*/

// functions
/*-----------------------------------------------------------
 * Function Name	: InitialiseRTC
 * Description		: RTC hardware initialisation
 * Input			: not used
 * Return			: None
 * Note				: RTC/RAM	@ 0xde
 * 					: EEPROM	@ 0xae
 *-----------------------------------------------------------*/
void InitialiseRTC( void )
{
	I2C_InitTypeDef structI2C_InitStruct;

	I2C_DeInit( I2C1 );

	// set default values for I2C1 peripheral RTC is connected to
	I2C_StructInit( &structI2C_InitStruct );

	// I2C_InitStruct->I2C_ClockSpeed = 5000;
	//structI2C_InitStruct.I2C_ClockSpeed = 400000; // 400KHz
	structI2C_InitStruct.I2C_ClockSpeed = 85000; // 85KHz
	// I2C_InitStruct->I2C_Mode = I2C_Mode_I2C;
	// I2C_InitStruct->I2C_DutyCycle = I2C_DutyCycle_2;
	// I2C_InitStruct->I2C_OwnAddress1 = 0;
	structI2C_InitStruct.I2C_OwnAddress1 = IIC_CLOCK_ADDRESS;
	// I2C_InitStruct->I2C_Ack = I2C_Ack_Disable;
	structI2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	// I2C_InitStruct->I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

	I2C_Cmd( I2C1, ENABLE );

	I2C_Init( I2C1, &structI2C_InitStruct );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: I2C1_GetFlagStatus_TimeOut
 * Description		: check flag, with timeout
 * Input			: Flag ID to wait for
 * 					: Flag Status to wait for
 * 					: timeout in ms
 * Return			: SUCCESS if ok
 * 					: ERROR if fail
 * Note				: DO NOT CALL ROUTINE OUTSIDE OF vRTCtask
 *-----------------------------------------------------------*/
ErrorStatus I2C1_GetFlagStatus_TimeOut( uint32_t I2C_FLAG, FlagStatus status, unsigned int timeout )
{

	I2C1timeout = timeout;

	while( I2C_GetFlagStatus( I2C1, I2C_FLAG ) == status )
	{
		// check timeout
		if( I2C1timeout == 0 )
		{
			return( ERROR );
		}
	}

	return( SUCCESS );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: I2C1_CheckEvent_TimeOut
 * Description		: check event, with timeout
 * Input			: Event ID to wait for
 * 					: timeout in ms
 * Return			: SUCCESS if ok
 * 					: ERROR if fail
 * Note				: DO NOT CALL ROUTINE OUTSIDE OF vRTCtask
 *-----------------------------------------------------------*/
ErrorStatus I2C1_CheckEvent_TimeOut( uint32_t I2C_EVENT, unsigned int timeout )
{

	I2C1timeout = timeout;

	while( I2C_CheckEvent( I2C1, I2C_EVENT ) == ERROR )
	{
		// check timeout
		if( I2C1timeout == 0 )
		{
			return( ERROR );
		}
	}

	return( SUCCESS );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: get_data
 * Description		: fetch data from I2C real time clock
 * Input			: offset - data location offset of data to fetch
 * 					: pt - pointer to array to store data
 * 					: count - number of bytes to read
 * Return			: 0 if success
 * Note				: DO NOT CALL ROUTINE OUTSIDE OF vRTCtask
 *-----------------------------------------------------------*/
unsigned int get_data( unsigned char offset, unsigned char *pt, unsigned char count )
{

	// check if bus is busy, 50ms timeout
	if( I2C1_GetFlagStatus_TimeOut( I2C_FLAG_BUSY, SET, 50 ) == ERROR )
	{
		return( 1 );
	}

	if( count > 2 )
	{
		// Set for START condition
		I2C_GenerateSTART( I2C1, ENABLE );

		// Test on EV5 and clear it (cleared by reading SR1 then writing to DR), 50ms timeout
		if( I2C1_CheckEvent_TimeOut( I2C_EVENT_MASTER_MODE_SELECT, 50 ) == ERROR )
		{
			return( 2 );
		}

		// send address
		I2C_Send7bitAddress( I2C1, IIC_CLOCK_ADDRESS, I2C_Direction_Transmitter );

		// Test on EV6 and clear it, 50ms timeout
		if( I2C1_CheckEvent_TimeOut( I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, 50 ) == ERROR )
		{
			return( 3 );
		}

		// Send the internal address offset to read from
		I2C_SendData( I2C1, offset );

		// Test on EV8 and clear it, 50ms timeout
		if( I2C1_GetFlagStatus_TimeOut( I2C_FLAG_BTF, RESET, 50 ) == ERROR )
		{
			return( 4 );
		}

		// Set START condition a second time
		I2C_GenerateSTART( I2C1, ENABLE );

		// Test on EV5 and clear it (cleared by reading SR1 then writing to DR), 50ms timeout
		if( I2C1_CheckEvent_TimeOut( I2C_EVENT_MASTER_MODE_SELECT, 50 ) == ERROR )
		{
			return( 5 );
		}

		// send address
		I2C_Send7bitAddress( I2C1, IIC_CLOCK_ADDRESS, I2C_Direction_Receiver );

		// wait for ADDR flag, 50ms timeout
		if( I2C1_GetFlagStatus_TimeOut( I2C_FLAG_ADDR, RESET, 50 ) == ERROR )
		{
			return( 6 );
		}

		// enable acknowledgement until last byte
		I2C_AcknowledgeConfig( I2C1, ENABLE );

		// Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read)
		(void)I2C1->SR2;

		while( count )
		{
			count--;

			// Wait for the byte to be received, 50ms timeout
			if( I2C1_GetFlagStatus_TimeOut( I2C_FLAG_BTF, RESET, 50 ) == ERROR )
			{
				return( 7 );
			}

			if( count == 1 )
			{
				// disable acknowledgement
				I2C_AcknowledgeConfig( I2C1, DISABLE );
			}
			else if( count == 0 )
			{
				// Send STOP Condition
				I2C_GenerateSTOP( I2C1, ENABLE );
			}

			// Read the byte received
			*pt++ = I2C_ReceiveData( I2C1 );
		}

		// 50ms timeout
		I2C1timeout = 50;
		// Wait to make sure that STOP control bit has been cleared
		while( I2C1->CR1 & I2C_CR1_STOP )
		{
			// check timeout
			if( I2C1timeout == 0 )
			{
				return( 8 );
			}
		}

		// Clear AF flag for next communication
		I2C_ClearFlag( I2C1, I2C_FLAG_AF );
	}

	return( 0 );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: get_time
 * Description		: fetch time from I2C real time clock
 * Input			: None
 * Return			: 0 if success
 * Note				: DO NOT CALL ROUTINE OUTSIDE OF vRTCtask
 *-----------------------------------------------------------*/
unsigned int get_time( void )
{
	unsigned int ret;
	unsigned char currentTime[8];

	ret = get_data( IIC_CLOCK_SECONDS_ADDRESS, &currentTime[0], 7 );

	if( ret == 0 )
	{
		ClockRam[0] = currentTime[0] & 0x7f; // seconds
		ClockRam[1] = currentTime[1];        // minutes
		ClockRam[2] = currentTime[2] & 0x3f; // hours
		ClockRam[3] = currentTime[3] & 0x07; // day
		ClockRam[4] = currentTime[4];        // date
		ClockRam[5] = currentTime[5] & 0x1f; // month
		ClockRam[6] = currentTime[6];        // year
	}

	return( ret );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: set_time
 * Description		: write clock data to I2C real time clock
 * Input			:
 * Return			: 0 if success
 * Note				: DO NOT CALL ROUTINE OUTSIDE OF vTWItask
 *-----------------------------------------------------------*/
unsigned int set_time( void  )
{
	unsigned int ix;

	// check if bus is busy, 50ms timeout
	if( I2C1_GetFlagStatus_TimeOut( I2C_FLAG_BUSY, SET, 50 ) == ERROR )
	{
		return( 1 );
	}

	// Set START condition
	I2C_GenerateSTART( I2C1, ENABLE );

	// Test on EV5 and clear it (cleared by reading SR1 then writing to DR), 50ms timeout
	if( I2C1_CheckEvent_TimeOut( I2C_EVENT_MASTER_MODE_SELECT, 50 ) == ERROR )
	{
		return( 2 );
	}

	// send address
	I2C_Send7bitAddress( I2C1, IIC_CLOCK_ADDRESS, I2C_Direction_Transmitter );

	// Test on EV6 and clear it, 50ms timeout
	if( I2C1_CheckEvent_TimeOut( I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, 50 ) == ERROR )
	{
		return( 3 );
	}

	// Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read)
	(void)I2C1->SR2;

	// Send the internal address to write to
	I2C_SendData( I2C1, IIC_CLOCK_SECONDS_ADDRESS );

	// Test on EV8 and clear it, 50ms timeout
	if( I2C1_CheckEvent_TimeOut( I2C_EVENT_MASTER_BYTE_TRANSMITTING, 50 ) == ERROR )
	{
		return( 4 );
	}

	for( ix = 0; ix < 9; ix++ )
	{
		// send byte
		I2C_SendData( I2C1, ClockRam[ix] );

		// Wait for the byte to be received, 50ms timeout
		if( I2C1_GetFlagStatus_TimeOut( I2C_FLAG_BTF, RESET, 50 ) == ERROR )
		{
			return( 5 );
		}
	}

	// Send STOP Condition
	I2C_GenerateSTOP( I2C1, ENABLE );

	return( 0 );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: vTWI_task
 * Description		: main TWI task
 * Input			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
static void vTWI_task( void *pvParameters )
{
	static unsigned int pass;
	portTickType xLastWakeTime;

	// Initialise RTC & I2C1 peripheral interface
	I2CMode = I2C_CLOCK_INIT;

	// set current time
	xLastWakeTime = xTaskGetTickCount();

	while( 1 )
	{
		// every 500ms
		vTaskDelayUntil( &xLastWakeTime, (500 / portTICK_RATE_MS) );

		switch( I2CMode )
		{
		case I2C_CLOCK_RUN:
			if( iic_get_error == 0 )
			{
				iic_get_error = get_time();
			}
			else
			{
				I2CMode = I2C_CLOCK_ERR;
			}
			break;

		case I2C_CLOCK_HOLD:
			break;

		case I2C_CLOCK_SET:
			iic_get_error = set_time();
			if( iic_get_error == 0 )
			{
				I2CMode = I2C_CLOCK_RUN;
			}
			else
			{
				iic_get_error += 20;
				I2CMode = I2C_CLOCK_ERR;
			}
			break;

		case I2C_CLOCK_ERR:
			pass = 8;
			iic_error_count++;

			I2C_GenerateSTOP( I2C1, ENABLE );
			I2C_SoftwareResetCmd( I2C1, ENABLE );
			I2C_SoftwareResetCmd( I2C1, DISABLE );

			I2CMode = I2C_CLOCK_WAIT;
			break;

		case I2C_CLOCK_WAIT:
			if( pass )
			{
				pass--;
			}
			else
			{
				I2CMode = I2C_CLOCK_INIT;
			}
			break;

		default:
		case I2C_CLOCK_INIT:
			// Initialise RTC & I2C1 peripheral interface
			InitialiseRTC();
			iic_get_error = 0;
			I2CMode = I2C_CLOCK_RUN;
			break;
		}
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: vStartTWITask
 * Description		: initialise and create RTC tasks
 * Input			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void vStartTWITask( unsigned portBASE_TYPE uxPriority )
{

	iic_error_count = 0;

	// create RTC operating task
	xTaskCreate( vTWI_task, "TWI", twiSTACK_SIZE, NULL, uxPriority, NULL );
}
/*-----------------------------------------------------------*/

