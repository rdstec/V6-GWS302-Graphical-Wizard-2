/*
 * Power.h
 *
 *  Created on: 25 May 2016
 *      Author: cparsons
 */

#ifndef POWER_H_
#define POWER_H_

// defines
/*-----------------------------------------------------------*/
#define mainPOWER_TASK_PRIORITY    (tskIDLE_PRIORITY + 4)
#define configPOWER_STACK_SIZE configMINIMAL_STACK_SIZE

#define PT_READY     0xb6
#define PT_INTERRUPT 0xd7

// set 4k regions
#define pFac_BASEADDRESS_MAIN 0x00000
#define pFac_BASEADDRESS_BACK 0x01000

// set 4k regions
#define pRun_BASEADDRESS_MAIN 0x02000
#define pRun_BASEADDRESS_BACK 0x03000

#define CS_LOW  0
#define CS_HIGH 1

enum spiMapIndex {
	CS_DISDAT, // 0
	CS_DISCOM, // 1
	CS_FLASH,  // 2
	CS_ATOD,   // 3
	CS_ACCEL,  // 4
	CS_MAX
	};

#define RFS_CLEAR	0
#define RFS_CS1FAIL	1
#define RFS_CS2FAIL 2
#define RFS_SW1FAIL 3
#define RFS_IS1FAIL 4
#define RFS_RV1FAIL 5
#define RFS_SW2FAIL 6
#define RFS_IS2FAIL 7
#define RFS_RV2FAIL 8
#define RFS_CS3FAIL 9
#define RFS_CS4FAIL 10

// flash
#define SPI_FLASH_SECTOR_SIZE	4096

#define DUMMY_BYTE	0xA5

#define READ		0x03	// Read Read Memory at 25 MHz		3	0	1 to n	25 MHz
#define HSREAD		0x0B	// Read Memory at 80 MHz			3	1	1 to n	80 MHz

#define ERASE_4K	0x20	// Erase 4 KByte block     			3	0	0		80 MHz
//#define ERASE_32K	0x52	// Erase 32 KByte block				3	0	0		80 MHz
#define ERASE_64K   0XD8	// Erase 64 KByte block				3	0	0		80 MHz
#define ERASE_ALL1	0x60	// Erase Chip						0	0	0		80 MHz
#define ERASE_ALL2	0xC7	// Erase Chip						0	0	0		80 MHz

#define WREN		0x06	// Write-Enable						0	0	0		80 MHz
#define WRDI		0x04	// Write-Disable					0	0	0		80 MHz
#define WRITE		0x02	// Byte Program						3	0	1		80 MHz
#define AAIWORD		0xAD	// Auto Address Increment Program	3	0	2 to n	80 MHz
#define EBSY		0x70	// Enable SO as AAI status pin		0	0	0		80 MHz
#define DBSY		0x80	// Disable SO as AAI status pin		0	0	0		80 MHz

#define RDSR		0x05	// Read-Status-Register				0	0	1 to n	80 MHz
#define EWSR		0x50	// Enable-Write-Status-Register		0	0	0		80 MHz
#define WRSR 		0x01	// Write-Status-Register			0	0	1		80 MHz
#define RDID1		0x90	// Read-ID							3	0	1 to n	80 MHz
#define RDID2		0xAB	// Read-ID							3	0	1 to n	80 MHz
#define JEDEC_ID	0x9F	// JEDEC ID read					0	0	3 to n	80 MHz

#define WIP_FLAG	0x01	// Write In Progress (WIP) flag
#define WEL_FLAG	0x02	// Memory Write Enabled flag
#define BP0_FLAG	0x04	// Block Write Protection flag 0
#define BP1_FLAG	0x08	// Block Write Protection flag 1
#define BP2_FLAG	0x10	// Block Write Protection flag 2
#define BP3_FLAG	0x20	// Block Write Protection flag 3
#define AAI_FLAG	0x40	// Auto Address Increment flag
#define BPL_FLAG	0x80	// Block Write Protection flag bits write status flag

// ADXL Register Map
#define ACC_DEVID			0x00 // R 	Device ID
#define ACC_THRESH_TAP		0x1D // R/W Tap threshold
#define ACC_OFSX			0x1E // R/W X-axis offset
#define ACC_OFSY			0x1F // R/W Y-axis offset
#define ACC_OFSZ			0x20 // R/W	Z-axis offset
#define ACC_DUR				0x21 // R/W	Tap duration
#define ACC_Latent			0x22 // R/W	Tap latency
#define ACC_Window			0x23 // R/W	Tap window
#define ACC_THRESH_ACT		0x24 //	R/W	Activity threshold
#define ACC_THRESH_INACT	0x25 // R/W	Inactivity threshold
#define ACC_TIME_INACT		0x26 // R/W	Inactivity time
#define ACC_ACT_INACT_CTL	0x27 // R/W	Axis enable control for activity and inactivity detection
#define ACC_THRESH_FF		0x28 // R/W	Free-fall threshold
#define ACC_TIME_FF			0x29 // R/W	Free-fall time
#define ACC_TAP_AXES		0x2A // R/W	Axis control for single tap/double tap
#define ACC_ACT_TAP_STATUS	0x2B // R	Source of single tap/double tap
#define ACC_BW_RATE			0x2C // R/W	Data rate and power mode control
#define ACC_POWER_CTL		0x2D // R/W Power-saving features control
#define ACC_INT_ENABLE		0x2E // R/W	Interrupt enable control
#define ACC_INT_MAP			0x2F // R/W	Interrupt mapping control
#define ACC_INT_SOURCE		0x30 // R	Source of interrupts
#define ACC_DATA_FORMAT		0x31 // R/W	Data format control
#define ACC_DATAX0			0x32 // R	X-Axis Data 0
#define ACC_DATAX1			0x33 // R	X-Axis Data 1
#define ACC_DATAY0			0x34 // R	Y-Axis Data 0
#define ACC_DATAY1			0x35 // R	Y-Axis Data 1
#define ACC_DATAZ0			0x36 // R	Z-Axis Data 0
#define ACC_DATAZ1			0x37 // R	Z-Axis Data 1
#define ACC_FIFO_CTL		0x38 // R/W	FIFO control
#define ACC_FIFO_STATUS		0x39 // R	FIFO status

