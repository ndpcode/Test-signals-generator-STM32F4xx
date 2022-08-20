//*********************************************************************************************************//
//generator 407 Menu5 - Signal Synthesizer #2
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

#define MENU5_FOCUSED_ELEM_COUNT 3

#define MENU5_FREQ_EDIT_POS_X 14
#define MENU5_FREQ_EDIT_POS_Y 25
#define MENU5_FREQ_EDIT_TEXT_POS_X 106
#define MENU5_FREQ_EDIT_TEXT_POS_Y 25
#define MENU5_PHASE_EDIT_POS_X 6
#define MENU5_PHASE_EDIT_POS_Y 40
#define MENU5_PHASE_EDIT_TEXT_POS_X 64
#define MENU5_PHASE_EDIT_TEXT_POS_Y 40
#define MENU5_DUTY_EDIT_POS_X 6
#define MENU5_DUTY_EDIT_POS_Y 55
#define MENU5_DUTY_EDIT_TEXT_POS_X 64
#define MENU5_DUTY_EDIT_TEXT_POS_Y 55

//*********************************************************************************************************//
/* Global variables ---------------------------------------------------------------------------------------*/
extern TREE_MENU* mainMenu;
extern D128X64_DISPLAY* Display128x64;
extern D128X64_BUFFER displayBuffer;
extern URG_CANVAS displayCanvas;
extern SYS_EVENTS_DATA valcoderEvents; //external valcoder events handler struct
extern uint8_t valcoderEventsBlock;    //flag - block/unblock change valcoderEvents variable
extern AD9850* signalSynth2;           //ad9850 #2

/* Private functions --------------------------------------------------------------------------------------*/
void synth2PowerOff(void);
void synth2PowerOn(void);
void synth2Reset(void);

/* Private variables --------------------------------------------------------------------------------------*/
static uint8_t menu5Synth2WNDName[] = "Synthesizer #2";
static MAIN_MENU_ELEMENT _menu5Elements[] = {
																								{0, 0, 0, 0}																					
																					  };
static MENU_DIALOG_WND menu5Synth2WND = {0};
static uint8_t menu5Synth2Cursor;
static URG_FONT_CONFIG numEditFont;
static UNNI_UI_NUMBEREDIT* synth2FrequencyEdit = 0;
static UNNI_UI_NUMBEREDIT* synth2PhaseEdit = 0;
static UNNI_UI_NUMBEREDIT* synth2DutyEdit = 0;
static uint8_t synth2FrequencyEditStr[] = "Hz";
static uint8_t synth2PhaseEditStr[] = "deg";
static uint8_t synth2DutyEditStr[] = "%";
static URG_FONT_CONFIG numEditStrFont;
static UNNI_UI_SCROLLLIST* menu5Synth2ScrollList;
static char numberStr[20] = "";
static UNNI_UI_SWITCH_BOX_ITEM synth2PowerOnBoxItems[] = {
                                                           {(uint8_t*)"Off", synth2PowerOff},
                                                           {(uint8_t*)"On", synth2PowerOn}
                                                         };
static UNNI_UI_SWITCHBOX synth2PowerOnBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Power : ", 2, 1, 0, synth2PowerOnBoxItems,1};
static UNNI_UI_SWITCH_BOX_ITEM synth2ResetItems[] = {
                                                      {(uint8_t*)"", 0},
																											{(uint8_t*)"OK", synth2Reset}
																										};
static UNNI_UI_SWITCHBOX synth2ResetBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Reset : ", 2, 1, 0, synth2ResetItems,1};

//*********************************************************************************************************//
/* Menu #5 functions */

