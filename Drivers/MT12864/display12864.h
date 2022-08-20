//*********************************************************************************************************//
//Display 128x64 library
//Created 15.08.2018
//Created by Novikov Dmitry
//*********************************************************************************************************//

#ifndef __DISPLAY12864_H
#define __DISPLAY12864_H

//*********************************************************************************************************//
/* includes */
#include "stm32fx_display_hal.h"

//*********************************************************************************************************//
/* defines, constants and macro */

/* set driver mode, uncomment target mode ******************************************************************/
//#define D128X64_MODE_NORMAL
#define D128X64_MODE_FAST
#define D128X64_DEFAULT_INPUT_TYPE DISPLAY_PIN_TYPE_INPUT
#define D128X64_DEFAULT_OUTPUT_TYPE DISPLAY_PIN_TYPE_OUTPUT_PP
#define D128X64_DEFAULT_FLOATING_TYPE DISPLAY_PIN_TYPE_FLOATING

/* memory alloc macro set **********************************************************************************/
#define DISPLIB_GET_MEMORY(memSize) STM32FX_DISPLAY_HAL_GET_MEMORY(memSize)
#define DISPLIB_FREE_MEMORY(memPointer) STM32FX_DISPLAY_HAL_FREE_MEMORY(memPointer)

/* display size ********************************************************************************************/
#define D128X64_WIDTH 128U
#define D128X64_HEIGHT 64U

/* display buffer size *************************************************************************************/
#define D128X64_BUF_COLS 128U
#define D128X64_BUF_ROWS 8U

/* display MT12864 config **********************************************************************************/
/* display MT12864 constants */
#define MT12864_MODE_COMMAND 0
#define MT12864_MODE_DATA 1
#define MT12864_CHIP_1 0
#define MT12864_CHIP_2 1
/* display MT12864 timings */
#define MT12864_PAUSE_BEFORE_PULSE 80U // ns
#define MT12864_PAUSE_AFTER_PULSE  200U // ns
#define MT12864_PULSE_TIME         200U // ns

/* display ST7920 config **********************************************************************************/
/* basic command set */
#define ST7920_CMDBASIC_CLEAR                   0x01    // Очистка DDRAM
#define ST7920_CMDBASIC_HOME                    0x02    // Перевод курсора в начало дисплея
#define ST7920_CMDBASIC_ENTRYMODE               0x04    // Параметры автосдвига курсора и экрана
#define ST7920_CMDBASIC_DISPLAYONOFF            0x08    // Управление дисплеем и курсором
#define ST7920_CMDBASIC_CURSORDISPLAYCONTROL    0x10    // 
#define ST7920_CMDBASIC_FUNCTIONSET             0x20    // Выбор битности интерфейса и управление Extended Mode 
#define ST7920_CMDBASIC_SETCGRAMADDR            0x40    // Установка адреса в CGRAM 
#define ST7920_CMDBASIC_SetDDRAMaddr            0x80    // Установка адреса в DDRAM
/* extended command set */
#define ST7920_CMDEXT_STANDBY                   0x01    // Перевод в режим StandBy
#define ST7920_CMDEXT_SELSCROLLORRAMADDR        0x02    // Выбор скрола либо адреса в памяти
#define ST7920_CMDEXT_REVERSE                   0x04    // Реверс одной из 4 строк в DDRAM
#define ST7920_CMDEXT_FUNCTIONSET               0x20    // Выбор битности интерфейса, управление Extended Mode и Graphic Mode
#define ST7920_CMDEXT_SETIRAMORSCROLLADDR       0x40    // Установка адреса в IRAM или сдвиг скролла
#define ST7920_CMDEXT_SETCGRAMADDR              0x80    // Установка адреса в GDRAM (память графического режима)
/* display ST7920 timings */
#define ST7920_PAUSE_BEFORE_PULSE 800U // ns
#define ST7920_PAUSE_AFTER_PULSE  19000U // ns
#define ST7920_PULSE_TIME         9000U // ns

//*********************************************************************************************************//
/* enumeration lists */

/* display type ********************************************************************************************/
typedef enum
{
		DTYPE_MT12864 = 1,
		DTYPE_ST7920		
} D128X64_TYPE;

/* display 128x64 function result enum *********************************************************************/
typedef enum
{
		D128X64_OK = 0,
		D128X64_ERROR,
		D128X64_BUSY,
		D128X64_INPUT_DATA_ERROR,
		D128X64_DRIVER_NOT_EMPTY,
		D128X64_MEMORY_ERROR
} D128X64_RESULT;

//*********************************************************************************************************//
/* struct declarations */

/* display 128x64 RAM buffer *******************************************************************************/
typedef struct
{
		uint8_t raw[D128X64_BUF_COLS][D128X64_BUF_ROWS];
} D128X64_BUFFER;

/* display 128x64 main config ******************************************************************************/
typedef struct
{
		D128X64_TYPE displayType;
		DISPLAY_HAL* displayHal;
} D128X64_DISPLAY;

//*********************************************************************************************************//
/* display 128x64 main functions ***************************************************************************/
D128X64_RESULT MT12864DisplayInit(uint8_t *portTemplate, void (*waitUS)(uint16_t timeUS),
                                  void (*waitNS)(uint16_t timeNS), D128X64_DISPLAY** display128x64);
D128X64_RESULT ST7920DisplayInit(uint8_t *portTemplate, void (*waitUS)(uint16_t timeUS),
                                  void (*waitNS)(uint16_t timeNS), D128X64_DISPLAY** display128x64);
D128X64_RESULT D128X64DisplayDestroy(D128X64_DISPLAY** display128x64);
D128X64_RESULT D128X64DisplayReset(D128X64_DISPLAY* display128x64);
D128X64_RESULT D128X64DisplayWritePoint(D128X64_DISPLAY* display128x64, uint8_t posX, uint8_t posY, uint8_t pointState);
D128X64_RESULT D128X64WriteBuf(D128X64_DISPLAY* display128x64, D128X64_BUFFER* buffer);
D128X64_RESULT MT12864DemoLogoWrite(D128X64_DISPLAY* display128x64);

#endif
//*****************************************end of display12864.h******************************************//
