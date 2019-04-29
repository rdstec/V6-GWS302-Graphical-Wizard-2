//============================================================================
// Name        : crc32.c
// Author      : Chris
// Version     :
// Copyright   : Your copyright notice
// Description : calculate the crc32 of data
//============================================================================

//----- Include files ---------------------------------------------------------
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//----- Defines ---------------------------------------------------------------

#define MAXSTRING 256
#define POLYNOMIAL 0x04c11db7UL      // Standard CRC-32 ppolynomial
//#define REFLECT
#define ORDER 32
#define	CRCMASK (((((unsigned int)1 << (ORDER - 1)) - 1) << 1) | 1);
#define	CRCHIGHBIT ((unsigned int)1 << (ORDER - 1));
//#define CRCXOR 0xffffffff
#define CRCXOR 0x00000000

//----- Gloabl variables ------------------------------------------------------

static unsigned int crc_table[256]; // Table of 8-bit remainders


//----- Functions--------------------------------------------------------------

// reflects the lower 'bitnum' bits of 'crc' if needed
unsigned int reflect( unsigned int crc, unsigned int bitnum )
{
    unsigned int i, j, crcout;

    j = 1;
    crcout = 0;

    for( i = (unsigned int)1 << (bitnum - 1); i != 0; i >>= 1 )
    {
    	if( (crc & i) != 0 )
    	{
    		crcout |= j;
    	}
    	j <<= 1;
    }
    return( crcout );
}

//=============================================================================
//=  CRC32 table initialization - not reflected                                              =
//=============================================================================
void generate_crc_table( void )
{
	// make CRC lookup table used by table algorithms
	unsigned short int i, j;
	unsigned int bit, crc;

	for( i = 0; i < 256; i++ )
	{
		crc = (unsigned int)i;

#ifdef REFLECT
		crc = reflect( crc, 8 );
#endif
		crc <<= (ORDER - 8);
		for( j = 0; j < 8; j++ )
		{
			bit = crc & CRCHIGHBIT;
			crc <<= 1;
			if( bit != 0 )
			{
				crc ^= POLYNOMIAL;
			}
		}
#ifdef REFLECT
		crc = reflect( crc, ORDER );
#endif
		crc &= CRCMASK;
		crc_table[i] = crc;
	}
}


//=============================================================================
//=  CRC32 generation                                                         =
//=============================================================================
unsigned int calculate_crc( unsigned int crc_accum, char *pt, unsigned int size )
{
	unsigned int ui, uc;
    unsigned char last[4];

#ifdef REFLECT
	crc_accum = reflect( crc_accum, ORDER );
#endif

	while( size >= 4 )
	{
        size -= 4;

		uc = 4;
		do
		{
			uc--;
#ifdef REFLECT
			ui = (crc_accum ^ *(pt + uc)) & 0xff;
			crc_accum = (crc_accum >> 8) ^ crc_table[ui];
#else
			ui = ((crc_accum >> (ORDER - 8)) ^ *(pt + uc)) & 0xff;
			crc_accum = (crc_accum << 8) ^ crc_table[ui];
#endif
		}
		while( uc );

        pt += 4;
 	}

   	// read remaining data
	if( size )
	{
        memset( last, 0xff, 4 ); // set all to 0xff
        memcpy( last, pt, size ); // copy last remaining byte, should be 3 or less!

        pt = &last[0];
        
		uc = 4;
		do
		{
			uc--;
#ifdef REFLECT
			ui = (crc_accum ^ *(pt + uc)) & 0xff;
			crc_accum = (crc_accum >> 8) ^ crc_table[ui];
#else
			ui = ((crc_accum >> (ORDER - 8)) ^ *(pt + uc)) & 0xff;
			crc_accum = (crc_accum << 8) ^ crc_table[ui];
#endif
		}
		while( uc );
	}

	crc_accum = crc_accum ^ CRCXOR;

	return crc_accum;
}


