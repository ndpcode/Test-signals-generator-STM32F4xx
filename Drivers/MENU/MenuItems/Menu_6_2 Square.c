//*********************************************************************************************************//
//generator 407 Menu_6_2 - signal generator
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

extern uint16_t _steps;
extern uint16_t _precision;
extern uint32_t _realFrequency;
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
GenParametersTypeDef Mode_2_GetAllParametrs(void);
/* Private variables --------------------------------------------------------------------------------------*/
static const uint8_t menu_6_2WNDName[] = "[2] Square";
static MAIN_MENU_ELEMENT _menu_6_2Elements[] = {
																								{0, 0, 0, 0}																					
																					   };
static MENU_DIALOG_WND menu_6_2WND = {0};
static URG_FONT_CONFIG numEditFont;
static URG_FONT_CONFIG numEditStrFont;
static UNNI_UI_SCROLLLIST* menu_6_2SignalScrollList = 0;
static UNNI_UI_SWITCH_BOX_ITEM signalGenTypeBoxItems[] = {
                                                           {(uint8_t*)"Tim 1", SelectTimer1},
                                                           {(uint8_t*)"Tim 2", SelectTimer2},
																													 {(uint8_t*)"Tim 3", SelectTimer3}
                                                      };
static UNNI_UI_SWITCHBOX signalGenTypeSwitchBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Timer : ", 3, 1, 0, signalGenTypeBoxItems};
static uint8_t signalGenFrequencyExtEditStr[] = "Hz e";
static UNNI_UI_NUMBEREDIT* signalGenFrequencyExtEdit = 0;
static uint8_t signalGenFrequencyIntEditStr[] = "Hz i";
static UNNI_UI_NUMBEREDIT* signalGenFrequencyIntEdit = 0;
static uint8_t signalGenDutyExtEditStr[] = "% , Duty e";
static UNNI_UI_NUMBEREDIT* signalGenDutyExtEdit = 0;
static uint8_t signalGenDutyIntEditStr[] = "% , Duty i";
static UNNI_UI_NUMBEREDIT* signalGenDutyIntEdit = 0;
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
#define MEMORY_SAVE_LOAD_SWITCH_BOX_Menu2 8-1
UNNI_UI_MESSAGEBOX* menu6_2_MessageBox = 0;
//*********************************************************************************************************//
/* Menu #_6_2 functions */