// Address Bits
#define ACC_READ			(1<<7)
#define ACC_WRITE			(0<<7)
#define ACC_MULTIBYTE 		(1<<6)

// Power Control Register Bits
#define ACC_WU_0			(1<<0)	//Wake Up Mode - Bit 0
#define	ACC_WU_1			(1<<1)	//Wake Up mode - Bit 1
#define ACC_SLEEP			(1<<2)	//Sleep Mode
#define	ACC_MEASURE			(1<<3)	//Measurement Mode
#define ACC_AUTO_SLP		(1<<4)	//Auto Sleep Mode bit
#define ACC_LINK			(1<<5)	//Link bit

// Interrupt Enable/Interrupt Map/Interrupt Source Register Bits
#define	ACC_OVERRUN			(1<<0)
#define	ACC_WATERMARK		(1<<1)
#define ACC_FREE_FALL		(1<<2)
#define	ACC_INACTIVITY		(1<<3)
#define	ACC_ACTIVITY		(1<<4)
#define ACC_DOUBLE_TAP		(1<<5)
#define	ACC_SINGLE_TAP		(1<<6)
#define	ACC_DATA_READY		(1<<7)

// Data Format Bits
#define ACC_RANGE_0			(1<<0)
#define	ACC_RANGE_1			(1<<1)
#define ACC_JUSTIFY			(1<<2)
#define	ACC_FULL_RES		(1<<3)
#define	ACC_INT_INVERT		(1<<5)
#define	ACC_SPI				(1<<6)
#define	ACC_SELF_TEST		(1<<7)

// FIFO Bits
#define ACC_FIFO_MODE_BYPASS	(0<<6)
#define ACC_FIFO_MODE_PAUSE		(1<<6)
#define ACC_FIFO_MODE_STREAM	(2<<6)
#define ACC_FIFO_MODE_TRIGGER	(3<<6)
#define ACC_FIFO_TRIGGER		(1<<5)
/*-----------------------------------------------------------*/

// variables
/*-----------------------------------------------------------*/
extern xTaskHandle POWERtaskhandle;

extern union LongToChar spiFlashID;

extern unsigned char RunFactorStatus;
extern unsigned char PowerSwitchedOff;
extern unsigned char PowerTaskReady;

struct SPI_Channel_Map {
	SPI_TypeDef *channel;
	unsigned short cpol;
	unsigned short cpha;
	unsigned short prescaler;
	struct structIoPins csmap;
	};
extern struct SPI_Channel_Map const SPI_map[5];

/*-----------------------------------------------------------*/

// function prototypes
/*-----------------------------------------------------------*/
extern void SPI_Setup( enum spiMapIndex spix );
extern uint8_t SPI_SendByte( enum spiMapIndex spix, uint8_t byte );
extern void SPI_SendString( enum spiMapIndex spix, unsigned char *pbuffer, unsigned int count );
extern void Set_SPI_CS( enum spiMapIndex spix, unsigned int level );

extern void SPI_FLASH_SectorErase( uint32_t SectorAddr, uint32_t SectorSize );
extern void SPI_FLASH_ChipErase( void );
extern void SPI_FLASH_ByteWrite( uint8_t data, uint32_t WriteAddr );
extern void SPI_FLASH_BufferWrite( uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite, uint32_t Offset, uint32_t Size );
extern void SPI_FLASH_BufferRead( uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead );
extern uint32_t SPI_FLASH_CheckID( void );
extern uint32_t SPI_FLASH_ReadID( void );
extern void SPI_FLASH_StartReadSequence( uint32_t ReadAddr );
extern void SPI_FLASH_BlockProtect( uint8_t block );
extern void SPI_FLASH_WriteEnable( void );
extern void SPI_FLASH_WriteDisable( void );
extern uint8_t SPI_FLASH_WaitForWriteEnd( void );
extern void SPI_FLASH_EraseSectors( uint32_t EraseAddr, uint32_t NumByteToErase );
extern void SPI_FLASH_EraseThenWrite( uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite, uint32_t Offset, uint32_t Size );
extern void CheckPowerFail( void );
extern void SPI_FLASH_DataRestore( void );
//static void vPowerTask( void *pvParameters )
extern void vStartPowerTask( unsigned portBASE_TYPE uxPriority );
/*-----------------------------------------------------------*/

#endif /* POWER_H_ */
