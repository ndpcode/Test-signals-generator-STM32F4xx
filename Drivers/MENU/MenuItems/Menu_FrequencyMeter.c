//*********************************************************************************************************//
//generator 407 menu_8 - signal generator
//Created 23.10.2018
//Created by Novikov Dmitry
//*********************************************************************************************************//

//*********************************************************************************************************//
/* includes */
#include "main.h"
#include "main_menu.h"
#include "counter.h"

//*********************************************************************************************************//
/* defines and constants */

//*********************************************************************************************************//
/* Global variables ---------------------------------------------------------------------------------------*/
extern TREE_MENU* 						mainMenu;
extern D128X64_DISPLAY* 			Display128x64;
extern D128X64_BUFFER 				displayBuffer;
extern URG_CANVAS 						displayCanvas;
extern SYS_EVENTS_DATA 				valcoderEvents; 				//external valcoder events handler struct
extern uint8_t 								valcoderEventsBlock;    //flag - block/unblock change valcoderEvents variable


/* Private functions --------------------------------------------------------------------------------------*/
void ChoiseTimeFreqMeter1000(uint32_t itemIndex);
void ChoiseTimeFreqMeter100(uint32_t itemIndex);
void ChoiseTimeFreqMeter10(uint32_t itemIndex);
void StartFreqMeter(uint32_t itemIndex);
void StopFreqMeter (uint32_t itemIndex);
	
/* Private variables --------------------------------------------------------------------------------------*/
static const uint8_t menu_8WNDName[] = "Frequency Meter";
static MAIN_MENU_ELEMENT _menu_8Elements[] = {
																								{0, 0, 0, 0}																					
																					   };
static MENU_DIALOG_WND menu_8WND = {0};
static URG_FONT_CONFIG numEditFont;
static URG_FONT_CONFIG numEditStrFont;
static UNNI_UI_SCROLLLIST* menu_8SignalScrollList = 0;
static UNNI_UI_SWITCH_BOX_ITEM signalGenTypeBoxItems[] = {
                                                           {(uint8_t*)"1S", 		ChoiseTimeFreqMeter1000},
                                                           {(uint8_t*)"0.1S",	  ChoiseTimeFreqMeter100},
																													 {(uint8_t*)"0.01S", 	ChoiseTimeFreqMeter10}
                                                      };
static UNNI_UI_SWITCHBOX signalGenTypeSwitchBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"1 Sample in : ", 3, 1, 0, signalGenTypeBoxItems ,1};
static uint8_t FrequencyMeterExt1Str[] = "Hz 1";
static UNNI_UI_NUMBEREDIT* FrequencyMeterExt1 = 0;
static uint8_t FrequencyMeterExt2Str[] = "Hz 2";
static UNNI_UI_NUMBEREDIT* FrequencyMeterExt2 = 0;
static UNNI_UI_SWITCH_BOX_ITEM generatorPowerOnBoxItems[] = {
                                                           {(uint8_t*)"Off", StopFreqMeter},
                                                           {(uint8_t*)"On",  StartFreqMeter}
                                                         };
static UNNI_UI_SWITCHBOX GeneratorPowerOnBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Counter : ", 2, 1, 0, generatorPowerOnBoxItems ,1};

//*********************************************************************************************************//
/* Menu #_8 functions */

void ChoiseTimeFreqMeter1000(uint32_t itemIndex)
{
	StopFreqMeter(0);
//TIM2->ARR       = 10000 - 1;
	FrequencyMetr_TimeSample = 10000;
	StartFreqMeter(0);
}
void ChoiseTimeFreqMeter100(uint32_t itemIndex)
{
	StopFreqMeter(0);
//TIM2->ARR       = 1000 - 1;
	FrequencyMetr_TimeSample = 1000;
	StartFreqMeter(0);
}
void ChoiseTimeFreqMeter10(uint32_t itemIndex)
{
	StopFreqMeter(0);
//TIM2->ARR       = 100 - 1;
	FrequencyMetr_TimeSample = 100;
	StartFreqMeter(0);
}
void StartFreqMeter(uint32_t itemIndex)
{
	FrequencyMeter_InitTimer();
  FrequencyMeter_InitGPIO();
	FrequencyMeter_InitStart();
}
void StopFreqMeter(uint32_t itemIndex)
{
  FrequencyMeter_InitStop();
}
/* Private functions --------------------------------------------------------------------------------------*/

