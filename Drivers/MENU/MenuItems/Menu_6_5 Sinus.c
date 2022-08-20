//*********************************************************************************************************//
//generator 407 Menu_6_5 - signal generator
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
extern SYS_EVENTS_DATA 				valcoderEvents; //external valcoder events handler struct
extern uint8_t 								valcoderEventsBlock;    //flag - block/unblock change valcoderEvents variable
extern GenParametersTypeDef* 	inputConfig;
extern GeneratorTypeDef* 			genStruct;

volatile extern uint16_t _steps;
volatile extern uint16_t _precision;
extern uint32_t _dead_time_ns;

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


void SelectDirectionRight(void);
void SelectDirectionLeft (void);
/* Private variables --------------------------------------------------------------------------------------*/
static const uint8_t menu_6_5WNDName[] = "Sinus";
static MAIN_MENU_ELEMENT _menu_6_5Elements[] = {
																								{0, 0, 0, 0}																					
																					   };
static MENU_DIALOG_WND menu_6_5WND = {0};
static URG_FONT_CONFIG numEditFont;
static URG_FONT_CONFIG numEditStrFont;
static UNNI_UI_SCROLLLIST* menu_6_5SignalScrollList = 0;
static UNNI_UI_SWITCH_BOX_ITEM signalGenTypeBoxItems[] = {
                                                           {(uint8_t*)"Tim 1", SelectTimer1},
                                                           {(uint8_t*)"Tim 2", SelectTimer2},
																													 {(uint8_t*)"Tim 3", SelectTimer3}
                                                      };
static UNNI_UI_SWITCHBOX signalGenTypeSwitchBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Timer : ", 3, 1, 0, signalGenTypeBoxItems};
static uint8_t signalGenFrequencyExtEditStr[] = "Hz";
static UNNI_UI_NUMBEREDIT* signalGenFrequencyExtEdit = 0;
static uint8_t signalGenFrequencyIntEditStr[] = "Hz i";
static UNNI_UI_NUMBEREDIT* signalGenFrequencyIntEdit = 0;
static uint8_t signalGenDutyExtEditStr[] = "% , Duty e";
static UNNI_UI_NUMBEREDIT* signalGenDutyExtEdit = 0;
static uint8_t signalGenDutyIntEditStr[] = "% , Duty i";
static UNNI_UI_NUMBEREDIT* signalGenDutyIntEdit = 0;
static UNNI_UI_SWITCH_BOX_ITEM generatorSinusDirectionItems[] = {
                                                           {(uint8_t*)"Right", SelectDirectionRight},
                                                           {(uint8_t*)"Left",  SelectDirectionLeft}
                                                         };
static UNNI_UI_SWITCHBOX GeneratorSinusDirectionBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Direction : ", 2, 1, 0, generatorSinusDirectionItems ,1};
static uint8_t signalGenKCenterEditStr[] = "% , center";
static UNNI_UI_NUMBEREDIT* signalGenKCenterEdit = 0;
static uint8_t signalGenDeadTimeEditStr[] = "ns, dead";
static UNNI_UI_NUMBEREDIT* signalGenDeadTimeEdit = 0;
static UNNI_UI_SWITCH_BOX_ITEM generatorPowerOnBoxItems[] = {
                                                           {(uint8_t*)"Off", GeneratorOff},
                                                           {(uint8_t*)"On",  GeneratorOn}
                                                         };
static UNNI_UI_SWITCHBOX GeneratorPowerOnBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Power : ", 2, 1, 0, generatorPowerOnBoxItems ,1};
static UNNI_UI_SWITCH_BOX_ITEM MemorySaveLoadGenTypeBoxItems[] = {
                                                           {(uint8_t*)"save", 		0},
																													 {(uint8_t*)"load",  		0},
																													 {(uint8_t*)"default",  0}
                                                      };
static UNNI_UI_SWITCHBOX MemorySaveLoadGenTypeSwitchBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Memory: ", 3, 1, 0, MemorySaveLoadGenTypeBoxItems};
#define MEMORY_SAVE_LOAD_SWITCH_BOX_Menu5 10-1
UNNI_UI_MESSAGEBOX* menu6_5_MessageBox = 0;
//*********************************************************************************************************//
/* Menu #_6_5 functions */