//---------------------------------------------------------------------------------------------------------//
/*  */
void synth2PowerOff(void)
{
		AD9850SignalOnOff(signalSynth2, 0);
		AD9850Update(signalSynth2);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
void synth2PowerOn(void)
{
		AD9850SignalOnOff(signalSynth2, 1);
		AD9850Update(signalSynth2);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
void synth2Reset(void)
{
		AD9850Reset(signalSynth2);
		AD9850Update(signalSynth2);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */																					 
static void Menu5_InitConfig(void)
{
		double doubleVal;
		float floatVal;
		
		//wnd
		//wnd border
		menu5Synth2WND.wndBorder.startPoint.x = 0;
		menu5Synth2WND.wndBorder.startPoint.y = displayCanvas.screenHeight - 1;
		menu5Synth2WND.wndBorder.endPoint.x = displayCanvas.screenWidth - 1;
		menu5Synth2WND.wndBorder.endPoint.y = 0;
		menu5Synth2WND.wndBorderThickness = 1;
		//set elements
		menu5Synth2WND.menuElements = &_menu5Elements[0];
		//wnd name and font
		menu5Synth2WND.wndName = &menu5Synth2WNDName[0];
		menu5Synth2WND.wndNameFont.symbolWidth = 5;
		menu5Synth2WND.wndNameFont.symbolHeight = 8;
		menu5Synth2WND.wndNameFont.centerAlignment = 0;
		menu5Synth2WND.wndNameFont.textSpacing = 1;
		menu5Synth2WND.wndNameFont.fontRaw = (uint8_t*)&DefaultFont[0];	
		//set display canvas struct
		menu5Synth2WND.displayCanvas = &displayCanvas;
		//init config
		MenuDialogWNDInitConfig(&menu5Synth2WND);

		//set cursor
		menu5Synth2Cursor = 1;
		
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
		if (!synth2FrequencyEdit)
		{
				AD9850GetFrequency(signalSynth2, &doubleVal);
				synth2FrequencyEdit = UnniUiNumberEditCreate(MENU5_FREQ_EDIT_POS_X, MENU5_FREQ_EDIT_POS_Y, 9, 1,
				                                             NUMBEREDIT_FRACTIONAL, AD9850_MIN_FREQUENCY, AD9850_MAX_FREQUENCY, doubleVal,
				                                             &numEditFont, &numEditStrFont, (uint8_t*)synth2FrequencyEditStr, &displayCanvas);
				UnniUiNumberEditSetFocus(synth2FrequencyEdit, 1);
		}
		//config synthesizer 1 phase edit
		if (!synth2PhaseEdit)
		{
				AD9850GetPhase(signalSynth2, &floatVal);
				synth2PhaseEdit = UnniUiNumberEditCreate(MENU5_PHASE_EDIT_POS_X, MENU5_PHASE_EDIT_POS_Y, 5, 2,
				                                         NUMBEREDIT_FRACTIONAL, AD9850_MIN_PHASE, AD9850_MAX_PHASE, floatVal,
				                                         &numEditFont, &numEditStrFont, (uint8_t*)synth2PhaseEditStr, &displayCanvas);
				UnniUiNumberEditSetFocus(synth2PhaseEdit, 0);
		}
		//config synthesizer 1 phase edit
		if (!synth2DutyEdit)
		{
				AD9850GetDuty(signalSynth2, &floatVal);
				synth2DutyEdit = UnniUiNumberEditCreate(MENU5_DUTY_EDIT_POS_X, MENU5_DUTY_EDIT_POS_Y, 5, 2,
				                                        NUMBEREDIT_FRACTIONAL, AD9850_MIN_DUTY, AD9850_MAX_DUTY, floatVal,
				                                        &numEditFont, &numEditStrFont, (uint8_t*)synth2DutyEditStr, &displayCanvas);
				UnniUiNumberEditSetFocus(synth2DutyEdit, 0);
		}
		//config synth1 and power SwitchBox
		AD9850SetAutoupdate(signalSynth2, 1);
		AD9850SignalOnOff(signalSynth2, 0);
		AD9850Update(signalSynth2);
		synth2PowerOnBox.currentElementNumber = 1;
		//config synth1 reset SwitchBox
		synth2ResetBox.currentElementNumber = 1;
		
		//create scroll list
		menu5Synth2ScrollList = UnniUiScrollListCreate(menu5Synth2WND.wndBorder.startPoint.x + 1, menu5Synth2WND.wndBorder.startPoint.y - 1,
		                                               menu5Synth2WND.wndBorder.endPoint.x - 1, menu5Synth2WND.wndBorder.startPoint.y - 11, 10, 5, 2, 5,
		                                               SCROLLLIST_LEFT_ALIGNED, &displayCanvas);
		//add elements to scroll list
		UnniUiScrollListAddItem(menu5Synth2ScrollList, menu5Synth2WND.wndBorder.endPoint.x - 30, 13, (void*)synth2FrequencyEdit, UnniUiNumberEditUpdate);
		UnniUiScrollListAddItem(menu5Synth2ScrollList, menu5Synth2WND.wndBorder.endPoint.x - 30, 13, (void*)synth2PhaseEdit, UnniUiNumberEditUpdate);
		UnniUiScrollListAddItem(menu5Synth2ScrollList, menu5Synth2WND.wndBorder.endPoint.x - 30, 13, (void*)synth2DutyEdit, UnniUiNumberEditUpdate);		
		UnniUiScrollListAddItem(menu5Synth2ScrollList, 100, 12,(void*)&synth2PowerOnBox, UnniUiSwitchBoxUpdate);
		UnniUiScrollListAddItem(menu5Synth2ScrollList, 100, 12, (void*)&synth2ResetBox, UnniUiSwitchBoxUpdate);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */																					 
static void Menu5_DeleteConfig(void)
{
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t Menu5_SignalSynth2_Draw(const uint8_t frameNum)
{
		//check menu window config
		if (!menu5Synth2WND.displayCanvas)
		{
				//set canvas
				displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
				displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
				//menu config init
				Menu5_InitConfig();				
		}
		//clear display buffer
		memset(displayBuffer.raw, 0, DISPLAY_BUFFER_SIZE);
		
		//draw menu dialog
		DrawMenuDialogWND(&menu5Synth2WND);
		//draw scroll list with components
		UnniUiScrollListDraw(menu5Synth2ScrollList);
	
		//write buf to display
		D128X64WriteBuf(Display128x64, &displayBuffer);
		return 0;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t Menu5_SignalSynth2_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents)
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
				UnniUiScrollListUp(menu5Synth2ScrollList);
				synth2ResetBox.currentElementNumber = 1;
		}
		// event - DOWN click
		if ( sysEvents & EVENT_BUTTON_DOWN_CLICK )
		{
				UnniUiScrollListDown(menu5Synth2ScrollList);
				synth2ResetBox.currentElementNumber = 1;
		}
		// event - button Left click
		if ( sysEvents & EVENT_BUTTON_LEFT_CLICK )
		{
				UnniUiNumberEditCursorMoveLeft(synth2FrequencyEdit);
				UnniUiNumberEditCursorMoveLeft(synth2PhaseEdit);
				UnniUiNumberEditCursorMoveLeft(synth2DutyEdit);
				UnniUiSwitchBoxPrev(&synth2PowerOnBox);
		}
		// event - button Right click
		if ( sysEvents & EVENT_BUTTON_RIGHT_CLICK )
		{
				UnniUiNumberEditCursorMoveRight(synth2FrequencyEdit);
				UnniUiNumberEditCursorMoveRight(synth2PhaseEdit);
				UnniUiNumberEditCursorMoveRight(synth2DutyEdit);
				UnniUiSwitchBoxNext(&synth2PowerOnBox);
		}
		// event - encoder button pressed and CCW
		if ( (sysEvents & EVENT_VALCODER_BUTTON_PRESSED) && (valcoderEvents & EVENT_VALCODER_CCW) )
		{
				valcoderEvents &= ~EVENT_VALCODER_CCW;
				UnniUiNumberEditCursorMoveLeft(synth2FrequencyEdit);
				UnniUiNumberEditCursorMoveLeft(synth2PhaseEdit);
				UnniUiNumberEditCursorMoveLeft(synth2DutyEdit);
		}		
		// event - encoder button pressed and CW
		if ( (sysEvents & EVENT_VALCODER_BUTTON_PRESSED) && (valcoderEvents & EVENT_VALCODER_CW) )
		{
				valcoderEvents &= ~EVENT_VALCODER_CW;
				UnniUiNumberEditCursorMoveRight(synth2FrequencyEdit);
				UnniUiNumberEditCursorMoveRight(synth2PhaseEdit);
				UnniUiNumberEditCursorMoveRight(synth2DutyEdit);
		}
		// event - OK click or encoder button click
		if ( (sysEvents & EVENT_BUTTON_OK_CLICK) || (sysEvents & EVENT_VALCODER_BUTTON_CLICK) )
		{
				//reset synth1
				UnniUiSwitchBoxNext(&synth2ResetBox);
		}		
		// event - encoder CCW
		if (valcoderEvents & EVENT_VALCODER_CCW)
		{
				//clear flag manually
				valcoderEvents &= ~EVENT_VALCODER_CCW;
				//try change frequency
				if (UnniUiNumberEditDec(synth2FrequencyEdit) == URG_OK)
				{
						AD9850SetFrequency(signalSynth2, UnniUiNumberEditGetFloatValue(synth2FrequencyEdit), 0);						
				}
				//try change phase
				if (UnniUiNumberEditDec(synth2PhaseEdit) == URG_OK)
				{
						//get current value
						AD9850GetPhase(signalSynth2, &floatVal1);
						//change value and get new
						AD9850SetPhase(signalSynth2, UnniUiNumberEditGetFloatValue(synth2PhaseEdit), &floatVal2);
						//compare
						if (floatVal1 == floatVal2)
						{
								//change phase forcibly
								AD9850DecPhase(signalSynth2, &floatVal2);
						}
						UnniUiNumberEditSetValue(synth2PhaseEdit, floatVal2);
				}
				if (UnniUiNumberEditDec(synth2DutyEdit) == URG_OK)
				{
						AD9850SetDuty(signalSynth2, UnniUiNumberEditGetFloatValue(synth2DutyEdit), 0);						
				}

				UnniUiSwitchBoxPrev(&synth2PowerOnBox);
		}		
		// event - encoder CW
		if (valcoderEvents & EVENT_VALCODER_CW)
		{
				valcoderEvents &= ~EVENT_VALCODER_CW;
				if (UnniUiNumberEditInc(synth2FrequencyEdit) == URG_OK)
				{
						AD9850SetFrequency(signalSynth2, UnniUiNumberEditGetFloatValue(synth2FrequencyEdit), 0);
				}
				//try change phase
				if (UnniUiNumberEditInc(synth2PhaseEdit) == URG_OK)
				{
						//get current value
						AD9850GetPhase(signalSynth2, &floatVal1);
						//change value and get new
						AD9850SetPhase(signalSynth2, UnniUiNumberEditGetFloatValue(synth2PhaseEdit), &floatVal2);
						//compare
						if (floatVal1 == floatVal2)
						{
								//change phase forcibly
								AD9850IncPhase(signalSynth2, &floatVal2);
								UnniUiNumberEditSetValue(synth2PhaseEdit, floatVal2);
						}
						UnniUiNumberEditSetValue(synth2PhaseEdit, floatVal2);
				}
				if (UnniUiNumberEditInc(synth2DutyEdit) == URG_OK)
				{
						AD9850SetDuty(signalSynth2, UnniUiNumberEditGetFloatValue(synth2DutyEdit), 0);						
				}

				UnniUiSwitchBoxNext(&synth2PowerOnBox);
		}
	
		//unblock valcoder events upd
		valcoderEventsBlock = 0;
		
		return 0;
}
//---------------------------------------------------------------------------------------------------------//

//**************************************end of Menu5_SignalSynthGen2.c*************************************//