//---------------------------------------------------------------------------------------------------------//
/*  */																					 
static void menu_8_InitConfig(void)
{
		//wnd
		//wnd border
		menu_8WND.wndBorder.startPoint.x = 0;
		menu_8WND.wndBorder.startPoint.y = displayCanvas.screenHeight - 1;
		menu_8WND.wndBorder.endPoint.x = displayCanvas.screenWidth - 1;
		menu_8WND.wndBorder.endPoint.y = 0;
		menu_8WND.wndBorderThickness = 1;
		//set elements
		menu_8WND.menuElements = &_menu_8Elements[0];
		//wnd name and font
		menu_8WND.wndName = (uint8_t*)&menu_8WNDName[0];
		menu_8WND.wndNameFont.symbolWidth = 5;
		menu_8WND.wndNameFont.symbolHeight = 8;
		menu_8WND.wndNameFont.centerAlignment = 0;
		menu_8WND.wndNameFont.textSpacing = 1;
		menu_8WND.wndNameFont.fontRaw = (uint8_t*)&DefaultFont[0];	
		//set display canvas struct
		menu_8WND.displayCanvas = &displayCanvas;
		//init config
		MenuDialogWNDInitConfig(&menu_8WND);
		
		//config number edit font
		numEditFont.symbolWidth = 7;
		numEditFont.symbolHeight = 13;
		numEditFont.centerAlignment = 1;
		numEditFont.textSpacing = 2;
		numEditFont.fontRaw = (uint8_t*)&a_LCDNova7x13[0];
		
		numEditStrFont.symbolWidth = 5;
		numEditStrFont.symbolHeight = 8;
		numEditStrFont.centerAlignment = 0;
		numEditStrFont.textSpacing = 1;
		numEditStrFont.fontRaw = (uint8_t*)&DefaultFont[0];
		

		//create scroll list
		if (!menu_8SignalScrollList)
		{
				menu_8SignalScrollList = UnniUiScrollListCreate(menu_8WND.wndBorder.startPoint.x + 1, menu_8WND.wndBorder.startPoint.y - 1,
																											  menu_8WND.wndBorder.endPoint.x - 1, menu_8WND.wndBorder.startPoint.y - 11, 10, 5, 2, 5,
																											  SCROLLLIST_LEFT_ALIGNED, &displayCanvas);
		}
		//add elements to scroll list
		//add switch box - signal type
		
		UnniUiScrollListAddItem(menu_8SignalScrollList, 100, 12, (void*)&signalGenTypeSwitchBox, UnniUiSwitchBoxUpdate);
		
		if (!FrequencyMeterExt1)
		{
				FrequencyMeterExt1 = UnniUiNumberEditCreate(0, 0, 8, 0, 0, 0, 168000000, 0,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)FrequencyMeterExt1Str, &displayCanvas);
				UnniUiScrollListAddItem(menu_8SignalScrollList, menu_8WND.wndBorder.endPoint.x - 30, 13, (void*)FrequencyMeterExt1, UnniUiNumberEditUpdate);

		}
		if (!FrequencyMeterExt2)
		{
				FrequencyMeterExt2 = UnniUiNumberEditCreate(0, 0, 8, 0, 0, 0, 168000000, 0,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)FrequencyMeterExt2Str, &displayCanvas);
				UnniUiScrollListAddItem(menu_8SignalScrollList, menu_8WND.wndBorder.endPoint.x - 30, 13, (void*)FrequencyMeterExt2, UnniUiNumberEditUpdate);

		}
		UnniUiScrollListAddItem(menu_8SignalScrollList, 100, 12, (void*)&GeneratorPowerOnBox, UnniUiSwitchBoxUpdate);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */																					 
