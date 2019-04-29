/*
 * COMtask.h
 *
 *  Created on: 26 May 2016
 *      Author: cparsons
 */

#ifndef COMTASK_H_
#define COMTASK_H_

// defines
/*-----------------------------------------------------------*/
#define mainCOM_TASK_PRIORITY (tskIDLE_PRIORITY + 2) // else peek loop will block other lower priority tasks
#define comRx_STACK_SIZE configSPRINT_STACK_SIZE // uses sprintf

#define comNO_BLOCK 0 // no block, returns imediately
#define comRX_BLOCK_TIME ((portTickType)(50 / portTICK_RATE_MS)) // 50ms
#define comTX_BLOCK_TIME ((portTickType)(50 / portTICK_RATE_MS)) // 50ms

// usart definitions
#define SOH_X 0x01
#define EOT_X 0x04
#define ACK_X 0x06
#define NAK_X 0x15
#define C_X   0x43

//#define BAUD9600
//#define BAUD19200
//#define BAUD38400
//#define BAUD115200
/*-----------------------------------------------------------*/

// variables
/*-----------------------------------------------------------*/
extern xQueueHandle RxCOM0, TxCOM0;
extern xQueueHandle RxCOM1, TxCOM1;
/*-----------------------------------------------------------*/

// function prototypes
/*-----------------------------------------------------------*/
// extern void InitialiseCOM0( void );
// extern void InitialiseCOM1( void );
// static void vCOM0RxTask( void *pvParameters );
// static void vCOM1RxTask( void *pvParameters );
extern unsigned long int SendString( xQueueHandle TxCOM, unsigned char *pt );
extern void vStartComTasks( unsigned portBASE_TYPE uxPriority );
/*-----------------------------------------------------------*/

#endif /* COMTASK_H_ */
