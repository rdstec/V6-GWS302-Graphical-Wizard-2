// Graphic Wizard Serial Coms Routines

// Include Standard files
#include "global.h"

// global variables
/*-----------------------------------------------------------*/
xQueueHandle RxCOM0, TxCOM0;
xQueueHandle RxCOM1, TxCOM1;

// functions
/*-----------------------------------------------------------*/
void InitialiseCOM0( void )
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RxCOM0 = xQueueCreate( COM_MAXRX, (unsigned portBASE_TYPE)sizeof( unsigned char ) );
	TxCOM0 = xQueueCreate( COM_MAXTX, (unsigned portBASE_TYPE)sizeof( unsigned char ) );

	// SystemInit_GPIO() enables peripherals and timer clocks & maps io pins

	USART_StructInit( &USART_InitStructure );
	// USART_InitStruct->USART_BaudRate = 9600;
	// USART_InitStruct->USART_WordLength = USART_WordLength_8b;
	// USART_InitStruct->USART_StopBits = USART_StopBits_1;
	// USART_InitStruct->USART_Parity = USART_Parity_No;
	// USART_InitStruct->USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// USART_InitStruct->USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_InitStructure.USART_BaudRate = 115200;
	USART_Init( USART1, &USART_InitStructure );
	USART_ITConfig( USART1, USART_IT_RXNE, ENABLE );

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_KERNEL_INTERRUPT_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	USART_Cmd( USART1, ENABLE );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void InitialiseCOM1( void )
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RxCOM1 = xQueueCreate( COM_MAXRX, (unsigned portBASE_TYPE)sizeof( unsigned char ) );
	TxCOM1 = xQueueCreate( COM_MAXTX, (unsigned portBASE_TYPE)sizeof( unsigned char ) );

	// SystemInit_GPIO() enables peripherals and timer clocks & maps io pins

	USART_StructInit( &USART_InitStructure );
	// USART_InitStruct->USART_BaudRate = 9600;
	// USART_InitStruct->USART_WordLength = USART_WordLength_8b;
	// USART_InitStruct->USART_StopBits = USART_StopBits_1;
	// USART_InitStruct->USART_Parity = USART_Parity_No;
	// USART_InitStruct->USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// USART_InitStruct->USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_InitStructure.USART_BaudRate = 115200;
	USART_Init( USART2, &USART_InitStructure );
	USART_ITConfig( USART2, USART_IT_RXNE, ENABLE );

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_KERNEL_INTERRUPT_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	USART_Cmd( USART2, ENABLE );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void USART1_IRQHandler( void )
{
	long xHigherPriorityTaskWoken = pdFALSE;
	char cChar;

	if( USART_GetITStatus( USART1, USART_IT_TXE ) == SET )
	{
		// The interrupt was caused by the THR becoming empty
		// Are there any more characters to transmit?
		if( xQueueReceiveFromISR( TxCOM0, &cChar, &xHigherPriorityTaskWoken ) )
		{
			// A character was retrieved from the buffer so can be sent to the THR now
			USART_SendData( USART1, cChar );
		}
		else
		{
			USART_ITConfig( USART1, USART_IT_TXE, DISABLE );
		}
	}

	if( USART_GetITStatus( USART1, USART_IT_RXNE ) == SET )
	{
		cChar = USART_ReceiveData( USART1 );
		xQueueSendFromISR( RxCOM0, &cChar, &xHigherPriorityTaskWoken );
	}

	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void USART2_IRQHandler( void )
{
	long xHigherPriorityTaskWoken = pdFALSE;
	char cChar;

	if( USART_GetITStatus( USART2, USART_IT_TXE ) == SET )
	{
		// The interrupt was caused by the THR becoming empty
		// Are there any more characters to transmit?
		if( xQueueReceiveFromISR( TxCOM1, &cChar, &xHigherPriorityTaskWoken ) )
		{
			// A character was retrieved from the buffer so can be sent to the THR now
			USART_SendData( USART2, cChar );
		}
		else
		{
			USART_ITConfig( USART2, USART_IT_TXE, DISABLE );
		}
	}

	if( USART_GetITStatus( USART2, USART_IT_RXNE ) == SET )
	{
		cChar = USART_ReceiveData( USART2 );
		xQueueSendFromISR( RxCOM1, &cChar, &xHigherPriorityTaskWoken );
	}

	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
static void vCOM0RxTask( void *pvParameters )
{
	unsigned char *pt, combuffer[COM_MAXRX];
	int count;
	
	count = 0;
	pt = &combuffer[0];
	
	while( 1 )
	{
  		if( xQueueReceive( RxCOM0, (void *)pt, portMAX_DELAY ) == pdTRUE )
		{
  			// check for newline
  			if( *pt == '\n' ) 
  			{
				// strip control characters
  				while( (*pt < 0x20) && count )
  				{
  					count--;
  					pt--;
  				}
  				
  				// if string is not empty increment pointer
  				if( *pt >= 0x20 )  
  				{
  					count = COM_MAXRX;
  					pt++;
  				}
  				
  				// terminate string
				*pt = 0x00;
  				
  				// copy to message buffer
  				strcpy( COM0_message, combuffer );
  				
  	  			COM0_message_received = 1;
  			}
  			
  			count++;
  			if( count >= COM_MAXRX ) // set to start if no space left in buffer
  			{
  				count = 0;
  				pt = &combuffer[0];
  			}
  			else
  			{
  				pt++;
  			}
		}
	}

}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
static void vCOM1RxTask( void *pvParameters )
{
	unsigned char *pt, combuffer[COM_MAXRX];
	int count;
	
	count = 0;
	pt = &combuffer[0];
	
	while( 1 )
	{
  		if( xQueueReceive( RxCOM1, (void *)pt, portMAX_DELAY ) == pdTRUE )
		{
  			// check for newline
  			if( *pt == '\n' ) 
  			{
				// strip control characters
  				while( (*pt < 0x20) && count )
  				{
  					count--;
  					pt--;
  				}
  				
  				// if string is not empty increment pointer
  				if( *pt >= 0x20 )  
  				{
  					count = COM_MAXRX;
  					pt++;
  				}
  				
  				// terminate string
				*pt = 0x00;
  				
  				// copy to message buffer
  				strcpy( COM1_message, combuffer );
  				
  	  			COM1_message_received = 1;
  			}
  			
  			count++;
  			if( count >= COM_MAXRX ) // set to start if no space left in buffer
  			{
  				count = 0;
  				pt = &combuffer[0];
  			}
  			else
  			{
  				pt++;
  			}
		}
	}

}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
unsigned long int SendString( xQueueHandle TxCOM, unsigned char *pt )
{
	
	while( *pt )
	{
		// post to queue, if block time expires return with error
  		if( xQueueSend( TxCOM, pt, comTX_BLOCK_TIME ) == pdTRUE )
  		{
			/* Turn on the Tx interrupt so the ISR will remove the character from the
			queue and send it. This does not need to be in a critical section as
			if the interrupt has already removed the character the next interrupt
			will simply turn off the Tx interrupt again. */
  			if( TxCOM == TxCOM0 )
  			{
  				USART_ITConfig( USART1, USART_IT_TXE, ENABLE );
  			}
  			else if( TxCOM == TxCOM1 )
  			{
  				USART_ITConfig( USART2, USART_IT_TXE, ENABLE );
  			}

			pt++;
  		}
  		else
  		{
  			return( pdFAIL );
  		}
	}
	
	// loop until queue empty - if queue block timeout
	//while( uxQueueMessagesWaiting( TxCOM ) > 0 )
	//{
	//}

	// loop while transmitter is busy
	//while( (USART2->SR & USART_FLAG_TC) == 0 )
	//{
	//}

	return( pdPASS );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void vStartComTasks( unsigned portBASE_TYPE uxPriority )
{
	// Initialise Serial peripheral
	InitialiseCOM0();
	InitialiseCOM1();

	xTaskCreate( vCOM0RxTask, "COM0Rx", comRx_STACK_SIZE, NULL, uxPriority,  NULL );
	xTaskCreate( vCOM1RxTask, "COM1Rx", comRx_STACK_SIZE, NULL, uxPriority,  NULL );
}
/*-----------------------------------------------------------*/
