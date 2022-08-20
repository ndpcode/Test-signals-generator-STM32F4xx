//*********************************************************************************************************//
//Display 128x64 library
//Created 15.08.2018
//Created by Novikov Dmitry
//*********************************************************************************************************//

//*********************************************************************************************************//
/* includes */
#include "display12864.h"

//*********************************************************************************************************//
/* function definitions */
static uint8_t getSignOfNumber(int8_t number);
static void displayMT12864WriteByte(D128X64_DISPLAY* display128x64, uint8_t dataByte, uint8_t displayMode, uint8_t displayChip);
static uint8_t displayMT12864ReadByte(D128X64_DISPLAY* display128x64, uint8_t displayMode, uint8_t displayChip);
static void displayST7920WriteByte(D128X64_DISPLAY* display128x64, uint8_t dataByte, uint8_t displayMode);
static uint8_t displayST7920ReadByte(D128X64_DISPLAY* display128x64, uint8_t displayMode);

/* extern functions */
extern inline void DisplayHalDriverConfigPinFast(DISPLAY_HAL* displayHal, uint8_t pinName, STM32FX_DISPLAY_HAL_PIN_TYPE pinType);
extern inline void DisplayHalWritePinLevelFast(DISPLAY_HAL* displayHal, uint8_t pinName, uint8_t pinLevel);
extern inline void DisplayHalSetPinHighFast(DISPLAY_HAL* displayHal, uint8_t pinName);
extern inline void DisplayHalSetPinLowFast(DISPLAY_HAL* displayHal, uint8_t pinName);
extern inline uint8_t DisplayHalReadPinLevelFast(DISPLAY_HAL* displayHal, uint8_t pinName);

//*********************************************************************************************************//
/* defines and constants */
#define displayMT12864WriteCodeL(displayStruct, codeByte) displayMT12864WriteByte(displayStruct, codeByte, MT12864_MODE_COMMAND, MT12864_CHIP_1)
#define displayMT12864WriteCodeR(displayStruct, codeByte) displayMT12864WriteByte(displayStruct, codeByte, MT12864_MODE_COMMAND, MT12864_CHIP_2)
#define displayMT12864WriteDataL(displayStruct, codeByte) displayMT12864WriteByte(displayStruct, codeByte, MT12864_MODE_DATA, MT12864_CHIP_1)
#define displayMT12864WriteDataR(displayStruct, codeByte) displayMT12864WriteByte(displayStruct, codeByte, MT12864_MODE_DATA, MT12864_CHIP_2)
#define displayMT12864ReadDataL(displayStruct) displayMT12864ReadByte(displayStruct, MT12864_MODE_DATA, MT12864_CHIP_1)
#define displayMT12864ReadDataR(displayStruct) displayMT12864ReadByte(displayStruct, MT12864_MODE_DATA, MT12864_CHIP_2)

//*********************************************************************************************************//
/* display 128x64 main functions */