static void menu_8_DeleteConfig(void)
{
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t Menu_8_FrequencyMeter_Draw(const uint8_t frameNum)
{
		//check menu window config
		if (!menu_8WND.displayCanvas)
		{
				//set canvas
				displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
				displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
				//menu config init
				menu_8_InitConfig();				
		}
		//clear display buffer
		memset(displayBuffer.raw, 0, DISPLAY_BUFFER_SIZE);
		
		//draw menu dialog
		DrawMenuDialogWND(&menu_8WND);
		//draw scroll list with components
		UnniUiScrollListDraw(menu_8SignalScrollList);
	
		//write buf to display
		D128X64WriteBuf(Display128x64, &displayBuffer);
		return 0;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t Menu_8_FrequencyMeter_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents)
{
		//block valcoder events upd
		valcoderEventsBlock = 1;
	
		if (DataReadyImpulse == SET)
		{
				DataReadyImpulse = RESET;
				UnniUiNumberEditSetFullValue(FrequencyMeterExt1,FrequencyMetr_1);
				UnniUiNumberEditSetFullValue(FrequencyMeterExt2,FrequencyMetr_2);
		}
		// event - MENU click
		if ( sysEvents & EVENT_BUTTON_MENU_CLICK )
		{
				MenuGoToItemId(mainMenu, Menu2_MainMenu_ID);
		}
		// event - UP click
		if ( sysEvents & EVENT_BUTTON_UP_CLICK )
		{
				UnniUiScrollListUp(menu_8SignalScrollList);
		}
		// event - DOWN click
		if ( sysEvents & EVENT_BUTTON_DOWN_CLICK )
		{
				UnniUiScrollListDown(menu_8SignalScrollList);
		}
		// event - button Left click
		if ( sysEvents & EVENT_BUTTON_LEFT_CLICK )
		{
				UnniUiSwitchBoxPrev(&signalGenTypeSwitchBox);
				UnniUiSwitchBoxPrev(&GeneratorPowerOnBox);
		}
		// event - button Right click
		if ( sysEvents & EVENT_BUTTON_RIGHT_CLICK )
		{
				UnniUiSwitchBoxNext(&signalGenTypeSwitchBox);
				UnniUiSwitchBoxNext(&GeneratorPowerOnBox);
		}
		// event - encoder button pressed and CCW
		if ( (sysEvents & EVENT_VALCODER_BUTTON_PRESSED) && (valcoderEvents & EVENT_VALCODER_CCW) )
		{
				valcoderEvents &= ~EVENT_VALCODER_CCW;

		}		
		// event - encoder button pressed and CW
		if ( (sysEvents & EVENT_VALCODER_BUTTON_PRESSED) && (valcoderEvents & EVENT_VALCODER_CW) )
		{
				valcoderEvents &= ~EVENT_VALCODER_CW;

		}
		// event - OK click or encoder button click
		if ( (sysEvents & EVENT_BUTTON_OK_CLICK) || (sysEvents & EVENT_VALCODER_BUTTON_CLICK) )
		{
		}		
		// event - encoder CCW
		if (valcoderEvents & EVENT_VALCODER_CCW)
		{
				//clear flag manually
				valcoderEvents &= ~EVENT_VALCODER_CCW;
	

				//switch box -> go to previous element
				UnniUiSwitchBoxPrev(&signalGenTypeSwitchBox);
				UnniUiSwitchBoxPrev(&GeneratorPowerOnBox);
		}		
		// event - encoder CW
		if (valcoderEvents & EVENT_VALCODER_CW)
		{
				//clear flag manually
				valcoderEvents &= ~EVENT_VALCODER_CW;

			
				//switch box -> go to next element
				UnniUiSwitchBoxNext(&signalGenTypeSwitchBox);
				UnniUiSwitchBoxNext(&GeneratorPowerOnBox);
		}
	
		//unblock valcoder events upd
		valcoderEventsBlock = 0;
		
		return 0;
}
//---------------------------------------------------------------------------------------------------------//

//**************************************end of menu_8_SignalGenerator.c************************************//
