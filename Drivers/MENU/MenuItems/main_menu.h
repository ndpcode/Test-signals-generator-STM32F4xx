//*********************************************************************************************************//
//generator 407 main menu header file
//Created 18.08.2018
//Created by Novikov Dmitry
//*********************************************************************************************************//

#ifndef __MAIN_MENU
#define __MAIN_MENU

//*********************************************************************************************************//
/* includes */
#include <stdio.h>
#include "bsc_tree_menu.h"
#include "display12864.h"
#include "UnniRamGraphic.h"
#include "UnniRamGraphicUIObjects.h"
#include "UnniRamFonts.h"
#include "generator407.h"
#include "gen_flash.h"
//*********************************************************************************************************//
/* defines, constants and macro */
#define MENU_DRAW_FPS 100
#define MENU_EVENTS_FPS 1000
#define MENU_TRANS_TIME 0
#define DISPLAY_BUFFER_SIZE 1024 //bytes

#define MENU_ELEM_TEXT_OX_INDENT 10
#define MENU_ELEM_TEXT_OY_INDENT 1
#define MENU_ELEM_TEXT_BORDER_INDENT 0 //text offset compensation
#define MENU_ELEMENTS_INDENT 3
#define MENU_SCROLL_BAR_WIDTH 5

//*********************************************************************************************************//
/* enumeration lists */

//*********************************************************************************************************//
/* struct declarations */

/* one string element of main menu *************************************************************************/
typedef struct
{
		uint8_t* text;
		uint16_t nextMenuID;
		uint8_t elemementVisible;
		uint8_t elementEnabled;
} MAIN_MENU_ELEMENT;

/* group of string menu elements **************************************************************************/
typedef struct
{
		URG_CANVAS* displayCanvas;
		URG_RECT wndBorder;
		uint8_t wndBorderThickness;
		uint8_t* wndName;
		URG_FONT_CONFIG wndNameFont;
		MAIN_MENU_ELEMENT* menuElements;
		uint32_t menuElementsCount;
		uint32_t displayedElemCount;
		URG_FONT_CONFIG menuElementFont;	
		uint32_t activeMenuElementIndex;
		uint8_t activeMenuElementThickness;
	
		int32_t menuElementsOYShift;		
} MENU_DIALOG_WND;

//*********************************************************************************************************//
/* main menu functions */

void ResetCommonMenuData(void);
void MenuDialogWNDInitConfig(MENU_DIALOG_WND* menuDialogStruct);
void DrawMenuDialogWND(MENU_DIALOG_WND* menuDialogStruct);
void MenuDialogWNDGoToNext(MENU_DIALOG_WND* menuDialogStruct);
void MenuDialogWNDGoToPrev(MENU_DIALOG_WND* menuDialogStruct);

//Menu1 - start menu
#define Menu1_StartMenu_ID 1U
uint8_t Menu1_StartMenu_Draw(const uint8_t frameNum);
uint8_t Menu1_StartMenu_Events(const uint16_t frameNum, SYS_EVENTS_DATA genEvents);

//Menu2 - main menu
#define Menu2_MainMenu_ID 2U
uint8_t Menu2_MainMenu_Draw(const uint8_t frameNum);
uint8_t Menu2_MainMenu_Events(const uint16_t frameNum, SYS_EVENTS_DATA genEvents);

//Menu3 - signal synthesizer menu
#define Menu3_SignalSynth_ID 3U
uint8_t Menu3_SignalSynth_Draw(const uint8_t frameNum);
uint8_t Menu3_SignalSynth_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents);

//Menu4 - signal synthesizer #1 menu
#define Menu4_SignalSynth1_ID 4U
uint8_t Menu4_SignalSynth1_Draw(const uint8_t frameNum);
uint8_t Menu4_SignalSynth1_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents);

//Menu5 - signal synthesizer #2 menu
#define Menu5_SignalSynth2_ID 5U
uint8_t Menu5_SignalSynth2_Draw(const uint8_t frameNum);
uint8_t Menu5_SignalSynth2_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents);

//Menu6 - pattern generator menu
#define Menu6_PatternGenerator_ID 6U
uint8_t Menu6_PatternGenerator_Draw(const uint8_t frameNum);
uint8_t Menu6_PatternGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents);

//Menu_6_1 - Square simple PWM
#define Menu_6_1_PatternGenerator_ID 8U
uint8_t Menu_6_1_PatternGenerator_Draw(const uint8_t frameNum);
uint8_t Menu_6_1_PatternGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents);
uint8_t Menu_6_1_PatternGenerator_InputInit(void);
GenParametersTypeDef Mode_1_GetAllParametrs(void);
GenParametersTypeDef Mode_1_SetAllParametrs(GenParametersTypeDef ParametrsAll);

