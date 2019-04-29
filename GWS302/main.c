/*
 *
 * STM32F2 GWiz Main Code
 *
 */

// global include
#include "global.h"

// defines
/*-----------------------------------------------------------*/

// variables
/*-----------------------------------------------------------*/
// The queue used to send messages to the Debug task
xQueueHandle xDebugQueue;

/*-----------------------------------------------------------*/

// constants
/*-----------------------------------------------------------*/
unsigned char const binHex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };


// function prototypes
/*-----------------------------------------------------------*/

/* Very simple task that toggles an LED */
static void prvDebugTask( void *pvparameters );
/*-----------------------------------------------------------*/

// functions
/*-----------------------------------------------------------*/
void main( void ) __attribute__ ((noreturn));

void main( void ) // never returns, so void - I think we can also tell compiler there is no return for this function
{
	// set vector table
	NVIC_SetVectorTable( NVIC_VectTab_FLASH, VECTOROFFSET_MAIN );

	// start the main tasks
	vStartPowerTask( mainPOWER_TASK_PRIORITY );
	vStartTWITask( mainTWI_TASK_READ_PRIORITY );
	vStartComTasks( mainCOM_TASK_PRIORITY );
	vStartCANTasks( mainCAN_TASK_PRIORITY );
	vStartOperateTask( mainOPERATE_TASK_PRIORITY );
	vStartDisplayTask( mainDISPLAY_TASK_PRIORITY );

	// Start the standard demo tasks
	// These are just here to exercise the kernel port and
	// provide examples of how the FreeRTOS API can be used
    vStartGenericQueueTasks( mainGEN_QUEUE_TASK_PRIORITY );
    vStartQueuePeekTasks();
	vStartBlockingQueueTasks( mainBLOCK_Q_PRIORITY );
	vStartSemaphoreTasks( mainSEM_TEST_PRIORITY );
	vStartPolledQueueTasks( mainQUEUE_POLL_PRIORITY );
	vStartIntegerMathTasks( mainINTEGER_TASK_PRIORITY );
    vStartRecursiveMutexTasks();

	// Create the queue used by the demo task, Debug messages are received via this queue
	xDebugQueue = xQueueCreate( mainQUEUE_SIZE, sizeof( char * ) );

	// Start the Debug task
	xTaskCreate( prvDebugTask, ( signed char * ) "Debug", configSPRINT_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY, NULL );

	// setup SPI1 peripheral for external flash
	SPI_Setup( CS_FLASH );

	// read flash status register - only returns if no write in progress, so no timeout
	SPI_FLASH_WaitForWriteEnd();

	// check spi flash id
	spiFlashID.ul = SPI_FLASH_CheckID();

	// restore factors and data held in external SPI flash memory before starting scheduler
	SPI_FLASH_DataRestore();

	// LED_RED on
	OUTPUT_LED_RED_ON;

	// setup power fail interrupt - can't seem to set NVIC in task
	// power task is not ready for about 40us
	SetUpPowerFailInterrupt();

	/* Configure the high frequency interrupt used to measure the interrupt
	jitter time.  When debugging it can be helpful to comment this line out
	to prevent the debugger repeatedly going into the interrupt service
	routine. */
	vSetupHighFrequencyTimer();

    /* Start the scheduler - does not return unless vTaskEndScheduler called */
	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task.  The idle task is created within vTaskStartScheduler(). */
	while( 1 )
	{
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
static void prvDebugTask( void *pvParameters )
{
	char *pcMessage;

#ifdef DEBUG
	char tstring[128];
#endif

	while( 1 )
	{
		// check for message in queue, wait until one arrives
		if( xQueueReceive( xDebugQueue, &pcMessage, portMAX_DELAY ) )
		{
#ifdef DEBUG
			// Display the received text, and the max jitter value
			sprintf( tstring, "%s [%uns] ",
				pcMessage,
				usMaxJitter * mainNS_PER_CLOCK
				);
			SendString( TxCOM0, tstring );

			SendString( TxCOM0, "\r\n" );
#endif
		} // if( xQueueReceive( xDebugQueue, &pcMessage, portMAX_DELAY ) )
	} // while( 1 )
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void vApplicationTickHook( void )
{
	static unsigned long ulTicksSinceLastDisplay = 0;

	char *pcMessage;
	portBASE_TYPE xHigherPriorityTaskWoken;

	/* Called from every tick interrupt as described in the comments at the top	of this file.
	Have enough ticks passed to make it	time to perform our health status check again? */
	ulTicksSinceLastDisplay++;
	if( ulTicksSinceLastDisplay >= mainCHECK_DELAY )
	{
		/* Reset the counter so these checks run again in mainCHECK_DELAY
		ticks time. */
		ulTicksSinceLastDisplay = 0;

		/* Has an error been found in any task? */
		if( xAreGenericQueueTasksStillRunning() != pdTRUE )
		{
			pcMessage = "ERROR: GEN Q";
		}
		else if( xAreQueuePeekTasksStillRunning() != pdTRUE )
		{
			pcMessage = "ERROR: PEEK Q";
		}
		else if( xAreBlockingQueuesStillRunning() != pdTRUE )
		{
			pcMessage = "ERROR: BLOCK Q";
		}
	    else if( xAreSemaphoreTasksStillRunning() != pdTRUE )
	    {
	        pcMessage = "ERROR: SEMAPHR";
	    }
	    else if( xArePollingQueuesStillRunning() != pdTRUE )
	    {
	        pcMessage = "ERROR: POLL Q";
	    }
	    else if( xAreIntegerMathsTaskStillRunning() != pdTRUE )
	    {
	        pcMessage = "ERROR: INT MATH";
	    }
	    else if( xAreRecursiveMutexTasksStillRunning() != pdTRUE )
	    {
	    	pcMessage = "ERROR: REC MUTEX";
	    }
	    else
	    {
	    	pcMessage = "Status: PASS";

	    }

		// Send the message to the Debug Queue
		// the xHigherPriorityTaskWoken parameter is not actually used here
		// as this function is running in the tick interrupt anyway - but
		// it must still be supplied
		xHigherPriorityTaskWoken = pdFALSE;
		xQueueSendFromISR( xDebugQueue, &pcMessage, &xHigherPriorityTaskWoken );
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
	/* This function will get called if a task overflows its stack.   If the
	parameters are corrupt then inspect pxCurrentTCB to find which was the
	offending task. */

	( void ) pxTask;
	( void ) pcTaskName;

	for( ;; );
}
/*-----------------------------------------------------------*/

#ifdef DEBUG
/*-----------------------------------------------------------*/
void debug_setup( void )
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_USART1, ENABLE );
	RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_GPIOA, ENABLE );

	GPIO_PinAFConfig( GPIOA, GPIO_PinSource9, GPIO_AF_USART1 ); // sets AFR[]
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource10, GPIO_AF_USART1 ); // sets AFR[]

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init( GPIOA, &GPIO_InitStruct );

	USART_DeInit( USART1 );

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init( USART1, &USART_InitStructure );

	USART_Cmd( USART1, ENABLE );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void debug_message( unsigned char *pt )
{

	while( *pt )
	{
		while( USART_GetFlagStatus( USART1, USART_FLAG_TXE ) != SET )
		{
		}

		USART_SendData( USART1, *pt++ );
	}
}
/*-----------------------------------------------------------*/

// 32 bit int to hex string 0x00000000 format
/*-----------------------------------------------------------*/
void IntToHexString( unsigned char *pt, unsigned int ul )
{
	unsigned char *pd;

	pd = pt + 10;

	*pd = 0x00; // 10
	pd--;
	*pd = binHex[ul % 0x10]; // 9
	ul = ul / 0x10;
	pd--;
	*pd = binHex[ul % 0x10]; // 8
	ul = ul / 0x10;
	pd--;
	*pd = binHex[ul % 0x10]; // 7
	ul = ul / 0x10;
	pd--;
	*pd = binHex[ul % 0x10]; // 6
	ul = ul / 0x10;
	pd--;
	*pd = binHex[ul % 0x10]; // 5
	ul = ul / 0x10;
	pd--;
	*pd = binHex[ul % 0x10]; // 4
	ul = ul / 0x10;
	pd--;
	*pd = binHex[ul % 0x10]; // 3
	ul = ul / 0x10;
	pd--;
	*pd = binHex[ul % 0x10]; // 2
	pd--;
	*pd = 'x'; // 1
	pd--;
	*pd = '0'; // 0
}
/*-----------------------------------------------------------*/


// 32 bit int to decimal format
/*-----------------------------------------------------------*/
void IntToDecString( unsigned char *pt, unsigned int ul )
{
	unsigned char *pd;

	pd = pt + 11;
	*pd = 0x00; // 11
	pd--;
	*pd = binHex[ul % 10]; // 10
	ul = ul / 10;
	pd--;
	*pd = binHex[ul % 10]; // 9
	ul = ul / 10;
	pd--;
	*pd = binHex[ul % 10]; // 8
	ul = ul / 10;
	pd--;
	*pd = binHex[ul % 10]; // 7
	ul = ul / 10;
	pd--;
	*pd = binHex[ul % 10]; // 6
	ul = ul / 10;
	pd--;
	*pd = binHex[ul % 10]; // 5
	ul = ul / 10;
	pd--;
	*pd = binHex[ul % 10]; // 4
	ul = ul / 10;
	pd--;
	*pd = binHex[ul % 10]; // 3
	ul = ul / 10;
	pd--;
	*pd = binHex[ul % 10]; // 2
	ul = ul / 10;
	pd--;
	*pd = binHex[ul % 10]; // 1

	// lzb
	while( *pd == '0' )
	{
		pd++;
	}

	if( *pd == 0x00 )
	{
		pd--;
	}

	// copy
	while( *pd )
	{
		*pt++ = *pd++;
	}
	*pt = *pd;
}
/*-----------------------------------------------------------*/
#endif

#ifdef USE_FULL_ASSERT
/*-----------------------------------------------------------*/

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
	char tstring[128];

	// if port not enabled
	if( (USART1->CR1 & USART_CR1_UE) == 0 )
	{
		debug_setup();
	}

	sprintf( tstring, "Wrong parameters value: file %s on line %d\r\n", file, line );

	debug_message( tstring );

	/* Infinite loop */
	while( 1 )
	{
	}
}
/*-----------------------------------------------------------*/
#endif

