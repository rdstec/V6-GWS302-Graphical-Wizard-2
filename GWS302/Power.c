// Graphic Wizard Power Task File

// Include Standard files
#include "global.h"

// variables
/*-----------------------------------------------------------*/
xTaskHandle POWERtaskhandle;

union LongToChar spiFlashID;

unsigned char RunFactorStatus;
unsigned char PowerSwitchedOff;
unsigned char PowerTaskReady;

/*struct SPI_Channel_Map {
	SPI_TypeDef *channel;
	unsigned short cpol;
	unsigned short cpha;
	unsigned short prescaler;
	struct structIoPins csmap;
	};*/
struct SPI_Channel_Map const SPI_map[5] = {
	SPI1, SPI_CPOL_High, SPI_CPHA_2Edge, SPI_BaudRatePrescaler_2,  GPIOA, GPIO_Pin_4,  // CS_DISDAT - SPI0_CS0 @ 30/2  = 15MHz
	SPI1, SPI_CPOL_High, SPI_CPHA_2Edge, SPI_BaudRatePrescaler_2,  GPIOA, GPIO_Pin_6,  // CS_DISCOM - SPI0_CS1 @ 30/2  = 15MHz
	SPI2, SPI_CPOL_High, SPI_CPHA_2Edge, SPI_BaudRatePrescaler_2,  GPIOB, GPIO_Pin_9,  // CS_FLASH  - SPI1_CS0 @ 30/2  = 15MHz
	SPI2, SPI_CPOL_High, SPI_CPHA_2Edge, SPI_BaudRatePrescaler_2,  GPIOC, GPIO_Pin_15, // CS_ATOD   - SPI1_CS1 @ 30/2  = 15MHz
	SPI2, SPI_CPOL_High, SPI_CPHA_2Edge, SPI_BaudRatePrescaler_16, GPIOC, GPIO_Pin_14, // CS_ACCEL  - SPI2_CS1 @ 30/16 = 1.875MHz
	};
/*-----------------------------------------------------------*/

// functions
/*-----------------------------------------------------------
 * Function Name	: SPI_Setup
 * Description		: setup and initialise SPI ports
 * Input			: spi  - SPI_Channel_Map structure index
 * 					:      - CS_DISDAT
 * 					:      - CS_DISCOM
 * 					:      - CS_FLASH
 * 					:      - CS_ATOD
 * 					:      - CS_ACCEL
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void SPI_Setup( enum spiMapIndex spix )
{
	SPI_InitTypeDef SPI_InitStruct;

	struct SPI_Channel_Map const *spi;

	spi = &SPI_map[spix];

	// Set Chip Select high
	spi->csmap.GPIO->BSRRL = spi->csmap.Pin;

	// disable spi channel
	SPI_Cmd( spi->channel, DISABLE );

	// set defaults in setup structure
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = spi->cpol;
	SPI_InitStruct.SPI_CPHA = spi->cpha;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = spi->prescaler;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;

	// initialise spi channel
	SPI_Init( spi->channel, &SPI_InitStruct );

	// enable spi channel
	SPI_Cmd( spi->channel, ENABLE );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name    : SPI2_SendByte
 * Description      : Sends a byte through the SPI interface and return the byte
 *                  :  received from the SPI bus.
 * Input			: spi  - SPI_Channel_Map structure index
 * 					:      - CS_DISDAT
 * 					:      - CS_DISCOM
 * 					:      - CS_FLASH
 * 					:      - CS_ATOD
 * 					:      - CS_ACCEL
 *                  : byte - byte to send.
 * Output           : None
 * Return           : The value of the received byte.
 *-----------------------------------------------------------*/