/* Private functions --------------------------------------------------------------------------------------*/
//---------------------------------------------------------------------------------------------------------//
/*  */																					 
static void Menu_6_2_InitConfig(void)
{
		//wnd
		//wnd border
		menu_6_2WND.wndBorder.startPoint.x = 0;
		menu_6_2WND.wndBorder.startPoint.y = displayCanvas.screenHeight - 1;
		menu_6_2WND.wndBorder.endPoint.x = displayCanvas.screenWidth - 1;
		menu_6_2WND.wndBorder.endPoint.y = 0;
		menu_6_2WND.wndBorderThickness = 1;
		//set elements
		menu_6_2WND.menuElements = &_menu_6_2Elements[0];
		//wnd name and font
		menu_6_2WND.wndName = (uint8_t*)&menu_6_2WNDName[0];
		menu_6_2WND.wndNameFont.symbolWidth = 5;
		menu_6_2WND.wndNameFont.symbolHeight = 8;
		menu_6_2WND.wndNameFont.centerAlignment = 0;
		menu_6_2WND.wndNameFont.textSpacing = 1;
		menu_6_2WND.wndNameFont.fontRaw = (uint8_t*)&DefaultFont[0];	
		//set display canvas struct
		menu_6_2WND.displayCanvas = &displayCanvas;
		//init config
		MenuDialogWNDInitConfig(&menu_6_2WND);
		
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
		
		inputConfig->modePWM = 2;
		Gen407SetConfig(genStruct, inputConfig);
		SelectTimer1();

		if (!menu_6_2SignalScrollList)
		{
				menu_6_2SignalScrollList = UnniUiScrollListCreate(menu_6_2WND.wndBorder.startPoint.x + 1, menu_6_2WND.wndBorder.startPoint.y - 1,
																											  menu_6_2WND.wndBorder.endPoint.x - 1, menu_6_2WND.wndBorder.startPoint.y - 11, 10, 5, 2, 5,
																											  SCROLLLIST_LEFT_ALIGNED, &displayCanvas);
		}

		//UnniUiScrollListAddItem(menu_6_2SignalScrollList, 100, 12, (void*)&signalGenTypeSwitchBox, UnniUiSwitchBoxUpdate);

		
		if (!signalGenFrequencyExtEdit)
		{
				signalGenFrequencyExtEdit = UnniUiNumberEditCreate(0, 0, 8, 0, 0, EXT_FREQ_MIN, EXT_FREQ_MAX, EXT_FREQ_DEFAULT,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenFrequencyExtEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_2SignalScrollList, menu_6_2WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenFrequencyExtEdit, UnniUiNumberEditUpdate);
				Gen407SetExtFreq( genStruct,  UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit));
		}
		if (!signalGenFrequencyIntEdit)
		{
				signalGenFrequencyIntEdit = UnniUiNumberEditCreate(0, 0, 8, 0, 0, INT_FREQ_MIN, INT_FREQ_MAX, INT_FREQ_DEFAULT,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenFrequencyIntEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_2SignalScrollList, menu_6_2WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenFrequencyIntEdit, UnniUiNumberEditUpdate);
				Gen407SetIntFreq( genStruct,  UnniUiNumberEditGetIntValue(signalGenFrequencyIntEdit));
		}
		if (!signalGenDutyExtEdit)
		{
				signalGenDutyExtEdit = UnniUiNumberEditCreate(0, 0, 4, 1, NUMBEREDIT_FRACTIONAL, EXT_DUTY_CYCLE_MIN/10, EXT_DUTY_CYCLE_MAX/10, EXT_DUTY_CYCLE_DEFAULT,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenDutyExtEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_2SignalScrollList, menu_6_2WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenDutyExtEdit, UnniUiNumberEditUpdate);
				UnniUiNumberEditSetFullValue(signalGenDutyExtEdit,EXT_DUTY_CYCLE_DEFAULT);
				Gen407SetExtDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyExtEdit) );
		}
		if (!signalGenDutyIntEdit)
		{
				signalGenDutyIntEdit = UnniUiNumberEditCreate(0, 0, 4, 1, NUMBEREDIT_FRACTIONAL, INT_DUTY_CYCLE_MIN/10, INT_DUTY_CYCLE_MAX/10, INT_DUTY_CYCLE_DEFAULT,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenDutyIntEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_2SignalScrollList, menu_6_2WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenDutyIntEdit, UnniUiNumberEditUpdate);
				UnniUiNumberEditSetFullValue(signalGenDutyIntEdit,INT_DUTY_CYCLE_DEFAULT);
				Gen407SetIntDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyIntEdit) );
		}
		if (!signalGenDeadTimeEdit)
		{
				signalGenDeadTimeEdit = UnniUiNumberEditCreate(0, 0, 7, 0, 0, DEADTIME_MIN, DEADTIME_MAX, DEADTIME_DEFAULT,
				                                               &numEditFont, &numEditStrFont, (uint8_t*)signalGenDeadTimeEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_2SignalScrollList, menu_6_2WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenDeadTimeEdit, UnniUiNumberEditUpdate);
				Gen407SetDeadTime( genStruct, UnniUiNumberEditGetIntValue(signalGenDeadTimeEdit) );
		}

		UnniUiScrollListAddItem(menu_6_2SignalScrollList, 100, 12, (void*)&GeneratorPowerOnBox, UnniUiSwitchBoxUpdate);
		
		UnniUiScrollListAddItem(menu_6_2SignalScrollList, 100, 12, (void*)&MemorySaveLoadGenTypeSwitchBox, UnniUiSwitchBoxUpdate);
		if (!menu6_2_MessageBox)
		{
				menu6_2_MessageBox = UnniUiMessageBoxInit(10, 56, 108, 46, &numEditStrFont, &numEditStrFont, menu_6_2WND.displayCanvas);
		}
}
																				 
static void Menu_6_2_DeleteConfig(void)
{
}

uint8_t Menu_6_2_PatternGenerator_Draw(const uint8_t frameNum)
{
		//check menu window config
		if (!menu_6_2WND.displayCanvas)
		{
				//set canvas
				displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
				displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
				//menu config init
				Menu_6_2_InitConfig();				
		}
		//clear display buffer
		memset(displayBuffer.raw, 0, DISPLAY_BUFFER_SIZE);
		
		//draw menu dialog
		DrawMenuDialogWND(&menu_6_2WND);
		//draw scroll list with components
		UnniUiScrollListDraw(menu_6_2SignalScrollList);
		//draw message box
		UnniUiMessageBoxDraw(menu6_2_MessageBox);
	
		//write buf to display
		D128X64WriteBuf(Display128x64, &displayBuffer);
		return 0;
}

