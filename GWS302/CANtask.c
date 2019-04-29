// Graphic Wizard CAN File

// Include Standard files
#include "global.h"

// defines
/*-----------------------------------------------------------*/
#define CANBAUD_RDS 0
#define CANBAUD_ISO 1

// constants
/*-----------------------------------------------------------*/
/*typedef struct {
	uint16_t CAN_Prescaler;   // length of a time quantum, it ranges from 1 to 1024
	uint8_t CAN_Mode;         // CAN operating mode, this parameter can be a value of @ref CAN_operating_mode
	uint8_t CAN_SJW;          // maximum number of time quanta the CAN hardware is allowed to lengthen or shorten a bit to perform resynchronization (1 to 4)
	uint8_t CAN_BS1;          // number of time quanta in Bit Segment 1 (1-16)
	uint8_t CAN_BS2;          // number of time quanta in Bit Segment 2 (1-8)
	FunctionalState CAN_TTCM; // Enable or disable the time triggered communication mode (ENABLE/DISABLE)
	FunctionalState CAN_ABOM; // Enable or disable the automatic bus-off management (ENABLE/DISABLE)
	FunctionalState CAN_AWUM; // Enable or disable the automatic wake-up mode (ENABLE/DISABLE)
	FunctionalState CAN_NART; // Enable or disable the non-automatic retransmission mode (ENABLE/DISABLE)
	FunctionalState CAN_RFLM; // Enable or disable the Receive FIFO Locked mode (ENABLE/DISABLE)
	FunctionalState CAN_TXFP; // Enable or disable the transmit FIFO priority (ENABLE/DISABLE)
	}*/
CAN_InitTypeDef const CAN_Init_settings[2] = {
	// RDS CAN - set baud rate to 83.3Khz
	// sjw = 2
	// tbit = 1 +  4 + 3 =  8tq (sample point = 63%), prescaler = 30000000 / ( 8 x 83333) = 45 - ok
	// tbit = 1 +  7 + 7 = 15tq (sample point = 53%), prescaler = 30000000 / (15 x 83333) = 24 - ok
	// tbit = 1 +  7 + 8 = 16tq (sample point = 50%), prescaler = 30000000 / (16 x 83333) = 22.5 - can't do
	// tbit = 1 + 11 + 8 = 20tq (sample point = 60%), prescaler = 30000000 / (20 x 83333) = 18 - ok
	// tbit = 1 + 11 + 6 = 18tq (sample point = 66%), prescaler = 30000000 / (18 x 83333) = 20 - ok
	// Pre  Mode             SJW          BS1           BS2          TTCM     ABOM    AWUM     NART     RFLM     TXFP
    20,     CAN_Mode_Normal, CAN_SJW_2tq, CAN_BS1_11tq, CAN_BS2_6tq, DISABLE, ENABLE, DISABLE, DISABLE, DISABLE, DISABLE,

	// RDS CAN - set baud rate to 250Khz
	// sjw = 2
	// tbit = 1 +  4 + 3 =  8tq (sample point = 63%), prescaler = 30000000 / ( 8 x 250000) = 15 - ok

	// ISO CAN - set baud rate to 250Khz
	// ISO11783-2 Table A.1
	// sjw = 2
	// tbit = 1 + 12 + 3 = 16tq (sample point = 81%), prescaler = 30000000 / (16 x 250000) = 7.5 - can't do!
	// tbit = 1 + 15 + 4 = 20tq (sample point = 80%), prescaler = 30000000 / (20 x 250000) = 6 - ok
	// Pre  Mode             SJW          BS1           BS2          TTCM     ABOM    AWUM     NART     RFLM     TXFP
    6,      CAN_Mode_Normal, CAN_SJW_2tq, CAN_BS1_15tq, CAN_BS2_4tq, DISABLE, ENABLE, DISABLE, DISABLE, DISABLE, DISABLE,
	};

// global variables
/*-----------------------------------------------------------*/
xQueueHandle RxCAN0, TxCAN0;
xQueueHandle RxCAN1, TxCAN1;

unsigned int CAN0timeout, CAN1timeout;
/*-----------------------------------------------------------*/

