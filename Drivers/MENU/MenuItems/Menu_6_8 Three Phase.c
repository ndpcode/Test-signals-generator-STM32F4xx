//*********************************************************************************************************//
//generator 407 Menu_6_8 - signal generator
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
extern uint16_t HzInSec_SpdFrq;
extern uint8_t  SmoothFlag;

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
void SmoothOff(void);
void SmoothOn(void);
/* Private variables --------------------------------------------------------------------------------------*/
static const uint8_t menu_6_8WNDName[] = "Three Phase";
static MAIN_MENU_ELEMENT _menu_6_8Elements[] = {
																								{0, 0, 0, 0}																					
																					   };
static MENU_DIALOG_WND menu_6_8WND = {0};
static URG_FONT_CONFIG numEditFont;
static URG_FONT_CONFIG numEditStrFont;
static UNNI_UI_SCROLLLIST* menu_6_8SignalScrollList = 0;
static uint8_t signalGenFrequencyExtEditStr[] = "Hz e";
static UNNI_UI_NUMBEREDIT* signalGenFrequencyExtEdit = 0;
static uint8_t signalGenFrequencyIntEditStr[] = "Hz i";
static UNNI_UI_NUMBEREDIT* signalGenFrequencyIntEdit = 0;
static uint8_t synth1PhaseEditStr[] = "deg";
static UNNI_UI_NUMBEREDIT* synth1PhaseEdit = 0;
static uint8_t signalGenDeadTimeEditStr[] = "ns, dead";
static UNNI_UI_NUMBEREDIT* signalGenDeadTimeEdit = 0;

static uint8_t Hz_In_Sec_EditStr[] = "Hz in Sec";
static UNNI_UI_NUMBEREDIT* Hz_In_Sec_Edit = 0;

static UNNI_UI_SWITCH_BOX_ITEM generatorSmoothOnBoxItems[] = {
                                                           {(uint8_t*)"Off", SmoothOff},
                                                           {(uint8_t*)"On",	 SmoothOn}
                                                         };
static UNNI_UI_SWITCHBOX GeneratorSmoothOnBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Smooth : ", 2, 1, 0, generatorSmoothOnBoxItems ,1};
static UNNI_UI_SWITCH_BOX_ITEM generatorPowerOnBoxItems[] = {
                                                           {(uint8_t*)"Off", GeneratorOff},
                                                           {(uint8_t*)"On",	 GeneratorOn}
                                                         };
static UNNI_UI_SWITCHBOX GeneratorPowerOnBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Power : ", 2, 1, 0, generatorPowerOnBoxItems ,1};
static UNNI_UI_SWITCH_BOX_ITEM MemorySaveLoadGenTypeBoxItems[] = {
                                                           {(uint8_t*)"save", 		0},
																													 {(uint8_t*)"load",  		0},
																													 {(uint8_t*)"default",  0}
                                                      };
static UNNI_UI_SWITCHBOX MemorySaveLoadGenTypeSwitchBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Memory: ", 3, 1, 0, MemorySaveLoadGenTypeBoxItems};
#define MEMORY_SAVE_LOAD_SWITCH_BOX_Menu8 8
UNNI_UI_MESSAGEBOX* menu6_8_MessageBox = 0;
//*********************************************************************************************************//
/* Menu #_6_8 functions */

/* Private functions --------------------------------------------------------------------------------------*/
void SmoothOff(void)
{
	SmoothFlag = 0;
}

