//*********************************************************************************************************//
//generator 407 menu_6_9 - signal generator
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
extern SYS_EVENTS_DATA 				valcoderEvents; 				//external valcoder events handler struct
extern uint8_t 								valcoderEventsBlock;    //flag - block/unblock change valcoderEvents variable
extern GenParametersTypeDef* 	inputConfig;
extern GeneratorTypeDef* 			genStruct;

extern uint32_t SG_dead_time_ns;

//extern uint16_t _steps;
//extern uint16_t _precision;
//extern uint32_t _realFrequency;
//extern uint32_t _dead_time_ns;
/* Private functions --------------------------------------------------------------------------------------*/
extern void MemorySaveParameters(void);
extern void MemoryLoadParameters(void);
extern void MemoryLoadDefaults(void);
extern void MemoryUnuse(void);

void SG_GeneratorOn(void);
void SG_GeneratorOff(void);

/* Private variables --------------------------------------------------------------------------------------*/
static const uint8_t menu_6_9WNDName[] = "Square Timer";
static MAIN_MENU_ELEMENT _menu_6_9Elements[] = {
																								{0, 0, 0, 0}																					
																					   };
static MENU_DIALOG_WND menu_6_9WND = {0};
static URG_FONT_CONFIG numEditFont;
static URG_FONT_CONFIG numEditStrFont;
static UNNI_UI_SCROLLLIST* menu_6_9SignalScrollList = 0;
static uint8_t signalGenFrequencyExtEditStr[] = "Hz";
static UNNI_UI_NUMBEREDIT* signalGenFrequencyExtEdit = 0;
static uint8_t signalGenDutyEditStr[] = "% , Duty";
static UNNI_UI_NUMBEREDIT* signalGenDutyEdit = 0;
static uint8_t signalGenDeadTimeEditStr[] = "ns, dead";
static UNNI_UI_NUMBEREDIT* signalGenDeadTimeEdit = 0;
static UNNI_UI_SWITCH_BOX_ITEM generatorPowerOnBoxItems[] = {
                                                           {(uint8_t*)"Off", SG_GeneratorOff},
                                                           {(uint8_t*)"On",  SG_GeneratorOn}
                                                         };
static UNNI_UI_SWITCHBOX GeneratorPowerOnBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Power : ", 2, 1, 0, generatorPowerOnBoxItems ,1};
static UNNI_UI_SWITCH_BOX_ITEM MemorySaveLoadGenTypeBoxItems[] = {
                                                           {(uint8_t*)"save", 		0},
																													 {(uint8_t*)"load",  		0},
																													 {(uint8_t*)"default",  0}
                                                      };
static UNNI_UI_SWITCHBOX MemorySaveLoadGenTypeSwitchBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Memory: ", 3, 1, 0, MemorySaveLoadGenTypeBoxItems};
#define MEMORY_SAVE_LOAD_SWITCH_BOX_Menu9 5
UNNI_UI_MESSAGEBOX* menu6_9_MessageBox = 0;
//*********************************************************************************************************//
/* Menu #_6_1 functions */

