//*********************************************************************************************************//
//generator 407 Menu17 - signal generator
//Created 23.10.2018
//Created by Novikov Dmitry
//*********************************************************************************************************//

//*********************************************************************************************************//
/* includes */
#include "main.h"
#include "main_menu.h"
#include "gen_timers.h"

//*********************************************************************************************************//
/* defines and constants */

#define DOUBLE_COMPARE_ACCURACY 0.01

//*********************************************************************************************************//
/* Global variables ---------------------------------------------------------------------------------------*/
extern TREE_MENU* mainMenu;
extern D128X64_DISPLAY* Display128x64;
extern D128X64_BUFFER displayBuffer;
extern URG_CANVAS displayCanvas;
extern SYS_EVENTS_DATA valcoderEvents; //external valcoder events handler struct
extern uint8_t valcoderEventsBlock;    //flag - block/unblock change valcoderEvents variable
extern signalGeneratorConfig SignalGenerator;      //signal generator config

/* Private functions --------------------------------------------------------------------------------------*/
void signalUpdate(void);
void signalSinusOn(uint32_t itemNumber);
void signalTrinagleOn(uint32_t itemNumber);
void signalSquareOn(uint32_t itemNumber);
void signalPulseOn(uint32_t itemNumber);

/* Private variables --------------------------------------------------------------------------------------*/
static const uint8_t menu17WNDName[] = "Signal generator";
static MAIN_MENU_ELEMENT _menu17Elements[] = {
																								{0, 0, 0, 0}																					
																					   };
static MENU_DIALOG_WND menu17WND = {0};
static URG_FONT_CONFIG numEditFont;
static URG_FONT_CONFIG numEditStrFont;
static UNNI_UI_SCROLLLIST* menu17SignalScrollList = 0;
static UNNI_UI_SWITCH_BOX_ITEM signalGenTypeBoxItems[] = {
                                                           {(uint8_t*)"Sinus", signalSinusOn},
                                                           {(uint8_t*)"Triangle", signalTrinagleOn},
																													 {(uint8_t*)"Square", signalSquareOn},
																													 {(uint8_t*)"Pulse", signalPulseOn}
                                                      };
static UNNI_UI_SWITCHBOX signalGenTypeSwitchBox = {&displayCanvas, 100, 12, &numEditStrFont, (uint8_t*)"Signal : ", 4, 1, 0, signalGenTypeBoxItems , 1};
static uint8_t signalGenFrequencyExtEditStr[] = "Hz e";
static UNNI_UI_NUMBEREDIT* signalGenFrequencyExtEdit = 0;
static uint8_t signalGenFrequencyIntEditStr[] = "Hz i";
static UNNI_UI_NUMBEREDIT* signalGenFrequencyIntEdit = 0;
static uint8_t signalGenKPowerEditStr[] = "% , power";
static UNNI_UI_NUMBEREDIT* signalGenKPowerEdit = 0;
static uint8_t signalGenKCenterEditStr[] = "% , center";
static UNNI_UI_NUMBEREDIT* signalGenKCenterEdit = 0;
static uint8_t signalGenDeadTimeEditStr[] = "ns, dead";
static UNNI_UI_NUMBEREDIT* signalGenDeadTimeEdit = 0;
static uint8_t signalGenMinPulseEditStr[] = "ns, pulse";
static UNNI_UI_NUMBEREDIT* signalGenMinPulseEdit = 0;
static uint8_t signalGenPulseCountEditStr[] = " [P] total";
static UNNI_UI_NUMBEREDIT* signalGenPulseCountEdit = 0;
static uint8_t signalGenPulseNumEditStr[] = " [P] #";
static UNNI_UI_NUMBEREDIT* signalGenPulseNumEdit = 0;
static uint8_t signalGenPulseWidthEditStr[] = " [P] duty";
static UNNI_UI_NUMBEREDIT* signalGenPulseWidthEdit = 0;

//*********************************************************************************************************//
/* Menu #17 functions */

/* Private functions --------------------------------------------------------------------------------------*/

