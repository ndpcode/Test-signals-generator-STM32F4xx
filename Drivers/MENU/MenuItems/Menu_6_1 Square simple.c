//*********************************************************************************************************//
//generator 407 Menu_6_1 - signal generator
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
extern GenParametersTypeDefForFlash MemoryFullParameters;

extern uint16_t _steps;
extern uint16_t _precision;
extern uint32_t _realFrequency;
extern uint32_t _dead_time_ns;

/* Private functions --------------------------------------------------------------------------------------*/
void SelectTimer(uint8_t Timer);
void SelectTimer1(void);
void SelectTimer2(void);
void SelectTimer3(void);
void GeneratorOn(void);
void GeneratorOff(void);
void MemorySaveParameters(void);
void MemoryLoadParameters(void);
void MemoryLoadDefaults(void);
void MemoryUnuse(void);
GenParametersTypeDef Mode_1_GetAllParametrs(void);
/* Private variables --------------------------------------------------------------------------------------*/
static const uint8_t menu_6_1WNDName[] = "[1] Square Simple";
static MAIN_MENU_ELEMENT _menu_6_1Elements[] = {
																								{0, 0, 0, 0}																					
																					   };
static MENU_DIALOG_WND menu_6_1WND = {0};
static URG_FONT_CONFIG numEditFont;
static URG_FONT_CONFIG numEditStrFont;
static UNNI_UI_SCROLLLIST* menu_6_1SignalScrollList = 0;
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
#define MEMORY_SAVE_LOAD_SWITCH_BOX_Menu1 6-1
UNNI_UI_MESSAGEBOX* menu6_1_MessageBox = 0;
//*********************************************************************************************************//
/* Menu #_6_1 functions */

/* Private functions --------------------------------------------------------------------------------------*/
void SelectTimer(uint8_t Timer)
{
	if(Timer==1)
	{
			inputConfig->timStatus = 1;
	}
	else if(Timer==2)
	{
			inputConfig->timStatus = 2;
	}
	else if(Timer==3)
	{
			inputConfig->timStatus = 4;
	}
	Gen407SetConfig(genStruct, inputConfig);
}
void SelectTimer1(void)
{
	inputConfig->timStatus = 1;
	Gen407SetConfig(genStruct, inputConfig);
}
void SelectTimer2(void)
{
	inputConfig->timStatus = 2;
	Gen407SetConfig(genStruct, inputConfig);
}
void SelectTimer3(void)
{
	inputConfig->timStatus = 4;
	Gen407SetConfig(genStruct, inputConfig);
}
void GeneratorOn(void)
{
	Gen407Start(genStruct);
}
void GeneratorOff(void)
{
	Gen407Stop(genStruct);
}

void MemorySaveParameters(void)
{
	MemoryFullParameters.mode1_All  = Mode_1_GetAllParametrs();
	MemoryFullParameters.mode2_All  = Mode_2_GetAllParametrs();
	MemoryFullParameters.mode3_All  = Mode_3_GetAllParametrs();
	MemoryFullParameters.mode4_All  = Mode_4_GetAllParametrs();
	MemoryFullParameters.mode5_All  = Mode_5_GetAllParametrs();
	MemoryFullParameters.mode6_All  = Mode_6_GetAllParametrs();
	MemoryFullParameters.mode7_All  = Mode_7_GetAllParametrs();
	MemoryFullParameters.mode8_All  = Mode_8_GetAllParametrs();
	MemoryFullParameters.mode9_All  = Mode_9_GetAllParametrs();
	
	#ifdef BOARD_MEMORY_FLASH
			FlashEraseSector(5);
			FlashWriteData   ((uint32_t*)&MemoryFullParameters, (uint32_t*)0x08020000, (sizeof(MemoryFullParameters) / sizeof(uint8_t)));
	#endif
	#ifdef BOARD_MEMORY_EEPROM
			EEPROM_Write_Data((uint8_t*)&MemoryFullParameters, 0, (sizeof(MemoryFullParameters) / sizeof(uint8_t)) );
	#endif
	
	
}

