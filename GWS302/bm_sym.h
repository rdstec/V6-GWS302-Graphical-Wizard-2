/*
 * bm_sym.h
 *
 *  Created on: 19 May 2016
 *      Author: cparsons
 */

#ifndef BM_SYM_H_
#define BM_SYM_H_

// Bm_sym.c
/*-----------------------------------------------------------*/
// defines

// variables
extern const unsigned char TAB_PAGE[82];
extern const unsigned char TABENT[86];
extern const unsigned char TABESC[86];

extern const unsigned char WARNING_CHOP_KNIV_POS[530];

extern const unsigned char SWCLOSED[16];
extern const unsigned char SWOPEN[16];

extern const unsigned char WARNING_KNIFEPRES[530];

extern const unsigned char Tick[20];

// ---------------McHale Bitmaps-----------------------
extern const unsigned char McHale_V6[769];
extern const unsigned char MODEAUTO[56];
extern const unsigned char MODEMAN[47];

extern const unsigned char BalerHitch[66];

extern const unsigned char Balerdoorclosed_withoutkicker[342];
extern const unsigned char Balerdooropen_withoutkicker[372];

extern const unsigned char BalerCenter[416];
extern const unsigned char ROLL01[53];
extern const unsigned char ROLL02[53];
extern const unsigned char ROLL03[53];
extern const unsigned char ROLL04[53];
extern const unsigned char ROLL05[53];
extern const unsigned char ROLL06[53];
extern const unsigned char ROLL07[53];
extern const unsigned char ROLL08[53];
extern const unsigned char EMPTY_CIRCLE[53];
extern const unsigned char FILLED_CIRCLE[53];

extern unsigned char const * const ROLLER[8];
extern const unsigned char NETROLL_OUTLINE[53];
extern const unsigned char OILCAN[90];
extern const unsigned char GRASS01[30];
extern const unsigned char GRASS02[30];
extern const unsigned char GRASS03[30];
extern const unsigned char GRASS04[30];
extern const unsigned char GRASS05[30];
extern const unsigned char GRASS06[30];

extern unsigned char const * const CROPFEED[12];
extern const unsigned char SIDEDISPLAY_1[342];
extern const unsigned char SIDEDISPLAY_2[342];

extern const unsigned char TABNET[82];
extern const unsigned char TABNAV01[122];
extern const unsigned char TABNAV04[122];
extern const unsigned char TABKNFON[82];
extern const unsigned char TABKNFOFF[82];
extern const unsigned char TABKNFDIV[82];
extern const unsigned char TABDIVERT[82];
extern const unsigned char TABAUTOMAN[82];
extern const unsigned char TAB[82];
extern const unsigned char TABMAN[82];
extern const unsigned char TABNAV00[122];
extern const unsigned char TABNAV_UDLR[122];
extern const unsigned char TABNAV_UDL[122];
extern const unsigned char TABNAV_DLR[122];
extern const unsigned char TABNAV_ULR[122];
extern const unsigned char TABNAV_DL[122];
extern const unsigned char TABNAV_UL[122];
extern const unsigned char TABNAV_LR[122];
extern const unsigned char TABNAV_L[122];
extern const unsigned char TABNAV_R[122];
extern const unsigned char TABNAV_[122];
extern const unsigned char TABNAV_MENU_DL[122];
extern const unsigned char KICK[12];
extern const unsigned char KICKER_UP[26];
extern const unsigned char KICKER_DOWN[26];
extern const unsigned char KNIFE[26];
extern const unsigned char TABRESET[82];
extern const unsigned char BRIGHTN1[24];
extern const unsigned char CONTRST1[20];
extern const unsigned char DAYSYM[106];
extern const unsigned char NIGHTSYM[62];
extern const unsigned char TABPLUS[82];
extern const unsigned char TABMINUS[82];
extern const unsigned char BUTTON01[22];
extern const unsigned char BUTTON03[22];
extern const unsigned char BUTTON04[22];
extern const unsigned char BATTSLIM[18];
extern const unsigned char TABLEFT[82];
extern const unsigned char TABRIGHT[82];
extern const unsigned char TABTICK[82];
extern const unsigned char TABCROSS[82];
extern const unsigned char TAB_DENSITY_RELEASE[82];
extern const unsigned char TAB_NET_CUT[82];
extern const unsigned char WARNINGBOX[102];
extern const unsigned char BAR_UNIT[16];
extern const unsigned char WARNING_DROPFLOOR[530];
extern const unsigned char Bale[254];
extern const unsigned char WARNING_LOWBATT[530];
extern const unsigned char WARNING_CHECKNET[530];
extern const unsigned char ENT[24];
extern const unsigned char POWER[24];
extern const unsigned char BRICK1[56];
extern const unsigned char BAR[145];
extern const unsigned char DENSITY[26];

