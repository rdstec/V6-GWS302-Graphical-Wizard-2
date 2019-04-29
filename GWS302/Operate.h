/*
 * operate.h
 *
 *  Created on: 31 May 2016
 *      Author: cparsons
 */

#ifndef OPERATE_H_
#define OPERATE_H_

// defines
/*-----------------------------------------------------------*/
#define mainOPERATE_TASK_PRIORITY (tskIDLE_PRIORITY + 2) // watchdog task
#define configOPERATE_STACK_SIZE configSPRINT_STACK_SIZE // uses sprintf
/*-----------------------------------------------------------*/

// variables
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

// functions
/*-----------------------------------------------------------*/
//static void vOperateTask( void *pvParameters );
extern void vStartOperateTask( unsigned portBASE_TYPE uxPriority );
/*-----------------------------------------------------------*/

#endif /* OPERATE_H_ */