void SmoothOn(void)
{
	SmoothFlag = 1;
}
//---------------------------------------------------------------------------------------------------------//
/*  */																					 
static void Menu_6_8_InitConfig(void)
{
		//wnd
		//wnd border
		menu_6_8WND.wndBorder.startPoint.x = 0;
		menu_6_8WND.wndBorder.startPoint.y = displayCanvas.screenHeight - 1;
		menu_6_8WND.wndBorder.endPoint.x = displayCanvas.screenWidth - 1;
		menu_6_8WND.wndBorder.endPoint.y = 0;
		menu_6_8WND.wndBorderThickness = 1;
		//set elements
		menu_6_8WND.menuElements = &_menu_6_8Elements[0];
		//wnd name and font
		menu_6_8WND.wndName = (uint8_t*)&menu_6_8WNDName[0];
		menu_6_8WND.wndNameFont.symbolWidth = 5;
		menu_6_8WND.wndNameFont.symbolHeight = 8;
		menu_6_8WND.wndNameFont.centerAlignment = 0;
		menu_6_8WND.wndNameFont.textSpacing = 1;
		menu_6_8WND.wndNameFont.fontRaw = (uint8_t*)&DefaultFont[0];	
		//set display canvas struct
		menu_6_8WND.displayCanvas = &displayCanvas;
		//init config
		MenuDialogWNDInitConfig(&menu_6_8WND);
		
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
		
		inputConfig->modePWM = 9;
		Gen407SetConfig(genStruct, inputConfig);
		SelectTimer1();
		
		if (!menu_6_8SignalScrollList)
		{
				menu_6_8SignalScrollList = UnniUiScrollListCreate(menu_6_8WND.wndBorder.startPoint.x + 1, menu_6_8WND.wndBorder.startPoint.y - 1,
																											  menu_6_8WND.wndBorder.endPoint.x - 1, menu_6_8WND.wndBorder.startPoint.y - 11, 10, 5, 2, 5,
																											  SCROLLLIST_LEFT_ALIGNED, &displayCanvas);
		}
		
		if (!signalGenFrequencyExtEdit)
		{
				signalGenFrequencyExtEdit = UnniUiNumberEditCreate(0, 0, 8, 0, 0, EXT_FREQ_MIN, EXT_FREQ_MAX, EXT_FREQ_DEFAULT,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenFrequencyExtEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_8SignalScrollList, menu_6_8WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenFrequencyExtEdit, UnniUiNumberEditUpdate);
				Gen407SetExtFreq( genStruct,  UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit));	
		}
		if (!signalGenFrequencyIntEdit)
		{
				signalGenFrequencyIntEdit = UnniUiNumberEditCreate(0, 0, 8, 0, 0, INT_FREQ_MIN, INT_FREQ_MAX, INT_FREQ_DEFAULT,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenFrequencyIntEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_8SignalScrollList, menu_6_8WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenFrequencyIntEdit, UnniUiNumberEditUpdate);
				Gen407SetIntFreq( genStruct,  UnniUiNumberEditGetIntValue(signalGenFrequencyIntEdit));
		}	
		if (!synth1PhaseEdit)
		{
				synth1PhaseEdit = UnniUiNumberEditCreate(0, 0, 4, 0, 0, PHASESHIFT_MIN, PHASESHIFT_MAX, PHASESHIFT_DEFAULT,
				                                         &numEditFont, &numEditStrFont, (uint8_t*)synth1PhaseEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_8SignalScrollList, menu_6_8WND.wndBorder.endPoint.x - 30, 13, (void*)synth1PhaseEdit, UnniUiNumberEditUpdate);
				Gen407SetPhaseShift( genStruct, UnniUiNumberEditGetIntValue(synth1PhaseEdit) );
		}
		if (!signalGenDeadTimeEdit)
		{
				signalGenDeadTimeEdit = UnniUiNumberEditCreate(0, 0, 5, 0, 0, DEADTIME_MIN, DEADTIME_MAX, DEADTIME_DEFAULT+100,
				                                               &numEditFont, &numEditStrFont, (uint8_t*)signalGenDeadTimeEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_8SignalScrollList, menu_6_8WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenDeadTimeEdit, UnniUiNumberEditUpdate);
				Gen407SetDeadTime( genStruct, UnniUiNumberEditGetIntValue(signalGenDeadTimeEdit) );	
				UnniUiNumberEditSetValue(signalGenDeadTimeEdit, convert_pt_in_ns(genStruct->currentConfig.deadTime) );

		}
		if (!Hz_In_Sec_Edit)
		{
				Hz_In_Sec_Edit = UnniUiNumberEditCreate(0, 0, 3, 0, 0, 1, 25, 5,
				                                               &numEditFont, &numEditStrFont, (uint8_t*)Hz_In_Sec_EditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_8SignalScrollList, menu_6_8WND.wndBorder.endPoint.x - 30, 13, (void*)Hz_In_Sec_Edit, UnniUiNumberEditUpdate);
				HzInSec_SpdFrq = UnniUiNumberEditGetFullValue(Hz_In_Sec_Edit);
		}
		UnniUiScrollListAddItem(menu_6_8SignalScrollList, 100, 12, (void*)&GeneratorSmoothOnBox, UnniUiSwitchBoxUpdate);
		UnniUiScrollListAddItem(menu_6_8SignalScrollList, 100, 12, (void*)&GeneratorPowerOnBox, UnniUiSwitchBoxUpdate);
		UnniUiScrollListAddItem(menu_6_8SignalScrollList, 100, 12, (void*)&MemorySaveLoadGenTypeSwitchBox, UnniUiSwitchBoxUpdate);
		if (!menu6_8_MessageBox)
		{
				menu6_8_MessageBox = UnniUiMessageBoxInit(10, 56, 108, 46, &numEditStrFont, &numEditStrFont, menu_6_8WND.displayCanvas);
		}
		
}
																				 