/* Private functions --------------------------------------------------------------------------------------*/
void SelectDirectionRight(void)
{
//	inputConfig->timStatus = 1;
//	Gen407SetConfig(genStruct, inputConfig);
	genStruct->currentConfig.sinDirection = 1;
	Gen407Update(genStruct);
}
void SelectDirectionLeft(void)
{
//	inputConfig->timStatus = 2;
//	Gen407SetConfig(genStruct, inputConfig);
	genStruct->currentConfig.sinDirection = 0;
	Gen407Update(genStruct);
}
//---------------------------------------------------------------------------------------------------------//
/*  */																					 
static void Menu_6_5_InitConfig(void)
{
		//wnd
		//wnd border
		menu_6_5WND.wndBorder.startPoint.x = 0;
		menu_6_5WND.wndBorder.startPoint.y = displayCanvas.screenHeight - 1;
		menu_6_5WND.wndBorder.endPoint.x = displayCanvas.screenWidth - 1;
		menu_6_5WND.wndBorder.endPoint.y = 0;
		menu_6_5WND.wndBorderThickness = 1;
		//set elements
		menu_6_5WND.menuElements = &_menu_6_5Elements[0];
		//wnd name and font
		menu_6_5WND.wndName = (uint8_t*)&menu_6_5WNDName[0];
		menu_6_5WND.wndNameFont.symbolWidth = 5;
		menu_6_5WND.wndNameFont.symbolHeight = 8;
		menu_6_5WND.wndNameFont.centerAlignment = 0;
		menu_6_5WND.wndNameFont.textSpacing = 1;
		menu_6_5WND.wndNameFont.fontRaw = (uint8_t*)&DefaultFont[0];	
		//set display canvas struct
		menu_6_5WND.displayCanvas = &displayCanvas;
		//init config
		MenuDialogWNDInitConfig(&menu_6_5WND);
		
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
		
		inputConfig->modePWM = 6;
		Gen407SetConfig(genStruct, inputConfig);
		SelectTimer1();
		
		if (!menu_6_5SignalScrollList)
		{
				menu_6_5SignalScrollList = UnniUiScrollListCreate(menu_6_5WND.wndBorder.startPoint.x + 1, menu_6_5WND.wndBorder.startPoint.y - 1,
																											  menu_6_5WND.wndBorder.endPoint.x - 1, menu_6_5WND.wndBorder.startPoint.y - 11, 10, 5, 2, 5,
																											  SCROLLLIST_LEFT_ALIGNED, &displayCanvas);
		}

		//UnniUiScrollListAddItem(menu_6_5SignalScrollList, 100, 12, (void*)&signalGenTypeSwitchBox, UnniUiSwitchBoxUpdate);
				
		if (!signalGenFrequencyExtEdit)
		{
				signalGenFrequencyExtEdit = UnniUiNumberEditCreate(0, 0, 8, 0, 0, EXT_FREQ_MIN, EXT_FREQ_MAX, EXT_FREQ_DEFAULT,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenFrequencyExtEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_5SignalScrollList, menu_6_5WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenFrequencyExtEdit, UnniUiNumberEditUpdate);
				Gen407SetExtFreq( genStruct,  UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit));
		}
		if (!signalGenFrequencyIntEdit)
		{
				signalGenFrequencyIntEdit = UnniUiNumberEditCreate(0, 0, 8, 0, 0, INT_FREQ_MIN, INT_FREQ_MAX, INT_FREQ_DEFAULT,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenFrequencyIntEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_5SignalScrollList, menu_6_5WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenFrequencyIntEdit, UnniUiNumberEditUpdate);
				Gen407SetIntFreq( genStruct,  UnniUiNumberEditGetIntValue(signalGenFrequencyIntEdit));
		}
		if (!signalGenDutyExtEdit)
		{
				signalGenDutyExtEdit = UnniUiNumberEditCreate(0, 0, 4, 1, NUMBEREDIT_FRACTIONAL, EXT_DUTY_CYCLE_MIN/10, EXT_DUTY_CYCLE_MAX/10, EXT_DUTY_CYCLE_DEFAULT,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenDutyExtEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_5SignalScrollList, menu_6_5WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenDutyExtEdit, UnniUiNumberEditUpdate);
				UnniUiNumberEditSetFullValue(signalGenDutyExtEdit,EXT_DUTY_CYCLE_DEFAULT);
				Gen407SetExtDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyExtEdit) );
		}
		if (!signalGenDutyIntEdit)
		{
				signalGenDutyIntEdit = UnniUiNumberEditCreate(0, 0, 4, 1, NUMBEREDIT_FRACTIONAL, INT_DUTY_CYCLE_MIN/10, INT_DUTY_CYCLE_MAX/10, INT_DUTY_CYCLE_DEFAULT,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenDutyIntEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_5SignalScrollList, menu_6_5WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenDutyIntEdit, UnniUiNumberEditUpdate);
				UnniUiNumberEditSetFullValue(signalGenDutyIntEdit,INT_DUTY_CYCLE_DEFAULT);
				Gen407SetIntDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyIntEdit) );
		}
		UnniUiScrollListAddItem(menu_6_5SignalScrollList, 100, 12, (void*)&GeneratorSinusDirectionBox, UnniUiSwitchBoxUpdate);
		
		if (!signalGenKCenterEdit)
		{
				signalGenKCenterEdit = UnniUiNumberEditCreate(0, 0, 4, 0, 0, SINSHIFT_MIN, SINSHIFT_MAX, SINSHIFT_DEFAULT, 
																														&numEditFont, &numEditStrFont, (uint8_t*)signalGenKCenterEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_5SignalScrollList, menu_6_5WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenKCenterEdit, UnniUiNumberEditUpdate);
		}
		if (!signalGenDeadTimeEdit)
		{
				signalGenDeadTimeEdit = UnniUiNumberEditCreate(0, 0, 7, 0, 0, DEADTIME_MIN, DEADTIME_MAX, DEADTIME_DEFAULT,
				                                               &numEditFont, &numEditStrFont, (uint8_t*)signalGenDeadTimeEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_5SignalScrollList, menu_6_5WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenDeadTimeEdit, UnniUiNumberEditUpdate);
				Gen407SetDeadTime( genStruct, UnniUiNumberEditGetIntValue(signalGenDeadTimeEdit) );
		}

		UnniUiScrollListAddItem(menu_6_5SignalScrollList, 100, 12, (void*)&GeneratorPowerOnBox, UnniUiSwitchBoxUpdate);
		
		UnniUiScrollListAddItem(menu_6_5SignalScrollList, 100, 12, (void*)&MemorySaveLoadGenTypeSwitchBox, UnniUiSwitchBoxUpdate);
		if (!menu6_5_MessageBox)
		{
				menu6_5_MessageBox = UnniUiMessageBoxInit(10, 56, 108, 46, &numEditStrFont, &numEditStrFont, menu_6_5WND.displayCanvas);
		}
}
																				 
