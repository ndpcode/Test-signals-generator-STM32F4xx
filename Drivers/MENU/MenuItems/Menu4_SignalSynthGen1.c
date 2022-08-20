//*********************************************************************************************************//
//generator 407 Menu4 - Signal Synthesizer #1
//Created 18.08.2018
//Created by Novikov Dmitry
//*********************************************************************************************************//

//*********************************************************************************************************//
/* includes */
#include "main.h"
#include "main_menu.h"
#include "ad9850.h"

//*********************************************************************************************************//
/* defines and constants */

#define MENU4_FOCUSED_ELEM_COUNT 3

#define MENU4_FREQ_EDIT_POS_X 14
#define MENU4_FREQ_EDIT_POS_Y 25
#define MENU4_FREQ_EDIT_TEXT_POS_X 106
#define MENU4_FREQ_EDIT_TEXT_POS_Y 25
#define MENU4_PHASE_EDIT_POS_X 6
#define MENU4_PHASE_EDIT_POS_Y 40
#define MENU4_PHASE_EDIT_TEXT_POS_X 64
#define MENU4_PHASE_EDIT_TEXT_POS_Y 40
#define MENU4_DUTY_EDIT_POS_X 6
#define MENU4_DUTY_EDIT_POS_Y 55
#define MENU4_DUTY_EDIT_TEXT_POS_X 64
#define MENU4_DUTY_EDIT_TEXT_POS_Y 55

//*********************************************************************************************************//
/* Global variables ---------------------------------------------------------------------------------------*/
extern TREE_MENU* mainMenu;
extern D128X64_DISPLAY* Display128x64;
extern D128X64_BUFFER displayBuffer;
extern URG_CANVAS displayCanvas;
extern SYS_EVENTS_DATA valcoderEvents; //external valcoder events handler struct
extern uint8_t valcoderEventsBlock;    //flag - block/unblock change valcoderEvents variable
extern AD9850* signalSynth1;           //ad9850 #1

/* Private functions --------------------------------------------------------------------------------------*/
void synth1PowerOff(void);
void synth1PowerOn(void);
void synth1Reset(void);

/* Private variables --------------------------------------------------------------------------------------*/
static uint8_t menu4Synth1WNDName[] = "Synthesizer #1";
static MAIN_MENU_ELEMENT _menu4Elements[] = {
																								{0, 0, 0, 0}																					
																					  };
static MENU_DIALOG_WND menu4Synth1WND = {0};
static uint8_t menu4Synth1Cursor;
static URG_FONT_CONFIG numEditFont;
static UNNI_UI_NUMBEREDIT* synth1FrequencyEdit = 0;
static UNNI_UI_NUMBEREDIT* synth1PhaseEdit = 0;
static UNNI_UI_NUMBEREDIT* synth1DutyEdit = 0;
static uint8_t synth1FrequencyEditStr[] = "Hz";
static uint8_t synth1PhaseEditStr[] = "deg";
static uint8_t synth1DutyEditStr[] = "%";
static URG_FONT_CONFIG numEditStrFont;
static UNNI_UI_SCROLLLIST* menu4Synth1ScrollList;
static char numberStr[20] = "";
static UNNI_UI_SWITCH_BOX_ITEM synth1PowerOnBoxItems[] = {
                                                           {(uint8_t*)"Off", synth1PowerOff},
                                                           {(uint8_t*)"On", synth1PowerOn}
                                                         };
static UNNI_UI_SWITCHBOX synth1PowerOnBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Power : ", 2, 1, 0, synth1PowerOnBoxItems,1};
static UNNI_UI_SWITCH_BOX_ITEM synth1ResetItems[] = {
                                                      {(uint8_t*)"", 0},
																											{(uint8_t*)"OK", synth1Reset}
																										};
static UNNI_UI_SWITCHBOX synth1ResetBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Reset : ", 2, 1, 0, synth1ResetItems,1};

//*********************************************************************************************************//
/* Menu #4 functions */