void MemoryLoadParameters(void)
{
	#ifdef BOARD_MEMORY_FLASH
			FlashReadData((uint32_t*)&MemoryFullParameters, (sizeof(MemoryFullParameters) / sizeof(uint32_t)), 0x08020000);
	#endif
	#ifdef BOARD_MEMORY_EEPROM
			EEPROM_Read_Data((uint8_t*)&MemoryFullParameters, 0, (sizeof(MemoryFullParameters) / sizeof(uint8_t)) );
	#endif
		
	Mode_1_SetAllParametrs(MemoryFullParameters.mode1_All);
	Mode_2_SetAllParametrs(MemoryFullParameters.mode2_All);
	Mode_3_SetAllParametrs(MemoryFullParameters.mode3_All);
	Mode_4_SetAllParametrs(MemoryFullParameters.mode4_All);
	Mode_5_SetAllParametrs(MemoryFullParameters.mode5_All);
	Mode_6_SetAllParametrs(MemoryFullParameters.mode6_All);
	Mode_7_SetAllParametrs(MemoryFullParameters.mode7_All);
	Mode_8_SetAllParametrs(MemoryFullParameters.mode8_All);
	Mode_9_SetAllParametrs(MemoryFullParameters.mode9_All);
}

void MemoryLoadDefaults(void)
{	
	MemoryFullParameters.mode1_All.extFreq 			= EXT_FREQ_DEFAULT;
	MemoryFullParameters.mode1_All.intFreq 			= INT_FREQ_DEFAULT;
	MemoryFullParameters.mode1_All.extDuty			= EXT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode1_All.intDuty 			= INT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode1_All.phaseShift		= PHASESHIFT_DEFAULT;
	MemoryFullParameters.mode1_All.modePWM			= 1;
	MemoryFullParameters.mode1_All.deadTime			= DEADTIME_DEFAULT;
	MemoryFullParameters.mode1_All.sinShift			= SINSHIFT_DEFAULT;
	MemoryFullParameters.mode1_All.countdown		= COUNTDOWN_DEFAULT;
	MemoryFullParameters.mode1_All.sinDirection	= SINDIRECTION_DEFAULT;
	MemoryFullParameters.mode1_All.timStatus		= TIMSTATUS_DEFAULT;
	MemoryFullParameters.mode1_All.Nothing			= 0;
	
	MemoryFullParameters.mode2_All.extFreq 			= EXT_FREQ_DEFAULT;
	MemoryFullParameters.mode2_All.intFreq 			= INT_FREQ_DEFAULT;
	MemoryFullParameters.mode2_All.extDuty			= EXT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode2_All.intDuty 			= INT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode2_All.phaseShift		= PHASESHIFT_DEFAULT;
	MemoryFullParameters.mode2_All.modePWM			= 2;
	MemoryFullParameters.mode2_All.deadTime			= DEADTIME_DEFAULT;
	MemoryFullParameters.mode2_All.sinShift			= SINSHIFT_DEFAULT;
	MemoryFullParameters.mode2_All.countdown		= COUNTDOWN_DEFAULT;
	MemoryFullParameters.mode2_All.sinDirection	= SINDIRECTION_DEFAULT;
	MemoryFullParameters.mode2_All.timStatus		= TIMSTATUS_DEFAULT;
	MemoryFullParameters.mode2_All.Nothing			= 0;
	
	MemoryFullParameters.mode3_All.extFreq 			= EXT_FREQ_DEFAULT;
	MemoryFullParameters.mode3_All.intFreq 			= INT_FREQ_DEFAULT;
	MemoryFullParameters.mode3_All.extDuty			= EXT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode3_All.intDuty 			= INT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode3_All.phaseShift		= PHASESHIFT_DEFAULT;
	MemoryFullParameters.mode3_All.modePWM			= 3;
	MemoryFullParameters.mode3_All.deadTime			= DEADTIME_DEFAULT;
	MemoryFullParameters.mode3_All.sinShift			= SINSHIFT_DEFAULT;
	MemoryFullParameters.mode3_All.countdown		= COUNTDOWN_DEFAULT;
	MemoryFullParameters.mode3_All.sinDirection	= SINDIRECTION_DEFAULT;
	MemoryFullParameters.mode3_All.timStatus		= TIMSTATUS_DEFAULT;
	MemoryFullParameters.mode3_All.Nothing			= 0;
	
	MemoryFullParameters.mode4_All.extFreq 			= EXT_FREQ_DEFAULT;
	MemoryFullParameters.mode4_All.intFreq 			= INT_FREQ_DEFAULT;
	MemoryFullParameters.mode4_All.extDuty			= EXT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode4_All.intDuty 			= INT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode4_All.phaseShift		= PHASESHIFT_DEFAULT;
	MemoryFullParameters.mode4_All.modePWM			= 4;
	MemoryFullParameters.mode4_All.deadTime			= DEADTIME_DEFAULT;
	MemoryFullParameters.mode4_All.sinShift			= SINSHIFT_DEFAULT;
	MemoryFullParameters.mode4_All.countdown		= COUNTDOWN_DEFAULT;
	MemoryFullParameters.mode4_All.sinDirection	= SINDIRECTION_DEFAULT;
	MemoryFullParameters.mode4_All.timStatus		= TIMSTATUS_DEFAULT;
	MemoryFullParameters.mode4_All.Nothing			= 0;
	
	MemoryFullParameters.mode5_All.extFreq 			= EXT_FREQ_DEFAULT;
	MemoryFullParameters.mode5_All.intFreq 			= INT_FREQ_DEFAULT;
	MemoryFullParameters.mode5_All.extDuty			= EXT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode5_All.intDuty 			= INT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode5_All.phaseShift		= PHASESHIFT_DEFAULT;
	MemoryFullParameters.mode5_All.modePWM			= 6;
	MemoryFullParameters.mode5_All.deadTime			= DEADTIME_DEFAULT;
	MemoryFullParameters.mode5_All.sinShift			= SINSHIFT_DEFAULT;
	MemoryFullParameters.mode5_All.countdown		= COUNTDOWN_DEFAULT;
	MemoryFullParameters.mode5_All.sinDirection	= SINDIRECTION_DEFAULT;
	MemoryFullParameters.mode5_All.timStatus		= TIMSTATUS_DEFAULT;
	MemoryFullParameters.mode5_All.Nothing			= 0;
	
	MemoryFullParameters.mode6_All.extFreq 			= EXT_FREQ_DEFAULT;
	MemoryFullParameters.mode6_All.intFreq 			= INT_FREQ_DEFAULT;
	MemoryFullParameters.mode6_All.extDuty			= EXT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode6_All.intDuty 			= INT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode6_All.phaseShift		= PHASESHIFT_DEFAULT;
	MemoryFullParameters.mode6_All.modePWM			= 7;
	MemoryFullParameters.mode6_All.deadTime			= DEADTIME_DEFAULT;
	MemoryFullParameters.mode6_All.sinShift			= SINSHIFT_DEFAULT;
	MemoryFullParameters.mode6_All.countdown		= COUNTDOWN_DEFAULT;
	MemoryFullParameters.mode6_All.sinDirection	= SINDIRECTION_DEFAULT;
	MemoryFullParameters.mode6_All.timStatus		= TIMSTATUS_DEFAULT;
	MemoryFullParameters.mode6_All.Nothing			= 0;
	
	MemoryFullParameters.mode7_All.extFreq 			= EXT_FREQ_DEFAULT;
	MemoryFullParameters.mode7_All.intFreq 			= INT_FREQ_DEFAULT;
	MemoryFullParameters.mode7_All.extDuty			= EXT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode7_All.intDuty 			= INT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode7_All.phaseShift		= PHASESHIFT_DEFAULT;
	MemoryFullParameters.mode7_All.modePWM			= 8;
	MemoryFullParameters.mode7_All.deadTime			= DEADTIME_DEFAULT;
	MemoryFullParameters.mode7_All.sinShift			= SINSHIFT_DEFAULT;
	MemoryFullParameters.mode7_All.countdown		= COUNTDOWN_DEFAULT;
	MemoryFullParameters.mode7_All.sinDirection	= SINDIRECTION_DEFAULT;
	MemoryFullParameters.mode7_All.timStatus		= TIMSTATUS_DEFAULT;
	MemoryFullParameters.mode7_All.Nothing			= 0;
	
	MemoryFullParameters.mode8_All.extFreq 			= EXT_FREQ_DEFAULT;
	MemoryFullParameters.mode8_All.intFreq 			= INT_FREQ_DEFAULT;
	MemoryFullParameters.mode8_All.extDuty			= 1000;//EXT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode8_All.intDuty 			= INT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode8_All.phaseShift		= PHASESHIFT_DEFAULT;
	MemoryFullParameters.mode8_All.modePWM			= 9;
	MemoryFullParameters.mode8_All.deadTime			= DEADTIME_DEFAULT;
	MemoryFullParameters.mode8_All.sinShift			= SINSHIFT_DEFAULT;
	MemoryFullParameters.mode8_All.countdown		= COUNTDOWN_DEFAULT;
	MemoryFullParameters.mode8_All.sinDirection	= SINDIRECTION_DEFAULT;
	MemoryFullParameters.mode8_All.timStatus		= TIMSTATUS_DEFAULT;
	MemoryFullParameters.mode8_All.Nothing			= 0;
	
	MemoryFullParameters.mode9_All.extFreq 			= EXT_FREQ_DEFAULT;
	MemoryFullParameters.mode9_All.intFreq 			= INT_FREQ_DEFAULT;
	MemoryFullParameters.mode9_All.extDuty			= EXT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode9_All.intDuty 			= INT_DUTY_CYCLE_DEFAULT/10;
	MemoryFullParameters.mode9_All.phaseShift		= PHASESHIFT_DEFAULT;
	MemoryFullParameters.mode9_All.modePWM			= 10;
	MemoryFullParameters.mode9_All.deadTime			= DEADTIME_DEFAULT;
	MemoryFullParameters.mode9_All.sinShift			= SINSHIFT_DEFAULT;
	MemoryFullParameters.mode9_All.countdown		= COUNTDOWN_DEFAULT;
	MemoryFullParameters.mode9_All.sinDirection	= SINDIRECTION_DEFAULT;
	MemoryFullParameters.mode9_All.timStatus		= TIMSTATUS_DEFAULT;
	MemoryFullParameters.mode9_All.Nothing			= 0;
	
	Mode_1_SetAllParametrs(MemoryFullParameters.mode1_All);
	Mode_2_SetAllParametrs(MemoryFullParameters.mode2_All);
	Mode_3_SetAllParametrs(MemoryFullParameters.mode3_All);
	Mode_4_SetAllParametrs(MemoryFullParameters.mode4_All);
	Mode_5_SetAllParametrs(MemoryFullParameters.mode5_All);
	Mode_6_SetAllParametrs(MemoryFullParameters.mode6_All);
	Mode_7_SetAllParametrs(MemoryFullParameters.mode7_All);
	Mode_8_SetAllParametrs(MemoryFullParameters.mode8_All);
	Mode_9_SetAllParametrs(MemoryFullParameters.mode9_All);
	
//	FlashEraseSector(5);
//	FlashWriteData((uint32_t*)&MemoryFullParameters, (uint32_t*)0x08020000, 49*4);
}