// --------------Iconized menu Bitmaps (start of)------

extern const unsigned char ICON_BALE_COUNT_MENU[219];
extern const unsigned char ICON_BALE_COUNT_MENU_SELECTED[219];
extern const unsigned char ICON_BALE_SETUP_MENU[219];
extern const unsigned char ICON_BALE_SETUP_MENU_SELECTED[219];
extern const unsigned char ICON_MACHINE_SETUP_MENU[219];
extern const unsigned char ICON_MACHINE_SETUP_MENU_SELECTED[219];
extern const unsigned char ICON_OPERATOR_SETUP_MENU[219];
extern const unsigned char ICON_OPERATOR_SETUP_MENU_SELECTED[219];

// ----------------------------------------------------

extern const unsigned char ICON_BALE_COUNT[41];
extern const unsigned char ICON_BALE_DENSITY[173];
extern const unsigned char ICON_BALE_DENSITY_SELECTED[173];
extern const unsigned char ICON_BALE_DIAMETER_SELECTED[740];
extern const unsigned char ICON_BALE_INDICATOR[138];
extern const unsigned char ICON_BALE_INDICATOR_BEEP[38];
extern const unsigned char ICON_BALE_INDICATOR_OFF[38];
extern const unsigned char ICON_BALE_INDICATOR_ON[38];
extern const unsigned char ICON_BALE_INDICATOR_SELECTED[138];
extern const unsigned char ICON_BALE_PROFILE_A[36];
extern const unsigned char ICON_BALE_PROFILE_B[36];
extern const unsigned char ICON_BALE_PROFILE_C[36];
extern const unsigned char ICON_BALE_PROFILE_CORE_DIAMETER_BALE_DIAMETER[740];
extern const unsigned char ICON_BALE_PROFILE_D[36];
extern const unsigned char ICON_BALE_PROFILE_E[36];
extern const unsigned char ICON_BALE_PROFILE_SELECTED[740];
extern const unsigned char ICON_CORE_DENSITY[154];
extern const unsigned char ICON_CORE_DENSITY_SELECTED[154];
extern const unsigned char ICON_CORE_DIAMETER_SELECTED[740];
extern const unsigned char ICON_CURRENT_SELECTION_INDICATOR[28];
extern const unsigned char ICON_DIAMETER_CORRECTION[288];
extern const unsigned char ICON_DIAMETER_CORRECTION_SELECTED[288];
extern const unsigned char ICON_FILL_POT_ZERO[206];
extern const unsigned char ICON_FILL_POT_ZERO_SELECTED[206];
extern const unsigned char ICON_GRAND_TOTAL_BALES[41];
extern const unsigned char ICON_LUBE_COUNT[41];
extern const unsigned char ICON_NET_DELAY[194];
extern const unsigned char ICON_NET_DELAY_SELECTED[194];
extern const unsigned char ICON_NET_LAYERS[82];
extern const unsigned char ICON_NET_LAYERS_SELECTED[82];
extern const unsigned char ICON_NET_METRES[254];
extern const unsigned char ICON_NET_METRES_SELECTED[254];
extern const unsigned char ICON_PRE_WARNING_BEEP[230];
extern const unsigned char ICON_PRE_WARNING_BEEP_SELECTED[230];
extern const unsigned char ICON_SCROLL_BAR_PAGE_1_OF_2[170];
extern const unsigned char ICON_SCROLL_BAR_PAGE_2_OF_2[170];
extern const unsigned char ICON_SELECTION_POINTER[15];

extern const unsigned char ICON_NET_TWINE_WITH_TWINE_HIGHLIGHTED[277];
extern const unsigned char ICON_NET_TWINE_WITH_NET_HIGHLIGHTED[277];
extern const unsigned char ICON_NET_TWINE_SELECTION_POINTER[52];

// ----------------------------------------------------

extern const unsigned char ICON_BRIGHTNESS_AND_CONTRAST_MENU[100];
extern const unsigned char ICON_BRIGHTNESS_AND_CONTRAST_MENU_SELECTED[100];
extern const unsigned char ICON_DIAGNOSTICS_SCREENS[100];
extern const unsigned char ICON_DIAGNOSTICS_SCREENS_SELECTED[100];
extern const unsigned char ICON_TECHNICIAN_MENU[65];
extern const unsigned char ICON_TECHNICIAN_MENU_SELECTED[65];
extern const unsigned char ICON_TIME_AND_DATE_ADJUST_MENU[107];
extern const unsigned char ICON_TIME_AND_DATE_ADJUST_MENU_SELECTED[107];