//Menu_6_2 - Square PWM
#define Menu_6_2_PatternGenerator_ID 9U
uint8_t Menu_6_2_PatternGenerator_Draw(const uint8_t frameNum);
uint8_t Menu_6_2_PatternGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents);
uint8_t Menu_6_2_PatternGenerator_InputInit(void);
GenParametersTypeDef Mode_2_GetAllParametrs(void);
GenParametersTypeDef Mode_2_SetAllParametrs(GenParametersTypeDef ParametrsAll);

//Menu_6_3 - Phase simple PWM
#define Menu_6_3_PatternGenerator_ID 10U
uint8_t Menu_6_3_PatternGenerator_Draw(const uint8_t frameNum);
uint8_t Menu_6_3_PatternGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents);
uint8_t Menu_6_3_PatternGenerator_InputInit(void);
GenParametersTypeDef Mode_3_GetAllParametrs(void);
GenParametersTypeDef Mode_3_SetAllParametrs(GenParametersTypeDef ParametrsAll);

//Menu_6_4 - Phase PWM
#define Menu_6_4_PatternGenerator_ID 11U
uint8_t Menu_6_4_PatternGenerator_Draw(const uint8_t frameNum);
uint8_t Menu_6_4_PatternGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents);
uint8_t Menu_6_4_PatternGenerator_InputInit(void);
GenParametersTypeDef Mode_4_GetAllParametrs(void);
GenParametersTypeDef Mode_4_SetAllParametrs(GenParametersTypeDef ParametrsAll);

//Menu_6_5 - Sinus PWM
#define Menu_6_5_PatternGenerator_ID 12U
uint8_t Menu_6_5_PatternGenerator_Draw(const uint8_t frameNum);
uint8_t Menu_6_5_PatternGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents);
uint8_t Menu_6_5_PatternGenerator_InputInit(void);
GenParametersTypeDef Mode_5_GetAllParametrs(void);
GenParametersTypeDef Mode_5_SetAllParametrs(GenParametersTypeDef ParametrsAll);

//Menu_6_6 - Triangle PWM
#define Menu_6_6_PatternGenerator_ID 13U
uint8_t Menu_6_6_PatternGenerator_Draw(const uint8_t frameNum);
uint8_t Menu_6_6_PatternGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents);
uint8_t Menu_6_6_PatternGenerator_InputInit(void);
GenParametersTypeDef Mode_6_GetAllParametrs(void);
GenParametersTypeDef Mode_6_SetAllParametrs(GenParametersTypeDef ParametrsAll);

//Menu_6_7 - Multi Pulse PWM
#define Menu_6_7_PatternGenerator_ID 14U
uint8_t Menu_6_7_PatternGenerator_Draw(const uint8_t frameNum);
uint8_t Menu_6_7_PatternGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents);
uint8_t Menu_6_7_PatternGenerator_InputInit(void);
GenParametersTypeDef Mode_7_GetAllParametrs(void);
GenParametersTypeDef Mode_7_SetAllParametrs(GenParametersTypeDef ParametrsAll);

//Menu_6_8 - Three Phase PWM
#define Menu_6_8_PatternGenerator_ID 15U
uint8_t Menu_6_8_PatternGenerator_Draw(const uint8_t frameNum);
uint8_t Menu_6_8_PatternGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents);
uint8_t Menu_6_8_PatternGenerator_InputInit(void);
GenParametersTypeDef Mode_8_GetAllParametrs(void);
GenParametersTypeDef Mode_8_SetAllParametrs(GenParametersTypeDef ParametrsAll);

//Menu_6_9 - Three Phase PWM
#define Menu_6_9_PatternGenerator_ID 16U
uint8_t Menu_6_9_PatternGenerator_Draw(const uint8_t frameNum);
uint8_t Menu_6_9_PatternGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents);
uint8_t Menu_6_9_PatternGenerator_InputInit(void);
GenParametersTypeDef Mode_9_GetAllParametrs(void);
GenParametersTypeDef Mode_9_SetAllParametrs(GenParametersTypeDef ParametrsAll);

//Menu_8 - Frequency Meter
#define Menu_8_FrequencyMeter_ID 17U
uint8_t Menu_8_FrequencyMeter_Draw(const uint8_t frameNum);
uint8_t Menu_8_FrequencyMeter_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents);


//Menu17 - signal generator menu
#define Menu17_SignalGenerator_ID 7U
uint8_t Menu17_SignalGenerator_Draw(const uint8_t frameNum);
uint8_t Menu17_SignalGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents);

#endif
//********************************************end of main_menu.h*********************************************//
