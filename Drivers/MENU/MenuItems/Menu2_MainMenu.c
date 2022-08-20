//*********************************************************************************************************//
//generator 407 Menu2 - MainMenu
//Created 18.08.2018
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
static uint8_t menu2WNDName[] = "Generator start menu";
static MAIN_MENU_ELEMENT _menuElements[] = {
																								{(uint8_t*)"Signal synthesizer",	Menu3_SignalSynth_ID, 1, 1},
																								{(uint8_t*)"Pattern generator",		Menu6_PatternGenerator_ID, 1, 1},
																								{(uint8_t*)"Frequency Meter", 		Menu_8_FrequencyMeter_ID, 1, 1},
																								{(uint8_t*)"Signal generator", 		Menu17_SignalGenerator_ID, 1, 1},
																								{(uint8_t*)"Settings", 0, 1, 1},
																								{(uint8_t*)"About", 0, 1, 1},
																								{0, 0, 0}																					
																					 };
MENU_DIALOG_WND menu2WND = {0};
static uint32_t _menuElementsCount = 0;
static uint32_t _activeMenuElement = 0;
static URG_RECT _activeMenuElementRect;
static URG_POINT _textPoint;
static URG_RECT _rect;
static int32_t _menuElementsOYShift = 0;
static uint32_t _index;
static char numberStr[20] = "";

//*********************************************************************************************************//
/* Menu1 functions */
																					 
static void Menu2_InitConfig(void)
{
		//wnd border
		menu2WND.wndBorder.startPoint.x = 0;
		menu2WND.wndBorder.startPoint.y = displayCanvas.screenHeight - 1;
		menu2WND.wndBorder.endPoint.x = displayCanvas.screenWidth - 1;
		menu2WND.wndBorder.endPoint.y = 0;
		menu2WND.wndBorderThickness = 1;
		//wnd name and font
		menu2WND.wndName = &menu2WNDName[0];
		menu2WND.wndNameFont.symbolWidth = 5;
		menu2WND.wndNameFont.symbolHeight = 8;
		menu2WND.wndNameFont.centerAlignment = 0;
		menu2WND.wndNameFont.textSpacing = 1;
		menu2WND.wndNameFont.fontRaw = (uint8_t*)&DefaultFont[0];
		//menu elements data and font
		menu2WND.menuElements = &_menuElements[0];
		menu2WND.menuElementFont.symbolWidth = 5;
		menu2WND.menuElementFont.symbolHeight = 8;
		menu2WND.menuElementFont.centerAlignment = 0;
		menu2WND.menuElementFont.textSpacing = 1;
		menu2WND.menuElementFont.fontRaw = (uint8_t*)&DefaultFont[0];
		//active menu element config
		menu2WND.activeMenuElementThickness = 1;
	
		//set display canvas struct
		menu2WND.displayCanvas = &displayCanvas;
		//init config
		MenuDialogWNDInitConfig(&menu2WND);
}

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t Menu2_MainMenu_Draw(const uint8_t frameNum)
{
		//check menu window config
		if (!menu2WND.displayCanvas)
		{
				//set canvas
				displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
				displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
				//menu config init
				Menu2_InitConfig();				
		}
		//clear display buffer
		memset(displayBuffer.raw, 0, DISPLAY_BUFFER_SIZE);
		
		//draw menu dialog
		DrawMenuDialogWND(&menu2WND);
	
		//write buf to display
		D128X64WriteBuf(Display128x64, &displayBuffer);
		return 0;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t Menu2_MainMenu_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents)
{
		//block valcoder events upd
		valcoderEventsBlock = 1;
	
		// event - button Left click
		if ( sysEvents & EVENT_BUTTON_LEFT_CLICK )
		{
				MenuGoToItemId(mainMenu, Menu1_StartMenu_ID);
		}
		// event - button Right click
		if ( sysEvents & EVENT_BUTTON_RIGHT_CLICK )
		{
				MenuGoToItemId(mainMenu, menu2WND.menuElements[menu2WND.activeMenuElementIndex].nextMenuID);
		}
		// event - UP click
		if ( sysEvents & EVENT_BUTTON_UP_CLICK )
		{
				MenuDialogWNDGoToPrev(&menu2WND);
		}
		// event - DOWN click
		if ( sysEvents & EVENT_BUTTON_DOWN_CLICK )
		{
				MenuDialogWNDGoToNext(&menu2WND);
		}
		// event - OK click
		if ( sysEvents & EVENT_BUTTON_OK_CLICK )
		{
				MenuGoToItemId(mainMenu, menu2WND.menuElements[menu2WND.activeMenuElementIndex].nextMenuID);
		}
		// event - VALCODER click
		if ( sysEvents & EVENT_VALCODER_BUTTON_CLICK )
		{
				MenuGoToItemId(mainMenu, menu2WND.menuElements[menu2WND.activeMenuElementIndex].nextMenuID);
		}
		//event - valcoder CW
		if ( valcoderEvents & EVENT_VALCODER_CW )
		{
				valcoderEvents &= ~EVENT_VALCODER_CW;
				MenuDialogWNDGoToNext(&menu2WND);
		}		
		//event - valcoder CCW
		if ( valcoderEvents & EVENT_VALCODER_CCW )
		{
				valcoderEvents &= ~EVENT_VALCODER_CCW;
				MenuDialogWNDGoToPrev(&menu2WND);
		}
		
		//unblock valcoder events upd
		valcoderEventsBlock = 0;
		
		return 0;
}
//---------------------------------------------------------------------------------------------------------//

//*****************************************end of Menu2_MainMenu.c*****************************************//