void MemoryUnuse(void)
{

}
//---------------------------------------------------------------------------------------------------------//																				 
static void Menu_6_1_InitConfig(void)
{
		//wnd
		//wnd border
		menu_6_1WND.wndBorder.startPoint.x = 0;
		menu_6_1WND.wndBorder.startPoint.y = displayCanvas.screenHeight - 1;
		menu_6_1WND.wndBorder.endPoint.x = displayCanvas.screenWidth - 1;
		menu_6_1WND.wndBorder.endPoint.y = 0;
		menu_6_1WND.wndBorderThickness = 1;
		//set elements
		menu_6_1WND.menuElements = &_menu_6_1Elements[0];
		//wnd name and font
		menu_6_1WND.wndName = (uint8_t*)&menu_6_1WNDName[0];
		menu_6_1WND.wndNameFont.symbolWidth = 5;
		menu_6_1WND.wndNameFont.symbolHeight = 8;
		menu_6_1WND.wndNameFont.centerAlignment = 0;
		menu_6_1WND.wndNameFont.textSpacing = 1;
		menu_6_1WND.wndNameFont.fontRaw = (uint8_t*)&DefaultFont[0];	
		//set display canvas struct
		menu_6_1WND.displayCanvas = &displayCanvas;
		//init config
		MenuDialogWNDInitConfig(&menu_6_1WND);
		
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
		
		inputConfig->modePWM = 1;
		Gen407SetConfig(genStruct, inputConfig);
		SelectTimer1();

		//create scroll list
		if (!menu_6_1SignalScrollList)
		{
				menu_6_1SignalScrollList = UnniUiScrollListCreate(menu_6_1WND.wndBorder.startPoint.x + 1, menu_6_1WND.wndBorder.startPoint.y - 1,
																											  menu_6_1WND.wndBorder.endPoint.x - 1, menu_6_1WND.wndBorder.startPoint.y - 11, 10, 5, 2, 5,
																											  SCROLLLIST_LEFT_ALIGNED, &displayCanvas);
		}
		
		//UnniUiScrollListAddItem(menu_6_1SignalScrollList, 100, 12, (void*)&signalGenTypeSwitchBox, UnniUiSwitchBoxUpdate);

		if (!signalGenFrequencyExtEdit)
		{
				signalGenFrequencyExtEdit = UnniUiNumberEditCreate(0, 0, 8, 0, 0, EXT_FREQ_MIN, EXT_FREQ_MAX, EXT_FREQ_DEFAULT,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenFrequencyExtEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_1SignalScrollList, menu_6_1WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenFrequencyExtEdit, UnniUiNumberEditUpdate);
				Gen407SetExtFreq( genStruct,  UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit));
		}
		
		if (!signalGenDutyEdit)
		{
			signalGenDutyEdit = UnniUiNumberEditCreate(0, 0, 4, 1, NUMBEREDIT_FRACTIONAL, EXT_DUTY_CYCLE_MIN/10, EXT_DUTY_CYCLE_MAX/10, EXT_DUTY_CYCLE_DEFAULT,
				                                                   &numEditFont, &numEditStrFont, (uint8_t*)signalGenDutyEditStr, &displayCanvas);
			UnniUiScrollListAddItem(menu_6_1SignalScrollList, menu_6_1WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenDutyEdit, UnniUiNumberEditUpdate);
			UnniUiNumberEditSetFullValue(signalGenDutyEdit,EXT_DUTY_CYCLE_DEFAULT);
			Gen407SetExtDutyCycle( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyEdit) );
		}
		
		if (!signalGenDeadTimeEdit)
		{
				signalGenDeadTimeEdit = UnniUiNumberEditCreate(0, 0, 7, 0, 0, DEADTIME_MIN, DEADTIME_MAX, DEADTIME_DEFAULT,
				                                               &numEditFont, &numEditStrFont, (uint8_t*)signalGenDeadTimeEditStr, &displayCanvas);
				UnniUiScrollListAddItem(menu_6_1SignalScrollList, menu_6_1WND.wndBorder.endPoint.x - 30, 13, (void*)signalGenDeadTimeEdit, UnniUiNumberEditUpdate);
				Gen407SetDeadTime( genStruct, UnniUiNumberEditGetIntValue(signalGenDeadTimeEdit) );
		}
		
		UnniUiScrollListAddItem(menu_6_1SignalScrollList, 100, 12, (void*)&GeneratorPowerOnBox, UnniUiSwitchBoxUpdate);
		
		UnniUiScrollListAddItem(menu_6_1SignalScrollList, 100, 12, (void*)&MemorySaveLoadGenTypeSwitchBox, UnniUiSwitchBoxUpdate);
		if (!menu6_1_MessageBox)
		{
				menu6_1_MessageBox = UnniUiMessageBoxInit(10, 56, 108, 46, &numEditStrFont, &numEditStrFont, menu_6_1WND.displayCanvas);
		}
}