uint8_t Menu_6_2_PatternGenerator_InputInit(void)
{
		inputConfig->modePWM = 2;
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

uint8_t Menu_6_2_PatternGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents)
{
		volatile int32_t floatVal1,  floatVal3, floatVal4, floatVal6=0;
		volatile int32_t floatVal2, floatVal5;
		uint32_t last_Duty, now_Duty;
	
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
				UnniUiScrollListUp(menu_6_2SignalScrollList);
		}
		// event - DOWN click
		if ( sysEvents & EVENT_BUTTON_DOWN_CLICK )
		{
				UnniUiScrollListDown(menu_6_2SignalScrollList);
		}
		// event - button Left click
		if ( sysEvents & EVENT_BUTTON_LEFT_CLICK )
		{
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyIntEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDutyExtEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDutyIntEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDeadTimeEdit);
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
				UnniUiNumberEditCursorMoveRight(signalGenDeadTimeEdit);
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
				UnniUiNumberEditCursorMoveRight(signalGenDeadTimeEdit);

		}
		// event - OK click or encoder button click
		if ( (sysEvents & EVENT_BUTTON_OK_CLICK) || (sysEvents & EVENT_VALCODER_BUTTON_CLICK) )
		{
				if (menu_6_2SignalScrollList->currentElementNumber == MEMORY_SAVE_LOAD_SWITCH_BOX_Menu2)
				{
						switch (MemorySaveLoadGenTypeSwitchBox.currentElementNumber)
						{
								case 1:
										MemorySaveParameters();
										Menu_6_2_PatternGenerator_InputInit();
										UnniUiMessageBoxShow(menu6_2_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Save OK", 40);	
								break;
								case 2:
										MemoryLoadParameters();
										Menu_6_2_PatternGenerator_InputInit();
										UnniUiMessageBoxShow(menu6_2_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Load OK", 40);	
								break;
								case 3:
										MemoryLoadDefaults();
										Menu_6_2_PatternGenerator_InputInit();
										UnniUiMessageBoxShow(menu6_2_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Load Def", 40);	
								break;
						}					
				}
			
		}		
		// event - encoder CCW
		if (valcoderEvents & EVENT_VALCODER_CCW)
		{
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
						genStruct->currentConfig.intFreq = 0;
						Gen407Update(genStruct);
						UnniUiNumberEditSetFullValue(signalGenDutyExtEdit, Gen407_GetRealDuty(UnniUiNumberEditGetFullValue(signalGenDutyExtEdit), _steps/2));
						Gen407SetExtDutyCycle( genStruct,  (Gen407_GetRealDuty(UnniUiNumberEditGetFullValue(signalGenDutyExtEdit), _steps/2)));
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
							UnniUiNumberEditSetFullValue(signalGenDutyExtEdit, Gen407_GetRealDuty(UnniUiNumberEditGetFullValue(signalGenDutyExtEdit), _steps/2));							
						}
				}
				last_Duty = UnniUiNumberEditGetFullValue(signalGenDutyExtEdit);
				if (UnniUiNumberEditDec(signalGenDutyExtEdit) == URG_OK)
				{
					if(genStruct->generatorStartFlag != 1)
					{
						StepsAndPrecisionValue		(genStruct->currentConfig.extFreq, genStruct->currentConfig.intFreq, genStruct->currentConfig.modePWM, genStruct->currentConfig.countdown, &_steps, &_precision);
					}
					now_Duty = UnniUiNumberEditGetFullValue(signalGenDutyExtEdit);
					if (((uint32_t)Gen407_GetRealDuty(now_Duty, _steps/2) == (uint32_t)Gen407_GetRealDuty(last_Duty, _steps/2)))
					{
							Gen407SetExtDutyCycle( genStruct,  (Gen407_GetPrevDuty(last_Duty, _steps/2)));	
							UnniUiNumberEditSetFullValue(signalGenDutyExtEdit, Gen407_GetPrevDuty(last_Duty, _steps/2));									
					} 
					else
					{					
							Gen407SetExtDutyCycle( genStruct,  (Gen407_GetRealDuty(now_Duty, _steps/2)));			
							UnniUiNumberEditSetFullValue(signalGenDutyExtEdit, Gen407_GetRealDuty(now_Duty, _steps/2));								
					}
					
						//Gen407SetExtDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyExtEdit) );
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
				UnniUiSwitchBoxPrev(&signalGenTypeSwitchBox);
				UnniUiSwitchBoxPrev(&GeneratorPowerOnBox);
				UnniUiSwitchBoxPrev(&MemorySaveLoadGenTypeSwitchBox);
		}		
		// event - encoder CW
		if (valcoderEvents & EVENT_VALCODER_CW)
		{
				valcoderEvents &= ~EVENT_VALCODER_CW;
			
				floatVal5 = UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit);
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
						UnniUiNumberEditSetFullValue(signalGenDutyExtEdit, Gen407_GetRealDuty(UnniUiNumberEditGetFullValue(signalGenDutyExtEdit), _steps/2));
						Gen407SetExtDutyCycle( genStruct,  (Gen407_GetRealDuty(UnniUiNumberEditGetFullValue(signalGenDutyExtEdit), _steps/2)));
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
							UnniUiNumberEditSetFullValue(signalGenDutyExtEdit, Gen407_GetRealDuty(UnniUiNumberEditGetFullValue(signalGenDutyExtEdit), _steps/2));
						}				
				}
				last_Duty = UnniUiNumberEditGetFullValue(signalGenDutyExtEdit);
				if (UnniUiNumberEditInc(signalGenDutyExtEdit) == URG_OK)
				{
					if(genStruct->generatorStartFlag != 1)
					{
						StepsAndPrecisionValue		(genStruct->currentConfig.extFreq, genStruct->currentConfig.intFreq, genStruct->currentConfig.modePWM, genStruct->currentConfig.countdown, &_steps, &_precision);
					}
					now_Duty = UnniUiNumberEditGetFullValue(signalGenDutyExtEdit);
					if (((uint32_t)Gen407_GetRealDuty(now_Duty, _steps/2) == (uint32_t)Gen407_GetRealDuty(last_Duty, _steps/2)))
					{
							Gen407SetExtDutyCycle( genStruct,  (Gen407_GetNextDuty(last_Duty, _steps/2)));	
							UnniUiNumberEditSetFullValue(signalGenDutyExtEdit, Gen407_GetNextDuty(last_Duty, _steps/2));									
					} 
					else
					{
							Gen407SetExtDutyCycle( genStruct,  (Gen407_GetRealDuty(now_Duty, _steps/2)));			
							UnniUiNumberEditSetFullValue(signalGenDutyExtEdit, Gen407_GetRealDuty(now_Duty, _steps/2));								
					}
						//Gen407SetExtDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyExtEdit) );
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
				UnniUiSwitchBoxNext(&signalGenTypeSwitchBox);
				UnniUiSwitchBoxNext(&GeneratorPowerOnBox);
				UnniUiSwitchBoxNext(&MemorySaveLoadGenTypeSwitchBox);
		}
	
		//unblock valcoder events upd
		valcoderEventsBlock = 0;
		
		return 0;
}

