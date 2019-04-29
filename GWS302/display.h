/*
 * display.h
 *
 *  Created on: 19 May 2016
 *      Author: cparsons
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

// Bm_num.c
/*-----------------------------------------------------------*/
// defines

/*-----------------------------------------------------------*/

// variables
/*-----------------------------------------------------------*/
struct NumericFont {
       unsigned char gap;
       unsigned char height;
       const unsigned char *bitmap[16];
       };

extern struct NumericFont const RDSfntX;
extern struct NumericFont const RDSfnt0;
extern struct NumericFont const RDSfnt1;
extern struct NumericFont const RDSfnt2;
extern struct NumericFont const RDSfnt3;
extern struct NumericFont const RDS7x11;
extern struct NumericFont const ComicSansBold12;
extern struct NumericFont const ComicSansBold10;
extern struct NumericFont const ComicSans26;
extern struct NumericFont const ComicSans28;
/*-----------------------------------------------------------*/

// function prototypes
/*-----------------------------------------------------------*/

// bm_f1all.c
/*-----------------------------------------------------------*/
#include "bm_f1all.h"

// defines
#define MAXLANGUAGES 2

// variables
struct fn1 {
       unsigned char spc;
       const unsigned char *f1[256];
       };
extern struct fn1 const fnt1_EUR;
extern struct fn1 const fnt1_CYR;
extern struct fn1 const * const fnt1[MAXLANGUAGES];

// function prototypes
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
// defines
#define mainDISPLAY_TASK_PRIORITY (tskIDLE_PRIORITY + 2) // watchdog task
#define configDISPLAY_STACK_SIZE configSPRINT_STACK_SIZE // uses sprintf

#define DISPLAY_DEFAULT_PANEL_LOADING 0x02// 20nF-56nF
#define DISPLAY_DEFAULT_BIAS_RATIO 0x01   // 0x00 5, 0x01 10, 0x02 11, 0x03 12
#define DISPLAY_DEFAULT_CHARGE_PUMP 0x03
#define DISPLAY_DEFAULT_GRAY 0x00
#define DISPLAY_DEFAULT_CONTRAST 0x20
#define DISPLAY_DEFAULT_DAY_CONTRAST 0x28
#define DISPLAY_DEFAULT_TEMPCOMP 0x00
#define DISPLAY_DEFAULT_PARTIAL 0x02
#define DISPLAY_DEFAULT_GAIN 0x80      // 0x00 0, 0x40 1, 0x80 2, 0xc0 3
#define DISPLAY_DEFAULT_COLUMNS 0x07
#define DISPLAY_DEFAULT_LINE_RATE 0x02    // 0x00 20.0kHz, 0x01 23.2kHz, 0x02 27.2kHz, 0x03 32.0kHz
#define DISPLAY_DEFAULT_GRAY_SCALE 0x00
// start on page 16 (line 32) for PM5718 240x128
#define DISPLAY_DEFAULT_STARTLINE 32
#define DISPLAY_DEFAULT_ENDLINE 159
#define DISPLAY_DEFAULT_COMEND 159
#define DISPLAY_DEFAULT_LINERATE 0x02
#define DISPLAY_DEFAULT_MAPPING 0x00
#define DISPLAY_DEFAULT_RAMCONTROL 0x01
#define DISPLAY_DEFAULT_MAXCOLUMN 199

#define DISPLAY_CONTRAST_MIN 0x00
#define DISPLAY_CONTRAST_MAX 0x3f
#define DISPLAY_DEFAULT_MASK 0xff

#define DISPLAY_DEFAULT_BRIGHTNESS ((PWM_DUTY100 * 80) / 10000) // 0 to 100 in unsigned char
#define DISPLAY_DEFAULT_DAY_BRIGHTNESS ((PWM_DUTY100 * 100) / 10000) // 0 to 100 in unsigned char



#define DISPLAY_OFF 0
#define DISPLAY_ON  1

