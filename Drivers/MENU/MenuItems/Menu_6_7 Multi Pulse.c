//*********************************************************************************************************//
//generator 407 Menu_6_7 - signal generator
//Created 23.10.2018
//Created by Novikov Dmitry
//*********************************************************************************************************//

//*********************************************************************************************************//
/* includes */
#include "main.h"
#include "main_menu.h"
#include "generator407.h"

//*********************************************************************************************************//
/* defines and constants */

//*********************************************************************************************************//
/* Global variables ---------------------------------------------------------------------------------------*/
extern TREE_MENU* 						mainMenu;
extern D128X64_DISPLAY* 			Display128x64;
extern D128X64_BUFFER 				displayBuffer;
extern URG_CANVAS 						displayCanvas;
extern SYS_EVENTS_DATA 				valcoderEvents;
extern uint8_t 								valcoderEventsBlock;
extern GenParametersTypeDef* 	inputConfig;
extern GeneratorTypeDef* 			genStruct;

extern uint16_t 							_steps;
extern uint16_t 							_precision;
extern uint16_t 							PWM_Countdown[500];
/* Private functions --------------------------------------------------------------------------------------*/
extern void SelectTimer(uint8_t Timer);
extern void SelectTimer1(void);
extern void SelectTimer2(void);
extern void SelectTimer3(void);
extern void GeneratorOn(void);
extern void GeneratorOff(void);
extern void MemorySaveParameters(void);
extern void MemoryLoadParameters(void);
extern void MemoryLoadDefaults(void);
extern void MemoryUnuse(void);

/* Private variables --------------------------------------------------------------------------------------*/
static const uint8_t menu_6_7WNDName[] = "Multi Pulse";
static MAIN_MENU_ELEMENT _menu_6_7Elements[] = {
																								{0, 0, 0, 0}																					
																					   };
static MENU_DIALOG_WND menu_6_7WND = {0};
static URG_FONT_CONFIG numEditFont;
static URG_FONT_CONFIG numEditStrFont;
static UNNI_UI_SCROLLLIST* menu_6_7SignalScrollList = 0;
static UNNI_UI_SWITCH_BOX_ITEM signalGenTypeBoxItems[] = {
                                                           {(uint8_t*)"Tim 1", SelectTimer1},
                                                           {(uint8_t*)"Tim 2", SelectTimer2},
																													 {(uint8_t*)"Tim 3", SelectTimer3}
                                                      };
static UNNI_UI_SWITCHBOX signalGenTypeSwitchBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Timer : ", 3, 1, 0, signalGenTypeBoxItems};
static uint8_t signalGenFrequencyExtEditStr[] = "Hz";
static UNNI_UI_NUMBEREDIT* signalGenFrequencyExtEdit = 0;
static uint8_t signalGenDutyEditStr[] = "% , Duty";
static UNNI_UI_NUMBEREDIT* signalGenDutyEdit = 0;

static uint8_t signalGenCountEditStr[] = "count";
static UNNI_UI_NUMBEREDIT* signalGenCountEdit = 0;

static UNNI_UI_SWITCH_BOX_ITEM generatorPowerOnBoxItems[] = {
                                                           {(uint8_t*)"Off", GeneratorOff},
                                                           {(uint8_t*)"On",  GeneratorOn}
                                                         };
static UNNI_UI_SWITCHBOX GeneratorPowerOnBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Power : ", 2, 1, 0, generatorPowerOnBoxItems,1};
static UNNI_UI_SWITCH_BOX_ITEM MemorySaveLoadGenTypeBoxItems[] = {
                                                           {(uint8_t*)"save", 		0},
																													 {(uint8_t*)"load",  		0},
																													 {(uint8_t*)"default",  0}
                                                      };
static UNNI_UI_SWITCHBOX MemorySaveLoadGenTypeSwitchBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Memory: ", 3, 1, 0, MemorySaveLoadGenTypeBoxItems};
#define MEMORY_SAVE_LOAD_SWITCH_BOX_Menu7 7-1
UNNI_UI_MESSAGEBOX* menu6_7_MessageBox = 0;