static void Menu_6_5_DeleteConfig(void)
{
}

uint8_t Menu_6_5_PatternGenerator_Draw(const uint8_t frameNum)
{
		//check menu window config
		if (!menu_6_5WND.displayCanvas)
		{
				//set canvas
				displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
				displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
				//menu config init
				Menu_6_5_InitConfig();				
		}
		//clear display buffer
		memset(displayBuffer.raw, 0, DISPLAY_BUFFER_SIZE);
		
		//draw menu dialog
		DrawMenuDialogWND(&menu_6_5WND);
		//draw scroll list with components
		UnniUiScrollListDraw(menu_6_5SignalScrollList);
		//draw message box
		UnniUiMessageBoxDraw(menu6_5_MessageBox);
	
		//write buf to display
		D128X64WriteBuf(Display128x64, &displayBuffer);
		return 0;
}

uint8_t Menu_6_5_PatternGenerator_InputInit(void)
{
		inputConfig->modePWM = 6;
		Gen407SetConfig(genStruct, inputConfig);
		SelectTimer1();
		
		Gen407SetExtFreq( genStruct,  UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit));		
		Gen407SetIntFreq( genStruct,  UnniUiNumberEditGetIntValue(signalGenFrequencyIntEdit));
		UnniUiNumberEditSetFullValue(signalGenDutyExtEdit,EXT_DUTY_CYCLE_DEFAULT);
		Gen407SetExtDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyExtEdit) );
		UnniUiNumberEditSetFullValue(signalGenDutyIntEdit,INT_DUTY_CYCLE_DEFAULT);
		Gen407SetIntDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyIntEdit) );
	
		Gen407SetDeadTime( genStruct, UnniUiNumberEditGetIntValue(signalGenDeadTimeEdit) );
	
		return 0;
}