// ----------------------------------------------------

extern const unsigned char TABNAV_DR[122];
extern const unsigned char TABNAV_U[122];
extern const unsigned char TABNAV_UDR[122];
extern const unsigned char TABNAV_UR[122];
extern const unsigned char TABNAV_D[122];
extern const unsigned char TABNAV_UD[122];

// ----------------------------------------------------

extern const unsigned char ICON_BALE_COUNT_RESET[233];
extern const unsigned char ICON_FILL_POT_ZERO_RESET[189];
extern const unsigned char ICON_LUBE_COUNT_RESET[233];
extern const unsigned char ICON_NET_METRES_RESET[233];

// ----------------------------------------------------

extern const unsigned char ICON_CLOCK_AT_TEN_TO_TWO[32];
extern const unsigned char UNDERLINE_TWO_DIGIT[6];
extern const unsigned char UNDERLINE_FOUR_DIGIT[8];

// ----------------------------------------------------

extern const unsigned char TAB_MAKE_SELECTION_CURRENT[82];
extern const unsigned char TABRESET_LUBECOUNT[82];

// ----------------------------------------------------

extern const unsigned char LUBE_COUNT_RESET_PROMPT[514];
extern const unsigned char TABCYCLE_ATOJ[82];


// --------------Iconized menu Bitmaps (end of)--------

extern const unsigned char CROSS[59];
extern const unsigned char TICK[59];
extern const unsigned char SPEAKER[59];

extern const unsigned char Clock[32];
extern const unsigned char DIASCALE[83];
extern const unsigned char DIASCALE_FROM_ZERO[83];
extern const unsigned char DIAMIND[7];
extern const unsigned char DRIVING_ARROWS_NONE[101];
extern const unsigned char DRIVING_ARROWS_X1_LEFT[101];
extern const unsigned char DRIVING_ARROWS_X1_RIGHT[101];
extern const unsigned char DRIVING_ARROWS_X2_LEFT[101];
extern const unsigned char DRIVING_ARROWS_X2_RIGHT[101];
extern const unsigned char DRIVING_ARROWS_X3_LEFT[101];
extern const unsigned char DRIVING_ARROWS_X3_RIGHT[101];
extern const unsigned char DRIVING_ARROWS_X4_LEFT[101];
extern const unsigned char DRIVING_ARROWS_X4_RIGHT[101];
extern const unsigned char WARNING_DENSITY_PRESSURE_SCREEN_IMAGE[1161];
extern const unsigned char TABCURS[122];

//Twiner Mode
extern const unsigned char SIDEDISPLAY_TWINE[342];
extern const unsigned char TWINE_MENU_TAB_SP[82];
extern const unsigned char TWINE_MENU_TAB_ARROW_IN[82];
extern const unsigned char TWINE_MENU_TAB_ARROW_OUT[82];
extern const unsigned char CAN_OFFLINE[530];
extern const unsigned char TWINE_PROTECTION_WARNING[530];
extern const unsigned char TwineSettingPage1[794];
extern const unsigned char TwineImage1[794];
extern const unsigned char Line3px[71];
extern const unsigned char ArrowDown[10];
extern const unsigned char ArrowUp[10];
extern const unsigned char ArrowLeft[16];
extern const unsigned char ArrowRight[16];
extern const unsigned char HourglassIcon[30];
extern const unsigned char SmallLine[9];
extern const unsigned char TwineMetresIcon[90];
extern const unsigned char TwineDelayIcon[86];
extern const unsigned char TwineDelayIconSelected[128];
extern const unsigned char TwineMetresIconSelected[134];
extern const unsigned char ICON_TWINER_METRES[266];
extern const unsigned char ICON_TWINER_METRES_SELECTED[266];
extern const unsigned char ICON_TWINER_DELAY[242];
extern const unsigned char ICON_TWINER_DELAY_SELECTED[242];
extern const unsigned char TAB_NET_CUT_TWINE[82];
extern const unsigned char TRACK_POSITION_WARNING[530];
extern const unsigned char X1_NO_TWINE[530];
extern const unsigned char X2_NO_TWINE[530];
// function prototypes
/*-----------------------------------------------------------*/

#endif /* BM_SYM_H_ */