#define ARRAY_FILL_DUTY_Menu7 5-1
static UNNI_UI_NUMBEREDIT* menu_NumberPulse = 0;
static uint8_t NumberPulse_EditStr[] = "num";
static UNNI_UI_NUMBEREDIT* menu_ValueDuty = 0;
static uint8_t ValueDuty_EditStr[] = "Duty";
//*********************************************************************************************************//
/* Menu #_6_7 functions */

/* Private functions --------------------------------------------------------------------------------------*/
//---------------------------------------------------------------------------------------------------------//
/*  */										
void UnniUiNumberEditUpdatePlus(void* uiEditNumber, int32_t posX, int32_t posY, UNNI_UI_ELEMENT_FLAGS flags)
{
		//check input data
		if (!uiEditNumber) return;
		if ((posX < 0) || (posY < 0)) return;
		
		//check flags
		if (flags & ELEMENT_VISIBLE)
		{
				//set X,Y
				((UNNI_UI_NUMBEREDIT*)uiEditNumber)->posX = posX;
				((UNNI_UI_NUMBEREDIT*)uiEditNumber)->posY = posY;
				//draw
				UnniUiNumberEditDraw(uiEditNumber);
				
				menu_ValueDuty->posX = posX + 50;
				menu_ValueDuty->posY = posY;
				UnniUiNumberEditDraw(menu_ValueDuty);
		}
		
		//check active or not
		if ( (flags & ELEMENT_ENABLED) && (flags & ELEMENT_ACTIVE) )
		{
				URGDrawRectInBufFast(2, posY + 1, 119, posY - 14, 1, menu_ValueDuty->canvas);
		}
}

