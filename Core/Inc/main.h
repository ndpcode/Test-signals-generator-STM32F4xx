//****************************************************************************//
//The generator 407 Firmware project
//main header file
//Created 09.08.2018
//Created by Novikov Dmitry
//****************************************************************************//

#ifndef __MAIN_H
#define __MAIN_H

/* board mcu name */
#define MCU_TYPE "STM32F407VGT6"

/* board mcu frequency */
#define MCU_FREQ 168000000UL

/* AD9850 clock frequency */
#define AD9850_CLOCK 125000000UL
 
/* board display type */
#define BOARD_DISPLAY_MT12864
//#define BOARD_DISPLAY_ST7920
/* board encoder type */
//#define BOARD_ENCODER_INVERSE
/* board Memory type */
#define BOARD_MEMORY_FLASH
//#define BOARD_MEMORY_EEPROM
//****************************************************************************//
/* defines and constants -----------------------------------------------------*/
#define RTC_ASYNCH_PRESCALER 124U
#define RTC_SYNCH_PRESCALER 3999U

//****************************************************************************//
/* compatibility arm compiler 5 to 6 */
#define __weak __attribute__((weak))
//#define __packed __attribute__((packed))
#define __packed

//****************************************************************************//
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "spi_eeprom.h"

//****************************************************************************//
/* Private define ------------------------------------------------------------*/

//****************************************************************************//
/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

//****************************************************************************//
#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

//*********************************************************************************************************//
/* enumeration lists */

/* controls events */
typedef enum
{
		EVENT_BUTTON_UP_CLICK         = 0x00000001,
		EVENT_BUTTON_DOWN_CLICK       = 0x00000002,
		EVENT_BUTTON_LEFT_CLICK       = 0x00000004,
		EVENT_BUTTON_RIGHT_CLICK      = 0x00000008,
		EVENT_BUTTON_OK_CLICK         = 0x00000010,
		EVENT_BUTTON_AUTO_CLICK       = 0x00000020,
		EVENT_BUTTON_F1_CLICK         = 0x00000040,
		EVENT_BUTTON_F2_CLICK         = 0x00000080,
		EVENT_BUTTON_F3_CLICK         = 0x00000100,
		EVENT_BUTTON_F4_CLICK         = 0x00000200,
		EVENT_BUTTON_MENU_CLICK       = 0x00000400,	
		EVENT_VALCODER_BUTTON_CLICK   = 0x00000800,
		EVENT_VALCODER_BUTTON_PRESSED = 0x00001000,
		EVENT_VALCODER_CCW            = 0x00002000,
		EVENT_VALCODER_CW             = 0x00004000		
} CONTROLS_EVENTS;

typedef struct 
{
	uint32_t Dat_1_8;
	uint32_t Dat_2_16;
	uint32_t Dat_3_32;
	uint32_t Dat_4_32;
	uint32_t Dat_5_32;
	uint32_t Dat_6_32;
	uint32_t Dat_7_32;
	uint32_t Dat_8_32;
	
	uint32_t Dat_9_32;
	uint32_t Dat_10_32;
	uint32_t Dat_11_32;
	uint32_t Dat_12_32;
	uint32_t Dat_13_32;
	uint32_t Dat_14_32;
	uint32_t Dat_15_32;
	uint32_t Dat_16_32;
	
	uint32_t Dat_17_32;
	uint32_t Dat_18_32;
	uint32_t Dat_19_32;
	uint32_t Dat_20_32;
	uint32_t Dat_21_32;
	uint32_t Dat_22_32;
	uint32_t Dat_23_32;
	uint32_t Dat_24_32;
	
}TestVarStruct_ee;

#endif /* __MAIN_H */

//****************************************************************************//
//******************************end of main.h*********************************//
