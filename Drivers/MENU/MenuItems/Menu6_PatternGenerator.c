//*********************************************************************************************************//
//generator 407 Menu6 - PatternGenerator
//Created 15.10.2018
//Created by Novikov Dmitry
//*********************************************************************************************************//

//*********************************************************************************************************//
/* includes */
#include "main.h"
#include "main_menu.h"

//*********************************************************************************************************//
/* defines and constants */

//*********************************************************************************************************//
/* Global variables ---------------------------------------------------------------------------------------*/
extern TREE_MENU* mainMenu;
extern D128X64_DISPLAY* Display128x64;
extern D128X64_BUFFER displayBuffer;
extern URG_CANVAS displayCanvas;
extern URG_FONT_CONFIG fontConfig;
extern SYS_EVENTS_DATA valcoderEvents; //external valcoder events handler struct
extern uint8_t valcoderEventsBlock;    //flag - block/unblock change valcoderEvents variable

/* Private variables --------------------------------------------------------------------------------------*/
static uint8_t menu6WNDName[] = "Pattern generator";
static MAIN_MENU_ELEMENT _menuElements[] = {
//																								{(uint8_t*)"Signal 1", 									Menu7_Signal1_ID, 1, 1},
																								{(uint8_t*)"[1] Square Simple", 					Menu_6_1_PatternGenerator_ID, 1, 1},
																								{(uint8_t*)"[2] Square", 									Menu_6_2_PatternGenerator_ID, 1, 1},
																								{(uint8_t*)"[3] Phase simple", 						Menu_6_3_PatternGenerator_ID, 1, 1},
																								{(uint8_t*)"[4] Phase", 									Menu_6_4_PatternGenerator_ID, 1, 1},
																								{(uint8_t*)"[5] Sinus", 									Menu_6_5_PatternGenerator_ID, 1, 1},
																								{(uint8_t*)"[6] Triangle", 								Menu_6_6_PatternGenerator_ID, 1, 1},
																								{(uint8_t*)"[7] Multi Pulse", 						Menu_6_7_PatternGenerator_ID, 1, 1},
																								{(uint8_t*)"[8] Three Phase", 						Menu_6_8_PatternGenerator_ID, 1, 1},
																								{(uint8_t*)"[ ] Square Timer", 				  	Menu_6_9_PatternGenerator_ID, 1, 1},
																								{0, 0, 0}																					
																					 };
MENU_DIALOG_WND menu6WND = {0};

//*********************************************************************************************************//
/* Menu6 functions */
																					 
static void Menu6_InitConfig(void)
{
		//wnd border
		menu6WND.wndBorder.startPoint.x = 0;
		menu6WND.wndBorder.startPoint.y = displayCanvas.screenHeight - 1;
		menu6WND.wndBorder.endPoint.x = displayCanvas.screenWidth - 1;
		menu6WND.wndBorder.endPoint.y = 0;
		menu6WND.wndBorderThickness = 1;
		//wnd name and font
		menu6WND.wndName = &menu6WNDName[0];
		menu6WND.wndNameFont.symbolWidth = 5;
		menu6WND.wndNameFont.symbolHeight = 8;
		menu6WND.wndNameFont.centerAlignment = 0;
		menu6WND.wndNameFont.textSpacing = 1;
		menu6WND.wndNameFont.fontRaw = (uint8_t*)&DefaultFont[0];
		//menu elements data and font
		menu6WND.menuElements = &_menuElements[0];
		menu6WND.menuElementFont.symbolWidth = 5;
		menu6WND.menuElementFont.symbolHeight = 8;
		menu6WND.menuElementFont.centerAlignment = 0;
		menu6WND.menuElementFont.textSpacing = 1;
		menu6WND.menuElementFont.fontRaw = (uint8_t*)&DefaultFont[0];
		//active menu element config
		menu6WND.activeMenuElementThickness = 1;
	
		//set display canvas struct
		menu6WND.displayCanvas = &displayCanvas;
		//init config
		MenuDialogWNDInitConfig(&menu6WND);
}

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t Menu6_PatternGenerator_Draw(const uint8_t frameNum)
{
		//check menu window config
		if (!menu6WND.displayCanvas)
		{
				//set canvas
				displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
				displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
				//menu config init
				Menu6_InitConfig();				
		}
		//clear display buffer
		memset(displayBuffer.raw, 0, DISPLAY_BUFFER_SIZE);
		
		//draw menu dialog
		DrawMenuDialogWND(&menu6WND);
	
		//write buf to display
		D128X64WriteBuf(Display128x64, &displayBuffer);
		return 0;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t Menu6_PatternGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents)
{
		//block valcoder events upd
		valcoderEventsBlock = 1;
	
		// event - button Left click
		if ( sysEvents & EVENT_BUTTON_LEFT_CLICK )
		{
				MenuGoToItemId(mainMenu, Menu2_MainMenu_ID);
		}
		// event - button Right click
		if ( sysEvents & EVENT_BUTTON_RIGHT_CLICK )
		{
				MenuGoToItemId(mainMenu, menu6WND.menuElements[menu6WND.activeMenuElementIndex].nextMenuID);
		}
		// event - UP click
		if ( sysEvents & EVENT_BUTTON_UP_CLICK )
		{
				MenuDialogWNDGoToPrev(&menu6WND);
		}
		// event - DOWN click
		if ( sysEvents & EVENT_BUTTON_DOWN_CLICK )
		{
				MenuDialogWNDGoToNext(&menu6WND);
		}
		// event - OK click
		if ( sysEvents & EVENT_BUTTON_OK_CLICK )
		{
				MenuGoToItemId(mainMenu, menu6WND.menuElements[menu6WND.activeMenuElementIndex].nextMenuID);
		}
		// event - MENU click
		if ( sysEvents & EVENT_BUTTON_MENU_CLICK )
		{
				MenuGoToItemId(mainMenu, Menu2_MainMenu_ID);
		}
		// event - VALCODER click
		if ( sysEvents & EVENT_VALCODER_BUTTON_CLICK )
		{
				MenuGoToItemId(mainMenu, menu6WND.menuElements[menu6WND.activeMenuElementIndex].nextMenuID);
		}
		//event - valcoder CW
		if ( valcoderEvents & EVENT_VALCODER_CW )
		{
				valcoderEvents &= ~EVENT_VALCODER_CW;
				MenuDialogWNDGoToNext(&menu6WND);
		}		
		//event - valcoder CCW
		if ( valcoderEvents & EVENT_VALCODER_CCW )
		{
				valcoderEvents &= ~EVENT_VALCODER_CCW;
				MenuDialogWNDGoToPrev(&menu6WND);
		}
		
		//unblock valcoder events upd
		valcoderEventsBlock = 0;
		
		return 0;
}
//---------------------------------------------------------------------------------------------------------//

//*************************************end of Menu6_PatternGenerator.c*************************************//