static void Menu_6_7_InitConfig(void)
{
		//wnd
		//wnd border
		menu_6_7WND.wndBorder.startPoint.x = 0;
		menu_6_7WND.wndBorder.startPoint.y = displayCanvas.screenHeight - 1;
		menu_6_7WND.wndBorder.endPoint.x = displayCanvas.screenWidth - 1;
		menu_6_7WND.wndBorder.endPoint.y = 0;
		menu_6_7WND.wndBorderThickness = 1;
		//set elements
		menu_6_7WND.menuElements = &_menu_6_7Elements[0];
		//wnd name and font
		menu_6_7WND.wndName = (uint8_t*)&menu_6_7WNDName[0];
		menu_6_7WND.wndNameFont.symbolWidth = 5;
		menu_6_7WND.wndNameFont.symbolHeight = 8;
		menu_6_7WND.wndNameFont.centerAlignment = 0;
		menu_6_7WND.wndNameFont.textSpacing = 1;
		menu_6_7WND.wndNameFont.fontRaw = (uint8_t*)&DefaultFont[0];	
		//set display canvas struct
		menu_6_7WND.displayCanvas = &displayCanvas;
		//init config
		MenuDialogWNDInitConfig(&menu_6_7WND);
		
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
		
		inputConfig->modePWM = 8;
		Gen407SetConfig(genStruct, inputConfig);
		SelectTimer1();
		
		menu_NumberPulse 	= 0;
		menu_ValueDuty 		= 0;
		menu_NumberPulse = UnniUiNumberEditCreate(10, 70, 3, 0, 0, 1, 9999, 1, &numEditFont, &numEditStrFont, (uint8_t*)NumberPulse_EditStr, &displayCanvas);
		UnniUiNumberEditSetFocus(menu_NumberPulse, 1);
		menu_ValueDuty = UnniUiNumberEditCreate(10, 70, 3, 0, 0, 0, 100, 0, &numEditFont, &numEditStrFont, (uint8_t*)ValueDuty_EditStr, &displayCanvas);
		UnniUiNumberEditSetFocus(menu_ValueDuty, 1);
		
		if (!menu_6_7SignalScrollList)
		{
				menu_6_7SignalScrollList = UnniUiScrollListCreate(menu_6_7WND.wndBorder.startPoint.x + 1, menu_6_7WND.wndBorder.startPoint.y - 1,
																											  menu_6_7WND.wndBorder.endPoint.x - 1, menu_6_7WND.wndBorder.startPoint.y - 11, 10, 5, 2, 5,
																											  SCROLLLIST_LEFT_ALIGNED, &displayCanvas);
		}

		//UnniUiScrollListAddItem(menu_6_7SignalScrollList, 100, 12, (void*)&signalGenTypeSwitchBox, UnniUiSwitchBoxUpdate);
		if (!signalGenFrequencyExtEdit)
		{
			signalGenFrequencyExtEdit = UnniUiNumberEditCreate(0, 0, 8, 0, 0, EXT_FREQ_MIN, EXT_FREQ_MAX, EXT_FREQ_DEFAULT,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenFrequencyExtEditStr, &displayCanvas);
			UnniUiScrollListAddItem(menu_6_7SignalScrollList, menu_6_7WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenFrequencyExtEdit, UnniUiNumberEditUpdate);
			UnniUiNumberEditSetFullValue(signalGenFrequencyExtEdit,EXT_FREQ_DEFAULT);	
			Gen407SetExtFreq( genStruct,  UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit));
		}
		if (!signalGenDutyEdit)
		{
			signalGenDutyEdit = UnniUiNumberEditCreate(0, 0, 4, 1, NUMBEREDIT_FRACTIONAL, EXT_DUTY_CYCLE_MIN/10, EXT_DUTY_CYCLE_MAX/10, EXT_DUTY_CYCLE_DEFAULT,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenDutyEditStr, &displayCanvas);
			UnniUiScrollListAddItem(menu_6_7SignalScrollList, menu_6_7WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenDutyEdit, UnniUiNumberEditUpdate);
			UnniUiNumberEditSetFullValue(signalGenDutyEdit,EXT_DUTY_CYCLE_DEFAULT);
			Gen407SetExtDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyEdit) );
		}
		if (!signalGenCountEdit)
		{
			signalGenCountEdit = UnniUiNumberEditCreate(0, 0, 4, 0, 0, COUNTDOWN_MIN, COUNTDOWN_MAX, COUNTDOWN_DEFAULT,
				                                               &numEditFont, &numEditStrFont, (uint8_t*)signalGenCountEditStr, &displayCanvas);
			UnniUiScrollListAddItem(menu_6_7SignalScrollList, menu_6_7WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenCountEdit, UnniUiNumberEditUpdate);
			UnniUiNumberEditSetFullValue(signalGenCountEdit,COUNTDOWN_DEFAULT);	
			Gen407SetCountdown(genStruct, UnniUiNumberEditGetIntValue(signalGenCountEdit));
		}
		UnniUiScrollListAddItem(menu_6_7SignalScrollList, 10, 13, (void*)menu_NumberPulse, UnniUiNumberEditUpdatePlus);
		UnniUiScrollListAddItem(menu_6_7SignalScrollList, 100, 12, (void*)&GeneratorPowerOnBox, UnniUiSwitchBoxUpdate);
		UnniUiScrollListAddItem(menu_6_7SignalScrollList, 100, 12, (void*)&MemorySaveLoadGenTypeSwitchBox, UnniUiSwitchBoxUpdate);
		if (!menu6_7_MessageBox)
		{
				menu6_7_MessageBox = UnniUiMessageBoxInit(10, 56, 108, 46, &numEditStrFont, &numEditStrFont, menu_6_7WND.displayCanvas);
		}			
}
																				 
static void Menu_6_7_DeleteConfig(void)
{
}

uint8_t Menu_6_7_PatternGenerator_Draw(const uint8_t frameNum)
{
		//check menu window config
		if (!menu_6_7WND.displayCanvas)
		{
				//set canvas
				displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
				displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
				//menu config init
				Menu_6_7_InitConfig();				
		}
		//clear display buffer
		memset(displayBuffer.raw, 0, DISPLAY_BUFFER_SIZE);
		
		//draw menu dialog
		DrawMenuDialogWND(&menu_6_7WND);
		//draw scroll list with components
		UnniUiScrollListDraw(menu_6_7SignalScrollList);
		//draw message box
		UnniUiMessageBoxDraw(menu6_7_MessageBox);
	
		//write buf to display
		D128X64WriteBuf(Display128x64, &displayBuffer);
		return 0;
}