uint8_t Menu_6_5_PatternGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents)
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
				UnniUiScrollListUp(menu_6_5SignalScrollList);
		}
		// event - DOWN click
		if ( sysEvents & EVENT_BUTTON_DOWN_CLICK )
		{
				UnniUiScrollListDown(menu_6_5SignalScrollList);
		}
		// event - button Left click
		if ( sysEvents & EVENT_BUTTON_LEFT_CLICK )
		{
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyIntEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDutyExtEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDutyIntEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenKCenterEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDeadTimeEdit);
				UnniUiSwitchBoxPrev(&GeneratorSinusDirectionBox);
				UnniUiSwitchBoxPrev(&signalGenTypeSwitchBox);
				UnniUiSwitchBoxPrev(&GeneratorPowerOnBox);
				UnniUiSwitchBoxPrev(&MemorySaveLoadGenTypeSwitchBox);
		}
		// event - button Right click
		if ( sysEvents & EVENT_BUTTON_RIGHT_CLICK )
		{
				UnniUiNumberEditCursorMoveRight(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveRight(signalGenFrequencyIntEdit);
				UnniUiNumberEditCursorMoveRight(signalGenDutyExtEdit);
				UnniUiNumberEditCursorMoveRight(signalGenDutyIntEdit);
				UnniUiNumberEditCursorMoveRight(signalGenKCenterEdit);
				UnniUiNumberEditCursorMoveRight(signalGenDeadTimeEdit);
				UnniUiSwitchBoxNext(&GeneratorSinusDirectionBox);
				UnniUiSwitchBoxNext(&signalGenTypeSwitchBox);
				UnniUiSwitchBoxNext(&GeneratorPowerOnBox);
				UnniUiSwitchBoxNext(&MemorySaveLoadGenTypeSwitchBox);
		}
		// event - encoder button pressed and CCW
		if ( (sysEvents & EVENT_VALCODER_BUTTON_PRESSED) && (valcoderEvents & EVENT_VALCODER_CCW) )
		{
				valcoderEvents &= ~EVENT_VALCODER_CCW;
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyIntEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenKCenterEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDutyExtEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDutyIntEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDeadTimeEdit);

		}		
		// event - encoder button pressed and CW
		if ( (sysEvents & EVENT_VALCODER_BUTTON_PRESSED) && (valcoderEvents & EVENT_VALCODER_CW) )
		{
				valcoderEvents &= ~EVENT_VALCODER_CW;
				UnniUiNumberEditCursorMoveRight(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveRight(signalGenFrequencyIntEdit);
				UnniUiNumberEditCursorMoveRight(signalGenDutyExtEdit);
				UnniUiNumberEditCursorMoveRight(signalGenDutyIntEdit);
				UnniUiNumberEditCursorMoveRight(signalGenKCenterEdit);
				UnniUiNumberEditCursorMoveRight(signalGenDeadTimeEdit);

		}
		// event - OK click or encoder button click
		if ( (sysEvents & EVENT_BUTTON_OK_CLICK) || (sysEvents & EVENT_VALCODER_BUTTON_CLICK) )
		{
				if (menu_6_5SignalScrollList->currentElementNumber == MEMORY_SAVE_LOAD_SWITCH_BOX_Menu5)
				{
						switch (MemorySaveLoadGenTypeSwitchBox.currentElementNumber)
						{
								case 1:
										MemorySaveParameters();
										Menu_6_5_PatternGenerator_InputInit();
										UnniUiMessageBoxShow(menu6_5_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Save OK", 40);	
								break;
								case 2:
										MemoryLoadParameters();
										Menu_6_5_PatternGenerator_InputInit();
										UnniUiMessageBoxShow(menu6_5_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Load OK", 40);	
								break;
								case 3:
										MemoryLoadDefaults();
										Menu_6_5_PatternGenerator_InputInit();
										UnniUiMessageBoxShow(menu6_5_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Load Def", 40);	
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
						genStruct->currentConfig.intFreq = 0;
						Gen407Update(genStruct);
					}
					genStruct->currentConfig.intFreq = _steps * genStruct->currentConfig.extFreq;
					UnniUiNumberEditSetValue(signalGenFrequencyIntEdit, genStruct->currentConfig.intFreq);
				}
				if (UnniUiNumberEditDec(signalGenFrequencyIntEdit) == URG_OK)
				{					
						//Gen407SetIntFreq( genStruct, UnniUiNumberEditGetFullValue(signalGenFrequencyIntEdit) );
						Gen407SetIntFreq( genStruct, Gen407_GetPrevInternalFrequency(UnniUiNumberEditGetIntValue(signalGenFrequencyIntEdit), genStruct->currentConfig.extFreq) );
						UnniUiNumberEditSetValue(signalGenFrequencyIntEdit, Gen407_GetPrevInternalFrequency(UnniUiNumberEditGetIntValue(signalGenFrequencyIntEdit), genStruct->currentConfig.extFreq));
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}
				}
				if (UnniUiNumberEditDec(signalGenKCenterEdit) == URG_OK)
				{
						Gen407SetSinShift( genStruct, UnniUiNumberEditGetFullValue(signalGenKCenterEdit) );
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}					
				}
				if (UnniUiNumberEditDec(signalGenDutyExtEdit) == URG_OK)
				{
						Gen407SetExtDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyExtEdit) );
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}
				}
				if (UnniUiNumberEditDec(signalGenDutyIntEdit) == URG_OK)
				{
						Gen407SetIntDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyIntEdit) );
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}
				}
				if (UnniUiNumberEditDec(signalGenDeadTimeEdit) == URG_OK)
				{
						if(genStruct->currentConfig.deadTime!=0){genStruct->currentConfig.deadTime--;}
						UnniUiNumberEditSetValue(signalGenDeadTimeEdit, (_dead_time_ns * genStruct->currentConfig.deadTime) );
						
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}
				}
				//switch box -> go to previous element
				UnniUiSwitchBoxPrev(&GeneratorSinusDirectionBox);
				UnniUiSwitchBoxPrev(&signalGenTypeSwitchBox);
				UnniUiSwitchBoxPrev(&GeneratorPowerOnBox);
				UnniUiSwitchBoxPrev(&MemorySaveLoadGenTypeSwitchBox);
		}		
		// event - encoder CW
		if (valcoderEvents & EVENT_VALCODER_CW)
		{
				//clear flag manually
				valcoderEvents &= ~EVENT_VALCODER_CW;
				
				if (UnniUiNumberEditInc(signalGenFrequencyExtEdit) == URG_OK)
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
						genStruct->currentConfig.intFreq = 0;
						Gen407Update(genStruct);
					}
					genStruct->currentConfig.intFreq = _steps * genStruct->currentConfig.extFreq;
					UnniUiNumberEditSetValue(signalGenFrequencyIntEdit, genStruct->currentConfig.intFreq);					
				}
				if (UnniUiNumberEditInc(signalGenFrequencyIntEdit) == URG_OK)
				{
						//Gen407SetIntFreq( genStruct, UnniUiNumberEditGetFullValue(signalGenFrequencyIntEdit) );
						Gen407SetIntFreq( genStruct, Gen407_GetNextInternalFrequency(UnniUiNumberEditGetIntValue(signalGenFrequencyIntEdit), genStruct->currentConfig.extFreq) );
						UnniUiNumberEditSetValue(signalGenFrequencyIntEdit, Gen407_GetNextInternalFrequency(UnniUiNumberEditGetIntValue(signalGenFrequencyIntEdit), genStruct->currentConfig.extFreq));
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}						
				}
				if (UnniUiNumberEditInc(signalGenKCenterEdit) == URG_OK)
				{
						Gen407SetSinShift( genStruct, UnniUiNumberEditGetFullValue(signalGenKCenterEdit) );
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}					
				}
				if (UnniUiNumberEditInc(signalGenDutyExtEdit) == URG_OK)
				{
						Gen407SetExtDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyExtEdit) );
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}
				}
				if (UnniUiNumberEditInc(signalGenDutyIntEdit) == URG_OK)
				{
						Gen407SetIntDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyIntEdit) );
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}
				}
				if (UnniUiNumberEditInc(signalGenDeadTimeEdit) == URG_OK)
				{
						genStruct->currentConfig.deadTime++;
						UnniUiNumberEditSetValue(signalGenDeadTimeEdit, (_dead_time_ns * genStruct->currentConfig.deadTime) );
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}
				}
				//switch box -> go to next element
				UnniUiSwitchBoxNext(&GeneratorSinusDirectionBox);
				UnniUiSwitchBoxNext(&signalGenTypeSwitchBox);
				UnniUiSwitchBoxNext(&GeneratorPowerOnBox);
				UnniUiSwitchBoxNext(&MemorySaveLoadGenTypeSwitchBox);
		}
	
		//unblock valcoder events upd
		valcoderEventsBlock = 0;
		
		return 0;
}