GenParametersTypeDef Mode_2_GetAllParametrs(void)
{
	GenParametersTypeDef ParametrsAll;
	
	if (!menu_6_2WND.displayCanvas)
	{
			//set canvas
			displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
			displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
			//menu config init
			Menu_6_2_InitConfig();				
	}
	
	ParametrsAll.extFreq 			= UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit);
	ParametrsAll.intFreq 			= UnniUiNumberEditGetIntValue(signalGenFrequencyIntEdit);
	ParametrsAll.extDuty			= UnniUiNumberEditGetIntValue(signalGenDutyExtEdit);
	ParametrsAll.intDuty 			= UnniUiNumberEditGetIntValue(signalGenDutyIntEdit);
	ParametrsAll.phaseShift		= 0;
	ParametrsAll.modePWM			= 2;
	ParametrsAll.deadTime			= UnniUiNumberEditGetIntValue(signalGenDeadTimeEdit);
	ParametrsAll.sinShift			= 0;
	ParametrsAll.countdown		= 0;
	ParametrsAll.sinDirection	= 0;
	ParametrsAll.timStatus		= 1;
	ParametrsAll.Nothing			= 0;
	
	return ParametrsAll;
}

GenParametersTypeDef Mode_2_SetAllParametrs(GenParametersTypeDef ParametrsAll)
{
	
	UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, ParametrsAll.extFreq);
	UnniUiNumberEditSetValue(signalGenFrequencyIntEdit, ParametrsAll.intFreq);
	UnniUiNumberEditSetValue(signalGenDutyExtEdit, ParametrsAll.extDuty);
	UnniUiNumberEditSetValue(signalGenDutyIntEdit, ParametrsAll.intDuty);
	UnniUiNumberEditSetValue(signalGenDeadTimeEdit, ParametrsAll.deadTime);
		
	if (!menu_6_2WND.displayCanvas)
	{
			//set canvas
			displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
			displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
			//menu config init
			Menu_6_2_InitConfig();				
	}
	
	return ParametrsAll;
}
//---------------------------------------------------------------------------------------------------------//

//**************************************end of Menu_6_2_SignalGenerator.c************************************//