static void Menu_6_8_DeleteConfig(void)
{
}

uint8_t Menu_6_8_PatternGenerator_Draw(const uint8_t frameNum)
{
		//check menu window config
		if (!menu_6_8WND.displayCanvas)
		{
				//set canvas
				displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
				displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
				//menu config init
				Menu_6_8_InitConfig();				
		}
		//clear display buffer
		memset(displayBuffer.raw, 0, DISPLAY_BUFFER_SIZE);
		
		//draw menu dialog
		DrawMenuDialogWND(&menu_6_8WND);
		//draw scroll list with components
		UnniUiScrollListDraw(menu_6_8SignalScrollList);
		//draw message box
		UnniUiMessageBoxDraw(menu6_8_MessageBox);
	
		//write buf to display
		D128X64WriteBuf(Display128x64, &displayBuffer);
		return 0;
}

uint8_t Menu_6_8_PatternGenerator_InputInit(void)
{
		inputConfig->modePWM = 9;
		Gen407SetConfig(genStruct, inputConfig);
		SelectTimer1();
		
		Gen407SetExtFreq( genStruct,  UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit));		
		Gen407SetIntFreq( genStruct,  UnniUiNumberEditGetIntValue(signalGenFrequencyIntEdit));
		Gen407SetPhaseShift( genStruct, UnniUiNumberEditGetIntValue(synth1PhaseEdit) );
	
		Gen407SetDeadTime( genStruct, convert_ns_in_pt(UnniUiNumberEditGetFullValue(signalGenDeadTimeEdit)) );	
		
		HzInSec_SpdFrq = UnniUiNumberEditGetFullValue(Hz_In_Sec_Edit);
		Gen407SetExtDutyCycle	( genStruct, 1000 );
	
		return 0;
}

