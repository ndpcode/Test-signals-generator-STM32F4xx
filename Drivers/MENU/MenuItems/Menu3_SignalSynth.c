//*********************************************************************************************************//
//generator 407 Menu3 - Signal Synthesizer
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
static uint8_t menu3WNDName[] = "Signal synthesizer";
static MAIN_MENU_ELEMENT _menu3Elements[] = {
																								{(uint8_t*)"Synthesizer #1", Menu4_SignalSynth1_ID, 1, 1},
																								{(uint8_t*)"Synthesizer #2", Menu5_SignalSynth2_ID, 1, 1},
																								{0, 0, 0}																					
																					 };
static MENU_DIALOG_WND menu3WND = {0};

//*********************************************************************************************************//
/* Menu #3 functions */

//---------------------------------------------------------------------------------------------------------//
/*  */																					 
static void Menu3_InitConfig(void)
{
		//wnd border
		menu3WND.wndBorder.startPoint.x = 0;
		menu3WND.wndBorder.startPoint.y = displayCanvas.screenHeight - 1;
		menu3WND.wndBorder.endPoint.x = displayCanvas.screenWidth - 1;
		menu3WND.wndBorder.endPoint.y = 0;
		menu3WND.wndBorderThickness = 1;
		//wnd name and font
		menu3WND.wndName = &menu3WNDName[0];
		menu3WND.wndNameFont.symbolWidth = 5;
		menu3WND.wndNameFont.symbolHeight = 8;
		menu3WND.wndNameFont.centerAlignment = 0;
		menu3WND.wndNameFont.textSpacing = 1;
		menu3WND.wndNameFont.fontRaw = (uint8_t*)&DefaultFont[0];
		//menu elements data and font
		menu3WND.menuElements = &_menu3Elements[0];
		menu3WND.menuElementFont.symbolWidth = 5;
		menu3WND.menuElementFont.symbolHeight = 8;
		menu3WND.menuElementFont.centerAlignment = 0;
		menu3WND.menuElementFont.textSpacing = 1;
		menu3WND.menuElementFont.fontRaw = (uint8_t*)&DefaultFont[0];
		//active menu element config
		menu3WND.activeMenuElementThickness = 1;
	
		//set display canvas struct
		menu3WND.displayCanvas = &displayCanvas;
		//init config
		MenuDialogWNDInitConfig(&menu3WND);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */																					 
static void Menu3_DeleteConfig(void)
{
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t Menu3_SignalSynth_Draw(const uint8_t frameNum)
{
		//check menu window config
		if (!menu3WND.displayCanvas)
		{
				//set canvas
				displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
				displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
				//menu config init
				Menu3_InitConfig();				
		}
		//clear display buffer
		memset(displayBuffer.raw, 0, DISPLAY_BUFFER_SIZE);
		
		//draw menu dialog
		DrawMenuDialogWND(&menu3WND);
	
		//write buf to display
		D128X64WriteBuf(Display128x64, &displayBuffer);
		return 0;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t Menu3_SignalSynth_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents)
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
				MenuGoToItemId(mainMenu, menu3WND.menuElements[menu3WND.activeMenuElementIndex].nextMenuID);
		}
		// event - UP click
		if ( sysEvents & EVENT_BUTTON_UP_CLICK )
		{
				MenuDialogWNDGoToPrev(&menu3WND);
		}
		// event - DOWN click
		if ( sysEvents & EVENT_BUTTON_DOWN_CLICK )
		{
				MenuDialogWNDGoToNext(&menu3WND);
		}
		// event - OK click
		if ( sysEvents & EVENT_BUTTON_OK_CLICK )
		{
				MenuGoToItemId(mainMenu, menu3WND.menuElements[menu3WND.activeMenuElementIndex].nextMenuID);
		}
		// event - MENU click
		if ( sysEvents & EVENT_BUTTON_MENU_CLICK )
		{
				MenuGoToItemId(mainMenu, Menu2_MainMenu_ID);
		}
		// event - VALCODER click
		if ( sysEvents & EVENT_VALCODER_BUTTON_CLICK )
		{
				MenuGoToItemId(mainMenu, menu3WND.menuElements[menu3WND.activeMenuElementIndex].nextMenuID);
		}
		//event - valcoder CW
		if ( valcoderEvents & EVENT_VALCODER_CW )
		{
				valcoderEvents &= ~EVENT_VALCODER_CW;
				MenuDialogWNDGoToNext(&menu3WND);
		}		
		//event - valcoder CCW
		if ( valcoderEvents & EVENT_VALCODER_CCW )
		{
				valcoderEvents &= ~EVENT_VALCODER_CCW;
				MenuDialogWNDGoToPrev(&menu3WND);
		}
		
		//unblock valcoder events upd
		valcoderEventsBlock = 0;
		
		return 0;
}
//---------------------------------------------------------------------------------------------------------//

//**************************************end of Menu3_SignalSynth.c*****************************************//