//---------------------------------------------------------------------------------------------------------//
/* config all display data pins to input */
static void displayConfigDataPinsAsInput(D128X64_DISPLAY* display128x64)
{
		uint8_t i;
		for (i = 0; i < 8; i++)
		{
				#ifndef D128X64_MODE_FAST 
						DisplayHalWritePinLevel(display128x64->displayHal, i, 0);
						DisplayHalDriverConfigPin(display128x64->displayHal, i, D128X64_DEFAULT_INPUT_TYPE);
				#else
						DisplayHalSetPinLowFast(display128x64->displayHal, i);
						DisplayHalDriverConfigPinFast(display128x64->displayHal, i, D128X64_DEFAULT_INPUT_TYPE);						
				#endif
		}
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* config all display data pins to output */
static void displayConfigDataPinsAsOutput(D128X64_DISPLAY* display128x64)
{
		uint8_t i;
		for (i = 0; i < 8; i++)
		{
				#ifndef D128X64_MODE_FAST 
						DisplayHalWritePinLevel(display128x64->displayHal, i, 0);
						DisplayHalDriverConfigPin(display128x64->displayHal, i, D128X64_DEFAULT_OUTPUT_TYPE);
				#else
						DisplayHalSetPinLowFast(display128x64->displayHal, i);
						DisplayHalDriverConfigPinFast(display128x64->displayHal, i, D128X64_DEFAULT_OUTPUT_TYPE);						
				#endif
		}
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* reset (set to 0) all data pins of display */
static void displayResetDataPins(D128X64_DISPLAY* display128x64)
{
		uint8_t i;
		for (i = 0; i < 8; i++)
		{
				#ifndef D128X64_MODE_FAST
						DisplayHalWritePinLevel(display128x64->displayHal, i, 0);
				#else
						DisplayHalSetPinLowFast(display128x64->displayHal, i);
				#endif
		}
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* send pulse to display MT12864 */
static void displayMT12864Pulse(D128X64_DISPLAY* display128x64)
{
		//wait ns
		display128x64->displayHal->waitNS(MT12864_PAUSE_BEFORE_PULSE);
		//E (enable) = 1
		#ifndef D128X64_MODE_FAST
				DisplayHalWritePinLevel(display128x64->displayHal, MT12864_HAL_PIN_ENABLE, 1);
		#else
				DisplayHalSetPinHighFast(display128x64->displayHal, MT12864_HAL_PIN_ENABLE);
		#endif
		//wait ns
		display128x64->displayHal->waitNS(MT12864_PULSE_TIME);
		//E (enable) = 0
		#ifndef D128X64_MODE_FAST
				DisplayHalWritePinLevel(display128x64->displayHal, MT12864_HAL_PIN_ENABLE, 0);
		#else
				DisplayHalSetPinLowFast(display128x64->displayHal, MT12864_HAL_PIN_ENABLE);
		#endif
		//wait ns
		display128x64->displayHal->waitNS(MT12864_PAUSE_AFTER_PULSE);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* send pulse to display ST7920 */
static void displayST7920Pulse(D128X64_DISPLAY* display128x64)
{
		//wait ns
		display128x64->displayHal->waitNS(ST7920_PAUSE_BEFORE_PULSE);
		//E (enable) = 1
		#ifndef D128X64_MODE_FAST
				DisplayHalWritePinLevel(display128x64->displayHal, ST7920_HAL_PIN_ENABLE, 1);
		#else
				DisplayHalSetPinHighFast(display128x64->displayHal, ST7920_HAL_PIN_ENABLE);
		#endif
		//wait ns
		display128x64->displayHal->waitNS(ST7920_PULSE_TIME);
		//E (enable) = 0
		#ifndef D128X64_MODE_FAST
				DisplayHalWritePinLevel(display128x64->displayHal, ST7920_HAL_PIN_ENABLE, 0);
		#else
				DisplayHalSetPinLowFast(display128x64->displayHal, ST7920_HAL_PIN_ENABLE);
		#endif
		//wait ns
		display128x64->displayHal->waitNS(ST7920_PAUSE_AFTER_PULSE);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* wait display ready state */
static void displayWaitReady(uint8_t displayChip)
{
		//????????????????????????????????????????????????//
}

//---------------------------------------------------------------------------------------------------------//
/* read byte from display MT12864 */
static uint8_t displayMT12864ReadByte(D128X64_DISPLAY* display128x64, uint8_t displayMode, uint8_t displayChip)
{
		uint8_t i, dataByte, dataBit;
		
		//wait until display not ready
		displayWaitReady(displayChip);
		//config display ports to input
		displayConfigDataPinsAsInput(display128x64);
		//RW = 1
		#ifndef D128X64_MODE_FAST
				DisplayHalWritePinLevel(display128x64->displayHal, MT12864_HAL_PIN_READ_WRITE, 1);
		#else
				DisplayHalSetPinHighFast(display128x64->displayHal, MT12864_HAL_PIN_READ_WRITE);
		#endif
		//A0 (mode) = 0 (command) or 1(data)
		#ifndef D128X64_MODE_FAST
				DisplayHalWritePinLevel(display128x64->displayHal, MT12864_HAL_PIN_MODE, displayMode);
		#else	
				DisplayHalWritePinLevelFast(display128x64->displayHal, MT12864_HAL_PIN_MODE, displayMode);
		#endif
		//E1 (chip 1) or E2 (chip 2)
		if ( displayChip == MT12864_CHIP_1 )
		{
				#ifndef D128X64_MODE_FAST
						DisplayHalWritePinLevel(display128x64->displayHal, MT12864_HAL_PIN_CHIP_1, 1);
						DisplayHalWritePinLevel(display128x64->displayHal, MT12864_HAL_PIN_CHIP_2, 0);
				#else
						DisplayHalSetPinHighFast(display128x64->displayHal, MT12864_HAL_PIN_CHIP_1);
						DisplayHalSetPinLowFast(display128x64->displayHal, MT12864_HAL_PIN_CHIP_2);
				#endif
		} else
		{
				#ifndef D128X64_MODE_FAST
						DisplayHalWritePinLevel(display128x64->displayHal, MT12864_HAL_PIN_CHIP_1, 0);		
						DisplayHalWritePinLevel(display128x64->displayHal, MT12864_HAL_PIN_CHIP_2, 1);
				#else
						DisplayHalSetPinLowFast(display128x64->displayHal, MT12864_HAL_PIN_CHIP_1);
						DisplayHalSetPinHighFast(display128x64->displayHal, MT12864_HAL_PIN_CHIP_2);
				#endif
		}
		//wait ns
		display128x64->displayHal->waitNS(MT12864_PAUSE_BEFORE_PULSE);
		//E (enable) = 1
		#ifndef D128X64_MODE_FAST
				DisplayHalWritePinLevel(display128x64->displayHal, MT12864_HAL_PIN_ENABLE, 1);
		#else
				DisplayHalSetPinHighFast(display128x64->displayHal, MT12864_HAL_PIN_ENABLE);
		#endif
		//wait ns
		display128x64->displayHal->waitNS(MT12864_PULSE_TIME);
		//read data from pins
		dataByte = 0x00;
		for (i = 0; i < 8; i++)
		{
				#ifndef D128X64_MODE_FAST
						dataBit = 0x00;
						DisplayHalReadPinLevel(display128x64->displayHal, i, &dataBit);
						dataByte |= dataBit << i;
				#else
						dataByte |= DisplayHalReadPinLevelFast(display128x64->displayHal, i) << i;
				#endif
		}		
		//E (enable) = 0
		#ifndef D128X64_MODE_FAST
				DisplayHalWritePinLevel(display128x64->displayHal, MT12864_HAL_PIN_ENABLE, 0);
		#else
				DisplayHalSetPinLowFast(display128x64->displayHal, MT12864_HAL_PIN_ENABLE);
		#endif
		//wait ns
		display128x64->displayHal->waitNS(MT12864_PAUSE_AFTER_PULSE);		
		//config display ports to output
		displayConfigDataPinsAsOutput(display128x64);
		//return byte
		return dataByte;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* write byte to display MT12864 */
static void displayMT12864WriteByte(D128X64_DISPLAY* display128x64, uint8_t dataByte, uint8_t displayMode, uint8_t displayChip)
{
		uint8_t i;
		
		//wait until display not ready
		displayWaitReady(displayChip);
		//RW = 0
		#ifndef D128X64_MODE_FAST
				DisplayHalWritePinLevel(display128x64->displayHal, MT12864_HAL_PIN_READ_WRITE, 0);
		#else
				DisplayHalSetPinLowFast(display128x64->displayHal, MT12864_HAL_PIN_READ_WRITE);
		#endif
		//A0 (mode) = 0 (command) or 1(data)
		#ifndef D128X64_MODE_FAST
				DisplayHalWritePinLevel(display128x64->displayHal, MT12864_HAL_PIN_MODE, displayMode);
		#else	
				DisplayHalWritePinLevelFast(display128x64->displayHal, MT12864_HAL_PIN_MODE, displayMode);
		#endif
		//E1 (chip 1) or E2 (chip 2)
		if ( displayChip == MT12864_CHIP_1 )
		{
				#ifndef D128X64_MODE_FAST
						DisplayHalWritePinLevel(display128x64->displayHal, MT12864_HAL_PIN_CHIP_1, 1);
						DisplayHalWritePinLevel(display128x64->displayHal, MT12864_HAL_PIN_CHIP_2, 0);
				#else
						DisplayHalSetPinHighFast(display128x64->displayHal, MT12864_HAL_PIN_CHIP_1);
						DisplayHalSetPinLowFast(display128x64->displayHal, MT12864_HAL_PIN_CHIP_2);
				#endif
		} else
		{
				#ifndef D128X64_MODE_FAST
						DisplayHalWritePinLevel(display128x64->displayHal, MT12864_HAL_PIN_CHIP_1, 0);		
						DisplayHalWritePinLevel(display128x64->displayHal, MT12864_HAL_PIN_CHIP_2, 1);
				#else
						DisplayHalSetPinLowFast(display128x64->displayHal, MT12864_HAL_PIN_CHIP_1);
						DisplayHalSetPinHighFast(display128x64->displayHal, MT12864_HAL_PIN_CHIP_2);
				#endif
		}
		//write data byte to DATA_7...DATA_0
		for (i = 0; i < 8; i++)
		{
				#ifndef D128X64_MODE_FAST
						DisplayHalWritePinLevel(display128x64->displayHal, i, dataByte & 0x01);
				#else
						DisplayHalWritePinLevelFast(display128x64->displayHal, i, dataByte & 0x01);
				#endif
				dataByte = dataByte >> 1;
		}
		//send one pulse to display
		displayMT12864Pulse(display128x64);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* read byte from display ST7920 */
static uint8_t displayST7920ReadByte(D128X64_DISPLAY* display128x64, uint8_t displayMode)
{
		uint8_t i, dataByte, dataBit;
		
		//wait until display not ready
		displayWaitReady(0);
			
		//config display ports to input
		displayConfigDataPinsAsInput(display128x64);
	
		//RW = 1
		#ifndef D128X64_MODE_FAST
				DisplayHalWritePinLevel(display128x64->displayHal, ST7920_HAL_PIN_READ_WRITE, 1);
		#else
				DisplayHalSetPinHighFast(display128x64->displayHal, ST7920_HAL_PIN_READ_WRITE);
		#endif
	
		//A0 (mode) = 0 (command) or 1(data)
		#ifndef D128X64_MODE_FAST
				DisplayHalWritePinLevel(display128x64->displayHal, ST7920_HAL_PIN_MODE, displayMode);
		#else	
				DisplayHalWritePinLevelFast(display128x64->displayHal, ST7920_HAL_PIN_MODE, displayMode);
		#endif
		
		//wait ns
		display128x64->displayHal->waitNS(ST7920_PAUSE_BEFORE_PULSE);
		
		//E (enable) = 1
		#ifndef D128X64_MODE_FAST
				DisplayHalWritePinLevel(display128x64->displayHal, ST7920_HAL_PIN_ENABLE, 1);
		#else
				DisplayHalSetPinHighFast(display128x64->displayHal, ST7920_HAL_PIN_ENABLE);
		#endif
		
		//wait ns
		display128x64->displayHal->waitNS(ST7920_PULSE_TIME);
		
		//read data from pins
		dataByte = 0x00;
		for (i = 0; i < 8; i++)
		{
				#ifndef D128X64_MODE_FAST
						dataBit = 0x00;
						DisplayHalReadPinLevel(display128x64->displayHal, i, &dataBit);
						dataByte |= dataBit << i;
				#else
						dataByte |= DisplayHalReadPinLevelFast(display128x64->displayHal, i) << i;
				#endif
		}

		//E (enable) = 0
		#ifndef D128X64_MODE_FAST
				DisplayHalWritePinLevel(display128x64->displayHal, ST7920_HAL_PIN_ENABLE, 0);
		#else
				DisplayHalSetPinLowFast(display128x64->displayHal, ST7920_HAL_PIN_ENABLE);
		#endif
		
		//wait ns
		display128x64->displayHal->waitNS(ST7920_PAUSE_AFTER_PULSE);

		//config display ports to output
		displayConfigDataPinsAsOutput(display128x64);
		
		//return byte
		return dataByte;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* write byte to display ST7920 */
static void displayST7920WriteByte(D128X64_DISPLAY* display128x64, uint8_t dataByte, uint8_t displayMode)
{
		uint8_t i;
	
		//wait until display not ready
		displayWaitReady(0);
	
		//A0 (mode) = 0 (command) or 1(data)
		#ifndef D128X64_MODE_FAST
				DisplayHalWritePinLevel(display128x64->displayHal, ST7920_HAL_PIN_MODE, !displayMode);
		#else	
				DisplayHalWritePinLevelFast(display128x64->displayHal, ST7920_HAL_PIN_MODE, !displayMode);
		#endif
	
		//write data byte to DATA_7...DATA_0
		for (i = 0; i < 8; i++)
		{
				#ifndef D128X64_MODE_FAST
						DisplayHalWritePinLevel(display128x64->displayHal, i, dataByte & 0x01);
				#else
						DisplayHalWritePinLevelFast(display128x64->displayHal, i, dataByte & 0x01);
				#endif
				dataByte = dataByte >> 1;
		}
		
		//send one pulse to display
		displayST7920Pulse(display128x64);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* init display ST7920 extended function set */
static void displayST7920InitExtFunctionSet(D128X64_DISPLAY* display128x64, uint8_t extendedMode, uint8_t graphicMode)
{
		uint8_t data = 0x30;
	
		if (extendedMode)
		{
				data = 0x34;
		}
		
		if (graphicMode)
		{
				data = 0x36;
		}
		
		displayST7920WriteByte(display128x64, data, 1);
		//wait
		display128x64->displayHal->waitUS(70);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* init display ST7920 basic function set */
static void displayST7920InitBasicFunctionSet(D128X64_DISPLAY* display128x64, uint8_t basicMode)
{
		uint8_t data = 0x30;
	
		if (basicMode)
		{
				data = 0x34;
		}
		
		displayST7920WriteByte(display128x64, data, 1);
		//wait
		display128x64->displayHal->waitUS(70);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* init display ST7920 in graphic mode */
static void displayST7920InitInGraphMode(D128X64_DISPLAY* display128x64)
{
		//switch basic mode to extended
		displayST7920InitBasicFunctionSet(display128x64, 1);
		//graphic mode on
		displayST7920InitExtFunctionSet(display128x64, 1, 1);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* set display ST7920 cursor on screen */
static void displayST7920ExtSetGDRAMAddr(D128X64_DISPLAY* display128x64, uint8_t vertAddr, uint8_t horizAddr)
{
		uint8_t data = ST7920_CMDEXT_SETCGRAMADDR;
	
		data |= (vertAddr & 0x7F);
		displayST7920WriteByte(display128x64, data, 1);
	
		data = ST7920_CMDEXT_SETCGRAMADDR;
		data |= (horizAddr & 0x0F);
	
		displayST7920WriteByte(display128x64, data, 1);
		//wait
		display128x64->displayHal->waitUS(2);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* function for init MT12864 display */
D128X64_RESULT MT12864DisplayInit(uint8_t *portTemplate, void (*waitUS)(uint16_t timeUS),
                                  void (*waitNS)(uint16_t timeNS), D128X64_DISPLAY** display128x64)
{
		D128X64_DISPLAY* newDisplayMT12864;
		DISPLAY_HAL_RESULT displayHalResult;
		uint8_t i;
		
		//check input data
		if ( !display128x64 ) return D128X64_INPUT_DATA_ERROR;
		if ( *display128x64 ) return D128X64_DRIVER_NOT_EMPTY;
		
		//get memory for main display struct, check and clear
		newDisplayMT12864 = (D128X64_DISPLAY*)DISPLIB_GET_MEMORY(sizeof(D128X64_DISPLAY));
		if ( !newDisplayMT12864 ) return D128X64_MEMORY_ERROR;
		memset(newDisplayMT12864, 0, sizeof(D128X64_DISPLAY));
	
		//create display hal data struct
		displayHalResult = MT12864HalDriverInit(portTemplate, waitUS, waitNS, &newDisplayMT12864->displayHal);
		//check result
		if ( displayHalResult != DH_OK )
		{
				//free memory
				DISPLIB_FREE_MEMORY(newDisplayMT12864);
				//check errors and exit
				switch (displayHalResult)
				{
						case DH_ERROR: return D128X64_ERROR;
						case DH_BUSY: return D128X64_BUSY;
						case DH_INPUT_DATA_ERROR: return D128X64_INPUT_DATA_ERROR;
						case DH_DRIVER_NOT_EMPTY: return D128X64_DRIVER_NOT_EMPTY;
						case DH_MEMORY_ERROR: return D128X64_MEMORY_ERROR;
						default: return D128X64_ERROR;
				}
		}
		
		//all display ports config and set to 0
		for (i = 0; i < newDisplayMT12864->displayHal->displayPinCount; i++)
		{
				if ( DisplayHalDriverConfigPin(newDisplayMT12864->displayHal, i, D128X64_DEFAULT_OUTPUT_TYPE) != DH_OK )
				{
						D128X64DisplayDestroy(&newDisplayMT12864);
						return D128X64_ERROR;
				}
				if ( DisplayHalWritePinLevel(newDisplayMT12864->displayHal, i, 0) != DH_OK )
				{
						D128X64DisplayDestroy(&newDisplayMT12864);
						return D128X64_ERROR;
				}
		}

		/* display reset and init */
		//wait 2000 ns
		newDisplayMT12864->displayHal->waitNS(2000);
		//RES (reset) = 1
		DisplayHalWritePinLevel(newDisplayMT12864->displayHal, MT12864_HAL_PIN_RESET, 1);
		//wait 12000 ns
		newDisplayMT12864->displayHal->waitNS(12000);
		//set top line to 0
		displayMT12864WriteCodeL(newDisplayMT12864, 0xC0);
		displayMT12864WriteCodeR(newDisplayMT12864, 0xC0);
		//display ON
		displayMT12864WriteCodeL(newDisplayMT12864, 0x3F);
		displayMT12864WriteCodeR(newDisplayMT12864, 0x3F);		
		
		//set config
		newDisplayMT12864->displayType = DTYPE_MT12864;
		//set pointer and return
		*display128x64 = newDisplayMT12864;
		return D128X64_OK;
}																		
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* function for init ST7920 display */
D128X64_RESULT ST7920DisplayInit(uint8_t *portTemplate, void (*waitUS)(uint16_t timeUS),
                                  void (*waitNS)(uint16_t timeNS), D128X64_DISPLAY** display128x64)
{
		D128X64_DISPLAY* newDisplayST7920;
		DISPLAY_HAL_RESULT displayHalResult;
		uint8_t i;
		uint8_t Data;
		
		//check input data
		if ( !display128x64 ) return D128X64_INPUT_DATA_ERROR;
		if ( *display128x64 ) return D128X64_DRIVER_NOT_EMPTY;
		
		//get memory for main display struct, check and clear
		newDisplayST7920 = (D128X64_DISPLAY*)DISPLIB_GET_MEMORY(sizeof(D128X64_DISPLAY));
		if ( !newDisplayST7920 ) return D128X64_MEMORY_ERROR;
		memset(newDisplayST7920, 0, sizeof(D128X64_DISPLAY));
	
		//create display hal data struct
		displayHalResult = ST7920HalDriverInit(portTemplate, waitUS, waitNS, &newDisplayST7920->displayHal);
		//check result
		if ( displayHalResult != DH_OK )
		{
				//free memory
				DISPLIB_FREE_MEMORY(newDisplayST7920);
				//check errors and exit
				switch (displayHalResult)
				{
						case DH_ERROR: return D128X64_ERROR;
						case DH_BUSY: return D128X64_BUSY;
						case DH_INPUT_DATA_ERROR: return D128X64_INPUT_DATA_ERROR;
						case DH_DRIVER_NOT_EMPTY: return D128X64_DRIVER_NOT_EMPTY;
						case DH_MEMORY_ERROR: return D128X64_MEMORY_ERROR;
						default: return D128X64_ERROR;
				}
		}
		
		//all display ports config and set to 0
		for (i = 0; i < newDisplayST7920->displayHal->displayPinCount; i++)
		{
				if ( DisplayHalDriverConfigPin(newDisplayST7920->displayHal, i, D128X64_DEFAULT_OUTPUT_TYPE) != DH_OK )
				{
						D128X64DisplayDestroy(&newDisplayST7920);
						return D128X64_ERROR;
				}
				if ( DisplayHalWritePinLevel(newDisplayST7920->displayHal, i, 0) != DH_OK )
				{
						D128X64DisplayDestroy(&newDisplayST7920);
						return D128X64_ERROR;
				}
		}
		
		//PSB = 1
		DisplayHalWritePinLevel(newDisplayST7920->displayHal, ST7920_HAL_PIN_PSB, 1);
		
		//wait
		newDisplayST7920->displayHal->waitUS(50000);
		
		//parallel 8 bit
		displayST7920InitBasicFunctionSet(newDisplayST7920, 0);
		//display init in graphic mode
		displayST7920InitInGraphMode(newDisplayST7920);

		//set config
		newDisplayST7920->displayType = DTYPE_ST7920;
		//set pointer and return
		*display128x64 = newDisplayST7920;
		return D128X64_OK;
}																		
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* function for destroy display struct */
D128X64_RESULT D128X64DisplayDestroy(D128X64_DISPLAY** display128x64)
{
		uint8_t i;
	
		//check input data
		if ( (!display128x64) || (!(*display128x64)) ) return D128X64_INPUT_DATA_ERROR;
		
		//reset all ports
		if ( (*display128x64)->displayHal )
		{
				for (i = 0; i < (*display128x64)->displayHal->displayPinCount; i++)
				{
						DisplayHalWritePinLevel((*display128x64)->displayHal, i, 0);
						DisplayHalDriverConfigPin((*display128x64)->displayHal, i, D128X64_DEFAULT_FLOATING_TYPE);
				}			
		}
		
		//free memory
		DISPLIB_FREE_MEMORY((*display128x64)->displayHal);
		(*display128x64)->displayHal = 0;
		DISPLIB_FREE_MEMORY(*display128x64);
		*display128x64 = 0;
	
		return D128X64_OK;
}																		
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* function for reset MT12864 display */
D128X64_RESULT MT12864DisplayReset(D128X64_DISPLAY* display128x64)
{
		uint8_t p, c;
		
		//check input data
		if ( !display128x64 ) return D128X64_INPUT_DATA_ERROR;
	
		/* display reset and init */
		//wait 2000 ns
		display128x64->displayHal->waitNS(2000);
		//RES (reset) = 1
		DisplayHalWritePinLevel(display128x64->displayHal, MT12864_HAL_PIN_RESET, 1);
		//wait 12000 ns
		display128x64->displayHal->waitNS(12000);
		//set top line to 0
		displayMT12864WriteCodeL(display128x64, 0xC0);
		displayMT12864WriteCodeR(display128x64, 0xC0);
		//display ON
		displayMT12864WriteCodeL(display128x64, 0x3F);
		displayMT12864WriteCodeR(display128x64, 0x3F);
	
		for(p = 0; p < 8; p++)
		{
				displayMT12864WriteCodeL(display128x64, p | 0xB8);
				displayMT12864WriteCodeL(display128x64, 0x40);
				for(c = 0; c < 64; c++)
				{
						displayMT12864WriteDataL(display128x64, 0);
				}
				displayMT12864WriteCodeR(display128x64, p | 0xB8);
				displayMT12864WriteCodeR(display128x64, 0x40);
				for(c = 64; c < 128; c++)
				{
						displayMT12864WriteDataR(display128x64, 0);
				}
		}	
		return D128X64_OK;
}																		
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* function for reset MT12864 display */
D128X64_RESULT ST7920DisplayReset(D128X64_DISPLAY* display128x64)
{
		//check input data
		if ( !display128x64 ) return D128X64_INPUT_DATA_ERROR;
	
		return D128X64_OK;
}																		
//---------------------------------------------------------------------------------------------------------//

D128X64_RESULT D128X64DisplayReset(D128X64_DISPLAY* display128x64)
{
		//check input data
		if (!display128x64) return D128X64_INPUT_DATA_ERROR;
	
		//choice display type
		switch (display128x64->displayType)
		{
				case DTYPE_MT12864:
						return MT12864DisplayReset(display128x64); 
				case DTYPE_ST7920:
						return ST7920DisplayReset(display128x64);
		}	
}

//---------------------------------------------------------------------------------------------------------//
/* function for write point to MT12864 display */
D128X64_RESULT MT12864DisplayWritePoint(D128X64_DISPLAY* display128x64, uint8_t posX, uint8_t posY, uint8_t pointState)
{
		uint8_t usedChip, lastByte, newByte;
		
		//check input data
		if ( !display128x64 ) return D128X64_INPUT_DATA_ERROR;
		if ( (posX > (D128X64_WIDTH - 1)) || (posY > (D128X64_HEIGHT - 1)) ) return D128X64_INPUT_DATA_ERROR;
		
		//check posX and choose chip
		usedChip = MT12864_CHIP_1;
		if ( posX > 63 )
		{
				//chip #2
				posX -= 64;
				usedChip = MT12864_CHIP_2;
		}
		
		//set display page
		displayMT12864WriteByte(display128x64, 0xB8 | (posY >> 3), MT12864_MODE_COMMAND, usedChip);
		//set byte address
		displayMT12864WriteByte(display128x64, 0x40 | posX, MT12864_MODE_COMMAND, usedChip);
		//read data from port x 2
		lastByte = displayMT12864ReadByte(display128x64, MT12864_MODE_DATA, usedChip);
		lastByte = displayMT12864ReadByte(display128x64, MT12864_MODE_DATA, usedChip);
		//get mask for target bit
		newByte = 1 << (posY & 0x07);
		//apply target bit to data
		if ( pointState )
		{
				lastByte |= newByte;
		} else
		{
				lastByte &= ~newByte;
		}
		//set byte address
		displayMT12864WriteByte(display128x64, 0x40 | posX, MT12864_MODE_COMMAND, usedChip);
		//write data to port
		displayMT12864WriteByte(display128x64, lastByte, MT12864_MODE_DATA, usedChip);
	
		return D128X64_OK;
}																		
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* function for write point to ST7920 display */
D128X64_RESULT ST7920DisplayWritePoint(D128X64_DISPLAY* display128x64, uint8_t posX, uint8_t posY, uint8_t pointState)
{
		uint8_t usedChip, lastByte, newByte;
		
		//check input data
		if ( !display128x64 ) return D128X64_INPUT_DATA_ERROR;
		if ( (posX > (D128X64_WIDTH - 1)) || (posY > (D128X64_HEIGHT - 1)) ) return D128X64_INPUT_DATA_ERROR;
	
		return D128X64_ERROR;
}																		
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* function for write point to display */
D128X64_RESULT D128X64DisplayWritePoint(D128X64_DISPLAY* display128x64, uint8_t posX, uint8_t posY, uint8_t pointState)
{
		//check input data
		if (!display128x64) return D128X64_INPUT_DATA_ERROR;
	
		//choice display type
		switch (display128x64->displayType)
		{
				case DTYPE_MT12864:
						return MT12864DisplayWritePoint(display128x64, posX, posY, pointState); 
				case DTYPE_ST7920:
						return ST7920DisplayWritePoint(display128x64, posX, posY, pointState);
		}	
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* function for write buffer to MT12864 display */
D128X64_RESULT MT12864WriteBuf(D128X64_DISPLAY* display128x64, D128X64_BUFFER* buffer)
{
		uint8_t row, col;
	
		//check input data
		if ( !display128x64 ) return D128X64_INPUT_DATA_ERROR;
		if ( !buffer ) return D128X64_INPUT_DATA_ERROR;
	
		//write data
		for(row = 0; row < 8; row++)
		{
				displayMT12864WriteCodeL(display128x64, row | 0xB8);
				displayMT12864WriteCodeL(display128x64, 0x40);
				for(col = 0; col < 64; col++)
				{
						displayMT12864WriteDataL(display128x64, buffer->raw[col][row]);
				}
				displayMT12864WriteCodeR(display128x64, row | 0xB8);
				displayMT12864WriteCodeR(display128x64, 0x40);
				for(col = 64; col < 128; col++)
				{
						displayMT12864WriteDataR(display128x64, buffer->raw[col][row]);
				}
		}	
		return D128X64_OK;		
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* function for transform buffer data to ST7920 display */
static inline uint8_t displayST7920GetHorizontalByte(D128X64_BUFFER* buffer, uint8_t row, uint8_t col)
{
		uint8_t byte = 0;
		uint8_t k;
	
		for (k = 0; k < 8; k++)
		{
				byte |= ( (buffer->raw[col * 8 + k][row / 8] & (0x01 << (row & 0x07))) >> (row & 0x07) ) << (7 - k);
		}
		
		return byte;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* function for write buffer to ST7920 display */
D128X64_RESULT ST7920WriteBuf(D128X64_DISPLAY* display128x64, D128X64_BUFFER* buffer)
{
		uint8_t row, col;
		
		//check input data
		if ( !display128x64 ) return D128X64_INPUT_DATA_ERROR;
		if ( !buffer ) return D128X64_INPUT_DATA_ERROR;
	
		//write data
		for (row = 0; row < 32; row++)
		{
				//set cursor pos
				displayST7920ExtSetGDRAMAddr(display128x64, row, 0);
				
				for (col = 0; col < 16; col++)
				{
						//top part of display
						displayST7920WriteByte(display128x64, displayST7920GetHorizontalByte(buffer, row, col), 0);
				}
				for (col = 0; col < 16; col++)
				{
						//bottom part of display
						displayST7920WriteByte(display128x64, displayST7920GetHorizontalByte(buffer, row + 32, col), 0);
				}
		}
		
		return D128X64_OK;		
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* function for write buffer to display */
D128X64_RESULT D128X64WriteBuf(D128X64_DISPLAY* display128x64, D128X64_BUFFER* buffer)
{
		//check input data
		if (!display128x64) return D128X64_INPUT_DATA_ERROR;
	
		//choice display type
		switch (display128x64->displayType)
		{
				case DTYPE_MT12864:
						return MT12864WriteBuf(display128x64, buffer); 
				case DTYPE_ST7920:
						return ST7920WriteBuf(display128x64, buffer);
		}	
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* function for write demo logo to MT12864 display */
D128X64_RESULT MT12864DemoLogoWrite(D128X64_DISPLAY* display128x64)
{
const uint8_t Logo128[8][128]=
{
	{	0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
		0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
		0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
		0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
		0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
		0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
		0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
		0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF
	},
	{	0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xC0,0xE0,0xF0,0xF0,0xF0,
		0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xE0,0xE0,0xE0,0xC0,0x80,0x00,
		0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
		0x00,0x80,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x80,0x00,0x00,
		0x80,0x00,0x00,0x80,0x00,0x80,0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x80,
		0x00,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,
		0x00,0x00,0x80,0x00,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x80,0x00,0x00,0x80,0x00,
		0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x3F
	},
	{	0x1F,0xC0,0xE0,0xE0,0xFC,0xFE,0xF3,0xE3,0xE3,0xE3,0xE3,0xAF,0x6F,0xFF,0xFF,0xFF,
		0xBF,0xBF,0xFF,0xFF,0x7F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xF6,0xF4,0x1F,0x21,0xE2,0x61,0x6F,0x60,0x6F,0x64,0x6F,0x60,0x6F,0x65,0x68,
		0x60,0x6F,0x62,0x61,0x67,0x68,0x67,0x60,0x6A,0x67,0x60,0x6E,0x61,0x6F,0x60,0x67,
		0x6A,0x6B,0x60,0x6F,0x65,0x68,0x60,0x60,0x6F,0x60,0x60,0x6F,0x62,0x61,0x67,0x68,
		0x67,0x60,0x6F,0x62,0x6F,0x60,0x6F,0x64,0x6F,0x60,0x6F,0x65,0x68,0x64,0x6A,0x6F,
		0x60,0x60,0x6F,0x64,0x6F,0x60,0x60,0x60,0x6F,0x60,0x67,0x6A,0x6B,0x60,0x6D,0x62,
		0x6D,0x60,0x6F,0x62,0x6F,0x60,0x6F,0x64,0x6F,0x60,0x6F,0x65,0x68,0x64,0x6A,0x6F
	},
	{	0xFE,0x3F,0x3F,0x3F,0xBF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xF9,0x77,0x37,
		0x03,0x03,0x01,0x00,0x00,0x00,0x00,0x01,0x07,0x0F,0x13,0x01,0x00,0x00,0x01,0x07,
		0xD7,0x0E,0x08,0xC0,0x00,0x10,0x0F,0xF8,0x00,0x00,0x80,0xE0,0xF8,0xFE,0xFF,0xFF,
		0xFF,0xFE,0x00,0x00,0x00,0x00,0x00,0xC0,0xF0,0xFE,0xFF,0xFF,0xFF,0xFE,0x00,0x00,
		0x00,0xE0,0xF0,0xF8,0xFC,0x7C,0x3E,0x1E,0x1F,0x0F,0x0F,0x0F,0x0F,0x1F,0x1E,0x1E,
		0x3E,0x7C,0xFC,0xF8,0xF0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x80,0xC0,0xF0,0xFC,0xFE,0xFF,0xFF,0xFE,0x00,0x00,0x06,0x0F,0x0F,0x0F,0x0F,0x0F,
		0x0F,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x06,0xF0
	},
	{	0x3F,0xE0,0x00,0x00,0x01,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0x3F,0x3F,0x00,0x00,0xE0,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x01,0x01,0xC1,0xE0,0xE0,0xF9,0xFF,0xFC,0xFF,0xFF,0x7F,0x1F,0xFF,0xFF,0xFF,
		0xFF,0xFF,0x80,0xE0,0xF8,0xFE,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
		0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x30,0x78,0x78,0xFC,0xFC,0xFC,0xFC,0x78,0x78,
		0x30,0x00,0x01,0x07,0xFF,0xFF,0xFF,0xFC,0x00,0x00,0x00,0x00,0x80,0xE0,0xF8,0xFE,
		0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF
	},
	{	0xFC,0x01,0x03,0x04,0x18,0x20,0x40,0x81,0x87,0x07,0x07,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,
		0x80,0xE4,0xE7,0xFB,0xFF,0xFF,0x7F,0x3F,0x0F,0x03,0x01,0x00,0x00,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0x7F,0x1F,0x07,0x03,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
		0x00,0x70,0xF8,0xF8,0xF0,0xE0,0xE0,0xC0,0xC0,0x80,0x80,0x80,0x80,0xC0,0xC0,0xC0,
		0xE0,0xF0,0xFC,0xFF,0x7F,0x3F,0x0F,0x81,0xE0,0xF8,0xFC,0xFF,0xFF,0x3F,0x1F,0x07,
		0x01,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF
	},
	{	0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x02,0x04,0x04,0x04,0x0C,
		0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x04,0x04,0x04,0x02,0x03,0x01,0x00,
		0x00,0x03,0x07,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x07,0x07,
		0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x07,0x07,0x07,0x03,0x00,0x00,
		0x00,0x00,0x00,0x01,0x01,0x03,0x03,0x03,0x07,0x07,0x07,0x07,0x07,0x07,0x03,0x03,
		0x03,0x01,0x01,0x00,0x00,0x00,0x00,0x03,0x07,0x07,0x07,0x03,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x03,0x07,0x07,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x03,0x07,0x07,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF
	},
	{	0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFF
	}
};		
		uint8_t p, c;
	
		for(p = 0; p < 8; p++)
		{
				displayMT12864WriteCodeL(display128x64, p | 0xB8);
				displayMT12864WriteCodeL(display128x64, 0x40);
				for(c = 0; c < 64; c++)
				{
						displayMT12864WriteDataL(display128x64, Logo128[p][c]);
				}
				displayMT12864WriteCodeR(display128x64, p | 0xB8);
				displayMT12864WriteCodeR(display128x64, 0x40);
				for(c = 64; c < 128; c++)
				{
						displayMT12864WriteDataR(display128x64, Logo128[p][c]);
				}
		}	
		return D128X64_OK;
}
//---------------------------------------------------------------------------------------------------------//

//********************************************end of mt12864.c*********************************************//