uint8_t Menu_6_8_PatternGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents)
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
				UnniUiScrollListUp(menu_6_8SignalScrollList);
		}
		// event - DOWN click
		if ( sysEvents & EVENT_BUTTON_DOWN_CLICK )
		{
				UnniUiScrollListDown(menu_6_8SignalScrollList);
		}
		// event - button Left click
		if ( sysEvents & EVENT_BUTTON_LEFT_CLICK )
		{
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyExtEdit);
		  	UnniUiNumberEditCursorMoveLeft(signalGenFrequencyIntEdit);
				UnniUiNumberEditCursorMoveLeft(synth1PhaseEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDeadTimeEdit);
				UnniUiNumberEditCursorMoveLeft(Hz_In_Sec_Edit);
				UnniUiSwitchBoxPrev(&GeneratorSmoothOnBox);
				UnniUiSwitchBoxPrev(&GeneratorPowerOnBox);
				UnniUiSwitchBoxPrev(&MemorySaveLoadGenTypeSwitchBox);
		}
		// event - button Right click
		if ( sysEvents & EVENT_BUTTON_RIGHT_CLICK )
		{
				UnniUiNumberEditCursorMoveRight(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveRight(signalGenFrequencyIntEdit);
				UnniUiNumberEditCursorMoveRight(synth1PhaseEdit);
				UnniUiNumberEditCursorMoveRight(signalGenDeadTimeEdit);
				UnniUiNumberEditCursorMoveRight(Hz_In_Sec_Edit);
				UnniUiSwitchBoxNext(&GeneratorSmoothOnBox);
				UnniUiSwitchBoxNext(&GeneratorPowerOnBox);
				UnniUiSwitchBoxNext(&MemorySaveLoadGenTypeSwitchBox);
		}
		// event - encoder button pressed and CCW
		if ( (sysEvents & EVENT_VALCODER_BUTTON_PRESSED) && (valcoderEvents & EVENT_VALCODER_CCW) )
		{
				valcoderEvents &= ~EVENT_VALCODER_CCW;
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyIntEdit);
				UnniUiNumberEditCursorMoveLeft(synth1PhaseEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDeadTimeEdit);

		}		
		// event - encoder button pressed and CW
		if ( (sysEvents & EVENT_VALCODER_BUTTON_PRESSED) && (valcoderEvents & EVENT_VALCODER_CW) )
		{
				valcoderEvents &= ~EVENT_VALCODER_CW;
				UnniUiNumberEditCursorMoveRight(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveRight(signalGenFrequencyIntEdit);
				UnniUiNumberEditCursorMoveRight(synth1PhaseEdit);
				UnniUiNumberEditCursorMoveRight(signalGenDeadTimeEdit);
		}
		// event - OK click or encoder button click
		if ( (sysEvents & EVENT_BUTTON_OK_CLICK) || (sysEvents & EVENT_VALCODER_BUTTON_CLICK) )
		{
				if (menu_6_8SignalScrollList->currentElementNumber == MEMORY_SAVE_LOAD_SWITCH_BOX_Menu8)
				{
						switch (MemorySaveLoadGenTypeSwitchBox.currentElementNumber)
						{
								case 1:
										MemorySaveParameters();
										Menu_6_8_PatternGenerator_InputInit();
//										if (!menu_6_8WND.displayCanvas)
//										{
//												displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
//												displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
//												Menu_6_8_InitConfig();				
//										}
										UnniUiMessageBoxShow(menu6_8_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Save OK", 40);	
								break;
								case 2:
										MemoryLoadParameters();
										Menu_6_8_PatternGenerator_InputInit();
//										if (!menu_6_8WND.displayCanvas)
//										{
//												displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
//												displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
//												Menu_6_8_InitConfig();				
//										}
										UnniUiMessageBoxShow(menu6_8_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Load OK", 40);	
								break;
								case 3:
										MemoryLoadDefaults();
										Menu_6_8_PatternGenerator_InputInit();
										UnniUiMessageBoxShow(menu6_8_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Load Def", 40);	
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
				
				uint16_t LastValue_FrequencyInt = UnniUiNumberEditGetIntValue(signalGenFrequencyIntEdit);
				if (UnniUiNumberEditDec(signalGenFrequencyIntEdit) == URG_OK)
				{
						if(UnniUiNumberEditGetIntValue(signalGenFrequencyIntEdit)!=Gen407_GetPrevInternalFrequency(UnniUiNumberEditGetIntValue(signalGenFrequencyIntEdit), genStruct->currentConfig.extFreq))
						{
							Gen407SetIntFreq( genStruct, Gen407_GetPrevInternalFrequency(UnniUiNumberEditGetIntValue(signalGenFrequencyIntEdit), genStruct->currentConfig.extFreq) );
							UnniUiNumberEditSetValue(signalGenFrequencyIntEdit, Gen407_GetPrevInternalFrequency(UnniUiNumberEditGetIntValue(signalGenFrequencyIntEdit), genStruct->currentConfig.extFreq));
						}else
						{
							Gen407SetIntFreq( genStruct, LastValue_FrequencyInt );
							UnniUiNumberEditSetValue(signalGenFrequencyIntEdit, LastValue_FrequencyInt );									
						}
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}
				}
				if (UnniUiNumberEditDec(synth1PhaseEdit) == URG_OK)
				{
						Gen407SetPhaseShift( genStruct, UnniUiNumberEditGetFullValue(synth1PhaseEdit) );
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
						UnniUiNumberEditSetValue(signalGenDeadTimeEdit, convert_pt_in_ns(genStruct->currentConfig.deadTime) );
				}
				if (UnniUiNumberEditDec(Hz_In_Sec_Edit) == URG_OK)
				{
						HzInSec_SpdFrq = UnniUiNumberEditGetFullValue(Hz_In_Sec_Edit);
				}
				UnniUiSwitchBoxPrev(&GeneratorSmoothOnBox);
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
				if (UnniUiNumberEditInc(synth1PhaseEdit) == URG_OK)
				{
						Gen407SetPhaseShift( genStruct, UnniUiNumberEditGetFullValue(synth1PhaseEdit) );
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}		
				}
				if (UnniUiNumberEditInc(signalGenDeadTimeEdit) == URG_OK)
				{
						genStruct->currentConfig.deadTime++;
						if (genStruct->currentConfig.deadTime>=256)
						{
							genStruct->currentConfig.deadTime--;
						}	
						if(genStruct->generatorStartFlag == 1)
						{
							Gen407Update(genStruct);
						}
						UnniUiNumberEditSetValue(signalGenDeadTimeEdit, convert_pt_in_ns(genStruct->currentConfig.deadTime) );
				}
				if (UnniUiNumberEditInc(Hz_In_Sec_Edit) == URG_OK)
				{
						HzInSec_SpdFrq = UnniUiNumberEditGetFullValue(Hz_In_Sec_Edit);
				}
				UnniUiSwitchBoxNext(&GeneratorSmoothOnBox);
				UnniUiSwitchBoxNext(&GeneratorPowerOnBox);
				UnniUiSwitchBoxNext(&MemorySaveLoadGenTypeSwitchBox);
		}
	
		//unblock valcoder events upd
		valcoderEventsBlock = 0;
		
		return 0;
}

GenParametersTypeDef Mode_8_GetAllParametrs(void)
{
	GenParametersTypeDef ParametrsAll;
	
	if (!menu_6_8WND.displayCanvas)
	{
			displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
			displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
			Menu_6_8_InitConfig();				
	}
	
	ParametrsAll.extFreq 			= UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit);
	ParametrsAll.intFreq 			= UnniUiNumberEditGetIntValue(signalGenFrequencyIntEdit);
	ParametrsAll.extDuty			= 1000;
	ParametrsAll.intDuty 			= 0;
	ParametrsAll.phaseShift		= UnniUiNumberEditGetFullValue(synth1PhaseEdit);
	ParametrsAll.modePWM			= 9;
	ParametrsAll.deadTime			= convert_ns_in_pt(UnniUiNumberEditGetFullValue(signalGenDeadTimeEdit));
	
	
	ParametrsAll.sinShift			= 0;
	ParametrsAll.countdown		= 0;
	ParametrsAll.sinDirection	= 0;
	ParametrsAll.timStatus		= 1;
	ParametrsAll.Nothing			= 0;
	
	return ParametrsAll;
}

GenParametersTypeDef Mode_8_SetAllParametrs(GenParametersTypeDef ParametrsAll)
{
		if (!menu_6_8WND.displayCanvas)
	{
			displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
			displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
			Menu_6_8_InitConfig();				
	}
	
	UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, ParametrsAll.extFreq);
	UnniUiNumberEditSetValue(signalGenFrequencyIntEdit, ParametrsAll.intFreq);
	UnniUiNumberEditSetValue(synth1PhaseEdit, ParametrsAll.phaseShift);
	
	Gen407SetDeadTime(genStruct, ParametrsAll.deadTime);
	UnniUiNumberEditSetValue(signalGenDeadTimeEdit, convert_pt_in_ns(ParametrsAll.deadTime) );
	
	Gen407SetExtDutyCycle	( genStruct, 1000 );
	

	
	return ParametrsAll;
}
//---------------------------------------------------------------------------------------------------------//

//**************************************end of Menu_6_8_SignalGenerator.c************************************//