uint8_t Menu_6_7_PatternGenerator_InputInit(void)
{
		inputConfig->modePWM = 8;
		Gen407SetConfig(genStruct, inputConfig);
		SelectTimer1();
		
		Gen407SetExtFreq( genStruct,  UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit));		
		UnniUiNumberEditSetFullValue(signalGenDutyEdit,EXT_DUTY_CYCLE_DEFAULT);
		Gen407SetExtDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyEdit) );
		UnniUiNumberEditSetFullValue(signalGenCountEdit,COUNTDOWN_DEFAULT);	
		Gen407SetCountdown(genStruct, UnniUiNumberEditGetIntValue(signalGenCountEdit));
	
		return 0;
}

uint8_t Menu_6_7_PatternGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents)
{
		volatile int32_t floatVal1,  floatVal3, floatVal4, floatVal6=0;
		volatile int32_t floatVal2, floatVal5;
		//block valcoder events upd
		valcoderEventsBlock = 1;
	
		// event - MENU click
		if ( sysEvents & EVENT_BUTTON_MENU_CLICK )
		{
				MenuGoToItemId(mainMenu, Menu6_PatternGenerator_ID);
		}
		// event - UP click
		if ( sysEvents & EVENT_BUTTON_UP_CLICK )
		{
				UnniUiScrollListUp(menu_6_7SignalScrollList);
		}
		// event - DOWN click
		if ( sysEvents & EVENT_BUTTON_DOWN_CLICK )
		{
				UnniUiScrollListDown(menu_6_7SignalScrollList);
		}
		// event - button Left click
		if ( sysEvents & EVENT_BUTTON_LEFT_CLICK )
		{
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDutyEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenCountEdit);
				if (menu_6_7SignalScrollList->currentElementNumber == ARRAY_FILL_DUTY_Menu7)
				{
					UnniUiNumberEditCursorMoveLeft(menu_ValueDuty);
				}
				UnniUiSwitchBoxPrev(&signalGenTypeSwitchBox);
				UnniUiSwitchBoxPrev(&GeneratorPowerOnBox);
				UnniUiSwitchBoxPrev(&MemorySaveLoadGenTypeSwitchBox);
		}
		// event - button Right click
		if ( sysEvents & EVENT_BUTTON_RIGHT_CLICK )
		{
				UnniUiNumberEditCursorMoveRight(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveRight(signalGenDutyEdit);
				UnniUiNumberEditCursorMoveRight(signalGenCountEdit);
				if (menu_6_7SignalScrollList->currentElementNumber == ARRAY_FILL_DUTY_Menu7)
				{
					UnniUiNumberEditCursorMoveRight(menu_ValueDuty);
				}
				UnniUiSwitchBoxNext(&signalGenTypeSwitchBox);
				UnniUiSwitchBoxNext(&GeneratorPowerOnBox);
				UnniUiSwitchBoxNext(&MemorySaveLoadGenTypeSwitchBox);
		}
		// event - encoder button pressed and CCW
		if ( (sysEvents & EVENT_VALCODER_BUTTON_PRESSED) && (valcoderEvents & EVENT_VALCODER_CCW) )
		{
				valcoderEvents &= ~EVENT_VALCODER_CCW;
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDutyEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenCountEdit);

				if (menu_6_7SignalScrollList->currentElementNumber == ARRAY_FILL_DUTY_Menu7)
				{
						UnniUiNumberEditDec(menu_NumberPulse);
						UnniUiNumberEditSetValue(menu_ValueDuty, PWM_Countdown[UnniUiNumberEditGetFullValue(menu_NumberPulse) - 1]);
				}
		}		
		// event - encoder button pressed and CW
		if ( (sysEvents & EVENT_VALCODER_BUTTON_PRESSED) && (valcoderEvents & EVENT_VALCODER_CW) )
		{
				valcoderEvents &= ~EVENT_VALCODER_CW;
				UnniUiNumberEditCursorMoveRight(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveRight(signalGenDutyEdit);
				UnniUiNumberEditCursorMoveRight(signalGenCountEdit);

				if (menu_6_7SignalScrollList->currentElementNumber == ARRAY_FILL_DUTY_Menu7)
				{
						if(UnniUiNumberEditGetFullValue(menu_NumberPulse) < UnniUiNumberEditGetFullValue(signalGenCountEdit))
						{
							UnniUiNumberEditInc(menu_NumberPulse);
							UnniUiNumberEditSetValue(menu_ValueDuty, PWM_Countdown[UnniUiNumberEditGetFullValue(menu_NumberPulse)]);
						}else
						{
							UnniUiNumberEditSetValue(menu_ValueDuty, PWM_Countdown[UnniUiNumberEditGetFullValue(menu_NumberPulse) - 1]);
						}
				}
		}
		// event - OK click or encoder button click
		if ( (sysEvents & EVENT_BUTTON_OK_CLICK) || (sysEvents & EVENT_VALCODER_BUTTON_CLICK) )
		{
				if (menu_6_7SignalScrollList->currentElementNumber == MEMORY_SAVE_LOAD_SWITCH_BOX_Menu7)
				{
						switch (MemorySaveLoadGenTypeSwitchBox.currentElementNumber)
						{
								case 1:
										MemorySaveParameters();
										Menu_6_7_PatternGenerator_InputInit();
										UnniUiMessageBoxShow(menu6_7_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Save OK", 40);	
								break;
								case 2:
										MemoryLoadParameters();
										Menu_6_7_PatternGenerator_InputInit();
										UnniUiMessageBoxShow(menu6_7_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Load OK", 40);	
								break;
								case 3:
										MemoryLoadDefaults();
										Menu_6_7_PatternGenerator_InputInit();
										UnniUiMessageBoxShow(menu6_7_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Load Def", 40);	
								break;
						}					
				}
		}		
		// event - encoder CCW
		if (valcoderEvents & EVENT_VALCODER_CCW)
		{
				//clear flag manually
				valcoderEvents &= ~EVENT_VALCODER_CCW;
								floatVal5 = UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit);
			
				if ((UnniUiNumberEditDec(signalGenFrequencyExtEdit) == URG_OK)&&(floatVal5!=1))
				{
					floatVal1 = UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit);
					floatVal2 = Gen407_GetRealFrequency(floatVal1);
					floatVal3 = Gen407_GetPrevFrequency(floatVal5);
					if ( floatVal2 > 0.0 )
					{
							if (((uint32_t)Gen407_GetRealFrequency(floatVal1) == (uint32_t)Gen407_GetRealFrequency(floatVal5))||(Gen407_GetRealFrequency_Now(_steps, _precision)==0)/*||(floatVal2>floatVal4)*/)
							{
									Gen407SetExtFreq( genStruct,  floatVal3);	
									UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, floatVal3);									
							} 
							else
							{
									Gen407SetExtFreq( genStruct,  floatVal2);			
									UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, floatVal2);								
							}
					}
					if(genStruct->generatorStartFlag == 1)
					{
						Gen407Update(genStruct);
					}
					if( (_steps /2) < genStruct->currentConfig.countdown)
					{
						genStruct->currentConfig.countdown = (_steps /2);
						UnniUiNumberEditSetValue(signalGenCountEdit, genStruct->currentConfig.countdown);
					}
				}
				if (UnniUiNumberEditDec(signalGenDutyEdit) == URG_OK)
				{
						Gen407SetExtDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyEdit) );
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}
				}
				if (UnniUiNumberEditDec(signalGenCountEdit) == URG_OK)
				{
					genStruct->currentConfig.countdown = UnniUiNumberEditGetFullValue(signalGenCountEdit);
					if(genStruct->generatorStartFlag == 1)
					{
						Gen407Update(genStruct);
					}	
				}
				if (menu_6_7SignalScrollList->currentElementNumber == ARRAY_FILL_DUTY_Menu7)
				{
						UnniUiNumberEditDec(menu_ValueDuty);
						PWM_Countdown[UnniUiNumberEditGetFullValue(menu_NumberPulse) - 1] = UnniUiNumberEditGetFullValue(menu_ValueDuty);
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}	
				}
				
				//switch box -> go to previous element
				UnniUiSwitchBoxPrev(&signalGenTypeSwitchBox);
				UnniUiSwitchBoxPrev(&GeneratorPowerOnBox);
				UnniUiSwitchBoxPrev(&MemorySaveLoadGenTypeSwitchBox);
		}		
		// event - encoder CW
		if (valcoderEvents & EVENT_VALCODER_CW)
		{
				//clear flag manually
				valcoderEvents &= ~EVENT_VALCODER_CW;
				floatVal5 = UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit);
				if ((UnniUiNumberEditInc(signalGenFrequencyExtEdit) == URG_OK)&&(floatVal5!=2333333))
				{
					floatVal1 = UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit);
					floatVal2 = Gen407_GetRealFrequency(floatVal1);
					floatVal3 = Gen407_GetNextFrequency(floatVal5);
					if ( floatVal2 > 0.0 )
					{
							if (((uint32_t)Gen407_GetRealFrequency(floatVal1) == (uint32_t)Gen407_GetRealFrequency(floatVal5)) || (Gen407_GetRealFrequency_Now(_steps, _precision)==0))
							{
									Gen407SetExtFreq( genStruct,  floatVal3);	
									UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, floatVal3);									
							} 
							else
							{
									Gen407SetExtFreq( genStruct,  floatVal2);			
									UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, floatVal2);								
							}
					}
					if((genStruct->generatorStartFlag == 1)&&(floatVal1!=1))
					{
						Gen407Update(genStruct);
					}
					if( (_steps /2) < genStruct->currentConfig.countdown)
					{
						genStruct->currentConfig.countdown = (_steps /2);
						UnniUiNumberEditSetValue(signalGenCountEdit, genStruct->currentConfig.countdown);
					}
				}
				if (UnniUiNumberEditInc(signalGenDutyEdit) == URG_OK)
				{
						Gen407SetExtDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyEdit) );
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}					
				}
				
				if((_steps/2) > UnniUiNumberEditGetFullValue(signalGenCountEdit)){
				if (UnniUiNumberEditInc(signalGenCountEdit) == URG_OK)
				{
					
					genStruct->currentConfig.countdown = UnniUiNumberEditGetFullValue(signalGenCountEdit);
					if(genStruct->generatorStartFlag == 1)
					{
						Gen407Update(genStruct);
					}	
				}}
				if (menu_6_7SignalScrollList->currentElementNumber == ARRAY_FILL_DUTY_Menu7)
				{
						UnniUiNumberEditInc(menu_ValueDuty);
						PWM_Countdown[UnniUiNumberEditGetFullValue(menu_NumberPulse) - 1] = UnniUiNumberEditGetFullValue(menu_ValueDuty);
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}	
				}
				//switch box -> go to next element
				UnniUiSwitchBoxNext(&signalGenTypeSwitchBox);
				UnniUiSwitchBoxNext(&GeneratorPowerOnBox);
				UnniUiSwitchBoxNext(&MemorySaveLoadGenTypeSwitchBox);
		}
	
		//unblock valcoder events upd
		valcoderEventsBlock = 0;
		
		return 0;
}

