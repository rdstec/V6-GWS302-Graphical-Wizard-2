/*
 * common.h
 *
 *  Created on: 6 Nov 2015
 *      Author: chris
 */

#ifndef COMMON_H_
#define COMMON_H_

/*-----------------------------------------------------------*/
#define HARDWARE GWS
#define SOFTWARE 302
#define ISSUE 0
#define REVISION 60
#define BOOTREV  4

//#define DEBUG

#define xstr(s) str(s)
#define str(s) #s
#define MODULE_NAME xstr( HARDWARE ) xstr( SOFTWARE )
#define BOOTMODE_SET   "RUNBOOT"
#define BOOTMODE_CLEAR "<xXvXx>"

#define VECTOROFFSET_BOOT 0x00000000
#define VECTOROFFSET_MAIN 0x00004000

#define FLASHSIZE 0x080000 // 512k

#define BLOCK_SIZE 128

#define BLANKDWORD 0xffffffff
//#define IRamNV(var) var __attribute__ ((section(".IRamNV")))
//#define IRamNV __attribute__ ((section(".IRamNV")))
//#define EXTRAM __attribute__ ((section (".mb1data")))
#define RAMFUNC __attribute__ ((long_call, section (".ramsection")))

#define TIMER_CLOCK_FREQUENCY  60000000 // 60MHz

/*
 GWS302rev03 - 16122016

 - input pulse counting for IP4 improved and work for all the ranges
 - added a pulse counter for IP4 as a diagnostics displayed on main screen
 - start-up screen now shows version as GWS302-000revXX
 - start-up screen shows the text ‘Mk3 G-Wiz’(same as F5500 software)
 - there is no more delay on analogue inputs
 - output diagnostics page now functions correctly

*/



/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/
/*
" ÀÁÂÃÄÅÆABCÇDEÈÉÊËFGHIÌÍÎÏÐJKLMNÑOÒÓÔÕÖØPQRSTUÙÚÛÜVWXYÝßZÞ";
" aàáâãäåæbcçdeèéêëfghiìíîïðjklmnñoòóôõöøpqrstuùúûüvwxyýÿzþ";
"0123456789.+-±!#¢£$";

"!\"#$%&'()*+,-./:;<=>?@[\\]^_`"
"{|}~ ¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿"
"0123456789"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß"
"àáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ"
*/
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
RDS CAN Module Identifier 11bit 83Kbs

11
1098 7654 3210 BIT No
|||| |||| ||||
0000 0000 0001 - TX bit - set by responding module module
0000 0111 1000 - Module Number - upto 16 allowed, 4 supported
nnnn nxxx xnnx - 7 bits for module type, 4 for module number, 1 for TX/RX
0000 1000 0100 – 084 MCM Motor Control Module
0010 0000 0100 – 204 ACM Actuator Control Module
0001 0000 0100 – 104 DIP Digital Input Module
0001 1000 0100 – 184 DOP Digital Output Module
0010 1000 0100 – 284 HBM H-Bridge Module
0110 0000 0100 – 604 WCM Weigh Cell Module - Sulky
0100 0000 0100 – 404 LMM Loadmaster Module
0011 0000 0100 – 304 IOM Input Output Module - McHale
0011 1000 0100 – 384 IOC Input Output Module - TypeC - Evrard
0011 1000 0010 – 382 IOB Input Output Module - TypeB - Sulky

The ID has 4 bits reserved for multiple module connection which is the same for all modules…
 ID_MASK  0x078
The ID has 1 bit reserved to identify master/slave
 MS_MASK  0x001

6 bits are used for module ID, so a maximum of 64 module types can be defined using this scheme
No             Mask -    ID
 1 0x0000 0ppp p00m - 0x000 - Head Units 0 = GWZ
 2 0x0000 0ppp p01m - 0x002
 3 0x0000 0ppp p10m - 0x004
 4 0x0000 0ppp p11m - 0x006
 5 0x0000 1ppp p00m - 0x080
 6 0x0000 1ppp p01m - 0x082
 7 0x0000 1ppp p10m - 0x084 - MCM
 8 0x0000 1ppp p11m - 0x086
 9 0x0001 0ppp p00m - 0x100
