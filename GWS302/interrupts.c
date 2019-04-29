// Graphic Wizard Timer Interrupt File

// Include Standard files
#include "global.h"

// global variables
/*-----------------------------------------------------------*/
// constant
/*-----------------------------------------------------------*/

// zeroed
/*-----------------------------------------------------------*/

// functions
/*-----------------------------------------------------------
 * Function Name	: EXTI2_IRQHandler
 * Description		: power fail pin interrupt
 * 					: switches to power task to enable data save
 * Input			: None
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void EXTI2_IRQHandler( void )
{
	portBASE_TYPE SwitchRequired = pdFALSE;

	// only if task is ready
	if( PowerTaskReady == PT_READY )
	{
		SwitchRequired = xTaskResumeFromISR( POWERtaskhandle );

		portEND_SWITCHING_ISR( SwitchRequired );
	}
	else
	{
		PowerTaskReady = PT_INTERRUPT;
	}

	EXTI_ClearITPendingBit( EXTI_Line2 );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: SetUpPowerFailInterrupt
 * Description		: power fail pin interrupt setup
 * Input			: None
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void SetUpPowerFailInterrupt( void )
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// setup /PFAIL pin interrupt
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_SYSCFG, ENABLE );
	SYSCFG_EXTILineConfig( EXTI_PortSourceGPIOB, EXTI_PinSource2 );

	// enable input pin edge triggered interrupt
	EXTI_StructInit( &EXTI_InitStructure );
	//EXTI_InitStructure->EXTI_Line = EXTI_LINENONE;
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	//EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	//EXTI_InitStructure->EXTI_LineCmd = DISABLE;
	EXTI_Init( &EXTI_InitStructure );

	// set task not ready
 	PowerTaskReady = (unsigned char)~PT_READY;

 	// Enable EXTI interrupt for line 0
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // HIGHEST PRIORITY
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );
}
/*-----------------------------------------------------------*/

#ifdef POWERFAILVOLTS
/*-----------------------------------------------------------*/
//* Function Name       : ADC0_irq_handler
//* Object              : C handler interrupt function
//* checks supply voltage
/*-----------------------------------------------------------*/
/* The interrupt is defined as "naked" as the full context is saved on entry as part of the context switch. */
void ADC0_irq_handler( void ) __attribute__((naked));
/* function is ordinary c routine */
void ADC0_irq_function( void );

void ADC0_irq_handler( void )
{
    /* Save the context of the interrupted task. */
    portSAVE_CONTEXT();
    
    /* Call the handler function.  This must be a separate 
    function unless you can guarantee that handling the 
    interrupt will never use any stack space. */
    ADC0_irq_function();

    /* Restore the context of the task that is going to 
    execute next. This might not be the same as the originally 
    interrupted task.*/
    portRESTORE_CONTEXT();
}

void ADC0_irq_function( void )
{
static unsigned char Vcount;

register unsigned long int ulStatus;
portBASE_TYPE SwitchRequired = pdFALSE;

EOC7_interrupt_count++;

// reading conversion register clears eoc flag
ulStatus = AT91C_BASE_ADC0->ADC_CDR7;

if( ulStatus < (unsigned int)ATODVMIN )
  {
  Vcount++;
  if( Vcount > 2 )
    {
    // Resume Power task - save factors and turn off
    PowerSwitchedOff = 0;
    SwitchRequired = xTaskResumeFromISR( POWERtaskhandle );
    }	
  }
else if( Vcount )
  {
  Vcount--;	
  }

// clear overrun errors
ulStatus = AT91C_BASE_ADC0->ADC_SR;

if( SwitchRequired )											\
{
	portYIELD_FROM_ISR();
}

// reset interrupt, any value can be written so use periperal ID 
AT91C_BASE_AIC->AIC_EOICR = AT91C_ID_ADC0;
} 
/*-----------------------------------------------------------*/
#endif
