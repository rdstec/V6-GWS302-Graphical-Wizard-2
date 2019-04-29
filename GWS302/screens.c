/*
 * screens.c
 *
 *  Created on: 12th June 2014
 *      Author: Chris Parsons
 */

#include "global.h"

// global variables
/*-----------------------------------------------------------*/
// constant
  
unsigned char const * const op_mode[3] = { " Off", "  On", " PWM" };
unsigned char KnifePresWasHigh, SecondScreen, keytime;
unsigned char Screen100Page;
unsigned char SelectedBaleStore;
unsigned int IP4PulsesCount;
unsigned char CheckIP4Pulses;
unsigned char DisplayIP4PulseCounter;
/*-----------------------------------------------------------*/

unsigned char TwineAutoManMode;

/*-----------------------------------------------------------*/
void ClockDisplay( unsigned char icon  )
{
	unsigned char xp, yp, tstring[32];

	if( icon == CL0CK_ICON_ON )
	{
		xp = write_bitmap( adm, F1_CLK, 2, 2, OR ) + 3;
		yp = 2;
	}
	else
	{
		xp = 3;
		yp = 3;
	}  

	sprintf( tstring, "%02X:%02X", ClockRam[2], ClockRam[1] );
	display_f1_text( tstring, TS5, xp, yp, FIXED ); // 5 wide

	if( (looptime / 5) & 0x01 )
	{
		draw_box( adm, xp + 12, yp, xp + 13, yp + 6,  CLEAR );
	}

}
/*-----------------------------------------------------------*/

#ifndef WINDOWS
/*-----------------------------------------------------------*/
void Boot_GetRevision( char *ptd )
{
	struct structBackUpSRAM *ptSRAM;

	// enable access
	PWR_BackupAccessCmd( ENABLE );

	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_BKPSRAM, ENABLE );

	// point to backup ram structure
	ptSRAM = (struct structBackUpSRAM *)BKPSRAM_BASE;

	strncpy( ptd, ptSRAM->BootLoaderVersion, MAXBOOTVERSION );

	// terminate bad string
	*(ptd + (MAXBOOTVERSION - 1)) = 0x00;

	// turn access off
	PWR_BackupAccessCmd( DISABLE );
}
/*-----------------------------------------------------------*/
#endif

// Startup Screen
/*-----------------------------------------------------------*/
void screen00( unsigned int lkey )
{
	unsigned char tstring[16];

	if( !startup )
	{
		startup = 1;

		draw_frame( adm );

		write_bitmap( adm, McHale_V6,  33, 22, OR );

		sprintf( tstring, ptText[78], SOFTWARE, ISSUE, REVISION ); // 'GW%03u-%03urev%02u'
		display_f1_text( tstring, TSMAX, 80, 96, FIXED | CENTER );



		display_f1_text( ptText[183], TSMAX, 80, 104, FIXED | CENTER );

		leaf = 1;
		deftime = 49;
		lkey = 0;
	}

	if( lkey & (SW_NEW | SW_REP) )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits
		if( lkey & SW_MASK ) // any key
		{
			deftime = 0;
			main_window = mainscreen;
		}
	}
}
/*-----------------------------------------------------------*/


// -----------------------------------------------------------
// Function Name       : mainscreen
// Description         : Main/Home Screen
// -----------------------------------------------------------
void mainscreen( unsigned int lkey )
{
	static unsigned char roll, roll1, CropFeedSel, i;
	static unsigned char ArrowsCleared;
	static unsigned char ResetLube;
	static unsigned char KnivKeyPressed, FloorKeyPressed;
	static unsigned char KnivKeyPressedAndOutputTurnedOn, FloorKeyPressedAndOutputTurnedOn;
	static unsigned char NetOnKeyPressed;
	static unsigned int TempIntKPV, TempIntTNM, TempIntDPV;
	static float TempFloatSV;
	static unsigned char DrivingArrowsX1BeepDelay;
	static unsigned char DrivingArrowsX2BeepDelay;
	static unsigned char DrivingArrowsX3BeepDelay;
	static unsigned char DrivingArrowsX4BeepDelay;
	static unsigned char BaleTotalFlashedOff;
	struct CAN_HBM *ptMod;
	static unsigned char LastIP5Status, OP5TTimer;

	static unsigned char HBM_IP4_Timeout;
	unsigned int key_status;
	float ArrowStepVoltage;
	unsigned char tstring[32], changed;
	unsigned char TempY;

	float tempFL;
	static unsigned char SoftKey2Pressed;

	ptMod = &HBM[0];

	if( !startup )
	{
		startup = 1;
		draw_frame( adm );
		windowdepth = 0;
		changed = 1;
		SecondScreen = 0;
	}
	else
	{
		changed = 0;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

        #ifdef WINDOWS
        ExtIn |= (1 << 2);
         ExtIn |= (1 << 4);
        #endif

	key_status = lkey & (SW_NEW | SW_REP);
	if( key_status ) // New key pressed or previous key repeated
	{
		KeyOpOff = OUT1 | OUT2 | OUT3 | OUT4 | OUT5 | OUT7 | OUT8;

		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits


		// -----------------------------------------------------------
		// Button 6: Access Machine Menu

		if( lkey == SW_CD )
		{
			main_window = screen100; // Goto Machine Menu
			Screen100Page = 0;
		}


		// -----------------------------------------------------------
		// Button 7: Return to first operating screen

		else if( lkey == SW_CL )
		{
			if( SecondScreen == 1 )
			{
				SecondScreen = 0; // Return to First Screen
			}
			else if((SecondScreen == 0) && (pFac.pf.GW302_NetTwineOption == TO_TWINE) && !(TwineAutoManMode))
				SecondScreen = 2;
			else if((SecondScreen == 3) && (pFac.pf.GW302_NetTwineOption == TO_TWINE) && (TwineAutoManMode))
			{
				SecondScreen = 1;
			}
			else if((SecondScreen == 4) && (pFac.pf.GW302_NetTwineOption == TO_TWINE) && (TwineAutoManMode))
			{
				SecondScreen = 3;
			}
		}


		// -----------------------------------------------------------
		// Button 8: Displays second operating screen

		else if( lkey == SW_CR )
		{
			if( !SecondScreen )
			{
				SecondScreen = 1; // Goto Second Screen
			}
			else if((SecondScreen == 2) && (pFac.pf.GW302_NetTwineOption == TO_TWINE))
				SecondScreen = 0;
			else if((SecondScreen == 1) && (pFac.pf.GW302_NetTwineOption == TO_TWINE) && (TwineAutoManMode) && (pFac.pf.GW302_DiagnosticMode == TO_ON ))
			{
				SecondScreen = 3;
			}
			else if((SecondScreen == 3) && (pFac.pf.GW302_NetTwineOption == TO_TWINE) && (TwineAutoManMode) && (pFac.pf.GW302_DiagnosticMode == TO_ON ))
			{
				SecondScreen = 4;
			}
		}


		// -----------------------------------------------------------
		// Button 4: Floor diverter / Cycle through bale sub-totals A to J / Cancel Reset Lube Counter

		else if( lkey == SW_OR )
		{
			if( !SecondScreen )  // First Screen
			{
				if( ResetLube )
				{
					ResetLube = 0; // Cancel Reset Lube Counter
					changed = 1;
				}
				else // !ResetLube
				{
					KeyOpOn = OP_MASTER_DIV;  // OP1 ON: Master Diverter
#if 0 // Code moved to outside 'New key pressed or previous key repeated' code block
					if( looptime > 3 )
						KeyOpOn |= OP_FLOOR_DIV;  // OP4 ON: Floor Diverter
#endif
					FloorKeyPressed = 1;

					// Disabling chopping knives position Warning
					DisableChoppingKnivesWarnings = 1;
					Kniv1_IPClosed = 0;
				}
			}
			else if(SecondScreen == 1)  // Second Screen
			{
				if( pFac.pf.BaleStore < PF_GW302_BaleStore[PF_MAX] )
				{
					pFac.pf.BaleStore += PF_GW302_BaleStore[PF_STEP];
				}
				else
				{
					pFac.pf.BaleStore = PF_GW302_BaleStore[PF_MIN];
				}
				SavecalDelayOnMainDisplay = 1;
			}
			else
			{
				//no pulses on IP4 then after 1s turn off output
				if(!HBM[0].HBM_Input[3].Frequency && (pFac.pf.GW302_NetTwineOption != TO_NET))
				{
					if(HBM_IP4_Timeout==pFac.pf.GW302_TwineTrackProtection)
					{
						HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF; //Track Motor Forward
						HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_OFF; //Track Motor Forward
						MWarn |= MW_NO_IP4_PULSES;
						OKWarnBox = EXCL;

						// Sounding 1 beep when message first appears
						buzz_count = 1;
						buzz_length = 250;
					}
					else
					{
						HBM_IP4_Timeout++;
						HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_ON; //Track Motor Forward
						HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_ON; //Track Motor Forward
					}
				}
				else
				{
					HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_ON; //Track Motor Forward
					HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_ON; //Track Motor Forward
				}

				// Disabling chopping knives position Warning
				DisableChoppingKnivesWarnings = 1;
				Kniv1_IPClosed = 0;
			}
		}


		// -----------------------------------------------------------
		// Button 1: MAN/AUTO toggle / Density release

		else if( lkey == SW_OL )
		{
			if( !SecondScreen )  // First Screen
			{



				if((pFac.pf.GW302_TwineOption != TO_ON) || (pFac.pf.GW302_NetTwineOption != TO_TWINE))
				{

					if( AutoMan || CHAMBER_CLOSED ) // inhibiting unless in AUTO mode or in MAN mode with chamber closed
					{
						AutoMan = AutoMan? 0:1;
						ResetSequenceFactors( SEQ_ALL );
					}

					//AutoMan = AutoMan? 0:1;
					//ResetSequenceFactors( SEQ_ALL );

				}
				else if((pFac.pf.GW302_TwineOption == TO_ON) && (pFac.pf.GW302_NetTwineOption == TO_TWINE))
				{


					if(TwineAutoManMode || CHAMBER_CLOSED)
					{
						TwineAutoManMode = TwineAutoManMode ? 0:1;
						ResetSequenceFactors( SEQ_ALL );
						Stage = TWINE_START; // reset sequence when changed from MAN->Auto
					}

					//Check if if HBM-IP3 is closed
					if(TwineAutoManMode && HBM[0].HBM_Input[2].Level_Now && (pFac.pf.GW302_NetTwineOption != TO_NET))
					{
						MWarn |= MW_TRACK_POSITION;
						OKWarnBox = EXCL;

						// Sounding 1 beep when message first appears
						buzz_count = 1;
						buzz_length = 250;
					}

					if(AutoTwineCutButtonPressed)
						AutoTwineCutButtonPressed = 0;


				}

			}
			else if(SecondScreen == 1)  // Second Screen
			{
				KeyOpOn |= OP_DENSITY_REL; // OP8 ON: Density Release Solenoid (AKA Density Control)
			}
			else if(SecondScreen == 2)  // Third Screen
			{
				if(TwineAutoManMode || CHAMBER_CLOSED)
				{
					TwineAutoManMode = TwineAutoManMode ? 0:1;

					//Check if if HBM-IP3 is closed
					if(TwineAutoManMode && HBM[0].HBM_Input[2].Level_Now && (pFac.pf.GW302_NetTwineOption != TO_NET))
					{
						MWarn |= MW_TRACK_POSITION;
						OKWarnBox = EXCL;

						// Sounding 1 beep when message first appears
						buzz_count = 1;
						buzz_length = 250;
					}

					if(AutoTwineCutButtonPressed)
						AutoTwineCutButtonPressed = 0;

				}
				if(TwineAutoManMode == 1)
					SecondScreen = 0;
			}
			else if((SecondScreen == 3) || (SecondScreen == 4))
			{
				TwineAutoManMode = TwineAutoManMode ? 0:1;

				//Check if if HBM-IP3 is closed
				if(TwineAutoManMode && HBM[0].HBM_Input[2].Level_Now && (pFac.pf.GW302_NetTwineOption != TO_NET))
				{
					MWarn |= MW_TRACK_POSITION;
					OKWarnBox = EXCL;

					// Sounding 1 beep when message first appears
					buzz_count = 1;
					buzz_length = 250;
				}

				if(AutoTwineCutButtonPressed)
					AutoTwineCutButtonPressed = 0;

				if(TwineAutoManMode == 0)
					SecondScreen = 0;
			}



		}


		// -----------------------------------------------------------
		// Button 2: Net on / Net cut / Escape Chopping Knives warning / Reset Lube Counter

		else if( lkey == SW_IL )
		{
			if( !SecondScreen )  // First Screen
			{
				if( ChoppingKnives1WarningOn ) // Chopping knives position Warning on & button pressed when tab marked ESC
				{
					// Disabling chopping knives position Warning
					DisableChoppingKnivesWarnings = 1;
					Kniv1_IPClosed = 0;
				}
				else if( pRun.pr.LubeCounter <= 0 ) // Lube Count warning shown & button pressed when tab marked RESET
				{
					if( !MWarn && !ResetLube )
					{
						ResetLube = 1; // Goto 'Reset Lube Counter to Lube Alarm Confirm/Cancel' Display -- Message box shown & 2 tabs changed
					}
				}
				else // Button pressed when tab displays Net On icon
				{


					if((pFac.pf.GW302_TwineOption == TO_ON) && (pFac.pf.GW302_NetTwineOption == TO_TWINE))
					{
						if((CHAMBER_CLOSED && TwineAutoManMode  && !(HBM[0].HBM_Input[2].Level_Now)) && ((Stage == TWINE_START ) || (Stage == TWINE_OP5 ) || (Stage == 11 ) || (Stage == TWINE_STAGE_12)) || (Stage == TWINE_STAGE_11) || (Stage == STAGE_NETTING_END) || (Stage == STAGE_UNLOADING)) // The twiner needs to be in home position (HBM-IP3 must be closed)
						{
							KeyOpOn |= OP_ROLL_ENGAGE; // OP5 ON: Roller Engage Solenoid (AKA Pre Net Position)

							SoftKey2Pressed = 1;
						}
					}
					else if( AutoMan )// AUTO mode
					{
						if((pFac.pf.GW302_TwineOption != TO_ON) || (pFac.pf.GW302_NetTwineOption != TO_TWINE))
						{
							switch( Stage )
							{
							case STAGE_START:
							case STAGE_FORMATION:
							case STAGE_FORMATION2:
							case STAGE_CORE:
							case STAGE_MAIN:
								if( CHAMBER_CLOSED ) // only enabled if chamber closed
								{
									if( SequenceInProgress )
									{
										SequenceInProgress = 0;
										AllSeqOpOff();  // All OPs turned off (& remain off while button held down)
									}
									StartAutoNet = 1;         // Freeze while key held down
									ContinueOnKeyRelease = 1; // Continue when key released
								}
								break;

							case STAGE_NETTING_START:
								break;

							case STAGE_NETTING_NET_DELAY:
								if( CHAMBER_CLOSED ) // only enabled if chamber closed
								{
									if( SequenceInProgress )
									{
										SequenceInProgress = 0;
										// Only OP5 is on -- this is to be left on
									}
									StartAutoNet = 1;         // Freeze while key held down
									ContinueOnKeyRelease = 1; // Continue when key released
								}
								break;

							case STAGE_NETTING_NET_ON:
								if( CHAMBER_CLOSED ) // only enabled if chamber closed
								{
									if( SequenceInProgress )
									{
										SequenceInProgress = 0;
										// Only OP2 is on -- this is to be left on
									}
									StartAutoNet = 1;         // Freeze while key held down
									ContinueOnKeyRelease = 1; // Continue when key released
								}
								break;

							case STAGE_NETTING_WAIT_REQ_NET_PULSES:
								if( CHAMBER_CLOSED ) // only enabled if chamber closed
								{
									if( SequenceInProgress )
									{
										SequenceInProgress = 0;
										// OP2 is turned on & kept on while key held down
										KeyOpOn |= OP_NET_ON;   // OP2 ON: Net Clutch (AKA Net On)
									}
									StartAutoNet = 1;         // Freeze while key held down
									ContinueOnKeyRelease = 1; // Continue when key released
								}
								break;
							}
						}
						else
						{

						}

					}
					else if( !AutoMan ) // MAN mode
					{


							if( pFac.pf.GW302_NetSystem == NS_ROLLER )
							{
								if(CHAMBER_CLOSED)
									KeyOpOn = OP_ROLL_ENGAGE; // OP5 ON: Roller Engage Solenoid (AKA Pre Net Position)

								if( looptime >= ( (unsigned char)(pFac.pf.GW302_RollerDelay * 10.0) ) )
								{
									KeyOpOn |= OP_NET_ON; // OP2 ON: Net Clutch (AKA Net On)
								}
							}
							else // 'Net System' set to 'Beak'
							{
								KeyOpOn = OP_NET_ON; // OP2 ON: Net Clutch (AKA Net On)

								if( !NetOnKeyPressed ) // Not previously set
								{
									NetOnKeyPressed = 1; // Set

									BeakEngagePulseCount = 0UL; // Resetting IP4 'Beak Engage' pulse count
								}

								if( BeakEngagePulseCount >= (unsigned long int)pFac.pf.GW302_BeakEngage ) // 'Beak Engage' Pulses counted on IP4
								{
									if(CHAMBER_CLOSED)
										KeyOpOn |= OP_ROLL_ENGAGE; // OP5 ON: Roller Engage Solenoid (AKA Pre Net Position)
								}
							}
							stagecomp |= NETFEEDON;    // Set NET FEED ON stage completed
							stagecomp &= ~(NETCUTON | CHAMBOP | CHAMBCLO); // These stages are to be completed next
					}


				}
			}
			else if(SecondScreen == 1)  // Second Screen
			{
				if( ChoppingKnives1WarningOn ) // Chopping knives position Warning on & button pressed when tab marked ESC
				{
					// Disabling chopping knives position Warning
					DisableChoppingKnivesWarnings = 1;
					Kniv1_IPClosed = 0;
				}
				else // Button pressed when tab displays Net Cut icon
				{

					if((pFac.pf.GW302_TwineOption != TO_ON) || (pFac.pf.GW302_NetTwineOption != TO_TWINE))
					{
						RollEngageTurnedOnInManMode = 0; // OP5 OFF: Roller Engage Solenoid (AKA Pre Net Position) -- turned on in MAN mode

						KeyOpOn |= OP_NET_CUT;  // OP7 ON: Net Cut Motor (AKA Net Cut Trip)

						if( stagecomp == NETFEEDON ) // Only set stage completed flag if previous stage has been completed (Net On button pressed)
						{
							stagecomp |= NETCUTON; // Set NET CUT ON stage completed
							stagecomp &= ~(CHAMBOP | CHAMBCLO); // These stages are to be completed next
							ChamberOpenDelayTime = CODT_CHAMBER_OPEN_TIME; // Setting Chamber open delay for the next stage to be completed
						}
					}
					else
					{
						if(TwineAutoManMode)
						{

							AutoTwineCutButtonPressed = 1;
							FwdOvershootPulses = 0;
							Stage = TWINE_START;
						}
					}
				}
			}
			else // Third Screen
			{

				if( ChoppingKnives1WarningOn ) // Chopping knives position Warning on & button pressed when tab marked ESC
				{
					// Disabling chopping knives position Warning
					DisableChoppingKnivesWarnings = 1;
					Kniv1_IPClosed = 0;
				}
				else
				{
					//3 seconds
					if(OP5TTimer>=12)
						HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_ON; //Feed Motor
					else
						OP5TTimer++;

					if(CHAMBER_CLOSED)
					{
						SeqOpOn |= OP_ROLL_ENGAGE; // OP5 ON: Roller Engage Solenoid (AKA Pre Net Position)
						SeqOpOff &= ~OP_ROLL_ENGAGE;
					}
				}

			}
		}


		// -----------------------------------------------------------
		// Button 3: Knife diverter / Reset currently selected bale count / Confirm Reset Lube Counter

		else if( lkey == SW_IR )
		{


			if((MWarn & MW_NO_IP4_PULSES) &&   (key_status & SW_NEW))
			{

					MWarn &=~MW_NO_IP4_PULSES;
					OKWarnBox &= ~ EXCL;

			}
			else if((MWarn & MW_SINGLE_NO_TWINE)&& (key_status & SW_NEW))
			{
				MWarn &=~MW_SINGLE_NO_TWINE;
				OKWarnBox &= ~ EXCL;

				TwineAlarmAck = 1;
			}
			else if((MWarn & MW_DOUBLE_NO_TWINE)&& (key_status & SW_NEW))
			{
				MWarn &=~MW_DOUBLE_NO_TWINE;
				OKWarnBox &= ~ EXCL;
				TwineAlarmAck = 1;

			}
			else
			{
			Stage = TWINE_START;

				if( !SecondScreen )  // First Screen
				{
					if( ResetLube )
					{
						ResetLubeCount(); // Reset Lube Counter to Lube Alarm (in bales) taking account of bales since alarm first appeared
						ResetLube = 0;
						changed = 1;
					}
					else // !ResetLube
					{
						if( pFac.pf.GW302_ChoppingKnivesFitted == CKF_ENABLED ) // 'Chopping Knives Fitted' set to enabled
						{
							KeyOpOn = OP_MASTER_DIV;  // OP1 ON: Master Diverter
	#if 0 // Code moved to outside 'New key pressed or previous key repeated' code block
							if( looptime > 3 )
								KeyOpOn |= OP_KNIV_DIV;  // OP3 ON: Knife Diverter (AKA Knives Diverter)
	#endif
							KnivKeyPressed = 1; // Set

							if( KnifePresVolts >= pFac.pf.GW302_KnifePressure )
							{
								if( !KnifePresWasHigh )
								{
									KeyOpOn &= ~OP_KNIV_DIV;  // OP3 OFF: Knife Diverter (AKA Knives Diverter)
									KnivKeyPressed = 0; // Reset
								}
							}
							else
							{
								KnifePresWasHigh = 0;
							}

							// Disabling chopping knives position Warning
							DisableChoppingKnivesWarnings = 1;
							Kniv1_IPClosed = 0;
						} // end of 'Chopping Knives Fitted' set to enabled
					}
				}
				else if(SecondScreen == 1)  // Second Screen
				{
					if( key_status & SW_NEW )
					{
						keytime = 0;
					}

					keytime++;

					if( !half_second ) // Flashing bale amount off in side panel 2 every half second
					{
						BaleTotalFlashedOff = 1;
					}
					else
					{
						BaleTotalFlashedOff = 0;
					}

					if( keytime >= 12 )
					{
						keytime = 0;
						if( pFac.pf.BaleTotal[pFac.pf.BaleStore] != PF_GW302_BaleTotal[PF_MIN] )
						{
							pFac.pf.BaleTotal[pFac.pf.BaleStore] = PF_GW302_BaleTotal[PF_MIN]; // 0UL
							SavecalDelayOnMainDisplay = 1;
							buzz_length = 500;
						}
					}
				}
				else
				{
					//no pulses on IP4 then after 1s turn off output
					if(!HBM[0].HBM_Input[3].Frequency && (pFac.pf.GW302_NetTwineOption != TO_NET))
					{
						if(HBM_IP4_Timeout==pFac.pf.GW302_TwineTrackProtection)
						{
							HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF; //Track Motor Reverse
							MWarn |= MW_NO_IP4_PULSES;
							OKWarnBox = EXCL;

							// Sounding 1 beep when message first appears
							buzz_count = 1;
							buzz_length = 250;
						}
						else
						{
							HBM_IP4_Timeout++;
							HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_ON; //Track Motor Reverse
						}
					}
					else
					{
						HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_ON; //Track Motor Reverse
					}

					// Disabling chopping knives position Warning
					DisableChoppingKnivesWarnings = 1;
					Kniv1_IPClosed = 0;
				}
			} // end else if( lkey == SW_IR )
		}



	} // end if( key_status ) -- New key pressed or previous key repeated


	// -----------------------------------------------------------
	// Key released

	if( !key )
	{

		// -----------------------------------------------------------
		// After key released: Outputs

		KeyOpOff = KeyOpOn; // switch off OPs that were on
		KeyOpOn = 0UL;

		//Reset all HBM CAN outputs if key is released
		if((pFac.pf.GW302_NetTwineOption == TO_TWINE) && (!TwineAutoManMode))
		{

			HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_OFF; //Feed Motor
			HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF; //Track Motor Forward
			HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF; //Track Motor Reverse
			HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_OFF; //Track Motor Forward
			HBM_IP4_Timeout = 0;
			OP5TTimer = 0;

			SeqOpOn &= ~OP_ROLL_ENGAGE; // OP5 OFF: Roller Engage Solenoid (AKA Pre Net Position)
			SeqOpOff |= OP_ROLL_ENGAGE;

		}




		// -----------------------------------------------------------
		// After key released: Saving Programmable Factors on releasing the keys for greater than 300 ms

		if( SavecalDelayOnMainDisplay || SavecalDelayWithinMenus )
		{
			if( swrel > 3 ) // > 3 x 100 ms
			{
				SavecalDelayOnMainDisplay = 0;
				SavecalDelayWithinMenus = 0;
				savecal = 1;
			}
		}


		// -----------------------------------------------------------
		// After Button 2 (Net On) released

		if( ContinueOnKeyRelease )
		{
			ContinueOnKeyRelease = 0;

			StartAutoNet = 0; // Continue
		}


		// -----------------------------------------------------------
		// After Button 2 (Net on) released

		// Used when 'Net System' set to 'Beak', to turn Roller Engage Solenoid (OP5) on, after counting 'Beak Engage' Pulses on IP4
		NetOnKeyPressed = 0; // Reset as soon as key released


		// -----------------------------------------------------------
		// After Button 3 (Knife diverter) released

		// Chopping knives position Warning
		DisableChoppingKnivesWarnings = 0; // keeps the warning on the screen after IP2 or IP6 closes

		KnivKeyPressed = 0; // reset as soon as key released
		if( KnivKeyPressedAndOutputTurnedOn ) // if key (now released) was pressed for more than Diverter Delay (to turn OP on)
		{
			KeyOpOn |= OP_MASTER_DIV;
			if( swrel > 4 ) // fixed period of 0.5s
			{
				KnivKeyPressedAndOutputTurnedOn = 0; // reset after fixed period to turn Master OP off
			}
		}



		// -----------------------------------------------------------
		// After Button 3 (Reset currently selected bale count) released


		// Ensuring bale amount appears in side panel 2
		BaleTotalFlashedOff = 0;


		// -----------------------------------------------------------
		// After Button 4 (Floor diverter) released

		FloorKeyPressed = 0; // reset as soon as key released
		if( FloorKeyPressedAndOutputTurnedOn ) // if key (now released) was pressed for more than Diverter Delay (to turn OP on)
		{
			KeyOpOn |= OP_MASTER_DIV;
			if( swrel > 4 ) // fixed period of 0.5s
			{
				FloorKeyPressedAndOutputTurnedOn = 0; // reset after fixed period to turn Master OP off
			}
		}


		// -----------------------------------------------------------
		// After key released: Knife diverter

		if( KnifePresVolts >= pFac.pf.GW302_KnifePressure )
		{
			KnifePresWasHigh = 1;
		}
		else
		{
			KnifePresWasHigh = 0;
		}

	} // end if( !key )


	// After Soft Key 2 is released in Auto Twiner Mode
	if(SoftKey2Pressed && !key)
	{
		SoftKey2Pressed = 0;
		TwineSeqStart = 1;
		//start 3 sedonds timer
		Stage = TWINE_START;

		if(CHAMBER_CLOSED)
		{
			SeqOpOn |= OP_ROLL_ENGAGE; // OP5 ON: Roller Engage Solenoid (AKA Pre Net Position)
			SeqOpOff &= ~OP_ROLL_ENGAGE;
			KeyOpOn |= OP_ROLL_ENGAGE; // OP5 ON: Roller Engage Solenoid (AKA Pre Net Position)
		}
	}


	// -----------------------------------------------------------
	// Turn on OP3 if key was pressed for more than Diverter Delay

	if( KnivKeyPressed && ( looptime >= ( (unsigned char)(pFac.pf.GW302_DiverterDelay * 10.0) ) ) )
	{
		KeyOpOn |= OP_KNIV_DIV; // OP3 ON: Knife Diverter (AKA Knives Diverter)
		KnivKeyPressedAndOutputTurnedOn = 1;
	}


	// -----------------------------------------------------------
	// Turn on OP4 if key was pressed for more than Diverter Delay

	if( FloorKeyPressed && ( looptime >= ( (unsigned char)(pFac.pf.GW302_DiverterDelay * 10.0) ) ) )
	{
		KeyOpOn |= OP_FLOOR_DIV; // OP4 ON: Floor Diverter
		FloorKeyPressedAndOutputTurnedOn = 1;
	}


	if((MWarn & MW_TRACK_POSITION)&& (!HBM[0].HBM_Input[2].Level_Now))
	{
		MWarn &=~MW_TRACK_POSITION;
		OKWarnBox &= ~  EXCL;
	}


	// -----------------------------------------------------------
	// Outputs

	KeyOpOff &= ~KeyOpOn; // do not switch OFF the ops that are to turned ON


	// -----------------------------------------------------------
	// Instrument display


	// -----------------------------------------------------------
	// Display operational mode (MAN/AUTO)

	draw_box( adm, 113, 1, 158, 4+9, CLEAR );  // Clearing area on right & top
	if((pFac.pf.GW302_TwineOption != TO_ON) || (pFac.pf.GW302_NetTwineOption != TO_TWINE))
		write_bitmap( adm, AutoMan? MODEAUTO:MODEMAN, 113, 4, OR );
	else if((pFac.pf.GW302_TwineOption == TO_ON) && (pFac.pf.GW302_NetTwineOption == TO_TWINE))
		write_bitmap( adm, TwineAutoManMode? MODEAUTO:MODEMAN, 113, 4, OR );



	draw_box( adm, 2, 92, 20, 106, CLEAR );  // Clearing area on right & top
	//sprintf( tstring, "%d", HBM[0].HBM_Input[3].Pulse_Accumulator );
	//display_f1_text( tstring, TSMAX, 3, 100, FIXED );


	//display
	if((pFac.pf.GW302_TwineOption == TO_ON) && (pFac.pf.GW302_NetTwineOption == TO_TWINE))
	{
		//display_f1_text( "1", TSMAX, 3, 10, FIXED );
		//display_f1_text( "2", TSMAX, 3, 47, FIXED );
	}

	// -----------------------------------------------------------
	// Display (Bale) Density Pressure (AnIP2) -- displayed here when on second screen & if Lube Count Alarm is NOT present
	//                                            displayed later when Density Pressure Too High Warning present

	// Calculated when on second screen & if Lube Count Alarm is NOT present OR Density Pressure Too High Warning present
	if( ((SecondScreen == 1) && (pRun.pr.LubeCounter > 0)) ||      // ( Second Screen AND if Lube Count Alarm is NOT present ) OR
			(MWarn & MW_DENSITYPRESS)                       )   // Density Pressure Too High Warning present
	{
		if( !(looptime % 5) )
		{
			TempIntDPV = (unsigned int)(DensityPresVolts * 50.0); // 1 V equivalent to 50 bar
		}
	}

	// Clearing part of Lube Count Alarm (oil can icon) covering bale density pressure
	draw_box( adm, 3, 13, 35, 23, CLEAR ); // Clearing -- needs to be done for both Screen 1 & 2

	// Displaying Bale Density Pressure when on second screen & if Lube Count Alarm is NOT present
	if( (SecondScreen == 1) && (pRun.pr.LubeCounter > 0) ) // Second screen & Lube Count Alarm is NOT present
	{
		sprintf( tstring, "%3u", TempIntDPV );
		display_f1_text( tstring, TSMAX, 3, 9+6, FIXED );
		write_bitmap( adm, BAR_UNIT,  3+(6*3)+2, 9+6, OR );
	}



	//sprintf( tstring, "%d", HBM[0].HBM_Input[3].Pulse_Accumulator );
	//display_f1_text( tstring, TSMAX, 7, 13, FIXED );


	// -----------------------------------------------------------
	// Display info boxes: top, middle & bottom

	draw_box( adm, 129, 18, (129+(28-1)), 106, CLEAR ); // Clearing info boxes area

	if( !SecondScreen )  // First Screen
	{
		if(pFac.pf.GW302_NetTwineOption == TO_TWINE)
		{
			display_number( (float)pFac.pf.GW302_NetLayers[pFac.pf.GW302_BaleProfile],
							(float)PF_GW302_NetLayers[PF_MIN], (float)PF_GW302_NetLayers[PF_MAX],
							1.0, DP1, 150, 35, DW3, RIGHT, &RDSfnt1, NULL, PNULL );

			draw_box( adm, 132, 30, (129+(26)), 55, CLEAR ); // Clearing info boxes area
			write_bitmap( adm, SIDEDISPLAY_TWINE,  129, 18, OR );

		}
		else
			write_bitmap( adm, SIDEDISPLAY_1,  129, 18, OR ); // Info box icons: Net layers; Bale diameter; Knife pressure


	}
	else if(SecondScreen == 1)  // Second Screen
	{
		write_bitmap( adm, SIDEDISPLAY_2,  129, 18, OR ); // Info box icons: Sub-total bale count; Supply voltage; Bale density
	}
	else if(SecondScreen == 2) // Third Screen
	{
		display_number( (float)pFac.pf.GW302_NetLayers[pFac.pf.GW302_BaleProfile],
									(float)PF_GW302_NetLayers[PF_MIN], (float)PF_GW302_NetLayers[PF_MAX],
									1.0, DP1, 150, 35, DW3, RIGHT, &RDSfnt1, NULL, PNULL );

		draw_box( adm, 132, 30, (129+(26)), 55, CLEAR ); // Clearing info boxes area
		write_bitmap( adm, SIDEDISPLAY_TWINE,  129, 18, OR );
	}
	else if(SecondScreen == 3) // Third Screen
	{
		draw_box( adm, 132, 15, (129+(26)), 120, CLEAR ); // Clearing info boxes area

		tempFL = ((BaleDiaVolts - 1.0) / (DiaTargetVolts-1))*100;

		sprintf( tstring, "%.1f", tempFL );
		display_f1_text( tstring, TSMAX, 129,  20, FIXED );
		tempFL = (pFac.pf.GW302_BaleDiameter[pFac.pf.GW302_BaleProfile] * (tempFL/100));
		sprintf( tstring, "%.0f", tempFL );
		display_f1_text( tstring, TSMAX, 129,  28, FIXED );
		sprintf( tstring, "%d", HBM[0].HBM_Input[3].Pulse_Accumulator );
		display_f1_text( tstring, TSMAX, 129,  36, FIXED );

		sprintf( tstring, "%d", FordwardPulses );
		display_f1_text( tstring, TSMAX, 129,  44, FIXED );

		sprintf( tstring, "%d", HBM[0].HBM_Input[3].Pulse_Accumulator );
		display_f1_text( tstring, TSMAX, 129,  52, FIXED );

		sprintf( tstring, "%d", ReversePulses );
		display_f1_text( tstring, TSMAX, 129,  60, FIXED );

		sprintf( tstring, "%d", HBM[0].HBM_Input[3].Pulse_Accumulator );
		display_f1_text( tstring, TSMAX, 129,  68, FIXED );

		sprintf( tstring, "%d", ReversePulsesBetweenStops );
		display_f1_text( tstring, TSMAX, 129,  76, FIXED );

		sprintf( tstring, "%d", HBM[0].HBM_Input[0].Frequency );
		display_f1_text( tstring, TSMAX, 129,  84, FIXED );

		sprintf( tstring, "%d", HBM[0].HBM_Input[1].Frequency );
		display_f1_text( tstring, TSMAX, 129,  92, FIXED );

	}

	else if(SecondScreen == 4) // Third Screen
		{
			draw_box( adm, 132, 15, (129+(26)), 120, CLEAR ); // Clearing info boxes area

			sprintf( tstring, "%d", HBM[0].HBM_Input[0].Pulse_Accumulator );
			display_f1_text( tstring, TSMAX, 129,  20, FIXED );

			sprintf( tstring, "%d", HBM[0].HBM_Input[1].Pulse_Accumulator );
			display_f1_text( tstring, TSMAX, 129,  28, FIXED );

			sprintf( tstring, "%d", TargetEdgePulses );
			display_f1_text( tstring, TSMAX, 129,  36, FIXED );

			sprintf( tstring, "%d", HBM[0].HBM_Input[0].Pulse_Accumulator );
			display_f1_text( tstring, TSMAX, 129,  44, FIXED );

			sprintf( tstring, "%d", HBM[0].HBM_Input[1].Pulse_Accumulator );
			display_f1_text( tstring, TSMAX, 129,  52, FIXED );

			sprintf( tstring, "%d", TargetCentrePulses );
			display_f1_text( tstring, TSMAX, 129,  60, FIXED );


			sprintf( tstring, "%d", FwdOvershootPulses );
			display_f1_text( tstring, TSMAX, 129,  68, FIXED );

			sprintf( tstring, "%d", tempIP4Pules );
			display_f1_text( tstring, TSMAX, 129,  76, FIXED );



		}
	// Top info box
	if( !SecondScreen )  // First Screen
	{
		// Net layers
		if(!pFac.pf.GW302_NetTwineOption == TO_TWINE)
		{
			display_number( (float)pFac.pf.GW302_NetLayers[pFac.pf.GW302_BaleProfile],
				(float)PF_GW302_NetLayers[PF_MIN], (float)PF_GW302_NetLayers[PF_MAX],
				1.0, DP1, 150, 35, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
		}
	}
	else if(SecondScreen == 1)  // Second Screen
	{
		// Sub-total bale count

		// Bale Total selection (A/B etc. )
		sprintf( tstring, "%1c", Store[pFac.pf.BaleStore]);
		display_f1_text( tstring, TSMAX, 148, 22, FIXED );

		// Bale amount
		if( BaleTotalFlashedOff )
		{
			display_f1_text( " ", TSMAX, 143, 34, FIXED | CENTER ); // Flashing off
		}
		else
		{
			sprintf ( tstring, "%lu", pFac.pf.BaleTotal[pFac.pf.BaleStore] );
			display_f1_text( tstring, TSMAX, 143, 34, FIXED | CENTER );
		}
	}

	// Middle info box
	if( (!SecondScreen) || (SecondScreen == 2)  )  // First or Third Screen
	{
		// Bale diameter
		if( (float)pFac.pf.GW302_BaleDiameter[pFac.pf.GW302_BaleProfile] > 99.9 ) // Value of the form XXX
		{
			display_number( (float)pFac.pf.GW302_BaleDiameter[pFac.pf.GW302_BaleProfile],
					(float)PF_GW302_BaleDiameter[PF_MIN], (float)PF_GW302_BaleDiameter[PF_MAX],
					1.0, DP0, (151-5), 55, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
		}
		else // <= 99.9 (Bale diameter Value of the form XX)
		{
			display_number( (float)pFac.pf.GW302_BaleDiameter[pFac.pf.GW302_BaleProfile],
					(float)PF_GW302_BaleDiameter[PF_MIN], (float)PF_GW302_BaleDiameter[PF_MAX],
					1.0, DP0, (151-5), 55, DW2, RIGHT, &RDSfnt1, NULL, PNULL );
		}
	}
	else if(SecondScreen == 1)  // Second Screen
	{
		// Supply voltage
		if( !(looptime % 5) )
		{
			TempFloatSV = (float)AtoDmux[4].smooth * ATOD_VIN_SCALE;
		}

		// Displaying Supply voltage in range 0.0 - 99.9 V -- if >  99.9 V set to 99.9 V
		if( TempFloatSV <= 99.9 )
		{
                        #ifndef WINDOWS
			display_number( TempFloatSV, 0.0, 0.0, 1.0, DP1, (153-4), (61-1), DW4, RIGHT, &RDSfnt1, NULL, PNULL );
                        #else
                        display_number( 13.5, 0.0, 0.0, 1.0, DP1, (153-4), (61-1), DW4, RIGHT, &RDSfnt1, NULL, PNULL );
                        #endif
		}
		else // > 99.9
		{
			display_number( 99.9, 0.0, 0.0, 1.0, DP1, (153-4), (61-1), DW4, RIGHT, &RDSfnt1, NULL, PNULL );
		}
	}

	// Bottom info box
	if( (!SecondScreen) || (SecondScreen == 2)  )  // First or Third Screen
	{
		// Knife pressure -- only displayed if chopping knives fitted (enabled)
		if( pFac.pf.GW302_ChoppingKnivesFitted == CKF_ENABLED )
		{
			if( !(looptime % 5) )
			{
				TempIntKPV = (unsigned int)(KnifePresVolts * 50.0); // 1 V equivalent to 50 bar
			}
			sprintf( tstring, "%3u", TempIntKPV );
			display_f1_text( tstring, TSMAX, 134, 85, FIXED );

			write_bitmap( adm, BAR_UNIT,  138, 94, OR );
		}
		else // if( pFac.pf.GW302_ChoppingKnivesFitted == CKF_DISABLED )
		{
			// Clearing bottom info area
			draw_box( adm, (129+2), 72, (129+2)+(28-4), 94, CLEAR );
		}
	}
	else if(SecondScreen == 1)  // Second Screen
	{
		// Bale density
		display_number( (float)Bar[pFac.pf.GW302_BaleDensity[pFac.pf.GW302_BaleProfile] - 1],
				(float)PF_GW302_BaleDensity[PF_MIN], (float)PF_GW302_BaleDensity[PF_MAX],
				1.0, DP0, 152, 93, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
	}


	// -----------------------------------------------------------
	// Display tabs

	draw_box( adm, 1, 107, 158, 126, CLEAR ); // clear from second to fourth tab

	if( !SecondScreen )  // First Screen
	{
		// Far left tab


		if((pFac.pf.GW302_TwineOption != TO_ON) || (pFac.pf.GW302_NetTwineOption != TO_TWINE))
			write_bitmap( adm, (AutoMan? TABAUTOMAN:TABMAN),   0, 107, OR );
		else if((pFac.pf.GW302_TwineOption == TO_ON) && (pFac.pf.GW302_NetTwineOption == TO_TWINE))
			write_bitmap( adm, (TwineAutoManMode? TABAUTOMAN:TABMAN),   0, 107, OR );

		// Left tab
		if( !MWarn && (pRun.pr.LubeCounter <= 0) ) // Lube Count Alarm present (when no other warnings are active)
		{
			write_bitmap( adm, TABRESET_LUBECOUNT,  31, 107, OR );
		}
		else
		{
			if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
				write_bitmap( adm, TABNET,    31, 107, OR );
			else
			{
				if(!TwineAutoManMode)
					write_bitmap( adm, TAB,    31, 107, OR );
				else
					write_bitmap( adm, TWINE_MENU_TAB_SP,    31, 107, OR );
			}
		}

		// Centre Navigation tab
		if((pFac.pf.GW302_NetTwineOption == TO_TWINE) && !TwineAutoManMode)
			write_bitmap( adm, TABNAV04,      62, 102, OR );
		else
			write_bitmap( adm, TABNAV01,      62, 102, OR );

		// Right tab
		if( (pFac.pf.GW302_ChoppingKnivesFitted == CKF_ENABLED) && !(MWarn & MW_DOUBLE_NO_TWINE) && !(MWarn & MW_SINGLE_NO_TWINE) && !(
				MWarn & MW_NO_IP4_PULSES)) // 'Chopping Knives Fitted' set to enabled
		{
			write_bitmap( adm, ResetLube? TABTICK:TABKNFDIV, 99, 107, OR );
		}
		else // 'Chopping Knives Fitted' set to disabled
		{
			write_bitmap( adm, ResetLube? TABTICK:TAB,       99, 107, OR );
		}

		// Far right tab
		write_bitmap( adm, ResetLube? TABCROSS:TABDIVERT,   130, 107, OR );
	}
	else if(SecondScreen == 1)  // Second Screen
	{
		write_bitmap( adm, TAB_DENSITY_RELEASE,  0, 107, OR ); // DENSITY RELEASE
		if((!pFac.pf.GW302_NetTwineOption == TO_TWINE) || TwineAutoManMode)
		{
			if((pFac.pf.GW302_NetTwineOption == TO_TWINE) && TwineAutoManMode)
				write_bitmap( adm, TAB_NET_CUT_TWINE,  		31, 107, OR ); // NET CUT
			else
				write_bitmap( adm, TAB_NET_CUT,  		31, 107, OR ); // NET CUT

		}
		else
			write_bitmap( adm, TAB,    31, 107, OR );
		write_bitmap( adm, TABNAV_MENU_DL,      62, 102, OR ); // NAVIGATION: Down Left & showing menu icon
		if(!(MWarn & MW_NO_IP4_PULSES)  && !(MWarn & MW_DOUBLE_NO_TWINE) && !(MWarn & MW_SINGLE_NO_TWINE) && !(MWarn & MW_NO_IP4_PULSES))
		{
			write_bitmap( adm, TABRESET, 			99, 107, OR ); // RESET
		}
		else
		{
			write_bitmap( adm, TAB, 130, 107, OR );
		}
		write_bitmap( adm, TABCYCLE_ATOJ, 	   130, 107, OR ); // A -> J
	}
	else if(SecondScreen == 2) //Third Screen
	{
		// Far left tab
		if((pFac.pf.GW302_TwineOption != TO_ON) || (pFac.pf.GW302_NetTwineOption != TO_TWINE))
			write_bitmap( adm, (AutoMan? TABAUTOMAN:TABMAN),   0, 107, OR );
		else if((pFac.pf.GW302_TwineOption == TO_ON) && (pFac.pf.GW302_NetTwineOption == TO_TWINE))
			write_bitmap( adm, (TwineAutoManMode? TABAUTOMAN:TABMAN),   0, 107, OR );
		write_bitmap( adm, TWINE_MENU_TAB_SP,    31, 107, OR );
		write_bitmap( adm, TABNAV01,      62, 102, OR );

		if(!(MWarn & MW_NO_IP4_PULSES))
		{
			write_bitmap( adm, TWINE_MENU_TAB_ARROW_IN, 99, 107, OR );
			write_bitmap( adm, TWINE_MENU_TAB_ARROW_OUT, 130, 107, OR ); // A -> J
		}
		else
		{
			write_bitmap( adm, TAB, 130, 107, OR );
		}


	}
	else if((SecondScreen == 3) || (SecondScreen == 4))
	{
		write_bitmap( adm, (TwineAutoManMode? TABAUTOMAN:TABMAN),   0, 107, OR );
		write_bitmap( adm, TWINE_MENU_TAB_SP,    31, 107, OR );

		if((SecondScreen == 3) )
			write_bitmap( adm, TABNAV04,      62, 102, OR );
		else
			write_bitmap( adm, TABNAV_MENU_DL,      62, 102, OR ); // NAVIGATION: Down Left & showing menu icon

		write_bitmap( adm, TAB,    99, 107, OR );
		write_bitmap( adm, TAB,    130, 107, OR );
	}

	// -----------------------------------------------------------
	// Show animated crop pickup while in AUTO mode

	// clear crop pickup display
	draw_box( adm, 8, 86, 8+28, 86+6, CLEAR );

	if( AutoMan || TwineAutoManMode)
	{
		CropFeedSel++;
		if( CropFeedSel > 11 )
		{
			CropFeedSel = 0;
		}

		// crop pickup display
		write_bitmap( adm, CROPFEED[CropFeedSel], 8, 86, OR );
	}


	// -----------------------------------------------------------
	// Show total net applied in metres -- if Lube Count Alarm NOT present

	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm

	if( pRun.pr.LubeCounter > 0 ) // Lube Count Alarm NOT present
	{



		if( !(looptime % 5) )
		{
			TempIntTNM = (unsigned int)(pFac.pf.GW302_TotalNetMetres + NetMetresAppliedForCount);
			//pFac.pf.GW302_TotalNetMetres = TempIntTNM;
		}

		// Clearing part of Lube Count Alarm (oil can icon) covering total net applied in metres
		draw_box( adm, 3, 24, 35, 34, CLEAR ); // Clearing


		if(DisplayIP4PulseCounter)
		{




			//Reset Counter
			if(ExtIn & (1 << 4))
			{
				IP4PulsesCount = 0;
			}

			// Displaying total net applied in metres
			sprintf( tstring, "%d", IP4PulsesCount); // Default=0m; Increment=1m; Range=0-9999m
			display_f1_text( tstring, TSMAX, 3, 12+2, FIXED );
		}



		//sprintf( tstring, "%d", Stage); // Default=0m; Increment=1m; Range=0-9999m
		//display_f1_text( tstring, TSMAX, 3, 12+2, FIXED );


		if((pFac.pf.GW302_TwineOption != TO_ON) || (pFac.pf.GW302_NetTwineOption != TO_TWINE))
		{

				draw_box( adm, 3, 10, 20, 14, CLEAR );
				// Displaying total net applied in metres
				sprintf( tstring, "%4u", TempIntTNM ); // Default=0m; Increment=1m; Range=0-9999m
				display_f1_text( tstring, TSMAX, 3, 24+4, FIXED );
				display_f1_text( "m", TSMAX, 3+(6*4)+2, 24+4, FIXED );

		}


	}

	if((pFac.pf.GW302_NetTwineOption == TO_NET))
	{


		if( !SecondScreen )
		{
			if( pFac.pf.GW302_DiagnosticMode == TO_ON )
			{
				sprintf( tstring, "%d", TargetNetPulses );
				display_f1_text( tstring, TSMAX, 14, 11, FIXED );


				sprintf( tstring, "%d", NetPulseCount );
				display_f1_text( tstring, TSMAX, 14, 19, FIXED );
			}
		}

	}


	// -----------------------------------------------------------
	// Display Lube Count Alarm if active -- showing oil can icon

	if( pRun.pr.LubeCounter <= 0 ) // Lube Count Alarm present (irrespective of when no other warnings are active)
	{
		draw_box( adm, 3, 13, 35, 34, CLEAR ); // Clearing area (covering bale density pressure & total net applied in metres)

		write_bitmap( adm, OILCAN, 2+2, 11+2, OR );
	}


	// -----------------------------------------------------------
	// Show net roll: rotating while net is being applied (pulses on IP4); otherwise static

#ifndef WINDOWS

	// Determining New Net pulses
	if( NetPulseCount > NetPulseCountPrev )
	{
		NewNetPulses = NetPulseCount - NetPulseCountPrev;
	}
	NetPulseCountPrev = NetPulseCount; // Previous pulse count saved



	if((pFac.pf.GW302_TwineOption != TO_ON) || (pFac.pf.GW302_NetTwineOption != TO_TWINE))
	{

		if( NetPulseCount && NewNetPulses ) // Pulses have been and are still being received on IP4
		{
			// Decrementing New Net Pulses
			NewNetPulses--;

			// Netting icon (rotating)
			roll++;
			if( roll > 7 )
			{
				roll = 0;
			}
			draw_box( adm, 1, 35+2-2, 6+28, 59+2, CLEAR );  // Clearing area on left & right & bottom
			write_bitmap( adm, ROLLER[roll], 14, 35+2, OR );
		}
		else // No pulses being received on IP4
		{
			// Netting icon (static)
			draw_box( adm, 1, 35+2-2, 6+28, 59+2, CLEAR );  // Clearing area on left & right & bottom
			write_bitmap( adm, ROLLER[roll], 14, 35+2, OR );
		}
	}
	else
	{
		//if there are pulses on HBM-IP1 or HBM-IP2 then top icon spins
		if(HBM[0].HBM_Input[0].Frequency || HBM[0].HBM_Input[1].Frequency)
		{

			if(HBM[0].HBM_Input[0].Frequency)
			{
				roll++;
				if( roll > 7 )
				{
					roll = 0;
				}
				draw_box( adm, 8, 28-6, 6+28, 54-16, CLEAR );  // Clearing area on left & right & bottom
				write_bitmap( adm, ROLLER[roll], 14, 24, OR );
			}
			else if((TwineAutoManMode && ((Stage != TWINE_STAGE_12) && (Stage != TWINE_START)) && !HBM[0].HBM_Input[0].Frequency) || (!TwineAutoManMode && HBM[0].HBM_Input[2].Level_Now))
			{
				draw_box( adm, 8, 28-6, 6+28, 54-6, CLEAR );  // Clearing area on left & right & bottom
				write_bitmap( adm, FILLED_CIRCLE, 14, 24, OR );
			}
			else
			{
				draw_box( adm, 8, 28-6, 6+28, 54-6, CLEAR );  // Clearing area on left & right & bottom
				write_bitmap( adm, EMPTY_CIRCLE, 14, 24, OR );
			}

			if(HBM[0].HBM_Input[1].Frequency)
			{
				roll1++;
				if( roll1 > 7 )
				{
					roll1 = 0;
				}
				draw_box( adm, 8, 28+15, 6+28, 54+13, CLEAR );  // Clearing area on left & right & bottom
				write_bitmap( adm, ROLLER[roll1], 14, 43, OR );
			}
			else if((TwineAutoManMode && ((Stage != TWINE_STAGE_12) && (Stage != TWINE_START)) && !HBM[0].HBM_Input[1].Frequency) || (!TwineAutoManMode && HBM[0].HBM_Input[2].Level_Now))
			{
				draw_box( adm, 8, 28+13, 6+28, 54+13, CLEAR );  // Clearing area on left & right & bottom
				write_bitmap( adm, FILLED_CIRCLE, 14, 43, OR );
			}
			else
			{
				draw_box( adm, 8, 28+13, 6+28, 54+13, CLEAR );  // Clearing area on left & right & bottom
				write_bitmap( adm, EMPTY_CIRCLE, 14, 43, OR );
			}
		}
		else if((TwineAutoManMode && ((Stage != TWINE_STAGE_12) && (Stage != TWINE_START)) && HBM[0].HBM_Input[2].Level_Now) || (!TwineAutoManMode && HBM[0].HBM_Input[2].Level_Now))
		{
			if(!(HBM[0].HBM_Input[0].Frequency))
			{
				draw_box( adm, 8, 28-6, 6+28, 54-6, CLEAR );  // Clearing area on left & right & bottom
				write_bitmap( adm, FILLED_CIRCLE, 14, 24, OR );
			}
			if(!(HBM[0].HBM_Input[1].Frequency))
			{
				draw_box( adm, 8, 28+13, 6+28, 54+13, CLEAR );  // Clearing area on left & right & bottom
				write_bitmap( adm, FILLED_CIRCLE, 14, 43, OR );
			}
		}
		else
		{
			draw_box( adm, 8, 28-6, 6+28, 54-6, CLEAR );  // Clearing area on left & right & bottom
			write_bitmap( adm, EMPTY_CIRCLE, 14, 24, OR );

			draw_box( adm, 8, 28+13, 6+28, 54+13, CLEAR );  // Clearing area on left & right & bottom
			write_bitmap( adm, EMPTY_CIRCLE, 14, 43, OR );
		}
	}
#else
	// Netting icon (rotating)
	roll++;
	if( roll > 7 )
	{
		roll = 0;
	}
	draw_box( adm, 1, 35+2-2, 6+28, 59+2, CLEAR );  // Clearing area on left & right & bottom
	write_bitmap( adm, ROLLER[roll], 14, 35+2, OR );
#endif


	// -----------------------------------------------------------
	// Clear warning messages

	// Clearing  'Reset Lube Counter to Lube Alarm Confirm/Cancel' Message box, if present
	if( changed )
	{
		draw_box( adm, 14+6, 40+10, 14+6+123-1, 40+10+32-1, CLEAR );
	}

	// Clear warning messages and bitmaps
	draw_box( adm, 35, 26, 120, 92, CLEAR );


	// -----------------------------------------------------------
	// Draw baler

	// Draw baler
	draw_box( adm, 1, 60, 6+29-1, 60+16-1+2, CLEAR ); // Clear area underneath Baler Hitch (including area to left & underneath)
	write_bitmap( adm, BalerHitch, 6, 60, OR );
	write_bitmap( adm, BalerCenter,  35, 26, OR );


	// -----------------------------------------------------------
	// Show baler door open/closed according to open/closed status of IP3
	// Bale inside chamber displayed where: in AUTO Mode, while unloading; in MAN mode, while Chamber Open Delay expiring

	// Clearing area underneath baler door (incl. bale kicker & part of wheel)
	draw_box( adm, 80, 20, 120, 97, CLEAR );

#ifndef WINDOWS
	if( AutoMan || TwineAutoManMode) // AUTO mode
	{
		if( CHAMBER_OPEN )
		{
			// Show baler door open
			write_bitmap( adm, Balerdooropen_withoutkicker, 80, 20, OR );

			if( Stage == STAGE_UNLOADING )
			{
				if( BaleKickerStatus != (BKS_BALE_KICKER_DOWN | BKS_BALE_KICKER_UP) ) // Bale Kicker has not yet gone down then up
				{
					// Show bale inside chamber
					draw_box( adm, 58, 33, 98, 74, CLEAR );
					write_bitmap( adm, Bale, 58, 33, OR );
				}
			}
		}
		else // !CHAMBER_OPEN
		{
			// Show baler door closed
			write_bitmap( adm, Balerdoorclosed_withoutkicker, 80, 26, OR );
		}
	}
	else  // MAN mode
	{
		if( CHAMBER_OPEN )
		{
			// Show baler door open
			write_bitmap( adm, Balerdooropen_withoutkicker, 80, 20, OR );

			// Show bale inside chamber
			if( ChamberOpenDelayTime && (stagecomp == (NETFEEDON | NETCUTON)) ) // while chamber open delay expiring; if first 2 stages completed
			{
				draw_box( adm, 58, 33, 98, 74, CLEAR );
				write_bitmap( adm, Bale, 58, 33, OR );
			}
		}
		else // !CHAMBER_OPEN
		{
			// Show baler door closed
			write_bitmap( adm, Balerdoorclosed_withoutkicker, 80, 26, OR );
		}
	}
   #else
	// Show baler door closed
      	write_bitmap( adm, Balerdoorclosed_withoutkicker, 80, 26, OR );
   #endif


	// -----------------------------------------------------------
	// Bale Kicker

	if( BALE_KICKER_UP ) // Bale Kicker Sensor closed
	{
		write_bitmap( adm, KICKER_UP, 98, 77, OR );
	}
	else // BALE_KICKER_DOWN -- Bale Kicker Sensor open
	{
		write_bitmap( adm, KICKER_DOWN, 98, 77, OR );
	}


	// -----------------------------------------------------------
	// Display knives

	if( CHOPPING_KNIVES_ON ) // IP2 open: 11 Knife Sensor (AKA Chopping Knives position / Knives Position Sensor 1) in the ‘ON’ position
		write_bitmap( adm, KNIFE, 55, 81, OR );


	// -----------------------------------------------------------
	// Display driving arrows

	if( pFac.pf.GW302_BaleShapeIndicator != BSI_OFF ) // Bale Shape Indicator to be displayed -- with/without beeps
	{
		if( CHAMBER_CLOSED && (stagecomp == SC_CLEAR) )
		{
			// Erase previous Driving arrows
			draw_box( adm, 38, 13, (38+67+6), (13+11), CLEAR );

			// Draw Driving arrows
			ArrowStepVoltage = (pFac.pf.GW302_FillPotRange - pFac.pf.GW302_FillPotIgnore)/3.0;
			ArrowsCleared = 0;
			if( BaleShapeVolts < (pFac.pf.GW302_BaleShapeIndicatorZero - pFac.pf.GW302_FillPotIgnore) )
			{
				if( BaleShapeVolts < (pFac.pf.GW302_BaleShapeIndicatorZero - (pFac.pf.GW302_FillPotIgnore+(3.0*ArrowStepVoltage)) ) )
				{
					if( pFac.pf.GW302_BaleIndicator == BI_FILL)
					{
						write_bitmap( adm, DRIVING_ARROWS_X4_RIGHT, 38+6, 13, OR );
					}
					else // pFac.pf.GW302_BaleIndicator == BI_STEER
					{
						write_bitmap( adm, DRIVING_ARROWS_X4_LEFT, 38, 13, OR );
					}
				}
				else if( BaleShapeVolts < (pFac.pf.GW302_BaleShapeIndicatorZero - (pFac.pf.GW302_FillPotIgnore+(2.0*ArrowStepVoltage)) ) )
				{
					if( pFac.pf.GW302_BaleIndicator == BI_FILL)
					{
						write_bitmap( adm, DRIVING_ARROWS_X3_RIGHT, 38+6, 13, OR );
					}
					else // pFac.pf.GW302_BaleIndicator == BI_STEER
					{
						write_bitmap( adm, DRIVING_ARROWS_X3_LEFT, 38, 13, OR );
					}
				}
				else if( BaleShapeVolts < (pFac.pf.GW302_BaleShapeIndicatorZero - (pFac.pf.GW302_FillPotIgnore+ArrowStepVoltage) ) )
				{
					if( pFac.pf.GW302_BaleIndicator == BI_FILL)
					{
						write_bitmap( adm, DRIVING_ARROWS_X2_RIGHT, 38+6, 13, OR );
					}
					else // pFac.pf.GW302_BaleIndicator == BI_STEER
					{
						write_bitmap( adm, DRIVING_ARROWS_X2_LEFT, 38, 13, OR );
					}
				}
				else
				{
					if( pFac.pf.GW302_BaleIndicator == BI_FILL)
					{
						write_bitmap( adm, DRIVING_ARROWS_X1_RIGHT, 38+6, 13, OR );
					}
					else // pFac.pf.GW302_BaleIndicator == BI_STEER
					{
						write_bitmap( adm, DRIVING_ARROWS_X1_LEFT, 38, 13, OR );
					}
				}
			}
			if( BaleShapeVolts > (pFac.pf.GW302_BaleShapeIndicatorZero + pFac.pf.GW302_FillPotIgnore) )
			{
				if( BaleShapeVolts > (pFac.pf.GW302_BaleShapeIndicatorZero + (pFac.pf.GW302_FillPotIgnore+(3.0*ArrowStepVoltage)) ) )
				{
					if( pFac.pf.GW302_BaleIndicator == BI_FILL)
					{
						write_bitmap( adm, DRIVING_ARROWS_X4_LEFT, 38, 13, OR );
					}
					else // pFac.pf.GW302_BaleIndicator == BI_STEER
					{
						write_bitmap( adm, DRIVING_ARROWS_X4_RIGHT, 38+6, 13, OR );
					}
				}
				else if( BaleShapeVolts > (pFac.pf.GW302_BaleShapeIndicatorZero + (pFac.pf.GW302_FillPotIgnore+(2.0*ArrowStepVoltage)) ) )
				{
					if( pFac.pf.GW302_BaleIndicator == BI_FILL)
					{
						write_bitmap( adm, DRIVING_ARROWS_X3_LEFT, 38, 13, OR );
					}
					else // pFac.pf.GW302_BaleIndicator == BI_STEER
					{
						write_bitmap( adm, DRIVING_ARROWS_X3_RIGHT, 38+6, 13, OR );
					}
				}
				else if( BaleShapeVolts > (pFac.pf.GW302_BaleShapeIndicatorZero + (pFac.pf.GW302_FillPotIgnore+ArrowStepVoltage) ) )
				{
					if( pFac.pf.GW302_BaleIndicator == BI_FILL)
					{
						write_bitmap( adm, DRIVING_ARROWS_X2_LEFT, 38, 13, OR );
					}
					else // pFac.pf.GW302_BaleIndicator == BI_STEER
					{
						write_bitmap( adm, DRIVING_ARROWS_X2_RIGHT, 38+6, 13, OR );
					}
				}
				else
				{
					if( pFac.pf.GW302_BaleIndicator == BI_FILL)
					{
						write_bitmap( adm, DRIVING_ARROWS_X1_LEFT, 38, 13, OR );
					}
					else // pFac.pf.GW302_BaleIndicator == BI_STEER
					{
						write_bitmap( adm, DRIVING_ARROWS_X1_RIGHT, 38+6, 13, OR );
					}
				}
			}

			// Always drawing the central dot in the middle
			{
				write_bitmap( adm, DRIVING_ARROWS_NONE, 38+3, 13, OR ); // drawing just the middle dot
			}
		}
		else // Chamber open or any of the stage completion flags have been set
		{
			if( !ArrowsCleared )
			{
				// Erase driving arrows (and middle dot)
				ArrowsCleared = 1;
				draw_box( adm, 38, 13, (38+67+6), (13+11), CLEAR );
			}
		}
	}


	// -----------------------------------------------------------
	// Sound beeps to accompany driving arrows

	if( pFac.pf.GW302_BaleShapeIndicator == BSI_BEEP ) // Bale Shape Indicator to be displayed with beeps
	{
		if( CHAMBER_CLOSED && (stagecomp == SC_CLEAR) && !DrivingArrowBeepsDisabled )
		{
			if( BaleShapeVolts < (pFac.pf.GW302_BaleShapeIndicatorZero - pFac.pf.GW302_FillPotIgnore) )
			{
				if( BaleShapeVolts < (pFac.pf.GW302_BaleShapeIndicatorZero - (pFac.pf.GW302_FillPotIgnore+(3.0*ArrowStepVoltage)) ) )
				{
					// Beeps to accompany DRIVING_ARROWS_X4_RIGHT / DRIVING_ARROWS_X4_LEFT
					if( DrivingArrowsX4BeepDelay )
					{
						DrivingArrowsX4BeepDelay--;
					}
					else
					{
						buzz_count = 4;
						buzz_length = DRIVING_ARROWS_X4_BEEP; // Beep length (for each of 4 beeps)
						DrivingArrowsX4BeepDelay = DRIVING_ARROWS_X4_BEEP_DELAY; // Delay (before first beep of 4 starts)
					}

					DrivingArrowsX3BeepDelay = DRIVING_ARROWS_X3_BEEP_DELAY; // To delay hearing 3 beeps if driving improves
					DrivingArrowsX2BeepDelay = DRIVING_ARROWS_X2_BEEP_DELAY; // To delay hearing 2 beeps if driving improves
					DrivingArrowsX1BeepDelay = DRIVING_ARROWS_X1_BEEP_DELAY; // To delay hearing 1 beep if driving improves
				}
				else if( BaleShapeVolts < (pFac.pf.GW302_BaleShapeIndicatorZero - (pFac.pf.GW302_FillPotIgnore+(2.0*ArrowStepVoltage)) ) )
				{
					// Beeps to accompany DRIVING_ARROWS_X3_RIGHT / DRIVING_ARROWS_X3_LEFT
					if( DrivingArrowsX3BeepDelay )
					{
						DrivingArrowsX3BeepDelay--;
					}
					else
					{
						buzz_count = 3;
						buzz_length = DRIVING_ARROWS_X3_BEEP; // Beep length (for each of 3 beeps)
						DrivingArrowsX3BeepDelay = DRIVING_ARROWS_X3_BEEP_DELAY; // Delay (before first beep of 3 starts)
					}

					DrivingArrowsX4BeepDelay = 0;  // To hear 4 beeps immediately if driving gets worst
					DrivingArrowsX2BeepDelay = DRIVING_ARROWS_X2_BEEP_DELAY; // To delay hearing 2 beeps if driving improves
					DrivingArrowsX1BeepDelay = DRIVING_ARROWS_X1_BEEP_DELAY; // To delay hearing 1 beep if driving improves
				}
				else if( BaleShapeVolts < (pFac.pf.GW302_BaleShapeIndicatorZero - (pFac.pf.GW302_FillPotIgnore+ArrowStepVoltage) ) )
				{
					// Beeps to accompany DRIVING_ARROWS_X2_RIGHT / DRIVING_ARROWS_X2_LEFT
					if( DrivingArrowsX2BeepDelay )
					{
						DrivingArrowsX2BeepDelay--;
					}
					else
					{
						buzz_count = 2;
						buzz_length = DRIVING_ARROWS_X2_BEEP; // Beep length (for each of 2 beeps)
						DrivingArrowsX2BeepDelay = DRIVING_ARROWS_X2_BEEP_DELAY; // Delay (before first beep of 2 starts)
					}

					DrivingArrowsX4BeepDelay = 0; // To hear 4 beeps immediately if driving gets worst
					DrivingArrowsX3BeepDelay = 0; // To hear 3 beeps immediately if driving gets worst
					DrivingArrowsX1BeepDelay = DRIVING_ARROWS_X1_BEEP_DELAY; // To delay hearing 1 beep if driving improves
				}
				else
				{
					// Beep to accompany DRIVING_ARROWS_X1_RIGHT / DRIVING_ARROWS_X1_LEFT
					if( DrivingArrowsX1BeepDelay )
					{
						DrivingArrowsX1BeepDelay--;
					}
					else
					{
						buzz_count = 1;
						buzz_length = DRIVING_ARROWS_X1_BEEP; // Beep length
						DrivingArrowsX1BeepDelay = DRIVING_ARROWS_X1_BEEP_DELAY; // Delay (before beep starts)
					}

					DrivingArrowsX4BeepDelay = 0; // To hear 4 beeps immediately if driving gets worst
					DrivingArrowsX3BeepDelay = 0; // To hear 3 beeps immediately if driving gets worst
					DrivingArrowsX2BeepDelay = 0; // To hear 2 beeps immediately if driving gets worst
				}
			}
			else if( BaleShapeVolts > (pFac.pf.GW302_BaleShapeIndicatorZero + pFac.pf.GW302_FillPotIgnore) )
			{
				if( BaleShapeVolts > (pFac.pf.GW302_BaleShapeIndicatorZero + (pFac.pf.GW302_FillPotIgnore+(3.0*ArrowStepVoltage)) ) )
				{
					// Beeps to accompany DRIVING_ARROWS_X4_RIGHT / DRIVING_ARROWS_X4_LEFT
					if( DrivingArrowsX4BeepDelay )
					{
						DrivingArrowsX4BeepDelay--;
					}
					else
					{
						buzz_count = 4;
						buzz_length = DRIVING_ARROWS_X4_BEEP; // Beep length (for each of 4 beeps)
						DrivingArrowsX4BeepDelay = DRIVING_ARROWS_X4_BEEP_DELAY; // Delay (before first beep of 4 starts)
					}

					DrivingArrowsX3BeepDelay = DRIVING_ARROWS_X3_BEEP_DELAY; // To delay hearing 3 beeps if driving improves
					DrivingArrowsX2BeepDelay = DRIVING_ARROWS_X2_BEEP_DELAY; // To delay hearing 2 beeps if driving improves
					DrivingArrowsX1BeepDelay = DRIVING_ARROWS_X1_BEEP_DELAY; // To delay hearing 1 beep if driving improves
				}
				else if( BaleShapeVolts > (pFac.pf.GW302_BaleShapeIndicatorZero + (pFac.pf.GW302_FillPotIgnore+(2.0*ArrowStepVoltage)) ) )
				{
					// Beeps to accompany DRIVING_ARROWS_X3_LEFT / DRIVING_ARROWS_X3_RIGHT
					if( DrivingArrowsX3BeepDelay )
					{
						DrivingArrowsX3BeepDelay--;
					}
					else
					{
						buzz_count = 3;
						buzz_length = DRIVING_ARROWS_X3_BEEP; // Beep length (for each of 3 beeps)
						DrivingArrowsX3BeepDelay = DRIVING_ARROWS_X3_BEEP_DELAY; // Delay (before first beep of 3 starts)
					}

					DrivingArrowsX4BeepDelay = 0;  // To hear 4 beeps immediately if driving gets worst
					DrivingArrowsX2BeepDelay = DRIVING_ARROWS_X2_BEEP_DELAY; // To delay hearing 2 beeps if driving improves
					DrivingArrowsX1BeepDelay = DRIVING_ARROWS_X1_BEEP_DELAY; // To delay hearing 1 beep if driving improves
				}
				else if( BaleShapeVolts > (pFac.pf.GW302_BaleShapeIndicatorZero + (pFac.pf.GW302_FillPotIgnore+ArrowStepVoltage) ) )
				{
					// Beeps to accompany DRIVING_ARROWS_X2_LEFT / DRIVING_ARROWS_X2_RIGHT
					if( DrivingArrowsX2BeepDelay )
					{
						DrivingArrowsX2BeepDelay--;
					}
					else
					{
						buzz_count = 2;
						buzz_length = DRIVING_ARROWS_X2_BEEP; // Beep length (for each of 2 beeps)
						DrivingArrowsX2BeepDelay = DRIVING_ARROWS_X2_BEEP_DELAY; // Delay (before first beep of 2 starts)
					}

					DrivingArrowsX4BeepDelay = 0; // To hear 4 beeps immediately if driving gets worst
					DrivingArrowsX3BeepDelay = 0; // To hear 3 beeps immediately if driving gets worst
					DrivingArrowsX1BeepDelay = DRIVING_ARROWS_X1_BEEP_DELAY; // To delay hearing 1 beep if driving improves
				}
				else
				{
					// Beep to accompany DRIVING_ARROWS_X1_LEFT / DRIVING_ARROWS_X1_RIGHT
					if( DrivingArrowsX1BeepDelay )
					{
						DrivingArrowsX1BeepDelay--;
					}
					else
					{
						buzz_count = 1;
						buzz_length = DRIVING_ARROWS_X1_BEEP; // Beep length
						DrivingArrowsX1BeepDelay = DRIVING_ARROWS_X1_BEEP_DELAY; // Delay (before beep starts)
					}

					DrivingArrowsX4BeepDelay = 0; // To hear 4 beeps immediately if driving gets worst
					DrivingArrowsX3BeepDelay = 0; // To hear 3 beeps immediately if driving gets worst
					DrivingArrowsX2BeepDelay = 0; // To hear 2 beeps immediately if driving gets worst
				}
			}
			else
			{
				// Beeping off
				DrivingArrowsX4BeepDelay = 0; // To hear 4 beeps immediately if driving gets worst
				DrivingArrowsX3BeepDelay = 0; // To hear 3 beeps immediately if driving gets worst
				DrivingArrowsX2BeepDelay = 0; // To hear 2 beeps immediately if driving gets worst
				DrivingArrowsX1BeepDelay = 0; // To hear 1 beep immediately if driving gets worst
			}
		}
		else // Chamber open or any of the stage completion flags have been set
		{
			// Beeping off
			DrivingArrowsX4BeepDelay = 0; // To hear 4 beeps immediately (when driving indication starts & if driving gets worst)
			DrivingArrowsX3BeepDelay = 0; // To hear 3 beeps immediately (when driving indication starts & if driving gets worst)
			DrivingArrowsX2BeepDelay = 0; // To hear 2 beeps immediately (when driving indication starts & if driving gets worst)
			DrivingArrowsX1BeepDelay = 0; // To hear 1 beep immediately  (when driving indication starts & if driving gets worst)
		}
	}


	// -----------------------------------------------------------
	// Display Bale Profile (Preset A - E)

	draw_box( adm, 68, 1, (68+13-1), (1+17-1), CLEAR ); // Clearing

	if( !(MWarn & MW_DENSITYPRESS) ) // Bale Profile letter (A - E) not displayed if Density Pressure Too High Warning present
	{
		switch( pFac.pf.GW302_BaleProfile )
		{
		case BP_PRESET_A:
			write_bitmap( adm, ICON_BALE_PROFILE_A, 68, 1, OR );
			break;
		case BP_PRESET_B:
			write_bitmap( adm, ICON_BALE_PROFILE_B, 68, 1, OR );
			break;
		case BP_PRESET_C:
			write_bitmap( adm, ICON_BALE_PROFILE_C, 68, 1, OR );
			break;
		case BP_PRESET_D:
			write_bitmap( adm, ICON_BALE_PROFILE_D, 68, 1, OR );
			break;
		default: /* BP_PRESET_E */
			write_bitmap( adm, ICON_BALE_PROFILE_E, 68, 1, OR );
			break;
		} /* end switch( pFac.pf.GW302_BaleProfile ) */
	}


	// -----------------------------------------------------------
	// Show current bale diameter -- shown on graduated scale with target & minimum bale diameters indicated

	// Erase previous diameter scale and indicator
	draw_box( adm, 112, 18, 128, 106, CLEAR );

	// Draw bale diameter scale -- 0 to 100 % of extended display range from PotZero to DiaMax
	write_bitmap( adm, DIASCALE, (114+3), (18+2), OR );

	// Draw minimum bale diameter indicator -- a line
	if( DiaMinExtDisplayPercent > 0.0 )
	{
		TempY = 100 - (unsigned char)( (DiaMinExtDisplayPercent + 0.5) * 0.8 ); // Percentage rounded up to nearest 1%
		draw_line( adm, 118, TempY, 122, TempY, OR );
	}

	// Draw target bale diameter indicator -- a line & arrow
	if( DiaTargetExtDisplayPercent > 0.0 )
	{
		TempY = (unsigned char)( (DiaTargetExtDisplayPercent + 0.5) * 0.8 ); // Percentage rounded up to nearest 1%
		write_bitmap( adm, DIAMIND, 114, (100-2) - TempY, OR );
	}

	// Show current bale diameter by black filling
	if( BaleDiaExtDisplayPercent > 0.0 )
	{
		TempY = 100 - (unsigned char)( (BaleDiaExtDisplayPercent + 0.5) * 0.8 ); // Percentage rounded up to nearest 1%
		draw_box( adm, 118, TempY, 122, 100, FULL );
	}


	// -----------------------------------------------------------
	// 	Display Stage Completed Indicator -- Only appears when diagnostics are displayed on the screen

#ifdef DEVELOP // Testing Stage Completed (stagecomp) Flags
	DisplayStageCompletedIndicator();
#endif


	// -----------------------------------------------------------
	// Display warnings

	if( MWarn )
	{
		draw_box( adm, 35, 40, 118, 87, CLEAR );


		if( MWarn & MW_DENSITYPRESS ) // Highest prioity
		{
			// Density Pressure Too High Warning

			// Clear area except for tabs
			draw_box( adm, 38, 13, (38+67+6), (13+11), CLEAR ); // Clearing driving arrows
			draw_box( adm, 112, 2, (129+(28-1)), 106, CLEAR );  // Clearing diameter scale and indicator & info boxes area & MAN/AUTO
			draw_box( adm, 1, 10, (129+28), 97, CLEAR );        // Clearing machine picture area -- including area beneath displayed time

			//  Display (Bale) Density Pressure (AnIP2)
			sprintf( tstring, "%3u", TempIntDPV );
			display_f1_text( tstring, TSMAX, 3, 9+6, FIXED );
			write_bitmap( adm, BAR_UNIT,  3+(6*3)+2, 9+6, OR );

			write_bitmap( adm, WARNING_DENSITY_PRESSURE_SCREEN_IMAGE, 2+6, 30, OR );
			if( !half_second ) // Flashing warning symbol every second
			{
				draw_box( adm, 1,   50, (129+28), 97, CLEAR );  // Flashing bottom warning symbol off
				draw_box( adm, 1,   30,       40, 97, CLEAR );  // Flashing left warning symbol off
				draw_box( adm, 120, 30, (129+28), 97, CLEAR );  // Flashing right warning symbol off
			}
		}
		else if( MWarn & MW_CHECKNET )
		{
			// Check Net Error (AKA Net Warning)
			write_bitmap( adm, WARNING_CHECKNET, 35, 40, OR );
		}
		else if( MWarn & MW_DROPFLOOR )
		{
			// Drop Floor (AKA Floor Position) Warning
			write_bitmap( adm, WARNING_DROPFLOOR, 35, 40, OR );
		}
		else if( MWarn & MW_KNIFEPRES )
		{
			// Knife Maximum Pressure Warning
			write_bitmap( adm, WARNING_KNIFEPRES, 35, 40, OR );
		}
		else if( MWarn & MW_CHOPKNIVPOS1 )
		{
			// Chopping Knives Position Warning
			write_bitmap( adm, WARNING_CHOP_KNIV_POS, 35, 40, OR );
			draw_box( adm, 31, 107, 61, 126, CLEAR );
			write_bitmap( adm, TABESC, 31, 107, OR );
		}
		else if( MWarn & MW_LOWBAT ) // Lowest priority
		{
			// Low Battery (AKA Low voltage) Warning
			write_bitmap( adm, WARNING_LOWBATT, 35, 40, OR );
		}
		else if(MWarn & MW_NO_CAN_COM)
		{
			write_bitmap( adm, CAN_OFFLINE, 35, 40, OR );
		}
		else if(MWarn & MW_NO_IP4_PULSES)
		{
			//draw_box( adm, 31, 107, 61, 126, CLEAR );
			write_bitmap( adm, TWINE_PROTECTION_WARNING, 35, 40, OR );
			write_bitmap( adm, TABESC, 99, 107, OR );
		}
		else if(MWarn & MW_TRACK_POSITION)
		{
			//draw_box( adm, 31, 107, 61, 126, CLEAR );
			write_bitmap( adm, TRACK_POSITION_WARNING, 35, 40, OR );

		}
		else if(MWarn & MW_SINGLE_NO_TWINE)
		{
			//draw_box( adm, 31, 107, 61, 126, CLEAR );
			write_bitmap( adm, X1_NO_TWINE, 35, 40, OR );
			write_bitmap( adm, TABESC, 99, 107, OR );
		}
		else if(MWarn & MW_DOUBLE_NO_TWINE)
		{
			//draw_box( adm, 31, 107, 61, 126, CLEAR );
			write_bitmap( adm, X2_NO_TWINE, 35, 40, OR );
			write_bitmap( adm, TABESC, 99, 107, OR );
		}




	} // end if( MWarn )


	// -----------------------------------------------------------
	// Display 'Reset Lube Counter to Lube Alarm Confirm/Cancel' Message box

	if( ResetLube )
	{
		draw_box( adm, 14+6, 40+10, 14+6+123-1, 40+10+32-1, CLEAR );

		// Graphical elements
		write_bitmap( adm, LUBE_COUNT_RESET_PROMPT, 14+6, 40+10, OR );

		// 'Lube Alarm' set in Technician Menu
		sprintf( tstring, "%3u", pFac.pf.GW302_LubeAlarm );
		display_f1_text( tstring, TSMAX, 14+6+17+62-2, 40+10+5+7, FIXED );

	}

}
// -----------------------------------------------------------

// -----------------------------------------------------------
// Function Name       : screen100
// Description         : Machine Menu
// -----------------------------------------------------------

/*  Origin
       x ------------------------------------------------------------
       | (1,1) Bale Setup Menu Icon | (2,1) Machine Setup Menu Icon  |
       | ----------------------------------------------------------- |
       | (1,2) Bale Count Menu Icon | (2,2) Operator Setup Menu Icon |
         ----------------------------------------------------------- */

/* Origin */
#define MACHINE_MENU_ICONS_ORIGIN_X         0
#define MACHINE_MENU_ICONS_ORIGIN_Y         0

/* Grid position (1,1) */
#define ICON_BALE_SETUP_MENU_OFFSET_X      15
#define ICON_BALE_SETUP_MENU_OFFSET_Y      20

/* Grid position (2,1) */
#define ICON_MACHINE_SETUP_MENU_OFFSET_X   78
#define ICON_MACHINE_SETUP_MENU_OFFSET_Y   20

/* Grid position (1,2) */
#define ICON_BALE_COUNT_MENU_OFFSET_X      15
#define ICON_BALE_COUNT_MENU_OFFSET_Y      58

/* Grid position (2,2) */
#define ICON_OPERATOR_SETUP_MENU_OFFSET_X  78
#define ICON_OPERATOR_SETUP_MENU_OFFSET_Y  58


/* Top left icon co-ordinates (x,y) */

#define ICON_BALE_SETUP_MENU_X      MACHINE_MENU_ICONS_ORIGIN_X + ICON_BALE_SETUP_MENU_OFFSET_X
#define ICON_BALE_SETUP_MENU_Y      MACHINE_MENU_ICONS_ORIGIN_Y + ICON_BALE_SETUP_MENU_OFFSET_Y

#define ICON_MACHINE_SETUP_MENU_X   MACHINE_MENU_ICONS_ORIGIN_X + ICON_MACHINE_SETUP_MENU_OFFSET_X
#define ICON_MACHINE_SETUP_MENU_Y   MACHINE_MENU_ICONS_ORIGIN_Y + ICON_MACHINE_SETUP_MENU_OFFSET_Y

#define ICON_BALE_COUNT_MENU_X      MACHINE_MENU_ICONS_ORIGIN_X + ICON_BALE_COUNT_MENU_OFFSET_X
#define ICON_BALE_COUNT_MENU_Y      MACHINE_MENU_ICONS_ORIGIN_Y + ICON_BALE_COUNT_MENU_OFFSET_Y

#define ICON_OPERATOR_SETUP_MENU_X  MACHINE_MENU_ICONS_ORIGIN_X + ICON_OPERATOR_SETUP_MENU_OFFSET_X
#define ICON_OPERATOR_SETUP_MENU_Y  MACHINE_MENU_ICONS_ORIGIN_Y + ICON_OPERATOR_SETUP_MENU_OFFSET_Y


void screen100( unsigned int lkey )
{
	unsigned int key_status;
	unsigned char old_selection;


	if( !startup )
	{
		startup = 1;

		draw_frame( adm );


		/* -----------------------------------------------------------
		 * Drawing icons -- none highlighted */

		write_bitmap( adm, ICON_BALE_SETUP_MENU,      ICON_BALE_SETUP_MENU_X,     ICON_BALE_SETUP_MENU_Y,     OR );
		write_bitmap( adm, ICON_MACHINE_SETUP_MENU,   ICON_MACHINE_SETUP_MENU_X,  ICON_MACHINE_SETUP_MENU_Y,  OR );
		write_bitmap( adm, ICON_BALE_COUNT_MENU,      ICON_BALE_COUNT_MENU_X,     ICON_BALE_COUNT_MENU_Y,     OR );
		write_bitmap( adm, ICON_OPERATOR_SETUP_MENU,  ICON_OPERATOR_SETUP_MENU_X, ICON_OPERATOR_SETUP_MENU_Y, OR );


		/* -----------------------------------------------------------
		 * Drawing tabs -- OL, IL, IR & OR tab */

		write_bitmap( adm, TAB,      0, 107, OR );
		write_bitmap( adm, TAB,     31, 107, OR );

		write_bitmap( adm, TABENT,  99, 107, OR ); // ENTER
		write_bitmap( adm, TABESC, 130, 107, OR ); // ESC


		looptime = 0;

		old_selection = 0xff;
	}
	else
	{
		old_selection = selection;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	/* -----------------------------------------------------------
	 * Key press */

	key_status = lkey & (SW_NEW | SW_REP);

	if( key_status )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_OR ) // outer-right button (softkey ESC)
		{
			escape = 1;
		}
		else if( lkey == SW_IR ) // inner-right button (softkey ENT)
		{
			switch( selection )
			{
			case 0: /* (1,1) */
				main_window = screen110; // Bale Setup Menu
				break;

			case 1: /* (2,1) */
				main_window = screen111; // Machine Setup Menu
				break;

			case 2: /* (1,2) */
				main_window = screen112; // Bale Count Menu
				break;

			case 3: /* (2,2) */
				main_window = screen113; // Operator Setup Menu
				break;
			}
		}
		else if( lkey == SW_CU ) // navigation-up button (softkey ^)
		{
			switch( selection )
			{
			case 0: /* (1,1) */
				break;

			case 1: /* (2,1) */
				selection = 2; /* (1,2) */
				break;

			case 2: /* (1,2) */
				selection = 0; /* (1,1) */
				break;

			case 3: /* (2,2) */
				selection = 1; /* (2,1) */
				break;
			}
		}
		else if( lkey == SW_CD ) // navigation-up button (softkey V)
		{
			switch( selection )
			{
			case 0: /* (1,1) */
				selection = 2; /* (1,2) */
				break;

			case 1: /* (2,1) */
				selection = 3; /* (2,2) */
				break;

			case 2: /* (1,2) */
				selection = 1; /* (2,1) */
				break;

			case 3: /* (2,2) */
				break;
			}
		}
		else if( lkey == SW_CL ) // navigation-up button (softkey <)
		{
			switch( selection )
			{
			case 1: /* (2,1) */
				selection = 0; /* (1,1) */
				break;

			case 2: /* (1,2) */
				selection = 1; /* (2,1) */
				break;

			case 3: /* (2,2) */
				selection = 2; /* (1,2) */
				break;
			}
		}
		else if( lkey == SW_CR ) // navigation-up button (softkey >)
		{
			switch( selection )
			{
			case 0: /* (1,1) */
				selection = 1; /* (2,1) */
				break;

			case 1: /* (2,1) */
				selection = 2; /* (1,2) */
				break;

			case 2: /* (1,2) */
				selection = 3; /* (2,2) */
				break;
			}
		}

	} /* end if( key_status ) */


	/* -----------------------------------------------------------
	 * Selection pointer & Icon highlighting */

	if( selection != old_selection )
	{
		/* Clear old selection */
		switch( old_selection )
		{
		case 0: /* (1,1) */
			write_bitmap( adm, ICON_BALE_SETUP_MENU, ICON_BALE_SETUP_MENU_X, ICON_BALE_SETUP_MENU_Y, AND );
			break;
		case 1: /* (2,1) */
			write_bitmap( adm, ICON_MACHINE_SETUP_MENU, ICON_MACHINE_SETUP_MENU_X, ICON_MACHINE_SETUP_MENU_Y, AND );
			break;
		case 2: /* (1,2) */
			write_bitmap( adm, ICON_BALE_COUNT_MENU, ICON_BALE_COUNT_MENU_X, ICON_BALE_COUNT_MENU_Y, AND );
			break;
		case 3: /* (2,2) */
			write_bitmap( adm, ICON_OPERATOR_SETUP_MENU, ICON_OPERATOR_SETUP_MENU_X, ICON_OPERATOR_SETUP_MENU_Y, AND );
			break;
		}

		/* Draw new selection */
		switch( selection )
		{
		case 0: /* (1,1) */
			write_bitmap( adm, ICON_BALE_SETUP_MENU_SELECTED, ICON_BALE_SETUP_MENU_X, ICON_BALE_SETUP_MENU_Y, OR );
			break;
		case 1: /* (2,1) */
			write_bitmap( adm, ICON_MACHINE_SETUP_MENU_SELECTED, ICON_MACHINE_SETUP_MENU_X, ICON_MACHINE_SETUP_MENU_Y, OR );
			break;
		case 2: /* (1,2) */
			write_bitmap( adm, ICON_BALE_COUNT_MENU_SELECTED, ICON_BALE_COUNT_MENU_X, ICON_BALE_COUNT_MENU_Y, OR );
			break;
		case 3: /* (2,2) */
			write_bitmap( adm, ICON_OPERATOR_SETUP_MENU_SELECTED, ICON_OPERATOR_SETUP_MENU_X, ICON_OPERATOR_SETUP_MENU_Y, OR );
			break;
		}
	} /* end if( selection != old_selection ) */


	/* -----------------------------------------------------------
	 * Drawing tabs -- NAVIGATION tab */

	if( selection != old_selection )
	{
		draw_box( adm, 62, 102, 62+36-1, 102+24-1, CLEAR ); // Clear NAVIGATION tab area

		switch( selection )
		{
		case 0: /* (1,1) */
			write_bitmap( adm, TABNAV_DR, 62, 102, OR ); // NAVIGATION: Down Right
			break;

		case 1: /* (2,1) */
			write_bitmap( adm, TABNAV_UDLR, 62, 102, OR ); // NAVIGATION: Up Down Left Right
			break;

		case 2: /* (1,2) */
			write_bitmap( adm, TABNAV_UDLR, 62, 102, OR ); // NAVIGATION: Up Down Left Right
			break;

		case 3: /* (2,2) */
			write_bitmap( adm, TABNAV_UL, 62, 102, OR ); // NAVIGATION: Up Left
			break;
		}
	} /* end if( selection != old_selection ) */

}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen110
// Description         : Machine Menu - Bale Setup Menu
// -----------------------------------------------------------

/*  Origin
       x ------------------------------------------------------------------
       | (1,1) Bale Profile  | (2,1) Core Diameter  | (3,1) Bale Diameter  |
       | ----------------------------------------------------------------- |
       | (1,2) Core Density             | (2,2) Bale Density               |
       | ----------------------------------------------------------------- |
       | (3,1) Net Layers                                                  |
         ----------------------------------------------------------------- */

/* Origin */
#define BALE_SETUP_MENU_ICONS_ORIGIN_X         0
#define BALE_SETUP_MENU_ICONS_ORIGIN_Y         0

/* Grid positions (1,1) (2,1) & (3,1) */
#define ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_OFFSET_X      10
#define ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_OFFSET_Y      10
/* Grid position (1,1) */
#define ICON_BALE_PROFILE_SETTING_OFFSET_X                          39 - 10 - 3
#define ICON_BALE_PROFILE_SETTING_OFFSET_Y                          26 - 10 - 4
/* Grid position (2,1) */
#define ICON_CORE_DIAMETER_TEXT_OFFSET_X                            77 - 10
#define ICON_CORE_DIAMETER_TEXT_OFFSET_Y                            26 - 10
/* Grid position (3,1) */
#define ICON_BALE_DIAMETER_TEXT_OFFSET_X                            115 - 10
#define ICON_BALE_DIAMETER_TEXT_OFFSET_Y                            26  - 10

/* Grid position (1,2) */
#define ICON_CORE_DENSITY_OFFSET_X        10
#define ICON_CORE_DENSITY_OFFSET_Y        59
#define ICON_CORE_DENSITY_TEXT_OFFSET_X   50 - 10
#define ICON_CORE_DENSITY_TEXT_OFFSET_Y   61 - 59

/* Grid position (2,2) */
#define ICON_BALE_DENSITY_OFFSET_X        81
#define ICON_BALE_DENSITY_OFFSET_Y        59
#define ICON_BALE_DENSITY_TEXT_OFFSET_X   130 - 81
#define ICON_BALE_DENSITY_TEXT_OFFSET_Y   61  - 59

/* Grid position (1,3) */
#define ICON_NET_LAYERS_OFFSET_X          10
#define ICON_NET_LAYERS_OFFSET_Y          86
#define ICON_NET_LAYERS_TEXT_OFFSET_X     51 - 10
#define ICON_NET_LAYERS_TEXT_OFFSET_Y     88 - 86


/* Top left icon co-ordinates (x,y) */

#define ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X   BALE_SETUP_MENU_ICONS_ORIGIN_X + ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_OFFSET_X
#define ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_Y   BALE_SETUP_MENU_ICONS_ORIGIN_Y + ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_OFFSET_Y

#define ICON_CORE_DENSITY_X   BALE_SETUP_MENU_ICONS_ORIGIN_X + ICON_CORE_DENSITY_OFFSET_X
#define ICON_CORE_DENSITY_Y   BALE_SETUP_MENU_ICONS_ORIGIN_Y + ICON_CORE_DENSITY_OFFSET_Y

#define ICON_BALE_DENSITY_X   BALE_SETUP_MENU_ICONS_ORIGIN_X + ICON_BALE_DENSITY_OFFSET_X
#define ICON_BALE_DENSITY_Y   BALE_SETUP_MENU_ICONS_ORIGIN_Y + ICON_BALE_DENSITY_OFFSET_Y

#define ICON_NET_LAYERS_X     BALE_SETUP_MENU_ICONS_ORIGIN_X + ICON_NET_LAYERS_OFFSET_X
#define ICON_NET_LAYERS_Y     BALE_SETUP_MENU_ICONS_ORIGIN_Y + ICON_NET_LAYERS_OFFSET_Y


/* Text/setting top left co-ordinates (x,y) & bottom right co-ordinates (x2,y2) */

#define ICON_BALE_PROFILE_SETTING_X    ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X + ICON_BALE_PROFILE_SETTING_OFFSET_X
#define ICON_BALE_PROFILE_SETTING_Y    ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_Y + ICON_BALE_PROFILE_SETTING_OFFSET_Y
#define ICON_BALE_PROFILE_SETTING_X2   ICON_BALE_PROFILE_SETTING_X + 13 - 1  /* ICON_BALE_PROFILE_A/_B /etc. bitmap X dim - 1 */
#define ICON_BALE_PROFILE_SETTING_Y2   ICON_BALE_PROFILE_SETTING_Y + 17 - 1  /* ICON_BALE_PROFILE_A/_B /etc. bitmap Y dim - 1 */

#define ICON_CORE_DIAMETER_TEXT_X   ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X + ICON_CORE_DIAMETER_TEXT_OFFSET_X
#define ICON_CORE_DIAMETER_TEXT_Y   ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_Y + ICON_CORE_DIAMETER_TEXT_OFFSET_Y

#define ICON_BALE_DIAMETER_TEXT_X   ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X + ICON_BALE_DIAMETER_TEXT_OFFSET_X
#define ICON_BALE_DIAMETER_TEXT_Y   ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_Y + ICON_BALE_DIAMETER_TEXT_OFFSET_Y

#define ICON_CORE_DENSITY_TEXT_X    ICON_CORE_DENSITY_X + ICON_CORE_DENSITY_TEXT_OFFSET_X
#define ICON_CORE_DENSITY_TEXT_Y    ICON_CORE_DENSITY_Y + ICON_CORE_DENSITY_TEXT_OFFSET_Y

#define ICON_BALE_DENSITY_TEXT_X    ICON_BALE_DENSITY_X + ICON_BALE_DENSITY_TEXT_OFFSET_X
#define ICON_BALE_DENSITY_TEXT_Y    ICON_BALE_DENSITY_Y + ICON_BALE_DENSITY_TEXT_OFFSET_Y

#define ICON_NET_LAYERS_TEXT_X      ICON_NET_LAYERS_X + ICON_NET_LAYERS_TEXT_OFFSET_X
#define ICON_NET_LAYERS_TEXT_Y      ICON_NET_LAYERS_Y + ICON_NET_LAYERS_TEXT_OFFSET_Y

unsigned char tempI1, tempi2;

void screen110( unsigned int lkey )
{
	unsigned int  key_status;
	unsigned char old_selection;
	unsigned char tstring[128];
	unsigned char old_page;

	static unsigned char page;
	static unsigned char TryingToSetBaleDiaBelowCore;



	if( startup != 1 )
	{
		startup = 1;

		draw_frame( adm );


		/* -----------------------------------------------------------
		 * Drawing icons -- none highlighted */

		write_bitmap( adm, ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER, pFac.pf.GW302_NetTwineOption ? (ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X - 3) :ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X, ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_Y, OR );
		write_bitmap( adm, ICON_CORE_DENSITY,  pFac.pf.GW302_NetTwineOption ? (ICON_CORE_DENSITY_X - 3) :  ICON_CORE_DENSITY_X,  ICON_CORE_DENSITY_Y,  OR );
		write_bitmap( adm, ICON_BALE_DENSITY,  pFac.pf.GW302_NetTwineOption ? (ICON_BALE_DENSITY_X - 3) :  ICON_BALE_DENSITY_X,  ICON_BALE_DENSITY_Y,  OR );

		if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
			write_bitmap( adm, ICON_NET_LAYERS,     ICON_NET_LAYERS_X,    ICON_NET_LAYERS_Y,    OR );


		/* -----------------------------------------------------------
		 * Drawing tabs -- IR & OR tab */

		write_bitmap( adm, TAB,     99, 107, OR );
		write_bitmap( adm, TABESC, 130, 107, OR ); // ESC


		TryingToSetBaleDiaBelowCore = 0;

		old_selection = 0xff;
		old_page = 0xff;
		page = 0;
		selection = 0;
	}
	else
	{
		old_selection = selection;
		old_page = page;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	/* -----------------------------------------------------------
	 * Key press */

	key_status = lkey & (SW_NEW | SW_REP);

	if( key_status )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_OR ) // outer-right button (softkey ESC)
		{
			escape = 1;
			page = 0;
		}
		else if( lkey == SW_OL ) // outer-left button (softkey -)
		{
			if( page == 0 )
			{
				TryingToSetBaleDiaBelowCore = 0;

				switch( selection )
				{
				case 0: // (1,1) Bale Profile
					if( pFac.pf.GW302_BaleProfile > PF_GW302_BaleProfile[PF_MIN] )
						pFac.pf.GW302_BaleProfile -= PF_GW302_BaleProfile[PF_STEP];
					else
						pFac.pf.GW302_BaleProfile = PF_GW302_BaleProfile[PF_MAX];  // Cycling round presets A-E
					break;

				case 1: // (2,1) Core Diameter
					pFac.pf.GW302_CoreDiameter[pFac.pf.GW302_BaleProfile] -= PF_GW302_CoreDiameter[PF_STEP];
					if( pFac.pf.GW302_CoreDiameter[pFac.pf.GW302_BaleProfile] < PF_GW302_CoreDiameter[PF_MIN] )
						pFac.pf.GW302_CoreDiameter[pFac.pf.GW302_BaleProfile] = PF_GW302_CoreDiameter[PF_MIN];  // Limiting to minimum value
					break;

				case 2: // (3,1) Bale Diameter
					pFac.pf.GW302_BaleDiameter[pFac.pf.GW302_BaleProfile] -= PF_GW302_BaleDiameter[PF_STEP];
					if( pFac.pf.GW302_BaleDiameter[pFac.pf.GW302_BaleProfile] < pFac.pf.GW302_CoreDiameter[pFac.pf.GW302_BaleProfile] )
					{
						pFac.pf.GW302_BaleDiameter[pFac.pf.GW302_BaleProfile] = pFac.pf.GW302_CoreDiameter[pFac.pf.GW302_BaleProfile];  // Preventing target being set below the core diameter
						TryingToSetBaleDiaBelowCore = 1;
					}
					break;

				case 3: // (1,2) Core Density
					pFac.pf.GW302_CoreDensity[pFac.pf.GW302_BaleProfile] -= PF_GW302_CoreDensity[PF_STEP];
					if( pFac.pf.GW302_CoreDensity[pFac.pf.GW302_BaleProfile] < PF_GW302_CoreDensity[PF_MIN] )
						pFac.pf.GW302_CoreDensity[pFac.pf.GW302_BaleProfile] = PF_GW302_CoreDensity[PF_MIN];   // Limiting to minimum value
					break;

				case 4: // (2,2) Bale Density
					pFac.pf.GW302_BaleDensity[pFac.pf.GW302_BaleProfile] -= PF_GW302_BaleDensity[PF_STEP];
					if( pFac.pf.GW302_BaleDensity[pFac.pf.GW302_BaleProfile] < PF_GW302_BaleDensity[PF_MIN] )
						pFac.pf.GW302_BaleDensity[pFac.pf.GW302_BaleProfile] = PF_GW302_BaleDensity[PF_MIN];   // Limiting to minimum value
					break;

				case 5: // (1,3) Net Layers

					pFac.pf.GW302_NetLayers[pFac.pf.GW302_BaleProfile] -= PF_GW302_NetLayers[PF_STEP];
					if( pFac.pf.GW302_NetLayers[pFac.pf.GW302_BaleProfile] < PF_GW302_NetLayers[PF_MIN] )
						pFac.pf.GW302_NetLayers[pFac.pf.GW302_BaleProfile] = PF_GW302_NetLayers[PF_MIN];  // Limiting to minimum value
					break;
				}

			}
			else
			{
				switch(selection)
				{
					case 0: //number of revolutions of twine around the edge of the bale

						pFac.pf.TwineNumberOfRevAroundEdge -= PF_TwineNumberOfRevAroundEdge[PF_STEP];
						if(pFac.pf.TwineNumberOfRevAroundEdge < PF_TwineNumberOfRevAroundEdge[PF_MIN])
							pFac.pf.TwineNumberOfRevAroundEdge = PF_TwineNumberOfRevAroundEdge[PF_MIN];
					break;

					case 1:

						pFac.pf.TwineDistanceFromEdge -= PF_TwineDistanceFromEdge[PF_STEP];


						if(pFac.pf.TwineDistanceFromEdge < (pFac.pf.GW302_TwineMinimumEdgeDistance/10))
						{
							pFac.pf.TwineDistanceFromEdge = (pFac.pf.GW302_TwineMinimumEdgeDistance/10);
						}



					break;

					case 2:
						pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre -= PF_TwineNumberOfStopsBetweenEdgeAndCentre[PF_STEP];
						if(pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre < PF_TwineNumberOfStopsBetweenEdgeAndCentre[PF_MIN])
							pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre = PF_TwineNumberOfStopsBetweenEdgeAndCentre[PF_MIN];

					break;

					case 3:
						pFac.pf.TwineStopTime -= PF_TwineStopTime[PF_STEP];
						if(pFac.pf.TwineStopTime < PF_TwineStopTime[PF_MIN])
							pFac.pf.TwineStopTime = PF_TwineStopTime[PF_MIN];


					break;

					case 4:
						pFac.pf.TwineNumberOfRevolutionsAroundCentre -= PF_TwineNumberOfRevolutionsAroundCentre[PF_STEP];
						if(pFac.pf.TwineNumberOfRevolutionsAroundCentre < PF_TwineNumberOfRevolutionsAroundCentre[PF_MIN])
							pFac.pf.TwineNumberOfRevolutionsAroundCentre = PF_TwineNumberOfRevolutionsAroundCentre[PF_MIN];

					break;


				}
			}
			SavecalDelayWithinMenus = 1;
		}
		else if( lkey == SW_IL ) // inner-left button (softkey +)
		{
			if( page == 0 )
			{
				switch( selection )
				{
				case 0: // (1,1) Bale Profile
					pFac.pf.GW302_BaleProfile += PF_GW302_BaleProfile[PF_STEP];
					if( pFac.pf.GW302_BaleProfile > PF_GW302_BaleProfile[PF_MAX] )
						pFac.pf.GW302_BaleProfile = PF_GW302_BaleProfile[PF_MIN];  // Cycling round presets A-E
					break;

				case 1: // (2,1) Core Diameter
					pFac.pf.GW302_CoreDiameter[pFac.pf.GW302_BaleProfile] += PF_GW302_CoreDiameter[PF_STEP];
					if( pFac.pf.GW302_CoreDiameter[pFac.pf.GW302_BaleProfile] > PF_GW302_CoreDiameter[PF_MAX] )
						pFac.pf.GW302_CoreDiameter[pFac.pf.GW302_BaleProfile] = PF_GW302_CoreDiameter[PF_MAX];  // Limiting to maximum value
					break;

				case 2: // (3,1) Bale Diameter
					pFac.pf.GW302_BaleDiameter[pFac.pf.GW302_BaleProfile] += PF_GW302_BaleDiameter[PF_STEP];
					if( pFac.pf.GW302_BaleDiameter[pFac.pf.GW302_BaleProfile] > PF_GW302_BaleDiameter[PF_MAX] )
						pFac.pf.GW302_BaleDiameter[pFac.pf.GW302_BaleProfile] = PF_GW302_BaleDiameter[PF_MAX];  // Limiting to maximum value
					break;

				case 3: // (1,2) Core Density
					pFac.pf.GW302_CoreDensity[pFac.pf.GW302_BaleProfile] += PF_GW302_CoreDensity[PF_STEP];
					if( pFac.pf.GW302_CoreDensity[pFac.pf.GW302_BaleProfile] > PF_GW302_CoreDensity[PF_MAX] )
						pFac.pf.GW302_CoreDensity[pFac.pf.GW302_BaleProfile] = PF_GW302_CoreDensity[PF_MAX];   // Limiting to maximum value
					break;

				case 4: // (2,2) Bale Density
					pFac.pf.GW302_BaleDensity[pFac.pf.GW302_BaleProfile] += PF_GW302_BaleDensity[PF_STEP];
					if( pFac.pf.GW302_BaleDensity[pFac.pf.GW302_BaleProfile] > PF_GW302_BaleDensity[PF_MAX] )
						pFac.pf.GW302_BaleDensity[pFac.pf.GW302_BaleProfile] = PF_GW302_BaleDensity[PF_MAX];   // Limiting to maximum value
					break;

				case 5: // (1,3) Net Layers
					pFac.pf.GW302_NetLayers[pFac.pf.GW302_BaleProfile] += PF_GW302_NetLayers[PF_STEP];
					if( pFac.pf.GW302_NetLayers[pFac.pf.GW302_BaleProfile] > PF_GW302_NetLayers[PF_MAX] )
						pFac.pf.GW302_NetLayers[pFac.pf.GW302_BaleProfile] = PF_GW302_NetLayers[PF_MAX];  // Limiting to maximum value
					break;
				}

			}
			else
			{
				switch(selection)
				{
					case 0: //number of revolutions of twine around the edge of the bale

						pFac.pf.TwineNumberOfRevAroundEdge += PF_TwineNumberOfRevAroundEdge[PF_STEP];
						if(pFac.pf.TwineNumberOfRevAroundEdge > PF_TwineNumberOfRevAroundEdge[PF_MAX])
							pFac.pf.TwineNumberOfRevAroundEdge = PF_TwineNumberOfRevAroundEdge[PF_MAX];
					break;

					case 1:
						pFac.pf.TwineDistanceFromEdge += PF_TwineDistanceFromEdge[PF_STEP];
						if(pFac.pf.TwineDistanceFromEdge > PF_TwineDistanceFromEdge[PF_MAX])
							pFac.pf.TwineDistanceFromEdge = PF_TwineDistanceFromEdge[PF_MAX];
					break;

					case 2:
						pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre += PF_TwineNumberOfStopsBetweenEdgeAndCentre[PF_STEP];
						if(pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre > PF_TwineNumberOfStopsBetweenEdgeAndCentre[PF_MAX])
							pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre = PF_TwineNumberOfStopsBetweenEdgeAndCentre[PF_MAX];

					break;

					case 3:
						pFac.pf.TwineStopTime += PF_TwineStopTime[PF_STEP];
						if(pFac.pf.TwineStopTime > PF_TwineStopTime[PF_MAX])
							pFac.pf.TwineStopTime = PF_TwineStopTime[PF_MAX];

					break;

					case 4:
						pFac.pf.TwineNumberOfRevolutionsAroundCentre += PF_TwineNumberOfRevolutionsAroundCentre[PF_STEP];
						if(pFac.pf.TwineNumberOfRevolutionsAroundCentre > PF_TwineNumberOfRevolutionsAroundCentre[PF_MAX])
							pFac.pf.TwineNumberOfRevolutionsAroundCentre = PF_TwineNumberOfRevolutionsAroundCentre[PF_MAX];

					break;


				}
			}
			SavecalDelayWithinMenus = 1;
		}
		else if( lkey == SW_CU ) // navigation-up button (softkey ^)
		{
			if( page == 0 )
			{
				switch( selection )
				{
				case 0: /* (1,1) Bale Profile */
					break;
				case 1: /* (2,1) Core Diameter */
					break;
				case 2: /* (3,1) Bale Diameter */
					break;
				case 3: /* (1,2) Core Density */
					selection = 0; /* (1,1) Bale Profile */
					break;
				case 4: /* (2,2) Bale Density */
					selection = 1; /* (2,1) Core Diameter */
					break;
				case 5: /* (1,3) Net Layers */
					selection = 3; /* (1,2) Core Density */
					break;
				}
			}
			else
			{
				switch(selection)
				{
					case 0:
						page = 0;
						selection = 0;
						startup = 0;
					break;

					case 1:
						selection = 0;
					break;

					case 2:
						selection = 1;
					break;

					case 3:
						selection = 2;
					break;

					case 4:
						selection = 3;
					break;

				}

			}
		}
		else if( lkey == SW_CD ) // navigation-up button (softkey V)
		{
			if( page == 0 )
			{
				switch( selection )
				{
				case 0: /* (1,1) Bale Profile */
					selection = 3; /* (1,2) Core Density */
					break;
				case 1: /* (2,1) Core Diameter */
					selection = 4; /* (2,2) Bale Density */
					break;
				case 2: /* (3,1) Bale Diameter */
					selection = 4; /* (2,2) Bale Density */
					break;
				case 3: /* (1,2) Core Density */
					if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
						selection = 5; /* (1,3) Net Layers */
					else
					{
						page = 1;
						selection = 0;
					}
					break;
				case 4: /* (2,2) Bale Density */
					if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
						selection = 5; /* (1,3) Net Layers */
					else
					{
						page = 1;
						selection = 0;
					}
					break;
				case 5: /* (1,3) Net Layers */
					break;
				}
			}
			else
			{
				switch( selection )
				{
				case 0:
					selection = 1;
				break;
				case 1:
					selection = 2;
				break;

				case 2:
					selection = 3;
				break;

				case 3:
					selection = 4;
				break;

				}
			}
		}
		else if( lkey == SW_CL ) // navigation-up button (softkey <)
		{
			if( page == 0 )
			{
				switch( selection )
				{
				case 0: /* (1,1) Bale Profile */
					break;
				case 1: /* (2,1) Core Diameter */
					selection = 0; /* (1,1) Bale Profile */
					break;
				case 2: /* (3,1) Bale Diameter */
					selection = 1; /* (2,1) Core Diameter */
					break;
				case 3: /* (1,2) Core Density */
					selection = 2; /* (3,1) Bale Diameter */
					break;
				case 4: /* (2,2) Bale Density */
					selection = 3; /* (1,2) Core Density */
					break;
				case 5: /* (1,3) Net Layers */
					selection = 4; /* (2,2) Bale Density */
					break;
				}
			}
		}
		else if( lkey == SW_CR ) // navigation-up button (softkey >)
		{
			if( page == 0 )
			{
				switch( selection )
				{
				case 0: /* (1,1) Bale Profile */
					selection = 1; /* (2,1) Core Diameter */
					break;
				case 1: /* (2,1) Core Diameter */
					selection = 2; /* (3,1) Bale Diameter */
					break;
				case 2: /* (3,1) Bale Diameter */
					selection = 3; /* (1,2) Core Density */
					break;
				case 3: /* (1,2) Core Density */
					selection = 4; /* (2,2) Bale Density */
					break;
				case 4: /* (2,2) Bale Density */
					if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
						selection = 5; /* (1,3) Net Layers */
					break;
				case 5: /* (1,3) Net Layers */
					break;
				}
			}
		}
		else // No key press
		{
			TryingToSetBaleDiaBelowCore = 0;
		}

	} /* end if( key_status ) */

	 /* Drawing scroll bar */



	/* -----------------------------------------------------------
	 * Selection pointer & Icon highlighting */
		if( page == 0 )
		{
			if( selection != old_selection )
			{
				/* Clear old selection */
				switch( old_selection )
				{
				case 0: /* (1,1) */
					write_bitmap( adm, ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER, pFac.pf.GW302_NetTwineOption ? (ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X - 3) : ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X, ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_Y, AND );
					draw_line( adm, 159, 0, 159, 127, OR ); /* restoring right-hand frame border */
					break;
				case 1: /* (2,1) */
					write_bitmap( adm, ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER, pFac.pf.GW302_NetTwineOption ? (ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X - 3) : ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X, ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_Y, AND );
					draw_line( adm, 159, 0, 159, 127, OR ); /* restoring right-hand frame border */
					break;
				case 2: /* (3,1) */
					write_bitmap( adm, ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER, pFac.pf.GW302_NetTwineOption ? (ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X - 3) : ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X, ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_Y, AND );
					draw_line( adm, 159, 0, 159, 127, OR ); /* restoring right-hand frame border */
					break;
				case 3: /* (1,2) */
					write_bitmap( adm, ICON_CORE_DENSITY, pFac.pf.GW302_NetTwineOption ? (ICON_CORE_DENSITY_X - 3) : ICON_CORE_DENSITY_X, ICON_CORE_DENSITY_Y, AND );
					break;
				case 4: /* (2,2) */
					write_bitmap( adm, ICON_BALE_DENSITY, pFac.pf.GW302_NetTwineOption ? (ICON_BALE_DENSITY_X - 3) : ICON_BALE_DENSITY_X, ICON_BALE_DENSITY_Y, AND );
					draw_line( adm, 159, 0, 159, 127, OR ); /* restoring right-hand frame border */
					break;
				case 5: /* (1,3) */
					if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
						write_bitmap( adm, ICON_NET_LAYERS, ICON_NET_LAYERS_X, ICON_NET_LAYERS_Y, AND );
					break;
				}

				/* Draw new selection */
				switch( selection )
				{
				case 0: /* (1,1) */
					write_bitmap( adm, ICON_BALE_PROFILE_SELECTED,  pFac.pf.GW302_NetTwineOption ? (ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X - 3) : ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X, ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_Y, OR );
					break;
				case 1: /* (2,1) */
					write_bitmap( adm, ICON_CORE_DIAMETER_SELECTED, pFac.pf.GW302_NetTwineOption ? (ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X - 3) : ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X, ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_Y, OR );
					break;
				case 2: /* (3,1) */
					write_bitmap( adm, ICON_BALE_DIAMETER_SELECTED, pFac.pf.GW302_NetTwineOption ? (ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X - 3) : ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_X, ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER_Y, OR );
					break;
				case 3: /* (1,2) */
					write_bitmap( adm, ICON_CORE_DENSITY_SELECTED, pFac.pf.GW302_NetTwineOption ? (ICON_CORE_DENSITY_X - 3) : ICON_CORE_DENSITY_X, ICON_CORE_DENSITY_Y, OR );
					break;
				case 4: /* (2,2) */
					write_bitmap( adm, ICON_BALE_DENSITY_SELECTED, pFac.pf.GW302_NetTwineOption ? (ICON_BALE_DENSITY_X - 3) : ICON_BALE_DENSITY_X, ICON_BALE_DENSITY_Y, OR );
					break;
				case 5: /* (1,3) */
					if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
						write_bitmap( adm, ICON_NET_LAYERS_SELECTED, ICON_NET_LAYERS_X, ICON_NET_LAYERS_Y, OR );
					break;
				}
			} /* end if( selection != old_selection ) */
		}
		else
		{
			draw_box( adm, 1, 2, 144, 100,   CLEAR );

			write_bitmap( adm, TwineImage1, 37, 20, FIXED );


			switch( selection )
			{
				case 0:
					/*Lines*/
					write_bitmap( adm, Line3px, 37+6, 22, OR );
					write_bitmap( adm, Line3px, 123-7, 22, OR );

					/*Arrows*/
					write_bitmap( adm, ArrowDown, 37+4, 12, OR );
					write_bitmap( adm, ArrowDown, 123-9, 12, OR );

					write_bitmap( adm, ArrowUp, 37+4, 93, OR );
					write_bitmap( adm, ArrowUp, 123-9, 93, OR );



					sprintf( tstring, "%d", pFac.pf.TwineNumberOfRevAroundEdge);
					display_f1_text( tstring, TSMAX, 126, 5, FIXED );
				break;

				case 1:
					/*Lines*/
					write_bitmap( adm, Line3px, 37+6, 22, OR );
					write_bitmap( adm, Line3px, 123-7, 22, OR );

					/*Arrows*/
					write_bitmap( adm, ArrowRight, 30, 12, OR );
					write_bitmap( adm, ArrowLeft, 44, 12, OR );

					write_bitmap( adm, ArrowRight, 109, 12, OR );
					write_bitmap( adm, ArrowLeft, 123, 12, OR );


					sprintf( tstring, "%dcm", pFac.pf.TwineDistanceFromEdge);
					display_f1_text( tstring, TSMAX, 120, 5, FIXED );
				break;

				case 2:
					/*Lines*/
					write_bitmap( adm, Line3px, 37+6 +9, 22, OR );
					write_bitmap( adm, Line3px, 123-7-9, 22, OR );

					write_bitmap( adm, Line3px, 37+6 +17, 22, OR );
					write_bitmap( adm, Line3px, 123-7-17, 22, OR );

					write_bitmap( adm, Line3px, 37+6 +25, 22, OR );
					write_bitmap( adm, Line3px, 123-7-25, 22, OR );




					sprintf( tstring, "x%d", pFac.pf.TwineNumberOfStopsBetweenEdgeAndCentre);
					display_f1_text( tstring, TSMAX, 110, 10, FIXED );
					break;


				case 3:
					/*Lines*/
					write_bitmap( adm, Line3px, 37+6 +9, 22, OR );
					write_bitmap( adm, Line3px, 123-7-9, 22, OR );

					write_bitmap( adm, Line3px, 37+6 +17, 22, OR );
					write_bitmap( adm, Line3px, 123-7-17, 22, OR );

					write_bitmap( adm, Line3px, 37+6 +25, 22, OR );
					write_bitmap( adm, Line3px, 123-7-25, 22, OR );

					write_bitmap( adm, HourglassIcon, 94, 5, OR );

					sprintf( tstring, "%.1fs", pFac.pf.TwineStopTime);
					display_f1_text( tstring, TSMAX, 110, 10, FIXED );
					break;

				case 4:

					write_bitmap( adm, Line3px, 37+6 +25 + 8, 22, OR );
					write_bitmap( adm, Line3px, 123-7-25 - 8, 22, OR );

					write_bitmap( adm, ArrowDown, 37+6 +25 + 6, 12, OR );
					write_bitmap( adm, ArrowDown, 123-7-25 - 9, 12, OR );

					write_bitmap( adm, ArrowUp, 37+6 +25 + 6, 93, OR );
					write_bitmap( adm, ArrowUp, 123-7-25 - 9, 93, OR );



					sprintf( tstring, "%d", pFac.pf.TwineNumberOfRevolutionsAroundCentre);
					display_f1_text( tstring, TSMAX, 95, 6, FIXED );
				break;



			}



		}
	if(pFac.pf.GW302_NetTwineOption == TO_TWINE)
	{
		if( page == 0 )
		{
			write_bitmap( adm, ICON_SCROLL_BAR_PAGE_1_OF_2, 150, 11, AND );
		}
		else /* page == 1 */
		{
			write_bitmap( adm, ICON_SCROLL_BAR_PAGE_2_OF_2, 150, 11, AND );
		}

		draw_line( adm,   0, 0,   0, 127, OR ); /* restoring left-hand frame border */
		draw_line( adm, 159, 0, 159, 127, OR ); /* restoring right-hand frame border */
	}
	/* -----------------------------------------------------------
	 * Update values */
	if( page == 0 )
	{
		/* Bale Profile (Preset A - E) */
		draw_box( adm, pFac.pf.GW302_NetTwineOption ? (ICON_BALE_PROFILE_SETTING_X-3) : ICON_BALE_PROFILE_SETTING_X, ICON_BALE_PROFILE_SETTING_Y, pFac.pf.GW302_NetTwineOption ? (ICON_BALE_PROFILE_SETTING_X2 - 3) : ICON_BALE_PROFILE_SETTING_X2, ICON_BALE_PROFILE_SETTING_Y2, CLEAR );
		switch( pFac.pf.GW302_BaleProfile )
		{
		case BP_PRESET_A:
			write_bitmap( adm, ICON_BALE_PROFILE_A,  pFac.pf.GW302_NetTwineOption ? (ICON_BALE_PROFILE_SETTING_X-3) : ICON_BALE_PROFILE_SETTING_X, ICON_BALE_PROFILE_SETTING_Y, OR );
			break;
		case BP_PRESET_B:
			write_bitmap( adm, ICON_BALE_PROFILE_B,  pFac.pf.GW302_NetTwineOption ? (ICON_BALE_PROFILE_SETTING_X - 3) : ICON_BALE_PROFILE_SETTING_X, ICON_BALE_PROFILE_SETTING_Y, OR );
			break;
		case BP_PRESET_C:
			write_bitmap( adm, ICON_BALE_PROFILE_C,  pFac.pf.GW302_NetTwineOption ? (ICON_BALE_PROFILE_SETTING_X - 3) :ICON_BALE_PROFILE_SETTING_X, ICON_BALE_PROFILE_SETTING_Y, OR );
			break;
		case BP_PRESET_D:
			write_bitmap( adm, ICON_BALE_PROFILE_D,  pFac.pf.GW302_NetTwineOption ?  (ICON_BALE_PROFILE_SETTING_X - 3):ICON_BALE_PROFILE_SETTING_X, ICON_BALE_PROFILE_SETTING_Y, OR );
			break;
		default: /* BP_PRESET_E */
			write_bitmap( adm, ICON_BALE_PROFILE_E,  pFac.pf.GW302_NetTwineOption ?  (ICON_BALE_PROFILE_SETTING_X - 3):ICON_BALE_PROFILE_SETTING_X, ICON_BALE_PROFILE_SETTING_Y, OR );
			break;
		} /* end switch( pFac.pf.GW302_BaleProfile ) */

		sprintf( tstring, "%3u", pFac.pf.GW302_CoreDiameter[pFac.pf.GW302_BaleProfile] );
		display_f1_text( tstring, TSMAX, pFac.pf.GW302_NetTwineOption ? (ICON_CORE_DIAMETER_TEXT_X - 3):ICON_CORE_DIAMETER_TEXT_X, ICON_CORE_DIAMETER_TEXT_Y, FIXED );

		sprintf( tstring, "%3u", pFac.pf.GW302_BaleDiameter[pFac.pf.GW302_BaleProfile] );
		display_f1_text( tstring, TSMAX, pFac.pf.GW302_NetTwineOption ? (ICON_BALE_DIAMETER_TEXT_X - 3) : ICON_BALE_DIAMETER_TEXT_X, ICON_BALE_DIAMETER_TEXT_Y, FIXED );

		sprintf( tstring, "%3u", Bar[pFac.pf.GW302_CoreDensity[pFac.pf.GW302_BaleProfile] - 1] );
		display_f1_text( tstring, TSMAX, pFac.pf.GW302_NetTwineOption ? (ICON_CORE_DENSITY_TEXT_X - 3) : ICON_CORE_DENSITY_TEXT_X, ICON_CORE_DENSITY_TEXT_Y, FIXED );

		sprintf( tstring, "%3u", Bar[pFac.pf.GW302_BaleDensity[pFac.pf.GW302_BaleProfile] - 1] );
		display_f1_text( tstring, TSMAX, pFac.pf.GW302_NetTwineOption ? (ICON_BALE_DENSITY_TEXT_X - 3) : ICON_BALE_DENSITY_TEXT_X, ICON_BALE_DENSITY_TEXT_Y, FIXED );

		if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
		{
			sprintf( tstring, "%2.1f", pFac.pf.GW302_NetLayers[pFac.pf.GW302_BaleProfile] );
			display_f1_text( tstring, TSMAX, ICON_NET_LAYERS_TEXT_X, ICON_NET_LAYERS_TEXT_Y, FIXED );
		}


		/* -----------------------------------------------------------
		 * Flash currently selected value */

		if( half_second )
		{
			/* flashing currently selected value off */
			switch( selection )
			{
			case 0: /* (1,1) */
				draw_box( adm, pFac.pf.GW302_NetTwineOption ? (ICON_BALE_PROFILE_SETTING_X-3) : ICON_BALE_PROFILE_SETTING_X, ICON_BALE_PROFILE_SETTING_Y, pFac.pf.GW302_NetTwineOption ? (ICON_BALE_PROFILE_SETTING_X2 - 3) : ICON_BALE_PROFILE_SETTING_X2, ICON_BALE_PROFILE_SETTING_Y2, CLEAR );
				break;
			case 1: /* (2,1) */
				display_f1_text( "   ", TSMAX, pFac.pf.GW302_NetTwineOption ? (ICON_CORE_DIAMETER_TEXT_X - 3):ICON_CORE_DIAMETER_TEXT_X, ICON_CORE_DIAMETER_TEXT_Y, FIXED );
				break;
			case 2: /* (3,1) */
				display_f1_text( "   ", TSMAX, pFac.pf.GW302_NetTwineOption ? (ICON_BALE_DIAMETER_TEXT_X - 3) : ICON_BALE_DIAMETER_TEXT_X, ICON_BALE_DIAMETER_TEXT_Y, FIXED );
				if( TryingToSetBaleDiaBelowCore )
				{
					display_f1_text( "   ", TSMAX, pFac.pf.GW302_NetTwineOption ? (ICON_CORE_DENSITY_TEXT_X - 3) : ICON_CORE_DENSITY_TEXT_X, ICON_CORE_DIAMETER_TEXT_Y, FIXED ); // Flash off
				}
				break;
			case 3: /* (1,2) */
				display_f1_text( "   ", TSMAX, pFac.pf.GW302_NetTwineOption ? (ICON_CORE_DENSITY_TEXT_X - 3) : ICON_CORE_DENSITY_TEXT_X, ICON_CORE_DENSITY_TEXT_Y, FIXED );
				break;
			case 4: /* (2,2) */
				display_f1_text( "   ", TSMAX, pFac.pf.GW302_NetTwineOption ? (ICON_BALE_DENSITY_TEXT_X - 3) : ICON_BALE_DENSITY_TEXT_X, ICON_BALE_DENSITY_TEXT_Y, FIXED );
				break;
			case 5: /* (1,3) */
				if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
					display_f1_text( "   ", TSMAX, ICON_NET_LAYERS_TEXT_X, ICON_NET_LAYERS_TEXT_Y, FIXED );
				break;
			}
		} /* end if( half_second ) */

	}
	/* -----------------------------------------------------------
	 * Drawing tabs -- OL, IL & NAVIGATION tab */
		if( page == 0 )
		{
			if( selection != old_selection )
			{
				// Clear OL, IL & NAVIGATION tab area -- avoiding deleting section of frame border
				draw_box( adm, 1, 102, 62+36-1, 102+24-1,   CLEAR );

				switch( selection )
				{
				case 0: /* (1,1) Bale Profile */
					write_bitmap( adm, TABLEFT,    0, 107, OR ); // <
					write_bitmap( adm, TABRIGHT,  31, 107, OR ); // >
					write_bitmap( adm, TABNAV_DR, 62, 102, OR ); // NAVIGATION: Down Right
					break;
				case 1: /* (2,1) Core Diameter */
					write_bitmap( adm, TABMINUS,    0, 107, OR ); // -
					write_bitmap( adm, TABPLUS,    31, 107, OR ); // +
					write_bitmap( adm, TABNAV_DLR, 62, 102, OR ); // NAVIGATION: Down Left Right
					break;
				case 2: /* (3,1) Bale Diameter */
					write_bitmap( adm, TABMINUS,    0, 107, OR ); // -
					write_bitmap( adm, TABPLUS,    31, 107, OR ); // +
					write_bitmap( adm, TABNAV_DLR, 62, 102, OR ); // NAVIGATION: Down Left Right
					break;
				case 3: /* (1,2) Core Density */
					write_bitmap( adm, TABMINUS,     0, 107, OR ); // -
					write_bitmap( adm, TABPLUS,     31, 107, OR ); // +
					write_bitmap( adm, TABNAV_UDLR, 62, 102, OR ); // NAVIGATION: Up Down Left Right
					break;
				case 4: /* (2,2) Bale Density */
					write_bitmap( adm, TABMINUS,     0, 107, OR ); // -
					write_bitmap( adm, TABPLUS,     31, 107, OR ); // +
					write_bitmap( adm, TABNAV_UDLR, 62, 102, OR ); // NAVIGATION: Up Down Left Right
					break;
				case 5: /* (1,3) Net Layers */
					write_bitmap( adm, TABMINUS,   0, 107, OR ); // -
					write_bitmap( adm, TABPLUS,   31, 107, OR ); // +
					write_bitmap( adm, TABNAV_UL, 62, 102, OR ); // NAVIGATION: Up Left
					break;
				}
			} /* end if( selection != old_selection ) */
		}

}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen111
// Description         : Machine Menu - Machine Setup Menu
// -----------------------------------------------------------

/*  Origin          Page 1                 Origin          Page 2
       x ----------------------------         x ----------------------------
       | (1,1) Diameter Correction   |        | (1,1) Bale Indicators       |
       | ----------------------------          -----------------------------
       | (1,2) Net Delay             |        | (1,2) Fill Pot Zero (Reset) |
       | ----------------------------          -----------------------------
       | (1,3) Net Metres (Reset)    |        |                             |
       | ----------------------------          -----------------------------
       | (1,4) Pre Warning Beep      |        |                             |
         ----------------------------          ---------------------------- */

/* Origin */
#define MACHINE_SETUP_MENU_ICONS_ORIGIN_X         0
#define MACHINE_SETUP_MENU_ICONS_ORIGIN_Y         0

/* Page 1 - Grid position (1,1) */
#define ICON_DIAMETER_CORRECTION_OFFSET_X        32
#define ICON_DIAMETER_CORRECTION_OFFSET_Y        5
#define ICON_DIAMETER_CORRECTION_TEXT_OFFSET_X   92 - 10   /* For right justified text */
#define ICON_DIAMETER_CORRECTION_TEXT_OFFSET_Y   15 - 5

/* Page 1 - Grid position (1,2) */
#define ICON_NET_DELAY_OFFSET_X        32
#define ICON_NET_DELAY_OFFSET_Y        32
#define ICON_NET_DELAY_TEXT_OFFSET_X   99 - 32
#define ICON_NET_DELAY_TEXT_OFFSET_Y   38 - 32

/* Page 1 - Grid position (1.3) */
#define ICON_NET_METRES_OFFSET_X        32
#define ICON_NET_METRES_OFFSET_Y        56 - 1 - 1
#define ICON_NET_METRES_TEXT_OFFSET_X   81        /* For right justified text */
#define ICON_NET_METRES_TEXT_OFFSET_Y   63 - 56

/* Page 1 - Grid position (1,4) */
#define ICON_PRE_WARNING_BEEP_OFFSET_X          32
#define ICON_PRE_WARNING_BEEP_OFFSET_Y          82 - 1 - 2
#define ICON_PRE_WARNING_BEEP_TEXT_OFFSET_X     102 - 32 + (2*6)  /* For right justified text */
#define ICON_PRE_WARNING_BEEP_TEXT_OFFSET_Y     88 - 82

/* Page 2 - Grid position (1,1) */
#define ICON_BALE_INDICATOR_OFFSET_X           32
#define ICON_BALE_INDICATOR_OFFSET_Y           10
#define ICON_BALE_INDICATOR_SETTING_OFFSET_X  113 - 32
#define ICON_BALE_INDICATOR_SETTING_OFFSET_Y   10 - 10

/* Page 2 - Grid position (1,2) */
#define ICON_FILL_POT_ZERO_OFFSET_X        32
#define ICON_FILL_POT_ZERO_OFFSET_Y        33
#define ICON_FILL_POT_ZERO_TEXT_OFFSET_X   99 - 32
#define ICON_FILL_POT_ZERO_TEXT_OFFSET_Y   38 - 33

/* Page 2 - Grid position (1,3) */
#define ICON_TWINE_ZERO_OFFSET_X        42
#define ICON_TWINE_ZERO_OFFSET_Y        56 - 1 - 1
#define ICON_TWINE_ZERO_TEXT_OFFSET_X   81
#define ICON_TWINE_ZERO_TEXT_OFFSET_Y   63 - 56

/* Top left icon co-ordinates (x,y) */

#define ICON_DIAMETER_CORRECTION_X  MACHINE_SETUP_MENU_ICONS_ORIGIN_X + ICON_DIAMETER_CORRECTION_OFFSET_X
#define ICON_DIAMETER_CORRECTION_Y  MACHINE_SETUP_MENU_ICONS_ORIGIN_Y + ICON_DIAMETER_CORRECTION_OFFSET_Y

#define ICON_NET_DELAY_X            MACHINE_SETUP_MENU_ICONS_ORIGIN_X + ICON_NET_DELAY_OFFSET_X
#define ICON_NET_DELAY_Y            MACHINE_SETUP_MENU_ICONS_ORIGIN_Y + ICON_NET_DELAY_OFFSET_Y

#define ICON_NET_METRES_X           MACHINE_SETUP_MENU_ICONS_ORIGIN_X + ICON_NET_METRES_OFFSET_X
#define ICON_NET_METRES_Y           MACHINE_SETUP_MENU_ICONS_ORIGIN_Y + ICON_NET_METRES_OFFSET_Y

#define ICON_PRE_WARNING_BEEP_X     MACHINE_SETUP_MENU_ICONS_ORIGIN_X + ICON_PRE_WARNING_BEEP_OFFSET_X
#define ICON_PRE_WARNING_BEEP_Y     MACHINE_SETUP_MENU_ICONS_ORIGIN_Y + ICON_PRE_WARNING_BEEP_OFFSET_Y

#define ICON_BALE_INDICATOR_X       MACHINE_SETUP_MENU_ICONS_ORIGIN_X + ICON_BALE_INDICATOR_OFFSET_X
#define ICON_BALE_INDICATOR_Y       MACHINE_SETUP_MENU_ICONS_ORIGIN_Y + ICON_BALE_INDICATOR_OFFSET_Y

#define ICON_FILL_POT_ZERO_X        MACHINE_SETUP_MENU_ICONS_ORIGIN_X + ICON_FILL_POT_ZERO_OFFSET_X
#define ICON_FILL_POT_ZERO_Y        MACHINE_SETUP_MENU_ICONS_ORIGIN_Y + ICON_FILL_POT_ZERO_OFFSET_Y

#define ICON_TWINE_ZERO_X       	MACHINE_SETUP_MENU_ICONS_ORIGIN_X + ICON_TWINE_ZERO_OFFSET_X
#define ICON_TWINE_ZERO_Y      	 	MACHINE_SETUP_MENU_ICONS_ORIGIN_Y + ICON_DIAMETER_CORRECTION_OFFSET_Y

/* Text (top left) co-ordinates (textx,texty) / Setting icon & dimensions */

#define ICON_DIAMETER_CORRECTION_TEXT_X   ICON_DIAMETER_CORRECTION_X + ICON_DIAMETER_CORRECTION_TEXT_OFFSET_X
#define ICON_DIAMETER_CORRECTION_TEXT_Y   ICON_DIAMETER_CORRECTION_Y + ICON_DIAMETER_CORRECTION_TEXT_OFFSET_Y

#define ICON_NET_DELAY_TEXT_X             ICON_NET_DELAY_X + ICON_NET_DELAY_TEXT_OFFSET_X
#define ICON_NET_DELAY_TEXT_Y             ICON_NET_DELAY_Y + ICON_NET_DELAY_TEXT_OFFSET_Y

#define ICON_NET_METRES_TEXT_X            ICON_NET_METRES_X + ICON_NET_METRES_TEXT_OFFSET_X
#define ICON_NET_METRES_TEXT_Y            ICON_NET_METRES_Y + ICON_NET_METRES_TEXT_OFFSET_Y

#define ICON_PRE_WARNING_BEEP_TEXT_X      ICON_PRE_WARNING_BEEP_X + ICON_PRE_WARNING_BEEP_TEXT_OFFSET_X
#define ICON_PRE_WARNING_BEEP_TEXT_Y      ICON_PRE_WARNING_BEEP_Y + ICON_PRE_WARNING_BEEP_TEXT_OFFSET_Y

#define ICON_BALE_INDICATOR_SETTING_X     ICON_BALE_INDICATOR_X + ICON_BALE_INDICATOR_SETTING_OFFSET_X
#define ICON_BALE_INDICATOR_SETTING_Y     ICON_BALE_INDICATOR_Y + ICON_BALE_INDICATOR_SETTING_OFFSET_Y
#define ICON_BALE_INDICATOR_SETTING_X2    ICON_BALE_INDICATOR_SETTING_X + 12 - 1  /* ICON_BALE_INDICATOR_OFF/_ON /_BEEP bitmap X dim - 1 */
#define ICON_BALE_INDICATOR_SETTING_Y2    ICON_BALE_INDICATOR_SETTING_Y + 18 - 1  /* ICON_BALE_INDICATOR_OFF/_ON /_BEEP bitmap Y dim - 1 */

#define ICON_FILL_POT_ZERO_TEXT_X         ICON_FILL_POT_ZERO_X + ICON_FILL_POT_ZERO_TEXT_OFFSET_X
#define ICON_FILL_POT_ZERO_TEXT_Y         ICON_FILL_POT_ZERO_Y + ICON_FILL_POT_ZERO_TEXT_OFFSET_Y

#define ICON_TWINE_TEXT_X        		  ICON_TWINE_ZERO_X + ICON_TWINE_ZERO_TEXT_OFFSET_X
#define ICON_TWINE_TEXT_Y        		  ICON_TWINE_ZERO_Y + ICON_TWINE_ZERO_TEXT_OFFSET_Y

/* Page (of icons) */
#define PAGE_X1    ICON_DIAMETER_CORRECTION_X
#define PAGE_Y1    ICON_DIAMETER_CORRECTION_Y
#define PAGE_X2    ICON_PRE_WARNING_BEEP_X + 100 - 1  /* ICON_DIAMETER_CORRECTION bitmap X dim - 1 */
#define PAGE_Y2    ICON_PRE_WARNING_BEEP_Y + 19  + 3  /* ICON_PRE_WARNING_BEEP bitmap Y dim - 1    */

/* Scroll bar */
#define ICON_SCROLL_BAR_X      146
#define ICON_SCROLL_BAR_Y       11


void screen111( unsigned int lkey )
{
	unsigned int  key_status;
	unsigned char old_selection;
	unsigned char old_page;

	static unsigned char page;


	if( startup != 1)
	{
		startup = 1;

		draw_frame( adm );


		/* -----------------------------------------------------------
		 * Drawing tabs -- IR & OR tab */

		write_bitmap( adm, TAB,     99, 107, OR );
		write_bitmap( adm, TABESC, 130, 107, OR ); // ESC


		old_selection = 0xff;
		old_page = 0xff;
	}
	else
	{
		old_selection = selection;
		old_page = page;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	/* -----------------------------------------------------------
	 * Key press */

	key_status = lkey & (SW_NEW | SW_REP);

	if( key_status )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_OR ) // outer-right button (softkey ESC)
		{
			escape = 1;
			page = 0;
		}
		else if( lkey == SW_OL ) // outer-left button (softkey -)
		{
			if( page == 0 )
			{
				switch( selection )
				{

				case 0:
					if( pFac.pf.GW302_TwineOption == TO_ON )
					{
						old_page = 0xff;
						selection = 0;
						old_selection = 0xff;

						if( pFac.pf.GW302_NetTwineOption > PF_GW302_NetTwineOption[PF_MIN] )  /* Net/Twine ICON  [Net/Twine] */
						{
							pFac.pf.GW302_NetTwineOption--;
						}
						else
						{
							pFac.pf.GW302_NetTwineOption = PF_GW302_NetTwineOption[PF_MAX];
						}

						draw_box( adm, ICON_TWINE_ZERO_X, ICON_TWINE_ZERO_Y, ICON_TWINE_ZERO_X + 100, ICON_TWINE_ZERO_Y + 30, CLEAR );
							switch( pFac.pf.GW302_NetTwineOption )
							{
							case TO_TWINE:
								write_bitmap( adm, ICON_NET_TWINE_WITH_TWINE_HIGHLIGHTED, ICON_TWINE_ZERO_X, ICON_TWINE_ZERO_Y, OR );
								TwineAutoManMode = 0;
								AutoMan = 0;
								ResetSequenceFactors( SEQ_ALL );
								MWarn = 0;
								break;
							default:  /*case NTO_NET*/
								write_bitmap( adm, ICON_NET_TWINE_WITH_NET_HIGHLIGHTED, ICON_TWINE_ZERO_X, ICON_TWINE_ZERO_Y, OR );
								AutoMan = 0;
								TwineAutoManMode = 0;
								ResetSequenceFactors( SEQ_ALL );
								HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_OFF; //Feed Motor
								HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF; //Track Motor Forward
								HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF; //Track Motor Reverse
								HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_OFF; //Track Motor Reverse

								MWarn &= ~MW_SINGLE_NO_TWINE;
								MWarn &= ~MW_DOUBLE_NO_TWINE;
								MWarn &= ~MW_NO_CAN_COM;
								MWarn &= ~MW_TRACK_POSITION;
								MWarn &= ~MW_NO_IP4_PULSES;

								TwineStage = TWINE_STAGE_12;
								MWarn = 0;
								break;
							}
					}
					break;


				case 1: /* Page 1 - Grid position (1,1) - Diameter Correction*/
					// -
					pFac.pf.GW302_DiameterCorrection -= PF_GW302_DiameterCorrection[PF_STEP];
					if( pFac.pf.GW302_DiameterCorrection < PF_GW302_DiameterCorrection[PF_MIN] )
						pFac.pf.GW302_DiameterCorrection = PF_GW302_DiameterCorrection[PF_MIN];  // Limiting to minimum value
					break;
				case 2: /* Page 1 - Grid position (1,2) - Net Delay */
					// -
					if( pFac.pf.GW302_NetTwineOption == TO_TWINE )
					{
						pFac.pf.GW302_TwineDelay -= PF_GW302_TwineDelay[PF_STEP];
						if( pFac.pf.GW302_TwineDelay < PF_GW302_TwineDelay[PF_MIN] )
							pFac.pf.GW302_TwineDelay = PF_GW302_TwineDelay[PF_MIN];  // Limiting to minimum value
						break;
					}
					else
					{
						pFac.pf.GW302_NetDelay -= PF_GW302_NetDelay[PF_STEP];
						if( pFac.pf.GW302_NetDelay < PF_GW302_NetDelay[PF_MIN] )
							pFac.pf.GW302_NetDelay = PF_GW302_NetDelay[PF_MIN];  // Limiting to minimum value
						break;
					}
				case 3: /* Page 1 - Grid position (1.3) - Net Metres */
					// RESET
					if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
					{
						main_window = screen125;
					}
					else
					{
						pFac.pf.GW302_PreWarningBeep -= PF_GW302_PreWarningBeep[PF_STEP];
						if( pFac.pf.GW302_PreWarningBeep < PF_GW302_PreWarningBeep[PF_MIN] )
							pFac.pf.GW302_PreWarningBeep = PF_GW302_PreWarningBeep[PF_MIN];    // Limiting to minimum value 1
					}

					break;

				}
			}
			else /* page == 1 */
			{
				switch( selection )
				{
				case 0: /* Page 1 - Grid position (1,4) - Pre Warning Beep */
				// -
				if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
				{
					pFac.pf.GW302_PreWarningBeep -= PF_GW302_PreWarningBeep[PF_STEP];
					if( pFac.pf.GW302_PreWarningBeep < PF_GW302_PreWarningBeep[PF_MIN] )
						pFac.pf.GW302_PreWarningBeep = PF_GW302_PreWarningBeep[PF_MIN];    // Limiting to minimum value 1
				}
				else
				{
					// <
					if( pFac.pf.GW302_BaleShapeIndicator == BSI_ON )
					{
						pFac.pf.GW302_BaleShapeIndicator = BSI_OFF;
					}
					else if( pFac.pf.GW302_BaleShapeIndicator == BSI_BEEP )
					{
						pFac.pf.GW302_BaleShapeIndicator = BSI_ON;
					}
					else /* pFac.pf.GW302_BaleShapeIndicator == BSI_OFF */
					{
						pFac.pf.GW302_BaleShapeIndicator = BSI_BEEP;
					}
				}
				break;


				case 1: /* Page 2 - Grid position (1,1) - Bale Shape Indicator  ( OFF < ON < BEEP < OFF ) */
					// <
					if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
					{
						// <
						if( pFac.pf.GW302_BaleShapeIndicator == BSI_ON )
						{
							pFac.pf.GW302_BaleShapeIndicator = BSI_OFF;
						}
						else if( pFac.pf.GW302_BaleShapeIndicator == BSI_BEEP )
						{
							pFac.pf.GW302_BaleShapeIndicator = BSI_ON;
						}
						else /* pFac.pf.GW302_BaleShapeIndicator == BSI_OFF */
						{
							pFac.pf.GW302_BaleShapeIndicator = BSI_BEEP;
						}
					}
					else
					{
						main_window = screen124; // Fill Pot Zero Reset Screen (AKA Bale Shape Indicator Zero)
					}

					break;
				case 2: /* Page 2 - Grid position (1,2) - Bale Shape Indicator Zero */
					// RESET
					main_window = screen124; // Fill Pot Zero Reset Screen (AKA Bale Shape Indicator Zero)
					break;


				}
			}
			SavecalDelayWithinMenus = 1;
		}
		else if( lkey == SW_IL ) // inner-left button (softkey +)
		{
			if( page == 0 )
			{
				switch( selection )
				{

				case 0:


					if( pFac.pf.GW302_TwineOption == TO_ON )
					{

						old_page = 0xff;
						old_selection = 0xff;
						selection = 0;

						if( pFac.pf.GW302_NetTwineOption < PF_GW302_NetTwineOption[PF_MAX] )  /* Net/Twine ICON  [Net/Twine] */
						{
							pFac.pf.GW302_NetTwineOption++; /*TO_TWINE*/
						}
						else
						{
							pFac.pf.GW302_NetTwineOption = PF_GW302_NetTwineOption[PF_MIN]; /*TO_NET*/
						}
						draw_box( adm, ICON_TWINE_ZERO_X, ICON_TWINE_ZERO_Y, ICON_TWINE_ZERO_X + 100, ICON_TWINE_ZERO_Y + 30, CLEAR );

							switch( pFac.pf.GW302_NetTwineOption )
							{
							case TO_TWINE:
								write_bitmap( adm, ICON_NET_TWINE_WITH_TWINE_HIGHLIGHTED, ICON_TWINE_ZERO_X, ICON_TWINE_ZERO_Y, OR );
								TwineAutoManMode = 0;
								AutoMan = 0;
								ResetSequenceFactors( SEQ_ALL );
								MWarn = 0;
								break;
							default:  /*case TO_NET*/
								write_bitmap( adm, ICON_NET_TWINE_WITH_NET_HIGHLIGHTED, ICON_TWINE_ZERO_X, ICON_TWINE_ZERO_Y, OR );
								AutoMan = 0;
								TwineAutoManMode = 0;
								ResetSequenceFactors( SEQ_ALL );
								HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_OFF; //Feed Motor
								HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF; //Track Motor Forward
								HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF; //Track Motor Reverse
								HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_OFF; //Track Motor Reverse

								MWarn &= ~MW_SINGLE_NO_TWINE;
								MWarn &= ~MW_DOUBLE_NO_TWINE;
								MWarn &= ~MW_NO_CAN_COM;
								MWarn &= ~MW_TRACK_POSITION;
								MWarn &= ~MW_NO_IP4_PULSES;

								TwineStage = TWINE_STAGE_12;
								MWarn = 0;

								break;
							}
					}
				break;

				case 1: /* Page 1 - Grid position (1,1) - Diameter Correction*/
					// +
					pFac.pf.GW302_DiameterCorrection += PF_GW302_DiameterCorrection[PF_STEP];
					if( pFac.pf.GW302_DiameterCorrection > PF_GW302_DiameterCorrection[PF_MAX] )
						pFac.pf.GW302_DiameterCorrection = PF_GW302_DiameterCorrection[PF_MAX];  // Limiting to maximum value
					break;
				case 2: /* Page 1 - Grid position (1,2) - Net Delay */
					// +
					if( pFac.pf.GW302_NetTwineOption == TO_TWINE )
					{
						pFac.pf.GW302_TwineDelay += PF_GW302_TwineDelay[PF_STEP];
						if( pFac.pf.GW302_TwineDelay > PF_GW302_TwineDelay[PF_MAX] )
							pFac.pf.GW302_TwineDelay = PF_GW302_TwineDelay[PF_MAX];  // Limiting to maximum value
						break;
					}
					else
					{
						pFac.pf.GW302_NetDelay += PF_GW302_NetDelay[PF_STEP];
						if( pFac.pf.GW302_NetDelay > PF_GW302_NetDelay[PF_MAX] )
							pFac.pf.GW302_NetDelay = PF_GW302_NetDelay[PF_MAX];  // Limiting to maximum value
						break;
					}
				case 3: /* Page 1 - Grid position (1,4) - Pre Warning Beep */
					// +
					if(pFac.pf.GW302_NetTwineOption == TO_TWINE)
					{
						pFac.pf.GW302_PreWarningBeep += PF_GW302_PreWarningBeep[PF_STEP];
						if( pFac.pf.GW302_PreWarningBeep > PF_GW302_PreWarningBeep[PF_MAX] )
							pFac.pf.GW302_PreWarningBeep = PF_GW302_PreWarningBeep[PF_MAX];  // Limiting to maximum value
					}
					break;

				}
			}
			else /* page == 1 */
			{
				switch( selection )
				{
				case 0: /* Page 2 - Grid position (1,1) - Bale Shape Indicator  ( OFF < ON < BEEP < OFF ) */
					// >
					if(pFac.pf.GW302_NetTwineOption == TO_TWINE)
					{
						if( pFac.pf.GW302_BaleShapeIndicator == BSI_ON )
						{
							pFac.pf.GW302_BaleShapeIndicator = BSI_BEEP;
						}
						else if( pFac.pf.GW302_BaleShapeIndicator == BSI_BEEP )
						{
							pFac.pf.GW302_BaleShapeIndicator = BSI_OFF;
						}
						else /* pFac.pf.GW302_BaleShapeIndicator == BSI_OFF */
						{
							pFac.pf.GW302_BaleShapeIndicator = BSI_ON;
						}
					}
					else
					{
						pFac.pf.GW302_PreWarningBeep += PF_GW302_PreWarningBeep[PF_STEP];
						if( pFac.pf.GW302_PreWarningBeep > PF_GW302_PreWarningBeep[PF_MAX] )
							pFac.pf.GW302_PreWarningBeep = PF_GW302_PreWarningBeep[PF_MAX];  // Limiting to maximum value
					}
					break;
				case 1: /* Page 2 - Grid position (1,2) - Bale Shape Indicator Zero */
					if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
					{
						if( pFac.pf.GW302_BaleShapeIndicator == BSI_ON )
						{
							pFac.pf.GW302_BaleShapeIndicator = BSI_BEEP;
						}
						else if( pFac.pf.GW302_BaleShapeIndicator == BSI_BEEP )
						{
							pFac.pf.GW302_BaleShapeIndicator = BSI_OFF;
						}
						else /* pFac.pf.GW302_BaleShapeIndicator == BSI_OFF */
						{
							pFac.pf.GW302_BaleShapeIndicator = BSI_ON;
						}
					}
					break;

				case 2:
					break;



				}
			}
			SavecalDelayWithinMenus = 1;
		}
		else if( lkey == SW_CU ) // navigation-up button (softkey ^)
		{
			if( page == 0 )
			{
				if( selection )
				{
					selection--;
					//if((pFac.pf.GW302_NetTwineOption == TO_TWINE) && (selection ==2))
					//	selection = 1;
				}
			}
			else /* page == 1 */
			{
				if( selection )
				{
					selection--;

				}
				else /* Goto first page */
				{
					selection = 3; /* last option */
					page = 0;
				}
			}
		}
		else if( lkey == SW_CD ) // navigation-up button (softkey V)
		{
			if( page == 0 )
			{
				if( selection < 3 )
				{
					//if((pFac.pf.GW302_NetTwineOption == TO_TWINE) && (selection == 1))
					//	selection = 3;
					//else
						selection++;


				}
				else /* Goto second page */
				{
					selection = 0; /* first option */
					page = 1;
				}
			}
			else /* page == 1 */
			{
				if( pFac.pf.GW302_NetTwineOption != TO_TWINE) /* Only displayed if 'Twine Option' set to On.*/
				{
					if( selection < 2 )
					{
						selection++;
					}
				}
				else
				{
					if( selection < 1 )
					{
						selection++;
					}
				}
			}
		}

	} /* end if( key_status ) */


	/* -----------------------------------------------------------
	 * Drawing icons */

	if( page != old_page )
	{
		draw_box( adm, PAGE_X1, PAGE_Y1, PAGE_X2, PAGE_Y2, CLEAR );

		if( page == 0 )
		{


			switch( pFac.pf.GW302_NetTwineOption )
			{
			case TO_TWINE:
				write_bitmap( adm, ICON_NET_TWINE_WITH_TWINE_HIGHLIGHTED, ICON_TWINE_ZERO_X, ICON_DIAMETER_CORRECTION_Y, OR );
				break;
			default:  /*case NTO_NET*/
				write_bitmap( adm, ICON_NET_TWINE_WITH_NET_HIGHLIGHTED, ICON_TWINE_ZERO_X, ICON_DIAMETER_CORRECTION_Y, OR );
				break;
			}


			if((pFac.pf.GW302_TwineOption == TO_ON) && (pFac.pf.GW302_NetTwineOption == TO_TWINE))
			{

				write_bitmap( adm, ICON_DIAMETER_CORRECTION, ICON_NET_DELAY_X,  ICON_NET_DELAY_Y,  OR );
				write_bitmap( adm, ICON_TWINER_DELAY,           ICON_NET_METRES_X,            ICON_NET_METRES_Y+2,            OR );
				write_bitmap( adm, ICON_PRE_WARNING_BEEP,    ICON_PRE_WARNING_BEEP_X,     ICON_PRE_WARNING_BEEP_Y,     OR );

			}
			else
			{
				write_bitmap( adm, ICON_DIAMETER_CORRECTION, ICON_NET_DELAY_X,  ICON_NET_DELAY_Y,  OR );
				write_bitmap( adm, ICON_NET_DELAY,           ICON_NET_METRES_X,            ICON_NET_METRES_Y+5,            OR );
				write_bitmap( adm, ICON_NET_METRES,          ICON_PRE_WARNING_BEEP_X,           ICON_PRE_WARNING_BEEP_Y,           OR );
			}
			if((pFac.pf.GW302_TwineOption == TO_ON) && (pFac.pf.GW302_NetTwineOption == TO_TWINE))
			{

			}
		}
		else /* page == 1 */
		{

			if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
			{
				write_bitmap( adm, ICON_PRE_WARNING_BEEP,    ICON_PRE_WARNING_BEEP_X,     ICON_BALE_INDICATOR_Y,     OR );
				write_bitmap( adm, ICON_BALE_INDICATOR,      ICON_BALE_INDICATOR_X,       ICON_FILL_POT_ZERO_Y,       OR );
				write_bitmap( adm, ICON_FILL_POT_ZERO,       ICON_FILL_POT_ZERO_X,        MACHINE_SETUP_MENU_ICONS_ORIGIN_Y + ICON_TWINE_ZERO_OFFSET_Y,        OR );
			}
			else
			{
				write_bitmap( adm, ICON_BALE_INDICATOR,      ICON_BALE_INDICATOR_X,       ICON_BALE_INDICATOR_Y,       OR );
				write_bitmap( adm, ICON_FILL_POT_ZERO,       ICON_FILL_POT_ZERO_X,        ICON_FILL_POT_ZERO_Y,        OR );
			}




		}
	}


	/* -----------------------------------------------------------
	 * Drawing scroll bar */

	if( page != old_page )
	{
		if( page == 0 )
		{
			write_bitmap( adm, ICON_SCROLL_BAR_PAGE_1_OF_2, ICON_SCROLL_BAR_X, ICON_SCROLL_BAR_Y, AND );
		}
		else /* page == 1 */
		{
			write_bitmap( adm, ICON_SCROLL_BAR_PAGE_2_OF_2, ICON_SCROLL_BAR_X, ICON_SCROLL_BAR_Y, AND );
		}

		draw_line( adm,   0, 0,   0, 127, OR ); /* restoring left-hand frame border */
		draw_line( adm, 159, 0, 159, 127, OR ); /* restoring right-hand frame border */
	}


	/* -----------------------------------------------------------
	 * Drawing tabs -- OL, IL & NAVIGATION tab */

	if( (selection != old_selection) || (page != old_page) )
	{
		/* Clear OL, IL & NAVIGATION tab area -- avoiding deleting section of frame border */
		draw_box( adm, 1, 102, 62+36-1, 102+24-1,   CLEAR );

		if( page == 0 )
		{
			switch( selection )
			{
			case 0:
				write_bitmap( adm, TABLEFT,    0, 107, OR ); // <
				write_bitmap( adm, TABRIGHT,  31, 107, OR ); // >
				write_bitmap( adm, TABNAV_U, 62, 102, OR ); // NAVIGATION: Up Down
			break;

			case 1: /* Page 1 - Grid position (1,1) */
				write_bitmap( adm, TABMINUS,  0, 107, OR ); // -
				write_bitmap( adm, TABPLUS,  31, 107, OR ); // +
				write_bitmap( adm, TABNAV_D, 62, 102, OR ); // NAVIGATION: Down
				break;
			case 2: /* Page 1 - Grid position (1,2) */
				write_bitmap( adm, TABMINUS,  0, 107, OR ); // -
				write_bitmap( adm, TABPLUS,  31, 107, OR ); // +
				write_bitmap( adm, TABNAV_UD,62, 102, OR ); // NAVIGATION: Up Down
				break;
			case 3: /* Page 1 - Grid position (1.3) */
				if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
				{
					write_bitmap( adm, TABRESET,   0, 107, OR ); // RESET
					write_bitmap( adm, TAB,       31, 107, OR );
					write_bitmap( adm, TABNAV_UD, 62, 102, OR ); // NAVIGATION: Up Down
				}
				else
				{
					write_bitmap( adm, TABMINUS,  0, 107, OR ); // -
					write_bitmap( adm, TABPLUS,  31, 107, OR ); // +
					write_bitmap( adm, TABNAV_UD,62, 102, OR ); // NAVIGATION: Up Down
				}
				break;
			}
		}
		else /* page == 1 */
		{
			switch( selection )
			{
			case 0: /* Page 2 - Grid position (1,1) */
				if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
				{
				write_bitmap( adm, TABMINUS,  0, 107, OR ); // -
				write_bitmap( adm, TABPLUS,  31, 107, OR ); // +
				write_bitmap( adm, TABNAV_UD,62, 102, OR ); // NAVIGATION: Up Down
				}
				else
				{
					write_bitmap( adm, TABLEFT,    0, 107, OR ); // <
					write_bitmap( adm, TABRIGHT,  31, 107, OR ); // >
					write_bitmap( adm, TABNAV_UD, 62, 102, OR ); // NAVIGATION: Up Down
				}
				break;
			case 1: /* Page 2 - Grid position (1,2) */
				if( pFac.pf.GW302_NetTwineOption != TO_TWINE) /* Only displayed if 'Twine Option' set to On.*/
				{
					write_bitmap( adm, TABLEFT,    0, 107, OR ); // <
					write_bitmap( adm, TABRIGHT,  31, 107, OR ); // >


					if( pFac.pf.GW302_NetTwineOption != TO_TWINE) /* Only displayed if 'Twine Option' set to On.*/
						write_bitmap( adm, TABNAV_UD,  62, 102, OR ); // NAVIGATION: Up
					else
						write_bitmap( adm, TABNAV_U,  62, 102, OR );
				}
				else
				{
					write_bitmap( adm, TABRESET,   0, 107, OR ); // RESET
					write_bitmap( adm, TAB,       31, 107, OR );
					write_bitmap( adm, TABNAV_U,  62, 102, OR );
				}
				break;

			case 2: /* Page 2 - Grid position (1,2) */
				if( pFac.pf.GW302_NetTwineOption != TO_TWINE ) /* Only displayed if 'Twine Option' set to On.*/
				{
					write_bitmap( adm, TABRESET,   0, 107, OR ); // RESET
					write_bitmap( adm, TAB,       31, 107, OR );
					write_bitmap( adm, TABNAV_U,  62, 102, OR );
				}

				break;
			}
		}
	} /* end selection != old_selection */


	/* -----------------------------------------------------------
	 * Selection pointer */

	if( selection != old_selection )
	{
		if( page == old_page )
		{
			/* Clear old selection */
			if( page == 0 )
			{
				switch( old_selection )
				{

				case 0: /* Page 1 - Grid position (1,1) */
					draw_box( adm, ICON_TWINE_ZERO_X - 12, ICON_TWINE_ZERO_Y + 4, ICON_TWINE_ZERO_X - 2, ICON_TWINE_ZERO_Y, CLEAR );
					//write_bitmap( adm, ICON_DIAMETER_CORRECTION, ICON_DIAMETER_CORRECTION_OFFSET_X, ICON_DIAMETER_CORRECTION_OFFSET_Y, AND );
					break;

				case 1:
					draw_box( adm, ICON_TWINE_ZERO_X - 12, ICON_TWINE_ZERO_Y + 4, ICON_TWINE_ZERO_X - 2, ICON_TWINE_ZERO_Y, CLEAR );
					if((pFac.pf.GW302_TwineOption == TO_ON) && (pFac.pf.GW302_NetTwineOption == TO_TWINE))
					{
						write_bitmap( adm, ICON_DIAMETER_CORRECTION, ICON_DIAMETER_CORRECTION_OFFSET_X, ICON_NET_DELAY_OFFSET_Y, AND );

					}
					else
						write_bitmap( adm, ICON_DIAMETER_CORRECTION, ICON_DIAMETER_CORRECTION_OFFSET_X, ICON_NET_DELAY_OFFSET_Y, AND );
					break;

				case 2: /* Page 1 - Grid position (1,2) */
					if((pFac.pf.GW302_TwineOption == TO_ON) && (pFac.pf.GW302_NetTwineOption == TO_TWINE))
						write_bitmap( adm, ICON_TWINER_DELAY,ICON_NET_DELAY_OFFSET_X, ICON_NET_METRES_OFFSET_Y+2,  AND );
					else
						write_bitmap( adm, ICON_NET_DELAY, ICON_NET_METRES_OFFSET_X, ICON_NET_METRES_OFFSET_Y+5, AND );
					break;

				case 3: /* Page 1 - Grid position (1.3) */
					if((pFac.pf.GW302_NetTwineOption != TO_TWINE))
						write_bitmap( adm, ICON_NET_METRES, ICON_PRE_WARNING_BEEP_OFFSET_X, ICON_PRE_WARNING_BEEP_OFFSET_Y, AND );
					else
						write_bitmap( adm, ICON_PRE_WARNING_BEEP, ICON_PRE_WARNING_BEEP_OFFSET_X, ICON_PRE_WARNING_BEEP_OFFSET_Y, AND );
					break;
#ifdef sh
				case 3: /* Page 1 - Grid position (1,4) */

					break;
#endif
				}
			}
			else /* page == 1 */
			{
				switch( old_selection )
				{
				case 0: /* Page 2 - Grid position (1,1) */
					if((pFac.pf.GW302_NetTwineOption != TO_TWINE))
					{
						write_bitmap( adm, ICON_PRE_WARNING_BEEP, ICON_PRE_WARNING_BEEP_OFFSET_X, ICON_BALE_INDICATOR_OFFSET_Y, AND );
					}
					else
					{
						write_bitmap( adm, ICON_BALE_INDICATOR, ICON_BALE_INDICATOR_OFFSET_X, ICON_BALE_INDICATOR_OFFSET_Y, AND );
					}
					break;
				case 1: /* Page 2 - Grid position (1,2) */
					if((pFac.pf.GW302_NetTwineOption != TO_TWINE))
					{
						write_bitmap( adm, ICON_BALE_INDICATOR, ICON_BALE_INDICATOR_OFFSET_X, ICON_FILL_POT_ZERO_OFFSET_Y, AND );
					}
					else
					{
						write_bitmap( adm, ICON_FILL_POT_ZERO, ICON_FILL_POT_ZERO_OFFSET_X, ICON_FILL_POT_ZERO_OFFSET_Y, AND );
					}
					break;
				case 2: /* Page 2 - Grid position (1,3) -- not reached if 'Twine Option' set to Off */
					/*write_bitmap( adm, ICON_SELECTION_POINTER, ICON_TWINE_ZERO_X - 8, ICON_TWINE_ZERO_Y + 4, AND );*/
					if((pFac.pf.GW302_NetTwineOption != TO_TWINE))
					{
						write_bitmap( adm, ICON_FILL_POT_ZERO, ICON_FILL_POT_ZERO_OFFSET_X, ICON_TWINE_ZERO_OFFSET_Y, AND );
					}

					break;
				}
			}
		}

		/* Draw new selection */
		if( page == 0 )
		{
			switch( selection )
			{
			case 0:
				write_bitmap( adm, ICON_SELECTION_POINTER, ICON_TWINE_ZERO_X - 10, ICON_TWINE_ZERO_Y + 4, OR );
				break;


			case 1: /* Page 1 - Grid position (1,1) */
				draw_box( adm, ICON_TWINE_ZERO_X - 16, ICON_TWINE_ZERO_Y + 20, ICON_TWINE_ZERO_X - 2, ICON_TWINE_ZERO_Y-4, CLEAR );
				if((pFac.pf.GW302_TwineOption == TO_ON) && (pFac.pf.GW302_NetTwineOption == TO_TWINE))
				{
					write_bitmap( adm, ICON_DIAMETER_CORRECTION_SELECTED, ICON_DIAMETER_CORRECTION_OFFSET_X, ICON_NET_DELAY_OFFSET_Y, OR );

				}
				else
					write_bitmap( adm, ICON_DIAMETER_CORRECTION_SELECTED, ICON_DIAMETER_CORRECTION_OFFSET_X, ICON_NET_DELAY_OFFSET_Y, OR );
				break;

			case 2: /* Page 1 - Grid position (1,2) */
				if((pFac.pf.GW302_TwineOption == TO_ON) && (pFac.pf.GW302_NetTwineOption == TO_TWINE))
					write_bitmap( adm, ICON_TWINER_DELAY_SELECTED,ICON_NET_DELAY_OFFSET_X, ICON_NET_METRES_OFFSET_Y+2, OR );
				else
					write_bitmap( adm, ICON_NET_DELAY_SELECTED, ICON_NET_DELAY_OFFSET_X, ICON_NET_METRES_OFFSET_Y+5, OR );
				break;

			case 3: /* Page 1 - Grid position (1.3) */
				if((pFac.pf.GW302_NetTwineOption != TO_TWINE))
					write_bitmap( adm, ICON_NET_METRES_SELECTED, ICON_NET_METRES_OFFSET_X, ICON_PRE_WARNING_BEEP_OFFSET_Y, OR );
				else
					write_bitmap( adm, ICON_PRE_WARNING_BEEP_SELECTED, ICON_PRE_WARNING_BEEP_OFFSET_X, ICON_PRE_WARNING_BEEP_OFFSET_Y, OR );



				break;

			}
		}
		else /* page == 1 */
		{
			switch( selection )
			{
			case 0: /* Page 2 - Grid position (1,1) */
				if((pFac.pf.GW302_NetTwineOption != TO_TWINE))
				{
					write_bitmap( adm, ICON_PRE_WARNING_BEEP_SELECTED, ICON_PRE_WARNING_BEEP_OFFSET_X, ICON_BALE_INDICATOR_OFFSET_Y, OR );
				}
				else
				{
					write_bitmap( adm, ICON_BALE_INDICATOR_SELECTED, ICON_BALE_INDICATOR_OFFSET_X, ICON_BALE_INDICATOR_OFFSET_Y, OR );
				}
				break;
			case 1: /* Page 2 - Grid position (1,2) */
				if((pFac.pf.GW302_NetTwineOption != TO_TWINE))
				{
					write_bitmap( adm, ICON_BALE_INDICATOR_SELECTED, ICON_BALE_INDICATOR_OFFSET_X, ICON_FILL_POT_ZERO_OFFSET_Y, OR );
				}
				else
				{
					write_bitmap( adm, ICON_FILL_POT_ZERO_SELECTED, ICON_FILL_POT_ZERO_OFFSET_X, ICON_FILL_POT_ZERO_OFFSET_Y, OR );
				}
				break;
			case 2: /* Page 2 - Grid position (1,3) -- not reached if 'Twine Option' set to Off */
				if((pFac.pf.GW302_NetTwineOption != TO_TWINE))
				{
					write_bitmap( adm, ICON_FILL_POT_ZERO_SELECTED, ICON_FILL_POT_ZERO_OFFSET_X, ICON_TWINE_ZERO_OFFSET_Y, OR );
				}
				break;
			}
		}
	} /* end if( selection != old_selection ) */


	/* -----------------------------------------------------------
	 * Update values */

	if( page == 0 )
	{
		// Diameter Correction
		if((pFac.pf.GW302_NetTwineOption == TO_TWINE))
		{
		display_number( (float)pFac.pf.GW302_DiameterCorrection,
				(float)PF_GW302_DiameterCorrection[PF_MIN], (float)PF_GW302_DiameterCorrection[PF_MAX],
				1.0, DP0, ICON_DIAMETER_CORRECTION_TEXT_X, ICON_NET_DELAY_TEXT_Y+5,
				DW3, (RIGHT|NSIGN), &RDSfnt1, NULL, PNULL );
		}
		else
		{
			display_number( (float)pFac.pf.GW302_DiameterCorrection,
							(float)PF_GW302_DiameterCorrection[PF_MIN], (float)PF_GW302_DiameterCorrection[PF_MAX],
							1.0, DP0, ICON_DIAMETER_CORRECTION_TEXT_X, ICON_NET_DELAY_TEXT_Y+5,
							DW3, (RIGHT|NSIGN), &RDSfnt1, NULL, PNULL );
		}



		{
		// Net Delay



		}
		if((pFac.pf.GW302_NetTwineOption != TO_TWINE))
		{
			// Net Delay
			display_number( (float)pFac.pf.GW302_NetDelay,
					(float)PF_GW302_NetDelay[PF_MIN], (float)PF_GW302_NetDelay[PF_MAX],
					1.0, DP1, ICON_NET_DELAY_TEXT_X, ICON_NET_METRES_TEXT_Y+3,
					DW3, LEFT, &RDSfnt1, NULL, PNULL );
		}
		else
		{
			display_number( (float)pFac.pf.GW302_TwineDelay,
					(float)PF_GW302_TwineDelay[PF_MIN], (float)PF_GW302_TwineDelay[PF_MAX],
					1.0, DP1, ICON_NET_DELAY_TEXT_X, ICON_NET_METRES_TEXT_Y+2,
					DW3, LEFT, &RDSfnt1, NULL, PNULL );
		}

		if((pFac.pf.GW302_NetTwineOption != TO_TWINE))
		{
			// Net Metres
			display_number( (float)((unsigned int)pFac.pf.GW302_TotalNetMetres + NetMetresAppliedForCount),
					(float)PF_GW302_TotalNetMetres[PF_MIN], (float)PF_GW302_TotalNetMetres[PF_MAX],
					1.0, DP0, ICON_NET_METRES_TEXT_X, ICON_PRE_WARNING_BEEP_TEXT_Y,
					DW4, RIGHT, &RDSfnt1, NULL, PNULL );
		}
		else
		{
		// Pre Warning Beep
		display_number( (float)pFac.pf.GW302_PreWarningBeep,
				(float)PF_GW302_PreWarningBeep[PF_MIN], (float)PF_GW302_PreWarningBeep[PF_MAX],
				1.0, DP0, ICON_PRE_WARNING_BEEP_TEXT_X, ICON_PRE_WARNING_BEEP_TEXT_Y,
				DW3, RIGHT, &RDSfnt1, NULL, PNULL );
		}
	}
	else /* page == 1 */
	{

		if((pFac.pf.GW302_NetTwineOption != TO_TWINE))
		{


			// Pre Warning Beep
			display_number( (float)pFac.pf.GW302_PreWarningBeep,
					(float)PF_GW302_PreWarningBeep[PF_MIN], (float)PF_GW302_PreWarningBeep[PF_MAX],
					1.0, DP0, ICON_PRE_WARNING_BEEP_TEXT_X, ICON_BALE_INDICATOR_SETTING_Y+5,
					DW3, RIGHT, &RDSfnt1, NULL, PNULL );


			// Bale Shape Indicator  ( OFF < ON < BEEP < OFF )
				draw_box( adm, ICON_BALE_INDICATOR_SETTING_X, ICON_FILL_POT_ZERO_TEXT_Y-5, ICON_BALE_INDICATOR_SETTING_X2, ICON_FILL_POT_ZERO_TEXT_Y + 17, CLEAR );
				switch( pFac.pf.GW302_BaleShapeIndicator )
				{
				case BSI_OFF:
					write_bitmap( adm, ICON_BALE_INDICATOR_OFF,  ICON_BALE_INDICATOR_SETTING_X, ICON_FILL_POT_ZERO_OFFSET_Y  , OR );
					break;
				case BSI_ON:
					write_bitmap( adm, ICON_BALE_INDICATOR_ON,   ICON_BALE_INDICATOR_SETTING_X, ICON_FILL_POT_ZERO_OFFSET_Y  , OR );
					break;
				default: /* case BSI_BEEP */
					write_bitmap( adm, ICON_BALE_INDICATOR_BEEP, ICON_BALE_INDICATOR_SETTING_X, ICON_FILL_POT_ZERO_OFFSET_Y  , OR );
					break;
				}


			display_number( (float)pFac.pf.GW302_BaleShapeIndicatorZero,
						(float)PF_GW302_BaleShapeIndicatorZero[PF_MIN], (float)PF_GW302_BaleShapeIndicatorZero[PF_MAX],
						1.0, DP1, ICON_FILL_POT_ZERO_TEXT_X, ICON_TWINE_ZERO_OFFSET_Y+3,
						DW4, LEFT, &RDSfnt1, NULL, PNULL );
				display_f1_text( "V", TSMAX, ICON_FILL_POT_ZERO_TEXT_X+(4*6)-2, ICON_TWINE_ZERO_OFFSET_Y+3,  FIXED );
		}
		else
		{
			// Bale Shape Indicator  ( OFF < ON < BEEP < OFF )
			draw_box( adm, ICON_BALE_INDICATOR_SETTING_X, ICON_BALE_INDICATOR_SETTING_Y, ICON_BALE_INDICATOR_SETTING_X2, ICON_BALE_INDICATOR_SETTING_Y2, CLEAR );
			switch( pFac.pf.GW302_BaleShapeIndicator )
			{
			case BSI_OFF:
				write_bitmap( adm, ICON_BALE_INDICATOR_OFF,  ICON_BALE_INDICATOR_SETTING_X, ICON_BALE_INDICATOR_SETTING_Y, OR );
				break;
			case BSI_ON:
				write_bitmap( adm, ICON_BALE_INDICATOR_ON,   ICON_BALE_INDICATOR_SETTING_X, ICON_BALE_INDICATOR_SETTING_Y, OR );
				break;
			default: /* case BSI_BEEP */
				write_bitmap( adm, ICON_BALE_INDICATOR_BEEP, ICON_BALE_INDICATOR_SETTING_X, ICON_BALE_INDICATOR_SETTING_Y, OR );
				break;
			}

			// Bale Shape Indicator Zero
			display_number( (float)pFac.pf.GW302_BaleShapeIndicatorZero,
					(float)PF_GW302_BaleShapeIndicatorZero[PF_MIN], (float)PF_GW302_BaleShapeIndicatorZero[PF_MAX],
					1.0, DP1, ICON_FILL_POT_ZERO_TEXT_X, ICON_FILL_POT_ZERO_TEXT_Y,
					DW4, LEFT, &RDSfnt1, NULL, PNULL );
			display_f1_text( "V", TSMAX, ICON_FILL_POT_ZERO_TEXT_X+(4*6)-2, ICON_FILL_POT_ZERO_TEXT_Y,  FIXED );

		}

	}


	/* -----------------------------------------------------------
	 * Flash currently selected value */

	if( half_second )
	{
		/* flashing currently selected value off */
		if( page == 0 )
		{
			switch( selection )
			{

			case 0:

			break;

			case 1: /* Page 1 - Grid position (1,1) */
				if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
					display_f1_text( "   ", TSMAX, ICON_DIAMETER_CORRECTION_TEXT_X -(3*6), ICON_NET_DELAY_TEXT_Y+5, FIXED );
				else
					display_f1_text( "   ", TSMAX, ICON_DIAMETER_CORRECTION_TEXT_X -(3*6), ICON_NET_DELAY_TEXT_Y+5, FIXED );
				break;

			case 2: /* Page 1 - Grid position (1,2) */
				if(pFac.pf.GW302_NetTwineOption == TO_TWINE)
					display_f1_text( "   ", TSMAX, ICON_NET_DELAY_TEXT_X, ICON_NET_METRES_TEXT_Y+3, FIXED );
				else
					display_f1_text( "   ", TSMAX, ICON_NET_DELAY_TEXT_X, ICON_NET_METRES_TEXT_Y+3, FIXED );
				break;

			case 3: /* Page 1 - Grid position (1.3) */
				if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
					display_f1_text( "     ", TSMAX, ICON_NET_METRES_TEXT_X -(5*6), ICON_PRE_WARNING_BEEP_TEXT_Y, FIXED );
				else
					display_f1_text( "   ", TSMAX, ICON_PRE_WARNING_BEEP_TEXT_X -(3*6), ICON_PRE_WARNING_BEEP_TEXT_Y, FIXED );
				break;

			}
		}
		else /* page == 1 */
		{
			switch( selection )
			{
			case 0: /* Page 2 - Grid position (1,1) */
				if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
				{
					display_f1_text( "   ", TSMAX, ICON_PRE_WARNING_BEEP_TEXT_X -(3*6), ICON_BALE_INDICATOR_SETTING_Y+5, FIXED );
				}
				else
					draw_box( adm, ICON_BALE_INDICATOR_SETTING_X, ICON_BALE_INDICATOR_SETTING_Y, ICON_BALE_INDICATOR_SETTING_X2+5, ICON_BALE_INDICATOR_SETTING_Y2, CLEAR );
				break;
			case 1: /* Page 2 - Grid position (1,2) */
				if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
				{
					draw_box( adm, ICON_BALE_INDICATOR_SETTING_X, ICON_FILL_POT_ZERO_TEXT_Y-5, ICON_BALE_INDICATOR_SETTING_X2, ICON_FILL_POT_ZERO_TEXT_Y + 17, CLEAR );
				}
				else
					display_f1_text( "   ", TSMAX, ICON_FILL_POT_ZERO_TEXT_X, ICON_FILL_POT_ZERO_TEXT_Y, FIXED );
				break;
			case 2:  /*Page 2 - Grid position (1,3) */
				if(pFac.pf.GW302_NetTwineOption != TO_TWINE)	// Only displayed if 'Twine Option' set to On; nothing displayed here if 'Twine Option' set to Off
					display_f1_text( "   ", TSMAX, ICON_FILL_POT_ZERO_TEXT_X, ICON_TWINE_ZERO_OFFSET_Y+3, FIXED );
				break;
			}
		}
	} /* end if( half_second ) */

}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen112
// Description         : Machine Menu - Bale Count Menu
// -----------------------------------------------------------

/*  Origin
       x ----------------------------------------------------------
       | (1,1) Bale Count in Store A | (2,1) Bale Count in Store G |
       | --------------------------------------------------------- |
       | (1,2) Bale Count in Store B | (2,2) Bale Count in Store H |
       | --------------------------------------------------------- |
       | (1,3) Bale Count in Store C | (2,3) Bale Count in Store I |
       | --------------------------------------------------------- |
       | (1,4) Bale Count in Store D | (2,4) Bale Count in Store J |
       | --------------------------------------------------------- |
       | (1,5) Bale Count in Store E | (2,5) Lube Count            |
       | --------------------------------------------------------- |
       | (1,6) Bale Count in Store F | (2,6) Grand Total Bales     |
         --------------------------------------------------------- */


/* Origin */
#define BALE_COUNT_MENU_ORIGIN_X         6
#define BALE_COUNT_MENU_ORIGIN_Y         16


/* Offsets from origin for each grid position:(1,1),(2,1),(1,2),(2,2),(1,3)(2,3),(1,4),(2,4),(1,5),(2,5),(1,6),(2,6) */
#define BALE_COUNT_MENU_ITEM_OFFSET_X      78
#define BALE_COUNT_MENU_ITEM_OFFSET_Y      14


/* Offsets within each grid position */

/* > */
#define SELECTION_POINTER_OFFSET_X  0
#define SELECTION_POINTER_OFFSET_Y  0

/* A - J */
#define BALE_STORE_OFFSET_X  8
#define BALE_STORE_OFFSET_Y  3

/* Pile of bales / Oil can / Large pile of bales */
#define PICTURE_OFFSET_X  8
#define PICTURE_OFFSET_Y  0

/* Tick */
#define CURRENT_SELECTION_INDICATOR_OFFSET_X  33
#define CURRENT_SELECTION_INDICATOR_OFFSET_Y   0

/* 0 - 9999 */
#define COUNT_OFFSET_X  33
#define COUNT_OFFSET_Y  3


/* Dimensions */

/* > */
#define SELECTION_POINTER_DIM_X  4   /* ICON_SELECTION_POINTER bitmap x dim */
#define SELECTION_POINTER_DIM_Y  13  /* ICON_SELECTION_POINTER bitmap y dim */

/* Tick */
#define CURRENT_SELECTION_INDICATOR_DIM_X  12  /* ICON_CURRENT_SELECTION_INDICATOR bitmap x dim */
#define CURRENT_SELECTION_INDICATOR_DIM_Y  13  /* ICON_CURRENT_SELECTION_INDICATOR bitmap y dim */


void screen112( unsigned int lkey )
{
	unsigned int  key_status;
	unsigned char tstring[50], i;
	unsigned char x, y;         /* Grid positions */
	unsigned int x_pos, y_pos;  /* Screen positions [pixels] */
	unsigned char old_selection;
	unsigned char old_BaleStore;


	if( startup != 1 )
	{
		startup = 1;

		draw_frame( adm );


		/* -----------------------------------------------------------
		 * Drawing tabs -- IR & OR tab */

		write_bitmap( adm, TAB,       99, 107, OR );
		write_bitmap( adm, TABESC,   130, 107, OR ); // ESC


		/* -----------------------------------------------------------
		 * Drawing pictures (including bale store letter A - J for bale count) */

		for( y=0; y<6; y++ )
		{
			for( x=0; x<2; x++ )
			{
				/* Drawing Bale Count Menu item in Grid position (x,y) */
				if( (x==1) && (y==4) ) /* (2,5) Lube Count */
				{
					/* Oil can */
					x_pos = BALE_COUNT_MENU_ORIGIN_X + (BALE_COUNT_MENU_ITEM_OFFSET_X * x) + PICTURE_OFFSET_X;
					y_pos = BALE_COUNT_MENU_ORIGIN_Y + (BALE_COUNT_MENU_ITEM_OFFSET_Y * y) + PICTURE_OFFSET_Y;
					write_bitmap( adm, ICON_LUBE_COUNT, x_pos, y_pos, OR );
				}
				else if( (x==1) && (y==5) ) /* (2,6) Grand Total Bales */
				{
					/* Large pile of bales */
					x_pos = BALE_COUNT_MENU_ORIGIN_X + (BALE_COUNT_MENU_ITEM_OFFSET_X * x) + PICTURE_OFFSET_X;
					y_pos = BALE_COUNT_MENU_ORIGIN_Y + (BALE_COUNT_MENU_ITEM_OFFSET_Y * y) + PICTURE_OFFSET_Y;
					write_bitmap( adm, ICON_GRAND_TOTAL_BALES, x_pos, y_pos, OR );
				}
				else /* Bale Count in Store A - J */
				{
					/* Pile of bales */
					x_pos = BALE_COUNT_MENU_ORIGIN_X + (BALE_COUNT_MENU_ITEM_OFFSET_X * x) + PICTURE_OFFSET_X;
					y_pos = BALE_COUNT_MENU_ORIGIN_Y + (BALE_COUNT_MENU_ITEM_OFFSET_Y * y) + PICTURE_OFFSET_Y;
					write_bitmap( adm, ICON_BALE_COUNT, x_pos, y_pos, OR );

					/* A - J */
					x_pos = BALE_COUNT_MENU_ORIGIN_X + (BALE_COUNT_MENU_ITEM_OFFSET_X * x) + BALE_STORE_OFFSET_X;
					y_pos = BALE_COUNT_MENU_ORIGIN_Y + (BALE_COUNT_MENU_ITEM_OFFSET_Y * y) + BALE_STORE_OFFSET_Y;
					i = (x*6) + y; /* 0 - 9 */
					sprintf ( tstring, "%c", ('A' + i) );
					display_f1_text( tstring, TSMAX, x_pos, y_pos, FIXED );
				}
			} /* end for( x=0; x<2; x++) */
		} /* end for( y=0; y<6; y++) */


		/* -----------------------------------------------------------
		 * Grand Total Bales at grid position (2, 6) */

		x = 1 ;
		x_pos = BALE_COUNT_MENU_ORIGIN_X + (BALE_COUNT_MENU_ITEM_OFFSET_X * x) + COUNT_OFFSET_X;
		y = 5 ;
		y_pos = BALE_COUNT_MENU_ORIGIN_Y + (BALE_COUNT_MENU_ITEM_OFFSET_Y * y) + COUNT_OFFSET_Y;
		sprintf ( tstring, "%6lu", pFac.pf.GrandBaleTotal );
		display_f1_text( tstring, TSMAX, x_pos, y_pos, FIXED );

		old_selection = 0xff;
		old_BaleStore = 0xff;
	}
	else
	{
		old_selection = selection;
		old_BaleStore = pFac.pf.BaleStore;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif


	/* -----------------------------------------------------------
	 * Key press */

	key_status = lkey & (SW_NEW | SW_REP);

	if( key_status )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_OR ) // outer-right button (softkey ESC)
		{
			escape = 1;
		}
		else if( lkey == SW_IL ) // inner-left button (softkey ENT)
		{
			if( selection < 10 ) /* Bale count, A - J */
			{
				pFac.pf.BaleStore = selection; /* Reset current bale store to that currently selected */
			}
		}
		else if( lkey == SW_OL ) // outer-left button (softkey RESET)
		{
			if( selection < 10 ) /* Bale count, A - J */
			{
				SelectedBaleStore = selection;
				main_window = screen121;  /* Bale Count Reset Screen */
			}
			else if( selection == 10 ) /* Lube count */
			{
				main_window = screen120;  /* Lube Count Reset Screen */
			}
		}
		else if( lkey == SW_CU ) // navigation-up button (softkey ^)
		{
			if( selection ) /* Bale count, B - J & Lube Count */
			{
				selection--;
			}
		}
		else if( lkey == SW_CD ) // navigation-up button (softkey V)
		{
			if( selection < 11 ) /* Bale count, A - J & Lube Count */
			{
				selection++;
			}
		}
		else if( lkey == SW_CL ) // navigation-up button (softkey <)
		{
			if( selection == 6 )      /* Bale count, G */
				selection = 0; /* Bale count, A */
			else if( selection == 7 ) /* Bale count, H */
				selection = 1; /* Bale count, B */
			else if( selection == 8 ) /* Bale count, I */
				selection = 2; /* Bale count, C */
			else if( selection == 9 ) /* Bale count, J */
				selection = 3; /* Bale count, D */
			else if( selection == 10 ) /* Lube Count */
				selection = 4; /* Bale count, E */
			else if( selection == 11 ) /* Grand Total Bales */
				selection = 5; /* Bale count, F */
		}
		else if( lkey == SW_CR ) // navigation-up button (softkey >)
		{
			if( selection == 0 )      /* Bale count, A */
				selection = 6; /* Bale count, G */
			else if( selection == 1 ) /* Bale count, B */
				selection = 7; /* Bale count, H */
			else if( selection == 2 ) /* Bale count, C */
				selection = 8; /* Bale count, I */
			else if( selection == 3 ) /* Bale count, D */
				selection = 9; /* Bale count, J */
			else if( selection == 4 ) /* Bale count, E */
				selection = 10; /* Lube Count */
			else if( selection == 5 ) /* Bale count, F */
				selection = 11; /* Grand Total Bales */
		}

	} /* end if( key_status ) */


	/* -----------------------------------------------------------
	 * Clearing previously selected bale store -- indicated by a tick */

	if( pFac.pf.BaleStore != old_BaleStore )
	{
		if( old_BaleStore != 0xff ) /* not starting up */
		{
			/* Clear old tick -- was superimposed over highest digits of bale count value */
			x = old_BaleStore / 6 ;
			x_pos = BALE_COUNT_MENU_ORIGIN_X + (BALE_COUNT_MENU_ITEM_OFFSET_X * x) + CURRENT_SELECTION_INDICATOR_OFFSET_X;
			y = old_BaleStore % 6 ;
			y_pos = BALE_COUNT_MENU_ORIGIN_Y + (BALE_COUNT_MENU_ITEM_OFFSET_Y * y) + CURRENT_SELECTION_INDICATOR_OFFSET_Y;
			draw_box( adm, x_pos, y_pos, (x_pos + CURRENT_SELECTION_INDICATOR_DIM_X - 1), (y_pos + CURRENT_SELECTION_INDICATOR_DIM_Y - 1), CLEAR ); /* Clear area containing tick in old position */
		}
	} /* end if( pFac.pf.BaleStore != BaleTotal ) */


	/* -----------------------------------------------------------
	 * Update all values to cover values updated / flashed off */

	for( y=0; y<6; y++)
	{
		for( x=0; x<2; x++)
		{
			/* Drawing Bale Count Menu item in Grid position (x,y) */
			if( (x==1) && (y==4) ) /* (2,5) Lube Count */
			{
				x_pos = BALE_COUNT_MENU_ORIGIN_X + (BALE_COUNT_MENU_ITEM_OFFSET_X * x) + COUNT_OFFSET_X;
				y_pos = BALE_COUNT_MENU_ORIGIN_Y + (BALE_COUNT_MENU_ITEM_OFFSET_Y * y) + COUNT_OFFSET_Y;
				if( pRun.pr.LubeCounter < 0 )
					sprintf( tstring, "   %3u", 0 );
				else
					sprintf( tstring, "   %3u", pRun.pr.LubeCounter );
				display_f1_text( tstring, TSMAX, x_pos, y_pos, FIXED );
			}
			else if( (x==1) && (y==5) ) /* (2,6) Grand Total Bales */
			{
				/* No change */
			}
			else /* Bale Count in Store A - J */
			{
				x_pos = BALE_COUNT_MENU_ORIGIN_X + (BALE_COUNT_MENU_ITEM_OFFSET_X * x) + COUNT_OFFSET_X;
				y_pos = BALE_COUNT_MENU_ORIGIN_Y + (BALE_COUNT_MENU_ITEM_OFFSET_Y * y) + COUNT_OFFSET_Y;
				i = (x*6) + y; /* 0 - 9 */
				sprintf ( tstring, "%6lu", pFac.pf.BaleTotal[i] );
				display_f1_text( tstring, TSMAX, x_pos, y_pos, FIXED );
			}
		} /* end for( x=0; x<2; x++) */
	} /* end for( y=0; y<6; y++) */


	/* -----------------------------------------------------------
	 * Selection pointer */

	if( selection != old_selection )
	{
		if( old_selection != 0xff ) /* not starting up */
		{
			/* Clear old selection */
			x = old_selection / 6 ;
			x_pos = BALE_COUNT_MENU_ORIGIN_X + (BALE_COUNT_MENU_ITEM_OFFSET_X * x) + SELECTION_POINTER_OFFSET_X;
			y = old_selection % 6 ;
			y_pos = BALE_COUNT_MENU_ORIGIN_Y + (BALE_COUNT_MENU_ITEM_OFFSET_Y * y) + SELECTION_POINTER_OFFSET_Y;
			draw_box( adm, x_pos, y_pos, (x_pos + SELECTION_POINTER_DIM_X - 1), (y_pos + SELECTION_POINTER_DIM_Y - 1), CLEAR ); /* Clear area containing pointer in old position */
		}

		/* Draw new selection */
		x = selection / 6 ;
		x_pos = BALE_COUNT_MENU_ORIGIN_X + (BALE_COUNT_MENU_ITEM_OFFSET_X * x) + SELECTION_POINTER_OFFSET_X;
		y = selection % 6 ;
		y_pos = BALE_COUNT_MENU_ORIGIN_Y + (BALE_COUNT_MENU_ITEM_OFFSET_Y * y) + SELECTION_POINTER_OFFSET_Y;
		write_bitmap( adm, ICON_SELECTION_POINTER, x_pos, y_pos, OR ); /* Draw pointer in new position */

	} /* end if( selection != old_selection ) */


	/* -----------------------------------------------------------
	 * Flash currently selected value -- except if Grand Total Bales selected which cannot be changed */

	if( half_second )
	{
		if( selection < 11 ) /* Bale count, A - J  &  Lube count  */
		{
			/* flashing currently selected value off */
			x = selection / 6 ;
			x_pos = BALE_COUNT_MENU_ORIGIN_X + (BALE_COUNT_MENU_ITEM_OFFSET_X * x) + COUNT_OFFSET_X;
			y = selection % 6 ;
			y_pos = BALE_COUNT_MENU_ORIGIN_Y + (BALE_COUNT_MENU_ITEM_OFFSET_Y * y) + COUNT_OFFSET_Y;
			display_f1_text( "      ", TSMAX, x_pos, y_pos, FIXED );
		}
	} /* end if( half_second ) */


	/* -----------------------------------------------------------
	 * Indicating currently selected bale store by a tick */

	/* Draw new selection -- superimposed over highest digits of bale count value */
	x = pFac.pf.BaleStore / 6 ;
	x_pos = BALE_COUNT_MENU_ORIGIN_X + (BALE_COUNT_MENU_ITEM_OFFSET_X * x) + CURRENT_SELECTION_INDICATOR_OFFSET_X;
	y = pFac.pf.BaleStore % 6 ;
	y_pos = BALE_COUNT_MENU_ORIGIN_Y + (BALE_COUNT_MENU_ITEM_OFFSET_Y * y) + CURRENT_SELECTION_INDICATOR_OFFSET_Y;
	write_bitmap( adm, ICON_CURRENT_SELECTION_INDICATOR, x_pos, y_pos, OR ); /* Draw tick in new position */


	/* -----------------------------------------------------------
	 * Drawing tabs -- tabs OL, IL & NAVIGATION */

	if( selection != old_selection )
	{
		/* Clear OL, IL & NAVIGATION tab area -- avoiding deleting section of frame border */
		draw_box( adm, 1, 102, 62+36-1, 102+24-1,   CLEAR );

		switch( selection )
		{
		case 0: /* (1,1) Bale Count in Store A */
			write_bitmap( adm, TABRESET,                    0, 107, OR ); // RESET
			write_bitmap( adm, TAB_MAKE_SELECTION_CURRENT, 31, 107, OR ); // TICK
			write_bitmap( adm, TABNAV_DR,                  62, 102, OR ); // NAVIGATION: Down Right
			break;
		case 1: /* (1,2) Bale Count in Store B */
		case 2: /* (1,3) Bale Count in Store C */
		case 3: /* (1,4) Bale Count in Store D */
		case 4: /* (1,5) Bale Count in Store E */
		case 5: /* (1,6) Bale Count in Store F */
			write_bitmap( adm, TABRESET,                    0, 107, OR ); // RESET
			write_bitmap( adm, TAB_MAKE_SELECTION_CURRENT, 31, 107, OR ); // TICK
			write_bitmap( adm, TABNAV_UDR,                 62, 102, OR ); // NAVIGATION: Up Down Right
			break;
		case 6: /* (2,1) Bale Count in Store G */
		case 7: /* (2,2) Bale Count in Store H */
		case 8: /* (2,3) Bale Count in Store I */
		case 9: /* (2,4) Bale Count in Store J */
			write_bitmap( adm, TABRESET,                    0, 107, OR ); // RESET
			write_bitmap( adm, TAB_MAKE_SELECTION_CURRENT, 31, 107, OR ); // TICK
			write_bitmap( adm, TABNAV_UDL,                 62, 102, OR ); // NAVIGATION: Up Down Left
			break;
		case 10: /* (2,5) Lube Count */
			write_bitmap( adm, TABRESET,    0, 107, OR ); // RESET
			write_bitmap( adm, TAB,        31, 107, OR );
			write_bitmap( adm, TABNAV_UDL, 62, 102, OR ); // NAVIGATION: Up Down Left
			break;
		default: /* 11 - (2,6) Grand Total Bales */
			write_bitmap( adm, TAB,         0, 107, OR );
			write_bitmap( adm, TAB,        31, 107, OR );
			write_bitmap( adm, TABNAV_UL,  62, 102, OR ); // NAVIGATION: Up Left
			break;
		}
	} /* if( selection != old_selection ) */

}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen113
// Description         : Machine Menu - Operator Setup Menu
// -----------------------------------------------------------

 /*  Origin
       x ----------------------------------------------
       | (1,1) Brightness & Contrast Adjust Menu Icon  |
       | ----------------------------------------------
       | (1,2) Time & Date Adjust Menu Icon            |
       | ----------------------------------------------
       | (1,3) Technician Menu Icon                    |
       | ----------------------------------------------
       | (1,4) Diagnostics (Screens Access) Icon       |
        ---------------------------------------------- */

/* Origin */
#define OPERATOR_SETUP_MENU_ICONS_ORIGIN_X         0
#define OPERATOR_SETUP_MENU_ICONS_ORIGIN_Y         0


/* Grid position (1,1) */
#define ICON_BRIGHTNESS_AND_CONTRAST_MENU_OFFSET_X     43
#define ICON_BRIGHTNESS_AND_CONTRAST_MENU_OFFSET_Y      8 + 5 + 4

/* Grid position (1,2) */
#define ICON_TIME_AND_DATE_ADJUST_MENU_OFFSET_X        43
#define ICON_TIME_AND_DATE_ADJUST_MENU_OFFSET_Y        30 + 10 + 4

/* Grid position (1,3) */
#define ICON_TECHNICIAN_MENU_OFFSET_X                  43
#define ICON_TECHNICIAN_MENU_OFFSET_Y                  78 - 27 + 15 + 4

/* Grid position (1,4) */
#define ICON_DIAGNOSTICS_SCREENS_OFFSET_X              43
#define ICON_DIAGNOSTICS_SCREENS_OFFSET_Y              91 - 27 + 20 + 4 - 1


/* Top left icon co-ordinates (x,y) */

#define ICON_BRIGHTNESS_AND_CONTRAST_MENU_X    OPERATOR_SETUP_MENU_ICONS_ORIGIN_X + ICON_BRIGHTNESS_AND_CONTRAST_MENU_OFFSET_X
#define ICON_BRIGHTNESS_AND_CONTRAST_MENU_Y    OPERATOR_SETUP_MENU_ICONS_ORIGIN_Y + ICON_BRIGHTNESS_AND_CONTRAST_MENU_OFFSET_Y

#define ICON_TIME_AND_DATE_ADJUST_MENU_X       OPERATOR_SETUP_MENU_ICONS_ORIGIN_X + ICON_TIME_AND_DATE_ADJUST_MENU_OFFSET_X
#define ICON_TIME_AND_DATE_ADJUST_MENU_Y       OPERATOR_SETUP_MENU_ICONS_ORIGIN_Y + ICON_TIME_AND_DATE_ADJUST_MENU_OFFSET_Y

#define ICON_TECHNICIAN_MENU_X                 OPERATOR_SETUP_MENU_ICONS_ORIGIN_X + ICON_TECHNICIAN_MENU_OFFSET_X
#define ICON_TECHNICIAN_MENU_Y                 OPERATOR_SETUP_MENU_ICONS_ORIGIN_Y + ICON_TECHNICIAN_MENU_OFFSET_Y

#define ICON_DIAGNOSTICS_SCREENS_X             OPERATOR_SETUP_MENU_ICONS_ORIGIN_X + ICON_DIAGNOSTICS_SCREENS_OFFSET_X
#define ICON_DIAGNOSTICS_SCREENS_Y             OPERATOR_SETUP_MENU_ICONS_ORIGIN_Y + ICON_DIAGNOSTICS_SCREENS_OFFSET_Y


/* Bottom right icon co-ordinates (x2,y2) */

#define ICON_DIAGNOSTICS_SCREENS_X2            ICON_DIAGNOSTICS_SCREENS_X + 54 - 1 /* + ICON_DIAGNOSTICS_SCREENS X dim - 1 */
#define ICON_DIAGNOSTICS_SCREENS_Y2            ICON_DIAGNOSTICS_SCREENS_Y + 14 - 1 /* + ICON_DIAGNOSTICS_SCREENS Y dim - 1 */


void screen113( unsigned int lkey )
{
	unsigned int  key_status;
	unsigned char old_selection;

	static unsigned char swtim;


	if( startup != 1 )
	{
		startup = 1;

		draw_frame( adm );


		/* -----------------------------------------------------------
		 * Drawing icons -- none highlighted */

		write_bitmap( adm, ICON_BRIGHTNESS_AND_CONTRAST_MENU, ICON_BRIGHTNESS_AND_CONTRAST_MENU_X,  ICON_BRIGHTNESS_AND_CONTRAST_MENU_Y,  OR );
		write_bitmap( adm, ICON_TIME_AND_DATE_ADJUST_MENU,    ICON_TIME_AND_DATE_ADJUST_MENU_X,     ICON_TIME_AND_DATE_ADJUST_MENU_Y,     OR );
		write_bitmap( adm, ICON_TECHNICIAN_MENU,              ICON_TECHNICIAN_MENU_X,               ICON_TECHNICIAN_MENU_Y,               OR );


		/* -----------------------------------------------------------
		 * Drawing tabs -- OL, IL, IR & OR tab */

		write_bitmap( adm, TAB,        0, 107, OR );
		write_bitmap( adm, TAB,       31, 107, OR );

		write_bitmap( adm, TABENT,    99, 107, OR ); // ENTER
		write_bitmap( adm, TABESC,   130, 107, OR ); // ESC


		old_selection = 0xff;
	}
	else
	{
		old_selection = selection;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif


	/* -----------------------------------------------------------
	 * Key press */

	key_status = lkey & (SW_NEW | SW_REP);

	if( key_status )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_OR ) // outer-right button (softkey ESC)
		{
			escape = 1;
		}
		else if( lkey == SW_IR ) // inner-right button (softkey ENT)
		{
			switch( selection )
			{
			case 0: /* (1,1) - Brightness & Contrast Adjust Menu */
				main_window = screen131; /* Brightness & Contrast Adjust Menu */
				break;

			case 1: /* (1,2) - Time & Date Adjust Menu */
				main_window = screen132; /* Time & Date Adjust Menu */
				break;

			case 2: /* (1,3) - Technician Menu */
#ifndef DEVELOP
				main_window = screen140; /* Pin Entry Display for Technician Menu */
#else
				main_window = screen141; /* Technician Menu */
#endif
				break;
			case 3: /* (1,4) - Diagnostics (Screens Access) */
				main_window = screen150; /* Diagnostics - Digital Inputs */
				break;
			}
		}
		else if( lkey == SW_CU ) // navigation-up button (softkey ^)
		{
			if( selection )
			{
				selection--;
			}
		}
		else if( lkey == SW_CD ) // navigation-up button (softkey V)
		{
			if( selection < 2 )  /* Technician Menu */
			{
				selection++;
			}
			else if( swtim > 25 ) /* 5 s */
			{
				selection = 3; /* Diagnostics Screens */
			}
			else
			{
				swtim++;
			}
		}
		else // No key press
		{
			swtim = 0;
		}

	} /* end if( key_status ) */


	/* -----------------------------------------------------------
	 * Selection pointer */

	if( selection != old_selection )
	{
		/* Clear old selection */
		switch( old_selection )
		{
		case 0: /* (1,1) */
			write_bitmap( adm, ICON_BRIGHTNESS_AND_CONTRAST_MENU, ICON_BRIGHTNESS_AND_CONTRAST_MENU_X, ICON_BRIGHTNESS_AND_CONTRAST_MENU_Y, AND );
			break;
		case 1: /* (1,2) */
			write_bitmap( adm, ICON_TIME_AND_DATE_ADJUST_MENU, ICON_TIME_AND_DATE_ADJUST_MENU_X, ICON_TIME_AND_DATE_ADJUST_MENU_Y, AND );
			break;
		case 2: /* (1,3) */
			write_bitmap( adm, ICON_TECHNICIAN_MENU, ICON_TECHNICIAN_MENU_X, ICON_TECHNICIAN_MENU_Y, AND );
			break;
		case 3: /* (1,4) -- Completely removing Diagnostics Screens Icon */
			draw_box( adm, ICON_DIAGNOSTICS_SCREENS_X, ICON_DIAGNOSTICS_SCREENS_Y, ICON_DIAGNOSTICS_SCREENS_X2, ICON_DIAGNOSTICS_SCREENS_Y2, CLEAR );
			break;
		}

		/* Draw new selection */
		switch( selection )
		{
		case 0: /* (1,1) */
			write_bitmap( adm, ICON_BRIGHTNESS_AND_CONTRAST_MENU_SELECTED, ICON_BRIGHTNESS_AND_CONTRAST_MENU_X, ICON_BRIGHTNESS_AND_CONTRAST_MENU_Y, OR );
			break;
		case 1: /* (1,2) */
			write_bitmap( adm, ICON_TIME_AND_DATE_ADJUST_MENU_SELECTED, ICON_TIME_AND_DATE_ADJUST_MENU_X, ICON_TIME_AND_DATE_ADJUST_MENU_Y, OR );
			break;
		case 2: /* (1,3) */
			write_bitmap( adm, ICON_TECHNICIAN_MENU_SELECTED, ICON_TECHNICIAN_MENU_X, ICON_TECHNICIAN_MENU_Y, OR );
			break;
		case 3: /* (1,4) */
			write_bitmap( adm, ICON_DIAGNOSTICS_SCREENS_SELECTED, ICON_DIAGNOSTICS_SCREENS_X, ICON_DIAGNOSTICS_SCREENS_Y, OR );
			break;
		}
	} /* end if( selection != old_selection ) */


	/* -----------------------------------------------------------
	 * Drawing tabs -- NAVIGATION tab */

	if( selection != old_selection )
	{
		draw_box( adm, 62, 102, 62+36-1, 102+24-1, CLEAR ); // Clear NAVIGATION tab area

		switch( selection )
		{
		case 0: /* (1,1) */
			write_bitmap( adm, TABNAV_D,  62, 102, OR ); // NAVIGATION: Down
			break;

		case 1: /* (1,2) */
			write_bitmap( adm, TABNAV_UD, 62, 102, OR ); // NAVIGATION: Up Down
			break;

		case 2: /* (1,3) */
		case 3: /* (1,4) */
			write_bitmap( adm, TABNAV_U,  62, 102, OR ); // NAVIGATION: Up
			break;
		}
	} /* end if( selection != old_selection ) */

}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen120
// Description         : Lube Count Reset Screen
// -----------------------------------------------------------

/* Top left screen image co-ordinates (x,y) */
#define ICON_LUBE_COUNT_RESET_X      38
#define ICON_LUBE_COUNT_RESET_Y      45

/* Top left text offsets -- offset from screen image co-ordinates (x,y) */
#define ICON_LUBE_COUNT_RESET_TEXT_OFFSET_X      62 - 2
#define ICON_LUBE_COUNT_RESET_TEXT_OFFSET_Y       7

/* Top left text co-ordinates */
#define ICON_LUBE_COUNT_RESET_TEXT_X   ICON_LUBE_COUNT_RESET_X + ICON_LUBE_COUNT_RESET_TEXT_OFFSET_X
#define ICON_LUBE_COUNT_RESET_TEXT_Y   ICON_LUBE_COUNT_RESET_Y + ICON_LUBE_COUNT_RESET_TEXT_OFFSET_Y


void screen120( unsigned int lkey )
{
	unsigned int key_status;
	unsigned char tstring[32];


	if( !startup )
	{
		startup = 1;

		draw_frame( adm );


		/* -----------------------------------------------------------
		 * Drawing screen image -- containing 'Lube Alarm' set in Technician Menu */

		write_bitmap( adm, ICON_LUBE_COUNT_RESET, ICON_LUBE_COUNT_RESET_X, ICON_LUBE_COUNT_RESET_Y, OR );

		sprintf( tstring, "%3u", pFac.pf.GW302_LubeAlarm );
		display_f1_text( tstring, TSMAX, ICON_LUBE_COUNT_RESET_TEXT_X, ICON_LUBE_COUNT_RESET_TEXT_Y, FIXED );


		/* -----------------------------------------------------------
		 * Drawing tabs */

		write_bitmap( adm, TAB,       0,  107, OR );
		write_bitmap( adm, TAB,       31, 107, OR );
		write_bitmap( adm, TABNAV_,   62, 102, OR ); // NAVIGATION: Blank
		write_bitmap( adm, TABTICK,   99, 107, OR ); // TICK
		write_bitmap( adm, TABCROSS, 130, 107, OR ); // CROSS
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	/* -----------------------------------------------------------
	 * Key press */

	key_status = lkey & (SW_NEW | SW_REP);

	if( key_status )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_OR ) // outer-right button (softkey CROSS)
		{
			escape = 1;
		}
		else if( lkey == SW_IR ) // inner-right button (softkey TICK)
		{
			pRun.pr.LubeCounter = pFac.pf.GW302_LubeAlarm;
			SavecalDelayWithinMenus = 1;
			escape = 1;
		}
	}

}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen121
// Description         : Bale Count Reset Screen
// -----------------------------------------------------------

/* Top left screen image co-ordinates (x,y) */
#define ICON_BALE_COUNT_RESET_X      38
#define ICON_BALE_COUNT_RESET_Y      45

/* Top left text offsets -- offset from screen image co-ordinates (x,y) */
#define ICON_BALE_COUNT_RESET_TEXT_OFFSET_X       2
#define ICON_BALE_COUNT_RESET_TEXT_OFFSET_Y       7

/* Top left text co-ordinates */
#define ICON_BALE_COUNT_RESET_TEXT_X   ICON_BALE_COUNT_RESET_X + ICON_BALE_COUNT_RESET_TEXT_OFFSET_X
#define ICON_BALE_COUNT_RESET_TEXT_Y   ICON_BALE_COUNT_RESET_Y + ICON_BALE_COUNT_RESET_TEXT_OFFSET_Y


void screen121( unsigned int lkey )
{
	unsigned int key_status;
	unsigned char tstring[32];


	if( !startup )
	{
		startup = 1;

		draw_frame( adm );


		/* -----------------------------------------------------------
		 * Drawing screen image */

		write_bitmap( adm, ICON_BALE_COUNT_RESET, ICON_BALE_COUNT_RESET_X, ICON_BALE_COUNT_RESET_Y, OR );

		sprintf ( tstring, "%c", ('A' + SelectedBaleStore) ); /* A - J */
		display_f1_text( tstring, TSMAX, ICON_BALE_COUNT_RESET_TEXT_X, ICON_BALE_COUNT_RESET_TEXT_Y, FIXED );


		/* -----------------------------------------------------------
		 * Drawing tabs */

		write_bitmap( adm, TAB,       0,  107, OR );
		write_bitmap( adm, TAB,       31, 107, OR );
		write_bitmap( adm, TABNAV_,   62, 102, OR ); // NAVIGATION: Blank
		write_bitmap( adm, TABTICK,   99, 107, OR ); // TICK
		write_bitmap( adm, TABCROSS, 130, 107, OR ); // CROSS
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	/* -----------------------------------------------------------
	 * Key press */

	key_status = lkey & (SW_NEW | SW_REP);

	if( key_status )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_OR ) // outer-right button (softkey CROSS)
		{
			escape = 1;
		}
		else if( lkey == SW_IR ) // inner-right button (softkey TICK)
		{
			pFac.pf.BaleTotal[SelectedBaleStore] = PF_GW302_BaleTotal[PF_MIN]; // 0UL
			SavecalDelayWithinMenus = 1;
			escape = 1;
		}
	}

}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen123
// Description         : Dia Pot Zero Reset Screen
// -----------------------------------------------------------
#if 0 // Removed -- Dia pot zero value fixed
void screen123( unsigned int lkey )
{
	unsigned int key_status;

	if( !startup )
	{
		startup = 1;

		draw_frame( adm );
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	key_status = lkey & (SW_NEW | SW_REP);
	if( key_status )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( (lkey == SW_OR) || (lkey == SW_CL) ) // outer-right button (softkey CROSS) OR centre-left navigation button (<)
		{
			escape = 1;
		}
		else if( lkey == SW_IR ) // inner-right button (softkey TICK)
		{
			pFac.pf.GW302_DiaPotZero = BaleDiaVolts;
			SavecalDelayWithinMenus = 1;
			escape = 1;
		}
	}

	// Clear display area (above tabs)
	draw_box( adm, 30, 1, 158, 11, CLEAR );
	draw_box( adm, 1, (11+1), 158, (102-1), CLEAR );

	// Current Bale Diameter Voltage
	display_number( BaleDiaVolts, 0.0, 0.0, 1.0, DP1, 147, 4, DW4, RIGHT, &RDSfnt1, NULL, PNULL );
	display_f1_text( "V", TSMAX, 149, 4,  FIXED );

	// Icon & '=' sign
	write_bitmap( adm, Reset_Dia_Pot_Zero, 31, 38, OR );

	// Default value & '?' symbol
	display_number( (float)0UL, 0.0, 0.0, 1.0, DP0, 108, 52, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
	display_f1_text( "?", TSMAX, 111, 52, FIXED );

	// Clear area underneath tabs
	draw_box( adm, 1, 102, 158, 126, CLEAR );

	// Tabs
	write_bitmap( adm, TAB,       0,  107, OR );
	write_bitmap( adm, TAB,       31, 107, OR );
	write_bitmap( adm, TABNAV_L,  62, 102, OR ); // NAVIGATION: left only
	write_bitmap( adm, TABTICK,   99, 107, OR ); // TICK
	write_bitmap( adm, TABCROSS, 130, 107, OR ); // CROSS

}
#endif
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen124
// Description         : Fill Pot Zero Reset Screen (AKA Bale Shape Indicator Zero)
// -----------------------------------------------------------

/* Top left screen image co-ordinates (x,y) */
#define ICON_FILL_POT_ZERO_RESET_X      38
#define ICON_FILL_POT_ZERO_RESET_Y      45


void screen124( unsigned int lkey )
{
	unsigned int key_status;


	if( !startup )
	{
		startup = 1;

		draw_frame( adm );


		/* -----------------------------------------------------------
		 * Drawing screen image */

		write_bitmap( adm, ICON_FILL_POT_ZERO_RESET, ICON_FILL_POT_ZERO_RESET_X, ICON_FILL_POT_ZERO_RESET_Y, OR );


		/* -----------------------------------------------------------
		 * Drawing tabs */

		write_bitmap( adm, TAB,       0,  107, OR );
		write_bitmap( adm, TAB,       31, 107, OR );
		write_bitmap( adm, TABNAV_,   62, 102, OR ); // NAVIGATION: Blank
		write_bitmap( adm, TABTICK,   99, 107, OR ); // TICK
		write_bitmap( adm, TABCROSS, 130, 107, OR ); // CROSS
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	/* -----------------------------------------------------------
	 * Key press */

	key_status = lkey & (SW_NEW | SW_REP);

	if( key_status )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_OR ) // outer-right button (softkey CROSS)
		{
			escape = 1;
		}
		else if( lkey == SW_IR ) // inner-right button (softkey TICK)
		{
			pFac.pf.GW302_BaleShapeIndicatorZero = BaleShapeVolts;
			SavecalDelayWithinMenus = 1;
			escape = 1;
		}
	}


	/* -----------------------------------------------------------
	 * Display voltage to which Bale Shape Indicator (Zero) can be set */

	display_number( BaleShapeVolts, 0.0, 0.0, 1.0, DP1, 147, 4, DW4, RIGHT, &RDSfnt1, NULL, PNULL );
	display_f1_text( "V", TSMAX, 149, 4,  FIXED );

}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen125
// Description         : Net Metres Reset Screen
// -----------------------------------------------------------

/* Top left screen image co-ordinates (x,y) */
#define ICON_NET_METRES_RESET_X      38
#define ICON_NET_METRES_RESET_Y      45


void screen125( unsigned int lkey )
{
	unsigned int key_status;

	if( !startup )
	{
		startup = 1;

		draw_frame( adm );


		/* -----------------------------------------------------------
		 * Drawing screen image */

		write_bitmap( adm, ICON_NET_METRES_RESET, ICON_NET_METRES_RESET_X, ICON_NET_METRES_RESET_Y, OR );


		/* -----------------------------------------------------------
		 * Drawing tabs */

		write_bitmap( adm, TAB,       0,  107, OR );
		write_bitmap( adm, TAB,       31, 107, OR );
		write_bitmap( adm, TABNAV_,   62, 102, OR ); // NAVIGATION: Blank
		write_bitmap( adm, TABTICK,   99, 107, OR ); // TICK
		write_bitmap( adm, TABCROSS, 130, 107, OR ); // CROSS
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	/* -----------------------------------------------------------
	 * Key press */

	key_status = lkey & (SW_NEW | SW_REP);

	if( key_status )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_OR ) // outer-right button (softkey CROSS)
		{
			escape = 1;
		}
		else if( lkey == SW_IR ) // inner-right button (softkey TICK)
		{
			pFac.pf.GW302_TotalNetMetres = PF_GW302_TotalNetMetres[PF_MIN]; // m
			NetMetresAppliedForCount = 0.0; // m -- will be reset on resetting below but resetting here so it is an immediate change
			NetMetresPulseCount = 0UL; // Resetting IP4 pulse count for Net applied (in metres)
			SavecalDelayWithinMenus = 1;
			escape = 1;
		}
	}

}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen131
// Description         : Operator Setup Menu - Brightness & Contrast Adjust Menu
// -----------------------------------------------------------

#define XSIZE 107
#define YSIZE 9
#define XPOS1 40
#define XPOS2 100
#define YPOS1 34
#define YPOS2 59
#define YPOS3 79


void screen131( unsigned int lkey  )
{
	//unsigned char *pd, x_co, change;
	unsigned char xp, change, command[8];
	unsigned int key_status;

	if( !startup )
	{
		startup = 1;

		draw_frame( adm );

		write_bitmap( adm, CONTRST1, 25, YPOS2+2, OR );
		write_bitmap( adm, BRIGHTN1, 25, YPOS3, OR );

		leaf = 1;
		change = 1;
	}
	else
	{
		change = 0;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	key_status = lkey & (SW_NEW | SW_REP);
	if( key_status )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits
		if( lkey == SW_OR )
		{
			escape = 1;
		}
		else if( lkey == SW_CR )
		{
			switch( selection )
			{
			case 0:
				pRun.pr.DayNight = pRun.pr.DayNight ? 0 : 1;
				change = 1;
				break;

			case 1:
				if( pRun.pr.Contrast[pRun.pr.DayNight] < DISPLAY_CONTRAST_MAX )
				{
					pRun.pr.Contrast[pRun.pr.DayNight]++;
				}

				change = 1;
				break;

			case 2:
				if( pRun.pr.Brightness[pRun.pr.DayNight] <= 96 )
				{
					pRun.pr.Brightness[pRun.pr.DayNight] += 4;
				}
				else
				{
					pRun.pr.Brightness[pRun.pr.DayNight] = 100;
				}
				change = 1;
				break;
			}
		}
		else if( lkey == SW_CL )
		{
			switch( selection )
			{
			case 0:
				/*if( pRun.pr.Contrast[pRun.pr.DayNight] < DISPLAY_CONTRAST_MAX )
			{
				pRun.pr.Contrast[pRun.pr.DayNight]++;
			}
    	  change = 1;*/
				pRun.pr.DayNight =   pRun.pr.DayNight ? 0 : 1;
				change = 1;
				break;

			case 1:
				if( pRun.pr.Contrast[pRun.pr.DayNight] > DISPLAY_CONTRAST_MIN )
				{
					pRun.pr.Contrast[pRun.pr.DayNight]--;
				}
				change = 1;
				break;

			case 2:
				if( pRun.pr.Brightness[pRun.pr.DayNight] >= 4 )
				{
					pRun.pr.Brightness[pRun.pr.DayNight] -= 4;
				}
				else
				{
					pRun.pr.Brightness[pRun.pr.DayNight] = 0;
				}
				change = 1;
				break;
			}
		}
		else if( lkey == SW_CU )
		{
			if( selection )
			{
				selection--;
			}
			change = 1;
		}
		else if( lkey == SW_CD )
		{
			if( selection < 2 )
				selection++;

			change = 1;
		}
	}

	// Tabs
	if( change == 1 )
	{
		// Clear area underneath tabs
		draw_box( adm, 1, 102, 158, 126, CLEAR );

		// Draw tabs according to position of selection pointer
		switch( selection )
		{
		case 0:
			write_bitmap( adm, TAB,         0, 107, OR );
			write_bitmap( adm, TAB,        31, 107, OR );
			write_bitmap( adm, TABNAV_DLR, 62, 102, OR ); // NAVIGATION: Down Left Right
			write_bitmap( adm, TAB,        99, 107, OR );
			write_bitmap( adm, TABESC,    130, 107, OR ); // ESC
			break;
		case 1:
			write_bitmap( adm, TAB,          0, 107, OR );
			write_bitmap( adm, TAB,         31, 107, OR );
			write_bitmap( adm, TABNAV_UDLR, 62, 102, OR ); // NAVIGATION: Up Down Left Right
			write_bitmap( adm, TAB,         99, 107, OR );
			write_bitmap( adm, TABESC,     130, 107, OR ); // ESC
			break;
		case 2:
			write_bitmap( adm, TAB,         0, 107, OR );
			write_bitmap( adm, TAB,        31, 107, OR );
			write_bitmap( adm, TABNAV_ULR, 62, 102, OR ); // NAVIGATION: Up Left Right
			write_bitmap( adm, TAB,        99, 107, OR );
			write_bitmap( adm, TABESC,    130, 107, OR ); // ESC
			break;
		}
	}

	if( change == 1 )
	{
		change = 0;

#ifndef WINDOWS
		// if Brightness is Zero PIO mode is used, else peripheral PWM is used, Backlight run at 500Hz
		SetOutputPWM( PWM_BACK, 500, (unsigned short)pRun.pr.Brightness[pRun.pr.DayNight]*100 );

		// set Gain/Pot (2 bytes 0x81 + 'GGPPPPPP')
		command[0] = 0x81;
		command[1] = pRun.pr.Gain | pRun.pr.Contrast[pRun.pr.DayNight];

		// send command
		SPI_SendString( CS_DISCOM, command, 2 );

		// write factors to SPI1 Flash @ 0x000000
		SPI_FLASH_EraseThenWrite( (uint8_t *)&pFac, pFac_BASEADDRESS_MAIN, sizeof( pFac ), 0, 0 );
#endif
	}


	// Pointer
	draw_box( adm, 9, (YPOS1-3 - 6), 13, (YPOS3+9), CLEAR );
	switch( selection )
	{
	case 0: // Day/Night
		write_bitmap( adm, F1_POINT, 9, (YPOS1+6 - 10), OR );
		break;

	case 1: // Contrast bar
		write_bitmap( adm, F1_POINT, 9, (YPOS1+6 + 21), OR );
		break;

	default: // case 2: Brightness bar
		write_bitmap( adm, F1_POINT, 9, (YPOS1+6 + 41), OR );
		break;
	}


	draw_box( adm, XPOS1, YPOS1-(DAYSYM[1]/2)-2,
			XPOS2+DAYSYM[1]+2, YPOS1+(DAYSYM[1]/2)+2, CLEAR );
	if( pRun.pr.DayNight )
	{
		draw_box( adm, XPOS2-3, YPOS1-(DAYSYM[1]/2)-2,
				XPOS2+DAYSYM[1]+1, YPOS1+(DAYSYM[1]/2)+1, EMPTY );
		clip_box( XPOS2-3, YPOS1-(DAYSYM[1]/2)-2,
				XPOS2+DAYSYM[1]+1, YPOS1+(DAYSYM[1]/2)+1 );
	}
	else
	{
		draw_box( adm, XPOS1, YPOS1-(DAYSYM[1]/2)-2,
				XPOS1+DAYSYM[1]+3, YPOS1+(DAYSYM[1]/2)+1, EMPTY );
		clip_box( XPOS1, YPOS1-(DAYSYM[1]/2)-2,
				XPOS1+DAYSYM[1]+3, YPOS1+(DAYSYM[1]/2)+1 );
	}

	write_bitmap( adm, DAYSYM, XPOS1+2, YPOS1-(DAYSYM[1]/2), OR );
	write_bitmap( adm, NIGHTSYM, XPOS2+2, YPOS1-(NIGHTSYM[1]/2), OR );

	// contrast bar
	xp = (unsigned char)((((unsigned int)pRun.pr.Contrast[pRun.pr.DayNight] - DISPLAY_CONTRAST_MIN) * (139 - 42)) / DISPLAY_CONTRAST_MAX) + 42;
	draw_box( adm, 42, 68-6, 139, 68-6 + 7, CLEAR );
	draw_box( adm, 42, 68-6,  xp, 68-6 + 7, FULL );
	draw_box( adm, xp, 68-6, 139, 68-6 + 7, EMPTY );
	clip_box(      42, 68-6, 139, 68-6 + 7 );

	// brightness bar
	xp = (unsigned char)((((unsigned int)pRun.pr.Brightness[pRun.pr.DayNight] - 0) * (139 - 42)) / 100) + 42;
	draw_box( adm, 42, 87-6, 139, 87-6 + 7, CLEAR );
	draw_box( adm, 42, 87-6,  xp, 87-6 + 7, FULL );
	draw_box( adm, xp, 87-6, 139, 87-6 + 7, EMPTY );
	clip_box(      42, 87-6, 139, 87-6 + 7 );

}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen132
// Description         : Operator Setup Menu - Time & Date Adjust Menu
// -----------------------------------------------------------

/*  Origin
       x ----------------------------------------------------------
       |  CLOCK ICON          HH:MM                                |
       |                      DD/MM/YYYY                           |
         --------------------------------------------------------- */


/* Origin */
#define TIME_AND_DATE_ADJUST_MENU_ORIGIN_X         0
#define TIME_AND_DATE_ADJUST_MENU_ORIGIN_Y         0


/* Offsets */

/* Clock picture */
#define CLOCK_PICTURE_OFFSET_X          31
#define CLOCK_PICTURE_OFFSET_Y          38

/* "Hours:Minutes" text string */
#define HOURS_MINUTES_TEXT_OFFSET_X     60
#define HOURS_MINUTES_TEXT_OFFSET_Y     44

/* "Date/Month/Year" string */
#define DATE_MONTH_YEAR_TEXT_OFFSET_X   60
#define DATE_MONTH_YEAR_TEXT_OFFSET_Y   63

/* Hours _ */
#define HOURS_UNDERLINE_OFFSET_X        60
#define HOURS_UNDERLINE_OFFSET_Y        53

/* Minutes _ */
#define MINUTES_UNDERLINE_OFFSET_X      75
#define MINUTES_UNDERLINE_OFFSET_Y      53

/* Date _ */
#define DATE_UNDERLINE_OFFSET_X         60
#define DATE_UNDERLINE_OFFSET_Y         72

/* Month _ */
#define MONTH_UNDERLINE_OFFSET_X        78
#define MONTH_UNDERLINE_OFFSET_Y        72

/* Year _ */
#define YEAR_UNDERLINE_OFFSET_X         96
#define YEAR_UNDERLINE_OFFSET_Y         72


/* Top left screen position co-ordinates (x,y) */

/* Clock picture */
#define CLOCK_PICTURE_X          TIME_AND_DATE_ADJUST_MENU_ORIGIN_X + CLOCK_PICTURE_OFFSET_X
#define CLOCK_PICTURE_Y          TIME_AND_DATE_ADJUST_MENU_ORIGIN_Y + CLOCK_PICTURE_OFFSET_Y

/* "Hours:Minutes" text string */
#define HOURS_MINUTES_TEXT_X     TIME_AND_DATE_ADJUST_MENU_ORIGIN_X + HOURS_MINUTES_TEXT_OFFSET_X
#define HOURS_MINUTES_TEXT_Y     TIME_AND_DATE_ADJUST_MENU_ORIGIN_Y + HOURS_MINUTES_TEXT_OFFSET_Y

/* "Date/Month/Year" string */
#define DATE_MONTH_YEAR_TEXT_X   TIME_AND_DATE_ADJUST_MENU_ORIGIN_X + DATE_MONTH_YEAR_TEXT_OFFSET_X
#define DATE_MONTH_YEAR_TEXT_Y   TIME_AND_DATE_ADJUST_MENU_ORIGIN_Y + DATE_MONTH_YEAR_TEXT_OFFSET_Y

/* Hours _ */
#define HOURS_UNDERLINE_X        TIME_AND_DATE_ADJUST_MENU_ORIGIN_X + HOURS_UNDERLINE_OFFSET_X
#define HOURS_UNDERLINE_Y        TIME_AND_DATE_ADJUST_MENU_ORIGIN_Y + HOURS_UNDERLINE_OFFSET_Y

/* Minutes _ */
#define MINUTES_UNDERLINE_X      TIME_AND_DATE_ADJUST_MENU_ORIGIN_X + MINUTES_UNDERLINE_OFFSET_X
#define MINUTES_UNDERLINE_Y      TIME_AND_DATE_ADJUST_MENU_ORIGIN_Y + MINUTES_UNDERLINE_OFFSET_Y

/* Date _ */
#define DATE_UNDERLINE_X         TIME_AND_DATE_ADJUST_MENU_ORIGIN_X + DATE_UNDERLINE_OFFSET_X
#define DATE_UNDERLINE_Y         TIME_AND_DATE_ADJUST_MENU_ORIGIN_Y + DATE_UNDERLINE_OFFSET_Y

/* Month _ */
#define MONTH_UNDERLINE_X        TIME_AND_DATE_ADJUST_MENU_ORIGIN_X + MONTH_UNDERLINE_OFFSET_X
#define MONTH_UNDERLINE_Y        TIME_AND_DATE_ADJUST_MENU_ORIGIN_Y + MONTH_UNDERLINE_OFFSET_Y

/* Year _ */
#define YEAR_UNDERLINE_X         TIME_AND_DATE_ADJUST_MENU_ORIGIN_X + YEAR_UNDERLINE_OFFSET_X
#define YEAR_UNDERLINE_Y         TIME_AND_DATE_ADJUST_MENU_ORIGIN_Y + YEAR_UNDERLINE_OFFSET_Y


void screen132( unsigned int lkey )
{
	unsigned int key_status;
	unsigned char old_selection;
	unsigned char ChangeTime;
	unsigned char tstring[32];

	static unsigned short int t_year;
	static unsigned char t_sec, t_min, t_hrs, t_date, t_day, t_month; // t_sec & t_day not displayed

	if( !startup )
	{
		startup = 1;

		draw_frame( adm );


		/* -----------------------------------------------------------
		 * Drawing tabs -- OL, IL, IR & OR tab */

		write_bitmap( adm, TABMINUS,  0, 107, OR ); // -
		write_bitmap( adm, TABPLUS,  31, 107, OR ); // +

		write_bitmap( adm, TAB,      99, 107, OR );
		write_bitmap( adm, TABESC,  130, 107, OR ); // ESC


		/* -----------------------------------------------------------
		 * Displaying clock picture */

		write_bitmap( adm, ICON_CLOCK_AT_TEN_TO_TWO, CLOCK_PICTURE_X, CLOCK_PICTURE_Y, OR );


		leaf = 1;
		ChangeTime = 0;

		old_selection = 0xff;
	}
	else
	{
		old_selection = selection;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	/* -----------------------------------------------------------
	 * Set local time variables */

	t_sec = bcdtob( ClockRam[0] );
	t_min = bcdtob( ClockRam[1] );
	t_hrs = bcdtob( ClockRam[2] );
	t_day = bcdtob( ClockRam[3] );
	t_date = bcdtob( ClockRam[4] );
	t_month = bcdtob( ClockRam[5] & 0x1f );
	t_year = ((ClockRam[5] & 0x80) ? 2000 : 2100) + bcdtob( ClockRam[6] );


	/* -----------------------------------------------------------
	 * Key press */

	key_status = !editing && (lkey & (SW_NEW | SW_REP));

	if( key_status )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_OR ) // outer-right button (softkey ESC)
		{
			escape = 1;
		}
		else if( lkey == SW_OL ) // outer-left button (softkey -)
		{
			switch( selection )
			{
			case 0:
				if( t_hrs )
				{
					t_hrs--;
					ChangeTime = 1;
				}
				break;

			case 1:
				if( t_min )
				{
					t_min--;
					ChangeTime = 1;
				}
				break;

			case 2:
				if( t_date > 1 )
				{
					t_date--;
					ChangeTime = 1;
				}
				break;

			case 3:
				if( t_month > 1 )
				{
					t_month--;
					ChangeTime = 1;
				}
				break;

			case 4:
				if( t_year > 2000 )
				{
					t_year--;
					ChangeTime = 1;
				}
				break;
			}
			savecal = 1;
		}
		else if( lkey == SW_IL ) // inner-left button (softkey +)
		{
			switch( selection )
			{
			case 0:
				if( t_hrs < 23 )
				{
					t_hrs++;
					ChangeTime = 1;
				}
				break;

			case 1:
				if( t_min < 59 )
				{
					t_min++;
					ChangeTime = 1;
				}
				break;

			case 2:
				if( t_date < 31 )
				{
					t_date++;
					ChangeTime = 1;
				}
				break;

			case 3:
				if( t_month < 12 )
				{
					t_month++;
					ChangeTime = 1;
				}
				break;

			case 4:
				if( t_year < 2099 )
				{
					t_year++;
					ChangeTime = 1;
				}
				break;
			}
			savecal = 1;
		}
		else if( lkey == SW_CU ) // navigation-up button (softkey ^)
		{
			switch( selection )
			{
			case 0: /* hours (t_hrs) */
				break;
			case 1: /* minutes (t_min) */
				break;
			case 2: /* _date (t_date) */
				selection = 0; /* hours (t_hrs) */
				break;
			case 3: /* month (t_month) */
				selection = 1; /* minutes (t_min) */
				break;
			case 4: /* year (t_year) */
				selection = 1; /* minutes (t_min) */
				break;
			}
		}
		else if( lkey == SW_CD ) // navigation-up button (softkey V)
		{
			switch( selection )
			{
			case 0: /* hours (t_hrs) */
				selection = 2; /* _date (t_date) */
				break;
			case 1: /* minutes (t_min) */
				selection = 3; /* month (t_month) */
				break;
			case 2: /* _date (t_date) */
				break;
			case 3: /* month (t_month) */
				break;
			case 4: /* year (t_year) */
				break;
			}
		}
		else if( lkey == SW_CL ) // navigation-up button (softkey <)
		{
			switch( selection )
			{
			case 0: /* hours (t_hrs) */
				break;
			case 1: /* minutes (t_min) */
				selection = 0; /* hours (t_hrs) */
				break;
			case 2: /* _date (t_date) */
				selection = 1; /* minutes (t_min) */
				break;
			case 3: /* month (t_month) */
				selection = 2; /* _date (t_date) */
				break;
			case 4: /* year (t_year) */
				selection = 3; /* month (t_month) */
				break;
			}
		}
		else if( lkey == SW_CR ) // navigation-up button (softkey >)
		{
			switch( selection )
			{
			case 0: /* hours (t_hrs) */
				selection = 1; /* minutes (t_min) */
				break;
			case 1: /* minutes (t_min) */
				selection = 2; /* _date (t_date) */
				break;
			case 2: /* _date (t_date) */
				selection = 3; /* month (t_month) */
				break;
			case 3: /* month (t_month) */
				selection = 4; /* year (t_year) */
				break;
			case 4: /* year (t_year) */
				break;
			}
		}
	} /* end if( key_status ) */


	/* -----------------------------------------------------------
	 * Displaying time & date -- all values displayed not limiting to those that have changed */

	sprintf( tstring, "%02u:%02u", t_hrs, t_min ); /* '%02u:%02u' */
	display_f1_text( tstring, TSMAX, HOURS_MINUTES_TEXT_X, HOURS_MINUTES_TEXT_Y, FIXED | LEFT );
	sprintf( tstring, "%02u/%02u/%04u", t_date, t_month, t_year ); /* '%02u/%02u/%04u' */
	display_f1_text( tstring, TSMAX, DATE_MONTH_YEAR_TEXT_X, DATE_MONTH_YEAR_TEXT_Y, FIXED | LEFT );


	/* -----------------------------------------------------------
	 * Flashing currently selected value */

	if( half_second )
	{
		/* flashing currently selected value off */
		switch( selection )
		{
		case 0: /* hours (t_hrs) flashed off */
			sprintf( tstring, "  :%02u", t_min ); /* '  :%02u' */
			display_f1_text( tstring, TSMAX, HOURS_MINUTES_TEXT_X, HOURS_MINUTES_TEXT_Y, FIXED | LEFT );
			break;
		case 1: /* minutes (t_min) flashed off */
			sprintf( tstring, "%02u:  ", t_hrs ); /* '%02u:  ' */
			display_f1_text( tstring, TSMAX, HOURS_MINUTES_TEXT_X, HOURS_MINUTES_TEXT_Y, FIXED | LEFT );
			break;
		case 2: /* _date (t_date) flashed off */
			sprintf( tstring, "  /%02u/%04u", t_month, t_year ); /* '  /%02u/%04u' */
			display_f1_text( tstring, TSMAX, DATE_MONTH_YEAR_TEXT_X, DATE_MONTH_YEAR_TEXT_Y, FIXED | LEFT );
			break;
		case 3: /* month (t_month) flashed off */
			sprintf( tstring, "%02u/  /%04u", t_date, t_year ); /* '%02u/  /%04u' */
			display_f1_text( tstring, TSMAX, DATE_MONTH_YEAR_TEXT_X, DATE_MONTH_YEAR_TEXT_Y, FIXED | LEFT );
			break;
		case 4: /* year (t_year) flashed off */
			sprintf( tstring, "%02u/%02u/    ", t_date, t_month ); /* '%02u/%02u/    ' */
			display_f1_text( tstring, TSMAX, DATE_MONTH_YEAR_TEXT_X, DATE_MONTH_YEAR_TEXT_Y, FIXED | LEFT );
			break;
		}
	} /* end if( half_second ) */


	/* -----------------------------------------------------------
	 * Underlining currently selected value */

	if( selection != old_selection )
	{
		if( old_selection != 0xff ) /* not starting up */
		{
			/* Undrawing underline for previously selected value */
			switch( old_selection )
			{
			case 0: /* hours (t_hrs) */
				write_bitmap( adm, UNDERLINE_TWO_DIGIT, HOURS_UNDERLINE_X, HOURS_UNDERLINE_Y, INV );
				break;
			case 1: /* minutes (t_min) */
				write_bitmap( adm, UNDERLINE_TWO_DIGIT, MINUTES_UNDERLINE_X, MINUTES_UNDERLINE_Y, INV );
				break;
			case 2: /* _date (t_date) */
				write_bitmap( adm, UNDERLINE_TWO_DIGIT, DATE_UNDERLINE_X, DATE_UNDERLINE_Y, INV );
				break;
			case 3: /* month (t_month) */
				write_bitmap( adm, UNDERLINE_TWO_DIGIT, MONTH_UNDERLINE_X, MONTH_UNDERLINE_Y, INV );
				break;
			case 4: /* year (t_year) */
				write_bitmap( adm, UNDERLINE_FOUR_DIGIT, YEAR_UNDERLINE_X, YEAR_UNDERLINE_Y, INV );
				break;
			}
		}

		/* Drawing underline for currently selected value */
		switch( selection )
		{
		case 0: /* hours (t_hrs) */
			write_bitmap( adm, UNDERLINE_TWO_DIGIT, HOURS_UNDERLINE_X, HOURS_UNDERLINE_Y, OR );
			break;
		case 1: /* minutes (t_min) */
			write_bitmap( adm, UNDERLINE_TWO_DIGIT, MINUTES_UNDERLINE_X, MINUTES_UNDERLINE_Y, OR );
			break;
		case 2: /* _date (t_date) */
			write_bitmap( adm, UNDERLINE_TWO_DIGIT, DATE_UNDERLINE_X, DATE_UNDERLINE_Y, OR );
			break;
		case 3: /* month (t_month) */
			write_bitmap( adm, UNDERLINE_TWO_DIGIT, MONTH_UNDERLINE_X, MONTH_UNDERLINE_Y, OR );
			break;
		case 4: /* year (t_year) */
			write_bitmap( adm, UNDERLINE_FOUR_DIGIT, YEAR_UNDERLINE_X, YEAR_UNDERLINE_Y, OR );
			break;
		}
	}


	/* -----------------------------------------------------------
	 * Drawing tabs -- NAVIGATION tab */

	if( selection != old_selection )
	{
		draw_box( adm, 62, 102, 62+36-1, 102+24-1, CLEAR ); // Clear NAVIGATION tab area

		switch( selection )
		{
		case 0: /* hours (t_hrs) */
			write_bitmap( adm, TABNAV_DR,  62, 102, OR ); // NAVIGATION: Down Right
			break;
		case 1: /* minutes (t_min) */
			write_bitmap( adm, TABNAV_DLR, 62, 102, OR ); // NAVIGATION: Down Left Right
			break;
		case 2: /* _date (t_date) */
			write_bitmap( adm, TABNAV_ULR, 62, 102, OR ); // NAVIGATION: Up Left Right
			break;
		case 3: /* month (t_month) */
			write_bitmap( adm, TABNAV_ULR, 62, 102, OR ); // NAVIGATION: Up Left Right
			break;
		case 4: /* year (t_year) */
			write_bitmap( adm, TABNAV_UL,  62, 102, OR ); // NAVIGATION: Up Left
			break;
		}
	} /* end if( selection != old_selection ) */


	/* -----------------------------------------------------------
	 * Setting time */

	if(	ChangeTime )
	{
		ChangeTime = 0;
#ifndef WINDOWS


			// only reset seconds if adjusting minutes
			tstring[0] = (selection == 1) ? 0x00 : btobcd( t_sec ); // seconds
			tstring[1] = btobcd( t_min ); // minutes
			tstring[2] = btobcd( t_hrs ); // hours
			tstring[3] = btobcd( t_day ); // day of week
			tstring[4] = btobcd( t_date ); // day of month
			tstring[5] = ((t_year > 2099) ? 0x00 : 0x80) | btobcd( t_month ); // century & month
			tstring[6]  = btobcd( (unsigned char)(t_year % 100) ); // year

			SetCurrentTime( &tstring[0] );

#else
		ClockRam[0] = (selection == 1) ? 0x00 : btobcd( t_sec ); // seconds
#endif
	}

}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen133
// Description         : Language Menu
// -----------------------------------------------------------
#if 0 // Removed -- Languages not needed with iconized menus
void screen133( unsigned int lkey )
{
	if( !startup )
	{
		startup = 1;

		draw_frame( adm );

		display_f1_text( ptText[16], TSMAX, 80, 8, FIXED | CENTER | UNDERLINED ); // "Language"

		display_f1_text( ptText[22], TSMAX, 28, 25, FIXED );  // "English"
		display_f1_text( "------",   TSMAX, 28, 40, FIXED );
		display_f1_text( "------",   TSMAX, 28, 55, FIXED );
		display_f1_text( "------",   TSMAX, 28, 70, FIXED );


		write_bitmap( adm, TAB,       0, 107, OR );
		write_bitmap( adm, TAB,      31, 107, OR );
		write_bitmap( adm, TABNAV00, 62, 102, OR );
		write_bitmap( adm, TAB,      99, 107, OR );
		write_bitmap( adm, TABESC,  130, 107, OR );

		leaf = 1;

		selection = pFac.pf.Lang;
	}

	ClockDisplay( CL0CK_ICON_OFF );

	if( !editing && (lkey & (SW_NEW | SW_REP)) )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits
		if( lkey == SW_OL )
		{
			//nextwindow = NEXTWINDOW_NOESCAPE;
		}
		else if( lkey == SW_CU )
		{
			/*if( selection )
			{
				selection--;
			}*/
		}
		else if( lkey == SW_CD )
		{
			/*if( selection < (MAXLANGUAGES - 1) )
			{
				selection++;
			}
			else
			{
				selection = 0;
			}*/
		}
		else if( (lkey == SW_OR) || (lkey == SW_CL) )
		{
			escape = 1;
		}
		else if( lkey == SW_IR )
		{
			/*pFac.pf.Lang = selection;
			ptText = TextString[pFac.pf.Lang];
			savecal = 1;*/
		}
	}

	/*if( !key && nextwindow )
	{
		window = screen431;
	}*/
	/*
	draw_box( adm, 100, 21-1, 100+8, 72+9, CLEAR );
	write_bitmap( adm, Tick, 100, 21-1+(15*pFac.pf.Lang), OR );
	 */
	draw_box( adm, 16, 25 , 20, 95, CLEAR );
	write_bitmap( adm, F1_POINT, 16, 25+(15*selection), OR );
}
#endif
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen140
// Description         : Technician Menu - PIN entry
// -----------------------------------------------------------
void screen140( unsigned int lkey )
{
	static unsigned char dig[5];

	struct PinWin const *Pop;

	unsigned short int pin;
	unsigned char *pd, c;

	Pop = &PinOption;

	if( startup != 1 )
	{
		startup = 1;

		draw_frame( adm );

		write_bitmap( adm, ICON_TECHNICIAN_MENU, 50, 14, OR ); // Spanner picture

		display_f1_text( "ENTER PIN CODE", TSMAX, 80, 45, FIXED | CENTER );

		write_bitmap( adm, TABMINUS, 0, 107, OR ); // -
		write_bitmap( adm, TABPLUS, 31, 107, OR ); // +
		write_bitmap( adm, TABNAV_, 62, 102, OR ); // NAVIGATION: blank
		write_bitmap( adm, TABENT,  99, 107, OR ); // ENT
		write_bitmap( adm, TABESC, 130, 107, OR ); // ESC

		dig[0] = '0';
		dig[1] = '0';
		dig[2] = '0';
		dig[3] = '0';
		dig[4] = 0x00;

		leaf = 1; /* don't save this screen */
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	if(lkey & (SW_NEW | SW_REP))
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_IL )
		{
			switch( selection )
			{
			case 0:
			case 1:
			case 2:
			case 3:
				pd = &dig[selection];
				if( *pd < '9' )
				{
					(*pd)++;
				}
				else
				{
					*pd = '0';
				}
				break;
			}
		}
		else if( lkey == SW_OL )
		{
			switch( selection )
			{
			case 0:
			case 1:
			case 2:
			case 3:
				pd = &dig[selection];
				if( *pd > '0' )
				{
					(*pd)--;
				}
				else
				{
					*pd = '9';
				}
				break;
			}
		}
		else if( lkey == SW_OR )
		{
			escape = 1;
		}
		else if( lkey == SW_IR )
		{
			if( selection < 3 )
			{
				selection++;
			}
			else
			{
				pin = Pop->PinNumber;
				c = (pin % 10) + '0';
				if( dig[3] == c )
				{
					pin /= 10;
					c = (pin % 10) + '0';
					if( dig[2] == c )
					{
						pin /= 10;
						c = (pin % 10) + '0';
						if( dig[1] == c )
						{
							pin /= 10;
							c = (pin % 10) + '0';
							if( dig[0] == c )
							{
								main_window = Pop->PinWindow; /* Pin Secured Window */
							}
						}
					}
				}
				else
				{
					selection = 0;
				}
			}
		}
	}

	display_f1_text( dig, TSMAX, 68, 60, FIXED );

	draw_line( adm, 68, 68, 68+22, 68, CLEAR );
	c = 68+(selection*6);
	if( !half_second )
	{
		draw_line( adm, c, 68, c+4, 68, OR );
	}
}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen141
// Description         : Technician Menu
// -----------------------------------------------------------
void screen141( unsigned int lkey )
{
	char update_disp;

	static unsigned char swtim;


	if( !startup )
	{
		startup = 1;

		draw_frame( adm );

		display_f1_text( ptText[23], TSMAX, 80, 7, FIXED | CENTER | UNDERLINED ); // "Technician Menu"

		// Options
		display_f1_text( "  Baler Full",     TSMAX, 24, 21, FIXED );
		display_f1_text( "  Netting Setup",  TSMAX, 24, 31, FIXED );
		display_f1_text( "  Twine Setup", TSMAX, 24, 41, FIXED );
		display_f1_text( "  Diameter Setup", TSMAX, 24, 51, FIXED );
		display_f1_text( "  Density Setup",  TSMAX, 24, 61, FIXED );
		display_f1_text( "  Knives Setup",   TSMAX, 24, 71, FIXED );
		display_f1_text( "  Reset Defaults", TSMAX, 24, 81, FIXED );
		display_f1_text( " ",                TSMAX, 24, 91, FIXED );

		update_disp = 1;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	if( !editing && (lkey & (SW_NEW | SW_REP)) )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( (lkey == SW_OL) || (lkey == SW_IL) )
		{
			// Do nothing
		}
		else if( lkey == SW_CU )
		{
			if( selection ) // not first option selected
			{
				selection--;
				if( selection == 6 ) // skip
					selection = 5;
				update_disp = 1; // Update display: tabs & selection pointer
			}
		}
		else if( lkey == SW_CD )
		{
			if( selection < 6 ) // not last [visible] option selected
			{
				selection++;
			}
			else if( swtim > 30 )
			{
				selection = 7; // Goto hidden menu -- selection 6 skipped
			}
			else
			{
				swtim++;
			}
			update_disp = 1; // Update display: draw/undraw hidden menu option
		}
		else if( (lkey == SW_OR) || (lkey == SW_CL) )
		{
			escape = 1;
		}
		else if( (lkey == SW_IR) || (lkey == SW_CR) )
		{
			switch( selection )
			{
			case 0:
				main_window = screen142; // Technician Menu - Baler Full
				break;

			case 1:
				main_window = screen143; // Technician Menu - Netting Setup
				break;

			case 2:
				main_window = TwineSetupScreen; // Technician Menu - Twine Setup
				break;


			case 3:
				main_window = screen144; // Technician Menu - Diameter Setup
				break;


			case 4:
				main_window = screen145; // Technician Menu - Density Setup
				break;

			case 5:
				main_window = screen146; // Technician Menu - Knives Setup
				break;

			case 6:
				main_window = screen147; // Technician Menu - Reset Defaults
				break;

			case 7:
				main_window = screen148; // Technician Menu - Formation 2
				break;

			}
		}
		else
		{
			swtim = 0;
		}
	}
	else if( !editing ) // Not editing and no keys being pressed
	{
		// Saving Programmable Factors on releasing the keys for greater than 300 ms
		if( SavecalDelayOnMainDisplay || SavecalDelayWithinMenus )
		{
			if( swrel > 3 ) // > 3 x 100 ms
			{
				SavecalDelayOnMainDisplay = 0;
				SavecalDelayWithinMenus = 0;
				savecal = 1;
			}
		}
	}

	// Update display: draw/undraw hidden menu option
	if( update_disp )
	{
		if( selection == 7 )
		{
			display_f1_text( "  Formation 2", TSMAX, 24, 91, FIXED );
		}
		else // hidden menu option not selected
		{
			draw_box( adm, 24, 90, 24+(13*6), 90+8, CLEAR );
		}
	}

	// Update display: tabs & selection pointer
	if( update_disp )
	{
		// Tabs
		draw_box( adm, 1, 102, 158, 126, CLEAR ); // Clear area underneath tabs
		switch( selection ) // Drawn according to position of selection pointer
		{

		case 0:
			write_bitmap( adm, TAB,          0, 107, OR );
			write_bitmap( adm, TAB,         31, 107, OR );
			write_bitmap( adm, TABNAV_DLR,  62, 102, OR ); // NAVIGATION: Down Left Right
			write_bitmap( adm, TABENT,      99, 107, OR ); // ENT
			write_bitmap( adm, TABESC,     130, 107, OR ); // ESC
			break;

		case 1:
		case 2:
		case 3:
		case 4:
			write_bitmap( adm, TAB,          0, 107, OR );
			write_bitmap( adm, TAB,         31, 107, OR );
			write_bitmap( adm, TABNAV_UDLR, 62, 102, OR ); // NAVIGATION: Up Down Left Right
			write_bitmap( adm, TABENT,      99, 107, OR ); // ENT
			write_bitmap( adm, TABESC,     130, 107, OR ); // ESC
			break;

		case 5:
		case 6:
		case 7:
			write_bitmap( adm, TAB,          0, 107, OR );
			write_bitmap( adm, TAB,         31, 107, OR );
			write_bitmap( adm, TABNAV_ULR,  62, 102, OR ); // NAVIGATION: Up Left Right
			write_bitmap( adm, TABENT,      99, 107, OR ); // ENT
			write_bitmap( adm, TABESC,     130, 107, OR ); // ESC
			break;
		}

		// Selection pointer
		draw_box( adm, 16, 21 , 20, 100, CLEAR );
		write_bitmap( adm, F1_POINT, 16, 21+(10*selection), OR );

		update_disp = 0;
	}

}
// -----------------------------------------------------------

unsigned short int tempshi;
// -----------------------------------------------------------
// Function Name       : TwineSetupScreen
// Description         : Technician Menu - Twine Setup
// -----------------------------------------------------------
void TwineSetupScreen( unsigned int lkey )
{

	char update_disp;
	char update_selection;
	char update_values;

	static char page;
	if( !startup )
	{
		startup = 1;

		draw_frame( adm );

		display_f1_text( "Twine Setup", TSMAX, 80, 7, FIXED | CENTER | UNDERLINED );

		page = 0;
		selection = 0;
		update_disp = 1;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	if( !editing && (lkey & (SW_NEW | SW_REP)) )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_OL ) // -
		{
			if( page == 0 )
			{
				switch( selection )
				{
					case 0:
						if( pFac.pf.GW302_TwineOption == TO_OFF )
							pFac.pf.GW302_TwineOption = TO_ON;
						else /* pFac.pf.GW302_NetSystem == TO_ON */
						{
							pFac.pf.GW302_TwineOption = TO_OFF;
							HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_OFF; //Feed Motor
							HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF; //Track Motor Forward
							HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF; //Track Motor Reverse
							HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_OFF; //Track Motor Reverse

							MWarn &= ~MW_SINGLE_NO_TWINE;
							MWarn &= ~MW_DOUBLE_NO_TWINE;
							MWarn &= ~MW_NO_CAN_COM;
							MWarn &= ~MW_TRACK_POSITION;
							MWarn &= ~MW_NO_IP4_PULSES;

							TwineStage = TWINE_STAGE_12;
						}

						pFac.pf.GW302_NetTwineOption = TO_NET; /* 'Net/Twine' option reset to Net on turning 'Twine Option' */
						TwineAutoManMode = 0; //reset to man mode
					break;

					case 1:
						pFac.pf.GW302_TwineFeedDiameter -= PF_GW302_TwineFeedDiameter[PF_STEP];
						if( pFac.pf.GW302_TwineFeedDiameter < PF_GW302_TwineFeedDiameter[PF_MIN] )
							pFac.pf.GW302_TwineFeedDiameter = PF_GW302_TwineFeedDiameter[PF_MIN]; // Limiting to minimum value
					break;

					case 2:
						pFac.pf.GW302_TwineFeedTime -= PF_GW302_TwineFeedTime[PF_STEP];
						if( pFac.pf.GW302_TwineFeedTime < PF_GW302_TwineFeedTime[PF_MIN] )
							pFac.pf.GW302_TwineFeedTime = PF_GW302_TwineFeedTime[PF_MIN]; // Limiting to minimum value


					break;

					case 3:
						pFac.pf.GW302_TwineFeedSpeed -= PF_GW302_TwineFeedSpeed[PF_STEP];
						if( pFac.pf.GW302_TwineFeedSpeed < PF_GW302_TwineFeedSpeed[PF_MIN] )
							pFac.pf.GW302_TwineFeedSpeed = PF_GW302_TwineFeedSpeed[PF_MIN]; // Limiting to minimum value


					break;

					case 4:
						pFac.pf.GW302_TwineTrackDelay -= PF_GW302_TwineTrackDelay[PF_STEP];
						if( pFac.pf.GW302_TwineTrackDelay < PF_GW302_TwineTrackDelay[PF_MIN] )
							pFac.pf.GW302_TwineTrackDelay = PF_GW302_TwineTrackDelay[PF_MIN]; // Limiting to minimum value

					break;

					case 5:
						pFac.pf.GW302_TwineLenghtPulse -= PF_GW302_TwineLenghtPulse[PF_STEP];
						if( pFac.pf.GW302_TwineLenghtPulse < PF_GW302_TwineLenghtPulse[PF_MIN] )
							pFac.pf.GW302_TwineLenghtPulse = PF_GW302_TwineLenghtPulse[PF_MIN]; // Limiting to minimum value

					break;

					case 6:
						pFac.pf.GW302_TrackDistancePulse -= PF_GW302_TrackDistancePulse[PF_STEP];
						if( pFac.pf.GW302_TrackDistancePulse < PF_GW302_TrackDistancePulse[PF_MIN] )
							pFac.pf.GW302_TrackDistancePulse = PF_GW302_TrackDistancePulse[PF_MIN]; // Limiting to minimum value
					break;

					case 7:
						pFac.pf.GW302_TrackMaxFwd -= PF_GW302_TrackMaxFwd[PF_STEP];
						if( pFac.pf.GW302_TrackMaxFwd < PF_GW302_TrackMaxFwd[PF_MIN] )
							pFac.pf.GW302_TrackMaxFwd = PF_GW302_TrackMaxFwd[PF_MIN]; // Limiting to minimum value


					break;
				}
			}
			else if(page == 1)
			{
				switch(selection)
				{
					case 0:
						pFac.pf.GW302_TrackMaxRev -= PF_GW302_TrackMaxRev[PF_STEP];
						if( pFac.pf.GW302_TrackMaxRev < PF_GW302_TrackMaxRev[PF_MIN] )
							pFac.pf.GW302_TrackMaxRev = PF_GW302_TrackMaxRev[PF_MIN]; // Limiting to minimum value

					break;

					case 1:
						pFac.pf.GW302_TwineMinimumEdgeDistance -= PF_GW302_TwineMinimumEdgeDistance[PF_STEP];
						if( pFac.pf.GW302_TwineMinimumEdgeDistance < PF_GW302_TwineMinimumEdgeDistance[PF_MIN] )
							pFac.pf.GW302_TwineMinimumEdgeDistance = PF_GW302_TwineMinimumEdgeDistance[PF_MIN]; // Limiting to minimum value
					break;

					case 2:
						pFac.pf.GW302_TwineTrackProtection -= PF_GW302_TwineTrackProtection[PF_STEP];
						if( pFac.pf.GW302_TwineTrackProtection < PF_GW302_TwineTrackProtection[PF_MIN] )
							pFac.pf.GW302_TwineTrackProtection = PF_GW302_TwineTrackProtection[PF_MIN]; // Limiting to minimum value
						break;
					case 3:
						pFac.pf.NoTwineWarningTime -= PF_NoTwineWarningTime[PF_STEP];
						if( pFac.pf.NoTwineWarningTime < PF_NoTwineWarningTime[PF_MIN] )
							pFac.pf.NoTwineWarningTime = PF_NoTwineWarningTime[PF_MIN]; // Limiting to minimum value

						break;

					case 4:
						pFac.pf.GW302_PreTwinePulses -=PF_GW302_PreTwinePulses[PF_STEP];
						if( pFac.pf.GW302_PreTwinePulses < PF_GW302_PreTwinePulses[PF_MIN] )
							pFac.pf.GW302_PreTwinePulses = PF_GW302_PreTwinePulses[PF_MIN]; // Limiting to minimum value

						if(pFac.pf.GW302_PreTwinePulses && (Stage !=TWINE_OP5))
						{
							PreTwineSeq = 1;
							PreTwineSeqActive = 1;
						}
						else
							PreTwineSeq = 0;
						break;

					case 5:
						pFac.pf.GW302_PreTwineDiam -=PF_GW302_PreTwineDiam[PF_STEP];
						if( pFac.pf.GW302_PreTwineDiam < PF_GW302_PreTwineDiam[PF_MIN] )
							pFac.pf.GW302_PreTwineDiam = PF_GW302_PreTwineDiam[PF_MIN]; // Limiting to minimum value
						break;

					case 6:
						pFac.pf.GW302_TwineEngageDiameter -=PF_GW302_TwineEngageDiameter[PF_STEP];
						if( pFac.pf.GW302_TwineEngageDiameter < PF_GW302_TwineEngageDiameter[PF_MIN] )
							pFac.pf.GW302_TwineEngageDiameter= PF_GW302_TwineEngageDiameter[PF_MIN]; // Limiting to minimum value
						break;

					case 7:


						if(pFac.pf.GW302_PreTwineRev != PF_GW302_PreTwineRev[PF_MIN])
							pFac.pf.GW302_PreTwineRev -=PF_GW302_PreTwineRev[PF_STEP];

						if( (pFac.pf.GW302_PreTwineRev < PF_GW302_PreTwineRev[PF_MIN]) || (pFac.pf.GW302_PreTwineRev == PF_GW302_PreTwineRev[PF_MIN]))
							pFac.pf.GW302_PreTwineRev = PF_GW302_PreTwineRev[PF_MIN]; // Limiting to minimum value
						break;



				}
			}
			else //page 2
			{
				if(pFac.pf.GW302_PreTwine2Rev != PF_GW302_PreTwine2Rev[PF_MIN])
					pFac.pf.GW302_PreTwine2Rev -=PF_GW302_PreTwine2Rev[PF_STEP];

				if( (pFac.pf.GW302_PreTwine2Rev < PF_GW302_PreTwine2Rev[PF_MIN]) || (pFac.pf.GW302_PreTwine2Rev == PF_GW302_PreTwine2Rev[PF_MIN]))
					pFac.pf.GW302_PreTwine2Rev = PF_GW302_PreTwine2Rev[PF_MIN]; // Limiting to minimum value

			}
			SavecalDelayWithinMenus = 1;
			valuechanged = 1;
			update_values = 1;
		}
		else if( lkey == SW_IL ) // -
		{
			if( page == 0 )
			{
				switch( selection )
				{
					case 0:
						if( pFac.pf.GW302_TwineOption == TO_OFF )
							pFac.pf.GW302_TwineOption = TO_ON;
						else /* pFac.pf.GW302_NetSystem == TO_ON */
						{
							pFac.pf.GW302_TwineOption = TO_OFF;
							HBM[0].HBM_Output[0].Duty[0] = HBM_OUT_OFF; //Feed Motor
							HBM[0].HBM_Output[0].Duty[1] = HBM_OUT_OFF; //Track Motor Forward
							HBM[0].HBM_Output[1].Duty[0] = HBM_OUT_OFF; //Track Motor Reverse
							HBM[0].HBM_Output[1].Duty[1] = HBM_OUT_OFF; //Track Motor Reverse

							MWarn &= ~MW_SINGLE_NO_TWINE;
							MWarn &= ~MW_DOUBLE_NO_TWINE;
							MWarn &= ~MW_NO_CAN_COM;
							MWarn &= ~MW_TRACK_POSITION;
							MWarn &= ~MW_NO_IP4_PULSES;

							TwineStage = TWINE_STAGE_12;
						}

						pFac.pf.GW302_NetTwineOption = TO_NET; /* 'Net/Twine' option reset to Net on turning 'Twine Option' */
						TwineAutoManMode = 0; //reset to man mode
					break;

					case 1:
						pFac.pf.GW302_TwineFeedDiameter += PF_GW302_TwineFeedDiameter[PF_STEP];
						if( pFac.pf.GW302_TwineFeedDiameter > PF_GW302_TwineFeedDiameter[PF_MAX] )
							pFac.pf.GW302_TwineFeedDiameter = PF_GW302_TwineFeedDiameter[PF_MAX]; // Limiting to minimum value
					break;

					case 2:
						pFac.pf.GW302_TwineFeedTime += PF_GW302_TwineFeedTime[PF_STEP];
						if( pFac.pf.GW302_TwineFeedTime > PF_GW302_TwineFeedTime[PF_MAX] )
							pFac.pf.GW302_TwineFeedTime = PF_GW302_TwineFeedTime[PF_MAX]; // Limiting to minimum value

					break;

					case 3:
						pFac.pf.GW302_TwineFeedSpeed += PF_GW302_TwineFeedSpeed[PF_STEP];
						if( pFac.pf.GW302_TwineFeedSpeed > PF_GW302_TwineFeedSpeed[PF_MAX] )
							pFac.pf.GW302_TwineFeedSpeed = PF_GW302_TwineFeedSpeed[PF_MAX]; // Limiting to minimum value

					break;

					case 4:
						pFac.pf.GW302_TwineTrackDelay += PF_GW302_TwineTrackDelay[PF_STEP];
						if( pFac.pf.GW302_TwineTrackDelay > PF_GW302_TwineTrackDelay[PF_MAX] )
							pFac.pf.GW302_TwineTrackDelay = PF_GW302_TwineTrackDelay[PF_MAX]; // Limiting to minimum value

					break;

					case 5:
						pFac.pf.GW302_TwineLenghtPulse += PF_GW302_TwineLenghtPulse[PF_STEP];
						if( pFac.pf.GW302_TwineLenghtPulse > PF_GW302_TwineLenghtPulse[PF_MAX] )
							pFac.pf.GW302_TwineLenghtPulse = PF_GW302_TwineLenghtPulse[PF_MAX]; // Limiting to minimum value
					break;

					case 6:
						pFac.pf.GW302_TrackDistancePulse += PF_GW302_TrackDistancePulse[PF_STEP];
						if( pFac.pf.GW302_TrackDistancePulse > PF_GW302_TrackDistancePulse[PF_MAX] )
							pFac.pf.GW302_TrackDistancePulse = PF_GW302_TrackDistancePulse[PF_MAX]; // Limiting to minimum value

					break;

					case 7:
						pFac.pf.GW302_TrackMaxFwd += PF_GW302_TrackMaxFwd[PF_STEP];
						if( pFac.pf.GW302_TrackMaxFwd > PF_GW302_TrackMaxFwd[PF_MAX] )
							pFac.pf.GW302_TrackMaxFwd = PF_GW302_TrackMaxFwd[PF_MAX]; // Limiting to minimum value

					break;
				}
			}
			else if(page == 1)
			{
				switch(selection)
				{
					case 0:
						pFac.pf.GW302_TrackMaxRev += PF_GW302_TrackMaxRev[PF_STEP];
						if( pFac.pf.GW302_TrackMaxRev > PF_GW302_TrackMaxRev[PF_MAX] )
							pFac.pf.GW302_TrackMaxRev = PF_GW302_TrackMaxRev[PF_MAX]; // Limiting to minimum value

					break;

					case 1:
						pFac.pf.GW302_TwineMinimumEdgeDistance += PF_GW302_TwineMinimumEdgeDistance[PF_STEP];
						if( pFac.pf.GW302_TwineMinimumEdgeDistance > PF_GW302_TwineMinimumEdgeDistance[PF_MAX] )
							pFac.pf.GW302_TwineMinimumEdgeDistance = PF_GW302_TwineMinimumEdgeDistance[PF_MAX]; // Limiting to minimum value


					break;

					case 2:
						pFac.pf.GW302_TwineTrackProtection += PF_GW302_TwineTrackProtection[PF_STEP];

						tempshi = pFac.pf.GW302_TwineTrackProtection;

						if( pFac.pf.GW302_TwineTrackProtection > PF_GW302_TwineTrackProtection[PF_MAX] )
							pFac.pf.GW302_TwineTrackProtection = PF_GW302_TwineTrackProtection[PF_MAX]; // Limiting to minimum value


					break;

					case 3:
						pFac.pf.NoTwineWarningTime += PF_NoTwineWarningTime[PF_STEP];
						if( pFac.pf.NoTwineWarningTime > PF_NoTwineWarningTime[PF_MAX] )
							pFac.pf.NoTwineWarningTime = PF_NoTwineWarningTime[PF_MAX]; // Limiting to minimum value
						break;

					case 4:
						pFac.pf.GW302_PreTwinePulses += PF_GW302_PreTwinePulses[PF_STEP];
						if( pFac.pf.GW302_PreTwinePulses > PF_GW302_PreTwinePulses[PF_MAX] )
							pFac.pf.GW302_PreTwinePulses = PF_GW302_PreTwinePulses[PF_MAX]; // Limiting to minimum value



						if(pFac.pf.GW302_PreTwinePulses && (Stage !=TWINE_OP5))
						{
							PreTwineSeq = 1;
							PreTwineSeqActive = 1;
						}
						else
							PreTwineSeq = 0;


						break;

					case 5:
						pFac.pf.GW302_PreTwineDiam += PF_GW302_PreTwineDiam[PF_STEP];
						if( pFac.pf.GW302_PreTwineDiam > PF_GW302_PreTwineDiam[PF_MAX] )
							pFac.pf.GW302_PreTwineDiam = PF_GW302_PreTwineDiam[PF_MAX]; // Limiting to minimum value
						break;

					case 6:
						pFac.pf.GW302_TwineEngageDiameter += PF_GW302_TwineEngageDiameter[PF_STEP];
						if( pFac.pf.GW302_TwineEngageDiameter > PF_GW302_TwineEngageDiameter[PF_MAX] )
							pFac.pf.GW302_TwineEngageDiameter = PF_GW302_TwineEngageDiameter[PF_MAX]; // Limiting to minimum value
						break;

					case 7:
						pFac.pf.GW302_PreTwineRev += PF_GW302_PreTwineRev[PF_STEP];
						if( pFac.pf.GW302_PreTwineRev > PF_GW302_PreTwineRev[PF_MAX] )
							pFac.pf.GW302_PreTwineRev = PF_GW302_PreTwineRev[PF_MAX]; // Limiting to minimum value
						break;



				}
			}
			else //page 2
			{
				pFac.pf.GW302_PreTwine2Rev += PF_GW302_PreTwine2Rev[PF_STEP];
				if( pFac.pf.GW302_PreTwine2Rev > PF_GW302_PreTwine2Rev[PF_MAX] )
					pFac.pf.GW302_PreTwine2Rev = PF_GW302_PreTwine2Rev[PF_MAX]; // Limiting to minimum value
			}
			SavecalDelayWithinMenus = 1;
			valuechanged = 1;
			update_values = 1;
		}
		else if( lkey == SW_CU ) // ^
		{
			if( selection )
			{
				selection--;
				update_selection = 1;
			}
		}
		else if( lkey == SW_CD ) // V
		{
			if( page == 0 )
			{
				if( selection < 7 )
				{
					selection++;
					update_selection = 1;
				}
			}
			else if(page == 1) /* page == 1 */
			{
				if( selection < 7 )
				{
					selection++;
					update_selection = 1;
				}
			}
		}
		else if( (lkey == SW_OR) || (lkey == SW_CL) ) // ESC /  <
		{
			escape = 1;
		}
		else if( lkey == SW_IR ) // PAGE
		{

			if(page == 0)
				page = 1;
			else if(page == 1)
				page = 2;
			else
				page = 0;


			selection = 0;
			update_disp = 1;
		}

	}
	else if( !editing ) // Not editing and no keys being pressed
	{
		// Saving Programmable Factors on releasing the keys for greater than 300 ms
		if( SavecalDelayOnMainDisplay || SavecalDelayWithinMenus )
		{
			if( swrel > 3 ) // > 3 x 100 ms
			{
				SavecalDelayOnMainDisplay = 0;
				SavecalDelayWithinMenus = 0;
				savecal = 1;
			}
		}
	}

	if( update_disp )
	{
		// Page number
		switch(page)
		{
		case 0:
			display_f1_text("1", TSMAX, 134, 7, FIXED );
			break;
		case 1:
			display_f1_text("2", TSMAX, 134, 7, FIXED );
			break;
		case 2:
			display_f1_text("3", TSMAX, 134, 7, FIXED );
			break;
		}
		display_f1_text( "/3", TSMAX, 140, 7, FIXED );

		// Clear display area covering factor names, values & units
		draw_box( adm, (20+1), 21, 156, 100, CLEAR );

		// Factor names & units
		if( page == 0 )
		{
			display_f1_text( "Twine Option", TSMAX, 21, 21, FIXED );

			display_f1_text( "Feed Diameter", TSMAX, 21, 31, FIXED );
			display_f1_text( "%", TSMAX, 145, 31, FIXED );

			display_f1_text( "Feed Time", TSMAX, 21, 41, FIXED );
			display_f1_text( "s", TSMAX, 145, 41, FIXED );

			display_f1_text( "Feed Speed", TSMAX, 21, 51, FIXED );
			display_f1_text( "Hz", TSMAX, 145, 51, FIXED );

			display_f1_text( "Track Delay", TSMAX, 21, 61, FIXED );
			display_f1_text( "s", TSMAX, 145, 61, FIXED );

			display_f1_text( "Twine Leng/Puls", TSMAX, 21, 71, FIXED );
			display_f1_text( "mm", TSMAX, 145, 71, FIXED );

			display_f1_text( "Track Dist/Puls", TSMAX, 21, 81, FIXED );
			display_f1_text( "mm", TSMAX, 145, 81, FIXED );

			display_f1_text( "Track Max Fwd", TSMAX, 21, 91, FIXED );
			display_f1_text( "mm", TSMAX, 145, 91, FIXED );


		}
		else if(page == 1) /* page == 1 */
		{
			display_f1_text( "Track Max Rev", TSMAX, 21, 21, FIXED );
			display_f1_text( "mm", TSMAX, 145, 21, FIXED );

			display_f1_text( "Min Edge Dist", TSMAX, 21, 31, FIXED );
			display_f1_text( "mm", TSMAX, 145, 31, FIXED );

			display_f1_text( "Track Protection", TSMAX, 21, 41, FIXED );
			display_f1_text( "s", TSMAX, 145, 41, FIXED );

			display_f1_text( "No Twine Warning", TSMAX, 21, 51, FIXED );
			display_f1_text( "s", TSMAX, 145, 51, FIXED );

			display_f1_text( "Pre Twine Pulses", TSMAX, 21, 61, FIXED );

			display_f1_text( "Pre Twine Diam", TSMAX, 21, 71, FIXED );
			display_f1_text( "%", TSMAX, 145, 71, FIXED );

			display_f1_text( "EngageDiamTwine", TSMAX, 21, 81, FIXED );
			display_f1_text( "%", TSMAX, 145, 81, FIXED );

			display_f1_text( "Pre Twine Rev", TSMAX, 21, 91, FIXED );
			display_f1_text( "mm", TSMAX, 145, 91, FIXED );

		}
		else
		{
			display_f1_text( "Pre Twine2 Pulses", TSMAX, 21, 21, FIXED );
		}
		update_disp = 0;

		update_values = 1;    // Display factor values
		update_selection = 1; // Display tabs & selection pointer

	} // End of update_disp

	if( update_values )
	{
		// Factor values
		if( page == 0 )
		{


			if( pFac.pf.GW302_TwineOption == TO_OFF )
				display_f1_text( "OFF", TSMAX, 128, 21, FIXED );
			else /* pFac.pf.GW302_TwineOption == TO_ON */
					display_f1_text( "ON", TSMAX, 133, 21, FIXED );


			display_number( (float)pFac.pf.GW302_TwineFeedDiameter,
					(float)PF_GW302_TwineFeedDiameter[PF_MIN], (float)PF_GW302_TwineFeedDiameter[PF_MAX],
					1.0, DP0, 142, 31, DW3, RIGHT, &RDSfnt1, NULL, PNULL );

			display_number( (float)pFac.pf.GW302_TwineFeedTime,
			(float)PF_GW302_TwineFeedTime[PF_MIN], (float)PF_GW302_TwineFeedTime[PF_MAX],
			1.0, DP1, 142, 41, DW4, RIGHT, &RDSfnt1, NULL, PNULL );

			display_number( (float)pFac.pf.GW302_TwineFeedSpeed,
			(float)PF_GW302_TwineFeedSpeed[PF_MIN], (float)PF_GW302_TwineFeedSpeed[PF_MAX],
			1.0, DP1, 142, 51, DW5, RIGHT, &RDSfnt1, NULL, PNULL );

			display_number( (float)pFac.pf.GW302_TwineTrackDelay,
			(float)PF_GW302_TwineTrackDelay[PF_MIN], (float)PF_GW302_TwineTrackDelay[PF_MAX],
			1.0, DP1, 142, 61, DW4, RIGHT, &RDSfnt1, NULL, PNULL );

			display_number( (float)pFac.pf.GW302_TwineLenghtPulse,
			(float)PF_GW302_TwineLenghtPulse[PF_MIN], (float)PF_GW302_TwineLenghtPulse[PF_MAX],
			1.0, DP1, 142, 71, DW5, RIGHT, &RDSfnt1, NULL, PNULL );

			display_number( (float)pFac.pf.GW302_TrackDistancePulse,
			(float)PF_GW302_TrackDistancePulse[PF_MIN], (float)PF_GW302_TrackDistancePulse[PF_MAX],
			1.0, DP2, 142, 81, DW4, RIGHT, &RDSfnt1, NULL, PNULL );

			display_number( (float)pFac.pf.GW302_TrackMaxFwd,
			(float)PF_GW302_TrackMaxFwd[PF_MIN], (float)PF_GW302_TrackMaxFwd[PF_MAX],
			1.0, DP0, 142, 91, DW3, RIGHT, &RDSfnt1, NULL, PNULL );

		}
		else if(page == 1) /* page == 1 */
		{


			display_number( (float)pFac.pf.GW302_TrackMaxRev,
			(float)PF_GW302_TrackMaxRev[PF_MIN], (float)PF_GW302_TrackMaxRev[PF_MAX],
			1.0, DP0, 142, 21, DW3, RIGHT, &RDSfnt1, NULL, PNULL );


			display_number( (float)pFac.pf.GW302_TwineMinimumEdgeDistance,
					(float)PF_GW302_TwineMinimumEdgeDistance[PF_MIN], (float)PF_GW302_TwineMinimumEdgeDistance[PF_MAX],
					1.0, DP0, 142, 31, DW3, RIGHT, &RDSfnt1, NULL, PNULL );

			display_number( (float)pFac.pf.GW302_TwineTrackProtection/10,
					(float)PF_GW302_TwineTrackProtection[PF_MIN], (float)PF_GW302_TwineTrackProtection[PF_MAX],
					1.0, DP1, 142, 41, DW4, RIGHT, &RDSfnt1, NULL, PNULL );

			display_number( (float)pFac.pf.NoTwineWarningTime,
					(float)PF_NoTwineWarningTime[PF_MIN], (float)PF_NoTwineWarningTime[PF_MAX],
					1.0, DP1, 142, 51, DW3, RIGHT, &RDSfnt1, NULL, PNULL );

			display_number( (float)pFac.pf.GW302_PreTwinePulses,
					(float)PF_GW302_PreTwinePulses[PF_MIN], (float)PF_GW302_PreTwinePulses[PF_MAX],
					1.0, DP0, 142, 61, DW4, RIGHT, &RDSfnt1, NULL, PNULL );

			display_number( (float)pFac.pf.GW302_PreTwineDiam,
					(float)PF_GW302_PreTwineDiam[PF_MIN], (float)PF_GW302_PreTwineDiam[PF_MAX],
					1.0, DP0, 142, 71, DW4, RIGHT, &RDSfnt1, NULL, PNULL );

			display_number( (float)pFac.pf.GW302_TwineEngageDiameter,
					(float)PF_GW302_TwineEngageDiameter[PF_MIN], (float)PF_GW302_TwineEngageDiameter[PF_MAX],
					1.0, DP0, 142, 81, DW3, RIGHT, &RDSfnt1, NULL, PNULL );

			display_number( (float)pFac.pf.GW302_PreTwineRev,
					(float)PF_GW302_PreTwineRev[PF_MIN], (float)PF_GW302_PreTwineRev[PF_MAX],
					1.0, DP0, 142, 91, DW3, RIGHT, &RDSfnt1, NULL, PNULL );




		}
		else
		{
			display_number( (float)pFac.pf.GW302_PreTwine2Rev,
					(float)PF_GW302_PreTwine2Rev[PF_MIN], (float)PF_GW302_PreTwine2Rev[PF_MAX],
					1.0, DP0, 142, 21, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
		}
		update_values = 0;

	} // End of update_values

	if( update_selection )
	{
		// Tabs
		draw_box( adm, 1, 102, 158, 126, CLEAR ); // Clear area underneath tabs
		// Drawn according to position of selection pointer
		if( page == 0 )
		{
			switch( selection )
			{
			case 0:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_DL,  62, 102, OR );    // NAVIGATION: Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( "1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( "1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;

			case 7:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UL,  62, 102, OR );    // NAVIGATION: Up Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text("1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;
			}
		}
		else if(page == 1) /* page == 1 */
		{
			switch( selection )
			{
			case 0:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_DL,  62, 102, OR );    // NAVIGATION: Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text("2", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;

			case 1:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text("2", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;

			case 2:

				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text("2", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;


			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text("2", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;


			}
		}
		else
		{
			switch( selection )
			{
				case 0:
					write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
					write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
					write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
					write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
					display_f1_text("3", TSMAX, 99+19, 113, FIXED );
					write_bitmap( adm, TABESC,    130, 107, OR );    // ESC

				break;
			}
		}

		// Selection pointer
		draw_box( adm, 13, 21 , 20, 100, CLEAR );
		write_bitmap( adm, F1_POINT, 13, 21+(10*selection), OR );

		update_selection = 0;

	} // End of update_selection


}
// -----------------------------------------------------------
// Function Name       : screen142
// Description         : Technician Menu - Baler Full
// -----------------------------------------------------------
void screen142( unsigned int lkey )
{
	char update_disp;
	char update_selection;
	char update_values;

	static char page;

	if( !startup )
	{
		startup = 1;

		draw_frame( adm );

		display_f1_text( "Baler Full", TSMAX, 80, 7, FIXED | CENTER | UNDERLINED );

		page = 0;
		selection = 0;
		update_disp = 1;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	if( !editing && (lkey & (SW_NEW | SW_REP)) )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_OL ) // -
		{
			if( page == 0 )
			{
				switch( selection )
				{
				case 0:
					if( pFac.pf.GW302_LubeAlarm > PF_GW302_LubeAlarm[PF_MIN] ) // Limiting unsigned type to minimum value
						pFac.pf.GW302_LubeAlarm -= PF_GW302_LubeAlarm[PF_STEP];
					break;

				case 1:
					pFac.pf.GW302_PresDeadband -= PF_GW302_PresDeadband[PF_STEP];
					if( pFac.pf.GW302_PresDeadband < PF_GW302_PresDeadband[PF_MIN] )
						pFac.pf.GW302_PresDeadband = PF_GW302_PresDeadband[PF_MIN]; // Limiting to minimum value
					break;

				case 2:
					pFac.pf.GW302_NetSpeed -= PF_GW302_NetSpeed[PF_STEP];
					if( pFac.pf.GW302_NetSpeed < PF_GW302_NetSpeed[PF_MIN] )
						pFac.pf.GW302_NetSpeed = PF_GW302_NetSpeed[PF_MIN]; // Limiting to minimum value
					break;

				case 3:
					pFac.pf.GW302_PresSwDelay -= PF_GW302_PresSwDelay[PF_STEP];
					if( pFac.pf.GW302_PresSwDelay < PF_GW302_PresSwDelay[PF_MIN] )
						pFac.pf.GW302_PresSwDelay = PF_GW302_PresSwDelay[PF_MIN]; // Limiting to minimum value
					break;

				case 4:
					pFac.pf.GW302_DoorClosed -= PF_GW302_DoorClosed[PF_STEP];
					if( pFac.pf.GW302_DoorClosed < PF_GW302_DoorClosed[PF_MIN] )
						pFac.pf.GW302_DoorClosed = PF_GW302_DoorClosed[PF_MIN]; // Limiting to minimum value
					break;

				case 5:
					if( pFac.pf.GW302_BaleSizeFactor > PF_GW302_BaleSizeFactor[PF_MIN] ) // Limiting unsigned type to minimum value
						pFac.pf.GW302_BaleSizeFactor -= PF_GW302_BaleSizeFactor[PF_STEP];
					break;
				}
			}
			else /* page == 1 */
			{
				switch( selection )
				{
				case 0:
					pFac.pf.GW302_FillPotRange -= PF_GW302_FillPotRange[PF_STEP];
					if( pFac.pf.GW302_FillPotRange < PF_GW302_FillPotRange[PF_MIN] )
						pFac.pf.GW302_FillPotRange = PF_GW302_FillPotRange[PF_MIN]; // Limiting to minimum value
					break;

				case 1:
					pFac.pf.GW302_FillPotIgnore -= PF_GW302_FillPotIgnore[PF_STEP];
					if( pFac.pf.GW302_FillPotIgnore < PF_GW302_FillPotIgnore[PF_MIN] )
						pFac.pf.GW302_FillPotIgnore = PF_GW302_FillPotIgnore[PF_MIN]; // Limiting to minimum value
					break;

				case 2:
					if( pFac.pf.GW302_BaleIndicator == BI_STEER )
						pFac.pf.GW302_BaleIndicator = BI_FILL;
					else /* pFac.pf.GW302_BaleIndicator == BI_FILL */
						pFac.pf.GW302_BaleIndicator = BI_STEER;
					break;

				case 3:
					pFac.pf.GW302_RollerDelay -= PF_GW302_RollerDelay[PF_STEP];
					if( pFac.pf.GW302_RollerDelay < PF_GW302_RollerDelay[PF_MIN] )
						pFac.pf.GW302_RollerDelay = PF_GW302_RollerDelay[PF_MIN]; // Limiting to minimum value
					break;

				case 4:
					if( pFac.pf.GW302_DiagnosticMode == TO_OFF )
						pFac.pf.GW302_DiagnosticMode = TO_ON;
					else /* pFac.pf.GW302_NetSystem == TO_ON */
						pFac.pf.GW302_DiagnosticMode = TO_OFF;

					break;

				case 5:
					pFac.pf.GW302_EngageTime -= PF_GW302_EngageTime[PF_STEP];
					if( pFac.pf.GW302_EngageTime < PF_GW302_EngageTime[PF_MIN] )
						pFac.pf.GW302_EngageTime = PF_GW302_EngageTime[PF_MIN]; // Limiting to minimum value
					break;
				}
			}
			SavecalDelayWithinMenus = 1;
			valuechanged = 1;
			update_values = 1;
		}
		else if( lkey == SW_IL ) // +
		{
			if( page == 0 )
			{
				switch( selection )
				{
				case 0:
					pFac.pf.GW302_LubeAlarm += PF_GW302_LubeAlarm[PF_STEP];
					if( pFac.pf.GW302_LubeAlarm > PF_GW302_LubeAlarm[PF_MAX] )
						pFac.pf.GW302_LubeAlarm = PF_GW302_LubeAlarm[PF_MAX]; // Limiting to maximum value
					break;

				case 1:
					pFac.pf.GW302_PresDeadband += PF_GW302_PresDeadband[PF_STEP];
					if( pFac.pf.GW302_PresDeadband > PF_GW302_PresDeadband[PF_MAX]  )
						pFac.pf.GW302_PresDeadband = PF_GW302_PresDeadband[PF_MAX]; // Limiting to maximum value
					break;

				case 2:
					pFac.pf.GW302_NetSpeed += PF_GW302_NetSpeed[PF_STEP];
					if( pFac.pf.GW302_NetSpeed > PF_GW302_NetSpeed[PF_MAX] )
						pFac.pf.GW302_NetSpeed = PF_GW302_NetSpeed[PF_MAX]; // Limiting to maximum value
					break;

				case 3:
					pFac.pf.GW302_PresSwDelay += PF_GW302_PresSwDelay[PF_STEP];
					if( pFac.pf.GW302_PresSwDelay > PF_GW302_PresSwDelay[PF_MAX] )
						pFac.pf.GW302_PresSwDelay = PF_GW302_PresSwDelay[PF_MAX]; // Limiting to maximum value
					break;

				case 4:
					pFac.pf.GW302_DoorClosed += PF_GW302_DoorClosed[PF_STEP];
					if( pFac.pf.GW302_DoorClosed > PF_GW302_DoorClosed[PF_MAX] )
						pFac.pf.GW302_DoorClosed = PF_GW302_DoorClosed[PF_MAX]; // Limiting to maximum value
					break;

				case 5:
					pFac.pf.GW302_BaleSizeFactor += PF_GW302_BaleSizeFactor[PF_STEP];
					if( pFac.pf.GW302_BaleSizeFactor > PF_GW302_BaleSizeFactor[PF_MAX] )
						pFac.pf.GW302_BaleSizeFactor = PF_GW302_BaleSizeFactor[PF_MAX]; // Limiting to maximum value
					break;
				}
			}
			else /* page == 1 */
			{
				switch( selection )
				{
				case 0:
					pFac.pf.GW302_FillPotRange += PF_GW302_FillPotRange[PF_STEP];
					if( pFac.pf.GW302_FillPotRange > PF_GW302_FillPotRange[PF_MAX] )
						pFac.pf.GW302_FillPotRange = PF_GW302_FillPotRange[PF_MAX]; // Limiting to maximum value
					break;

				case 1:
					pFac.pf.GW302_FillPotIgnore += PF_GW302_FillPotIgnore[PF_STEP];
					if( pFac.pf.GW302_FillPotIgnore > PF_GW302_FillPotIgnore[PF_MAX] )
						pFac.pf.GW302_FillPotIgnore = PF_GW302_FillPotIgnore[PF_MAX]; // Limiting to maximum value
					break;

				case 2:
					if( pFac.pf.GW302_BaleIndicator == BI_STEER )
						pFac.pf.GW302_BaleIndicator = BI_FILL;
					else /* pFac.pf.GW302_BaleIndicator == BI_FILL */
						pFac.pf.GW302_BaleIndicator = BI_STEER;
					break;

				case 3:
					pFac.pf.GW302_RollerDelay += PF_GW302_RollerDelay[PF_STEP];
					if( pFac.pf.GW302_RollerDelay > PF_GW302_RollerDelay[PF_MAX] )
						pFac.pf.GW302_RollerDelay = PF_GW302_RollerDelay[PF_MAX]; // Limiting to maximum value
					break;

				case 4:
					if( pFac.pf.GW302_DiagnosticMode == TO_OFF )
						pFac.pf.GW302_DiagnosticMode = TO_ON;
					else /* pFac.pf.GW302_NetSystem == TO_ON */
						pFac.pf.GW302_DiagnosticMode = TO_OFF;

					break;

				case 5:
					pFac.pf.GW302_EngageTime += PF_GW302_EngageTime[PF_STEP];
					if( pFac.pf.GW302_EngageTime > PF_GW302_EngageTime[PF_MAX] )
						pFac.pf.GW302_EngageTime = PF_GW302_EngageTime[PF_MAX]; // Limiting to maximum value
					break;

				}
			}
			SavecalDelayWithinMenus = 1;
			valuechanged = 1;
			update_values = 1;
		}
		else if( lkey == SW_CU ) // ^
		{
			if( selection )
			{
				selection--;
				update_selection = 1;
			}
		}
		else if( lkey == SW_CD ) // V
		{
			if( page == 0 )
			{
				if( selection < 5 )
				{
					selection++;
					update_selection = 1;
				}
			}
			else /* page == 1 */
			{
				if( selection < 5 )
				{
					selection++;
					update_selection = 1;
				}
			}
		}
		else if( (lkey == SW_OR) || (lkey == SW_CL) ) // ESC /  <
		{
			escape = 1;
		}
		else if( lkey == SW_IR ) // PAGE
		{
			page = page? 0:1;

			selection = 0;
			update_disp = 1;
		}
	}
	else if( !editing ) // Not editing and no keys being pressed
	{
		// Saving Programmable Factors on releasing the keys for greater than 300 ms
		if( SavecalDelayOnMainDisplay || SavecalDelayWithinMenus )
		{
			if( swrel > 3 ) // > 3 x 100 ms
			{
				SavecalDelayOnMainDisplay = 0;
				SavecalDelayWithinMenus = 0;
				savecal = 1;
			}
		}
	}


	if( update_disp )
	{
		// Page number
		display_f1_text( (page==0)? "1":"2", TSMAX, 134, 7, FIXED );
		display_f1_text( "/2", TSMAX, 140, 7, FIXED );

		// Clear display area covering factor names, values & units
		draw_box( adm, (20+1), 21, 156, 100, CLEAR );

		// Factor names & units
		if( page == 0 )
		{
			display_f1_text( "Lube Alarm", TSMAX, 24, 21, FIXED );

			display_f1_text( "Pres. Deadband", TSMAX, 24, 31, FIXED );
			display_f1_text( "V", TSMAX, 145, 31, FIXED );

			display_f1_text( "Net Speed", TSMAX, 24, 41, FIXED );
			display_f1_text( "Hz", TSMAX, 145, 41, FIXED );

			display_f1_text( "Pres. Sw. Delay", TSMAX, 24, 51, FIXED );
			display_f1_text( "s", TSMAX, 145, 51, FIXED );

			display_f1_text( "Door Closed", TSMAX, 24, 61, FIXED );
			display_f1_text( "s", TSMAX, 145, 61, FIXED );

			display_f1_text( "Bale Size Factor", TSMAX, 24, 71, FIXED );

			/* Displaying calculated max dia voltage in brackets */
			display_f1_text( "(     V )", TSMAX, 106, 81, FIXED );
		}
		else /* page == 1 */
		{
			display_f1_text( "Fill Pot Range", TSMAX, 24, 21, FIXED );
			display_f1_text( "V", TSMAX, 145, 21, FIXED );

			display_f1_text( "Fill Pot Ignore", TSMAX, 24, 31, FIXED );
			display_f1_text( "V", TSMAX, 145, 31, FIXED );

			display_f1_text( "Bale Indicator", TSMAX, 24, 41, FIXED );

			display_f1_text( "Roller Delay", TSMAX, 24, 51, FIXED );
			display_f1_text( "s", TSMAX, 145, 51, FIXED );

			display_f1_text( "Diagnostic Mode", TSMAX, 24, 61, FIXED );

			display_f1_text( "Engage Time", TSMAX, 24, 71, FIXED );
			display_f1_text( "s", TSMAX, 145, 71, FIXED );

		}
		update_disp = 0;

		update_values = 1;    // Display factor values
		update_selection = 1; // Display tabs & selection pointer

	} // End of update_disp

	if( update_values )
	{
		// Factor values
		if( page == 0 )
		{
			display_number( (float)pFac.pf.GW302_LubeAlarm,
					(float)PF_GW302_LubeAlarm[PF_MIN], (float)PF_GW302_LubeAlarm[PF_MAX],
					1.0, DP0, 142, 21, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_PresDeadband,
					(float)PF_GW302_PresDeadband[PF_MIN], (float)PF_GW302_PresDeadband[PF_MAX],
					1.0, DP2, 142, 31, DW4, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_NetSpeed,
					(float)PF_GW302_NetSpeed[PF_MIN], (float)PF_GW302_NetSpeed[PF_MAX],
					1.0, DP1, 142, 41, DW4, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_PresSwDelay,
					(float)PF_GW302_PresSwDelay[PF_MIN], (float)PF_GW302_PresSwDelay[PF_MAX],
					1.0, DP1, 142, 51, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_DoorClosed,
					(float)PF_GW302_DoorClosed[PF_MIN], (float)PF_GW302_DoorClosed[PF_MAX],
					1.0, DP1, 142, 61, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_BaleSizeFactor,
					(float)PF_GW302_BaleSizeFactor[PF_MIN], (float)PF_GW302_BaleSizeFactor[PF_MAX],
					1.0, DP0, 142, 71, DW3, RIGHT, &RDSfnt1, NULL, PNULL );

			/* Displaying calculated max dia voltage in brackets */
			display_number( DiaMaxVolts, 0.0, 9.99,  1.0, DP2, 136, 81, DW4, RIGHT, &RDSfnt1, NULL, PNULL );
		}
		else /* page == 1 */
		{
			display_number( (float)pFac.pf.GW302_FillPotRange,
					(float)PF_GW302_FillPotRange[PF_MIN], (float)PF_GW302_FillPotRange[PF_MAX],
					1.0, DP2, 142, 21, DW4, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_FillPotIgnore,
					(float)PF_GW302_FillPotIgnore[PF_MIN], (float)PF_GW302_FillPotIgnore[PF_MAX],
					1.0, DP2, 142, 31, DW4, RIGHT, &RDSfnt1, NULL, PNULL );


			if( pFac.pf.GW302_BaleIndicator == BI_STEER )
				display_f1_text( "Steer", TSMAX, 125, 41, FIXED );
			else /* pFac.pf.GW302_BaleIndicator == BI_FILL */
				display_f1_text( " Fill", TSMAX, 125, 41, FIXED );

			display_number( (float)pFac.pf.GW302_RollerDelay,
					(float)PF_GW302_RollerDelay[PF_MIN], (float)PF_GW302_RollerDelay[PF_MAX],
					1.0, DP1, 142, 51, DW3, RIGHT, &RDSfnt1, NULL, PNULL );

			if( pFac.pf.GW302_DiagnosticMode == TO_OFF )
						display_f1_text( "OFF", TSMAX, 128, 61, FIXED );
					else /* pFac.pf.GW302_TwineOption == TO_ON */
							display_f1_text( "ON", TSMAX, 133, 61, FIXED );

			display_number( (float)pFac.pf.GW302_EngageTime,
					(float)PF_GW302_EngageTime[PF_MIN], (float)PF_GW302_EngageTime[PF_MAX],
					1.0, DP1, 142, 71, DW3, RIGHT, &RDSfnt1, NULL, PNULL );

		}
		update_values = 0;

	} // End of update_values

	if( update_selection )
	{
		// Tabs
		draw_box( adm, 1, 102, 158, 126, CLEAR ); // Clear area underneath tabs
		// Drawn according to position of selection pointer
		if( page == 0 )
		{
			switch( selection )
			{
			case 0:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_DL,  62, 102, OR );    // NAVIGATION: Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;

			case 1:
			case 2:
			case 3:
			case 4:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;

			case 5:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UL,  62, 102, OR );    // NAVIGATION: Up Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;
			}
		}
		else /* page == 1 */
		{
			switch( selection )
			{
			case 0:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_DL,  62, 102, OR );    // NAVIGATION: Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;

			case 1:
			case 2:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;

			case 3:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;

			case 4:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;


			case 5:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UL,  62, 102, OR );    // NAVIGATION: Up Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;

			}
		}

		// Selection pointer
		draw_box( adm, 16, 21 , 20, 100, CLEAR );
		write_bitmap( adm, F1_POINT, 16, 21+(10*selection), OR );

		update_selection = 0;

	} // End of update_selection

}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen143
// Description         : Technician Menu - Netting Setup
// -----------------------------------------------------------
void screen143( unsigned int lkey )
{
	char update_disp;
	char update_selection;
	char update_values;

	static char page;

	if( !startup )
	{
		startup = 1;

		draw_frame( adm );

		display_f1_text( "Netting Setup", TSMAX, 80, 7, FIXED | CENTER | UNDERLINED );

		page = 0;
		selection = 0;
		update_disp = 1;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	if( !editing && (lkey & (SW_NEW | SW_REP)) )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_OL ) // -
		{
			if( page == 0 )
			{
				switch( selection )
				{
				case 0:
					pFac.pf.GW302_NetOn -= PF_GW302_NetOn[PF_STEP];
					if( pFac.pf.GW302_NetOn < PF_GW302_NetOn[PF_MIN] )
						pFac.pf.GW302_NetOn = PF_GW302_NetOn[PF_MIN]; // Limiting to minimum value
					break;

				case 1:
					if( pFac.pf.GW302_NetCutPulse > PF_GW302_NetCutPulse[PF_MIN] ) // Limiting unsigned type to minimum value
						pFac.pf.GW302_NetCutPulse -= PF_GW302_NetCutPulse[PF_STEP];
					break;

				case 2:
					pFac.pf.GW302_NetCutTime -= PF_GW302_NetCutTime[PF_STEP];
					if( pFac.pf.GW302_NetCutTime < PF_GW302_NetCutTime[PF_MIN] )
						pFac.pf.GW302_NetCutTime = PF_GW302_NetCutTime[PF_MIN]; // Limiting to minimum value
					break;

				case 3:
					pFac.pf.GW302_MinDiaPulsPerLay -= PF_GW302_MinDiaPulsPerLay[PF_STEP];
					if( pFac.pf.GW302_MinDiaPulsPerLay < PF_GW302_MinDiaPulsPerLay[PF_MIN] )
						pFac.pf.GW302_MinDiaPulsPerLay = PF_GW302_MinDiaPulsPerLay[PF_MIN]; // Limiting to minimum value
					break;

				case 4:
					pFac.pf.GW302_MaxDiaPulsPerLay -= PF_GW302_MaxDiaPulsPerLay[PF_STEP];
					if( pFac.pf.GW302_MaxDiaPulsPerLay < PF_GW302_MaxDiaPulsPerLay[PF_MIN] )
						pFac.pf.GW302_MaxDiaPulsPerLay = PF_GW302_MaxDiaPulsPerLay[PF_MIN]; // Limiting to minimum value
					break;

				case 5:
					pFac.pf.GW302_NetFactor -= PF_GW302_NetFactor[PF_STEP];
					if( pFac.pf.GW302_NetFactor < PF_GW302_NetFactor[PF_MIN] )
						pFac.pf.GW302_NetFactor = PF_GW302_NetFactor[PF_MIN]; // Limiting to minimum value
					break;

				case 6:
					pFac.pf.GW302_NoNetPulse -= PF_GW302_NoNetPulse[PF_STEP];
					if( pFac.pf.GW302_NoNetPulse < PF_GW302_NoNetPulse[PF_MIN] )
						pFac.pf.GW302_NoNetPulse = PF_GW302_NoNetPulse[PF_MIN]; // Limiting to minimum value
					break;

				case 7:
					if(pFac.pf.GW302_NetTwineOption == TO_NET)
					{
						if( pFac.pf.GW302_PreNetPulse > PF_GW302_PreNetPulse[PF_MIN] ) // Limiting unsigned type to minimum value
							pFac.pf.GW302_PreNetPulse -= PF_GW302_PreNetPulse[PF_STEP];
					}
					break;
				}
			}
			else /* page == 1 */
			{
				switch( selection )
				{
				case 0:
					if( pFac.pf.GW302_NetSystem == NS_BEAK )
						pFac.pf.GW302_NetSystem = NS_ROLLER;
					else /* pFac.pf.GW302_NetSystem == NS_ROLLER */
						pFac.pf.GW302_NetSystem = NS_BEAK;
					break;

				case 1:
					if( pFac.pf.GW302_BeakEngage > PF_GW302_BeakEngage[PF_MIN] ) // Limiting unsigned type to minimum value
						pFac.pf.GW302_BeakEngage -= PF_GW302_BeakEngage[PF_STEP];
					break;

				case 2:
					if( pFac.pf.GW302_EngageDiameter > PF_GW302_EngageDiameter[PF_MIN] ) // Limiting unsigned type to minimum value
						pFac.pf.GW302_EngageDiameter -= PF_GW302_EngageDiameter[PF_STEP];
					break;


				}
			}
			SavecalDelayWithinMenus = 1;
			valuechanged = 1;
			update_values = 1;
		}
		else if( lkey == SW_IL ) // +
		{
			if( page == 0 )
			{
				switch( selection )
				{
				case 0:
					pFac.pf.GW302_NetOn += PF_GW302_NetOn[PF_STEP];
					if( pFac.pf.GW302_NetOn > PF_GW302_NetOn[PF_MAX] )
						pFac.pf.GW302_NetOn = PF_GW302_NetOn[PF_MAX]; // Limiting to maximum value
					break;

				case 1:
					pFac.pf.GW302_NetCutPulse += PF_GW302_NetCutPulse[PF_STEP];
					if( pFac.pf.GW302_NetCutPulse > PF_GW302_NetCutPulse[PF_MAX] )
						pFac.pf.GW302_NetCutPulse = PF_GW302_NetCutPulse[PF_MAX]; // Limiting to maximum value
					break;

				case 2:
					pFac.pf.GW302_NetCutTime += PF_GW302_NetCutTime[PF_STEP];
					if( pFac.pf.GW302_NetCutTime > PF_GW302_NetCutTime[PF_MAX] )
						pFac.pf.GW302_NetCutTime = PF_GW302_NetCutTime[PF_MAX]; // Limiting to maximum value
					break;

				case 3:
					pFac.pf.GW302_MinDiaPulsPerLay += PF_GW302_MinDiaPulsPerLay[PF_STEP];
					if( pFac.pf.GW302_MinDiaPulsPerLay > PF_GW302_MinDiaPulsPerLay[PF_MAX] )
						pFac.pf.GW302_MinDiaPulsPerLay = PF_GW302_MinDiaPulsPerLay[PF_MAX]; // Limiting to maximum value
					break;

				case 4:
					pFac.pf.GW302_MaxDiaPulsPerLay += PF_GW302_MaxDiaPulsPerLay[PF_STEP];
					if( pFac.pf.GW302_MaxDiaPulsPerLay > PF_GW302_MaxDiaPulsPerLay[PF_MAX] )
						pFac.pf.GW302_MaxDiaPulsPerLay = PF_GW302_MaxDiaPulsPerLay[PF_MAX]; // Limiting to maximum value
					break;

				case 5:
					pFac.pf.GW302_NetFactor += PF_GW302_NetFactor[PF_STEP];
					if( pFac.pf.GW302_NetFactor > PF_GW302_NetFactor[PF_MAX] )
						pFac.pf.GW302_NetFactor = PF_GW302_NetFactor[PF_MAX]; // Limiting to maximum value
					break;

				case 6:
					pFac.pf.GW302_NoNetPulse += PF_GW302_NoNetPulse[PF_STEP];
					if( pFac.pf.GW302_NoNetPulse > PF_GW302_NoNetPulse[PF_MAX] )
						pFac.pf.GW302_NoNetPulse = PF_GW302_NoNetPulse[PF_MAX]; // Limiting to maximum value
					break;

				case 7:
					if(pFac.pf.GW302_NetTwineOption == TO_NET)
					{
						pFac.pf.GW302_PreNetPulse += PF_GW302_PreNetPulse[PF_STEP];
						if( pFac.pf.GW302_PreNetPulse > PF_GW302_PreNetPulse[PF_MAX] )
							pFac.pf.GW302_PreNetPulse = PF_GW302_PreNetPulse[PF_MAX]; // Limiting to maximum value
					}
					break;
				}
			}
			else /* page == 1 */
			{
				switch( selection )
				{
				case 0:
					if( pFac.pf.GW302_NetSystem == NS_BEAK )
						pFac.pf.GW302_NetSystem = NS_ROLLER;
					else /* pFac.pf.GW302_NetSystem == NS_ROLLER */
						pFac.pf.GW302_NetSystem = NS_BEAK;
					break;

				case 1:
					pFac.pf.GW302_BeakEngage += PF_GW302_BeakEngage[PF_STEP];
					if( pFac.pf.GW302_BeakEngage > PF_GW302_BeakEngage[PF_MAX] )
						pFac.pf.GW302_BeakEngage = PF_GW302_BeakEngage[PF_MAX]; // Limiting to maximum value
					break;

				case 2:
					pFac.pf.GW302_EngageDiameter += PF_GW302_EngageDiameter[PF_STEP];
					if( pFac.pf.GW302_EngageDiameter > PF_GW302_EngageDiameter[PF_MAX] )
						pFac.pf.GW302_EngageDiameter = PF_GW302_EngageDiameter[PF_MAX]; // Limiting to maximum value

					break;


				}
			}
			SavecalDelayWithinMenus = 1;
			valuechanged = 1;
			update_values = 1;
		}
		else if( lkey == SW_CU ) // ^
		{
			if( selection )
			{
				selection--;
				update_selection = 1;
			}
		}
		else if( lkey == SW_CD ) // V
		{
			if( page == 0 )
			{
				if( selection < 7 )
				{
					selection++;
					update_selection = 1;
				}
			}
			else /* page == 1 */
			{
				if( selection < 2 )
				{
					selection++;
					update_selection = 1;
				}
			}
		}
		else if( (lkey == SW_OR) || (lkey == SW_CL) ) // ESC /  <
		{
			escape = 1;
		}
		else if( lkey == SW_IR ) // PAGE
		{
			page = page? 0:1;

			selection = 0;
			update_disp = 1;
		}
	}
	else if( !editing ) // Not editing and no keys being pressed
	{
		// Saving Programmable Factors on releasing the keys for greater than 300 ms
		if( SavecalDelayOnMainDisplay || SavecalDelayWithinMenus )
		{
			if( swrel > 3 ) // > 3 x 100 ms
			{
				SavecalDelayOnMainDisplay = 0;
				SavecalDelayWithinMenus = 0;
				savecal = 1;
			}
		}
	}


	if( update_disp )
	{
		// Page number
		display_f1_text( (page==0)? "1":"2", TSMAX, 134, 7, FIXED );
		display_f1_text( "/2", TSMAX, 140, 7, FIXED );

		// Clear display area covering factor names, values & units
		draw_box( adm, (20+1), 21, 156, 100, CLEAR );

		// Factor names & units
		if( page == 0 )
		{
			display_f1_text( "Net On", TSMAX, 24, 21, FIXED );
			display_f1_text( "s", TSMAX, 146, 21, FIXED );

			display_f1_text( "Net Cut Pulse", TSMAX, 24, 31, FIXED );

			display_f1_text( "Net Cut Time", TSMAX, 24, 41, FIXED );
			display_f1_text( "s", TSMAX, 146, 41, FIXED );

			display_f1_text( "MinDia puls/lay", TSMAX, 24, 51, FIXED );

			display_f1_text( "MaxDia puls/lay", TSMAX, 24, 61, FIXED );

			display_f1_text( "Net Factor", TSMAX, 24, 71, FIXED );

			display_f1_text( "No Net Pulse", TSMAX, 24, 81, FIXED );
			display_f1_text( "s", TSMAX, 146, 81, FIXED );

			display_f1_text( "Pre Net Pulse", TSMAX, 24, 91, FIXED );
		}
		else /* page == 1 */
		{
			display_f1_text( "Net System", TSMAX, 24, 21, FIXED );

			display_f1_text( "Beak Engage", TSMAX, 24, 31, FIXED );

			display_f1_text( "Engage Diam Net", TSMAX, 24, 41, FIXED );
			display_f1_text( "%", TSMAX, 145, 41, FIXED );

		}
		update_disp = 0;

		update_values = 1;    // Display factor values
		update_selection = 1; // Display tabs & selection pointer

	} // End of update_disp

	if( update_values )
	{
		// Factor values
		if( page == 0 )
		{
			display_number( (float)pFac.pf.GW302_NetOn,
					(float)PF_GW302_NetOn[PF_MIN], (float)PF_GW302_NetOn[PF_MAX],
					1.0, DP1, 143, 21, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_NetCutPulse,
					(float)PF_GW302_NetCutPulse[PF_MIN], (float)PF_GW302_NetCutPulse[PF_MAX],
					1.0, DP0, 143, 31, DW2, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_NetCutTime,
					(float)PF_GW302_NetCutTime[PF_MIN], (float)PF_GW302_NetCutTime[PF_MAX],
					1.0, DP1, 143, 41, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_MinDiaPulsPerLay,
					(float)PF_GW302_MinDiaPulsPerLay[PF_MIN], (float)PF_GW302_MinDiaPulsPerLay[PF_MAX],
					1.0, DP1, 143, 51, DW5, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_MaxDiaPulsPerLay,
					(float)PF_GW302_MaxDiaPulsPerLay[PF_MIN], (float)PF_GW302_MaxDiaPulsPerLay[PF_MAX],
					1.0, DP1, 143, 61, DW5, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_NetFactor,
					(float)PF_GW302_NetFactor[PF_MIN], (float)PF_GW302_NetFactor[PF_MAX],
					1.0, DP1, 143, 71, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_NoNetPulse,
					(float)PF_GW302_NoNetPulse[PF_MIN], (float)PF_GW302_NoNetPulse[PF_MAX],
					1.0, DP1, 143, 81, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
			if(pFac.pf.GW302_NetTwineOption == TO_NET)
			{
				display_number( (float)pFac.pf.GW302_PreNetPulse,
					(float)PF_GW302_PreNetPulse[PF_MIN], (float)PF_GW302_PreNetPulse[PF_MAX],
					1.0, DP0, 143, 91, DW2, RIGHT, &RDSfnt1, NULL, PNULL );
			}
			else
			{
				display_f1_text( "0", TSMAX, 143, 91, FIXED );
			}
		}
		else /* page == 1 */
				{
			if( pFac.pf.GW302_NetSystem == NS_BEAK )
				display_f1_text( "Beak  ", TSMAX, 119, 21, FIXED );
			else /* pFac.pf.GW302_NetSystem == NS_ROLLER */
					display_f1_text( "Roller", TSMAX, 119, 21, FIXED );

			display_number( (float)pFac.pf.GW302_BeakEngage,
					(float)PF_GW302_BeakEngage[PF_MIN], (float)PF_GW302_BeakEngage[PF_MAX],
					1.0, DP0, 143, 31, DW2, RIGHT, &RDSfnt1, NULL, PNULL );

			display_number( (float)pFac.pf.GW302_EngageDiameter,
					(float)PF_GW302_EngageDiameter[PF_MIN], (float)PF_GW302_EngageDiameter[PF_MAX],
					1.0, DP0, 142, 41, DW3, RIGHT, &RDSfnt1, NULL, PNULL );


				}
		update_values = 0;

	} // End of update_values

	if( update_selection )
	{
		// Tabs
		draw_box( adm, 1, 102, 158, 126, CLEAR ); // Clear area underneath tabs
		if( page == 0 )
		{
			switch( selection ) // Drawn according to position of selection pointer
			{
			case 0:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_DL,  62, 102, OR );    // NAVIGATION: Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;

			case 7:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UL,  62, 102, OR );    // NAVIGATION: Up Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;
			}
		}
		else /* page == 1 */
		{
			switch( selection ) // Drawn according to position of selection pointer
			{
			case 0:
			case 1:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_DL,  62, 102, OR );    // NAVIGATION: Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;

			case 2:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UL,  62, 102, OR );    // NAVIGATION: Up Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;
			}
		}

		// Selection pointer
		draw_box( adm, 16, 21 , 20, 100, CLEAR );
		write_bitmap( adm, F1_POINT, 16, 21+(10*selection), OR );

		update_selection = 0;

	} // End of update_selection

}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen144
// Description         : Technician Menu - Diameter Setup
// -----------------------------------------------------------
void screen144( unsigned int lkey )
{
	char update_disp;
	char update_selection;
	char update_values;

	if( !startup )
	{
		startup = 1;

		draw_frame( adm );

		display_f1_text( "Diameter Setup", TSMAX, 80, 7, FIXED | CENTER | UNDERLINED );

		selection = 0;
		update_disp = 1;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	if( !editing && (lkey & (SW_NEW | SW_REP)) )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_OL ) // -
		{
			switch( selection )
			{
			case 0:
				if( pFac.pf.GW302_DiameterMin > PF_GW302_DiameterMin[PF_MIN] ) // Limiting unsigned type to minimum value
					pFac.pf.GW302_DiameterMin -= PF_GW302_DiameterMin[PF_STEP];
				break;

			case 1:
				if( pFac.pf.GW302_DiameterMax > PF_GW302_DiameterMax[PF_MIN] ) // Limiting unsigned type to minimum value
					pFac.pf.GW302_DiameterMax -= PF_GW302_DiameterMax[PF_STEP];
				break;

			case 2:
				pFac.pf.GW302_DiaCorrection -= PF_GW302_DiaCorrection[PF_STEP];
				if( pFac.pf.GW302_DiaCorrection < PF_GW302_DiaCorrection[PF_MIN] )
					pFac.pf.GW302_DiaCorrection = PF_GW302_DiaCorrection[PF_MIN]; // Limiting to minimum value
				break;

			case 3:
				if( pFac.pf.GW302_BaleDiaSmoothingFactor > PF_GW302_BaleDiaSmoothingFactor[PF_MIN] ) // Limiting unsigned type to minimum value
					pFac.pf.GW302_BaleDiaSmoothingFactor -= PF_GW302_BaleDiaSmoothingFactor[PF_STEP];
				break;

			case 4:
				if( pFac.pf.GW302_DensityPresSmoothingFactor > PF_GW302_DensityPresSmoothingFactor[PF_MIN] ) // Limiting unsigned type to minimum value
					pFac.pf.GW302_DensityPresSmoothingFactor -= PF_GW302_DensityPresSmoothingFactor[PF_STEP];
				break;

			case 5:
				if( pFac.pf.GW302_BaleShapeSmoothingFactor >  PF_GW302_BaleShapeSmoothingFactor[PF_MIN] ) // Limiting unsigned type to minimum value
					pFac.pf.GW302_BaleShapeSmoothingFactor -=  PF_GW302_BaleShapeSmoothingFactor[PF_STEP];
				break;

			case 6:
				if( pFac.pf.GW302_KnifePresSmoothingFactor > PF_GW302_KnifePresSmoothingFactor[PF_MIN] ) // Limiting unsigned type to minimum value
					pFac.pf.GW302_KnifePresSmoothingFactor -= PF_GW302_KnifePresSmoothingFactor[PF_STEP];
				break;
			}
			SavecalDelayWithinMenus = 1;
			valuechanged = 1;
			update_values = 1;
		}
		else if( lkey == SW_IL ) // +
		{
			switch( selection )
			{
			case 0:
				pFac.pf.GW302_DiameterMin += PF_GW302_DiameterMin[PF_STEP];
				if( pFac.pf.GW302_DiameterMin > PF_GW302_DiameterMin[PF_MAX] )
					pFac.pf.GW302_DiameterMin = PF_GW302_DiameterMin[PF_MAX]; // Limiting to maximum value
				break;

			case 1:
				pFac.pf.GW302_DiameterMax += PF_GW302_DiameterMax[PF_STEP];
				if( pFac.pf.GW302_DiameterMax > PF_GW302_DiameterMax[PF_MAX] )
					pFac.pf.GW302_DiameterMax = PF_GW302_DiameterMax[PF_MAX]; // Limiting to maximum value
				break;

			case 2:
				pFac.pf.GW302_DiaCorrection += PF_GW302_DiaCorrection[PF_STEP];
				if( pFac.pf.GW302_DiaCorrection > PF_GW302_DiaCorrection[PF_MAX] )
					pFac.pf.GW302_DiaCorrection = PF_GW302_DiaCorrection[PF_MAX]; // Limiting to maximum value
				break;

			case 3:
				pFac.pf.GW302_BaleDiaSmoothingFactor += PF_GW302_BaleDiaSmoothingFactor[PF_STEP];
				if( pFac.pf.GW302_BaleDiaSmoothingFactor > PF_GW302_BaleDiaSmoothingFactor[PF_MAX] )
					pFac.pf.GW302_BaleDiaSmoothingFactor = PF_GW302_BaleDiaSmoothingFactor[PF_MAX]; // Limiting to maximum value
				break;

			case 4:
				pFac.pf.GW302_DensityPresSmoothingFactor += PF_GW302_DensityPresSmoothingFactor[PF_STEP];
				if( pFac.pf.GW302_DensityPresSmoothingFactor > PF_GW302_DensityPresSmoothingFactor[PF_MAX] )
					pFac.pf.GW302_DensityPresSmoothingFactor = PF_GW302_DensityPresSmoothingFactor[PF_MAX]; // Limiting to maximum value
				break;

			case 5:
				pFac.pf.GW302_BaleShapeSmoothingFactor +=  PF_GW302_BaleShapeSmoothingFactor[PF_STEP];
				if( pFac.pf.GW302_BaleShapeSmoothingFactor >  PF_GW302_BaleShapeSmoothingFactor[PF_MAX] )
					pFac.pf.GW302_BaleShapeSmoothingFactor =  PF_GW302_BaleShapeSmoothingFactor[PF_MAX]; // Limiting to maximum value
				break;

			case 6:
				pFac.pf.GW302_KnifePresSmoothingFactor += PF_GW302_KnifePresSmoothingFactor[PF_STEP];
				if( pFac.pf.GW302_KnifePresSmoothingFactor > PF_GW302_KnifePresSmoothingFactor[PF_MAX] )
					pFac.pf.GW302_KnifePresSmoothingFactor = PF_GW302_KnifePresSmoothingFactor[PF_MAX]; // Limiting to maximum value
				break;
			}
			SavecalDelayWithinMenus = 1;
			valuechanged = 1;
			update_values = 1;
		}
		else if( lkey == SW_CU ) // ^
		{
			if( selection )
			{
				selection--;
				update_selection = 1;
			}
		}
		else if( lkey == SW_CD ) // V
		{
			if( selection < 6 )
			{
				selection++;
				update_selection = 1;
			}
		}
		else if( (lkey == SW_OR) || (lkey == SW_CL) ) // ESC /  <
		{
			escape = 1;
		}
		else if( lkey == SW_IR ) // BLANK
		{
			// nothing
		}
	}
	else if( !editing ) // Not editing and no keys being pressed
	{
		// Saving Programmable Factors on releasing the keys for greater than 300 ms
		if( SavecalDelayOnMainDisplay || SavecalDelayWithinMenus )
		{
			if( swrel > 3 ) // > 3 x 100 ms
			{
				SavecalDelayOnMainDisplay = 0;
				SavecalDelayWithinMenus = 0;
				savecal = 1;
			}
		}
	}


	if( update_disp )
	{
		// Clear display area covering factor names, values & units
		draw_box( adm, (20+1), 21, 156, 100, CLEAR );

		// Factor names & units
		display_f1_text( "Diameter Min", TSMAX, 24, 21, FIXED );
		display_f1_text( "%", TSMAX, 146, 21, FIXED );

		display_f1_text( "Diameter Max", TSMAX, 24, 31, FIXED );
		display_f1_text( "%", TSMAX, 146, 31, FIXED );

		display_f1_text( "Dia. Correction", TSMAX, 24, 41, FIXED );
		display_f1_text( "V", TSMAX, 146, 41, FIXED );

		// Bale Diameter (AnIP1) Smoothing Factor
		display_f1_text( "BaleDia  AnIP1 SF", TSMAX, 24, 51, FIXED );
		display_f1_text( "%", TSMAX, 146, 51, FIXED );

		// (Bale) Density Pressure (AnIP2) Smoothing Factor
		display_f1_text( "DensPres AnIP2 SF", TSMAX, 24, 61, FIXED );
		display_f1_text( "%", TSMAX, 146, 61, FIXED );

		// Bale Shape Indicator (AnIP3) Smoothing Factor
		display_f1_text( "BaleShapeAnIP3 SF", TSMAX, 24, 71, FIXED );
		display_f1_text( "%", TSMAX, 146, 71, FIXED );

		// Knife Pressure (AnIP4) Smoothing Factor
		display_f1_text( "KnifePresAnIP4 SF", TSMAX, 24, 81, FIXED );
		display_f1_text( "%", TSMAX, 146, 81, FIXED );

		display_f1_text( "(1%=MaxSmoothingFactor)", TSMAX, 21, 91, FIXED );


		update_disp = 0;

		update_values = 1;    // Display factor values
		update_selection = 1; // Display tabs & selection pointer

	} // End of update_disp

	if( update_values )
	{
		// Factor values
		display_number( (float)pFac.pf.GW302_DiameterMin,
				(float)PF_GW302_DiameterMin[PF_MIN], (float)PF_GW302_DiameterMin[PF_MAX],
				1.0, DP0, 143, 21, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
		display_number( (float)pFac.pf.GW302_DiameterMax,
				(float)PF_GW302_DiameterMax[PF_MIN], (float)PF_GW302_DiameterMax[PF_MAX],
				1.0, DP0, 143, 31, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
		display_number( (float)pFac.pf.GW302_DiaCorrection,
				(float)PF_GW302_DiaCorrection[PF_MIN], (float)PF_GW302_DiaCorrection[PF_MAX],
				1.0, DP2, 143, 41, DW4, RIGHT, &RDSfnt1, NULL, PNULL );
		display_number( (float)pFac.pf.GW302_BaleDiaSmoothingFactor,
				(float)PF_GW302_BaleDiaSmoothingFactor[PF_MIN], (float)PF_GW302_BaleDiaSmoothingFactor[PF_MAX],
				1.0, DP0, 143, 51, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
		display_number( (float)pFac.pf.GW302_DensityPresSmoothingFactor,
				(float)PF_GW302_DensityPresSmoothingFactor[PF_MIN], (float)PF_GW302_DensityPresSmoothingFactor[PF_MAX],
				1.0, DP0, 143, 61, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
		display_number( (float)pFac.pf.GW302_BaleShapeSmoothingFactor,
				(float)PF_GW302_BaleShapeSmoothingFactor[PF_MIN], (float)PF_GW302_BaleShapeSmoothingFactor[PF_MAX],
				1.0, DP0, 143, 71, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
		display_number( (float)pFac.pf.GW302_KnifePresSmoothingFactor,
				(float)PF_GW302_KnifePresSmoothingFactor[PF_MIN], (float)PF_GW302_KnifePresSmoothingFactor[PF_MAX],
				1.0, DP0, 143, 81, DW3, RIGHT, &RDSfnt1, NULL, PNULL );

		update_values = 0;

	} // End of update_values

	if( update_selection )
	{
		// Tabs
		draw_box( adm, 1, 102, 158, 126, CLEAR ); // Clear area underneath tabs
		switch( selection ) // Drawn according to position of selection pointer
		{
		case 0:
			write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
			write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
			write_bitmap( adm, TABNAV_DL,  62, 102, OR );    // NAVIGATION: Down Left
			write_bitmap( adm, TAB,        99, 107, OR );
			write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
			break;

		case 1:
			write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
			write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
			write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
			write_bitmap( adm, TAB,        99, 107, OR );
			write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
			break;

		case 2:
			write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
			write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
			write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
			write_bitmap( adm, TAB,        99, 107, OR );
			write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
			break;

		case 3:
			write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
			write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
			write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
			write_bitmap( adm, TAB,        99, 107, OR );
			write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
			break;

		case 4:
			write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
			write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
			write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
			write_bitmap( adm, TAB,        99, 107, OR );
			write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
			break;

		case 5:
			write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
			write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
			write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
			write_bitmap( adm, TAB,        99, 107, OR );
			write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
			break;

		case 6:
			write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
			write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
			write_bitmap( adm, TABNAV_UL,  62, 102, OR );    // NAVIGATION: Up Left
			write_bitmap( adm, TAB,        99, 107, OR );
			write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
			break;
		}

		// Selection pointer
		draw_box( adm, 16, 21 , 20, 100, CLEAR );
		write_bitmap( adm, F1_POINT, 16, 21+(10*selection), OR );

		update_selection = 0;

	} // End of update_selection
}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen145
// Description         : Technician Menu - Density Setup
// -----------------------------------------------------------
void screen145( unsigned int lkey )
{
	char update_disp;
	char update_selection;
	char update_values;

	static char page;

	if( !startup )
	{
		startup = 1;

		draw_frame( adm );

		display_f1_text( "Density Setup", TSMAX, 80, 7, FIXED | CENTER | UNDERLINED );

		page = 0;
		selection = 0;
		update_disp = 1;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	if( !editing && (lkey & (SW_NEW | SW_REP)) )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_OL ) // -
		{
			if( page == 0 )
			{
				switch( selection )
				{
				case 0:
					pFac.pf.GW302_Density1 -= PF_GW302_Density1[PF_STEP];
					if( pFac.pf.GW302_Density1 < PF_GW302_Density1[PF_MIN] )
						pFac.pf.GW302_Density1 = PF_GW302_Density1[PF_MIN]; // Limiting to minimum value
					break;

				case 1:
					pFac.pf.GW302_Density2 -= PF_GW302_Density2[PF_STEP];
					if( pFac.pf.GW302_Density2 < PF_GW302_Density2[PF_MIN] )
						pFac.pf.GW302_Density2 = PF_GW302_Density2[PF_MIN]; // Limiting to minimum value
					break;

				case 2:
					pFac.pf.GW302_Density3 -= PF_GW302_Density3[PF_STEP];
					if( pFac.pf.GW302_Density3 < PF_GW302_Density3[PF_MIN] )
						pFac.pf.GW302_Density3 = PF_GW302_Density3[PF_MIN]; // Limiting to minimum value
					break;

				case 3:
					pFac.pf.GW302_Density4 -= PF_GW302_Density4[PF_STEP];
					if( pFac.pf.GW302_Density4 < PF_GW302_Density4[PF_MIN] )
						pFac.pf.GW302_Density4 = PF_GW302_Density4[PF_MIN]; // Limiting to minimum value
					break;

				case 4:
					pFac.pf.GW302_Density5 -= PF_GW302_Density5[PF_STEP];
					if( pFac.pf.GW302_Density5 < PF_GW302_Density5[PF_MIN] )
						pFac.pf.GW302_Density5 = PF_GW302_Density5[PF_MIN]; // Limiting to minimum value
					break;

				case 5:
					pFac.pf.GW302_Density6 -= PF_GW302_Density6[PF_STEP];
					if( pFac.pf.GW302_Density6 < PF_GW302_Density6[PF_MIN] )
						pFac.pf.GW302_Density6 = PF_GW302_Density6[PF_MIN]; // Limiting to minimum value
					break;
				}
			}
			else /* page == 1 */
			{
				switch( selection )
				{
				case 0:
					pFac.pf.GW302_Density7 -= PF_GW302_Density7[PF_STEP];
					if( pFac.pf.GW302_Density7 < PF_GW302_Density7[PF_MIN] )
						pFac.pf.GW302_Density7 = PF_GW302_Density7[PF_MIN]; // Limiting to minimum value
					break;

				case 1:
					pFac.pf.GW302_Density8 -= PF_GW302_Density8[PF_STEP];
					if( pFac.pf.GW302_Density8 < PF_GW302_Density8[PF_MIN] )
						pFac.pf.GW302_Density8 = PF_GW302_Density8[PF_MIN]; // Limiting to minimum value
					break;

				case 2:
					pFac.pf.GW302_Density9 -= PF_GW302_Density9[PF_STEP];
					if( pFac.pf.GW302_Density9 < PF_GW302_Density9[PF_MIN] )
						pFac.pf.GW302_Density9 = PF_GW302_Density9[PF_MIN]; // Limiting to minimum value
					break;

				case 3:
					pFac.pf.GW302_Density10 -= PF_GW302_Density10[PF_STEP];
					if( pFac.pf.GW302_Density10 < PF_GW302_Density10[PF_MIN] )
						pFac.pf.GW302_Density10 = PF_GW302_Density10[PF_MIN]; // Limiting to minimum value
					break;
				}
			}
			SavecalDelayWithinMenus = 1;
			valuechanged = 1;
			update_values = 1;
		}
		else if( lkey == SW_IL ) // +
		{
			if( page == 0 )
			{
				switch( selection )
				{
				case 0:
					pFac.pf.GW302_Density1 += PF_GW302_Density1[PF_STEP];
					if( pFac.pf.GW302_Density1 > PF_GW302_Density1[PF_MAX] )
						pFac.pf.GW302_Density1 = PF_GW302_Density1[PF_MAX]; // Limiting to maximum value
					break;

				case 1:
					pFac.pf.GW302_Density2 += PF_GW302_Density2[PF_STEP];
					if( pFac.pf.GW302_Density2 > PF_GW302_Density2[PF_MAX] )
						pFac.pf.GW302_Density2 = PF_GW302_Density2[PF_MAX]; // Limiting to maximum value
					break;

				case 2:
					pFac.pf.GW302_Density3 += PF_GW302_Density3[PF_STEP];
					if( pFac.pf.GW302_Density3 > PF_GW302_Density3[PF_MAX] )
						pFac.pf.GW302_Density3 = PF_GW302_Density3[PF_MAX]; // Limiting to maximum value
					break;

				case 3:
					pFac.pf.GW302_Density4 += PF_GW302_Density4[PF_STEP];
					if( pFac.pf.GW302_Density4 > PF_GW302_Density4[PF_MAX] )
						pFac.pf.GW302_Density4 = PF_GW302_Density4[PF_MAX]; // Limiting to maximum value
					break;

				case 4:
					pFac.pf.GW302_Density5 += PF_GW302_Density5[PF_STEP];
					if( pFac.pf.GW302_Density5 > PF_GW302_Density5[PF_MAX] )
						pFac.pf.GW302_Density5 = PF_GW302_Density5[PF_MAX]; // Limiting to maximum value
					break;

				case 5:
					pFac.pf.GW302_Density6 += PF_GW302_Density6[PF_STEP];
					if( pFac.pf.GW302_Density6 > PF_GW302_Density6[PF_MAX] )
						pFac.pf.GW302_Density6 = PF_GW302_Density6[PF_MAX]; // Limiting to maximum value
					break;
				}
			}
			else /* page == 1 */
			{
				switch( selection )
				{
				case 0:
					pFac.pf.GW302_Density7 += PF_GW302_Density7[PF_STEP];
					if( pFac.pf.GW302_Density7 > PF_GW302_Density7[PF_MAX] )
						pFac.pf.GW302_Density7 = PF_GW302_Density7[PF_MAX]; // Limiting to maximum value
					break;

				case 1:
					pFac.pf.GW302_Density8 += PF_GW302_Density8[PF_STEP];
					if( pFac.pf.GW302_Density8 > PF_GW302_Density8[PF_MAX] )
						pFac.pf.GW302_Density8 = PF_GW302_Density8[PF_MAX]; // Limiting to maximum value
					break;

				case 2:
					pFac.pf.GW302_Density9 += PF_GW302_Density9[PF_STEP];
					if( pFac.pf.GW302_Density9 > PF_GW302_Density9[PF_MAX] )
						pFac.pf.GW302_Density9 = PF_GW302_Density9[PF_MAX]; // Limiting to maximum value
					break;

				case 3:
					pFac.pf.GW302_Density10 += PF_GW302_Density10[PF_STEP];
					if( pFac.pf.GW302_Density10 > PF_GW302_Density10[PF_MAX] )
						pFac.pf.GW302_Density10 = PF_GW302_Density10[PF_MAX]; // Limiting to maximum value
					break;
				}
			}
			SavecalDelayWithinMenus = 1;
			valuechanged = 1;
			update_values = 1;
		}
		else if( lkey == SW_CU ) // ^
		{
			if( selection )
			{
				selection--;
				update_selection = 1;
			}
		}
		else if( lkey == SW_CD ) // V
		{
			if( page == 0 )
			{
				if( selection < 5 )
				{
					selection++;
					update_selection = 1;
				}
			}
			else /* page == 1 */
			{
				if( selection < 3 )
				{
					selection++;
					update_selection = 1;
				}
			}
		}
		else if( (lkey == SW_OR) || (lkey == SW_CL) ) // ESC /  <
		{
			escape = 1;
		}
		else if( lkey == SW_IR ) // PAGE
		{
			page = page? 0:1;

			selection = 0;
			update_disp = 1;
		}
	}
	else if( !editing ) // Not editing and no keys being pressed
	{
		// Saving Programmable Factors on releasing the keys for greater than 300 ms
		if( SavecalDelayOnMainDisplay || SavecalDelayWithinMenus )
		{
			if( swrel > 3 ) // > 3 x 100 ms
			{
				SavecalDelayOnMainDisplay = 0;
				SavecalDelayWithinMenus = 0;
				savecal = 1;
			}
		}
	}


	if( update_disp )
	{
		// Page number
		display_f1_text( (page==0)? "1":"2", TSMAX, 134, 7, FIXED );
		display_f1_text( "/2", TSMAX, 140, 7, FIXED );

		// Clear display area covering factor names, values & units
		draw_box( adm, (20+1), 21, 156, 100, CLEAR );

		// Factors names & units
		if( page == 0 )
		{
			display_f1_text( "Density 1", TSMAX, 24, 21, FIXED );
			display_f1_text( "V", TSMAX, 145, 21, FIXED );

			display_f1_text( "Density 2", TSMAX, 24, 31, FIXED );
			display_f1_text( "V", TSMAX, 145, 31, FIXED );

			display_f1_text( "Density 3", TSMAX, 24, 41, FIXED );
			display_f1_text( "V", TSMAX, 145, 41, FIXED );

			display_f1_text( "Density 4", TSMAX, 24, 51, FIXED );
			display_f1_text( "V", TSMAX, 145, 51, FIXED );

			display_f1_text( "Density 5", TSMAX, 24, 61, FIXED );
			display_f1_text( "V", TSMAX, 145, 61, FIXED );

			display_f1_text( "Density 6", TSMAX, 24, 71, FIXED );
			display_f1_text( "V", TSMAX, 145, 71, FIXED );
		}
		else /* page == 1 */
		{
			display_f1_text( "Density 7", TSMAX, 24, 21, FIXED );
			display_f1_text( "V", TSMAX, 145, 21, FIXED );

			display_f1_text( "Density 8", TSMAX, 24, 31, FIXED );
			display_f1_text( "V", TSMAX, 145, 31, FIXED );

			display_f1_text( "Density 9", TSMAX, 24, 41, FIXED );
			display_f1_text( "V", TSMAX, 145, 41, FIXED );

			display_f1_text( "Density 10", TSMAX, 24, 51, FIXED );
			display_f1_text( "V", TSMAX, 145, 51, FIXED );
		}
		update_disp = 0;

		update_values = 1;    // Display factor values
		update_selection = 1; // Display tabs & selection pointer

	} // End of update_disp

	if( update_values )
	{
		// Factor values
		if( page == 0 )
		{
			display_number( (float)pFac.pf.GW302_Density1,
					(float)PF_GW302_Density1[PF_MIN], (float)PF_GW302_Density1[PF_MAX],
					1.0, DP2, 142, 21, DW5, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_Density2,
					(float)PF_GW302_Density2[PF_MIN], (float)PF_GW302_Density2[PF_MAX],
					1.0, DP2, 142, 31, DW5, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_Density3,
					(float)PF_GW302_Density3[PF_MIN], (float)PF_GW302_Density3[PF_MAX],
					1.0, DP2, 142, 41, DW5, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_Density4,
					(float)PF_GW302_Density4[PF_MIN], (float)PF_GW302_Density4[PF_MAX],
					1.0, DP2, 142, 51, DW5, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_Density5,
					(float)PF_GW302_Density5[PF_MIN], (float)PF_GW302_Density5[PF_MAX],
					1.0, DP2, 142, 61, DW5, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_Density6,
					(float)PF_GW302_Density6[PF_MIN], (float)PF_GW302_Density6[PF_MAX],
					1.0, DP2, 142, 71, DW5, RIGHT, &RDSfnt1, NULL, PNULL );
		}
		else /* page == 1 */
				{
			display_number( (float)pFac.pf.GW302_Density7,
					(float)PF_GW302_Density7[PF_MIN], (float)PF_GW302_Density7[PF_MAX],
					1.0, DP2, 142, 21, DW5, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_Density8,
					(float)PF_GW302_Density8[PF_MIN], (float)PF_GW302_Density8[PF_MAX],
					1.0, DP2, 142, 31, DW5, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_Density9,
					(float)PF_GW302_Density9[PF_MIN], (float)PF_GW302_Density9[PF_MAX],
					1.0, DP2, 142, 41, DW5, RIGHT, &RDSfnt1, NULL, PNULL );
			display_number( (float)pFac.pf.GW302_Density10,
					(float)PF_GW302_Density10[PF_MIN], (float)PF_GW302_Density10[PF_MAX],
					1.0, DP2, 142, 51, DW5, RIGHT, &RDSfnt1, NULL, PNULL );
				}
		update_values = 0;

	} // End of update_values

	if( update_selection )
	{
		// Tabs
		draw_box( adm, 1, 102, 158, 126, CLEAR ); // Clear area underneath tabs
		if( page == 0 )
		{
			// Drawn according to position of selection pointer
			switch( selection )
			{
			case 0:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_DL,  62, 102, OR );    // NAVIGATION: Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;

			case 1:
			case 2:
			case 3:
			case 4:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;

			case 5:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UL,  62, 102, OR );    // NAVIGATION: Up Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;
			}
		}
		else /* page == 1 */
		{
			switch( selection ) // Drawn according to position of selection pointer
			{
			case 0:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_DL,  62, 102, OR );    // NAVIGATION: Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;

			case 1:
			case 2:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;

			case 3:
				write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
				write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
				write_bitmap( adm, TABNAV_UL,  62, 102, OR );    // NAVIGATION: Up Left
				write_bitmap( adm, TAB_PAGE,   99, 107, OR );    // PAGE
				display_f1_text( (page==0)? "2":"1", TSMAX, 99+19, 113, FIXED );
				write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
				break;
			}
		}

		// Selection pointer
		draw_box( adm, 16, 21 , 20, 100, CLEAR );
		write_bitmap( adm, F1_POINT, 16, 21+(10*selection), OR );

		update_selection = 0;

	} // End of update_selection

}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen146
// Description         : Technician Menu - Knives Setup
// -----------------------------------------------------------
void screen146( unsigned int lkey )
{
	char update_disp;
	char update_selection;
	char update_values;

	if( !startup )
	{
		startup = 1;

		draw_frame( adm );

		display_f1_text( "Knives Setup", TSMAX, 80, 7, FIXED | CENTER | UNDERLINED );

		selection = 0;
		update_disp = 1;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	if( !editing && (lkey & (SW_NEW | SW_REP)) )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_OL ) // -
		{
			switch( selection )
			{
			case 0:
				pFac.pf.GW302_KnifePressure -= PF_GW302_KnifePressure[PF_STEP];
				if( pFac.pf.GW302_KnifePressure < PF_GW302_KnifePressure[PF_MIN] )
					pFac.pf.GW302_KnifePressure = PF_GW302_KnifePressure[PF_MIN]; // Limiting to minimum value
				break;

			case 1:
				pFac.pf.GW302_KnifePresWarn -= PF_GW302_KnifePresWarn[PF_STEP];
				if( pFac.pf.GW302_KnifePresWarn < PF_GW302_KnifePresWarn[PF_MIN] )
					pFac.pf.GW302_KnifePresWarn = PF_GW302_KnifePresWarn[PF_MIN]; // Limiting to minimum value
				break;

			case 2:
				pFac.pf.GW302_DiverterDelay -= PF_GW302_DiverterDelay[PF_STEP];
				if( pFac.pf.GW302_DiverterDelay < PF_GW302_DiverterDelay[PF_MIN] )
					pFac.pf.GW302_DiverterDelay = PF_GW302_DiverterDelay[PF_MIN]; // Limiting to minimum value
				break;
			}
			SavecalDelayWithinMenus = 1;
			valuechanged = 1;
			update_values = 1;
		}
		else if( lkey == SW_IL ) // +
		{
			switch( selection )
			{
			case 0:
				pFac.pf.GW302_KnifePressure += PF_GW302_KnifePressure[PF_STEP];
				if( pFac.pf.GW302_KnifePressure > PF_GW302_KnifePressure[PF_MAX] )
					pFac.pf.GW302_KnifePressure = PF_GW302_KnifePressure[PF_MAX]; // Limiting to maximum value
				break;

			case 1:
				pFac.pf.GW302_KnifePresWarn += PF_GW302_KnifePresWarn[PF_STEP];
				if( pFac.pf.GW302_KnifePresWarn > PF_GW302_KnifePresWarn[PF_MAX] )
					pFac.pf.GW302_KnifePresWarn = PF_GW302_KnifePresWarn[PF_MAX]; // Limiting to maximum value
				break;

			case 2:
				pFac.pf.GW302_DiverterDelay += PF_GW302_DiverterDelay[PF_STEP];
				if( pFac.pf.GW302_DiverterDelay > PF_GW302_DiverterDelay[PF_MAX] )
					pFac.pf.GW302_DiverterDelay = PF_GW302_DiverterDelay[PF_MAX]; // Limiting to maximum value
				break;
			}
			SavecalDelayWithinMenus = 1;
			valuechanged = 1;
			update_values = 1;
		}
		else if( lkey == SW_CU ) // ^
		{
			if( selection )
			{
				selection--;
				update_selection = 1;
			}
		}
		else if( lkey == SW_CD ) // V
		{
			if( selection < 2 )
			{
				selection++;
				update_selection = 1;
			}
		}
		else if( (lkey == SW_OR) || (lkey == SW_CL) ) // ESC /  <
		{
			escape = 1;
		}
		else if( lkey == SW_IR ) // BLANK
		{
			// nothing
		}
	}
	else if( !editing ) // Not editing and no keys being pressed
	{
		// Saving Programmable Factors on releasing the keys for greater than 300 ms
		if( SavecalDelayOnMainDisplay || SavecalDelayWithinMenus )
		{
			if( swrel > 3 ) // > 3 x 100 ms
			{
				SavecalDelayOnMainDisplay = 0;
				SavecalDelayWithinMenus = 0;
				savecal = 1;
			}
		}
	}


	if( update_disp )
	{
		// Clear display area covering factor names, values & units
		draw_box( adm, (20+1), 21, 156, 100, CLEAR );

		// Factor names & units
		display_f1_text( "Knife Pressure",       TSMAX, 24, 21, FIXED );
		display_f1_text( "V", TSMAX, 146, 21, FIXED );

		display_f1_text( "Knife Pres. Warn",   TSMAX, 24, 31, FIXED );
		display_f1_text( "V", TSMAX, 146, 31, FIXED );

		display_f1_text( "Diverter Delay",   TSMAX, 24, 41, FIXED );
		display_f1_text( "s", TSMAX, 146, 41, FIXED );

		update_disp = 0;

		update_values = 1;    // Display factor values
		update_selection = 1; // Display tabs & selection pointer

	} // End of update_disp

	if( update_values )
	{
		// Factor values
		display_number( (float)pFac.pf.GW302_KnifePressure,
				(float)PF_GW302_KnifePressure[PF_MIN], (float)PF_GW302_KnifePressure[PF_MAX],
				1.0, DP2, 143, 21, DW4, RIGHT, &RDSfnt1, NULL, PNULL );
		display_number( (float)pFac.pf.GW302_KnifePresWarn,
				(float)PF_GW302_KnifePresWarn[PF_MIN], (float)PF_GW302_KnifePresWarn[PF_MAX],
				1.0, DP2, 143, 31, DW4, RIGHT, &RDSfnt1, NULL, PNULL );
		display_number( (float)pFac.pf.GW302_DiverterDelay,
				(float)PF_GW302_DiverterDelay[PF_MIN], (float)PF_GW302_DiverterDelay[PF_MAX],
				1.0, DP1, 143, 41, DW3, RIGHT, &RDSfnt1, NULL, PNULL );

		update_values = 0;

	} // End of update_values

	if( update_selection )
	{
		// Tabs
		draw_box( adm, 1, 102, 158, 126, CLEAR ); // Clear area underneath tabs
		switch( selection ) // Drawn according to position of selection pointer
		{
		case 0:
			write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
			write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
			write_bitmap( adm, TABNAV_DL,  62, 102, OR );    // NAVIGATION: Down Left
			write_bitmap( adm, TAB,        99, 107, OR );
			write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
			break;

		case 1:
			write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
			write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
			write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
			write_bitmap( adm, TAB,        99, 107, OR );
			write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
			break;

		case 2:
			write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
			write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
			write_bitmap( adm, TABNAV_UL,  62, 102, OR );    // NAVIGATION: Up Left
			write_bitmap( adm, TAB,        99, 107, OR );
			write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
			break;
		}

		// Selection pointer
		draw_box( adm, 16, 21 , 20, 100, CLEAR );
		write_bitmap( adm, F1_POINT, 16, 21+(10*selection), OR );

		update_selection = 0;

	} // End of update_selection

}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen147
// Description         : Technician Menu - Reset Defaults
// -----------------------------------------------------------
void screen147( unsigned int lkey )
{
	static unsigned char ResetFactors;

	if( !startup )
	{
		startup = 1;

		draw_frame( adm );

		display_f1_text( ptText[37], TSMAX, 80, 8, FIXED | CENTER | UNDERLINED ); // "Reset Defaults"

		display_f1_text( ptText[38], TSMAX, 32, 37, FIXED ); // "Press \001 to Reset"

		write_bitmap( adm, TAB,      0, 107, OR );
		write_bitmap( adm, TAB,     31, 107, OR );
		write_bitmap( adm, TABNAV_, 62, 102, OR ); // NAVIGATION: blank
		write_bitmap( adm, TABESC, 130, 107, OR ); // ESC

		leaf = 1;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	if( !editing && (lkey & (SW_NEW | SW_REP)) )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits
		if( lkey == SW_OR )
		{
			escape = 1;
			ResetFactors = 0;
		}
		else if( lkey == SW_IR ) // if enter key is pressed
		{
			if( !ResetFactors )
			{
				SetDefaultFactors();
				SetDefaultTotals();
				ResetFactors = 1;

				SavecalDelayOnMainDisplay = 0;
				SavecalDelayWithinMenus = 0;
			}
		}
	}

	if( ResetFactors )
	{
		draw_box( adm, 24, 37, 150, 47, CLEAR );
		// ptText[39] "Factors Reset to Defaults" replaced by 2 strings
		display_f1_text( "Factors Reset", TSMAX, 44, 37, FIXED );
		display_f1_text( "to Defaults",   TSMAX, 44, 47, FIXED );
	}

	draw_box( adm, 99, 107, 128, 126, CLEAR );
	write_bitmap( adm, ResetFactors? TAB:TABENT, 99, 107, OR );
}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen148
// Description         : Technician Menu - Formation 2
// -----------------------------------------------------------
void screen148( unsigned int lkey )
{
	char update_disp;
	char update_selection;
	char update_values;

	if( !startup )
	{
		startup = 1;

		draw_frame( adm );

		display_f1_text( "Formation 2", TSMAX, 80, 7, FIXED | CENTER | UNDERLINED );

		selection = 0;
		update_disp = 1;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	if( !editing && (lkey & (SW_NEW | SW_REP)) )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_OL ) // -
		{
			switch( selection )
			{
			case 0:
				pFac.pf.GW302_FormationDensity[pFac.pf.GW302_BaleProfile] -= PF_GW302_FormationDensity[PF_STEP];
				if( pFac.pf.GW302_FormationDensity[pFac.pf.GW302_BaleProfile] < PF_GW302_FormationDensity[PF_MIN] )
					pFac.pf.GW302_FormationDensity[pFac.pf.GW302_BaleProfile] = PF_GW302_FormationDensity[PF_MIN]; // Limiting to minimum value
				break;

			case 1:
				if( pFac.pf.GW302_FormationDiameter[pFac.pf.GW302_BaleProfile] > PF_GW302_FormationDiameter[PF_MIN] ) // Limiting unsigned type to minimum value
					pFac.pf.GW302_FormationDiameter[pFac.pf.GW302_BaleProfile] -= PF_GW302_FormationDiameter[PF_STEP];
				break;

			case 2:
				pFac.pf.GW302_Formation2Density[pFac.pf.GW302_BaleProfile] -= PF_GW302_Formation2Density[PF_STEP];
				if( pFac.pf.GW302_Formation2Density[pFac.pf.GW302_BaleProfile] < PF_GW302_Formation2Density[PF_MIN] )
					pFac.pf.GW302_Formation2Density[pFac.pf.GW302_BaleProfile] = PF_GW302_Formation2Density[PF_MIN]; // Limiting to minimum value
				break;

			case 3:
				if( pFac.pf.GW302_Formation2Diameter[pFac.pf.GW302_BaleProfile] > PF_GW302_Formation2Diameter[PF_MIN] ) // Limiting unsigned type to minimum value
					pFac.pf.GW302_Formation2Diameter[pFac.pf.GW302_BaleProfile] -= PF_GW302_Formation2Diameter[PF_STEP];
				break;
			}
			SavecalDelayWithinMenus = 1;
			valuechanged = 1;
			update_values = 1;
		}
		else if( lkey == SW_IL ) // +
		{
			switch( selection )
			{
			case 0:
				pFac.pf.GW302_FormationDensity[pFac.pf.GW302_BaleProfile] += PF_GW302_FormationDensity[PF_STEP];
				if( pFac.pf.GW302_FormationDensity[pFac.pf.GW302_BaleProfile] > PF_GW302_FormationDensity[PF_MAX] )
					pFac.pf.GW302_FormationDensity[pFac.pf.GW302_BaleProfile] = PF_GW302_FormationDensity[PF_MAX]; // Limiting to maximum value
				break;

			case 1:
				pFac.pf.GW302_FormationDiameter[pFac.pf.GW302_BaleProfile] += PF_GW302_FormationDiameter[PF_STEP];
				if( pFac.pf.GW302_FormationDiameter[pFac.pf.GW302_BaleProfile] > PF_GW302_FormationDiameter[PF_MAX] )
					pFac.pf.GW302_FormationDiameter[pFac.pf.GW302_BaleProfile] = PF_GW302_FormationDiameter[PF_MAX]; // Limiting to maximum value
				break;

			case 2:
				pFac.pf.GW302_Formation2Density[pFac.pf.GW302_BaleProfile] += PF_GW302_Formation2Density[PF_STEP];
				if( pFac.pf.GW302_Formation2Density[pFac.pf.GW302_BaleProfile] > PF_GW302_Formation2Density[PF_MAX] )
					pFac.pf.GW302_Formation2Density[pFac.pf.GW302_BaleProfile] = PF_GW302_Formation2Density[PF_MAX]; // Limiting to maximum value
				break;

			case 3:
				pFac.pf.GW302_Formation2Diameter[pFac.pf.GW302_BaleProfile] += PF_GW302_Formation2Diameter[PF_STEP];
				if( pFac.pf.GW302_Formation2Diameter[pFac.pf.GW302_BaleProfile] > PF_GW302_Formation2Diameter[PF_MAX] )
					pFac.pf.GW302_Formation2Diameter[pFac.pf.GW302_BaleProfile] = PF_GW302_Formation2Diameter[PF_MAX]; // Limiting to maximum value
				break;
			}
			SavecalDelayWithinMenus = 1;
			valuechanged = 1;
			update_values = 1;
		}
		else if( lkey == SW_CU ) // ^
		{
			if( selection )
			{
				selection--;
				update_selection = 1;
			}
		}
		else if( lkey == SW_CD ) // V
		{
			if( selection < 3 )
			{
				selection++;
				update_selection = 1;
			}
		}
		else if( (lkey == SW_OR) || (lkey == SW_CL) ) // ESC /  <
		{
			escape = 1;
		}
		else if( lkey == SW_IR ) // BLANK
		{
			// nothing
		}
	}
	else if( !editing ) // Not editing and no keys being pressed
	{
		// Saving Programmable Factors on releasing the keys for greater than 300 ms
		if( SavecalDelayOnMainDisplay || SavecalDelayWithinMenus )
		{
			if( swrel > 3 ) // > 3 x 100 ms
			{
				SavecalDelayOnMainDisplay = 0;
				SavecalDelayWithinMenus = 0;
				savecal = 1;
			}
		}
	}


	if( update_disp )
	{
		// Clear display area covering factor names, values & units
		draw_box( adm, (20+1), 21, 156, 100, CLEAR );

		// Factor names & units
		display_f1_text( "Formation Density", TSMAX, 24, 21, FIXED );

		display_f1_text( "Formation Dia.", TSMAX, 24, 31, FIXED );
		display_f1_text( "%", TSMAX, 149, 31, FIXED );

		display_f1_text( "Formation2 Density", TSMAX, 24, 41, FIXED );

		display_f1_text( "Formation2 Dia.", TSMAX, 24, 51, FIXED );
		display_f1_text( "%", TSMAX, 149, 51, FIXED );

		update_disp = 0;

		update_values = 1;    // Display factor values
		update_selection = 1; // Display tabs & selection pointer

	} // End of update_disp

	if( update_values )
	{
		// Factor values
		display_number( (float)pFac.pf.GW302_FormationDensity[pFac.pf.GW302_BaleProfile],
				(float)PF_GW302_FormationDensity[PF_MIN], (float)PF_GW302_FormationDensity[PF_MAX],
				1.0, DP0, 146, 21, DW2, RIGHT, &RDSfnt1, NULL, PNULL );
		display_number( (float)pFac.pf.GW302_FormationDiameter[pFac.pf.GW302_BaleProfile],
				(float)PF_GW302_FormationDiameter[PF_MIN], (float)PF_GW302_FormationDiameter[PF_MAX],
				1.0, DP0, 146, 31, DW3, RIGHT, &RDSfnt1, NULL, PNULL );
		display_number( (float)pFac.pf.GW302_Formation2Density[pFac.pf.GW302_BaleProfile],
				(float)PF_GW302_Formation2Density[PF_MIN], (float)PF_GW302_Formation2Density[PF_MAX],
				1.0, DP0, 146, 41, DW2, RIGHT, &RDSfnt1, NULL, PNULL );
		display_number( (float)pFac.pf.GW302_Formation2Diameter[pFac.pf.GW302_BaleProfile],
				(float)PF_GW302_Formation2Diameter[PF_MIN], (float)PF_GW302_Formation2Diameter[PF_MAX],
				1.0, DP0, 146, 51, DW3, RIGHT, &RDSfnt1, NULL, PNULL );

		update_values = 0;

	} // End of update_values

	if( update_selection )
	{
		// Tabs
		draw_box( adm, 1, 102, 158, 126, CLEAR ); // Clear area underneath tabs
		switch( selection ) // Drawn according to position of selection pointer
		{
		case 0:
			write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
			write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
			write_bitmap( adm, TABNAV_DL,  62, 102, OR );    // NAVIGATION: Down Left
			write_bitmap( adm, TAB,        99, 107, OR );
			write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
			break;

		case 1:
		case 2:
			write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
			write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
			write_bitmap( adm, TABNAV_UDL, 62, 102, OR );    // NAVIGATION: Up Down Left
			write_bitmap( adm, TAB,        99, 107, OR );
			write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
			break;

		case 3:
			write_bitmap( adm, TABMINUS,    0, 107, OR );    // -
			write_bitmap( adm, TABPLUS,    31, 107, OR );    // +
			write_bitmap( adm, TABNAV_UL,  62, 102, OR );    // NAVIGATION: Up Left
			write_bitmap( adm, TAB,        99, 107, OR );
			write_bitmap( adm, TABESC,    130, 107, OR );    // ESC
			break;
		}

		// Selection pointer
		draw_box( adm, 16, 21 , 20, 100, CLEAR );
		write_bitmap( adm, F1_POINT, 16, 21+(10*selection), OR );

		update_selection = 0;

	} // End of update_selection

}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen150
// Description         : Diagnostics Digital Inputs
// -----------------------------------------------------------
void screen150 ( unsigned int lkey )
{
	unsigned char c, tstring[32];
	static unsigned char swtim;
	if( !startup )
	{
		startup = 1;

		draw_frame( adm );

		display_f1_text( ptText[30], TSMAX, 80, 12,  FIXED | CENTER | UNDERLINED  ); // "Digital Inputs"

		// Tabs
		write_bitmap( adm, TAB,        0, 107, OR );
		write_bitmap( adm, TAB,       31, 107, OR );
		write_bitmap( adm, TABNAV_LR, 62, 102, OR ); // NAVIGATION: left right
#if 0 // 3rd tab drawn according to status of 'Chopping Knives Fitted' setting
		write_bitmap( adm, TAB,       99, 107, OR );
#endif
		write_bitmap( adm, TABESC,   130, 107, OR ); // ESC

		ResetSequenceFactors(SEQ_ALL);
		leaf = 1;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif
	if( lkey & (SW_NEW | SW_REP) )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits
		if( lkey == SW_CR )
		{
			main_window = screen152; // Ananlog inputs
		}
		else if( (lkey == SW_OR) || (lkey == SW_CL) )
		{
			SavecalDelayWithinMenus = 1;
			escape = 1;
		}
		else if( lkey == SW_IR ) // Toggling 'Chopping Knives Fitted' setting between enabled & disabled
		{
			if( pFac.pf.GW302_ChoppingKnivesFitted == CKF_ENABLED )
			{
				pFac.pf.GW302_ChoppingKnivesFitted = CKF_DISABLED;
			}
			else // if( pFac.pf.GW302_ChoppingKnivesFitted == CKF_DISABLED )
			{
				pFac.pf.GW302_ChoppingKnivesFitted = CKF_ENABLED;
			}
		}
		else if(lkey==SW_IL)
		{

			if( swtim < 15 )
			{
				swtim++;

				   if( swtim >= 10 )
				   {
					   //DisplayIP4PulseCounter = 1;
					  // IP4PulsesCount = 0;
				   }
			}
			else
			{
				swtim = 0;
			}


		}
	}


	// 3rd tab drawn according to status of 'Chopping Knives Fitted' setting
	draw_box( adm, 99, 107, 128, 126, CLEAR ); // Clear 3rd tab area
	if( pFac.pf.GW302_ChoppingKnivesFitted == CKF_ENABLED )
	{
		write_bitmap( adm, TABKNFON, 99, 107, OR );
	}
	else // if( pFac.pf.GW302_ChoppingKnivesFitted == CKF_DISABLED )
	{
		write_bitmap( adm, TABKNFOFF, 99, 107, OR );
	}


	draw_box( adm, 22, 36, 138, 70, CLEAR );
	draw_box( adm, 18, 78, 137, 100, CLEAR );

	display_f1_text( "1      2      3", TSMAX,  22, 38, FIXED );
	display_f1_text( "4      5      6", TSMAX,  22, 60, FIXED );

	c = (ExtIn & (1 << 0)) ? 1 : 0;
	write_bitmap( adm, ip_status[c], 31, 36, OR );

	c = (ExtIn & (1 << 1)) ? 1 : 0;
	write_bitmap( adm, ip_status[c], 75, 36, OR );

	c = (ExtIn & (1 << 2)) ? 1 : 0;
	write_bitmap( adm, ip_status[c], 117, 36, OR );

	c = (ExtIn & (1 << 3)) ? 1 : 0;
	write_bitmap( adm, ip_status[c], 31, 58, OR );

	c = (ExtIn & (1 << 4)) ? 1 : 0;
	write_bitmap( adm, ip_status[c], 75, 58, OR );

	c = (ExtIn & (1 << 5)) ? 1 : 0;
	write_bitmap( adm, ip_status[c], 117, 58, OR );



	// Displaying total net applied in metres
	/*
	sprintf( tstring, "%d", fIP[2].pcount ); // Default=0m; Increment=1m; Range=0-9999m
	display_f1_text( tstring, TSMAX, 3, 70, FIXED );

	sprintf( tstring, "%f", fIP[2].frequency ); // Default=0m; Increment=1m; Range=0-9999m
		display_f1_text( tstring, TSMAX, 3, 80, FIXED );
		*/

	/*

	// Displaying total net applied in metres
	sprintf( tstring, "%d", PulseCounter ); // Default=0m; Increment=1m; Range=0-9999m
	display_f1_text( tstring, TSMAX, 3, 80, FIXED );

	// Displaying total net applied in metres
	sprintf( tstring, "%d", fIP[3].pcount ); // Default=0m; Increment=1m; Range=0-9999m
	display_f1_text( tstring, TSMAX, 3, 90, FIXED );

	// Displaying total net applied in metres
	sprintf( tstring, "%d", tempL ); // Default=0m; Increment=1m; Range=0-9999m
	display_f1_text( tstring, TSMAX, 3, 100, FIXED );


	sprintf( tstring, "%d", fIP[3].pstore ); // Default=0m; Increment=1m; Range=0-9999m
	display_f1_text( tstring, TSMAX, 60, 90, FIXED );


*/

}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen152
// Description         : Diagnostics Analogue Inputs
// -----------------------------------------------------------
void screen152 ( unsigned int lkey )
{
	unsigned char tstring[32];
	float tmpF;
	static unsigned char c;

	if( !startup )
	{
		startup = 1;
		draw_frame( adm );
		display_f1_text( ptText[31], TSMAX, 80, 12,  FIXED | CENTER | UNDERLINED  ); // "Analogue Inputs"

		write_bitmap( adm, TAB,        0, 107, OR );
		write_bitmap( adm, TAB,       31, 107, OR );
		write_bitmap( adm, TABNAV_LR, 62, 102, OR ); // NAVIGATION: left right
		write_bitmap( adm, TAB,       99, 107, OR );
		write_bitmap( adm, TABESC,   130, 107, OR ); // ESC

		leaf = 1;
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif
	if( lkey & (SW_NEW | SW_REP) )
	{
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits
		if( lkey == SW_CL )
		{
			main_window = screen150;
		}
		else if( lkey == SW_CR )
		{
			main_window = screen151; /* Outputs */
		}
		else if( lkey == SW_OR )
		{
			escape = 1;
		}
	}

	// Clear area above tabs
	draw_box( adm, 1, 25, 158, 101, CLEAR );

	// Analogue inputs
	sprintf(tstring, ptText[32], 1);
	display_f1_text( tstring, TSMAX, 16, 35,  FIXED  ); // AnIP%u

	sprintf(tstring, ptText[32], 2);
	display_f1_text( tstring, TSMAX, 90, 35,  FIXED  ); // AnIP%u

	sprintf(tstring, ptText[32], 3);
	display_f1_text( tstring, TSMAX, 16, 69,  FIXED  ); // AnIP%u

	sprintf(tstring, ptText[32], 4);
	display_f1_text( tstring, TSMAX, 90, 69,  FIXED  ); // AnIP%u

	// AnIP1: Bale Diameter
	tmpF = (float)AtoDmux[0].smooth;
	tmpF = (float)AtoDmux[0].smooth * ATOD_IP_SCALE;



	sprintf( tstring, "%8.3fV", BaleDiaVolts );
	display_f1_text( tstring, TSMAX, 26, 47, FIXED );

	if( BaleDiaVolts >= POTDIV )
	{
		c = 0;
	}
	else if( BaleDiaVolts <= POTDIV)
	{
		c = 1;
	}
	write_bitmap( adm, ip_status[c], 48+3, 33, OR );

	// AnIP2: (Bale) Density Pressure


	sprintf( tstring, "%8.3fV", DensityPresVolts );
	display_f1_text( tstring, TSMAX, 100, 47, FIXED );

	if( DensityPresVolts >= POTDIV)
	{
		c = 0;
	}
	else if( DensityPresVolts <= POTDIV)
	{
		c = 1;
	}
	write_bitmap( adm, ip_status[c], 122+3, 33, OR );

	// AnIP3: Bale Shape Indicator

	sprintf( tstring, "%8.3fV", BaleShapeVolts );
	display_f1_text( tstring, TSMAX, 26, 81, FIXED );

	if( BaleShapeVolts >= POTDIV)
	{
		c = 0;
	}
	else if( BaleShapeVolts <= POTDIV )
	{
		c = 1;
	}
	write_bitmap( adm, ip_status[c], 48+3, 67, OR );

	// AnIP4: Knife Pressure
	sprintf( tstring, "%8.3fV", KnifePresVolts );
	display_f1_text( tstring, TSMAX, 100, 81, FIXED );

	if( KnifePresVolts >= POTDIV)
	{
		c = 0;
	}
	else if( KnifePresVolts <= POTDIV)
	{
		c = 1;
	}
	write_bitmap( adm, ip_status[c], 122+3, 67, OR );
}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen151
// Description         : Diagnostics Outputs
// -----------------------------------------------------------
void screen151 ( unsigned int lkey )
{
	unsigned char c, i;


	static unsigned char swtim;


	if(escapeFromOutputDiag)
	{
		escape = 1;
		escapeFromOutputDiag = 0;
	}


	if( !startup )
	{
		startup = 1;

		draw_frame( adm );

		display_f1_text( ptText[33], TSMAX, 80, 12,  FIXED | CENTER | UNDERLINED  ); // "Outputs"
		display_f1_text( "V", TSMAX, 149, 4,  FIXED );
		write_bitmap( adm, BATTSLIM, 108, 3, OR );

		write_bitmap( adm, TABLEFT,    0, 107, OR ); // <
		write_bitmap( adm, TABRIGHT,  31, 107, OR ); // >
		if(pFac.pf.GW302_NetTwineOption != TO_TWINE)
			write_bitmap( adm, TABNAV_L,  62, 102, OR ); // NAVIGATION: left only
		else
			write_bitmap( adm, TABNAV_LR, 62, 102, OR ); // NAVIGATION: left right
		write_bitmap( adm, TAB,       99, 107, OR );
		write_bitmap( adm, TABESC,   130, 107, OR ); // ESC

		leaf = 1;
	}

	for(i=0; i<8; i++)
	{
		SetOutputOUT(i, OUT_OFF);
		SetOutputPWM(i+2, 0, PWM_DUTY000);
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif


	 if( (lkey & SW_IL) || (lkey & SW_OL) )
	   {
	   if( swtim < 20 )
		 swtim++;

	   if( swtim >= 10 )
	   {

		   for(i=0; i<8; i++)
		   	 {
		   		 if(selection == i)
		   		 {
		   			SetOutputOUT(i, OUT_ON);
		   			 SetOutputPWM(i+2, 0, PWM_DUTY100);
		   		 }
		   		 else
		   		 {
		   			 SetOutputPWM(i+2, 0, PWM_DUTY000);
		   			 SetOutputOUT(i, OUT_OFF);
		   		 }
		   	 }
	     }
		 //SetOutputPWM( OP01 + selection, 1000, 100 ); //dont use this.
	   }
	 else
	   {
	   //SetOutputPWM( OP01 + selection, 1000, 0 );
		 swtim = 0;
	   }


	 if( lkey & (SW_NEW | SW_REP) )
		 {
			lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

			if( lkey == SW_CL )
			{
				main_window = screen152;
				/* turn on OP6 - 12V supply on the machine */
				SetOutputOUT(OUT_OP06, OUT_ON);
				SetOutputPWM(PWM_OP06, 0, PWM_DUTY100);

			}
			else if(lkey == SW_CR)
			{
				if(pFac.pf.GW302_NetTwineOption == TO_TWINE)
					main_window = CANDiagnostics;
			}
			else if( lkey == SW_OL )
			{
			  /*if( selection )
				selection--;*/
				if (swtim < 10)
					lastoutput = 1;
				else
					lastoutput = 0;
			}
			else if( lkey == SW_IL )
			{
				if (swtim < 10)
					nextoutput = 1;
				else
					nextoutput = 0;
			}
			else if( lkey == SW_OR )
			{
				escape = 1;
				/* turn on OP6 - 12V supply on the machine */
				SetOutputOUT(OUT_OP06, OUT_ON);
				SetOutputPWM(PWM_OP06, 0, PWM_DUTY100);

			}

		 }

		if( !lkey && nextoutput)
		  {
		  if( selection < 7 )
			selection++;

		  nextoutput = 0;
		  }

		if( !lkey && lastoutput)
		  {
			  if( selection )
			  selection--;

			  lastoutput = 0;
		  }

	//template = (pRun.pr.status[0] << 0) | (pRun.pr.status[1] << 1) | (pRun.pr.status[2] << 2) |
	//           (pRun.pr.status[3] << 3) | (pRun.pr.status[4] << 4) | (pRun.pr.status[5] << 5);

	draw_box( adm, 22, 28, 130, 58, CLEAR );

	display_f1_text( "1      2      3", TSMAX,  23, 30, FIXED );
	display_f1_text( "4      5      6", TSMAX,  22, 50, FIXED );
	display_f1_text( "7      8       ", TSMAX,  22, 70, FIXED );

	//tempf = ((float)AT91C_BASE_ADC0->ADC_CDR7 * ATODIPVOUTSCALE) + ATODIPVOUTDROP;
         #ifdef WINDOWS
	display_number( 13.5, 0.0, 0.0, 1.0, DP1, 147, 4, DW4, RIGHT, &RDSfnt1, NULL, PNULL );
         #endif
        #ifndef WINDOWS
	c = SetOutputOUT(OUT_OP01, OUT_STATUS) - 1;
        #else
        c = 0;
        #endif
	write_bitmap( adm, (selection==0)? op_status[c]: BUTTON03, 32, 28, OR );
        #ifndef WINDOWS
	c = SetOutputOUT(OUT_OP02, OUT_STATUS) - 1;
        #else
         c = 0;
        #endif
	write_bitmap( adm, (selection==1)? op_status[c]: BUTTON03, 77, 28, OR );
         #ifndef WINDOWS
	c = SetOutputOUT(OUT_OP03, OUT_STATUS) - 1;
        #else
         c = 0;
        #endif
	write_bitmap( adm, (selection==2)? op_status[c]: BUTTON03, 119, 28, OR );
         #ifndef WINDOWS
	c = SetOutputOUT(OUT_OP04, OUT_STATUS) - 1;
        #else
         c = 0;
        #endif
	write_bitmap( adm, (selection==3)? op_status[c]: BUTTON03, 32, 48, OR );
         #ifndef WINDOWS
	c = SetOutputOUT(OUT_OP05, OUT_STATUS) - 1;
       #else
         c = 0;
        #endif
	write_bitmap( adm, (selection==4)? op_status[c]: BUTTON03, 77, 48, OR );
         #ifndef WINDOWS
	c = SetOutputOUT(OUT_OP06, OUT_STATUS) - 1;
         #else
         c = 0;
        #endif
	write_bitmap( adm, (selection==5)? op_status[c]: BUTTON03, 119, 48, OR );
         #ifndef WINDOWS
	c = SetOutputOUT(OUT_OP07, OUT_STATUS) - 1;
         #else
         c = 0;
        #endif
	write_bitmap( adm, (selection==6)? op_status[c]: BUTTON03, 32, 68, OR );
         #ifndef WINDOWS
	c = SetOutputOUT(OUT_OP08, OUT_STATUS) - 1;
         #else
         c = 0;
        #endif
	write_bitmap( adm, (selection==7)? op_status[c]: BUTTON03, 77, 68, OR );
}
// -----------------------------------------------------------


// -----------------------------------------------------------
// Function Name       : screen99
// Description         : Power Down
// -----------------------------------------------------------
void screen99( unsigned int lkey )
{
	static unsigned char old_offtime;

	if( !startup )
	{
		startup = 1;

		draw_frame( adm );

		/*display_f1_text( "Graphic Wizard", TSMAX, 80, 4, FIXED | UNDERLINED | CENTER );*/

		write_bitmap( adm, TAB,   0, 107, OR );
		write_bitmap( adm, POWER,10, 111, OR );
		write_bitmap( adm, TAB,  31, 107, OR );
		write_bitmap( adm, TABNAV00, 62, 102, OR );
		write_bitmap( adm, TAB,  99, 107, OR );
		write_bitmap( adm, TAB, 130, 107, OR );

		//write_bitmap( adm, BAR, 31, 45, OR );

		display_f1_text( "I", TSMAX, 22, 47, FIXED );
		display_f1_text( "0", TSMAX, 135, 47, FIXED );
		//display_f1_text( "to", TSMAX, 80, 55, FIXED | CENTER );
		//display_f1_text( "Power Down", TSMAX, 80, 70, FIXED | CENTER );
		offtime = 0;
	}
	else if( lkey & SW_OL )
	{
		if( lkey & SW_NEW )
		{
			// set a common window to both modes
			//pRun.pr.windowSet = screen04;

			// toggle demo mode
			//pFac.pf.DemoType ^= DT_DEMOTYPE;

#ifndef WINDOWS
			// write factors to SPI1 Flash @ 0x000000
#endif
			offtime = 0;
		}
		else if( offtime > 18 )
		{
			// Resume Power task - save factors and turn off
			PowerSwitchedOff = 1;
#ifndef WINDOWS
			vTaskResume( POWERtaskhandle );

			// wait for 2 seconds
			vTaskDelay( 2000 / portTICK_RATE_MS );
#endif
		}
	}
	else if( lkey & SW_IL )
	{
		if( lkey & SW_NEW )
		{
			// toggle demo mode
			// pFac.pf.DemoType ^= DT_DEMOMENU;

#ifndef WINDOWS
			// write factors to SPI1 Flash @ 0x000000
#endif
		}
		offtime = 0;
	}
	else // restore window if key released
	{
		// check for valid default window and set default if none
		//checkwindowSet();
		// window = pRun.pr.windowSet;

		// while(1); // gives watchdog reset

		// window = (void(*)())0xffffffff; // gives undefine instruction abort

		// *((AT91_REG *)0x1FF80278) = 0xa5; // gives data abort
	}

#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif

	if( offtime != old_offtime )
	{
		draw_box( adm, 31, 45, 131, 55, CLEAR );
		write_bitmap( adm, BAR, 31, 45, OR );
		write_bitmap( adm, BRICK1, 32+(offtime*3), 46, OR );
		old_offtime = offtime;
	}
	//display_f1_text( (pFac.pf.DemoType & DT_DEMOTYPE) ? "IND" : "AGR", TSMAX,  0+5, 107+7, FIXED );

	//display_f1_text( (pFac.pf.DemoType & DT_DEMOMENU) ? "C&C" : "ALL", TSMAX, 31+5, 107+7, FIXED );

}
// -----------------------------------------------------------



// Clock Set
/*-----------------------------------------------------------*/
void screen710( unsigned int lkey )
{
	void *fac[6];
	unsigned short int *ps, t_year;
	unsigned char *pc, change, t_sec, t_min, t_hrs, t_day, t_date, t_month, tstring[8];
	
	if( !startup )
	{
		startup = 1;

		draw_frame( adm );

		display_f1_text( "CLOCK ADJUST", TSMAX, 80, 4, FIXED | CENTER );

		write_bitmap( adm, TAB,       0, 107, OR );
		write_bitmap( adm, TABESC,   31, 107, OR );
		write_bitmap( adm, TABCURS,  62, 102, OR );
		write_bitmap( adm, TAB,      99, 107, OR );
		write_bitmap( adm, TAB,     130, 107, OR );
	}

	// set temporary pointers  
	fac[0] = &t_hrs;
	fac[1] = &t_min;
	fac[2] = &t_day;
	fac[3] = &t_date;
	fac[4] = &t_month;
	fac[5] = &t_year;

	// set local time variables
	t_sec = bcdtob( ClockRam[0] & 0x7f );
	t_min = bcdtob( ClockRam[1] );
	t_hrs = bcdtob( ClockRam[2] & 0x3f );
	t_day = bcdtob( ClockRam[3] & 0x07 );
	if( t_day > 7 )
	{
		t_day = 0;
	}
	t_date = bcdtob( ClockRam[4] );
	t_month = bcdtob( ClockRam[5] & 0x1f );
	if( t_month > 12 )
	{
		t_month = 0;
	}
	t_year = 2000 + bcdtob( ClockRam[6] );
	
	change = 0;

	if( lkey & (SW_NEW | SW_REP) )
	{
		lkey &= ~(SW_NEW | SW_REP); // mask off status bits
		if( lkey == SW_CL )
		{
			switch( selection )
			{
			case 0: // hours
			case 1: // minutes
			case 2: // day
			case 3: // date
			case 4: // month
				pc = (unsigned char *)fac[selection];
				if( *pc > (unsigned char)clk_min[selection] )
				{
					(*pc)--;
				}
				else
				{
					*pc = (unsigned char)clk_max[selection];
				}        
				change = 1;        
				break;	

			case 5: // year
				ps = (unsigned short int *)fac[selection];
				if( *ps > clk_min[selection] )
				{
					(*ps)--;
				}
				else
				{
					*ps = clk_max[selection];
				}        
				change = 1;        
				break;
			}	
		}
		else if( lkey == SW_CR )
		{
			switch( selection )
			{
			case 0: // hours
			case 1: // minutes
			case 2: // day
			case 3: // date
			case 4: // month
				pc = (unsigned char *)fac[selection];
				if( *pc < (unsigned char)clk_max[selection] )
				{
					(*pc)++;
				}
				else
				{
					*pc = (unsigned char)clk_min[selection];
				}        
				change = 1;        
				break;	

			case 5: // year
				ps = (unsigned short int *)fac[selection];
				if( *ps < clk_max[selection] )
				{
					(*ps)++;
				}
				else
				{
					*ps = clk_min[selection];
				}        
				change = 1;        
				break;
			}	
		}
		else if( lkey == SW_CU )
		{
			if( selection )
			{
				selection--;
			}	
		}
		else if( lkey == SW_CD )
		{
			if( selection < 5 )
			{
				selection++;	
			}
		}
		else if( lkey == SW_IL )
		{
			escape = 1;
		}
	}  

	if( change )
	{
		// only reset seconds if adjusting minutes
		tstring[0] = (selection == 1) ? 0x00 : btobcd( t_sec ); // seconds
		tstring[1] = btobcd( t_min ); // minutes
		tstring[2] = btobcd( t_hrs ); // hours
		tstring[3] = btobcd( t_day ); // day of week
		tstring[4] = btobcd( t_date ); // day of month
		tstring[5] = btobcd( t_month ); // month
		tstring[6]  = btobcd( t_year % 100 ); // year

		SetCurrentTime( &tstring[0] );
	}

	display_f1_text( "Hours", TSMAX, 12, 25, FIXED );
	display_number( t_hrs, 0.0, 0.0, 1.0, 0, 120, 25, 20, RIGHT, &RDSfnt1, NULL, PNULL );

	display_f1_text( "Minutes", TSMAX, 12, 35, FIXED );
	display_number( t_min, 0.0, 0.0, 1.0, 0, 120, 35, 20, RIGHT, &RDSfnt1, NULL, PNULL );

	display_f1_text( "Day", TSMAX, 12, 45, FIXED );
	display_f1_text( DayofWeek[t_day], TSMAX, 103, 45, FIXED );

	display_f1_text( "Date", TSMAX, 12, 55, FIXED );
	display_number( t_date, 0.0, 0.0, 1.0, 0, 120, 55, 20, RIGHT, &RDSfnt1, NULL, PNULL );

	display_f1_text( "Month", TSMAX, 12, 65, FIXED );
	display_f1_text( MonthofYear[t_month], TSMAX, 103, 65, FIXED );

	display_f1_text( "Year", TSMAX, 12, 75, FIXED );
	display_number( t_year, 0.0, 0.0, 1.0, 0, 120, 75, 40, RIGHT, &RDSfnt1, NULL, PNULL );

	draw_box( adm, 2, 25, 2+5, 85+6, CLEAR );
	write_bitmap( adm, F1_POINT, 2, (selection * 10) + 25, OR );
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

// PIN check screen
/*-----------------------------------------------------------*/
void screenPIN( unsigned int lkey )
{
	static unsigned short int local_pin;
	
	if( !startup )
	{
		startup = 1;

		draw_frame( adm );

		display_f1_text( "Check PIN Number", TSMAX, 80, 8, FIXED | CENTER | UNDERLINED ); /* 'PIN Number' */

		//write_bitmap( adm, LOCK, 4, 30, OR );
		
		display_f1_text( "PIN:", TSMAX, 20, 30, FIXED ); /* 'Code' */

		write_bitmap( adm, TAB,      0, 107, OR );  
		write_bitmap( adm, TAB,     31, 107, OR );
		//write_bitmap( adm, TABCURS, 62, 102, OR );
		write_bitmap( adm, TAB,     99, 107, OR );
		write_bitmap( adm, TAB,    130, 107, OR );
		
		leaf = 1;
		local_pin = 0;
	}
	
	if( lkey & (SW_NEW | SW_REP) )
	{		
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits
		if( lkey == SW_IL )
		{
			escape = 1;
		}
	}

	display_number( (float)local_pin, 0.0, 9999.0, 1.0, DP0, 90, 30, DW4, EDIT | LEFT | EDSCAPE, &RDSfnt1, &local_pin, PSHORT );
	//display_number( (float)local_pin, 0.0, 9999.0, 1.0, DP0, 90+DW4, 30, DW4, EDIT | RIGHT | EDSCAPE, &RDSfnt1, &local_pin, PSHORT );

	// if EDSCAPE used for number entry, escape is set to 3 if value entered
	// could also use 'valuechanged', as this is always set if number changes, but it needs zeroing
	if( escape == 3 )
	{
		escape = 0;
		if( local_pin == active_pin )
		{
			main_window = pin_window;
			pin_window = NULL;
		}
		else
		{
			display_f1_text( "PIN Incorrect", TSMAX, 20, 60, FIXED ); /* 'PIN Incorrect' */
		}
	}

	if( editing )
	{
		draw_box( adm, 20, 60, 150, 66, CLEAR );
	}

}
/*-----------------------------------------------------------*/
unsigned char escapeFromOutputDiag;

void CANDiagnostics( unsigned int lkey  ) /* Diagnostics - HBM CAN module */
{

#ifndef WINDOWS
	unsigned char tstring[32], i;

	static unsigned char swtim;
	unsigned char c;

	if( startup != 1 )
	{
		startup = 1;

		draw_frame( adm );

		display_f1_text( "CAN Diagnostics", TSMAX, 80, 12, FIXED | CENTER | UNDERLINED ); /* CAN Diagnostics */

		write_bitmap( adm, TABLEFT,    0, 107, OR ); // <
		write_bitmap( adm, TABRIGHT,  31, 107, OR ); // >
		write_bitmap( adm, TABNAV_L,  62, 102, OR ); // NAVIGATION: left only
		write_bitmap( adm, TAB,       99, 107, OR );
		write_bitmap( adm, TABESC,   130, 107, OR ); // ESC

		leaf = 1;

	}

	for(i=0; i<2; i++)
	{
		HBM[0].HBM_Output[0].Duty[i] = HBM_OUT_OFF;
		HBM[0].HBM_Output[1].Duty[i] = HBM_OUT_OFF;
	}


#ifdef SHOW_TIME
	ClockDisplay( CL0CK_ICON_OFF ); // Time in format hh:mm
#endif



	if( (lkey & SW_IL) || (lkey & SW_OL) )
	{
		if( swtim < 20 )
			swtim++;

		if( swtim >= 10 )
		{

			for(i=0; i<2; i++)
			{
				if(selection<2)
				{
					if(selection == i)
					{
						HBM[0].HBM_Output[0].Duty[i] = HBM_OUT_ON;
					}
					else
					{
						HBM[0].HBM_Output[0].Duty[i] = HBM_OUT_OFF;
					}
				}
				else
				{
					if((selection - 2) == i)
					{
						HBM[0].HBM_Output[1].Duty[i] = HBM_OUT_ON;
					}
					else
					{
						HBM[0].HBM_Output[1].Duty[i] = HBM_OUT_OFF;
					}
				}
			}
		}

	}
	else
	{
		//SetOutputPWM( OP01 + selection, 1000, 0 );
		swtim = 0;
	}


	 if( lkey & (SW_NEW | SW_REP) )
	 {
		lkey &= ~(SW_NEW | SW_REP ); // mask off status bits

		if( lkey == SW_CL )
		{
			main_window = screen151;
		}
		else if( lkey == SW_OL )
		{
		  /*if( selection )
			selection--;*/
			if (swtim < 10)
				lastoutput = 1;
			else
				lastoutput = 0;
		}
		else if( lkey == SW_OR )
		{
			escape = 1;
			SetOutputOUT(OUT_OP06, OUT_ON);
			SetOutputPWM(PWM_OP06, 0, PWM_DUTY100);
		}
		else if( lkey == SW_IL )
		{
			if (swtim < 10)
				nextoutput = 1;
			else
				nextoutput = 0;
		}
		else if( lkey == SW_IR )
		{
			for(i=0; i<4; i++)
			{
				HBM[0].HBM_Input[i].Pulse_Accumulator = 0;
			}
		}
	 }

	if( !lkey && nextoutput)
	  {
	  if( selection < 4 )
		selection++;

	  nextoutput = 0;
	  }

	if( !lkey && lastoutput)
	{
	  if( selection )
	  selection--;

	  lastoutput = 0;
	}
	draw_box( adm, 22, 25, 150, 100, CLEAR );
	display_f1_text( "STATUS:", TSMAX, 8, 27, FIXED ); /* STATUS: */
	if( HBM[0].status & CAN_MODULE_PRESENT )
	{
	display_f1_text( "CONNECTED", TSMAX, 54, 27, FIXED ); /* CONNECTED */
	}
	else
	{
	display_f1_text( "NOT CONNECTED", TSMAX, 54, 27, FIXED ); /* NOT CONNECTED */
	}

	sprintf( tstring, "SOFTWARE: CMS%03i-%03i", HBM[0].software_type, HBM[0].software_issue );
	display_f1_text(tstring, 25, 9, 94, FIXED);

	/* Digital Inputs */
	display_f1_text( " 1     2     3     4", TSMAX, 7, 39, FIXED );
	write_bitmap( adm, HBM[0].HBM_Input[0].Level_Now? SWOPEN : SWCLOSED,  21, 37, OR );
	write_bitmap( adm, HBM[0].HBM_Input[1].Level_Now? SWOPEN : SWCLOSED,  57, 37, OR );
	write_bitmap( adm, HBM[0].HBM_Input[2].Level_Now? SWOPEN : SWCLOSED,  93, 37, OR );
	write_bitmap( adm, HBM[0].HBM_Input[3].Level_Now? SWOPEN : SWCLOSED, 129, 37, OR );


	display_f1_text( "Hz", TS5, 26, 52, RIGHT | FIXED);
	display_number( HBM[0].HBM_Input[0].Frequency, 0.0, 0.0, 1.0, DP1, 25, 52, DW4, RIGHT | FIXED, &RDSfnt1, NULL, PNULL );

	display_number( HBM[0].HBM_Input[0].Pulse_Accumulator, 0.0, 0.0, 1.0, DP0, 25, 67, DW4, RIGHT | FIXED, &RDSfnt1, NULL, PNULL );

	display_f1_text( "Hz", TS5, 62, 52, RIGHT | FIXED);
	display_number( HBM[0].HBM_Input[1].Frequency, 0.0, 0.0, 1.0, DP1, 61, 52, DW4, RIGHT | FIXED, &RDSfnt1, NULL, PNULL );

	display_number( HBM[0].HBM_Input[1].Pulse_Accumulator, 0.0, 0.0, 1.0, DP0, 61, 67, DW4, RIGHT | FIXED, &RDSfnt1, NULL, PNULL );


	display_f1_text( "Hz", TS5, 98, 52, RIGHT | FIXED);
	display_number( HBM[0].HBM_Input[2].Frequency, 0.0, 0.0, 1.0, DP1, 97, 52, DW4, RIGHT | FIXED, &RDSfnt1, NULL, PNULL );

	display_number( HBM[0].HBM_Input[2].Pulse_Accumulator, 0.0, 0.0, 1.0, DP0, 97, 67, DW4, RIGHT | FIXED, &RDSfnt1, NULL, PNULL );


	display_f1_text( "Hz", TS5, 135, 52, RIGHT | FIXED);
	display_number( HBM[0].HBM_Input[3].Frequency, 0.0, 0.0, 1.0, DP1, 134, 52, DW4, RIGHT | FIXED, &RDSfnt1, NULL, PNULL );

	display_number( HBM[0].HBM_Input[3].Pulse_Accumulator, 0.0, 0.0, 1.0, DP0, 134, 67, DW4, RIGHT | FIXED, &RDSfnt1, NULL, PNULL );



	/* Outputs */
	display_f1_text( " 1     2     3     4", TSMAX, 7, 80, FIXED );


	if(HBM[0].HBM_Output[0].Duty[0] == HBM_OUT_ON)
		c = 1;
	else
		c = 0;
	write_bitmap( adm, (selection==0)? op_status[c]: BUTTON03, 21, 80, OR );

	if(HBM[0].HBM_Output[0].Duty[1] == HBM_OUT_ON)
		c = 1;
	else
		c = 0;
	write_bitmap( adm, (selection==1)? op_status[c]: BUTTON03, 57, 80, OR );

	if(HBM[0].HBM_Output[1].Duty[0] == HBM_OUT_ON)
		c = 1;
	else
		c = 0;
	write_bitmap( adm, (selection==2)? op_status[c]: BUTTON03, 93, 80, OR );

	if(HBM[0].HBM_Output[1].Duty[1] == HBM_OUT_ON)
		c = 1;
	else
		c = 0;
	write_bitmap( adm, (selection==3)? op_status[c]: BUTTON03, 129, 80, OR );
#endif
}