10 0x0001 0ppp p01m - 0x102
11 0x0001 0ppp p10m - 0x104 - DIP
12 0x0001 0ppp p11m - 0x106
13 0x0001 1ppp p00m - 0x180
14 0x0001 1ppp p01m - 0x182
15 0x0001 1ppp p10m - 0x184 - DOP
16 0x0001 1ppp p11m - 0x186
17 0x0010 0ppp p00m - 0x200
18 0x0010 0ppp p01m - 0x202
19 0x0010 0ppp p10m - 0x204 - ACM
20 0x0010 0ppp p11m - 0x206
21 0x0010 1ppp p00m - 0x280
22 0x0010 1ppp p01m - 0x282
23 0x0010 1ppp p10m - 0x284 - HBM
24 0x0010 1ppp p11m - 0x286
25 0x0011 0ppp p00m - 0x300
26 0x0011 0ppp p01m - 0x302
27 0x0011 0ppp p10m - 0x304 - IOM
28 0x0011 0ppp p11m - 0x306
29 0x0011 1ppp p00m - 0x380
30 0x0011 1ppp p01m - 0x382
31 0x0011 1ppp p10m - 0x384 - IOC
32 0x0011 1ppp p11m - 0x386
33 0x0100 0ppp p00m - 0x400
34 0x0100 0ppp p01m - 0x402
35 0x0100 0ppp p10m - 0x404
36 0x0100 0ppp p11m - 0x406
37 0x0100 1ppp p00m - 0x480
38 0x0100 1ppp p01m - 0x482
39 0x0100 1ppp p10m - 0x484
40 0x0100 1ppp p11m - 0x486
41 0x0101 0ppp p00m - 0x500
42 0x0101 0ppp p01m - 0x502
43 0x0101 0ppp p10m - 0x504
44 0x0101 0ppp p11m - 0x506
45 0x0101 1ppp p00m - 0x580
46 0x0101 1ppp p01m - 0x582
47 0x0101 1ppp p10m - 0x584
48 0x0101 1ppp p11m - 0x586
49 0x0110 0ppp p00m - 0x600
50 0x0110 0ppp p01m - 0x602
51 0x0110 0ppp p10m - 0x604 - WCM
52 0x0110 0ppp p11m - 0x606
53 0x0110 1ppp p00m - 0x680
54 0x0110 1ppp p01m - 0x682
55 0x0110 1ppp p10m - 0x684
56 0x0110 1ppp p11m - 0x686
57 0x0111 0ppp p00m - 0x700
58 0x0111 0ppp p01m - 0x702
59 0x0111 0ppp p10m - 0x704
60 0x0111 0ppp p11m - 0x706
61 0x0111 1ppp p00m - 0x780
62 0x0111 1ppp p01m - 0x782
63 0x0111 1ppp p10m - 0x784
64 0x0111 1ppp p11m - 0x786

