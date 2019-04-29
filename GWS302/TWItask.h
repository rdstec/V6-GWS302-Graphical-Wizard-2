/*
 * TWItask.h
 *
 *  Created on: 30 Sep 2013
 *      Author: Chris
 */

#ifndef TWITASK_H_
#define TWITASK_H_


// defines
/*-----------------------------------------------------------*/
#define mainTWI_TASK_READ_PRIORITY (tskIDLE_PRIORITY + 3)
#define twiSTACK_SIZE configMINIMAL_STACK_SIZE

#define IIC_CLOCK_ADDRESS 0xde
#define IIC_CLOCK_SECONDS_ADDRESS 0x00
#define IIC_CLOCK_MINUTES_ADDRESS 0x01
#define IIC_CLOCK_HOURS_ADDRESS 0x02
#define IIC_CLOCK_DAY_ADDRESS 0x03
#define IIC_CLOCK_DATE_ADDRESS 0x04
#define IIC_CLOCK_MONTH_ADDRESS 0x05
#define IIC_CLOCK_YEARS_ADDRESS 0x06
#define IIC_CLOCK_CONTROL_ADDRESS 0x07
#define IIC_CLOCK_CALIBRATION_ADDRESS 0x08
#define IIC_CLOCK_ALARM0_ADDRESS 0x0a
#define IIC_CLOCK_ALARM1_ADDRESS 0x11
#define IIC_CLOCK_TIME_STAMP_ADDRESS 0x18
#define IIC_CLOCK_MEMORY_ADDRESS 0x20

#define IIC_CLOCK_START 0x80
#define IIC_CLOCK_OSCON 0x20
#define IIC_CLOCK_VBAT  0x10
#define IIC_CLOCK_BATEN 0x08
#define IIC_CLOCK_LEAPYEAR 0x20

#define IIC_CLOCK_CONOUT 0x80
#define IIC_CLOCK_CONSQWE 0x40
#define IIC_CLOCK_CONALM1 0x20
#define IIC_CLOCK_CONALM0 0x10
#define IIC_CLOCK_CONEXOS 0x08
#define IIC_CLOCK_CONRS2 0x04
#define IIC_CLOCK_CONRS1 0x02
#define IIC_CLOCK_CONRS0 0x01

#define I2C_EEPROM_ADDRESS 0xae

#define I2C_CLOCK_RUN				0x00
#define I2C_CLOCK_HOLD				0x01
#define I2C_CLOCK_SET				0x02
#define I2C_CLOCK_ERR				0x03
#define I2C_CLOCK_WAIT				0x04
#define I2C_CLOCK_INIT				0x05
/*-----------------------------------------------------------*/

// constants
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

// variables
/*-----------------------------------------------------------*/
extern unsigned char I2CMode;
extern unsigned char ClockRam[9];

extern unsigned int iic_get_error;
extern unsigned int iic_error_count;

struct structTimeOutError {
	unsigned int start;
	unsigned int count;
	unsigned int time;
};
extern struct structTimeOutError T3TimeOut;

/*-----------------------------------------------------------*/

// function prototypes
#ifndef WINDOWS
/*-----------------------------------------------------------*/

extern void InitialiseRTC( void );
// static void vTWItask( void *pvParameters );
extern void vStartTWITask( unsigned portBASE_TYPE uxPriority );
/*-----------------------------------------------------------*/
#endif

#endif /* TWITASK_H_ */
