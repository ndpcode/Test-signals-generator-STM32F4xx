//*********************************************************************************************************//
//stm32fxxx display hal library
//Created 12.08.2018
//Created by Novikov Dmitry
//*********************************************************************************************************//

//*********************************************************************************************************//
/* includes */
#include "stm32fx_display_hal.h"

//*********************************************************************************************************//
/* defines and constants */

//*********************************************************************************************************//
/* stm32fx display hal functions */

//---------------------------------------------------------------------------------------------------------//
/* driver config validate */
static uint8_t checkDisplayHalDriverConfig(DISPLAY_HAL* displayHal)
{
		if ( !displayHal ) return 0;
		if ( !displayHal->displayPort ) return 0;
		if ( ( displayHal->displayType != DISPLAY_HAL_TYPE_HD44780 ) &&
		     ( displayHal->displayType != DISPLAY_HAL_TYPE_MT12864 ) &&
				 ( displayHal->displayType != DISPLAY_HAL_TYPE_ST7920  )	) return 0;
		if ( !displayHal->displayPinCount ) return 0;
		
		return 1;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* init hal driver for HD44780 display */
DISPLAY_HAL_RESULT HD44780HalDriverInit(uint8_t *portTemplate, uint8_t is4BitMode, void (*waitMS)(uint16_t timeMS),
                                        void (*waitUS)(uint16_t timeUS), DISPLAY_HAL** displayHal)
{
		DISPLAY_HAL* newDisplayHalDriver;
		uint8_t i;
	
		//check input data
		if ( !portTemplate ) return DH_INPUT_DATA_ERROR;
		if ( ( is4BitMode != 0 ) && ( is4BitMode != 1 ) ) return DH_INPUT_DATA_ERROR;
		if ( !waitMS ) return DH_INPUT_DATA_ERROR;
		if ( !waitUS ) return DH_INPUT_DATA_ERROR;
		if ( !displayHal ) return DH_INPUT_DATA_ERROR;
		if ( *displayHal ) return DH_DRIVER_NOT_EMPTY;
	
		//get memory for main display hal struct, check and clear
		newDisplayHalDriver = (DISPLAY_HAL*)STM32FX_DISPLAY_HAL_GET_MEMORY(sizeof(DISPLAY_HAL));
		if ( !newDisplayHalDriver ) return DH_MEMORY_ERROR;
		memset(newDisplayHalDriver, 0, sizeof(DISPLAY_HAL));
		
		//get memory for display pin struct config, check and clear
		newDisplayHalDriver->displayPort =
				(DISPLAY_HAL_PIN_STRUCT*)STM32FX_DISPLAY_HAL_GET_MEMORY(sizeof(DISPLAY_HAL_PIN_STRUCT)*HD44780_DISPLAY_PIN_COUNT);
		if ( !newDisplayHalDriver->displayPort )
		{
				STM32FX_DISPLAY_HAL_FREE_MEMORY(newDisplayHalDriver);
				return DH_MEMORY_ERROR;
		}
		memset(newDisplayHalDriver->displayPort, 0, sizeof(DISPLAY_HAL_PIN_STRUCT)*HD44780_DISPLAY_PIN_COUNT);
	
		//set display driver port config
		for (i = is4BitMode*4; i < HD44780_DISPLAY_PIN_COUNT; i++)
		{
				newDisplayHalDriver->displayPort[i].portAddress = GetPortAddressFromLetter(portTemplate[i*2]);
				newDisplayHalDriver->displayPort[i].pinIndex = portTemplate[i*2 + 1];
				if ( ( !newDisplayHalDriver->displayPort[i].portAddress ) ||
						 ( newDisplayHalDriver->displayPort[i].pinIndex > (GPIO_MAX_PIN_COUNT - 1) ) )
				{
						//set config error, clear and exit
						STM32FX_DISPLAY_HAL_FREE_MEMORY(newDisplayHalDriver->displayPort);
						STM32FX_DISPLAY_HAL_FREE_MEMORY(newDisplayHalDriver);
						return DH_INPUT_DATA_ERROR;
				}
				STM32FXSetGPIOPower(portTemplate[i*2], 1); 
		}
		
		//set display driver config
		newDisplayHalDriver->displayType = DISPLAY_HAL_TYPE_HD44780;
		newDisplayHalDriver->displayPinCount = HD44780_DISPLAY_PIN_COUNT;
		newDisplayHalDriver->waitMS = waitMS;
		newDisplayHalDriver->waitUS = waitUS;
		
		//return
		*displayHal = newDisplayHalDriver;
		return DH_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* init hal driver for MT12864 display */
DISPLAY_HAL_RESULT MT12864HalDriverInit(uint8_t *portTemplate, void (*waitUS)(uint16_t timeUS),
                                        void (*waitNS)(uint16_t timeNS), DISPLAY_HAL** displayHal)
{
		DISPLAY_HAL* newDisplayHalDriver;
		uint8_t i;
	
		//check input data
		if ( !portTemplate ) return DH_INPUT_DATA_ERROR;
		if ( !waitUS ) return DH_INPUT_DATA_ERROR;
		if ( !waitNS ) return DH_INPUT_DATA_ERROR;
		if ( !displayHal ) return DH_INPUT_DATA_ERROR;
		if ( *displayHal ) return DH_DRIVER_NOT_EMPTY;
	
		//get memory for main display hal struct, check and clear
		newDisplayHalDriver = (DISPLAY_HAL*)STM32FX_DISPLAY_HAL_GET_MEMORY(sizeof(DISPLAY_HAL));
		if ( !newDisplayHalDriver ) return DH_MEMORY_ERROR;
		memset(newDisplayHalDriver, 0, sizeof(DISPLAY_HAL));
		
		//get memory for display pin struct config, check and clear
		newDisplayHalDriver->displayPort =
				(DISPLAY_HAL_PIN_STRUCT*)STM32FX_DISPLAY_HAL_GET_MEMORY(sizeof(DISPLAY_HAL_PIN_STRUCT)*MT12864_DISPLAY_PIN_COUNT);
		if ( !newDisplayHalDriver->displayPort ) return DH_MEMORY_ERROR;
		memset(newDisplayHalDriver->displayPort, 0, sizeof(DISPLAY_HAL_PIN_STRUCT)*MT12864_DISPLAY_PIN_COUNT);
	
		//set display driver port config
		for (i = 0; i < MT12864_DISPLAY_PIN_COUNT; i++)
		{
				newDisplayHalDriver->displayPort[i].portAddress = GetPortAddressFromLetter(portTemplate[i*2]);
				newDisplayHalDriver->displayPort[i].pinIndex = portTemplate[i*2 + 1];
				if ( ( !newDisplayHalDriver->displayPort[i].portAddress ) ||
						 ( newDisplayHalDriver->displayPort[i].pinIndex > (GPIO_MAX_PIN_COUNT - 1) ) )
				{
						//set config error, clear and exit
						STM32FX_DISPLAY_HAL_FREE_MEMORY(newDisplayHalDriver->displayPort);
						STM32FX_DISPLAY_HAL_FREE_MEMORY(newDisplayHalDriver);
						return DH_INPUT_DATA_ERROR;
				}
				STM32FXSetGPIOPower(portTemplate[i*2], 1);
		}
		
		//set display driver config
		newDisplayHalDriver->displayType = DISPLAY_HAL_TYPE_MT12864;
		newDisplayHalDriver->displayPinCount = MT12864_DISPLAY_PIN_COUNT;
		newDisplayHalDriver->waitUS = waitUS;
		newDisplayHalDriver->waitNS = waitNS;
	
		//return
		*displayHal = newDisplayHalDriver;
		return DH_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* init hal driver for ST7920 display */
DISPLAY_HAL_RESULT ST7920HalDriverInit(uint8_t *portTemplate, void (*waitUS)(uint16_t timeUS),
                                        void (*waitNS)(uint16_t timeNS), DISPLAY_HAL** displayHal)
{
		DISPLAY_HAL* newDisplayHalDriver;
		uint8_t i;
	
		//check input data
		if ( !portTemplate ) return DH_INPUT_DATA_ERROR;
		if ( !waitUS ) return DH_INPUT_DATA_ERROR;
		if ( !waitNS ) return DH_INPUT_DATA_ERROR;
		if ( !displayHal ) return DH_INPUT_DATA_ERROR;
		if ( *displayHal ) return DH_DRIVER_NOT_EMPTY;
	
		//get memory for main display hal struct, check and clear
		newDisplayHalDriver = (DISPLAY_HAL*)STM32FX_DISPLAY_HAL_GET_MEMORY(sizeof(DISPLAY_HAL));
		if ( !newDisplayHalDriver ) return DH_MEMORY_ERROR;
		memset(newDisplayHalDriver, 0, sizeof(DISPLAY_HAL));
		
		//get memory for display pin struct config, check and clear
		newDisplayHalDriver->displayPort =
				(DISPLAY_HAL_PIN_STRUCT*)STM32FX_DISPLAY_HAL_GET_MEMORY(sizeof(DISPLAY_HAL_PIN_STRUCT)*ST7920_DISPLAY_PIN_COUNT);
		if ( !newDisplayHalDriver->displayPort ) return DH_MEMORY_ERROR;
		memset(newDisplayHalDriver->displayPort, 0, sizeof(DISPLAY_HAL_PIN_STRUCT)*ST7920_DISPLAY_PIN_COUNT);
	
		//set display driver port config
		for (i = 0; i < ST7920_DISPLAY_PIN_COUNT; i++)
		{
				newDisplayHalDriver->displayPort[i].portAddress = GetPortAddressFromLetter(portTemplate[i*2]);
				newDisplayHalDriver->displayPort[i].pinIndex = portTemplate[i*2 + 1];
				if ( ( !newDisplayHalDriver->displayPort[i].portAddress ) ||
						 ( newDisplayHalDriver->displayPort[i].pinIndex > (GPIO_MAX_PIN_COUNT - 1) ) )
				{
						//set config error, clear and exit
						STM32FX_DISPLAY_HAL_FREE_MEMORY(newDisplayHalDriver->displayPort);
						STM32FX_DISPLAY_HAL_FREE_MEMORY(newDisplayHalDriver);
						return DH_INPUT_DATA_ERROR;
				}
				STM32FXSetGPIOPower(portTemplate[i*2], 1); 
		}
		
		//set display driver config
		newDisplayHalDriver->displayType = DISPLAY_HAL_TYPE_ST7920;
		newDisplayHalDriver->displayPinCount = ST7920_DISPLAY_PIN_COUNT;
		newDisplayHalDriver->waitUS = waitUS;
		newDisplayHalDriver->waitNS = waitNS;
	
		//return
		*displayHal = newDisplayHalDriver;
		return DH_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* destroy display hal driver */										
DISPLAY_HAL_RESULT DisplayHalDriverDestroy(DISPLAY_HAL** displayHal)
{
		//check input data
		if ( !displayHal ) return DH_INPUT_DATA_ERROR;
		if ( !*displayHal ) return DH_INPUT_DATA_ERROR;

		//free display port
		STM32FX_DISPLAY_HAL_FREE_MEMORY((*displayHal)->displayPort);
		(*displayHal)->displayPort = 0;
		//free display hal driver struct
		STM32FX_DISPLAY_HAL_FREE_MEMORY(*displayHal);
		*displayHal = 0; 
	
		return DH_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* config display IO pin */
DISPLAY_HAL_RESULT DisplayHalDriverConfigPin(DISPLAY_HAL* displayHal, uint8_t pinName, STM32FX_DISPLAY_HAL_PIN_TYPE pinType)
{
		//check input data
		if ( !checkDisplayHalDriverConfig(displayHal) ) return DH_INPUT_DATA_ERROR;
		if ( pinName > (displayHal->displayPinCount - 1) ) return DH_INPUT_DATA_ERROR;
		if ( !displayHal->displayPort[pinName].portAddress ) return DH_INPUT_DATA_ERROR;
		if ( displayHal->displayPort[pinName].pinIndex > (GPIO_MAX_PIN_COUNT - 1) ) return DH_INPUT_DATA_ERROR;

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
						return DH_INPUT_DATA_ERROR;
				break;
		}
	
		//return
		return DH_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* write level 1 or 0 to display IO pin */
DISPLAY_HAL_RESULT DisplayHalWritePinLevel(DISPLAY_HAL* displayHal, uint8_t pinName, uint8_t pinLevel)
{
		//check input data
		if ( !checkDisplayHalDriverConfig(displayHal) ) return DH_INPUT_DATA_ERROR;
		if ( pinName > (displayHal->displayPinCount - 1) ) return DH_INPUT_DATA_ERROR;
		if ( (pinLevel != 0) && (pinLevel != 1) ) return DH_INPUT_DATA_ERROR;
	
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
		
		//return
		return DH_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* read display IO pin level */
DISPLAY_HAL_RESULT DisplayHalReadPinLevel(DISPLAY_HAL* displayHal, uint8_t pinName, uint8_t* pinLevel)
{
		//check input data
		if ( !checkDisplayHalDriverConfig(displayHal) ) return DH_INPUT_DATA_ERROR;
		if ( pinName > (displayHal->displayPinCount - 1) ) return DH_INPUT_DATA_ERROR;
		if ( !pinLevel ) return DH_INPUT_DATA_ERROR;
	
		//read pin level
		*pinLevel = GPIOReadPinP(displayHal->displayPort[pinName].portAddress, displayHal->displayPort[pinName].pinIndex);
	
		//return
		return DH_OK;
}
//---------------------------------------------------------------------------------------------------------//

//*************************************end of stm32fx_display_hal.c****************************************//