p = module group id
m = master slave bit
-----------------------------------------------------------*/
#define CAN_ID_BIT_SHIFT 0
#define CAN_MOD_ID_OFFSET 3
#define CAN_MOD_BIT_SHIFT (CAN_ID_BIT_SHIFT + CAN_MOD_ID_OFFSET)
#define CAN_MIDvA (0x0000007ff << CAN_ID_BIT_SHIFT)
#define CAN_MIDvB 0x01fffffff
#define CAN_IDMASK (~(0x00f << CAN_MOD_BIT_SHIFT) & CAN_MIDvA)
#define CAN_TYPEMASK (~CAN_IDMASK & CAN_MIDvA)
#define CAN_TXID_BIT ((0x001 << CAN_ID_BIT_SHIFT) & CAN_MIDvA)
#define CAN_RXID_GWS ((0x000 << CAN_ID_BIT_SHIFT) & CAN_MIDvA)
#define CAN_TXID_GWS (CAN_RXID_GWS | CAN_TXID_BIT)
#define CAN_RXID_MCM ((0x084 << CAN_ID_BIT_SHIFT) & CAN_MIDvA)
#define CAN_TXID_MCM (CAN_RXID_MCM | CAN_TXID_BIT)
#define CAN_RXID_ACM ((0x204 << CAN_ID_BIT_SHIFT) & CAN_MIDvA)
#define CAN_TXID_ACM (CAN_RXID_ACM | CAN_TXID_BIT)
#define CAN_RXID_DIP ((0x104 << CAN_ID_BIT_SHIFT) & CAN_MIDvA)
#define CAN_TXID_DIP (CAN_RXID_DIP | CAN_TXID_BIT)
#define CAN_RXID_DOP ((0x184 << CAN_ID_BIT_SHIFT) & CAN_MIDvA)
#define CAN_TXID_DOP (CAN_RXID_DOP | CAN_TXID_BIT)
#define CAN_RXID_HBM ((0x284 << CAN_ID_BIT_SHIFT) & CAN_MIDvA)
#define CAN_TXID_HBM (CAN_RXID_HBM | CAN_TXID_BIT)
#define CAN_RXID_WCM ((0x604 << CAN_ID_BIT_SHIFT) & CAN_MIDvA)
#define CAN_TXID_WCM (CAN_RXID_WCM | CAN_TXID_BIT)
#define CAN_RXID_LMM ((0x404 << CAN_ID_BIT_SHIFT) & CAN_MIDvA)
#define CAN_TXID_LMM (CAN_RXID_LMM | CAN_TXID_BIT)
#define CAN_RXID_IOM ((0x304 << CAN_ID_BIT_SHIFT) & CAN_MIDvA)
#define CAN_TXID_IOM (CAN_RXID_IOM | CAN_TXID_BIT)
#define CAN_RXID_IOC ((0x384 << CAN_ID_BIT_SHIFT) & CAN_MIDvA)
#define CAN_TXID_IOC (CAN_RXID_IOC | CAN_TXID_BIT)
#define CAN_RXID_IOB ((0x382 << CAN_ID_BIT_SHIFT) & CAN_MIDvA)
#define CAN_TXID_IOB (CAN_RXID_IOB | CAN_TXID_BIT)
#define CAN_TXID_ALL (CAN_TXID_MCM | CAN_TXID_ACM | CAN_TXID_DIP | CAN_TXID_DOP | CAN_TXID_HBM | CAN_TXID_WCM | CAN_TXID_LMM | CAN_TXID_IOM | CAN_TXID_IOC | CAN_TXID_IOB)
#define CAN_TXID_MASK (~CAN_TXID_ALL & CAN_MIDvA)

enum canModuleTypes {
	CMT_MCM,
	CMT_ACM,
	CMT_DIP,
	CMT_DOP,
	CMT_HBM,
	CMT_WCM,
	CMT_LMM,
	CMT_IOM,
	CMT_IOC,
	CMT_IOB,
	CAN_MODULE_TYPES
	};

// CAN Mailbox Transmit Request
#define TMIDxR_TXRQ  ((uint32_t)0x00000001)

enum canBaudRates {
	CAN_BAUD_RDS,
	CAN_BAUD_ISO,
	CAN_BAUD_MAX
	};

/*-----------------------------------------------------------*/

// variables
/*-----------------------------------------------------------*/
union ShortToChar {
	unsigned short int us;
	signed short int ss;
	unsigned char uc[2];
	signed char sc[2];
	};

union LongToChar {
	unsigned long int ul; // ints are 4 bytes long
	signed long int sl;
	unsigned short int us[2];
	signed short int ss[2];
	unsigned char uc[4];
	signed char sc[4];
	float f;
	};

union CanData {
	unsigned long long ull; // long long ints are 8 bytes long
	unsigned int ul[2]; // long ints are 4 bytes long
	unsigned short int us[4]; // short ints are 2 bytes long
	unsigned char uc[8];
	};

struct CanMessage {
	union LongToChar Id;
	union CanData Data;
	unsigned char IDE; // ID type, standard or extended
	unsigned char RTR; // frame type, data or remote
	unsigned char DLC; // length 0 to 8
	unsigned char FMI; // filter 0 to 255
	unsigned short int TimeStamp;
	};

// 4K bytes available
#define MAXBOOTSTATUS 8
#define MAXBOOTVERSION 20
struct structBackUpSRAM {
	char BootStatus[MAXBOOTSTATUS];
	char BootLoaderVersion[MAXBOOTVERSION];
	unsigned char CanBaud;
	};
#ifndef WINDOWS
 _Static_assert( sizeof( struct structBackUpSRAM ) <= 4096, "Backup SRAM Structure > 4096 bytes, size needs reducing" );
#endif
/*-----------------------------------------------------------*/

#endif /* COMMON_H_ */