static void Menu_6_1_DeleteConfig(void)
{
}

uint8_t Menu_6_1_PatternGenerator_Draw(const uint8_t frameNum)
{
		//check menu window config
		if (!menu_6_1WND.displayCanvas)
		{
				//set canvas
				displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
				displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
				//menu config init
				Menu_6_1_InitConfig();				
		}
		//clear display buffer
		memset(displayBuffer.raw, 0, DISPLAY_BUFFER_SIZE);
		
		//draw menu dialog
		DrawMenuDialogWND(&menu_6_1WND);
		//draw scroll list with components
		UnniUiScrollListDraw(menu_6_1SignalScrollList);
		//draw message box
		UnniUiMessageBoxDraw(menu6_1_MessageBox);
	
		//write buf to display
		D128X64WriteBuf(Display128x64, &displayBuffer);
		return 0;
}

uint8_t Menu_6_1_PatternGenerator_InputInit(void)
{
		inputConfig->modePWM = 1;
		Gen407SetConfig(genStruct, inputConfig);
		SelectTimer1();
		
		Gen407SetExtFreq			( genStruct, UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit));		
		Gen407SetExtDutyCycle	( genStruct, UnniUiNumberEditGetFullValue(signalGenDutyEdit) );
		Gen407SetDeadTime			( genStruct, UnniUiNumberEditGetIntValue(signalGenDeadTimeEdit) );	
	
		return 0;
}