/*-----------------------------------------------------------*/
void NMI_Handler( void )
{
#ifdef DEBUG
	debug_setup();

	debug_message( "\r\nNMI Handler\r\n" );
#endif

	// stay here until watchdog reset after about 2 seconds
	while( 1 )
	{
	}
	// NVIC_SystemReset(); Privileged only
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void HardFault_Handler( void )
{
	__asm volatile
	(
		"	TST		LR, #4	\n"
		"	ITE		EQ	\n"
		"	MRSEQ	R0, MSP	\n"
		"	MRSNE	R0, PSP	\n"
		"	B		hard_fault_handler_c"
	);
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void hard_fault_handler_c( unsigned int * hardfault_args )
{
#ifdef DEBUG
	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;

	char ErrorMessage[32];

	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);

	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);

	debug_setup();

	debug_message( "\r\nHard Fault\r\n" );

	debug_message( "LR [R14] = " );
	IntToHexString( ErrorMessage, stacked_lr );
	debug_message( ErrorMessage );
	debug_message( " subroutine call return address\r\n" );

	debug_message( "PC [R15] = " );
	IntToHexString( ErrorMessage, stacked_pc );
	debug_message( ErrorMessage );
	debug_message( " program counter\r\n" );

	debug_message( "R0  = " );
	IntToHexString( ErrorMessage, stacked_r0 );
	debug_message( ErrorMessage );
	debug_message( "\r\n" );

	debug_message( "R1  = " );
	IntToHexString( ErrorMessage, stacked_r1 );
	debug_message( ErrorMessage );
	debug_message( "\r\n" );

	debug_message( "R2  = " );
	IntToHexString( ErrorMessage, stacked_r2 );
	debug_message( ErrorMessage );
	debug_message( "\r\n" );

	debug_message( "R3  = " );
	IntToHexString( ErrorMessage, stacked_r3 );
	debug_message( ErrorMessage );
	debug_message( "\r\n" );

	debug_message( "R12 = " );
	IntToHexString( ErrorMessage, stacked_r12 );
	debug_message( ErrorMessage );
	debug_message( "\r\n" );

	debug_message( "PSR = " );
	IntToHexString( ErrorMessage, stacked_psr );
	debug_message( ErrorMessage );
	debug_message( "\r\n" );

	debug_message( "BFAR = " );
	IntToHexString( ErrorMessage, (*((volatile unsigned long *)(0xE000ED38))) );
	debug_message( ErrorMessage );
	debug_message( "\r\n" );

	debug_message( "CFSR = " );
	IntToHexString( ErrorMessage, (*((volatile unsigned long *)(0xE000ED28))) );
	debug_message( ErrorMessage );
	debug_message( "\r\n" );

	debug_message( "HFSR = " );
	IntToHexString( ErrorMessage, (*((volatile unsigned long *)(0xE000ED2C))) );
	debug_message( ErrorMessage );
	debug_message( "\r\n" );

	debug_message( "DFSR = " );
	IntToHexString( ErrorMessage, (*((volatile unsigned long *)(0xE000ED30))) );
	debug_message( ErrorMessage );
	debug_message( "\r\n" );

	debug_message( "AFSR = " );
	IntToHexString( ErrorMessage, (*((volatile unsigned long *)(0xE000ED3C))) );
	debug_message( ErrorMessage );
	debug_message( "\r\n" );

	debug_message( "SCB_SHCSR = " );
	IntToHexString( ErrorMessage, SCB->SHCSR );
	debug_message( ErrorMessage );
	debug_message( "\r\n" );
#endif
	// stay here until watchdog reset after about 2 seconds
	while( 1 )
	{
	}
	// NVIC_SystemReset(); Privileged only
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void MemManage_Handler( void )
{
	__asm volatile
	(
		"	TST		LR, #4	\n"
		"	ITE		EQ	\n"
		"	MRSEQ	R0, MSP	\n"
		"	MRSNE	R0, PSP	\n"
		"	B		MemManage_Handler_c"
	);
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void MemManage_Handler_c( unsigned int * hardfault_args )
{
#ifdef DEBUG
	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;

	char ErrorMessage[32];

	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);

	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);

	debug_setup();

	debug_message( "\r\nMemory Fault\r\n" );

	debug_message( " LR [R14] = " );
	IntToHexString( ErrorMessage, stacked_lr );
	debug_message( ErrorMessage );
	debug_message( " subroutine call return address\r\n" );

	debug_message( " PC [R15] = " );
	IntToHexString( ErrorMessage, stacked_pc );
	debug_message( ErrorMessage );
	debug_message( " program counter\r\n" );

	debug_message( " CFSR = " );
	IntToHexString( ErrorMessage, SCB->CFSR );
	debug_message( ErrorMessage );
	debug_message( " configurable fault status register\r\n" );

	debug_message( " MMFAR = " );
	IntToHexString( ErrorMessage, SCB->MMFAR );
	debug_message( ErrorMessage );
	debug_message( " memory fault address register\r\n\r\n" );
#endif
	// stay here until watchdog reset after about 2 seconds
	while( 1 )
	{
	}
	// NVIC_SystemReset(); Privileged only
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void BusFault_Handler( void )
{
#ifdef DEBUG
	debug_setup();

	debug_message( "\r\nBus Fault\r\n" );
#endif
	// stay here until watchdog reset after about 2 seconds
	while( 1 )
	{
	}
	// NVIC_SystemReset(); Privileged only
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void UsageFault_Handler( void )
{
#ifdef DEBUG
	debug_setup();

	debug_message( "\r\nUsage Fault\r\n" );
#endif

	// stay here until watchdog reset after about 2 seconds
	while( 1 )
	{
	}
	// NVIC_SystemReset(); Privileged only
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void DebugMon_Handler( void )
{
#ifdef DEBUG
	debug_setup();

	debug_message( "\r\nDebug Monitor\r\n" );
#endif
	// stay here until watchdog reset after about 2 seconds
	while( 1 )
	{
	}
	// NVIC_SystemReset(); Privileged only
}
/*-----------------------------------------------------------*/