/* compare two double numbers */
static uint8_t twoDoubleEqual(double x1, double x2)
{
		return ( fabs(x1 - x2) < DOUBLE_COMPARE_ACCURACY );
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
void signalCalc(void)
{
		SignalGenerator.timerPrescaler = GenGetPrescalerValue(SignalGenerator.freqPWM, SignalGenerator.signalType);
		SignalGenerator.timerARR = GenGetARRValueFromFreq(SignalGenerator.timerPrescaler, SignalGenerator.freqPWM, SignalGenerator.signalType);
		SignalGenerator.timerStepsCCR = GenGetStepsCCRValueFromFreq(SignalGenerator.timerPrescaler, SignalGenerator.freqPWM, SignalGenerator.freqSignal, SignalGenerator.timerARR,
		                                                            (double)SignalGenerator.powerK / 100, SignalGenerator.pwmMinPulseLengthInNS, SignalGenerator.pwmDeadTimeInNS, SignalGenerator.signalType);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
void signalUpdate(void)
{
		GenUpdateSignal(SignalGenerator.timerPrescaler, SignalGenerator.timerARR, SignalGenerator.timerStepsCCR, (double)SignalGenerator.powerK / 100,
										(double)SignalGenerator.centerK / 100, SignalGenerator.pwmMinPulseLengthInNS, SignalGenerator.pwmDeadTimeInNS, SignalGenerator.signalType);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
void signalSinusOn(uint32_t itemNumber)
{
		SignalGenerator.signalType = SSIGNAL_SINUS;
		//calc new signal
		signalCalc();
		//update signal
		signalUpdate();
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
void signalTrinagleOn(uint32_t itemNumber)
{
		SignalGenerator.signalType = SSIGNAL_TRIANGLE;
		//calc new signal
		signalCalc();
		//update signal
		signalUpdate();
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
void signalSquareOn(uint32_t itemNumber)
{
		SignalGenerator.signalType = SSIGNAL_SQUARE;
		//calc new signal
		signalCalc();
		//update signal
		signalUpdate();
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
void signalPulseOn(uint32_t itemNumber)
{
		SignalGenerator.signalType = SSIGNAL_PULSE;
		//calc new signal
		signalCalc();
		//update signal
		signalUpdate();
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */																					 
static void Menu17_InitConfig(void)
{
		//wnd
		//wnd border
		menu17WND.wndBorder.startPoint.x = 0;
		menu17WND.wndBorder.startPoint.y = displayCanvas.screenHeight - 1;
		menu17WND.wndBorder.endPoint.x = displayCanvas.screenWidth - 1;
		menu17WND.wndBorder.endPoint.y = 0;
		menu17WND.wndBorderThickness = 1;
		//set elements
		menu17WND.menuElements = &_menu17Elements[0];
		//wnd name and font
		menu17WND.wndName = (uint8_t*)&menu17WNDName[0];
		menu17WND.wndNameFont.symbolWidth = 5;
		menu17WND.wndNameFont.symbolHeight = 8;
		menu17WND.wndNameFont.centerAlignment = 0;
		menu17WND.wndNameFont.textSpacing = 1;
		menu17WND.wndNameFont.fontRaw = (uint8_t*)&DefaultFont[0];	
		//set display canvas struct
		menu17WND.displayCanvas = &displayCanvas;
		//init config
		MenuDialogWNDInitConfig(&menu17WND);
		
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
		if (!menu17SignalScrollList)
		{
				menu17SignalScrollList = UnniUiScrollListCreate(menu17WND.wndBorder.startPoint.x + 1, menu17WND.wndBorder.startPoint.y - 1,
																											  menu17WND.wndBorder.endPoint.x - 1, menu17WND.wndBorder.startPoint.y - 11, 10, 5, 2, 5,
																											  SCROLLLIST_LEFT_ALIGNED, &displayCanvas);
		}
		//add elements to scroll list
		//add switch box - signal type
		UnniUiScrollListAddItem(menu17SignalScrollList, 100, 12, (void*)&signalGenTypeSwitchBox, UnniUiSwitchBoxUpdate);
		//create and add number edit - external frequency
		if (!signalGenFrequencyExtEdit)
		{
				signalGenFrequencyExtEdit = UnniUiNumberEditCreate(0, 0, 8, 1, NUMBEREDIT_FRACTIONAL, SSIGNAL_FREQ_EXT_MIN, SSIGNAL_FREQ_EXT_MAX, SignalGenerator.freqSignal,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenFrequencyExtEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu17SignalScrollList, menu17WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenFrequencyExtEdit, UnniUiNumberEditUpdate);
		}
		//create and add number edit - internal frequency
		if (!signalGenFrequencyIntEdit)
		{
				signalGenFrequencyIntEdit = UnniUiNumberEditCreate(0, 0, 8, 1, NUMBEREDIT_FRACTIONAL, SSIGNAL_FREQ_INT_MIN, SSIGNAL_FREQ_INT_MAX, SignalGenerator.freqPWM,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenFrequencyIntEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu17SignalScrollList, menu17WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenFrequencyIntEdit, UnniUiNumberEditUpdate);
		}
		//create and add number edit - k power
		if (!signalGenKPowerEdit)
		{
				signalGenKPowerEdit = UnniUiNumberEditCreate(0, 0, 4, 1, NUMBEREDIT_FRACTIONAL, SSIGNAL_MIN_POWER_K / 100.0, SSIGNAL_MAX_POWER_K / 100.0, SignalGenerator.powerK / 100.0,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenKPowerEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu17SignalScrollList, menu17WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenKPowerEdit, UnniUiNumberEditUpdate);
		}
		//create and add number edit - k center
		if (!signalGenKCenterEdit)
		{
				signalGenKCenterEdit = UnniUiNumberEditCreate(0, 0, 4, 1, NUMBEREDIT_FRACTIONAL | NUMBEREDIT_SIGN, SSIGNAL_MIN_CENTER_K / 100.0, SSIGNAL_MAX_CENTER_K / 100.0,
				                                              SignalGenerator.centerK / 100.0, &numEditFont, &numEditStrFont, (uint8_t*)signalGenKCenterEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu17SignalScrollList, menu17WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenKCenterEdit, UnniUiNumberEditUpdate);
		}
		//create and add number edit - dead time
		if (!signalGenDeadTimeEdit)
		{
				signalGenDeadTimeEdit = UnniUiNumberEditCreate(0, 0, 5, 0, 0, SSIGNAL_MIN_DEAD_TIME, SSIGNAL_MAX_DEAD_TIME, SignalGenerator.pwmDeadTimeInNS,
				                                               &numEditFont, &numEditStrFont, (uint8_t*)signalGenDeadTimeEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu17SignalScrollList, menu17WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenDeadTimeEdit, UnniUiNumberEditUpdate);
		}
		//create and add number edit - pulse min time
		if (!signalGenMinPulseEdit)
		{
				signalGenMinPulseEdit = UnniUiNumberEditCreate(0, 0, 5, 0, 0, SSIGNAL_MIN_PULSE_TIME, SSIGNAL_MAX_PULSE_TIME, SignalGenerator.pwmMinPulseLengthInNS,
				                                               &numEditFont, &numEditStrFont, (uint8_t*)signalGenMinPulseEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu17SignalScrollList, menu17WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenMinPulseEdit, UnniUiNumberEditUpdate);
		}
		//create and add number edit - total pulses count
		if (!signalGenPulseCountEdit)
		{
				signalGenPulseCountEdit = UnniUiNumberEditCreate(0, 0, 5, 0, 0, 2, SSIGNAL_MAX_SIGN_STEPS, SignalGenerator.timerStepsCCR,
				                                               &numEditFont, &numEditStrFont, (uint8_t*)signalGenPulseCountEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu17SignalScrollList, menu17WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenPulseCountEdit, UnniUiNumberEditUpdate);
		}
		//create and add number edit - selected pulse number
		if (!signalGenPulseNumEdit)
		{
				signalGenPulseNumEdit = UnniUiNumberEditCreate(0, 0, 5, 0, 0, 1, SSIGNAL_MAX_SIGN_STEPS, 0,
				                                               &numEditFont, &numEditStrFont, (uint8_t*)signalGenPulseNumEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu17SignalScrollList, menu17WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenPulseNumEdit, UnniUiNumberEditUpdate);
		}
		//create and add number edit - selected pulse width
		if (!signalGenPulseWidthEdit)
		{
				signalGenPulseWidthEdit = UnniUiNumberEditCreate(0, 0, 5, 0, 0, 0, 65535, 0,
				                                               &numEditFont, &numEditStrFont, (uint8_t*)signalGenPulseWidthEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu17SignalScrollList, menu17WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenPulseWidthEdit, UnniUiNumberEditUpdate);
		}
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */																					 
static void Menu17_DeleteConfig(void)
{
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t Menu17_SignalGenerator_Draw(const uint8_t frameNum)
{
		//check menu window config
		if (!menu17WND.displayCanvas)
		{
				//set canvas
				displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
				displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
				//menu config init
				Menu17_InitConfig();				
		}
		//clear display buffer
		memset(displayBuffer.raw, 0, DISPLAY_BUFFER_SIZE);
		
		//draw menu dialog
		DrawMenuDialogWND(&menu17WND);
		//draw scroll list with components
		UnniUiScrollListDraw(menu17SignalScrollList);
	
		//write buf to display
		D128X64WriteBuf(Display128x64, &displayBuffer);
		return 0;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
void updatePulseCountAndWidth(void)
{
		volatile uint32_t intVal1;
	
		//set pulse count value
		UnniUiNumberEditSetValue(signalGenPulseCountEdit, SignalGenerator.timerStepsCCR);
		//check pulse number value
		intVal1 = UnniUiNumberEditGetIntValue(signalGenPulseNumEdit);
		if ( intVal1 > SignalGenerator.timerStepsCCR )
		{
				intVal1 = SignalGenerator.timerStepsCCR;							
		} else if ( intVal1 < 1 )
		{
				intVal1 = 1;
		}
		UnniUiNumberEditSetValue(signalGenPulseNumEdit, intVal1);
		//set pulse width value
		UnniUiNumberEditSetValue(signalGenPulseWidthEdit, GenGetPulseWidthByNumber(intVal1));
}

uint8_t Menu17_SignalGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents)
{
		volatile float floatVal1, floatVal2;
		volatile uint32_t intVal2;
		
		//block valcoder events upd
		valcoderEventsBlock = 1;
	
		// event - MENU click
		if ( sysEvents & EVENT_BUTTON_MENU_CLICK )
		{
				MenuGoToItemId(mainMenu, Menu2_MainMenu_ID);
		}
		// event - UP click
		if ( sysEvents & EVENT_BUTTON_UP_CLICK )
		{
				UnniUiScrollListUp(menu17SignalScrollList);
		}
		// event - DOWN click
		if ( sysEvents & EVENT_BUTTON_DOWN_CLICK )
		{
				UnniUiScrollListDown(menu17SignalScrollList);
		}
		// event - button Left click
		if ( sysEvents & EVENT_BUTTON_LEFT_CLICK )
		{
				//switch box -> go to previous element
				UnniUiSwitchBoxPrev(&signalGenTypeSwitchBox);
				//num edits - cursor move left
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyIntEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenKPowerEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenKCenterEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDeadTimeEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenMinPulseEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenPulseCountEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenPulseNumEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenPulseWidthEdit);
		}
		// event - button Right click
		if ( sysEvents & EVENT_BUTTON_RIGHT_CLICK )
		{
				//switch box -> go to next element
				UnniUiSwitchBoxNext(&signalGenTypeSwitchBox);
				//num edits - cursor move right
				UnniUiNumberEditCursorMoveRight(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveRight(signalGenFrequencyIntEdit);
				UnniUiNumberEditCursorMoveRight(signalGenKPowerEdit);
				UnniUiNumberEditCursorMoveRight(signalGenKCenterEdit);
				UnniUiNumberEditCursorMoveRight(signalGenDeadTimeEdit);
				UnniUiNumberEditCursorMoveRight(signalGenMinPulseEdit);
				UnniUiNumberEditCursorMoveRight(signalGenPulseCountEdit);
				UnniUiNumberEditCursorMoveRight(signalGenPulseNumEdit);
				UnniUiNumberEditCursorMoveRight(signalGenPulseWidthEdit);			
		}
		// event - encoder button pressed and CCW
		if ( (sysEvents & EVENT_VALCODER_BUTTON_PRESSED) && (valcoderEvents & EVENT_VALCODER_CCW) )
		{
				valcoderEvents &= ~EVENT_VALCODER_CCW;
				//num edits - cursor move left
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyIntEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenKPowerEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenKCenterEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDeadTimeEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenMinPulseEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenPulseCountEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenPulseNumEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenPulseWidthEdit);
		}		
		// event - encoder button pressed and CW
		if ( (sysEvents & EVENT_VALCODER_BUTTON_PRESSED) && (valcoderEvents & EVENT_VALCODER_CW) )
		{
				valcoderEvents &= ~EVENT_VALCODER_CW;
				//num edits - cursor move right
				UnniUiNumberEditCursorMoveRight(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveRight(signalGenFrequencyIntEdit);
				UnniUiNumberEditCursorMoveRight(signalGenKPowerEdit);
				UnniUiNumberEditCursorMoveRight(signalGenKCenterEdit);
				UnniUiNumberEditCursorMoveRight(signalGenDeadTimeEdit);
				UnniUiNumberEditCursorMoveRight(signalGenMinPulseEdit);
				UnniUiNumberEditCursorMoveRight(signalGenPulseCountEdit);
				UnniUiNumberEditCursorMoveRight(signalGenPulseNumEdit);
				UnniUiNumberEditCursorMoveRight(signalGenPulseWidthEdit);
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

				//external frequency edit - dec
				if (UnniUiNumberEditDec(signalGenFrequencyExtEdit) == URG_OK)
				{
						//get real ext frequency
						floatVal1 = UnniUiNumberEditGetFloatValue(signalGenFrequencyExtEdit);
						floatVal2 = GetRealAvailableSignalFreq(SignalGenerator.timerPrescaler, SignalGenerator.freqPWM,
						                                       floatVal1, SignalGenerator.signalType);
						//check value
						if ( floatVal2 > 0.0 )
						{
								//compare
								if (twoDoubleEqual(floatVal2, SignalGenerator.freqSignal))
								{
										SignalGenerator.freqSignal = GetPrevAvailableSignalFreq(SignalGenerator.timerPrescaler, SignalGenerator.freqPWM,
								                                                        floatVal2, SignalGenerator.signalType);							
								} else
								{
										SignalGenerator.freqSignal = floatVal2;								
								}
						}
						//calc new signal
						signalCalc();
						//update signal
						signalUpdate();
						//set number edit value
						UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, SignalGenerator.freqSignal);
				}
				//internal frequency edit - dec
				if (UnniUiNumberEditDec(signalGenFrequencyIntEdit) == URG_OK)
				{
						//get real int frequency
						floatVal1 = UnniUiNumberEditGetFloatValue(signalGenFrequencyIntEdit);
						floatVal2 = GetRealAvailablePWMFreq(SignalGenerator.timerPrescaler, floatVal1,
						                                    SignalGenerator.freqSignal, SignalGenerator.signalType);
						//check value
						if ( floatVal2 > 0.0 )
						{
								//compare
								if (twoDoubleEqual(floatVal2, SignalGenerator.freqPWM))
								{
										SignalGenerator.freqPWM = GetPrevAvailablePWMFreq(SignalGenerator.timerPrescaler, floatVal2,
								                                                  SignalGenerator.freqSignal, SignalGenerator.signalType);							
								} else
								{
										SignalGenerator.freqPWM = floatVal2;								
								}
						}
						//calc new signal
						signalCalc();
						//update signal
						signalUpdate();
						//set number edit value
						UnniUiNumberEditSetValue(signalGenFrequencyIntEdit, SignalGenerator.freqPWM);
				}
				//power - dec
				if (UnniUiNumberEditDec(signalGenKPowerEdit) == URG_OK)
				{
						SignalGenerator.powerK = UnniUiNumberEditGetFullValue(signalGenKPowerEdit) * 10;
						//calc new signal
						signalCalc();
						//update signal
						signalUpdate();
				}
				//center - dec
				if (UnniUiNumberEditDec(signalGenKCenterEdit) == URG_OK)
				{
						SignalGenerator.centerK = UnniUiNumberEditGetFullValue(signalGenKCenterEdit) * 10;
						//calc new signal
						signalCalc();
						//update signal
						signalUpdate();
				}
				//dead time - dec
				if (UnniUiNumberEditDec(signalGenDeadTimeEdit) == URG_OK)
				{
						SignalGenerator.pwmDeadTimeInNS = UnniUiNumberEditGetIntValue(signalGenDeadTimeEdit);
						//calc new signal
						signalCalc();
						//update signal
						signalUpdate();
				}
				//min pulse time - dec
				if (UnniUiNumberEditDec(signalGenMinPulseEdit) == URG_OK)
				{
						SignalGenerator.pwmMinPulseLengthInNS = UnniUiNumberEditGetIntValue(signalGenMinPulseEdit);
						//calc new signal
						signalCalc();
						//update signal
						signalUpdate();
				}
				if ( SignalGenerator.signalType != SSIGNAL_SQUARE )
				{
						//pulse count - dec
						if (UnniUiNumberEditDec(signalGenPulseCountEdit) == URG_OK)
						{
								//check value
								intVal2 = UnniUiNumberEditGetIntValue(signalGenPulseCountEdit);
								if ( intVal2 > SSIGNAL_MAX_SIGN_STEPS )
								{
										intVal2 = SSIGNAL_MAX_SIGN_STEPS;
								} else if ( intVal2 < 2 )
								{
										intVal2 = 2;
								}
								//set checked value
								UnniUiNumberEditSetValue(signalGenPulseCountEdit, intVal2);
								//save new value
								SignalGenerator.timerStepsCCR = intVal2;
								//update signal frequency
								SignalGenerator.freqSignal = GenGetSignalFreqValueFromTimer(SignalGenerator.timerPrescaler, SignalGenerator.timerARR,
																																						SignalGenerator.timerStepsCCR, SignalGenerator.signalType);
								//update signal
								signalUpdate();
								//set number edit value
								UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, SignalGenerator.freqSignal);						
						}
						//pulse number - dec
						if (UnniUiNumberEditDec(signalGenPulseNumEdit) == URG_OK)
						{
								//check value
								intVal2 = UnniUiNumberEditGetIntValue(signalGenPulseNumEdit);
								if ( intVal2 > SignalGenerator.timerStepsCCR )
								{
										intVal2 = SignalGenerator.timerStepsCCR;
								} else if ( intVal2 < 1 )
								{
										intVal2 = 1;
								}
								//set checked value
								UnniUiNumberEditSetValue(signalGenPulseNumEdit, intVal2);						
						}
						//target pulse width - dec
						if (UnniUiNumberEditDec(signalGenPulseWidthEdit) == URG_OK)
						{
								//check value
								intVal2 = UnniUiNumberEditGetIntValue(signalGenPulseWidthEdit);
								if ( intVal2 > (SignalGenerator.timerARR - 1) )
								{
										intVal2 = SignalGenerator.timerARR - 1;
								}
								//set checked value
								UnniUiNumberEditSetValue(signalGenPulseWidthEdit, intVal2);
								//set pulse value
								GenSetPulseWidthByNumber(UnniUiNumberEditGetIntValue(signalGenPulseNumEdit), intVal2);					
						}
						//update pulse count, number, width
						updatePulseCountAndWidth();
				}
				//switch box -> go to previous element
				UnniUiSwitchBoxPrev(&signalGenTypeSwitchBox);
		}		
		// event - encoder CW
		if (valcoderEvents & EVENT_VALCODER_CW)
		{
				//clear flag manually
				valcoderEvents &= ~EVENT_VALCODER_CW;
			
				//external frequency edit - inc
				if (UnniUiNumberEditInc(signalGenFrequencyExtEdit) == URG_OK)
				{
						//get real ext frequency
						floatVal1 = UnniUiNumberEditGetFloatValue(signalGenFrequencyExtEdit);
						floatVal2 = GetRealAvailableSignalFreq(SignalGenerator.timerPrescaler, SignalGenerator.freqPWM,
						                                       floatVal1, SignalGenerator.signalType);
						//check value
						if ( floatVal2 > 0.0 )
						{
								//compare
								if (twoDoubleEqual(floatVal2, SignalGenerator.freqSignal))
								{
										SignalGenerator.freqSignal = GetNextAvailableSignalFreq(SignalGenerator.timerPrescaler, SignalGenerator.freqPWM,
								                                                        floatVal2, SignalGenerator.signalType);							
								} else
								{
										SignalGenerator.freqSignal = floatVal2;								
								}
						}
						//calc new signal
						signalCalc();
						//update signal
						signalUpdate();
						//set number edit value
						UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, SignalGenerator.freqSignal);
				}
				//internal frequency edit - inc
				if (UnniUiNumberEditInc(signalGenFrequencyIntEdit) == URG_OK)
				{
						//get real int frequency
						floatVal1 = UnniUiNumberEditGetFloatValue(signalGenFrequencyIntEdit);
						floatVal2 = GetRealAvailablePWMFreq(SignalGenerator.timerPrescaler, floatVal1,
						                                    SignalGenerator.freqSignal, SignalGenerator.signalType);
						//check value
						if ( floatVal2 > 0.0 )
						{
								//compare
								if (twoDoubleEqual(floatVal2, SignalGenerator.freqPWM))
								{
										SignalGenerator.freqPWM = GetNextAvailablePWMFreq(SignalGenerator.timerPrescaler, floatVal2,
								                                                  SignalGenerator.freqSignal, SignalGenerator.signalType);							
								} else
								{
										SignalGenerator.freqPWM = floatVal2;								
								}
						}
						//calc new signal
						signalCalc();
						//update signal
						signalUpdate();
						//set number edit value
						UnniUiNumberEditSetValue(signalGenFrequencyIntEdit, SignalGenerator.freqPWM);
				}
				//power - inc
				if (UnniUiNumberEditInc(signalGenKPowerEdit) == URG_OK)
				{
						SignalGenerator.powerK = UnniUiNumberEditGetFullValue(signalGenKPowerEdit) * 10;
						//calc new signal
						signalCalc();
						//update signal
						signalUpdate();
				}
				//center - inc
				if (UnniUiNumberEditInc(signalGenKCenterEdit) == URG_OK)
				{
						SignalGenerator.centerK = UnniUiNumberEditGetFullValue(signalGenKCenterEdit) * 10;
						//calc new signal
						signalCalc();
						//update signal
						signalUpdate();
				}
				//dead time - inc
				if (UnniUiNumberEditInc(signalGenDeadTimeEdit) == URG_OK)
				{
						SignalGenerator.pwmDeadTimeInNS = UnniUiNumberEditGetIntValue(signalGenDeadTimeEdit);
						//calc new signal
						signalCalc();
						//update signal
						signalUpdate();
				}
				//min pulse time - inc
				if (UnniUiNumberEditInc(signalGenMinPulseEdit) == URG_OK)
				{
						SignalGenerator.pwmMinPulseLengthInNS = UnniUiNumberEditGetIntValue(signalGenMinPulseEdit);
						//calc new signal
						signalCalc();
						//update signal
						signalUpdate();
				}
				if ( SignalGenerator.signalType != SSIGNAL_SQUARE )
				{
						//pulse count - inc
						if (UnniUiNumberEditInc(signalGenPulseCountEdit) == URG_OK)
						{
								//check value
								intVal2 = UnniUiNumberEditGetIntValue(signalGenPulseCountEdit);
								if ( intVal2 > SSIGNAL_MAX_SIGN_STEPS )
								{
										intVal2 = SSIGNAL_MAX_SIGN_STEPS;
								} else if ( intVal2 < 2 )
								{
										intVal2 = 2;
								}
								//set checked value
								UnniUiNumberEditSetValue(signalGenPulseCountEdit, intVal2);
								//save new value
								SignalGenerator.timerStepsCCR = intVal2;
								//update signal frequency
								SignalGenerator.freqSignal = GenGetSignalFreqValueFromTimer(SignalGenerator.timerPrescaler, SignalGenerator.timerARR,
																																						SignalGenerator.timerStepsCCR, SignalGenerator.signalType);
								//update signal
								signalUpdate();
								//set number edit value
								UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, SignalGenerator.freqSignal);						
						}
						//pulse number - inc
						if (UnniUiNumberEditInc(signalGenPulseNumEdit) == URG_OK)
						{
								//check value
								intVal2 = UnniUiNumberEditGetIntValue(signalGenPulseNumEdit);
								if ( intVal2 > SignalGenerator.timerStepsCCR )
								{
										intVal2 = SignalGenerator.timerStepsCCR;
								} else if ( intVal2 < 1 )
								{
										intVal2 = 1;
								}
								//set checked value
								UnniUiNumberEditSetValue(signalGenPulseNumEdit, intVal2);						
						}
						//target pulse width - inc
						if (UnniUiNumberEditInc(signalGenPulseWidthEdit) == URG_OK)
						{
								//check value
								intVal2 = UnniUiNumberEditGetIntValue(signalGenPulseWidthEdit);
								if ( intVal2 > (SignalGenerator.timerARR - 1) )
								{
										intVal2 = SignalGenerator.timerARR - 1;
								}
								//set checked value
								UnniUiNumberEditSetValue(signalGenPulseWidthEdit, intVal2);
								//set pulse value
								GenSetPulseWidthByNumber(UnniUiNumberEditGetIntValue(signalGenPulseNumEdit), intVal2);					
						}
						//update pulse count, number, width
						updatePulseCountAndWidth();
				}
				//switch box -> go to next element
				UnniUiSwitchBoxNext(&signalGenTypeSwitchBox);
		}
	
		//unblock valcoder events upd
		valcoderEventsBlock = 0;
		
		return 0;
}
//---------------------------------------------------------------------------------------------------------//

//**************************************end of Menu17_SignalGenerator.c************************************//
