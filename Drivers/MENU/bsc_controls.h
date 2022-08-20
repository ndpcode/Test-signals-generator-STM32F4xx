//*********************************************************************************************************//
//stm32fxxx controls driver
//Created 02.02.2017
//Created by Novikov Dmitry
//*********************************************************************************************************//
#ifndef __BSC_CONTROLS_H
#define __BSC_CONTROLS_H

//*********************************************************************************************************//
/* includes */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "gpio.h"

//*********************************************************************************************************//
/* defines, constatnts and macro */
#define VALCODER_ERROR 0x0F
#define SYS_EVENTS_DATA uint32_t

/* memory alloc macro set */
#define STM32FX_CONTROLS_GET_MEMORY(memSize) malloc(memSize)
#define STM32FX_CONTROLS_FREE_MEMORY(memPointer) free(memPointer)

//*********************************************************************************************************//
/* enumeration lists */
typedef enum
{
		UPDEVENTS_GETEVENTS  = 1,
		UPDEVENTS_HANDLERS_LAUNCH = 2
} updateEventsParams;

//*********************************************************************************************************//
/* struct declarations */
typedef struct 
{
		GPIO_TypeDef* portAddress;
		uint8_t pinNumber;
		void ( *controlClickHandler )(void);
		unsigned controlBusyFlag : 1;
		SYS_EVENTS_DATA eventCodeClick;
		SYS_EVENTS_DATA eventCodePressed;
		void *nextControlElement;
} controlsBaseStruct;

//*********************************************************************************************************//
/* stm32fx controls functions */
uint8_t controlsDataClear(void);
uint8_t controlsRegNewButton(char portLetter, uint8_t pinNumber, void (*controlClickHandler)(void),
                             SYS_EVENTS_DATA eventClickCode, SYS_EVENTS_DATA eventPressedCode);
uint8_t controlsRegNewValcoder(char portLetterCCW, uint8_t pinNumberCCW,
	                             char portLetterCW, uint8_t pinNumberCW,
															 void (*ccwHandler)(void), SYS_EVENTS_DATA eventCodeCCW,
															 void (*cwHandler)(void), SYS_EVENTS_DATA eventCodeCW);																 
SYS_EVENTS_DATA controlsUpdateAllEvents(uint32_t params);
SYS_EVENTS_DATA controlsUpdateButtons(uint32_t params);
SYS_EVENTS_DATA controlsUpdateValcoder(uint32_t params);

#endif