/* Private functions --------------------------------------------------------------------------------------*/
void SG_GeneratorOn(void)
{
	Gen407Start(genStruct);
}
void SG_GeneratorOff(void)
{
	Gen407Stop(genStruct);
}
//---------------------------------------------------------------------------------------------------------//
/*  */																					 
static void menu_6_9_InitConfig(void)
{
		//wnd
		//wnd border
		menu_6_9WND.wndBorder.startPoint.x = 0;
		menu_6_9WND.wndBorder.startPoint.y = displayCanvas.screenHeight - 1;
		menu_6_9WND.wndBorder.endPoint.x = displayCanvas.screenWidth - 1;
		menu_6_9WND.wndBorder.endPoint.y = 0;
		menu_6_9WND.wndBorderThickness = 1;
		//set elements
		menu_6_9WND.menuElements = &_menu_6_9Elements[0];
		//wnd name and font
		menu_6_9WND.wndName = (uint8_t*)&menu_6_9WNDName[0];
		menu_6_9WND.wndNameFont.symbolWidth = 5;
		menu_6_9WND.wndNameFont.symbolHeight = 8;
		menu_6_9WND.wndNameFont.centerAlignment = 0;
		menu_6_9WND.wndNameFont.textSpacing = 1;
		menu_6_9WND.wndNameFont.fontRaw = (uint8_t*)&DefaultFont[0];	
		//set display canvas struct
		menu_6_9WND.displayCanvas = &displayCanvas;
		//init config
		MenuDialogWNDInitConfig(&menu_6_9WND);
		
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
		
		inputConfig->modePWM = 10;
		Gen407SetConfig(genStruct, inputConfig);

		//create scroll list
		if (!menu_6_9SignalScrollList)
		{
				menu_6_9SignalScrollList = UnniUiScrollListCreate(menu_6_9WND.wndBorder.startPoint.x + 1, menu_6_9WND.wndBorder.startPoint.y - 1,
																											  menu_6_9WND.wndBorder.endPoint.x - 1, menu_6_9WND.wndBorder.startPoint.y - 11, 10, 5, 2, 5,
																											  SCROLLLIST_LEFT_ALIGNED, &displayCanvas);
		}
		
		if (!signalGenFrequencyExtEdit)
		{
				signalGenFrequencyExtEdit = UnniUiNumberEditCreate(0, 0, 8, 0, 0, 0, 10000000, EXT_FREQ_DEFAULT,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenFrequencyExtEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_9SignalScrollList, menu_6_9WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenFrequencyExtEdit, UnniUiNumberEditUpdate);
				Gen407SetExtFreq( genStruct,  UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit));
		}
		if (!signalGenDutyEdit)
		{
				signalGenDutyEdit = UnniUiNumberEditCreate(0, 0, 4, 1, NUMBEREDIT_FRACTIONAL, EXT_DUTY_CYCLE_MIN/10, EXT_DUTY_CYCLE_MAX/10, EXT_DUTY_CYCLE_DEFAULT,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenDutyEditStr, &displayCanvas);
			UnniUiScrollListAddItem(menu_6_9SignalScrollList, menu_6_9WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenDutyEdit, UnniUiNumberEditUpdate);
			UnniUiNumberEditSetFullValue(signalGenDutyEdit,EXT_DUTY_CYCLE_DEFAULT);
			Gen407SetExtDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyEdit) );
			
		}
		if (!signalGenDeadTimeEdit)
		{
				signalGenDeadTimeEdit = UnniUiNumberEditCreate(0, 0, 6, 0, 0, DEADTIME_MIN, DEADTIME_MAX, DEADTIME_DEFAULT,
				                                               &numEditFont, &numEditStrFont, (uint8_t*)signalGenDeadTimeEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_9SignalScrollList, menu_6_9WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenDeadTimeEdit, UnniUiNumberEditUpdate);
				Gen407SetDeadTime( genStruct, UnniUiNumberEditGetIntValue(signalGenDeadTimeEdit) );
		}
		UnniUiScrollListAddItem(menu_6_9SignalScrollList, 100, 12, (void*)&GeneratorPowerOnBox, UnniUiSwitchBoxUpdate);
		
		UnniUiScrollListAddItem(menu_6_9SignalScrollList, 100, 12, (void*)&MemorySaveLoadGenTypeSwitchBox, UnniUiSwitchBoxUpdate);
		if (!menu6_9_MessageBox)
		{
				menu6_9_MessageBox = UnniUiMessageBoxInit(10, 56, 108, 46, &numEditStrFont, &numEditStrFont, menu_6_9WND.displayCanvas);
		}
		
}
																				 
static void menu_6_9_DeleteConfig(void)
{
}