GenParametersTypeDef Mode_7_GetAllParametrs(void)
{
	GenParametersTypeDef ParametrsAll;
	
	if (!menu_6_7WND.displayCanvas)
	{
			//set canvas
			displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
			displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
			//menu config init
			Menu_6_7_InitConfig();				
	}
	
	ParametrsAll.extFreq 			= UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit);
	ParametrsAll.intFreq 			= 0;
	ParametrsAll.extDuty			= UnniUiNumberEditGetIntValue(signalGenDutyEdit);
	ParametrsAll.intDuty 			= 0;
	ParametrsAll.phaseShift		= 0;
	ParametrsAll.modePWM			= 8;
	ParametrsAll.deadTime			= 0;
	ParametrsAll.sinShift			= 0;
	ParametrsAll.countdown		= UnniUiNumberEditGetIntValue(signalGenCountEdit);
	ParametrsAll.sinDirection	= 0;
	ParametrsAll.timStatus		= 1;
	ParametrsAll.Nothing			= 0;
	
	return ParametrsAll;
}

GenParametersTypeDef Mode_7_SetAllParametrs(GenParametersTypeDef ParametrsAll)
{
	
	UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, ParametrsAll.extFreq);
	UnniUiNumberEditSetValue(signalGenDutyEdit, ParametrsAll.extDuty);
	UnniUiNumberEditSetValue(signalGenCountEdit, ParametrsAll.countdown);
	
	if (!menu_6_7WND.displayCanvas)
	{
			//set canvas
			displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
			displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
			//menu config init
			Menu_6_7_InitConfig();				
	}
	
	return ParametrsAll;
}
//---------------------------------------------------------------------------------------------------------//

//**************************************end of Menu_6_7_SignalGenerator.c************************************//