// functions
/*-----------------------------------------------------------*/
void InitialiseCAN0( void )
{
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	union LongToChar ul;

	RxCAN0 = xQueueCreate( CAN_MAXRX, (unsigned portBASE_TYPE)sizeof( struct CanMessage ) );
	TxCAN0 = xQueueCreate( CAN_MAXTX, (unsigned portBASE_TYPE)sizeof( struct CanMessage ) );

	// clocks & pins setup in SystemInit_GPIO()

	// reset registers
	CAN_DeInit( CAN1 );

	CAN_Init( CAN1, (CAN_InitTypeDef *)&CAN_Init_settings[CANBAUD_RDS] );

	// split filters - start of filter bank for CAN1
	// 0 - 14 for CAN1, 15-27 for CAN2
	CAN_SlaveStartBank( 15 ); // Both CAN Tasks Set the same filter split point - we only need to do it in one

	// 16 bit filters
	// could combine the following two filters if a single FIFO was the destination

	// 16 bit id filters are set in the low/high parts of an extended id filter
	// either standard id 11 bits ST10,ST09,ST08,ST07,SD06,SD05,SD04,SD03,SD02,SD01,SD00, RTR, IDE,EX17,EX16,EX15
	// or extended id top 14 bits EX28,EX27,EX26,EX25,EX24,EX23,EX22,EX21,EX20,EX19,EX18, RTR, IDE,EX17,EX16,EX15
	// defined bits CAN_Id_Extended (0x04) & CAN_RTR_Remote (0x02) need shifting before using in ID mask
	// defined bits CAN_Id_Extended (0x04) & CAN_RTR_Remote (0x02) need shifting before using in ID mask
	// (CAN_Id_Extended << 1), (CAN_RTR_Remote << 3)
	// defined bits CAN_Id_Standard (0x00) & CAN_RTR_Data (0x00) don't really need shifting
	// using high 16 bits here because we can
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; // DISABLE or ENABLE
	CAN_FilterInitStructure.CAN_FilterNumber = 2; // 0-27 CAN1 = 2, CAN2 = 17
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit; // 16/32
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; // IdMask/IdList
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO1; // CAN_Filter_FIFO0 or CAN_Filter_FIFO1

	// IDE = CAN_Id_Standard or CAN_Id_Extended, RTR = CAN_RTR_Data or CAN_RTR_Remote
	// for id - 0 or 1 as expected
	ul.us[1] = (0x07f7 << 5) | (CAN_Id_Standard << 1) | (CAN_RTR_Data << 3); // low module address bit set low
	CAN_FilterInitStructure.CAN_FilterIdHigh = ul.us[1];
	// for mask - 0 = don't care/ 1 = must match
	ul.us[0] = (0x0000 << 5) | (CAN_Id_Extended << 1) | (CAN_RTR_Remote << 3); // id 0
	CAN_FilterInitStructure.CAN_FilterIdLow = ul.us[0];

	// IDE = CAN_Id_Standard or CAN_Id_Extended, RTR = CAN_RTR_Data or CAN_RTR_Remote
	// for id - 0 or 1 as expected
	ul.us[1] = (0x0008 << 5) | (CAN_Id_Standard << 1) | (CAN_RTR_Data << 3); // low module address bit matters
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = ul.us[1];
	// for mask - 0 = don't care/ 1 = must match
	ul.us[0] = (0x07ff << 5) | (CAN_Id_Extended << 1) | (CAN_RTR_Remote << 3); // all bits matter
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = ul.us[0];

	CAN_FilterInit( &CAN_FilterInitStructure );

	// 16 bit id filters are set in the low/high parts of an extended id filter
	// either standard id 11 bits ST10,ST09,ST08,ST07,SD06,SD05,SD04,SD03,SD02,SD01,SD00,RTR,IDE,EX17,EX16,EX15
	// or extended id top 14 bits EX28,EX27,EX26,EX25,EX24,EX23,EX22,EX21,EX20,EX19,EX18,RTR,IDE,EX17,EX16,EX15
	// using low 16 bits here because we can
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; // DISABLE or ENABLE
	CAN_FilterInitStructure.CAN_FilterNumber = 3; // 0-27 CAN1 = 3, CAN2 = 18
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit; // 16/32
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; // IdMask/IdList
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0; // CAN_Filter_FIFO0 or CAN_Filter_FIFO1

	// IDE = CAN_Id_Standard or CAN_Id_Extended, RTR = CAN_RTR_Data or CAN_RTR_Remote
	// for id - 0 or 1 as expected
	ul.us[1] = (0x07ff << 5) | (CAN_Id_Standard << 1) | (CAN_RTR_Data << 3); // low module address bit set high
	CAN_FilterInitStructure.CAN_FilterIdHigh = ul.us[1];
	// for mask - 0 = don't care/ 1 = must match
	ul.us[0] = (0x0000 << 5) | (CAN_Id_Extended << 1) | (CAN_RTR_Remote << 3);
	CAN_FilterInitStructure.CAN_FilterIdLow = ul.us[0];

	// IDE = CAN_Id_Standard or CAN_Id_Extended, RTR = CAN_RTR_Data or CAN_RTR_Remote
	// for id - 0 or 1 as expected
	ul.us[1] = (0x0008 << 5) | (CAN_Id_Standard << 1) | (CAN_RTR_Data << 3); // low module address bit matters
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = ul.us[1];
	// for mask - 0 = don't care/ 1 = must match
	ul.us[0] = (0x07ff << 5) | (CAN_Id_Extended << 1) | (CAN_RTR_Remote << 3); // all bits matter
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = ul.us[0];

	CAN_FilterInit( &CAN_FilterInitStructure );

	NVIC_InitStructure.NVIC_IRQChannel = CAN1_TX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_KERNEL_INTERRUPT_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_Init( &NVIC_InitStructure );

	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
	NVIC_Init( &NVIC_InitStructure );

	// enable receive interrupts, transmit enabled when message is queued
	CAN1->IER |= (CAN_IT_FMP0 | CAN_IT_FMP1);
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void InitialiseCAN1( void )
{
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	union LongToChar ul;

	RxCAN1 = xQueueCreate( CAN_MAXRX, (unsigned portBASE_TYPE)sizeof( struct CanMessage ) );
	TxCAN1 = xQueueCreate( CAN_MAXTX, (unsigned portBASE_TYPE)sizeof( struct CanMessage ) );

	// clocks & pins setup in SystemInit_GPIO()

	// reset registers
	CAN_DeInit( CAN2 );

	CAN_Init( CAN2, (CAN_InitTypeDef *)&CAN_Init_settings[CANBAUD_RDS] );

	// split filters - start of filter bank for CAN2
	// 0 - 14 for CAN1, 15-27 for CAN2
	CAN_SlaveStartBank( 15 ); // Both CAN Tasks Set the same filter split point - we only need to do it in one

	// 16 bit filters
	// could combine the following two filters if a single FIFO was the destination

	// 16 bit id filters are set in the low/high parts of an extended id filter
	// either standard id 11 bits ST10,ST09,ST08,ST07,SD06,SD05,SD04,SD03,SD02,SD01,SD00, RTR, IDE,EX17,EX16,EX15
	// or extended id top 14 bits EX28,EX27,EX26,EX25,EX24,EX23,EX22,EX21,EX20,EX19,EX18, RTR, IDE,EX17,EX16,EX15
	// defined bits CAN_Id_Extended (0x04) & CAN_RTR_Remote (0x02) need shifting before using in ID mask
	// defined bits CAN_Id_Extended (0x04) & CAN_RTR_Remote (0x02) need shifting before using in ID mask
	// (CAN_Id_Extended << 1), (CAN_RTR_Remote << 3)
	// defined bits CAN_Id_Standard (0x00) & CAN_RTR_Data (0x00) don't really need shifting
	// using high 16 bits here because we can
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; // DISABLE or ENABLE
	CAN_FilterInitStructure.CAN_FilterNumber = 17; // 0-27 CAN1 = 2, CAN2 = 17
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit; // 16/32
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; // IdMask/IdList
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO1; // CAN_Filter_FIFO0 or CAN_Filter_FIFO1

	// IDE = CAN_Id_Standard or CAN_Id_Extended, RTR = CAN_RTR_Data or CAN_RTR_Remote
	// for id - 0 or 1 as expected
	ul.us[1] = (0x07f7 << 5) | (CAN_Id_Standard << 1) | (CAN_RTR_Data << 3); // low module address bit set low
	CAN_FilterInitStructure.CAN_FilterIdHigh = ul.us[1];
	// for mask - 0 = don't care/ 1 = must match
	ul.us[0] = (0x0000 << 5) | (CAN_Id_Extended << 1) | (CAN_RTR_Remote << 3); // id 0
	CAN_FilterInitStructure.CAN_FilterIdLow = ul.us[0];

	// IDE = CAN_Id_Standard or CAN_Id_Extended, RTR = CAN_RTR_Data or CAN_RTR_Remote
	// for id - 0 or 1 as expected
	ul.us[1] = (0x0008 << 5) | (CAN_Id_Standard << 1) | (CAN_RTR_Data << 3); // low module address bit matters
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = ul.us[1];
	// for mask - 0 = don't care/ 1 = must match
	ul.us[0] = (0x07ff << 5) | (CAN_Id_Extended << 1) | (CAN_RTR_Remote << 3); // all bits matter
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = ul.us[0];

	CAN_FilterInit( &CAN_FilterInitStructure );

	// 16 bit id filters are set in the low/high parts of an extended id filter
	// either standard id 11 bits ST10,ST09,ST08,ST07,SD06,SD05,SD04,SD03,SD02,SD01,SD00,RTR,IDE,EX17,EX16,EX15
	// or extended id top 14 bits EX28,EX27,EX26,EX25,EX24,EX23,EX22,EX21,EX20,EX19,EX18,RTR,IDE,EX17,EX16,EX15
	// using low 16 bits here because we can
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; // DISABLE or ENABLE
	CAN_FilterInitStructure.CAN_FilterNumber = 18; // 0-27 CAN1 = 3, CAN2 = 18
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit; // 16/32
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; // IdMask/IdList
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0; // CAN_Filter_FIFO0 or CAN_Filter_FIFO1

	// IDE = CAN_Id_Standard or CAN_Id_Extended, RTR = CAN_RTR_Data or CAN_RTR_Remote
	// for id - 0 or 1 as expected
	ul.us[1] = (0x07ff << 5) | (CAN_Id_Standard << 1) | (CAN_RTR_Data << 3); // low module address bit set high
	CAN_FilterInitStructure.CAN_FilterIdHigh = ul.us[1];
	// for mask - 0 = don't care/ 1 = must match
	ul.us[0] = (0x0000 << 5) | (CAN_Id_Extended << 1) | (CAN_RTR_Remote << 3);
	CAN_FilterInitStructure.CAN_FilterIdLow = ul.us[0];

	// IDE = CAN_Id_Standard or CAN_Id_Extended, RTR = CAN_RTR_Data or CAN_RTR_Remote
	// for id - 0 or 1 as expected
	ul.us[1] = (0x0008 << 5) | (CAN_Id_Standard << 1) | (CAN_RTR_Data << 3); // low module address bit matters
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = ul.us[1];
	// for mask - 0 = don't care/ 1 = must match
	ul.us[0] = (0x07ff << 5) | (CAN_Id_Extended << 1) | (CAN_RTR_Remote << 3); // all bits matter
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = ul.us[0];

	CAN_FilterInit( &CAN_FilterInitStructure );

	NVIC_InitStructure.NVIC_IRQChannel = CAN2_TX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_KERNEL_INTERRUPT_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_Init( &NVIC_InitStructure );

	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
	NVIC_Init( &NVIC_InitStructure );

	// enable receive interrupts, transmit enabled when message is queued
	CAN2->IER |= (CAN_IT_FMP0 | CAN_IT_FMP1);
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void SendTestMessage( unsigned int ModuleType, unsigned int ModuleNumber )
{
	struct CanMessage CanM_Tx;

	// return module info
	CanM_Tx.Id.ul = ModuleType | (ModuleNumber % 4);
	CanM_Tx.Data.uc[0] = CAN_TEST;
	CanM_Tx.Data.uc[1] = pFac.pf.software_type / 256;
	CanM_Tx.Data.uc[2] = pFac.pf.software_type % 256;
	CanM_Tx.Data.uc[3] = pFac.pf.software_issue / 256;
	CanM_Tx.Data.uc[4] = pFac.pf.software_issue % 256;
	CanM_Tx.Data.uc[5] = pFac.pf.software_revision / 256;
	CanM_Tx.Data.uc[6] = pFac.pf.software_revision % 256;
	CanM_Tx.Data.uc[7] = 0xa5;
	xQueueSend( (ModuleNumber < 4) ? TxCAN0 : TxCAN1, (void *)&CanM_Tx, CANTX_BLOCK_TIME );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void Boot_SetRestartStatus( void )
{
	struct structBackUpSRAM *ptSRAM;

	// enable access
	PWR_BackupAccessCmd( ENABLE );

	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_BKPSRAM, ENABLE );

	// point to backup ram structure
	ptSRAM = (struct structBackUpSRAM *)BKPSRAM_BASE;

	// copy boot mode enable string
	strcpy( ptSRAM->BootStatus, BOOTMODE_SET );

	// save current baud rate
	ptSRAM->CanBaud = CANBAUD_RDS; // CANBaud

#ifdef DEBUG
	SendString( TxCOM0, ptSRAM->BootStatus );
	SendString( TxCOM0, "\r\n" "Main - Waiting for Watchdog Reset\r\n" );
#endif

	vTaskDelay( (portTickType)(100 / portTICK_RATE_MS) ); // wait 100ms

	// turn access off
	PWR_BackupAccessCmd( DISABLE );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void ProcessGWSmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx )
{
	if( ModuleNumber == 0 )
	{
		// RDS modules have message type in byte 0
		switch( ptCanM_Rx->Data.uc[0] )
		{
		case 0xf7: // set for software update
			if( ptCanM_Rx->Data.uc[1] == 0x60 )
			{
				// check data for key value
				if( memcmp( &ptCanM_Rx->Data.uc[2], BOOTMODE_SET, 6 ) == 0 )
				{
					// set boot mode enable string for boot mode after restart
					Boot_SetRestartStatus();

					// watch dog is reset in operate task
					vTaskSuspendAll();

					// stay here until watch dog reset, which is about 1 seconds
					while( 1 )
					{
					}
					// NVIC_SystemReset(); Privileged only
				}
			}
			break;
		}
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void ProcessMCMmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx )
{
	struct CAN_MCM *ptMod;
	
	unsigned short int temp_us;

	ptMod = &MCM[ModuleNumber];
		
	// RDS modules have message type in byte 0
	switch( ptCanM_Rx->Data.uc[0] )
	{
	case MCM_RESET:
		// Reset_Variables();

		// transmitted to module - reset module
		// CanM_Tx.Data.uc[0] = 0x01;
		// CanM_Tx.Data.uc[1] = 'R';
		// CanM_Tx.Data.uc[2] = 'E';
		// CanM_Tx.Data.uc[3] = 'S';
		// CanM_Tx.Data.uc[4] = 'E';
		// CanM_Tx.Data.uc[5] = 'T';
		// CanM_Tx.Data.uc[6] = 1;
		// CanM_Tx.Data.uc[7] = 1;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );

		// received from module
		// CanM_Rx.Data.uc[0] = 0x01;
		// tstring[0] = CanM_Rx.Data.uc[1]; // 'R'
		// tstring[1] = CanM_Rx.Data.uc[2]; // 'E'
		// tstring[2] = CanM_Rx.Data.uc[3]; // 'S'
		// tstring[3] = CanM_Rx.Data.uc[4]; // 'E'
		// tstring[4] = CanM_Rx.Data.uc[5]; // 'T'
		// tstring[5] = CanM_Rx.Data.uc[6]; // '1'
		// tstring[6] = CanM_Rx.Data.uc[7]; // '1'
		// tstring[7] = 0x00;
		break;

	case MCM_SET_MOTOR_SPEED:
		// transmitted to module - set motor speed and mode
		// CanM_Tx.Data.uc[0] = 0x02;
		// CanM_Tx.Data.uc[1] = Ratio_D;
		// CanM_Tx.Data.uc[2] = Ratio_R / 256;
		// CanM_Tx.Data.uc[3] = Ratio_R % 256;
		// CanM_Tx.Data.uc[4] = Motor_Mode;
		// CanM_Tx.Data.uc[5] = 0;
		// CanM_Tx.Data.uc[6] = 0;
		// CanM_Tx.Data.uc[7] = 0;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );

		// received from module
		// CanM_Rx.Data.uc[0] = 0x02;
		ptMod->motor_frequency = (ptCanM_Rx->Data.uc[1] << 8) | ptCanM_Rx->Data.uc[2];
		ptMod->motor_frequency_error = (signed char)ptCanM_Rx->Data.uc[5];
		// frequency = (frequency * 200) + frequency_error;

		ptMod->controller_voltage = ptCanM_Rx->Data.uc[6];
		ptMod->controller_pwm = ptCanM_Rx->Data.uc[7];

		temp_us = (ptCanM_Rx->Data.uc[3] << 8) | ptCanM_Rx->Data.uc[4];
		ptMod->calibration_motor_pulse_accumulator += temp_us;
		if( ptMod->motor_mode != METER_CALIBRATION )
		{
			ptMod->motor_pulse_accumulator += temp_us;
		}
		break;

	case MCM_GET_FORWARD_SPEED:
		// transmitted to module - get travel information
		// CanM_Tx.Data.uc[0] = 0x03;
		// CanM_Tx.Data.uc[1] = speed_frequency  / 256;
		// CanM_Tx.Data.uc[2] = speed_frequency % 256;
		// CanM_Tx.Data.uc[3] = speed_pulses / 256;
		// CanM_Tx.Data.uc[4] = speed_pulses % 256;
		// CanM_Tx.Data.uc[5] = 0;
		// CanM_Tx.Data.uc[6] = 0;
		// CanM_Tx.Data.uc[7] = 0;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );

		// received from module
		// CanM_Rx.Data.uc[0] = 0x03;
		ptMod->speed_frequency = (ptCanM_Rx->Data.uc[1] << 8) | ptCanM_Rx->Data.uc[2];
		ptMod->speed_frequency_error = (signed char)ptCanM_Rx->Data.uc[5];
		// frequency = (frequency * 200) + frequency_error;

		temp_us = (ptCanM_Rx->Data.uc[3] << 8) | ptCanM_Rx->Data.uc[4];
		ptMod->speed_pulse_accumulator += temp_us;
		break;

	case MCM_AUTOCAL_RESET:
		// transmitted to module - start auto cal
		// CanM_Tx.Data.uc[0] = 0x04;
		// CanM_Tx.Data.uc[1] = 0;
		// CanM_Tx.Data.uc[2] = 0;
		// CanM_Tx.Data.uc[3] = 0;
		// CanM_Tx.Data.uc[4] = 0;
		// CanM_Tx.Data.uc[5] = 0;
		// CanM_Tx.Data.uc[6] = 0;
		// CanM_Tx.Data.uc[7] = 0;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );

		// received from module
		// CanM_Rx.Data.uc[0] = 0x04;
		break;

	case MCM_AUTOCAL_PROGRESS:
		// transmitted to module - auto cal progress
		// CanM_Tx.Data.uc[0] = 0x05;
		// CanM_Tx.Data.uc[1] = 0;
		// CanM_Tx.Data.uc[2] = 0;
		// CanM_Tx.Data.uc[3] = 0;
		// CanM_Tx.Data.uc[4] = 0;
		// CanM_Tx.Data.uc[5] = 0;
		// CanM_Tx.Data.uc[6] = 0;
		// CanM_Tx.Data.uc[7] = 0;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );

		// received from module
		// CanM_Rx.Data.uc[0] = 0x05;
		// motor_frequency = (CanM_Rx.Data.uc[1] << 8) | CanM_Rx.Data.uc[2];
		// motor_pulses = (CanM_Rx.Data.uc[3] << 8) | CanM_Rx.Data.uc[4];
		// data_valid = CanM_Rx.Data.uc[4];
		break;

	case MCM_AUTOCAL_STOP:
		// transmitted to module - stop auto cal
		// CanM_Tx.Data.uc[0] = 0x06;
		// CanM_Tx.Data.uc[1] = 0;
		// CanM_Tx.Data.uc[2] = 0;
		// CanM_Tx.Data.uc[3] = 0;
		// CanM_Tx.Data.uc[4] = 0;
		// CanM_Tx.Data.uc[5] = 0;
		// CanM_Tx.Data.uc[6] = 0;
		// CanM_Tx.Data.uc[7] = 0;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );

		// received from module
		// CanM_Rx.Data.uc[0] = 0x06;
		// motor_frequency = (CanM_Rx.Data.uc[1] << 8) | CanM_Rx.Data.uc[2];
		// motor_pulses =(CanM_Rx.Data.uc[3] << 8) | CanM_Rx.Data.uc[4];
		// data_valid = CanM_Rx.Data.uc[4];
		break;

	case MCM_GET_AUX_INPUT:
		// transmitted to module - get auxillary inputs
		// CanM_Tx.Data.uc[0] = 0x07;
		// CanM_Tx.Data.uc[1] = 0;
		// CanM_Tx.Data.uc[2] = 0;
		// CanM_Tx.Data.uc[3] = 0;
		// CanM_Tx.Data.uc[4] = 0;
		// CanM_Tx.Data.uc[5] = 0;
		// CanM_Tx.Data.uc[6] = 0;
		// CanM_Tx.Data.uc[7] = 0;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );

		// received from module
		// CanM_Rx.Data.uc[0] = 0x07;
		// motor_temp = CanM_Rx.Data.uc[1]; - not used sends input6 status 0x00 or 0xff
		// motor_current = CanM_Rx.Data.uc[2];
		// drive_temp = CanM_Rx.Data.uc[3];
		// input_status = CanM_Rx.Data.uc[4];

		ptMod->motor_current = (unsigned char)((float)ptCanM_Rx->Data.uc[2] / 2.55);
		ptMod->controller_temperature = (unsigned char)((unsigned short int)ptCanM_Rx->Data.uc[3] * 150 / 255);
		ptMod->dip_status = ptCanM_Rx->Data.uc[4];
		break;

	case MCM_GET_AUX_TIME:
		// transmitted to module - get auxillary inputs time
		// CanM_Tx.Data.uc[0] = 0x08;
		// CanM_Tx.Data.uc[1] = 0;
		// CanM_Tx.Data.uc[2] = 0;
		// CanM_Tx.Data.uc[3] = 0;
		// CanM_Tx.Data.uc[4] = 0;
		// CanM_Tx.Data.uc[5] = 0;
		// CanM_Tx.Data.uc[6] = 0;
		// CanM_Tx.Data.uc[7] = 0;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );

		// received from module
		// CanM_Rx.Data.uc[0] = 0x08;
		ptMod->dip_status = ptCanM_Rx->Data.uc[1];
		ptMod->dip_time_since_last_pulse[0] = ptCanM_Rx->Data.uc[2];
		ptMod->dip_time_since_last_pulse[1] = ptCanM_Rx->Data.uc[3];
		ptMod->dip_time_since_last_pulse[2] = ptCanM_Rx->Data.uc[4];
		ptMod->dip_time_since_last_pulse[3] = ptCanM_Rx->Data.uc[5];
		ptMod->dip_time_since_last_pulse[4] = ptCanM_Rx->Data.uc[6];
		ptMod->dip_time_since_last_pulse[5] = ptCanM_Rx->Data.uc[7];
		break;

	case MCM_SET_PID_DATA:
		// transmitted to module - send PID configuration
		// CanM_Tx.Data.uc[0] = 0x09;
		// CanM_Tx.Data.uc[1] = con_PI / 256;
		// CanM_Tx.Data.uc[2] = con_PI % 256;
		// CanM_Tx.Data.uc[3] = con_D / 256;
		// CanM_Tx.Data.uc[4] = con_D % 256;
		// CanM_Tx.Data.uc[5] = con_Frequency / 256;
		// CanM_Tx.Data.uc[6] = con_Frequency % 256;
		// CanM_Tx.Data.uc[7] = 0;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );

		// received from module
		// CanM_Rx.Data.uc[0] = 0x09;
		break;

	case MCM_GET_AUX_FREQ:
		// transmitted to module - send aux frequency
		// CanM_Tx.Data.uc[0] = 0x0a;
		// CanM_Tx.Data.uc[1] = aux_frequency / 256;
		// CanM_Tx.Data.uc[2] = aux_frequency % 256;
		// CanM_Tx.Data.uc[3] = aux_pulses / 256;
		// CanM_Tx.Data.uc[4] = aux_pulses % 256;
		// CanM_Tx.Data.uc[5] = 0;
		// CanM_Tx.Data.uc[6] = 0;
		// CanM_Tx.Data.uc[7] = 0;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );

		// received from module
		// CanM_Rx.Data.uc[0] = 0x0a;
		break;

	case MCM_GET_SOFTWARE: // software info
		// transmitted to module - get module info
		// CanM_Tx.Data.uc[0] = 0xff;
		// CanM_Tx.Data.uc[1] = software_type / 256;
		// CanM_Tx.Data.uc[2] = software_type % 256;
		// CanM_Tx.Data.uc[3] = software_issue / 256;
		// CanM_Tx.Data.uc[4] = software_issue % 256;
		// CanM_Tx.Data.uc[5] = inputs9to12_status;
		// CanM_Tx.Data.uc[6] = reset_fault;
		// CanM_Tx.Data.uc[7] = 0;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );

		// received from module
		// CanM_Rx.Data.uc[0] = 0xff;
		ptMod->software_type = (ptCanM_Rx->Data.uc[1] << 8) | ptCanM_Rx->Data.uc[2];
		ptMod->software_issue = (ptCanM_Rx->Data.uc[3] << 8) | ptCanM_Rx->Data.uc[4];
		ptMod->dil_setting = ptCanM_Rx->Data.uc[5];
		ptMod->reset_fault = ptCanM_Rx->Data.uc[6];
		break;

	case CAN_TIME: // set time message - no return message
		SetCanTime( ptCanM_Rx );
		break;

	case CAN_TEST: // software info
		SendTestMessage( CAN_RXID_MCM, ModuleNumber );
		break;
	}

	CanStat[MCM_ID][ModuleNumber].rx_status = CS_RECEIVED;

}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void ProcessACMmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx )
{
	struct CAN_ACM *ptMod;

	ptMod = &ACM[ModuleNumber];
		
	// RDS modules have message type in byte 0
	switch( ptCanM_Rx->Data.uc[0] )
	{
	case ACM_RESET:
		// transmitted to module - reset module
		// CanM_Tx.Data.uc[0] = ACM_RESET;
		// CanM_Tx.Data.uc[1] = 'R';
		// CanM_Tx.Data.uc[2] = 'E';
		// CanM_Tx.Data.uc[3] = 'S';
		// CanM_Tx.Data.uc[4] = 'E';
		// CanM_Tx.Data.uc[5] = 'T';
		// CanM_Tx.Data.uc[6] = 1;
		// CanM_Tx.Data.uc[7] = 1;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );
		break;

	case CAN_TIME: // set time message - no return message
		SetCanTime( ptCanM_Rx );
		break;

	case CAN_TEST: // software info
		SendTestMessage( CAN_RXID_ACM, ModuleNumber );
		break;
		
	case 0xff: // software/issue
		ptMod->software_type = (ptCanM_Rx->Data.uc[1] << 8) | ptCanM_Rx->Data.uc[2];
		ptMod->software_issue = (ptCanM_Rx->Data.uc[3] << 8) | ptCanM_Rx->Data.uc[4];
		break;
	}

	CanStat[ACM_ID][ModuleNumber].rx_status = CS_RECEIVED;
}
/*-----------------------------------------------------------*/



/*-----------------------------------------------------------*/
void ProcessHBMmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx )
{
	struct CAN_HBM *ptMod;
	struct HBM_Input_struct *ptInput;

	static unsigned long int lastPulseAccu;

	ptMod = &HBM[ModuleNumber];

	// RDS modules have message type in byte 0
	switch( ptCanM_Rx->Data.uc[0] )
	{
	case HBM_RESET:
		// transmitted to module - reset module
		// CanM_Tx.Data.uc[0] = HBM_RESET;
		// CanM_Tx.Data.uc[1] = 'R';
		// CanM_Tx.Data.uc[2] = 'E';
		// CanM_Tx.Data.uc[3] = 'S';
		// CanM_Tx.Data.uc[4] = 'E';
		// CanM_Tx.Data.uc[5] = 'T';
		// CanM_Tx.Data.uc[6] = 1;
		// CanM_Tx.Data.uc[7] = 1;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );
		break;

	case HBM_GETINPUTS:
		if( ptCanM_Rx->Data.uc[1] < 4 )
		{
			ptInput = &(ptMod->HBM_Input[ptCanM_Rx->Data.uc[1]]);
			ptInput->Level_Now = ptCanM_Rx->Data.uc[2];
			ptInput->Frequency = (ptCanM_Rx->Data.uc[3] << 8) | ptCanM_Rx->Data.uc[4];


			ptInput->Pulse_Accumulator += ((ptCanM_Rx->Data.uc[5] << 8) | ptCanM_Rx->Data.uc[6]);


		}
		break;

	case HBM_GETANALOG:
		ptMod->UnscaledVoltage[0] = ptCanM_Rx->Data.uc[1];
		ptMod->UnscaledVoltage[1] = ptCanM_Rx->Data.uc[2];
		break;

	case HBM_GET_SOFTWARE: // software/issue
		ptMod->software_type = (ptCanM_Rx->Data.uc[1] << 8) | ptCanM_Rx->Data.uc[2];
		ptMod->software_issue = (ptCanM_Rx->Data.uc[3] << 8) | ptCanM_Rx->Data.uc[4];
		break;
	}
	ptMod->status |= CAN_MODULE_MESSAGE_RECEIVED;
	ptMod->status |= CAN_MODULE_PRESENT;

	CanStat[HBM_ID][ModuleNumber].rx_status = CS_RECEIVED;
}  				
/*-----------------------------------------------------------*/

	
/*-----------------------------------------------------------*/
void ProcessWCMmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx )
{
	struct CAN_WCM *ptMod;

	ptMod = &WCM[ModuleNumber];
		
	// RDS modules have message type in byte 0
	switch( ptCanM_Rx->Data.uc[0] )
	{
	case 0x81: // All Voltages
		ptMod->WeightVoltage = (ptCanM_Rx->Data.uc[1] << 24) | (ptCanM_Rx->Data.uc[2] << 16) | (ptCanM_Rx->Data.uc[3] << 8) | ptCanM_Rx->Data.uc[4];
		//ptMod->WeighFilter = ptCanM_Rx->Data.uc[1]; 0x55 on/
		ptMod->RollVoltage = ptCanM_Rx->Data.uc[5];
		ptMod->PitchVoltage = ptCanM_Rx->Data.uc[7];
		break;
			
	case 0x82: // software/issue
		ptMod->software_type = (ptCanM_Rx->Data.uc[1] << 8) | ptCanM_Rx->Data.uc[2];
		ptMod->software_issue = (ptCanM_Rx->Data.uc[3] << 8) | ptCanM_Rx->Data.uc[4];
		break;
			
	case 0x83: // Angle Voltages
		ptMod->RollVoltage = ptCanM_Rx->Data.uc[1];
		ptMod->PitchVoltage = ptCanM_Rx->Data.uc[2];
		break;
	}

	CanStat[WCM_ID][ModuleNumber].rx_status = CS_RECEIVED;
}
/*-----------------------------------------------------------*/

	
/*-----------------------------------------------------------*/
void ProcessDOPmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx )
{
	struct CAN_DOP *ptMod;

	ptMod = &DOP[ModuleNumber];

	// RDS modules have message type in byte 0
	switch( ptCanM_Rx->Data.uc[0] )
	{
	case DOP_RESET:
		// transmitted to module - reset module
		// CanM_Tx.Data.uc[0] = DOP_RESET;
		// CanM_Tx.Data.uc[1] = 'R';
		// CanM_Tx.Data.uc[2] = 'E';
		// CanM_Tx.Data.uc[3] = 'S';
		// CanM_Tx.Data.uc[4] = 'E';
		// CanM_Tx.Data.uc[5] = 'T';
		// CanM_Tx.Data.uc[6] = 1;
		// CanM_Tx.Data.uc[7] = 1;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );
		break;
		
	case DOP_SET_OUTPUT:
		break;
		
	case DOP_GET_SOFTWARE: // software/issue
		ptMod->software_type = (ptCanM_Rx->Data.uc[1] << 8) | ptCanM_Rx->Data.uc[2];
		ptMod->software_issue = (ptCanM_Rx->Data.uc[3] << 8) | ptCanM_Rx->Data.uc[4];
		break;
	}

	CanStat[DOP_ID][ModuleNumber].rx_status = CS_RECEIVED;
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void ProcessDIPmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx )
{
	struct CAN_DIP *ptMod;

	unsigned int ui;
	
	ptMod = &DIP[ModuleNumber];

	// RDS modules have message type in byte 0
	switch( ptCanM_Rx->Data.uc[0] )
	{
	case DIP_RESET:
		// transmitted to module - reset module
		// CanM_Tx.Data.uc[0] = DIP_RESET;
		// CanM_Tx.Data.uc[1] = 'R';
		// CanM_Tx.Data.uc[2] = 'E';
		// CanM_Tx.Data.uc[3] = 'S';
		// CanM_Tx.Data.uc[4] = 'E';
		// CanM_Tx.Data.uc[5] = 'T';
		// CanM_Tx.Data.uc[6] = 1;
		// CanM_Tx.Data.uc[7] = 1;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );
		break;

	case DIP_SETPULLUP:
		// transmitted to module
		// CanM_Tx.Data.uc[0] = DIP_SETPULLUP;
		// CanM_Tx.Data.uc[1] = ptMod->pullup;
		// CanM_Tx.Data.uc[2] = 0;
		// CanM_Tx.Data.uc[3] = 0;
		// CanM_Tx.Data.uc[4] = 0;
		// CanM_Tx.Data.uc[5] = 0;
		// CanM_Tx.Data.uc[6] = 0;
		// CanM_Tx.Data.uc[7] = 0;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );
		break;

	case DIP_GETFREQ:
		// transmitted to module
		// CanM_Tx.Data.uc[0] = DIP_SETPULLUP;
		// CanM_Tx.Data.uc[1] = ip;
		// CanM_Tx.Data.uc[2] = freq[ip+0] / 256;
		// CanM_Tx.Data.uc[3] = freq[ip+0] % 256;
		// CanM_Tx.Data.uc[4] = freq[ip+1] / 256;
		// CanM_Tx.Data.uc[5] = freq[ip+1] % 256;
		// CanM_Tx.Data.uc[6] = freq[ip+2] / 256;
		// CanM_Tx.Data.uc[7] = freq[ip+2] % 256;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );
		ui = ptCanM_Rx->Data.uc[1];
		ptMod->frequency[ui] = (ptCanM_Rx->Data.uc[2] << 8) | ptCanM_Rx->Data.uc[3];
		ui++;
		ptMod->frequency[ui] = (ptCanM_Rx->Data.uc[4] << 8) | ptCanM_Rx->Data.uc[5];
		ui++;
		ptMod->frequency[ui] = (ptCanM_Rx->Data.uc[6] << 8) | ptCanM_Rx->Data.uc[7];
		break;

	case DIP_GETSTATUS: // ip status
		// transmitted to module
		//CanM_Tx.Data.uc[0] = DIP_GETSTATUS;
		//CanM_Tx.Data.uc[1] = ptMod->ipstatus;
		//CanM_Tx.Data.uc[2] = 0;
		//CanM_Tx.Data.uc[3] = 0;
		//CanM_Tx.Data.uc[4] = 0;
		//CanM_Tx.Data.uc[5] = 0;
		//CanM_Tx.Data.uc[6] = 0;
		//CanM_Tx.Data.uc[7] = 0;
		//xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );
		ptMod->ipstatus = ptCanM_Rx->Data.uc[1];
		break;

	case DIP_GET_SOFTWARE: // software/issue
		ptMod->software_type = (ptCanM_Rx->Data.uc[1] << 8) | ptCanM_Rx->Data.uc[2];
		ptMod->software_issue = (ptCanM_Rx->Data.uc[3] << 8) | ptCanM_Rx->Data.uc[4];
		break;
	}
	CanStat[DIP_ID][ModuleNumber].rx_status = CS_RECEIVED;
}  				
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void ProcessLMMmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx )
{
	struct CAN_LMM *ptMod;

	ptMod = &LMM[ModuleNumber];

	// RDS modules have message type in byte 0
	switch( ptCanM_Rx->Data.uc[0] )
	{
	case LMM_RESET:
		// transmitted to module - reset module
		// CanM_Tx.Data.uc[0] = DIP_RESET;
		// CanM_Tx.Data.uc[1] = 'R';
		// CanM_Tx.Data.uc[2] = 'E';
		// CanM_Tx.Data.uc[3] = 'S';
		// CanM_Tx.Data.uc[4] = 'E';
		// CanM_Tx.Data.uc[5] = 'T';
		// CanM_Tx.Data.uc[6] = 1;
		// CanM_Tx.Data.uc[7] = 1;
		// xQueueSend( TxCAN, (void *)&CanM_Tx, CANTX_BLOCK_TIME );
		break;

	case LMM_GET_SOFTWARE: // software/issue
		ptMod->software_type = (ptCanM_Rx->Data.uc[1] << 8) | ptCanM_Rx->Data.uc[2];
		ptMod->software_issue = (ptCanM_Rx->Data.uc[3] << 8) | ptCanM_Rx->Data.uc[4];
		break;
	}
	CanStat[LMM_ID][ModuleNumber].rx_status = CS_RECEIVED;
}  				
/*-----------------------------------------------------------*/

	
/*-----------------------------------------------------------*/
void ProcessIOMmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx )
{
	struct CanMessage CanM_Tx;
	
	struct CAN_IOM *ptMod;

	unsigned int ui;

	ptMod = &IOM[ModuleNumber];

	CanM_Tx.Id.ul = CAN_RXID_IOM | (ModuleNumber % 4);

	// RDS modules have message type in byte 0
	switch( ptCanM_Rx->Data.uc[0] )
	{
	case 0x01: // pulse count reached zero
		ui = ptCanM_Rx->Data.uc[1]; // get requested input
        if( ui < 8 )
        {
        	ptMod->PulseCountZero |= (1 << ui); // set bit corresponding to input 
        }
		break;
		
	case 0x02: // input status
		ptMod->Inputs =
				((unsigned long int)ptCanM_Rx->Data.uc[1] <<  0) |
				((unsigned long int)ptCanM_Rx->Data.uc[2] <<  8);

		// input 1 change
		if( ptMod->Inputs & IOM_IP01 )
		{
			if( !(ptMod->IPstatus & IOM_IP01) )
			{
				ptMod->IPstatus |= IOM_IP01;
				
				ptMod->OutputCount++;
				if( ptMod->OutputCount > 10 )
				{
					ptMod->OutputCount = 0;
				}
				
			  	// immediately set outputs - just for test software
				CanM_Tx.Data.uc[0] = 0x03;
				ui = OpSetIOM[ptMod->OutputCount]; // outputs to turn on
 				CanM_Tx.Data.uc[1] = (unsigned char)((ui >>  0) & 0xff);
   				CanM_Tx.Data.uc[2] = (unsigned char)((ui >>  8) & 0x03);
				ui = ~OpSetIOM[ptMod->OutputCount]; // outputs to turn off
   				CanM_Tx.Data.uc[3] = (unsigned char)((ui >> 0) & 0xff);
   				CanM_Tx.Data.uc[4] = (unsigned char)((ui >> 8) & 0x03);
   				CanM_Tx.Data.uc[5] = 0;
   				CanM_Tx.Data.uc[6] = 0;
    			CanM_Tx.Data.uc[7] = 0;
				xQueueSend( TxCAN1, (void *)&CanM_Tx, CANTX_BLOCK_TIME );
			}
		}
		else
		{
			ptMod->IPstatus &= ~IOM_IP01;
		}
		
		// input 2 change
		if( ptMod->Inputs & IOM_IP02 )
		{
			if( !(ptMod->IPstatus & IOM_IP02) )
			{
				ptMod->IPstatus |= IOM_IP02;
				
				ptMod->OutputCount++;
				if( ptMod->OutputCount > 10 )
				{
					ptMod->OutputCount = 0;
				}
				
			  	// immediately set outputs - just for test software
					CanM_Tx.Data.uc[0] = 0x03;
				ui = OpSetIOM[ptMod->OutputCount]; // outputs to turn on
 				CanM_Tx.Data.uc[1] = (unsigned char)((ui >>  0) & 0xff);
   				CanM_Tx.Data.uc[2] = (unsigned char)((ui >>  8) & 0x03);
   				// only turn off outputs if all off
   				if( ui == 0 )
   				{
   					ui = ~OpSetIOM[ptMod->OutputCount]; // outputs to turn off
   				}
   				else
   				{
   					ui = 0;
   				}
   				CanM_Tx.Data.uc[3] = (unsigned char)((ui >> 0) & 0xff);
   				CanM_Tx.Data.uc[4] = (unsigned char)((ui >> 8) & 0x03);
   				CanM_Tx.Data.uc[5] = 0;
   				CanM_Tx.Data.uc[6] = 0;
    			CanM_Tx.Data.uc[7] = 0;
				xQueueSend( TxCAN1, (void *)&CanM_Tx, CANTX_BLOCK_TIME );
			}
		}
		else
		{
			ptMod->IPstatus &= ~IOM_IP02;
		}
		break;

	case 0x03: // output status
		ptMod->Outputs =
				((unsigned long int)ptCanM_Rx->Data.uc[1] <<  0) |
				((unsigned long int)ptCanM_Rx->Data.uc[2] <<  8);
		break;
		
	case 0x04: // voltage
		ptMod->Voltage[0] = ptCanM_Rx->Data.uc[1] | (((unsigned int)ptCanM_Rx->Data.uc[2] << 8) & 0x0f00);
		ptMod->Voltage[1] = ptCanM_Rx->Data.uc[3] | (((unsigned int)ptCanM_Rx->Data.uc[2] << 4) & 0x0f00);
		ptMod->Voltage[2] = ptCanM_Rx->Data.uc[4] | (((unsigned int)ptCanM_Rx->Data.uc[5] << 8) & 0x0f00);
		ptMod->Voltage[3] = ptCanM_Rx->Data.uc[6] | (((unsigned int)ptCanM_Rx->Data.uc[5] << 4) & 0x0f00);
		ptMod->Supply = (float)ptCanM_Rx->Data.uc[7];
		break;
		
	case 0x05: // current
		ptMod->Current[0] = ptCanM_Rx->Data.uc[1] | (((unsigned int)ptCanM_Rx->Data.uc[2] << 8) & 0x0f00);
		ptMod->Current[1] = ptCanM_Rx->Data.uc[3] | (((unsigned int)ptCanM_Rx->Data.uc[2] << 4) & 0x0f00);
		ptMod->Current[2] = ptCanM_Rx->Data.uc[4] | (((unsigned int)ptCanM_Rx->Data.uc[5] << 8) & 0x0f00);
		ptMod->Current[3] = ptCanM_Rx->Data.uc[6] | (((unsigned int)ptCanM_Rx->Data.uc[5] << 4) & 0x0f00);
		break;
		
	case 0x06: // frequency
		ui = ptCanM_Rx->Data.uc[1]; // get requested input
        if( ui < 8 )
        {
			ptMod->frequency[ui] = (ptCanM_Rx->Data.uc[3] << 8) | ptCanM_Rx->Data.uc[2];
			ptMod->pulsecount[ui] =
				(unsigned int)(ptCanM_Rx->Data.uc[4] <<  0) |
				(unsigned int)(ptCanM_Rx->Data.uc[5] <<  8) |
				(unsigned int)(ptCanM_Rx->Data.uc[6] << 16) |
				(unsigned int)(ptCanM_Rx->Data.uc[7] << 24);
        }
		break;
		
	case 0x07: // voltage threshold reached
		ui = ptCanM_Rx->Data.uc[1]; // get requested input
        if( ui < 4 )
        {
        	ptMod->VoltageThreshold |= (1 << ui); // set bit corresponding to input 
        }
		break;
		
	case IOM_GET_SOFTWARE: // software/issue
		ptMod->software_type = (ptCanM_Rx->Data.uc[2] << 8) | ptCanM_Rx->Data.uc[1];
		ptMod->software_issue = (ptCanM_Rx->Data.uc[4] << 8) | ptCanM_Rx->Data.uc[3];
		break;
	}
	CanStat[IOM_ID][ModuleNumber].rx_status = CS_RECEIVED;
}  				
/*-----------------------------------------------------------*/

	
/*-----------------------------------------------------------*/
void ProcessIOCmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx )
{
	//struct CanMessage CanM_Tx;

	struct CAN_IOC *ptMod;

	unsigned int ui;

	ptMod = &IOC[ModuleNumber];

	//CanM_Tx.Id.ul = CAN_RXID_IOC | (ModuleNumber % 4);

	// RDS modules have message type in byte 0
	switch( ptCanM_Rx->Data.uc[0] )
	{
	case 0x01: // pulse count reached zero
		ui = ptCanM_Rx->Data.uc[1]; // get requested input
        if( ui < 8 )
        {
        	ptMod->PulseCountZero |= (1 << ui); // set bit corresponding to input
        }
		break;

	case 0x02: // input status
		ptMod->Inputs =
				((unsigned long int)ptCanM_Rx->Data.uc[1] <<  0) |
				((unsigned long int)ptCanM_Rx->Data.uc[2] <<  8);

		ptMod->LastInputTime = 0;

		// input 1 change
		if( ptMod->Inputs & IOC_IP01 )
		{
			if( !(ptMod->IPstatus & IOC_IP01) )
			{
				ptMod->IPstatus |= IOC_IP01;
			}
		}
		else
		{
			ptMod->IPstatus &= ~IOC_IP01;
		}
		break;

	case 0x03: // output status
		ptMod->Outputs =
				((unsigned long int)ptCanM_Rx->Data.uc[1] <<  0) |
				((unsigned long int)ptCanM_Rx->Data.uc[2] <<  8) |
				((unsigned long int)ptCanM_Rx->Data.uc[3] << 16);
		break;

	case 0x04: // voltage
		ptMod->Voltage[0] = ptCanM_Rx->Data.uc[1] | (((unsigned int)ptCanM_Rx->Data.uc[2] << 8) & 0x0f00);
		ptMod->Voltage[1] = (((unsigned int)ptCanM_Rx->Data.uc[2] >> 4) & 0x000f) | (((unsigned int)ptCanM_Rx->Data.uc[3] << 4) & 0x0ff0);
		ptMod->Voltage[2] = ptCanM_Rx->Data.uc[4] | (((unsigned int)ptCanM_Rx->Data.uc[5] << 8) & 0x0f00);
		ptMod->Voltage[3] = (((unsigned int)ptCanM_Rx->Data.uc[5] >> 4) & 0x000f) | (((unsigned int)ptCanM_Rx->Data.uc[6] << 4) & 0x0ff0);
		ptMod->Supply = (float)ptCanM_Rx->Data.uc[7];
		break;

	case 0x06: // frequency
		ui = ptCanM_Rx->Data.uc[1]; // get requested input
        if( ui < 8 )
        {
			ptMod->frequency[ui] = (((ptCanM_Rx->Data.uc[3] << 8) | ptCanM_Rx->Data.uc[2]) + 10) / 20;
			ptMod->pulsecount[ui] =
				(unsigned int)(ptCanM_Rx->Data.uc[4] <<  0) |
				(unsigned int)(ptCanM_Rx->Data.uc[5] <<  8) |
				(unsigned int)(ptCanM_Rx->Data.uc[6] << 16) |
				(unsigned int)(ptCanM_Rx->Data.uc[7] << 24);
        }
		break;

	case 0x07: // voltage threshold reached
		ui = ptCanM_Rx->Data.uc[1]; // get requested input
        if( ui < 4 )
        {
        	ptMod->VoltageThreshold |= (1 << ui); // set bit corresponding to input
        }
		break;

	case 0x0e: // module id jumper/dip switch settings
		ptMod->DipSwitchSetting = ptCanM_Rx->Data.uc[7]; // get requested input
		break;

	case IOC_GET_SOFTWARE: // software/issue
		ptMod->software_type = (ptCanM_Rx->Data.uc[2] << 8) | ptCanM_Rx->Data.uc[1];
		ptMod->software_issue = (ptCanM_Rx->Data.uc[4] << 8) | ptCanM_Rx->Data.uc[3];
		break;
	}
	CanStat[IOC_ID][ModuleNumber].rx_status = CS_RECEIVED;
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void ProcessIOBmessages( unsigned int ModuleNumber, struct CanMessage *ptCanM_Rx )
{
	//struct CanMessage CanM_Tx;

	struct CAN_IOB *ptMod;

	unsigned int ui;

	ptMod = &IOB[ModuleNumber];

	//CanM_Tx.Id.ul = CAN_RXID_IOB | (ModuleNumber % 4);

	// RDS modules have message type in byte 0
	switch( ptCanM_Rx->Data.uc[0] )
	{
	case 0x01: // pulse count reached zero
		ui = ptCanM_Rx->Data.uc[1]; // get requested input
        if( ui < 8 )
        {
        	ptMod->PulseCountZero |= (1 << ui); // set bit corresponding to input
        }
		break;

	case 0x02: // input status
		ptMod->Inputs =
			((unsigned long int)ptCanM_Rx->Data.uc[1] <<  0) |
			((unsigned long int)ptCanM_Rx->Data.uc[2] <<  8);

		ptMod->LastInputTime = 0;

		// input 1 change
		if( ptMod->Inputs & IOB_IP01 )
		{
			if( !(ptMod->IPstatus & IOB_IP01) )
			{
				ptMod->IPstatus |= IOB_IP01;
			}
		}
		else
		{
			ptMod->IPstatus &= ~IOB_IP01;
		}
		break;

	case 0x03: // output status
		ptMod->Outputs =
				((unsigned long int)ptCanM_Rx->Data.uc[1] <<  0) |
				((unsigned long int)ptCanM_Rx->Data.uc[2] <<  8);
		break;

	case 0x04: // voltage
		ptMod->Voltage[0] = ptCanM_Rx->Data.uc[1] | (((unsigned int)ptCanM_Rx->Data.uc[2] << 8) & 0x0f00);
		ptMod->Voltage[1] = (((unsigned int)ptCanM_Rx->Data.uc[2] >> 4) & 0x000f) | (((unsigned int)ptCanM_Rx->Data.uc[3] << 4) & 0x0ff0);
		ptMod->Supply = (float)ptCanM_Rx->Data.uc[7];
		break;

	case 0x05: // current
		ptMod->Current[0] = ptCanM_Rx->Data.uc[1] | (((unsigned int)ptCanM_Rx->Data.uc[2] << 8) & 0x0f00);
		ptMod->Current[1] = (((unsigned int)ptCanM_Rx->Data.uc[2] >> 4) & 0x000f) | (((unsigned int)ptCanM_Rx->Data.uc[3] << 4) & 0x0ff0);
		break;

	case 0x06: // frequency
		ui = ptCanM_Rx->Data.uc[1]; // get requested input
        if( ui < 3 )
        {
			ptMod->frequency[ui] = (ptCanM_Rx->Data.uc[3] << 8) | ptCanM_Rx->Data.uc[2];
			ptMod->pulsecount[ui] =
				(unsigned int)(ptCanM_Rx->Data.uc[4] <<  0) |
				(unsigned int)(ptCanM_Rx->Data.uc[5] <<  8) |
				(unsigned int)(ptCanM_Rx->Data.uc[6] << 16) |
				(unsigned int)(ptCanM_Rx->Data.uc[7] << 24);
        }
		break;

	case 0x07: // voltage threshold reached
		ui = ptCanM_Rx->Data.uc[1]; // get requested input
        if( ui < 4 )
        {
        	ptMod->VoltageThreshold |= (1 << ui); // set bit corresponding to input
        }
		break;

	case 54: // actuator stall status
       	ptMod->StallPulse = ptCanM_Rx->Data.uc[1];
       	ptMod->StallNone = ptCanM_Rx->Data.uc[2];
		break;

	case 0x81: // Weigh Cell voltage & Angles
        ptMod->WeighVoltage.c[0] = ptCanM_Rx->Data.uc[1];
        ptMod->WeighVoltage.c[1] = ptCanM_Rx->Data.uc[2];
        ptMod->WeighVoltage.c[2] = ptCanM_Rx->Data.uc[3];
        ptMod->WeighVoltage.c[3] = ptCanM_Rx->Data.uc[4];
        ptMod->AccPos.x = (float)((signed char)ptCanM_Rx->Data.uc[5]) * (45.0 / 63.0);
        ptMod->AccPos.y = (float)((signed char)ptCanM_Rx->Data.uc[6]) * (45.0 / 63.0);
        ptMod->AccPos.z = (float)((signed char)ptCanM_Rx->Data.uc[7]) * (45.0 / 63.0);
		break;

	case 0x84: // Set Current Accelerometer Values as Zero
        ptMod->ZerPosX.c[0] = ptCanM_Rx->Data.uc[1];
        ptMod->ZerPosX.c[1] = ptCanM_Rx->Data.uc[2];
        ptMod->ZerPosY.c[0] = ptCanM_Rx->Data.uc[3];
        ptMod->ZerPosY.c[1] = ptCanM_Rx->Data.uc[4];
        ptMod->ZerPosZ.c[0] = ptCanM_Rx->Data.uc[5];
        ptMod->ZerPosZ.c[1] = ptCanM_Rx->Data.uc[6];
        // also set as current values
        // fall through
        // no break
	case 0x83: // Get Current Accelerometer Values
        ptMod->CurPosX.c[0] = ptCanM_Rx->Data.uc[1];
        ptMod->CurPosX.c[1] = ptCanM_Rx->Data.uc[2];
        ptMod->CurPosY.c[0] = ptCanM_Rx->Data.uc[3];
        ptMod->CurPosY.c[1] = ptCanM_Rx->Data.uc[4];
        ptMod->CurPosZ.c[0] = ptCanM_Rx->Data.uc[5];
        ptMod->CurPosZ.c[1] = ptCanM_Rx->Data.uc[6];
        break;

	case 0xa0:
		switch( ptCanM_Rx->Data.uc[1] ) // factor requested
		{
		case 0: // COS of Current Composite Angle
	        ptMod->COSangle.c[0] = ptCanM_Rx->Data.uc[2];
	        ptMod->COSangle.c[1] = ptCanM_Rx->Data.uc[3];
	        ptMod->COSangle.c[2] = ptCanM_Rx->Data.uc[4];
	        ptMod->COSangle.c[3] = ptCanM_Rx->Data.uc[5];
			break;

		case 1: // Current Composite Angle
	        ptMod->RESangle.c[0] = ptCanM_Rx->Data.uc[2];
	        ptMod->RESangle.c[1] = ptCanM_Rx->Data.uc[3];
	        ptMod->RESangle.c[2] = ptCanM_Rx->Data.uc[4];
	        ptMod->RESangle.c[3] = ptCanM_Rx->Data.uc[5];
	        break;

		case 3: // Current Weight
	        ptMod->CellWeight.c[0] = ptCanM_Rx->Data.uc[2];
	        ptMod->CellWeight.c[1] = ptCanM_Rx->Data.uc[3];
	        ptMod->CellWeight.c[2] = ptCanM_Rx->Data.uc[4];
	        ptMod->CellWeight.c[3] = ptCanM_Rx->Data.uc[5];
	        break;
		}
		break;

	case IOB_GET_SOFTWARE: // software/issue
		ptMod->software_type = (ptCanM_Rx->Data.uc[2] << 8) | ptCanM_Rx->Data.uc[1];
		ptMod->software_issue = (ptCanM_Rx->Data.uc[4] << 8) | ptCanM_Rx->Data.uc[3];
		break;
	}
	CanStat[IOB_ID][ModuleNumber].rx_status = CS_RECEIVED;
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void CAN0check( void )
{
	// Check CAN Error Flag Bits - Bus Off and Error Passive
	if( CAN1->ESR & 0x06 )
	{
		// abort transmission
		CAN1->TSR |= (CAN_TSR_ABRQ0 | CAN_TSR_ABRQ1 | CAN_TSR_ABRQ2);
	}

	// 3 second timeout
	if( CAN0timeout > 0 )
	{
		CAN0timeout--;
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
static void vCAN0RxTask( void *pvParameters )
{
	struct CanMessage CanM_Rx;
	
	unsigned int ModuleType, ModuleNumber;
	
	while( 1 )
  	{
  		// get message from queue - block indefinitely
		// should unblock when interrupt receives MCM_frame and posts to queue
  		if( xQueueReceive( RxCAN0, (void *)&CanM_Rx, portMAX_DELAY ) == pdTRUE )
    	{
    		// CAN message received so do something with it!
            ModuleType = CanM_Rx.Id.ul & CAN_IDMASK; // get type bits
  			// get id bits, 0x00 to 0x0f for 16 modules of same type
  			ModuleNumber = CAN1_HW + ((CanM_Rx.Id.ul & CAN_TYPEMASK) >> CAN_MOD_BIT_SHIFT);

            // is this message for us?
  			switch( ModuleType )
  			{
  			case CAN_TXID_GWS:
  				ProcessGWSmessages( ModuleNumber, &CanM_Rx );
  				break;

  			case CAN_TXID_MCM:
  				ProcessMCMmessages( ModuleNumber, &CanM_Rx );
  				break;
  				
  			case CAN_TXID_ACM:
  				ProcessACMmessages( ModuleNumber, &CanM_Rx );
  				break;
  				
  			case CAN_TXID_HBM:
  				ProcessHBMmessages( ModuleNumber, &CanM_Rx );
  				break;
  				
  			case CAN_TXID_WCM:
  				ProcessWCMmessages( ModuleNumber, &CanM_Rx );
  				break;
  				
  			case CAN_TXID_DOP:
  				ProcessDOPmessages( ModuleNumber, &CanM_Rx );
  				break;
  				
  			case CAN_TXID_DIP:
  				ProcessDIPmessages( ModuleNumber, &CanM_Rx );
  				break;
  				
  			case CAN_TXID_LMM:
  				ProcessLMMmessages( ModuleNumber, &CanM_Rx );
  				break;
  				
  			case CAN_TXID_IOM:
  				ProcessIOMmessages( ModuleNumber, &CanM_Rx );
  				break;

  			case CAN_TXID_IOC:
  				ProcessIOCmessages( ModuleNumber, &CanM_Rx );
  				break;

  			case CAN_TXID_IOB:
  				ProcessIOBmessages( ModuleNumber, &CanM_Rx );
  				break;
  			}

  			CAN0timeout = CANTIMEOUT;
    	}

  	}	  
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void CAN1check( void )
{
	// Check CAN Error Flag Bits - Bus Off and Error Passive
	if( CAN2->ESR & 0x06 )
	{
		// abort transmission
		CAN2->TSR |= (CAN_TSR_ABRQ0 | CAN_TSR_ABRQ1 | CAN_TSR_ABRQ2);
	}

	// 3 second timeout
	if( CAN1timeout > 0 )
	{
		CAN1timeout--;
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
static void vCAN1RxTask( void *pvParameters )
{
	struct CanMessage CanM_Rx;
	
	unsigned int ModuleType, ModuleNumber;

	while( 1 )
  	{
  		// get message from queue - block indefinitely
		// should unblock when interrupt receives MCM_frame and posts to queue
  		if( xQueueReceive( RxCAN1, (void *)&CanM_Rx, portMAX_DELAY ) == pdTRUE )
    	{
    		// CAN message received so do something with it!
            ModuleType = CanM_Rx.Id.ul & CAN_IDMASK; // get type bits
  			// get id bits, 0x00 to 0x0f for 16 modules of same type
  			ModuleNumber = CAN1_HW + ((CanM_Rx.Id.ul & CAN_TYPEMASK) >> CAN_MOD_BIT_SHIFT);

  			// is this message for us?
  			switch( ModuleType )
  			{
  			case CAN_TXID_MCM:
  				ProcessMCMmessages( ModuleNumber, &CanM_Rx );
  				break;
  				
  			case CAN_TXID_ACM:
  				ProcessACMmessages( ModuleNumber, &CanM_Rx );
  				break;
  				
  			case CAN_TXID_HBM:
  				ProcessHBMmessages( ModuleNumber, &CanM_Rx );
  				break;
  			
  			case CAN_TXID_WCM:
  				ProcessWCMmessages( ModuleNumber, &CanM_Rx );
  				break;
  			
  			case CAN_TXID_DOP:
  				ProcessDOPmessages( ModuleNumber, &CanM_Rx );
  				break;
  			
  			case CAN_TXID_DIP:
  				ProcessDIPmessages( ModuleNumber, &CanM_Rx );
  				break;
  				
  			case CAN_TXID_LMM:
  				ProcessLMMmessages( ModuleNumber, &CanM_Rx );
  				break;
  				
  			case CAN_TXID_IOM:		
  				ProcessIOMmessages( ModuleNumber, &CanM_Rx );
  				break;

  			case CAN_TXID_IOC:
  				ProcessIOCmessages( ModuleNumber, &CanM_Rx );
  				break;

  			case CAN_TXID_IOB:
  				ProcessIOBmessages( ModuleNumber, &CanM_Rx );
  				break;
  			}

  			CAN1timeout = CANTIMEOUT;
    	}
  	}	  
}
/*-----------------------------------------------------------*/

void HBMRoutines(struct CAN_HBM *ptMod, unsigned char message, unsigned char index)
{
	struct CanMessage CanM_Tx;

	ptMod->status &= ~CAN_MODULE_MESSAGE_RECEIVED;
	// set ID of CAN message structure
	CanM_Tx.Id.ul = CAN_RXID_HBM;

	switch(message)
	{
		case HBM_GET_SOFTWARE: // software/issue
			// request software type
			CanM_Tx.Data.uc[0] = HBM_GET_SOFTWARE;
			CanM_Tx.Data.uc[1] = 0;
			CanM_Tx.Data.uc[2] = 0;
			CanM_Tx.Data.uc[3] = 0;
			CanM_Tx.Data.uc[4] = 0;
			CanM_Tx.Data.uc[5] = 0;
			CanM_Tx.Data.uc[6] = 0;
			CanM_Tx.Data.uc[7] = 0;
			break;

		case HBM_GETINPUTS:
			CanM_Tx.Data.uc[0] = HBM_GETINPUTS;
			CanM_Tx.Data.uc[1] = index;
			CanM_Tx.Data.uc[2] = 0;
			CanM_Tx.Data.uc[3] = 0;
			CanM_Tx.Data.uc[4] = 0;
			CanM_Tx.Data.uc[5] = 0;
			CanM_Tx.Data.uc[6] = 0;
			CanM_Tx.Data.uc[7] = 0;
			break;

		case HBM_SETOUTPUTS:
			CanM_Tx.Data.uc[0] = HBM_SETOUTPUTS;
			CanM_Tx.Data.uc[1] = ptMod->HBM_Output[0].Duty[0];
			CanM_Tx.Data.uc[2] = ptMod->HBM_Output[0].Duty[1];
			CanM_Tx.Data.uc[3] = ptMod->HBM_Output[1].Duty[0];
			CanM_Tx.Data.uc[4] = ptMod->HBM_Output[1].Duty[1];
			CanM_Tx.Data.uc[5] = 0;
			CanM_Tx.Data.uc[6] = 0;
			CanM_Tx.Data.uc[7] = 0;
			break;

		case HBM_SETPARAMETERS:
			CanM_Tx.Data.uc[0] = HBM_SETPARAMETERS;

			CanM_Tx.Data.uc[1] = (20.0 * 255/20.9);
			CanM_Tx.Data.uc[2] = 0;
			CanM_Tx.Data.uc[3] = (20.0 * 255/20.9);
			CanM_Tx.Data.uc[4] = 0;
			CanM_Tx.Data.uc[5] = 0;
			CanM_Tx.Data.uc[6] = 0;
			CanM_Tx.Data.uc[7] = 0;
			break;
	}

#ifndef WINDOWS
	CAN1Tx( &CanM_Tx );

	//CAN1Tx( &CanM_Tx );
#endif
}

/*-----------------------------------------------------------
 * Function Name	: CAN0Tx
 * Description		: CAN transmit routine
 * Input			: not used
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void CAN0Tx( struct CanMessage *ptCanM )
{
	ptCanM->DLC = 8; // Data Length
	ptCanM->IDE = CAN_Id_Standard; // CAN_Id_Standard, CAN_Id_Extended
	ptCanM->RTR = CAN_RTR_Data; // CAN_RTR_Data, CAN_RTR_Remote
	ptCanM->FMI = 0; // Filter Number
	ptCanM->TimeStamp = 0;

	// queue message if transmit empty interrupt is ON
	if( CAN1->IER & CAN_IT_TME )
	{
		xQueueSend( TxCAN0, (void *)ptCanM, CANTX_BLOCK_TIME );
	}
	else
	// send message & turn interrupt on if interrupt OFF
	{
		CAN1->IER |= CAN_IT_TME;
		// use mailbox 0 for message if interrupts off

		/* Set up the DLC */
		CAN1->sTxMailBox[0].TDTR = ptCanM->DLC;

		/* Set up the data field */
		CAN1->sTxMailBox[0].TDLR = ptCanM->Data.ul[0];
	    CAN1->sTxMailBox[0].TDHR = ptCanM->Data.ul[1];

	    // Set up the Id & set to transmit
    	CAN1->sTxMailBox[0].TIR = ((ptCanM->Id.ul << ((ptCanM->IDE == CAN_Id_Standard) ? 21 : 3)) | ptCanM->IDE | ptCanM->RTR | TMIDxR_TXRQ);
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: CAN1Tx
 * Description		: CAN transmit routine
 * Input			: not used
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void CAN1Tx( struct CanMessage *ptCanM )
{
	ptCanM->DLC = 8; // Data Length
	ptCanM->IDE = CAN_Id_Standard; // CAN_Id_Standard, CAN_Id_Extended
	ptCanM->RTR = CAN_RTR_Data; // CAN_RTR_Data, CAN_RTR_Remote
	ptCanM->FMI = 0; // Filter Number
	ptCanM->TimeStamp = 0;

	// queue message if transmit empty interrupt is ON
	if( CAN2->IER & CAN_IT_TME )
	{
		xQueueSend( TxCAN1, (void *)ptCanM, CANTX_BLOCK_TIME );
	}
	else
	// send message & turn interrupt on if interrupt OFF
	{
		CAN2->IER |= CAN_IT_TME;
		// use mailbox 0 for message if interrupts off

		/* Set up the DLC */
		CAN2->sTxMailBox[0].TDTR = ptCanM->DLC;

		/* Set up the data field */
		CAN2->sTxMailBox[0].TDLR = ptCanM->Data.ul[0];
	    CAN2->sTxMailBox[0].TDHR = ptCanM->Data.ul[1];

	    // Set up the Id & set to transmit
    	CAN2->sTxMailBox[0].TIR = ((ptCanM->Id.ul << ((ptCanM->IDE == CAN_Id_Standard) ? 21 : 3)) | ptCanM->IDE | ptCanM->RTR | TMIDxR_TXRQ);
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: CAN1_TX_IRQHandler
 * Description		: CAN transmit mailbox empty interrupt
 * Input			: not used
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void CAN1_TX_IRQHandler( void ) // CAN1 TX
{
	long xHigherPriorityTaskWoken = pdFALSE;
	struct CanMessage CanM;
	int txBox, txFlag;

	/* Select one empty transmit mailbox - should not have interrupted unless one is empty! */
	if( CAN1->TSR & CAN_TSR_TME0 )
	{
		txBox = 0;
		txFlag = CAN_TSR_RQCP0;
	}
	else if( CAN1->TSR & CAN_TSR_TME1 )
	{
		txBox = 1;
		txFlag = CAN_TSR_RQCP1;
	}
	else // if( CAN1->TSR & CAN_TSR_TME2 )
	{
		txBox = 2;
		txFlag = CAN_TSR_RQCP2;
	}

	if( xQueueReceiveFromISR( TxCAN0, &CanM, &xHigherPriorityTaskWoken ) == pdTRUE )
	{
		/* Set up the DLC */
		CAN1->sTxMailBox[txBox].TDTR = CanM.DLC;

		/* Set up the data field */
		CAN1->sTxMailBox[txBox].TDLR = CanM.Data.ul[0];
	    CAN1->sTxMailBox[txBox].TDHR = CanM.Data.ul[1];

	    // Set up the Id & set to transmit
    	CAN1->sTxMailBox[txBox].TIR = ((CanM.Id.ul << ((CanM.IDE == CAN_Id_Standard) ? 21 : 3)) | CanM.IDE | CanM.RTR | TMIDxR_TXRQ);
	}
	else
	{
		// no messages or mailbox, then disable transmit empty interrupt
		// CAN_ITConfig( CAN1, CAN_IT_TME, DISABLE );
	    CAN1->IER &= ~CAN_IT_TME;
	}

	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

	// CAN_ClearFlag( CAN1, uint32_t CAN_FLAG );
	// FLAGS
	// CAN_FLAG_RQCP0	Request completed MailBox 0 (transmit or abort)
	// CAN_FLAG_RQCP1	Request completed MailBox 1 (transmit or abort)
	// CAN_FLAG_RQCP2	Request completed MailBox 2 (transmit or abort)
	// INTS
	// CAN_IT_TME		Transmit mailbox empty Interrupt

	// CAN_ClearITPendingBit( CAN1, CAN_IT_TME );
	// CAN_TSR_RQCP0	((uint32_t)0x00000001)
	// CAN_TSR_RQCP1	((uint32_t)0x00000100)
	// CAN_TSR_RQCP2	((uint32_t)0x00010000)
    CAN1->TSR = txFlag;
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: CAN1_RX0_IRQHandler
 * Description		: CAN receive FIFO 0 interrupt
 * Input			: not used
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void CAN1_RX0_IRQHandler( void ) // CAN1 RX0
{
	CAN_FIFOMailBox_TypeDef *ptCAN;
	struct CanMessage CanM;

	long xHigherPriorityTaskWoken = pdFALSE;

	ptCAN = &CAN1->sFIFOMailBox[CAN_FIFO0];

	if( ptCAN->RIR & CAN_Id_Extended )
	{
		CanM.Id.ul = ptCAN->RIR >> 3;
		CanM.IDE = CAN_Id_Extended;
	}
	else
	{
		CanM.Id.ul = ptCAN->RIR >> 21;
		CanM.IDE = CAN_Id_Standard;
	}

	CanM.RTR = ptCAN->RIR & CAN_RTR_Remote;

	// Get the DLC
	CanM.DLC = ptCAN->RDTR & 0x0f;

	// Get the FMI
	CanM.FMI = (ptCAN->RDTR >> 8) & 0xff;

	// get timestamp
	CanM.TimeStamp = ptCAN->RDTR >> 16;

	// Get the data field
	CanM.Data.ul[0] = ptCAN->RDLR;
	CanM.Data.ul[1] = ptCAN->RDHR;

	// Release FIFO0
    CAN1->RF0R |= CAN_RF0R_RFOM0;

	// post the CAN message message to queue
	xQueueSendFromISR( RxCAN0, (void *)&CanM, &xHigherPriorityTaskWoken );

	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

	// RX0
	// FLAGS
	// CAN_FLAG_FMP0	FIFO 0 Message Pending Flag
	// CAN_FLAG_FF0		FIFO 0 Full Flags
	// CAN_FLAG_FOV0	FIFO 0 Overrun Flags

	// INTS
	// CAN_IT_FMP0		FIFO 0 message pending Interrupts
	// CAN_IT_FF0		FIFO 0 full Interrupts
	// CAN_IT_FOV0		FIFO 0 overrun Interrupts

	// CAN_IT_FMP0 cleared by hardware
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: CAN1_RX1_IRQHandler
 * Description		: CAN receive FIFO 1 interrupt
 * Input			: not used
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void CAN1_RX1_IRQHandler( void ) // CAN1 RX1
{
	CAN_FIFOMailBox_TypeDef *ptCAN;
	struct CanMessage CanM;

	long xHigherPriorityTaskWoken = pdFALSE;

	ptCAN = &CAN1->sFIFOMailBox[CAN_FIFO1];

	if( ptCAN->RIR & CAN_Id_Extended )
	{
		CanM.Id.ul = ptCAN->RIR >> 3;
		CanM.IDE = CAN_Id_Extended;
	}
	else
	{
		CanM.Id.ul = ptCAN->RIR >> 21;
		CanM.IDE = CAN_Id_Standard;
	}

	CanM.RTR = ptCAN->RIR & CAN_RTR_Remote;

	// Get the DLC
	CanM.DLC = ptCAN->RDTR & 0x0f;

	// Get the FMI
	CanM.FMI = (ptCAN->RDTR >> 8) & 0xff;

	// get timestamp
	CanM.TimeStamp = ptCAN->RDTR >> 16;

	// Get the data field
	CanM.Data.ul[0] = ptCAN->RDLR;
	CanM.Data.ul[1] = ptCAN->RDHR;

	// Release FIFO1 */
	CAN1->RF1R |= CAN_RF1R_RFOM1;

	// post the CAN message message to queue
	xQueueSendFromISR( RxCAN0, (void *)&CanM, &xHigherPriorityTaskWoken );

	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

	// RX1
	// FLAGS
	// CAN_FLAG_FMP1	FIF0 1 Message Pending Flag
	// CAN_FLAG_FF1		FIFO 1 Full Flags
	// CAN_FLAG_FOV1	FIFO 1 Overrun Flags
	// INTS
	// CAN_IT_FMP1		FIFO 1 message pending Interrupts
	// CAN_IT_FF1		FIFO 1 full Interrupts
	// CAN_IT_FOV1		FIFO 1 overrun Interrupts

	// CAN_IT_FMP1 cleared by hardware
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: CAN1_SCE_IRQHandler
 * Description		: CAN Status Change Error interrupt
 * Input			: not used
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void CAN1_SCE_IRQHandler( void ) // CAN1 SCE
{
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: CAN2_TX_IRQHandler
 * Description		: CAN transmit mailbox empty interrupt
 * Input			: not used
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void CAN2_TX_IRQHandler( void ) // CAN2 TX
{
	long xHigherPriorityTaskWoken = pdFALSE;
	struct CanMessage CanM;
	int txBox, txFlag;

	/* Select one empty transmit mailbox - should not have interrupted unless one is empty! */
	if( CAN2->TSR & CAN_TSR_TME0 )
	{
		txBox = 0;
		txFlag = CAN_TSR_RQCP0;
	}
	else if( CAN2->TSR & CAN_TSR_TME1 )
	{
		txBox = 1;
		txFlag = CAN_TSR_RQCP1;
	}
	else // if( CAN2->TSR & CAN_TSR_TME2 )
	{
		txBox = 2;
		txFlag = CAN_TSR_RQCP2;
	}

	if( xQueueReceiveFromISR( TxCAN1, &CanM, &xHigherPriorityTaskWoken ) == pdTRUE )
	{
		/* Set up the DLC */
		CAN2->sTxMailBox[txBox].TDTR = CanM.DLC;

		/* Set up the data field */
		CAN2->sTxMailBox[txBox].TDLR = CanM.Data.ul[0];
	    CAN2->sTxMailBox[txBox].TDHR = CanM.Data.ul[1];

	    // Set up the Id & set to transmit
    	CAN2->sTxMailBox[txBox].TIR = ((CanM.Id.ul << ((CanM.IDE == CAN_Id_Standard) ? 21 : 3)) | CanM.IDE | CanM.RTR | TMIDxR_TXRQ);
	}
	else
	{
		// no messages or mailbox, then disable transmit empty interrupt
		// CAN_ITConfig( CAN2, CAN_IT_TME, DISABLE );
	    CAN2->IER &= ~CAN_IT_TME;
	}

	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

	// CAN_ClearFlag( CAN2, uint32_t CAN_FLAG );
	// FLAGS
	// CAN_FLAG_RQCP0	Request completed MailBox 0 (transmit or abort)
	// CAN_FLAG_RQCP1	Request completed MailBox 1 (transmit or abort)
	// CAN_FLAG_RQCP2	Request completed MailBox 2 (transmit or abort)
	// INTS
	// CAN_IT_TME		Transmit mailbox empty Interrupt

	// CAN_ClearITPendingBit( CAN2, CAN_IT_TME );
	// CAN_TSR_RQCP0	((uint32_t)0x00000001)
	// CAN_TSR_RQCP1	((uint32_t)0x00000100)
	// CAN_TSR_RQCP2	((uint32_t)0x00010000)
    CAN2->TSR = txFlag;
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: CAN2_RX0_IRQHandler
 * Description		: CAN receive FIFO 0 interrupt
 * Input			: not used
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void CAN2_RX0_IRQHandler( void ) // CAN2 RX0
{
	CAN_FIFOMailBox_TypeDef *ptCAN;
	struct CanMessage CanM;

	long xHigherPriorityTaskWoken = pdFALSE;

	ptCAN = &CAN2->sFIFOMailBox[CAN_FIFO0];

	if( ptCAN->RIR & CAN_Id_Extended )
	{
		CanM.Id.ul = ptCAN->RIR >> 3;
		CanM.IDE = CAN_Id_Extended;
	}
	else
	{
		CanM.Id.ul = ptCAN->RIR >> 21;
		CanM.IDE = CAN_Id_Standard;
	}

	CanM.RTR = ptCAN->RIR & CAN_RTR_Remote;

	// Get the DLC
	CanM.DLC = ptCAN->RDTR & 0x0f;

	// Get the FMI
	CanM.FMI = (ptCAN->RDTR >> 8) & 0xff;

	// get timestamp
	CanM.TimeStamp = ptCAN->RDTR >> 16;

	// Get the data field
	CanM.Data.ul[0] = ptCAN->RDLR;
	CanM.Data.ul[1] = ptCAN->RDHR;

	// Release FIFO0
    CAN2->RF0R |= CAN_RF0R_RFOM0;

	// post the CAN message message to queue
	xQueueSendFromISR( RxCAN1, (void *)&CanM, &xHigherPriorityTaskWoken );

	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

	// RX0
	// FLAGS
	// CAN_FLAG_FMP0	FIFO 0 Message Pending Flag
	// CAN_FLAG_FF0		FIFO 0 Full Flags
	// CAN_FLAG_FOV0	FIFO 0 Overrun Flags

	// INTS
	// CAN_IT_FMP0		FIFO 0 message pending Interrupts
	// CAN_IT_FF0		FIFO 0 full Interrupts
	// CAN_IT_FOV0		FIFO 0 overrun Interrupts

	// CAN_IT_FMP0 cleared by hardware
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: CAN2_RX1_IRQHandler
 * Description		: CAN receive FIFO 1 interrupt
 * Input			: not used
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void CAN2_RX1_IRQHandler( void ) // CAN2 RX1
{
	CAN_FIFOMailBox_TypeDef *ptCAN;
	struct CanMessage CanM;

	long xHigherPriorityTaskWoken = pdFALSE;

	ptCAN = &CAN2->sFIFOMailBox[CAN_FIFO1];

	if( ptCAN->RIR & CAN_Id_Extended )
	{
		CanM.Id.ul = ptCAN->RIR >> 3;
		CanM.IDE = CAN_Id_Extended;
	}
	else
	{
		CanM.Id.ul = ptCAN->RIR >> 21;
		CanM.IDE = CAN_Id_Standard;
	}

	CanM.RTR = ptCAN->RIR & CAN_RTR_Remote;

	// Get the DLC
	CanM.DLC = ptCAN->RDTR & 0x0f;

	// Get the FMI
	CanM.FMI = (ptCAN->RDTR >> 8) & 0xff;

	// get timestamp
	CanM.TimeStamp = ptCAN->RDTR >> 16;

	// Get the data field
	CanM.Data.ul[0] = ptCAN->RDLR;
	CanM.Data.ul[1] = ptCAN->RDHR;

	// Release FIFO1 */
	CAN2->RF1R |= CAN_RF1R_RFOM1;

	// post the CAN message message to queue
	xQueueSendFromISR( RxCAN1, (void *)&CanM, &xHigherPriorityTaskWoken );

	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

	// RX1
	// FLAGS
	// CAN_FLAG_FMP1	FIF0 1 Message Pending Flag
	// CAN_FLAG_FF1		FIFO 1 Full Flags
	// CAN_FLAG_FOV1	FIFO 1 Overrun Flags
	// INTS
	// CAN_IT_FMP1		FIFO 1 message pending Interrupts
	// CAN_IT_FF1		FIFO 1 full Interrupts
	// CAN_IT_FOV1		FIFO 1 overrun Interrupts

	// CAN_IT_FMP1 cleared by hardware
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: CAN2_SCE_IRQHandler
 * Description		: CAN Status Change Error interrupt
 * Input			: not used
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void CAN2_SCE_IRQHandler( void ) // CAN1 SCE
{
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void vStartCANTasks( unsigned portBASE_TYPE uxPriority )
{
	// Initialise CAN peripheral
	InitialiseCAN0();
	InitialiseCAN1();

	// set up task
	xTaskCreate( vCAN0RxTask, "CAN0Rx", canRx_STACK_SIZE, NULL, uxPriority, NULL );
	xTaskCreate( vCAN1RxTask, "CAN1Rx", canRx_STACK_SIZE, NULL, uxPriority, NULL );
}
/*-----------------------------------------------------------*/
