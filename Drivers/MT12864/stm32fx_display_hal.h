//*********************************************************************************************************//
//stm32fxxx display hal library
//Created 12.08.2018
//Created by Novikov Dmitry
//*********************************************************************************************************//

#ifndef __STM32FX_DISPLAY_HAL_H
#define __STM32FX_DISPLAY_HAL_H

//*********************************************************************************************************//
/* includes */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "gpio.h"

//*********************************************************************************************************//
/* defines, constants and macro */

/* pin count for different displays */
#define HD44780_DISPLAY_PIN_COUNT 11
#define MT12864_DISPLAY_PIN_COUNT 14
#define ST7920_DISPLAY_PIN_COUNT  13

/* memory alloc macro set */
#define STM32FX_DISPLAY_HAL_GET_MEMORY(memSize) malloc(memSize)
#define STM32FX_DISPLAY_HAL_FREE_MEMORY(memPointer) free(memPointer)

//*********************************************************************************************************//
/* enumeration lists */

/* display type enum */
typedef enum
{
		/* for HD44780 display, 2x16 etc. */
		DISPLAY_HAL_TYPE_HD44780 = 1,
		/* for MELT 128x64 display */
		DISPLAY_HAL_TYPE_MT12864 = 2,
		/* for ST7920 128x64 display */
		DISPLAY_HAL_TYPE_ST7920  = 3
} DISPLAY_HAL_TYPE;

/* display hal func result enum */
typedef enum
{
		DH_OK = 0,
		DH_ERROR,
		DH_BUSY,
		DH_INPUT_DATA_ERROR,
		DH_DRIVER_NOT_EMPTY,
		DH_MEMORY_ERROR
} DISPLAY_HAL_RESULT;

/* display hal port type enum */
typedef enum
{
	DISPLAY_PIN_TYPE_INPUT = 0,
	DISPLAY_PIN_TYPE_OUTPUT_PP,
	DISPLAY_PIN_TYPE_OUTPUT_OD,
	DISPLAY_PIN_TYPE_FLOATING
} STM32FX_DISPLAY_HAL_PIN_TYPE;

/* HD44780 display ports enum */
typedef enum
{
		HD44780_HAL_PIN_DATA_0 = 0,
		HD44780_HAL_PIN_DATA_1,
		HD44780_HAL_PIN_DATA_2,
		HD44780_HAL_PIN_DATA_3,
		HD44780_HAL_PIN_DATA_4,
		HD44780_HAL_PIN_DATA_5,
		HD44780_HAL_PIN_DATA_6,
		HD44780_HAL_PIN_DATA_7,
		HD44780_HAL_PIN_ENABLE,
		HD44780_HAL_PIN_REG_SELECT,
		HD44780_HAL_PIN_READ_WRITE	
} DISPLAY_HD44780_PIN_NAME;

/* MT12864 display ports enum */
typedef enum
{
		MT12864_HAL_PIN_DATA_0 = 0,
		MT12864_HAL_PIN_DATA_1,
		MT12864_HAL_PIN_DATA_2,
		MT12864_HAL_PIN_DATA_3,
		MT12864_HAL_PIN_DATA_4,
		MT12864_HAL_PIN_DATA_5,
		MT12864_HAL_PIN_DATA_6,
		MT12864_HAL_PIN_DATA_7,
		MT12864_HAL_PIN_MODE,
		MT12864_HAL_PIN_READ_WRITE,
		MT12864_HAL_PIN_ENABLE,
		MT12864_HAL_PIN_CHIP_1,
		MT12864_HAL_PIN_CHIP_2,
		MT12864_HAL_PIN_RESET	
} DISPLAY_MT12864_PIN_NAME;

/* ST7920 display ports enum */
typedef enum
{
		ST7920_HAL_PIN_DATA_0 = 0,
		ST7920_HAL_PIN_DATA_1,
		ST7920_HAL_PIN_DATA_2,
		ST7920_HAL_PIN_DATA_3,
		ST7920_HAL_PIN_DATA_4,
		ST7920_HAL_PIN_DATA_5,
		ST7920_HAL_PIN_DATA_6,
		ST7920_HAL_PIN_DATA_7,
		ST7920_HAL_PIN_MODE,					//Pin  RS
		ST7920_HAL_PIN_READ_WRITE,		//Pin  R/W
		ST7920_HAL_PIN_ENABLE,				//Pin  E
		ST7920_HAL_PIN_PSB,
		ST7920_HAL_PIN_RESET	
} DISPLAY_ST7920_PIN_NAME;

//*********************************************************************************************************//
/* struct declarations */

/* dispay hal one pin config struct */
typedef struct
{
		GPIO_TypeDef *portAddress;
		uint8_t pinIndex;	
} DISPLAY_HAL_PIN_STRUCT;

/* display hal main config */
typedef struct
{
		DISPLAY_HAL_PIN_STRUCT* displayPort;
		DISPLAY_HAL_TYPE displayType;
		uint8_t displayPinCount;
		void (*waitMS)(uint16_t timeMS);
		void (*waitUS)(uint16_t timeUS);
		void (*waitNS)(uint16_t timeNS);
} DISPLAY_HAL;