uint8_t Menu_6_9_PatternGenerator_Draw(const uint8_t frameNum)
{
		//check menu window config
		if (!menu_6_9WND.displayCanvas)
		{
				//set canvas
				displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
				displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
				//menu config init
				menu_6_9_InitConfig();				
		}
		//clear display buffer
		memset(displayBuffer.raw, 0, DISPLAY_BUFFER_SIZE);
		
		//draw menu dialog
		DrawMenuDialogWND(&menu_6_9WND);
		//draw scroll list with components
		UnniUiScrollListDraw(menu_6_9SignalScrollList);
		//draw message box
		UnniUiMessageBoxDraw(menu6_9_MessageBox);
	
		//write buf to display
		D128X64WriteBuf(Display128x64, &displayBuffer);
		return 0;
}

uint8_t Menu_6_9_PatternGenerator_InputInit(void)
{
		inputConfig->modePWM = 10;
		Gen407SetConfig(genStruct, inputConfig);
		
		Gen407SetExtFreq( genStruct,  UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit));		

		UnniUiNumberEditSetFullValue(signalGenDutyEdit,EXT_DUTY_CYCLE_DEFAULT);
		Gen407SetExtDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyEdit) );
	
		Gen407SetDeadTime( genStruct, UnniUiNumberEditGetIntValue(signalGenDeadTimeEdit) );
	
		return 0;
}