GenParametersTypeDef Mode_5_GetAllParametrs(void)
{
	GenParametersTypeDef ParametrsAll;
	
	if (!menu_6_5WND.displayCanvas)
	{
			//set canvas
			displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
			displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
			//menu config init
			Menu_6_5_InitConfig();				
	}
	
	ParametrsAll.extFreq 			= UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit);
	ParametrsAll.intFreq 			= UnniUiNumberEditGetIntValue(signalGenFrequencyIntEdit);
	ParametrsAll.extDuty			= UnniUiNumberEditGetIntValue(signalGenDutyExtEdit);
	ParametrsAll.intDuty 			= UnniUiNumberEditGetIntValue(signalGenDutyIntEdit);
	ParametrsAll.phaseShift		= 0;
	ParametrsAll.modePWM			= 6;
	ParametrsAll.deadTime			= UnniUiNumberEditGetIntValue(signalGenDeadTimeEdit);
	ParametrsAll.sinShift			= UnniUiNumberEditGetFullValue(signalGenKCenterEdit);
	ParametrsAll.countdown		= 0;
	UnniUiSwitchBoxGetCurrent(&GeneratorSinusDirectionBox, (uint32_t*)&ParametrsAll.sinDirection);
	ParametrsAll.timStatus		= 1;
	ParametrsAll.Nothing			= 0;
	
	return ParametrsAll;
}

GenParametersTypeDef Mode_5_SetAllParametrs(GenParametersTypeDef ParametrsAll)
{
	
	UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, ParametrsAll.extFreq);
	UnniUiNumberEditSetValue(signalGenFrequencyIntEdit, ParametrsAll.intFreq);
	UnniUiNumberEditSetValue(signalGenDutyExtEdit, ParametrsAll.extDuty);
	UnniUiNumberEditSetValue(signalGenDutyIntEdit, ParametrsAll.intDuty);
	UnniUiNumberEditSetValue(signalGenDeadTimeEdit, ParametrsAll.deadTime);
	UnniUiNumberEditSetValue(signalGenKCenterEdit, ParametrsAll.sinShift);
	UnniUiSwitchBoxSetCurrent(&GeneratorSinusDirectionBox, (uint32_t)ParametrsAll.sinDirection, 0);
	
	if (!menu_6_5WND.displayCanvas)
	{
			//set canvas
			displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
			displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
			//menu config init
			Menu_6_5_InitConfig();				
	}
	
	return ParametrsAll;
}
//---------------------------------------------------------------------------------------------------------//

//**************************************end of Menu_6_5_SignalGenerator.c************************************//