//---------------------------------------------------------------------------------------------------------//
/*  */
void synth1PowerOff(void)
{
		AD9850SignalOnOff(signalSynth1, 0);
		AD9850Update(signalSynth1);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
void synth1PowerOn(void)
{
		AD9850SignalOnOff(signalSynth1, 1);
		AD9850Update(signalSynth1);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
void synth1Reset(void)
{
		AD9850Reset(signalSynth1);
		AD9850Update(signalSynth1);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */																					 
static void Menu4_InitConfig(void)
{
		double doubleVal;
		float floatVal;
		
		//wnd
		//wnd border
		menu4Synth1WND.wndBorder.startPoint.x = 0;
		menu4Synth1WND.wndBorder.startPoint.y = displayCanvas.screenHeight - 1;
		menu4Synth1WND.wndBorder.endPoint.x = displayCanvas.screenWidth - 1;
		menu4Synth1WND.wndBorder.endPoint.y = 0;
		menu4Synth1WND.wndBorderThickness = 1;
		//set elements
		menu4Synth1WND.menuElements = &_menu4Elements[0];
		//wnd name and font
		menu4Synth1WND.wndName = &menu4Synth1WNDName[0];
		menu4Synth1WND.wndNameFont.symbolWidth = 5;
		menu4Synth1WND.wndNameFont.symbolHeight = 8;
		menu4Synth1WND.wndNameFont.centerAlignment = 0;
		menu4Synth1WND.wndNameFont.textSpacing = 1;
		menu4Synth1WND.wndNameFont.fontRaw = (uint8_t*)&DefaultFont[0];	
		//set display canvas struct
		menu4Synth1WND.displayCanvas = &displayCanvas;
		//init config
		MenuDialogWNDInitConfig(&menu4Synth1WND);

		//set cursor
		menu4Synth1Cursor = 1;
		
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
		
		//config synthesizer 1 frequency edit
		if (!synth1FrequencyEdit)
		{
				AD9850GetFrequency(signalSynth1, &doubleVal);
				synth1FrequencyEdit = UnniUiNumberEditCreate(MENU4_FREQ_EDIT_POS_X, MENU4_FREQ_EDIT_POS_Y, 9, 1,
				                                             NUMBEREDIT_FRACTIONAL, AD9850_MIN_FREQUENCY, AD9850_MAX_FREQUENCY, doubleVal,
				                                             &numEditFont, &numEditStrFont, (uint8_t*)synth1FrequencyEditStr, &displayCanvas);
				UnniUiNumberEditSetFocus(synth1FrequencyEdit, 1);
		}
		//config synthesizer 1 phase edit
		if (!synth1PhaseEdit)
		{
				AD9850GetPhase(signalSynth1, &floatVal);
				synth1PhaseEdit = UnniUiNumberEditCreate(MENU4_PHASE_EDIT_POS_X, MENU4_PHASE_EDIT_POS_Y, 5, 2,
				                                         NUMBEREDIT_FRACTIONAL, AD9850_MIN_PHASE, AD9850_MAX_PHASE, floatVal,
				                                         &numEditFont, &numEditStrFont, (uint8_t*)synth1PhaseEditStr, &displayCanvas);
				UnniUiNumberEditSetFocus(synth1PhaseEdit, 0);
		}
		//config synthesizer 1 phase edit
		if (!synth1DutyEdit)
		{
				AD9850GetDuty(signalSynth1, &floatVal);
				synth1DutyEdit = UnniUiNumberEditCreate(MENU4_DUTY_EDIT_POS_X, MENU4_DUTY_EDIT_POS_Y, 5, 2,
				                                        NUMBEREDIT_FRACTIONAL, AD9850_MIN_DUTY, AD9850_MAX_DUTY, floatVal,
				                                        &numEditFont, &numEditStrFont, (uint8_t*)synth1DutyEditStr, &displayCanvas);
				UnniUiNumberEditSetFocus(synth1DutyEdit, 0);
		}
		//config synth1 and power SwitchBox
		AD9850SetAutoupdate(signalSynth1, 1);
		AD9850SignalOnOff(signalSynth1, 0);
		AD9850Update(signalSynth1);
		synth1PowerOnBox.currentElementNumber = 1;
		//config synth1 reset SwitchBox
		synth1ResetBox.currentElementNumber = 1;
		
		//create scroll list
		menu4Synth1ScrollList = UnniUiScrollListCreate(menu4Synth1WND.wndBorder.startPoint.x + 1, menu4Synth1WND.wndBorder.startPoint.y - 1,
		                                               menu4Synth1WND.wndBorder.endPoint.x - 1, menu4Synth1WND.wndBorder.startPoint.y - 11, 10, 5, 2, 5,
		                                               SCROLLLIST_LEFT_ALIGNED, &displayCanvas);
		//add elements to scroll list
		UnniUiScrollListAddItem(menu4Synth1ScrollList, menu4Synth1WND.wndBorder.endPoint.x - 30, 13, (void*)synth1FrequencyEdit, UnniUiNumberEditUpdate);
		UnniUiScrollListAddItem(menu4Synth1ScrollList, menu4Synth1WND.wndBorder.endPoint.x - 30, 13, (void*)synth1PhaseEdit, UnniUiNumberEditUpdate);
		UnniUiScrollListAddItem(menu4Synth1ScrollList, menu4Synth1WND.wndBorder.endPoint.x - 30, 13, (void*)synth1DutyEdit, UnniUiNumberEditUpdate);		
		UnniUiScrollListAddItem(menu4Synth1ScrollList, 100, 12,(void*)&synth1PowerOnBox, UnniUiSwitchBoxUpdate);
		UnniUiScrollListAddItem(menu4Synth1ScrollList, 100, 12, (void*)&synth1ResetBox, UnniUiSwitchBoxUpdate);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */																					 
static void Menu4_DeleteConfig(void)
{
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t Menu4_SignalSynth1_Draw(const uint8_t frameNum)
{
		//check menu window config
		if (!menu4Synth1WND.displayCanvas)
		{
				//set canvas
				displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
				displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
				//menu config init
				Menu4_InitConfig();				
		}
		//clear display buffer
		memset(displayBuffer.raw, 0, DISPLAY_BUFFER_SIZE);
		
		//draw menu dialog
		DrawMenuDialogWND(&menu4Synth1WND);
		//draw scroll list with components
		UnniUiScrollListDraw(menu4Synth1ScrollList);
	
		//write buf to display
		D128X64WriteBuf(Display128x64, &displayBuffer);
		return 0;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t Menu4_SignalSynth1_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents)
{
		float floatVal1, floatVal2;

		//block valcoder events upd
		valcoderEventsBlock = 1;
	
		// event - MENU click
		if ( sysEvents & EVENT_BUTTON_MENU_CLICK )
		{
				MenuGoToItemId(mainMenu, Menu3_SignalSynth_ID);
		}		
		// event - UP click
		if ( sysEvents & EVENT_BUTTON_UP_CLICK )
		{
				UnniUiScrollListUp(menu4Synth1ScrollList);
				synth1ResetBox.currentElementNumber = 1;
		}
		// event - DOWN click
		if ( sysEvents & EVENT_BUTTON_DOWN_CLICK )
		{
				UnniUiScrollListDown(menu4Synth1ScrollList);
				synth1ResetBox.currentElementNumber = 1;
		}
		// event - button Left click
		if ( sysEvents & EVENT_BUTTON_LEFT_CLICK )
		{
				UnniUiNumberEditCursorMoveLeft(synth1FrequencyEdit);
				UnniUiNumberEditCursorMoveLeft(synth1PhaseEdit);
				UnniUiNumberEditCursorMoveLeft(synth1DutyEdit);
				UnniUiSwitchBoxPrev(&synth1PowerOnBox);
		}
		// event - button Right click
		if ( sysEvents & EVENT_BUTTON_RIGHT_CLICK )
		{
				UnniUiNumberEditCursorMoveRight(synth1FrequencyEdit);
				UnniUiNumberEditCursorMoveRight(synth1PhaseEdit);
				UnniUiNumberEditCursorMoveRight(synth1DutyEdit);
				UnniUiSwitchBoxNext(&synth1PowerOnBox);
		}
		// event - encoder button pressed and CCW
		if ( (sysEvents & EVENT_VALCODER_BUTTON_PRESSED) && (valcoderEvents & EVENT_VALCODER_CCW) )
		{
				valcoderEvents &= ~EVENT_VALCODER_CCW;
				UnniUiNumberEditCursorMoveLeft(synth1FrequencyEdit);
				UnniUiNumberEditCursorMoveLeft(synth1PhaseEdit);
				UnniUiNumberEditCursorMoveLeft(synth1DutyEdit);
		}		
		// event - encoder button pressed and CW
		if ( (sysEvents & EVENT_VALCODER_BUTTON_PRESSED) && (valcoderEvents & EVENT_VALCODER_CW) )
		{
				valcoderEvents &= ~EVENT_VALCODER_CW;
				UnniUiNumberEditCursorMoveRight(synth1FrequencyEdit);
				UnniUiNumberEditCursorMoveRight(synth1PhaseEdit);
				UnniUiNumberEditCursorMoveRight(synth1DutyEdit);
		}
		// event - OK click or encoder button click
		if ( (sysEvents & EVENT_BUTTON_OK_CLICK) || (sysEvents & EVENT_VALCODER_BUTTON_CLICK) )
		{
				//reset synth1
				UnniUiSwitchBoxNext(&synth1ResetBox);
		}		
		// event - encoder CCW
		if (valcoderEvents & EVENT_VALCODER_CCW)
		{
				//clear flag manually
				valcoderEvents &= ~EVENT_VALCODER_CCW;
				//try change frequency
				if (UnniUiNumberEditDec(synth1FrequencyEdit) == URG_OK)
				{
						AD9850SetFrequency(signalSynth1, UnniUiNumberEditGetFloatValue(synth1FrequencyEdit), 0);						
				}
				//try change phase
				if (UnniUiNumberEditDec(synth1PhaseEdit) == URG_OK)
				{
						//get current value
						AD9850GetPhase(signalSynth1, &floatVal1);
						//change value and get new
						AD9850SetPhase(signalSynth1, UnniUiNumberEditGetFloatValue(synth1PhaseEdit), &floatVal2);
						//compare
						if (floatVal1 == floatVal2)
						{
								//change phase forcibly
								AD9850DecPhase(signalSynth1, &floatVal2);
						}
						UnniUiNumberEditSetValue(synth1PhaseEdit, floatVal2);
				}
				if (UnniUiNumberEditDec(synth1DutyEdit) == URG_OK)
				{
						AD9850SetDuty(signalSynth1, UnniUiNumberEditGetFloatValue(synth1DutyEdit), 0);						
				}
				
				UnniUiSwitchBoxPrev(&synth1PowerOnBox);
		}		
		// event - encoder CW
		if (valcoderEvents & EVENT_VALCODER_CW)
		{
				valcoderEvents &= ~EVENT_VALCODER_CW;
				if (UnniUiNumberEditInc(synth1FrequencyEdit) == URG_OK)
				{
						AD9850SetFrequency(signalSynth1, UnniUiNumberEditGetFloatValue(synth1FrequencyEdit), 0);
				}
				//try change phase
				if (UnniUiNumberEditInc(synth1PhaseEdit) == URG_OK)
				{
						//get current value
						AD9850GetPhase(signalSynth1, &floatVal1);
						//change value and get new
						AD9850SetPhase(signalSynth1, UnniUiNumberEditGetFloatValue(synth1PhaseEdit), &floatVal2);
						//compare
						if (floatVal1 == floatVal2)
						{
								//change phase forcibly
								AD9850IncPhase(signalSynth1, &floatVal2);
								UnniUiNumberEditSetValue(synth1PhaseEdit, floatVal2);
						}
						UnniUiNumberEditSetValue(synth1PhaseEdit, floatVal2);
				}
				if (UnniUiNumberEditInc(synth1DutyEdit) == URG_OK)
				{
						AD9850SetDuty(signalSynth1, UnniUiNumberEditGetFloatValue(synth1DutyEdit), 0);						
				}

				UnniUiSwitchBoxNext(&synth1PowerOnBox);
		}
	
		//unblock valcoder events upd
		valcoderEventsBlock = 0;
		
		return 0;
}
//---------------------------------------------------------------------------------------------------------//

//**************************************end of Menu4_SignalSynthGen1.c*************************************//