uint8_t Menu_6_1_PatternGenerator_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents)
{
//		volatile int32_t floatVal1,  floatVal3, floatVal4, floatVal6=0;
//		volatile int32_t floatVal2, floatVal5;
		uint32_t last_Frequency, now_Frequency;
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
				UnniUiScrollListUp(menu_6_1SignalScrollList);
		}
		// event - DOWN click
		if ( sysEvents & EVENT_BUTTON_DOWN_CLICK )
		{
				UnniUiScrollListDown(menu_6_1SignalScrollList);
		}
		// event - button Left click
		if ( sysEvents & EVENT_BUTTON_LEFT_CLICK )
		{
				UnniUiNumberEditCursorMoveLeft(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDutyEdit);
				UnniUiNumberEditCursorMoveLeft(signalGenDeadTimeEdit);
				UnniUiSwitchBoxPrev(&signalGenTypeSwitchBox);
				UnniUiSwitchBoxPrev(&GeneratorPowerOnBox);
				UnniUiSwitchBoxPrev(&MemorySaveLoadGenTypeSwitchBox);
		}
		// event - button Right click
		if ( sysEvents & EVENT_BUTTON_RIGHT_CLICK )
		{
				UnniUiNumberEditCursorMoveRight(signalGenFrequencyExtEdit);
				UnniUiNumberEditCursorMoveRight(signalGenDutyEdit);
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
				if (menu_6_1SignalScrollList->currentElementNumber == MEMORY_SAVE_LOAD_SWITCH_BOX_Menu1)
				{
						switch (MemorySaveLoadGenTypeSwitchBox.currentElementNumber)
						{
								case 1:
										MemorySaveParameters();
										Menu_6_1_PatternGenerator_InputInit();
										UnniUiMessageBoxShow(menu6_1_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Save OK", 40);	
								break;
								case 2:
										MemoryLoadParameters();
										Menu_6_1_PatternGenerator_InputInit();
										UnniUiMessageBoxShow(menu6_1_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Load OK", 40);	
								break;
								case 3:
										MemoryLoadDefaults();
										Menu_6_1_PatternGenerator_InputInit();
										UnniUiMessageBoxShow(menu6_1_MessageBox, (const uint8_t*)"Message", (const uint8_t*)"Load Def", 40);	
								break;
						}
				}
		}	
		
		// event - encoder CCW
		if (valcoderEvents & EVENT_VALCODER_CCW)
		{
				//clear flag manually
				valcoderEvents &= ~EVENT_VALCODER_CCW;
			
				last_Frequency = UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit);
				if ((UnniUiNumberEditDec(signalGenFrequencyExtEdit) == URG_OK)&&(last_Frequency!=1))
				{
					now_Frequency = UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit);
					if ( Gen407_GetRealFrequency(now_Frequency) > 0.0 )
					{
							if (((uint32_t)Gen407_GetRealFrequency(now_Frequency) == (uint32_t)Gen407_GetRealFrequency(last_Frequency))||(Gen407_GetRealFrequency_Now(_steps, _precision)==0)/*||(floatVal2>floatVal4)*/)
							{
									Gen407SetExtFreq( genStruct,  Gen407_GetPrevFrequency(last_Frequency));	
									UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, Gen407_GetPrevFrequency(last_Frequency));									
							} 
							else
							{
									Gen407SetExtFreq( genStruct,  Gen407_GetRealFrequency(now_Frequency));			
									UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, Gen407_GetRealFrequency(now_Frequency));								
							}
							UnniUiNumberEditSetValue(signalGenDeadTimeEdit, (_dead_time_ns * genStruct->currentConfig.deadTime) );
					}
					if(genStruct->generatorStartFlag == 1)
					{
						Gen407Update(genStruct);
//						GeneratorOff();
//						GeneratorOn();
						UnniUiNumberEditSetFullValue(signalGenDutyEdit, Gen407_GetRealDuty(UnniUiNumberEditGetFullValue(signalGenDutyEdit), _steps/2));
						Gen407SetExtDutyCycle( genStruct,  (Gen407_GetRealDuty(UnniUiNumberEditGetFullValue(signalGenDutyEdit), _steps/2)));
						Gen407Update(genStruct);
					}
				}
				
				last_Duty = UnniUiNumberEditGetFullValue(signalGenDutyEdit);
				if (UnniUiNumberEditDec(signalGenDutyEdit) == URG_OK)
				{
					if(genStruct->generatorStartFlag != 1)
					{
						StepsAndPrecisionValue		(genStruct->currentConfig.extFreq, genStruct->currentConfig.intFreq, genStruct->currentConfig.modePWM, genStruct->currentConfig.countdown, &_steps, &_precision);
					}
					now_Duty = UnniUiNumberEditGetFullValue(signalGenDutyEdit);
					if (((uint32_t)Gen407_GetRealDuty(now_Duty, _steps/2) == (uint32_t)Gen407_GetRealDuty(last_Duty, _steps/2)))
					{
							Gen407SetExtDutyCycle( genStruct,  (Gen407_GetPrevDuty(last_Duty, _steps/2)));	
							UnniUiNumberEditSetFullValue(signalGenDutyEdit, Gen407_GetPrevDuty(last_Duty, _steps/2));									
					} 
					else
					{					
							Gen407SetExtDutyCycle( genStruct,  (Gen407_GetRealDuty(now_Duty, _steps/2)));			
							UnniUiNumberEditSetFullValue(signalGenDutyEdit, Gen407_GetRealDuty(now_Duty, _steps/2));								
					}

					if(genStruct->generatorStartFlag == 1)
					{
						Gen407Update(genStruct);
//						GeneratorOff();
//						GeneratorOn();
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
				//clear flag manually
				valcoderEvents &= ~EVENT_VALCODER_CW;
			
				last_Frequency = UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit);
				if ((UnniUiNumberEditInc(signalGenFrequencyExtEdit) == URG_OK)&&(last_Frequency!=2333333))
				{
					now_Frequency = UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit);
					if ( Gen407_GetRealFrequency(now_Frequency) > 0.0 )
					{
							if (((uint32_t)Gen407_GetRealFrequency(now_Frequency) == (uint32_t)Gen407_GetRealFrequency(last_Frequency)) || (Gen407_GetRealFrequency_Now(_steps, _precision)==0))
							{
									Gen407SetExtFreq( genStruct,  Gen407_GetNextFrequency(last_Frequency));	
									UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, Gen407_GetNextFrequency(last_Frequency));									
							}
							else
							{
									Gen407SetExtFreq( genStruct,  Gen407_GetRealFrequency(now_Frequency));			
									UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, Gen407_GetRealFrequency(now_Frequency));								
							}
							UnniUiNumberEditSetValue(signalGenDeadTimeEdit, (_dead_time_ns * genStruct->currentConfig.deadTime) );
					}
					if((genStruct->generatorStartFlag == 1)&&(now_Frequency!=1))
					{
						
						Gen407Update(genStruct);
//						GeneratorOff();
//						GeneratorOn();
						UnniUiNumberEditSetFullValue(signalGenDutyEdit, Gen407_GetRealDuty(UnniUiNumberEditGetFullValue(signalGenDutyEdit), _steps/2));
						Gen407SetExtDutyCycle( genStruct,  (Gen407_GetRealDuty(UnniUiNumberEditGetFullValue(signalGenDutyEdit), _steps/2)));
						Gen407Update(genStruct);
					}
				}
				
				last_Duty = UnniUiNumberEditGetFullValue(signalGenDutyEdit);
				if ((UnniUiNumberEditInc(signalGenDutyEdit) == URG_OK)&&(last_Duty!=1000))
				{
					if(genStruct->generatorStartFlag != 1)
					{
						StepsAndPrecisionValue		(genStruct->currentConfig.extFreq, genStruct->currentConfig.intFreq, genStruct->currentConfig.modePWM, genStruct->currentConfig.countdown, &_steps, &_precision);
					}
					now_Duty = UnniUiNumberEditGetFullValue(signalGenDutyEdit);
					if (((uint32_t)Gen407_GetRealDuty(now_Duty, _steps/2) == (uint32_t)Gen407_GetRealDuty(last_Duty, _steps/2)))
					{
							Gen407SetExtDutyCycle( genStruct,  (Gen407_GetNextDuty(last_Duty, _steps/2)));	
							UnniUiNumberEditSetFullValue(signalGenDutyEdit, Gen407_GetNextDuty(last_Duty, _steps/2));									
					} 
					else
					{
							Gen407SetExtDutyCycle( genStruct,  (Gen407_GetRealDuty(now_Duty, _steps/2)));			
							UnniUiNumberEditSetFullValue(signalGenDutyEdit, Gen407_GetRealDuty(now_Duty, _steps/2));								
					}

					if(genStruct->generatorStartFlag == 1)
					{
						Gen407Update(genStruct);
//						GeneratorOff();
//						GeneratorOn();
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

GenParametersTypeDef Mode_1_GetAllParametrs(void)
{
	GenParametersTypeDef ParametrsAll;
	
	if (!menu_6_1WND.displayCanvas)
	{
			displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
			displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
			Menu_6_1_InitConfig();				
	}
		
	ParametrsAll.extFreq 			= UnniUiNumberEditGetIntValue(signalGenFrequencyExtEdit);
	ParametrsAll.intFreq 			= 0;
	ParametrsAll.extDuty			= UnniUiNumberEditGetFullValue(signalGenDutyEdit);
	ParametrsAll.intDuty 			= 0;
	ParametrsAll.phaseShift		= 0;
	ParametrsAll.modePWM			= 1;
//	ParametrsAll.deadTime			= UnniUiNumberEditGetIntValue(signalGenDeadTimeEdit);
	ParametrsAll.deadTime			= genStruct->currentConfig.deadTime;
	ParametrsAll.sinShift			= 0;
	ParametrsAll.countdown		= 0;
	ParametrsAll.sinDirection	= 0;
	ParametrsAll.timStatus		= 1;
	ParametrsAll.Nothing			= 0;
	
	return ParametrsAll;
}

GenParametersTypeDef Mode_1_SetAllParametrs(GenParametersTypeDef ParametrsAll)
{
	if (!menu_6_1WND.displayCanvas)
	{
			displayCanvas.color.color1B = URG_COLOR_1B_WHITE;
			displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
			Menu_6_1_InitConfig();				
	}
	
	UnniUiNumberEditSetValue(signalGenFrequencyExtEdit, ParametrsAll.extFreq);
	UnniUiNumberEditSetFullValue(signalGenDutyEdit, ParametrsAll.extDuty);
//	UnniUiNumberEditSetValue(signalGenDeadTimeEdit, ParametrsAll.deadTime);
	Gen407SetDeadTime(genStruct, ParametrsAll.deadTime);

	
	return ParametrsAll;
}
//---------------------------------------------------------------------------------------------------------//

//**************************************end of Menu_6_1_SignalGenerator.c************************************//