uint8_t Menu_6_9_PatternGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents)
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
				UnniUiScrollListUp(menu_6_9SignalScrollList);
		}
		// event - DOWN click
		if ( sysEvents & EVENT_BUTTON_DOWN_CLICK )
		{
				UnniUiScrollListDown(menu_6_9SignalScrollList);
		}
		// event - button Left click
		if ( sysEvents & EVENT_BUTTON_LEFT_CLICK )
		{
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDutyEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDeadTimeEdit);
				UnniUiSwitchBoxPrev(&GeneratorPowerOnBox);
				UnniUiSwitchBoxPrev(&MemorySaveLoadGenTypeSwitchBox);
		}
		// event - button Right click
		if ( sysEvents & EVENT_BUTTON_RIGHT_CLICK )
		{
				UnniUiNumberEditCursorMoveRight(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveRight(signalGenDutyEdit);
				UnniUiNumberEditCursorMoveRight(signalGenDeadTimeEdit);
				UnniUiSwitchBoxNext(&GeneratorPowerOnBox);
				UnniUiSwitchBoxNext(&MemorySaveLoadGenTypeSwitchBox);
		}
		// event - encoder button pressed and CCW
		if ( (sysEvents & EVENT_VALCODER_BUTTON_PRESSED) && (valcoderEvents & EVENT_VALCODER_CCW) )
		{
				valcoderEvents &= ~EVENT_VALCODER_CCW;
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDutyEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDeadTimeEdit);

		}		
		// event - encoder button pressed and CW
		if ( (sysEvents & EVENT_VALCODER_BUTTON_PRESSED) && (valcoderEvents & EVENT_VALCODER_CW) )
		{
				valcoderEvents &= ~EVENT_VALCODER_CW;
				UnniUiNumberEditCursorMoveRight(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveRight(signalGenDutyEdit);
				UnniUiNumberEditCursorMoveRight(signalGenDeadTimeEdit);

		}
		// event - OK click or encoder button click
		if ( (sysEvents & EVENT_BUTTON_OK_CLICK) || (sysEvents & EVENT_VALCODER_BUTTON_CLICK) )
		{
				if (menu_6_9SignalScrollList->currentElementNumber == MEMORY_SAVE_LOAD_SWITCH_BOX_Menu9)
				{
						switch (MemorySaveLoadGenTypeSwitchBox.currentElementNumber)
						{
								case 1:
										MemorySaveParameters();
										Menu_6_9_PatternGenerator_InputInit();
										UnniUiMessageBoxShow(menu6_9_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Save OK", 40);	
								break;
								case 2:
										MemoryLoadParameters();
										Menu_6_9_PatternGenerator_InputInit();
										UnniUiMessageBoxShow(menu6_9_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Load OK", 40);	
								break;
								case 3:
										MemoryLoadDefaults();
										Menu_6_9_PatternGenerator_InputInit();
										UnniUiMessageBoxShow(menu6_9_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Load Def", 40);	
								break;
						}					
				}
		}		
		// event - encoder CCW
		if (valcoderEvents & EVENT_VALCODER_CCW)
		{
				//clear flag manually
				valcoderEvents &= ~EVENT_VALCODER_CCW;
			
				if (UnniUiNumberEditDec(signalGenFrequencyExtEdit) == URG_OK)
				{
						Gen407SetExtFreq( genStruct, UnniUiNumberEditGetFullValue(signalGenFrequencyExtEdit) );
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
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
				if (UnniUiNumberEditDec(signalGenDeadTimeEdit) == URG_OK)
				{				
						if(genStruct->currentConfig.deadTime!=0){genStruct->currentConfig.deadTime--;}
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}
						
						if(genStruct->currentConfig.deadTime<=127)
						{
							UnniUiNumberEditSetValue(signalGenDeadTimeEdit, ( SG_dead_time_ns*genStruct->currentConfig.deadTime ) );
						}else if (genStruct->currentConfig.deadTime>127 && genStruct->currentConfig.deadTime<=127+64)
						{
							UnniUiNumberEditSetValue(signalGenDeadTimeEdit, ( ((SG_dead_time_ns/2)*127) + SG_dead_time_ns*(genStruct->currentConfig.deadTime-127)) );
						}else if (genStruct->currentConfig.deadTime>127+64 && genStruct->currentConfig.deadTime<=127+64+32)
						{
							UnniUiNumberEditSetValue(signalGenDeadTimeEdit, ( ((SG_dead_time_ns/2)*127) + ((SG_dead_time_ns/8)*64) + SG_dead_time_ns*(genStruct->currentConfig.deadTime-127-64)) );
						}else if (genStruct->currentConfig.deadTime>127+64+32 && genStruct->currentConfig.deadTime<127+64+32+32)
						{
							UnniUiNumberEditSetValue(signalGenDeadTimeEdit, ( ((SG_dead_time_ns/2)*127) + ((SG_dead_time_ns/8)*64) + ((SG_dead_time_ns/16)*32) + SG_dead_time_ns*(genStruct->currentConfig.deadTime-127-64-32) ) );
						}else if (genStruct->currentConfig.deadTime>=127+64+32+32)
						{
							genStruct->currentConfig.deadTime--;
							UnniUiNumberEditSetValue(signalGenDeadTimeEdit, ( ((SG_dead_time_ns/2)*127) + ((SG_dead_time_ns/8)*64) + ((SG_dead_time_ns/16)*32) + SG_dead_time_ns*(genStruct->currentConfig.deadTime-127-64-32) ) );
						}
				}
				//switch box -> go to previous element
				UnniUiSwitchBoxPrev(&GeneratorPowerOnBox);
				UnniUiSwitchBoxPrev(&MemorySaveLoadGenTypeSwitchBox);
		}		
		// event - encoder CW
		if (valcoderEvents & EVENT_VALCODER_CW)
		{
				//clear flag manually
				valcoderEvents &= ~EVENT_VALCODER_CW;

				if ((UnniUiNumberEditInc(signalGenFrequencyExtEdit) == URG_OK)&&(floatVal5!=2333333))
				{
						Gen407SetExtFreq( genStruct, UnniUiNumberEditGetFullValue(signalGenFrequencyExtEdit) );
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
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
				if (UnniUiNumberEditInc(signalGenDeadTimeEdit) == URG_OK)
				{
						genStruct->currentConfig.deadTime++;
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}
						
						if(genStruct->currentConfig.deadTime<=127)
						{
							UnniUiNumberEditSetValue(signalGenDeadTimeEdit, ( SG_dead_time_ns*genStruct->currentConfig.deadTime ) );
						}else if (genStruct->currentConfig.deadTime>127 && genStruct->currentConfig.deadTime<=127+64)
						{
							UnniUiNumberEditSetValue(signalGenDeadTimeEdit, ( ((SG_dead_time_ns/2)*127) + SG_dead_time_ns*(genStruct->currentConfig.deadTime-127)) );
						}else if (genStruct->currentConfig.deadTime>127+64 && genStruct->currentConfig.deadTime<=127+64+32)
						{
							UnniUiNumberEditSetValue(signalGenDeadTimeEdit, ( ((SG_dead_time_ns/2)*127) + ((SG_dead_time_ns/8)*64) + SG_dead_time_ns*(genStruct->currentConfig.deadTime-127-64)) );
						}else if (genStruct->currentConfig.deadTime>127+64+32 && genStruct->currentConfig.deadTime<127+64+32+32)
						{
							UnniUiNumberEditSetValue(signalGenDeadTimeEdit, ( ((SG_dead_time_ns/2)*127) + ((SG_dead_time_ns/8)*64) + ((SG_dead_time_ns/16)*32) + SG_dead_time_ns*(genStruct->currentConfig.deadTime-127-64-32) ) );
						}else if (genStruct->currentConfig.deadTime>=127+64+32+32)
						{
							genStruct->currentConfig.deadTime--;
							UnniUiNumberEditSetValue(signalGenDeadTimeEdit, ( ((SG_dead_time_ns/2)*127) + ((SG_dead_time_ns/8)*64) + ((SG_dead_time_ns/16)*32) + SG_dead_time_ns*(genStruct->currentConfig.deadTime-127-64-32) ) );
						}	
				}
				//switch box -> go to next element
				UnniUiSwitchBoxNext(&GeneratorPowerOnBox);
				UnniUiSwitchBoxNext(&MemorySaveLoadGenTypeSwitchBox);
		}
	
		//unblock valcoder events upd
		valcoderEventsBlock = 0;
		
		return 0;
}

GenParametersTypeDef Mode_9_GetAllParametrs(void)
{
	GenParametersTypeDef ParametrsAll;
	
	if (!menu_6_9WND.displayCanvas)
	{
			displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
			displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
			menu_6_9_InitConfig();				
	}
	
	ParametrsAll.extFreq 			= UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit);
	ParametrsAll.intFreq 			= 0;
	ParametrsAll.extDuty			= UnniUiNumberEditGetIntValue(signalGenDutyEdit);
	ParametrsAll.intDuty 			= 0;
	ParametrsAll.phaseShift		= 0;
	ParametrsAll.modePWM			= 10;
	ParametrsAll.deadTime			= UnniUiNumberEditGetIntValue(signalGenDeadTimeEdit);
	ParametrsAll.sinShift			= 0;
	ParametrsAll.countdown		= 0;
	ParametrsAll.sinDirection	= 0;
	ParametrsAll.timStatus		= 1;
	ParametrsAll.Nothing			= 0;
	
	return ParametrsAll;
}

GenParametersTypeDef Mode_9_SetAllParametrs(GenParametersTypeDef ParametrsAll)
{
	
	UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, ParametrsAll.extFreq);
	UnniUiNumberEditSetValue(signalGenDutyEdit, ParametrsAll.extDuty);
	UnniUiNumberEditSetValue(signalGenDeadTimeEdit, ParametrsAll.deadTime);
		
	if (!menu_6_9WND.displayCanvas)
	{
			//set canvas
			displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
			displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
			//menu config init
			menu_6_9_InitConfig();				
	}
	
	return ParametrsAll;
}
//---------------------------------------------------------------------------------------------------------//

//**************************************end of menu_6_9_SignalGenerator.c************************************//
