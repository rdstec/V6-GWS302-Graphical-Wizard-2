/*
 * screens.h
 *
 *  Created on: 25 May 2016
 *      Author: cparsons
 */

#ifndef SCREENS_H_
#define SCREENS_H_

// defines
/*-----------------------------------------------------------*/
// G-Wiz Keys - all on GPIOD
#define KEY1 (1 <<  0) // PD00
#define KEY2 (1 <<  1) // PD01
#define KEY3 (1 <<  3) // PD03
#define KEY4 (1 <<  4) // PD04
#define KEY5 (1 <<  7) // PD07
#define KEY6 (1 <<  8) // PD08
#define KEY7 (1 <<  9) // PD09
#define KEY8 (1 << 10) // PD10

#define SW_CU KEY5
#define SW_CL KEY7
#define SW_CR KEY8
#define SW_CD KEY6
#define SW_IL KEY2
#define SW_OL KEY1
#define SW_IR KEY3
#define SW_OR KEY4

#define SW_NEW (1 << 31)
#define SW_REP (1 << 30)

#define SW_MASK (KEY1 | KEY2 | KEY3 | KEY4 | KEY5 | KEY6 | KEY7 | KEY8)

extern unsigned char escapeFromOutputDiag;
/*-----------------------------------------------------------*/

// variables
/*-----------------------------------------------------------*/
/*struct structIOmap {
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
	};*/
/*struct structIOmap const ioKeys[8] = {
	GPIOD, GPIO_Pin_0,  // Key1
	GPIOD, GPIO_Pin_1,  // Key2
	GPIOD, GPIO_Pin_3,  // Key3
	GPIOD, GPIO_Pin_4,  // Key4
	GPIOD, GPIO_Pin_7,  // Key5
	GPIOD, GPIO_Pin_8,  // Key6
	GPIOD, GPIO_Pin_9,  // Key7
	GPIOD, GPIO_Pin_10  // Key8
	};*/
/*-----------------------------------------------------------*/

// function prototypes
/*-----------------------------------------------------------*/
extern void screen00( unsigned int lkey ); // Startup Screen
extern void mainscreen( unsigned int lkey ); // Main/Home Screen
extern void screen100( unsigned int lkey );  // Machine Menu

extern void screen110( unsigned int lkey );  // Machine Menu - Bale Setup Menu
extern void screen111( unsigned int lkey );  // Machine Menu - Machine Setup Menu
extern void screen112( unsigned int lkey );  // Machine Menu - Bale Count Menu
extern void screen113( unsigned int lkey );  // Machine Menu - Operator Setup Menu

extern void screen120( unsigned int lkey );  // Lube Count Reset Screen
extern void screen121( unsigned int lkey );  // Bale Count Reset Screen
#if 0 // Removed -- Dia pot zero value fixed
extern void screen123( unsigned int lkey );  // Dia Pot Zero Reset Screen
#endif
extern void screen124( unsigned int lkey );  // Fill Pot Zero Reset Screen (AKA Bale Shape Indicator Zero)
extern void screen125( unsigned int lkey );  // Net Metres Reset Screen

extern void screen131( unsigned int lkey );  // Operator Setup Menu - Brightness & Contrast Adjust Menu
extern void screen132( unsigned int lkey );  // Operator Setup Menu - Time & Date Adjust Menu
#if 0 // Removed -- Languages not needed with iconized menus
extern void screen133( unsigned int lkey ); // Language Menu
#endif

extern void screen140( unsigned int lkey );  // Technician Menu - PIN entry
extern void screen141( unsigned int lkey );  // Technician Menu
extern void screen142( unsigned int lkey );  // Technician Menu - Baler Full
extern void screen143( unsigned int lkey );  // Technician Menu - Netting Setup
extern void screen144( unsigned int lkey );  // Technician Menu - Diameter Setup
extern void screen145( unsigned int lkey );  // Technician Menu - Density Setup
extern void screen146( unsigned int lkey );  // Technician Menu - Knives Setup
extern void screen147( unsigned int lkey );  // Technician Menu - Reset Defaults
extern void screen148( unsigned int lkey );  // Technician Menu - Formation 2
extern void TwineSetupScreen( unsigned int lkey );

extern void screen150( unsigned int lkey );  // Diagnostics - Digital Inputs
extern void screen152( unsigned int lkey );  // Diagnostics - Analogue Inputs
extern void screen151( unsigned int lkey );  // Diagnostics - Outputs
extern void CANDiagnostics( unsigned int lkey  ); // Diagnostics - HBM CAN module
extern void screen99( unsigned int lkey );


extern void screen710( unsigned int lkey ); // Clock Set



extern void screenPIN( unsigned int lkey ); // PIN check screen
/*-----------------------------------------------------------*/

#endif /* SCREENS_H_ */