uint8_t SPI_SendByte( enum spiMapIndex spix, uint8_t byte )
{
	struct SPI_Channel_Map const *spi;

	spi = &SPI_map[spix];

	// Loop while DR register in not empty
	while( SPI_I2S_GetFlagStatus( spi->channel, SPI_I2S_FLAG_TXE ) == RESET )
	{
	}

	// Send byte through the SPI peripheral
	SPI_I2S_SendData( spi->channel, byte );

	// Wait to receive a byte
	while( SPI_I2S_GetFlagStatus( spi->channel, SPI_I2S_FLAG_RXNE ) == RESET )
	{
	}

	// Return the byte read from the SPI bus
	return SPI_I2S_ReceiveData( spi->channel );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: SPI_SendString
 * Description		: send multiple bytes of data
 * Input			: spi   - SPI_Channel_Map structure
 * 					:       - CS_DISDAT
 * 					:       - CS_DISCOM
 * 					:       - CS_FLASH
 * 					:       - CS_ATOD
 * 					:       - CS_ACCEL
 * 					: pbuffer - data buffer
 * 					: count - number of bytes to send
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void SPI_SendString( enum spiMapIndex spix, unsigned char *pbuffer, unsigned int count )
{
	// setup SPI peripheral
	SPI_Setup( spix );

	// select the display command or data chip select
	Set_SPI_CS( spix, CS_LOW );

	while( count > 0 )
	{
		count--;

		SPI_SendByte( spix, *pbuffer++ );
	}

	// reset the display command or data chip select
	Set_SPI_CS( spix, CS_HIGH );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: Set_SPI_CS
 * Description		: set flash chip select low
 * Input			: spi   - SPI_Channel_Map structure
 * 					:       - CS_DISDAT
 * 					:       - CS_DISCOM
 * 					:       - CS_FLASH
 * 					:       - CS_ATOD
 * 					:       - CS_ACCEL
 * 					: level - required level
 * 					:		- CS_LOW
 * 					:		- CS_HIGH
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void Set_SPI_CS( enum spiMapIndex spix, unsigned int level )
{
	struct SPI_Channel_Map const *spi;

	spi = &SPI_map[spix];

	if( level == CS_HIGH )
	{
		spi->csmap.GPIO->BSRRL = spi->csmap.Pin;
	}
	else
	{
		spi->csmap.GPIO->BSRRH = spi->csmap.Pin;
	}

}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name  : SPI_FLASH_SectorErase
 * Description    : Erases the specified FLASH sector.
 * Input          : SectorAddr: address of the sector to erase.
 *                : Size: 4K, 32K or 64K sector size
 * Output         : None
 * Return         : None
 * Note           :
 *-----------------------------------------------------------*/
void SPI_FLASH_SectorErase( uint32_t SectorAddr, uint32_t SectorSize )
{
	/* Send write enable instruction */
	SPI_FLASH_WriteEnable();

	/* Select the FLASH: Chip Select low */
	Set_SPI_CS( CS_FLASH, CS_LOW );

	/* Send Sector Erase instruction */
	SPI_SendByte( CS_FLASH,  SectorSize );
	/* Send SectorAddr high nibble address byte */
	SPI_SendByte( CS_FLASH,  (SectorAddr >> 16) & 0xff );
	/* Send SectorAddr medium nibble address byte */
	SPI_SendByte( CS_FLASH,  (SectorAddr >>  8) & 0xff );
	/* Send SectorAddr low nibble address byte */
	SPI_SendByte( CS_FLASH,  (SectorAddr >>  0) & 0xff );

	/* Deselect the FLASH: Chip Select high */
	Set_SPI_CS( CS_FLASH, CS_HIGH );

	/* Wait the end of Flash writing */
	SPI_FLASH_WaitForWriteEnd();
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name  : SPI_FLASH_ChipErase
 * Description    : Erases the entire FLASH.
 * Input          : None
 * Output         : None
 * Return         : None
 * Note				:
 *-----------------------------------------------------------*/
void SPI_FLASH_ChipErase( void )
{
	/* Send write enable instruction */
	SPI_FLASH_WriteEnable();

	/* Chip Erase */
	/* Select the FLASH: Chip Select low */
	Set_SPI_CS( CS_FLASH, CS_LOW );

	/* Send Bulk Erase instruction  */
	SPI_SendByte( CS_FLASH,  ERASE_ALL1 );

	/* Deselect the FLASH: Chip Select high */
	Set_SPI_CS( CS_FLASH, CS_HIGH );

	/* Wait the end of Flash writing */
	SPI_FLASH_WaitForWriteEnd();
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name  : SPI_FLASH_ByteWrite
 * Description    : Writes byte of data to the FLASH.
 * Input          : - pBuffer : pointer to the buffer  containing the data to be
 *                    written to the FLASH.
 *                  - WriteAddr : FLASH's internal address to write to.
 *                  - NumByteToWrite : number of bytes to write to the FLASH.
 * Output         : None
 * Return         : None
 * Note				:
 *-----------------------------------------------------------*/
void SPI_FLASH_ByteWrite( uint8_t data, uint32_t WriteAddr )
{
	/* Send write enable instruction */
	SPI_FLASH_WriteEnable();

	/* Byte Program */
	/* Select the FLASH: Chip Select low */
	Set_SPI_CS( CS_FLASH, CS_LOW );

	/* Send Write instruction  */
	SPI_SendByte( CS_FLASH,  WRITE );
	/* Send WriteAddr high nibble address byte */
	SPI_SendByte( CS_FLASH,  (WriteAddr >> 16) & 0xff );
	/* Send WriteAddr medium nibble address byte */
	SPI_SendByte( CS_FLASH,  (WriteAddr >>  8) & 0xff );
	/* Send WriteAddr low nibble address byte */
	SPI_SendByte( CS_FLASH,  (WriteAddr >>  0) & 0xff );
	/* Send Data */
	SPI_SendByte( CS_FLASH,  data );

	/* Deselect the FLASH: Chip Select high */
	Set_SPI_CS( CS_FLASH, CS_HIGH );

	/* Wait the end of Flash writing */
	SPI_FLASH_WaitForWriteEnd();
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name  : SPI_FLASH_BufferWrite
 * Description    : Writes block of data to the FLASH. In this function, the
 *                  number of WRITE cycles are reduced, using the Auto Address
 *                  increment sequence.
 * Input          : - pBuffer : pointer to the buffer  containing the data to be
 *                    written to the FLASH. If using a circular buffer this is the
 *                    start of that buffer
 *                  - WriteAddr : FLASH's internal address to write to.
 *                  - NumByteToWrite : number of bytes to write to the FLASH.
 *                  - Offset: if circular buffer is offset from start
 *                    if standard buffer should be 0
 *                  - Size: indicates circular buffer, so wrap offset according to Size
 * Output         : None
 * Return         : None
 * Note				:
 *-----------------------------------------------------------*/
void SPI_FLASH_BufferWrite( uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite, uint32_t Offset, uint32_t Size )
{
	unsigned int bytecount;
	union LongToChar address;

	if( Size )
	{
		Offset = Offset % Size;
	}

	// byte 3 = 0x00
	// byte 2 = Manufacturer
	// byte 1 = Memory Type
	// byte 0 = Capacity
	// SST25VF016B - 0xBF == SST,      0x25 == SPI Serial Flash, 0x41 = 16Mbit SST25V016B (byte/word program)
	//               0x62 == ON,       0x06                      0x13 =  4Mbit
	// S25FL216K   - 0x01 == Spansion, 0x40 == Type              0x15 = 16Mbit Capacity   (page program)
	// S25FL164K   - 0x01 == Spansion, 0x40 == Type              0x17 = 64Mbit Capacity   (page program)
	if( spiFlashID.ul == 0x0BF2541 )
	{
		// byte write & multiple word auto address increment write routine

		SPI_FLASH_BlockProtect( 0 ); // unlocks all blocks

		if( (WriteAddr & 0x01) && NumByteToWrite ) // odd address
		{
			SPI_FLASH_ByteWrite( *(pBuffer + Offset++), WriteAddr++ );
			NumByteToWrite--;
		}

		if( Size )
		{
			Offset = Offset % Size;
		}

		if( NumByteToWrite > 1 )
		{
			/* Send write enable instruction */
			SPI_FLASH_WriteEnable();

			/* Select the FLASH: Chip Select low */
			Set_SPI_CS( CS_FLASH, CS_LOW );
			/* Send Auto Increment and Write instruction  */
			SPI_SendByte( CS_FLASH,  AAIWORD );
			/* Send SectorAddr high nibble address byte */
			SPI_SendByte( CS_FLASH,  (WriteAddr >> 16) & 0xff );
			/* Send SectorAddr medium nibble address byte */
			SPI_SendByte( CS_FLASH,  (WriteAddr >>  8) & 0xff );
			/* Send SectorAddr low nibble address byte */
			SPI_SendByte( CS_FLASH,  (WriteAddr >>  0) & 0xff );
			/* Send Data */
			SPI_SendByte( CS_FLASH,  *(pBuffer + Offset++) );
			if( Size )
			{
				Offset = Offset % Size;
			}
			/* Send Data */
			SPI_SendByte( CS_FLASH,  *(pBuffer + Offset++) );
			if( Size )
			{
				Offset = Offset % Size;
			}
			/* Deselect the FLASH: Chip Select high */
			Set_SPI_CS( CS_FLASH, CS_HIGH );

			WriteAddr += 2;
			NumByteToWrite -= 2;

			/* Wait the end of Flash writing */
			SPI_FLASH_WaitForWriteEnd();

			while( NumByteToWrite > 1 )
			{
				/* Select the FLASH: Chip Select low */
				Set_SPI_CS( CS_FLASH, CS_LOW );
				/* Send Auto Increment and Write instruction  */
				SPI_SendByte( CS_FLASH,  AAIWORD );
				/* Send Data */
				SPI_SendByte( CS_FLASH,  *(pBuffer + Offset++) );
				if( Size )
				{
					Offset = Offset % Size;
				}
				/* Send Data */
				SPI_SendByte( CS_FLASH,  *(pBuffer + Offset++) );
				if( Size )
				{
					Offset = Offset % Size;
				}

				/* Deselect the FLASH: Chip Select high */
				Set_SPI_CS( CS_FLASH, CS_HIGH );

				WriteAddr += 2;
				NumByteToWrite -= 2;

				/* Wait the end of Flash writing */
				SPI_FLASH_WaitForWriteEnd();
			}

			/* Send write disable instruction */
			SPI_FLASH_WriteDisable();
		}

		SPI_FLASH_WaitForWriteEnd();

		if( NumByteToWrite ) // last byte
		{
			SPI_FLASH_ByteWrite( *(pBuffer + Offset), WriteAddr );
		}
	}
	else
	{
		// page program

		address.ul = WriteAddr;
		while( NumByteToWrite != 0 )
		{
			// get program byte count - one page, or remainder
			bytecount = (NumByteToWrite >= 256) ? 256 : NumByteToWrite;
			// decrement after usinf
			NumByteToWrite -= bytecount;

			// Send write enable instruction
			SPI_FLASH_WriteEnable();

			// Select the FLASH: Chip Select low
			Set_SPI_CS( CS_FLASH, CS_LOW );

			// Send Page Program instruction
			SPI_SendByte( CS_FLASH,  WRITE );

			// Send SectorAddr high nibble address byte
			SPI_SendByte( CS_FLASH,  address.uc[2] );
			// Send SectorAddr medium nibble address byte
			SPI_SendByte( CS_FLASH,  address.uc[1] );
			// Send SectorAddr low nibble address byte
			SPI_SendByte( CS_FLASH,  address.uc[0] );

			// increment after using
			address.ul += bytecount;

			while( bytecount != 0 )
			{
				bytecount--;

				// Send Data
				SPI_SendByte( CS_FLASH,  *(pBuffer + Offset++) );
				if( Size )
				{
					Offset = Offset % Size;
				}
			}

			// Deselect the FLASH: Chip Select high
			Set_SPI_CS( CS_FLASH, CS_HIGH );

			// Wait the end of Flash writing
			SPI_FLASH_WaitForWriteEnd();
		}


	}

}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name  : SPI_FLASH_BufferRead
 * Description    : Reads a block of data from the FLASH.
 * Input          : - pBuffer : pointer to the buffer that receives the data read
 *                    from the FLASH.
 *                  - ReadAddr : FLASH's internal address to read from.
 *                  - NumByteToRead : number of bytes to read from the FLASH.
 * Output         : None
 * Return         : None
 * Note				:
 *-----------------------------------------------------------*/
void SPI_FLASH_BufferRead( uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead )
{
	/* Select the FLASH: Chip Select low */
	Set_SPI_CS( CS_FLASH, CS_LOW );

	/* Send "Read from Memory " instruction */
	SPI_SendByte( CS_FLASH,  HSREAD );

	/* Send ReadAddr high nibble address byte to read from */
	SPI_SendByte( CS_FLASH,  (ReadAddr >> 16) & 0xff );
	/* Send ReadAddr medium nibble address byte to read from */
	SPI_SendByte( CS_FLASH,  (ReadAddr >>  8) & 0xff );
	/* Send ReadAddr low nibble address byte to read from */
	SPI_SendByte( CS_FLASH,  (ReadAddr >>  0) & 0xff );
	/* Send Dummy Byte */
	SPI_SendByte( CS_FLASH,  DUMMY_BYTE );

	while( NumByteToRead-- ) /* while there is data to be read */
	{
		/* Read a byte from the FLASH */
		*pBuffer++ = SPI_SendByte( CS_FLASH,  DUMMY_BYTE );
		/* Point to the next location where the byte read will be saved */
		//pBuffer++;
	}

	/* Deselect the FLASH: Chip Select high */
	Set_SPI_CS( CS_FLASH, CS_HIGH );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name : SPI_FLASH_CheckID
 * Description   : Checks SPI flash ID against known ID's
 * Input         : None
 * Output        : None
 * Return        : FLASH identification
 * Note          :
 *-----------------------------------------------------------*/
uint32_t SPI_FLASH_CheckID( void )
{
	unsigned int id, ReadCount;

	// read flash ID until valid, reset will occur after 20 failed attempts
	ReadCount = 0;

	#define FLASHID_SST25VF016B 0x0BF2541 // SST25VF016B, 0xBF == SST,      0x25 == SPI Serial Flash, 0x41 = SST25V016B (byte program)
	#define FLASHID_S25FL164K   0x0014017 // S25FL164K,   0x01 == Spansion, 0x40 == Type              0x17 = 64Mbit Capacity (page program)

	do
	{
		// if PFAIL pin low then reset
		CheckPowerFail();

		// read id
		id = SPI_FLASH_ReadID();

		// check id
		switch( id )
		{
		// known spi flash ids
		case FLASHID_SST25VF016B:
		case FLASHID_S25FL164K:
			break;

		// unknow id
		default:
			ReadCount++;
			if( ReadCount > 20 ) // after 20 tries
			{
				// stay here until watchdog reset after about 2 seconds
				while( 1 )
				{
				}
				// NVIC_SystemReset(); Privileged only
			}

			// short delay before trying again
			id = 50000;
			while( id > 0 )
			{
				id--;
			}
			break;
		}
	}
	while( id == 0 );

	return( id );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name  : SPI_FLASH_ReadID
 * Description    : Reads FLASH identification.
 * Input          : None
 * Output         : None
 * Return         : FLASH identification
 * Note				:
 *-----------------------------------------------------------*/
uint32_t SPI_FLASH_ReadID( void )
{
	union LongToChar id;

	id.uc[3] = 0;

	/* Select the FLASH: Chip Select low */
	Set_SPI_CS( CS_FLASH, CS_LOW );

	/* Send "JEDEC_ID" instruction */
	SPI_SendByte( CS_FLASH,  JEDEC_ID );
	/* Read a byte from the FLASH */
	id.uc[2] = SPI_SendByte( CS_FLASH,  DUMMY_BYTE );
	/* Read a byte from the FLASH */
	id.uc[1] = SPI_SendByte( CS_FLASH,  DUMMY_BYTE );
	/* Read a byte from the FLASH */
	id.uc[0] = SPI_SendByte( CS_FLASH,  DUMMY_BYTE );

	/* Deselect the FLASH: Chip Select high */
	Set_SPI_CS( CS_FLASH, CS_HIGH );

	return id.ul;
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name  : SPI_FLASH_StartReadSequence
 * Description    : Initiates a read data byte (READ) sequence from the Flash.
 *                  This is done by driving the /CS line low to select the device,
 *                  then the READ instruction is transmitted followed by 3 bytes
 *                  address. This function exit and keep the /CS line low, so the
 *                  Flash still being selected. With this technique the whole
 *                  content of the Flash is read with a single READ instruction.
 * Input          : - ReadAddr : FLASH's internal address to read from.
 * Output         : None
 * Return         : None
 * Note				:
 *-----------------------------------------------------------*/
void SPI_FLASH_StartReadSequence( uint32_t ReadAddr )
{
	/* Select the FLASH: Chip Select low */
	Set_SPI_CS( CS_FLASH, CS_LOW );

	/* Send "Read from Memory " instruction */
	SPI_SendByte( CS_FLASH,  HSREAD );

	/* Send the 24-bit address of the address to read from
	/* Send ReadAddr high nibble address byte */
	SPI_SendByte( CS_FLASH,  (ReadAddr >> 16) & 0xff );
	/* Send ReadAddr medium nibble address byte */
	SPI_SendByte( CS_FLASH,  (ReadAddr >>  8) & 0xff );
	/* Send ReadAddr low nibble address byte */
	SPI_SendByte( CS_FLASH,  (ReadAddr >>  0) & 0xff );
	/* Send Dummy Byte */
	SPI_SendByte( CS_FLASH,  DUMMY_BYTE );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name : SPI_FLASH_BlockProtect
 * Description   : Clears or Sets Block Protect Bits
 * Input         : Block Protect bits
 * 				 : BP0_FLAG, BP1_FLAG, BP2_FLAG & BPL_FLAG
 * Output        : None
 * Return        : None
 * Note          :
 *-----------------------------------------------------------*/
void SPI_FLASH_BlockProtect( uint8_t block )
{

	/* Select the FLASH: Chip Select low */
	Set_SPI_CS( CS_FLASH, CS_LOW );
	/* Send "Enable Write Status Register" instruction */
	SPI_SendByte( CS_FLASH,  EWSR );
	/* Deselect the FLASH: Chip Select high */
	Set_SPI_CS( CS_FLASH, CS_HIGH );

	/* Select the FLASH: Chip Select low */
	Set_SPI_CS( CS_FLASH, CS_LOW );
	/* Send "Write Status Register" instruction */
	SPI_SendByte( CS_FLASH,  WRSR );
	SPI_SendByte( CS_FLASH,  block );
	/* Deselect the FLASH: Chip Select high */
	Set_SPI_CS( CS_FLASH, CS_HIGH );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name  : SPI_FLASH_WriteEnable
 * Description    : Enables the write access to the FLASH.
 * Input          : None
 * Output         : None
 * Return         : None
 * Note				:
 *-----------------------------------------------------------*/
void SPI_FLASH_WriteEnable( void )
{
	/* Select the FLASH: Chip Select low */
	Set_SPI_CS( CS_FLASH, CS_LOW );

	/* Send "Write Enable" instruction */
	SPI_SendByte( CS_FLASH,  WREN );

	/* Deselect the FLASH: Chip Select high */
	Set_SPI_CS( CS_FLASH, CS_HIGH );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name  : SPI_FLASH_WriteDisable
 * Description    : Disable write access to the FLASH.
 * Input          : None
 * Output         : None
 * Return         : None
 * Note				:
 *-----------------------------------------------------------*/
void SPI_FLASH_WriteDisable( void )
{
	/* Select the FLASH: Chip Select low */
	Set_SPI_CS( CS_FLASH, CS_LOW );

	/* Send "Write Disable" instruction */
	SPI_SendByte( CS_FLASH,  WRDI );

	/* Deselect the FLASH: Chip Select high */
	Set_SPI_CS( CS_FLASH, CS_HIGH );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name  : SPI_FLASH_WaitForWriteEnd
 * Description    : Polls the status of the Write In Progress (WIP) flag in the
 *                  FLASH's status  register  and  loop  until write  opertaion
 *                  has completed.
 * Input          : None
 * Output         : None
 * Return         : None
 * Note				:
 *-----------------------------------------------------------*/
uint8_t SPI_FLASH_WaitForWriteEnd( void )
{
	uint8_t FStatus = 0;

	/* Select the FLASH: Chip Select low */
	Set_SPI_CS( CS_FLASH, CS_LOW );

	/* Send "Read Status Register" instruction */
	SPI_SendByte( CS_FLASH,  RDSR );

	/* Loop as long as the memory is busy with a write cycle */
	do
	{
		/* Send a dummy byte to generate the clock needed by the FLASH
    		and put the value of the status register in FStatus variable */
		FStatus = SPI_SendByte( CS_FLASH,  DUMMY_BYTE );
	}
	while( (FStatus & WIP_FLAG) == SET ); /* Write in progress */

	/* Deselect the FLASH: Chip Select high */
	Set_SPI_CS( CS_FLASH, CS_HIGH );

	return( FStatus );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: SPI_FLASH_EraseSectors
 * Description		: Erase flash data prior to writing
 * Input			: Address of first sector to erase
 *					: Number of bytes to erase
 * Output			: None
 * Return			: None
 * Note				: multiples of 4k will be erased
 *-----------------------------------------------------------*/
void SPI_FLASH_EraseSectors( uint32_t EraseAddr, uint32_t NumByteToErase )
{
	unsigned int ul;

	// calculate number of 4K sectors to erase
	ul = (NumByteToErase + (SPI_FLASH_SECTOR_SIZE - 1)) / SPI_FLASH_SECTOR_SIZE;
	while( ul )
	{
		ul--;

		// erase data in multiples of 4K
		SPI_FLASH_SectorErase( EraseAddr, ERASE_4K );

		// address in next 4K sector
		EraseAddr += SPI_FLASH_SECTOR_SIZE;
	}

}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: SPI_FLASH_EraseTheWrite
 * Description		: Erase then Write block of datat to spi flash
 * Input			: pointer to data to write
 * 					: address of first sector to write
 * 					: number of bytes to write
 * 					: circular buffer size if not 0
 * Output			: None
 * Return			: None
 * Note				: multiples of 4k will be erased
 *-----------------------------------------------------------*/
void SPI_FLASH_EraseThenWrite( uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite, uint32_t Offset, uint32_t Size )
{

	// erase data first - in multiples of 4K blocks
	SPI_FLASH_EraseSectors( WriteAddr, NumByteToWrite );
	// write data to SPI1 Flash
	SPI_FLASH_BufferWrite( pBuffer, WriteAddr, NumByteToWrite, Offset, Size );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: CheckPowerFail
 * Description		: checks status of power fail pin
 * 					: if low waits for watch dog reset
 * Input			: None
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void CheckPowerFail( void )
{

	// if PFAIL pin low then reset if not PowerTask
	if( INPUT_PFAIL == 0 )
	{
		// stay here until watchdog reset after about 2 seconds
		while( 1 )
		{
		}
		// NVIC_SystemReset(); Privileged only
	}
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Function Name	: SPI_FLASH_DataRestore
 * Description		: restores saved Factors, Totals and other
 * 					: saved data
 * Input			: None
 * Output			: None
 * Return			: None
 * Note				:
 *-----------------------------------------------------------*/
void SPI_FLASH_DataRestore( void )
{
	unsigned short int ReadCount;

	// read factors from SPI1 Flash
	SPI_FLASH_BufferRead( (uint8_t *)&pFac, pFac_BASEADDRESS_MAIN, sizeof( pFac ) );

	// if PFAIL pin low then reset
	CheckPowerFail();
	
	// set ok
	RunFactorStatus = RFS_CLEAR;

	// checksum fail on main block, then try backup block
	if( pFac.pf.CheckSum != CheckSumIt( (uint8_t *)&pFac, (unsigned char *)&(pFac.pf.CheckSum) ) )
	{
#ifdef DEBUG
		debug_message( "FDR: CheckSum Factors Main Fail\r\n" );
#endif
		// read factors from backup block
		SPI_FLASH_BufferRead( (uint8_t *)&pFac, pFac_BASEADDRESS_BACK, sizeof( pFac ) );

		// if PFAIL pin low then reset
		CheckPowerFail();

		// checksum backup block
		if( pFac.pf.CheckSum != CheckSumIt( (uint8_t *)&pFac, (unsigned char *)&(pFac.pf.CheckSum) ) )
		{
#ifdef DEBUG
			debug_message( "FDR: CheckSum Factors Backup Fail\r\n" );
#endif
			// second set fail...
			RunFactorStatus = RFS_CS2FAIL;
		}
		else
		{
			RunFactorStatus = RFS_CS1FAIL;
		}
	}
#ifdef DEBUG
	else
	{
		debug_message( "FDR: CheckSum Factors OK\r\n" );
	}
#endif

	// checksum ok, then has software changed
	if( RunFactorStatus == RFS_CLEAR )
	{
		if( pFac.pf.software_type != SOFTWARE )
		{
#ifdef DEBUG
			debug_message( "FDR: Software Changed\r\n" );
#endif
			RunFactorStatus = RFS_SW1FAIL;
		}
		else if( pFac.pf.software_issue != ISSUE )
		{
#ifdef DEBUG
			debug_message( "FDR: Issue Changed\r\n" );
#endif
			RunFactorStatus = RFS_IS1FAIL;
		}
		else if( pFac.pf.software_revision != REVISION )
		{
#ifdef DEBUG
			debug_message( "FDR: Issue Changed\r\n" );
#endif
			RunFactorStatus = RFS_RV1FAIL;
		}
	}
	
	// set defaults
	if( RunFactorStatus != RFS_CLEAR )
	{
#ifdef DEBUG
		debug_message( "\r\nFDR: Setting Defaults\r\n" );
#endif
		// set default program factors & write to flash
		SetDefaultFactors();
		SetDefaultTotals();
	}
	else
	{
		// read runtime factors and totals from SPI1 Flash
		SPI_FLASH_BufferRead( (uint8_t *)&pRun, pRun_BASEADDRESS_MAIN, sizeof( pRun ) );

		// if PFAIL pin low then reset
		CheckPowerFail();

		// checksum fail on first block ?
		if( pRun.pr.CheckSum != CheckSumIt( (uint8_t *)&pRun, (unsigned char *)&(pRun.pr.CheckSum) ) )
		{
#ifdef DEBUG
			debug_message( "FDR: CheckSum Totals Main Fail\r\n" );
#endif
			// first block checksum fail, then try second block
			RunFactorStatus = RFS_CS3FAIL;

			// read runtime factors and totals from SPI1 Flash
			SPI_FLASH_BufferRead( (uint8_t *)&pRun, pRun_BASEADDRESS_BACK, sizeof( pRun ) );

			// if PFAIL pin low then reset
			CheckPowerFail();

			if( pRun.pr.CheckSum != CheckSumIt( (uint8_t *)&pRun, (unsigned char *)&(pRun.pr.CheckSum) ) )
			{
#ifdef DEBUG
				debug_message( "FDR: CheckSum Totals Backup Fail\r\n" );
#endif
				// second set fail...
				RunFactorStatus = RFS_CS4FAIL;

				// set default runtime factors and reset totals
				// main runtime data saved at power down, backup block saved now
				SetDefaultTotals();
			}

		}
#ifdef DEBUG
		else
		{
			// ###test
			debug_message( "FDR: CheckSum Totals OK\r\n" );
		}
#endif
	}

	// if current factors come from backup block, or a totals reset, there is no need to re-write them
	if( (RunFactorStatus != RFS_CS3FAIL) && (RunFactorStatus != RFS_CS4FAIL) )
	{
		// try writing backup block up to 5 times
		ReadCount = 5;
		while( ReadCount )
		{
			// if PFAIL pin low then reset
			CheckPowerFail();

			// erase runtime factors and totals backup
			SPI_FLASH_EraseThenWrite( (uint8_t *)&pRun, pRun_BASEADDRESS_BACK, sizeof( pRun ), 0, 0 );

			// if PFAIL pin low then reset
			CheckPowerFail();

			// read back runtime factors and totals from SPI1 Flash
			SPI_FLASH_BufferRead( (uint8_t *)&pRun, pRun_BASEADDRESS_BACK, sizeof( pRun ) );

			// check the checksum to see if they have been saved ok
			if( pRun.pr.CheckSum == CheckSumIt( (uint8_t *)&pRun, (unsigned char *)&(pRun.pr.CheckSum) ) )
			{
				// exit if checksum ok
				break;
			}

			if( ReadCount ) // after 5 failed tries wait for watch dog reset
			{
				ReadCount--;
			}
			else
			{
				// stay here until watch dog reset after about 2 seconds
				while( 1 )
				{
				}
				// NVIC_SystemReset(); Privileged only
			}

			// read runtime factors and totals from SPI1 Flash
			SPI_FLASH_BufferRead( (uint8_t *)&pRun, pRun_BASEADDRESS_MAIN, sizeof( pRun ) );

#ifdef DEBUG
			debug_message( "FDR: Backup Totals\r\n" );
#endif
		}
	}

	// if PFAIL pin low then reset
	CheckPowerFail();

	// erase runtime factors and totals ready for power down save
	SPI_FLASH_EraseSectors( pRun_BASEADDRESS_MAIN, sizeof( pRun ) );

	SetStartupDefaults();

	pRun.pr.PowerCount++;
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
//* Function Name       : vPowerTask
//* Object              : Save Totals to SPI Flash at power down
//* Output Parameters   :
/*-----------------------------------------------------------*/
static void vPowerTask( void *pvParameters )
{
	unsigned int ix;

	// LED_RED off
	OUTPUT_LED_RED_OFF;

	// LED_AMBER on
	//GPIOD->BSRRH = GPIO_Pin_11;
	// LED_GEEN on
	//GPIOE->BSRRH = GPIO_Pin_12;

	// if PFAIL level ok after scheduler has started, and interrupt hasn't fired, suspend task
	// will be resumed by PFAIL interrupt
	if( (INPUT_PFAIL != 0) && (PowerTaskReady != PT_INTERRUPT) )
	{
		// enable power fail interrupt action
		PowerTaskReady = PT_READY;

		// suspend current task
		// task is awakened by PFAIL pin interrupt
		vTaskSuspend( NULL );
	}
	
	// else we save factors and wait for reset

	// LED_RED on
	OUTPUT_LED_RED_ON;

	// all interrupts off - need privileges to turn interrupts off
	// taskDISABLE_INTERRUPTS();
	// portDISABLE_INTERRUPTS();
	__disable_irq();
	__disable_fault_irq();
	
	// de select all spi peripherals
	for( ix = 0; ix < CS_MAX; ix++ )
	{
		Set_SPI_CS( ix, CS_HIGH );
	}

	// turn all pwm outputs off
	for( ix = 0; ix < MAX_PWM_OUTPUTS; ix++ )
	{
		SetOutputPWM( ix, 0, PWM_DUTY000 );
	}

	// LED_RED off
	//GPIOA->BSRRL = GPIO_Pin_8;
	// LED_AMBER off
	GPIOD->BSRRL = GPIO_Pin_11;
	// BUZZER 0ff, LED_GEEN off
	GPIOE->BSRRL = GPIO_Pin_3 | GPIO_Pin_12;

	// reset registers
	CAN_DeInit( CAN1 );
	// reset registers
	CAN_DeInit( CAN2 );

	// setup SPI1 peripheral for external flash
	SPI_Setup( CS_FLASH );

	// disable new writes if any
	SPI_FLASH_WriteDisable();

	pRun.pr.CheckSum = CheckSumIt( (uint8_t *)&pRun, (unsigned char *)&(pRun.pr.CheckSum) );

	// wait until any programming is complete
	SPI_FLASH_WaitForWriteEnd();

	// write runtime factors and totals
	SPI_FLASH_BufferWrite( (uint8_t *)&pRun, pRun_BASEADDRESS_MAIN, sizeof( pRun ), 0, 0 );

	// LED_RED off
	OUTPUT_LED_RED_OFF;

	if( PowerSwitchedOff == 1 )	
	{
		// set /PFAIL input to output mode, and set low to 
		// indicate to PIC16F506 that power should remain off
		// if removed then restored

		// need GPIOB peripheral clock for PFAIL pin5
		GPIOB->MODER &= ~0x00000030; // PB02 is normally input (00), set to output (01)
		GPIOB->MODER |=  0x00000010;
		GPIOB->BSRRH = GPIO_Pin_2; // set 'PFAIL' to LOW, high byte for reset bits, low byte for set bits
		// if ECU PIC attempts to read this pin now it will appear LOW, PIC in input mode with pullup
		// would read pin HIGH if PFAIL still set as input
	}

	SetOutputOUT( OUT_SHUTDOWN, OUT_OFF );

	// all done so stay here
	while( 1 ) 
	{
		// reload watchdog countdown
		IWDG->KR = 0xAAAA;
	}

}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
void vStartPowerTask( unsigned portBASE_TYPE uxPriority )
{

	/* Spawn the task. */
	xTaskCreate( vPowerTask, "POWER", configPOWER_STACK_SIZE, (void *)NULL, uxPriority, (xTaskHandle *)&POWERtaskhandle );
}
/*-----------------------------------------------------------*/