//*********************************************************************************************************//
/* stm32fx display hal functions */
DISPLAY_HAL_RESULT HD44780HalDriverInit(uint8_t *portTemplate, uint8_t is4BitMode, void (*waitMS)(uint16_t timeMS),
                                        void (*waitUS)(uint16_t timeUS), DISPLAY_HAL** displayHal);
DISPLAY_HAL_RESULT MT12864HalDriverInit(uint8_t *portTemplate, void (*waitUS)(uint16_t timeUS),
                                        void (*waitNS)(uint16_t timeNS), DISPLAY_HAL** displayHal);
DISPLAY_HAL_RESULT ST7920HalDriverInit(uint8_t *portTemplate, void (*waitUS)(uint16_t timeUS),
                                        void (*waitNS)(uint16_t timeNS), DISPLAY_HAL** displayHal);
DISPLAY_HAL_RESULT DisplayHalDriverDestroy(DISPLAY_HAL** displayHal);
DISPLAY_HAL_RESULT DisplayHalDriverConfigPin(DISPLAY_HAL* displayHal, uint8_t pinName, STM32FX_DISPLAY_HAL_PIN_TYPE pinType);
DISPLAY_HAL_RESULT DisplayHalWritePinLevel(DISPLAY_HAL* displayHal, uint8_t pinName, uint8_t pinLevel);
DISPLAY_HAL_RESULT DisplayHalReadPinLevel(DISPLAY_HAL* displayHal, uint8_t pinName, uint8_t* pinLevel);

//*********************************************************************************************************//
/* stm32fx display fast functions - as inline */

/* config display IO pin, without input data check */
inline void DisplayHalDriverConfigPinFast(DISPLAY_HAL* displayHal, uint8_t pinName, STM32FX_DISPLAY_HAL_PIN_TYPE pinType)
{
		//set pin config depending input param
		switch (pinType)
		{
				case DISPLAY_PIN_TYPE_INPUT:
						GPIOConfigPinP(displayHal->displayPort[pinName].portAddress,
						               displayHal->displayPort[pinName].pinIndex,
						               GPIO_PIN_MODE_INPUT,
						               GPIO_PIN_TYPE_RESET,
						               GPIO_PIN_SPEED_RESET,
						               GPIO_PIN_NOPULLUPDOWN,
						               GPIO_PIN_ALTF_RESET);				
				break;
				
				case DISPLAY_PIN_TYPE_OUTPUT_PP:
						GPIOConfigPinP(displayHal->displayPort[pinName].portAddress,
						               displayHal->displayPort[pinName].pinIndex,
						               GPIO_PIN_MODE_OUTPUT,
						               GPIO_PIN_TYPE_PUSHPULL,
						               GPIO_PIN_SPEED_VERYHIGH,
						               GPIO_PIN_NOPULLUPDOWN,
						               GPIO_PIN_ALTF_RESET);
				break;
				
				case DISPLAY_PIN_TYPE_OUTPUT_OD:
						GPIOConfigPinP(displayHal->displayPort[pinName].portAddress,
						               displayHal->displayPort[pinName].pinIndex,
						               GPIO_PIN_MODE_OUTPUT,
						               GPIO_PIN_TYPE_OPENDRAIN,
						               GPIO_PIN_SPEED_VERYHIGH,
						               GPIO_PIN_NOPULLUPDOWN,
						               GPIO_PIN_ALTF_RESET);
				break;
				
				case DISPLAY_PIN_TYPE_FLOATING:
						GPIOConfigPinP(displayHal->displayPort[pinName].portAddress,
						               displayHal->displayPort[pinName].pinIndex,
						               GPIO_PIN_MODE_RESET,
						               GPIO_PIN_TYPE_RESET,
						               GPIO_PIN_SPEED_RESET,
						               GPIO_PIN_NOPULLUPDOWN,
						               GPIO_PIN_ALTF_RESET);
				break;
				
				default:
				break;
		}
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* write level 1 or 0 to display IO pin, without input data check */
inline void DisplayHalWritePinLevelFast(DISPLAY_HAL* displayHal, uint8_t pinName, uint8_t pinLevel)
{
		//set pin state
		if (pinLevel)
		{
				//set
				GPIOSetPinP(displayHal->displayPort[pinName].portAddress, displayHal->displayPort[pinName].pinIndex);
		} else
		{
				//reset
				GPIOResetPinP(displayHal->displayPort[pinName].portAddress, displayHal->displayPort[pinName].pinIndex);
		}
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* write level 1 to display IO pin, without input data check */
inline void DisplayHalSetPinHighFast(DISPLAY_HAL* displayHal, uint8_t pinName)
{
		GPIOSetPinP(displayHal->displayPort[pinName].portAddress, displayHal->displayPort[pinName].pinIndex);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* write level 0 to display IO pin, without input data check */
inline void DisplayHalSetPinLowFast(DISPLAY_HAL* displayHal, uint8_t pinName)
{
		GPIOResetPinP(displayHal->displayPort[pinName].portAddress, displayHal->displayPort[pinName].pinIndex);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* read display IO pin level, without input data check */
inline uint8_t DisplayHalReadPinLevelFast(DISPLAY_HAL* displayHal, uint8_t pinName)
{
		return GPIOReadPinP(displayHal->displayPort[pinName].portAddress, displayHal->displayPort[pinName].pinIndex);
}
//---------------------------------------------------------------------------------------------------------//

#endif
//*************************************end of stm32fx_display_hal.h****************************************//