#define AND    0x0001
#define OR     0x0002
#define INV    0x0004
#define XOR    0x0008
#define EMPTY  0x0010
#define FULL   0x0020
#define CLEAR  0x0040
#define GREY   0x0080
#define INVCLP 0x0100

// display number/test
#define FIXED   0x00
#define EDIT    0x01
#define CENTER  0x02
//#define INV   0x04
#define UNDERLINED 0x08
#define dn_JUST 0x10
#define LEFT  0x00
#define RIGHT 0x10
#define NSIGN 0x20
#define EDSCAPE 0x40
#define UNDERS 0x80
//#define GREY  0x80

#define TS5 5
#define TS10 10
#define TS15 15
#define TSNAME 19
#define TS24 24
#define TSMAX 26

#define FARRIGHT 156

#define DP0 0
#define DP1 1
#define DP2 2
#define DP3 3
#define DP4 4
#define DP5 5
#define DP6 6

#define DW1 6
#define DW2 12
#define DW3 18
#define DW4 24
#define DW5 30
#define DW6 36
#define DW7 42
#define DW8 48

/* number program options */
#define PMODE 0x08
#define PINC   0x08
#define PENT   0x00

#define PTYPE 0x07
#define PFLOAT 4
#define PLONG  3
#define PSHORT 2
#define PCHAR  1
#define PNULL  0x00

/* text program options */
#define EDIT_UPPER   0x01
#define EDIT_LOWER   0x02
#define EDIT_NUMERIC 0x03

#define NEXTWINDOW_NOESCAPE 1
#define NEXTWINDOW_ESCAPE   2

#define KEY_BEEP 48 // 48 mS beep if new key pressed
#define BACK_TO_MAIN_SCR_BEEP   100  // 100 ms beep (x2) on reverting back to the Main Screen (after a period of no is key activity)
#define BEEP_OFF 250 // 250 mS
#define BEEP_ON 250 // 250 mS

// function prototypes
extern void setPixel( unsigned char *buff, unsigned int x, unsigned int y, unsigned int method );
extern void circlePoints( unsigned char *buff, unsigned int cx, unsigned int cy, unsigned int x, unsigned int y, unsigned int method );
extern void circleMidpoint( unsigned char *buff, unsigned int xCenter, unsigned int yCenter, unsigned int radius, unsigned int method );
extern int greatest( int *a, int *b );
extern void draw_line( unsigned char *buff, unsigned int xp1, unsigned int yp1, unsigned int xp2, unsigned int yp2, unsigned int method );
extern void draw_box( unsigned char *buff, unsigned int xp1, unsigned int yp1, unsigned int xp2, unsigned int yp2, unsigned int method );
extern unsigned char write_bitmap( unsigned char *ptd, const unsigned char *pts, unsigned int xpos, unsigned int ypos, unsigned int method );
extern unsigned char display_f1_text( const unsigned char *data, unsigned int max, unsigned int co, unsigned int ro, unsigned int prog );
extern unsigned char display_number( float data, float min, float max, float con,
	unsigned int dp, unsigned int xc, unsigned int yc, unsigned int width, unsigned int just,
	struct NumericFont const *font, void *factor, int ftype );

extern void draw_frame( unsigned char *pt );

#ifndef WINDOWS
 extern void WriteDisplay( unsigned char *pts );
 extern void SPI0Setup( void );
 extern void InitialiseDisplay( void );
 extern void ReInitialiseDisplay( unsigned char mode );
 extern void clip_box( unsigned char xp1, unsigned char yp1, unsigned char xp2, unsigned char yp2 );

 // static void vDisplayTask( void *pvParameters );
 extern void vStartDisplayTask( unsigned portBASE_TYPE uxPriority );
#else
 /*extern void sim_main( int );*/
 /*extern void WriteToScreen( void );*/
#endif
/*-----------------------------------------------------------*/

#endif /* DISPLAY_H_ */
