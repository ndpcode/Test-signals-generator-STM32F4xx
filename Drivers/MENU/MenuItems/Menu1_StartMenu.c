//*********************************************************************************************************//
//generator 407 Menu1 - StartMenu
//Created 18.08.2018
//Created by Novikov Dmitry
//*********************************************************************************************************//

//*********************************************************************************************************//
/* includes */
#include "main.h"
#include "rtc.h"
#include "main_menu.h"
#include "math.h"

//*********************************************************************************************************//
/* defines and constants */
#define __rtc_reg_into_num(_reg, _tens_mask, _tens_pos, _units_mask, _units_pos)\
						              ( ( ( ( _reg & _tens_mask ) >> _tens_pos ) * 10 ) +\
							            ( ( _reg & _units_mask ) >> _units_pos ) )
#define pi 3.14159

//*********************************************************************************************************//
/* Private variables --------------------------------------------------------------------------------------*/
extern TREE_MENU* mainMenu;
extern D128X64_DISPLAY* Display128x64;
extern D128X64_BUFFER displayBuffer;
extern URG_CANVAS displayCanvas;
URG_FONT_CONFIG fontConfig;
extern RTC_HandleTypeDef hrtc;
extern SYS_EVENTS_DATA valcoderEvents; //external valcoder events handler struct
extern uint8_t valcoderEventsBlock;    //flag - block/unblock change valcoderEvents variable

static char numberStr[20] = "";
static uint8_t color = 1;
volatile static uint32_t _frames_counter = 0, _time_counter = 0, _fps = 0;

//*********************************************************************************************************//
/* Menu1 functions */

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t Menu1_StartMenu_Draw(const uint8_t frameNum)
{
		uint32_t tmpreg = 0U;
		uint8_t Hours, Minutes, Seconds;
		uint8_t i, x, y;
	
		//get time
		/* Get the TR register */
		tmpreg = (uint32_t)RTC->TR;  
		/* Fill the structure fields with the read parameters */
		Hours = (uint8_t)__rtc_reg_into_num(tmpreg, RTC_TR_HT, RTC_TR_HT_Pos, RTC_TR_HU, RTC_TR_HU_Pos);
		Minutes = (uint8_t)__rtc_reg_into_num(tmpreg, RTC_TR_MNT, RTC_TR_MNT_Pos, RTC_TR_MNU, RTC_TR_MNU_Pos);
		Seconds = (uint8_t)__rtc_reg_into_num(tmpreg, RTC_TR_ST, RTC_TR_ST_Pos, RTC_TR_SU, RTC_TR_SU_Pos);
		tmpreg = (uint32_t)RTC->DR;	
		
		//calc FPS
		if ( _time_counter != Seconds )
		{
				_fps = _frames_counter;
				_time_counter = Seconds;
				_frames_counter = 0;
		}
		_frames_counter++;
	
		//clear display buffer
		memset(displayBuffer.raw, color - 1, DISPLAY_BUFFER_SIZE);
	
		//set font config
		fontConfig.symbolWidth = 11;
		fontConfig.symbolHeight = 12;
		fontConfig.centerAlignment = 0;
		fontConfig.textSpacing = 1;
		fontConfig.fontRaw = (uint8_t*)&Calibri11x12[0];
		
		//set canvas
		displayCanvas.color.color1B = color;
		displayCanvas.buffer->raw = (uint32_t*)displayBuffer.raw;
		
		//draw clock
		//circle
		URGDrawCircleInBufFast(64, 32, 31, 1, &displayCanvas);
		//points
		for (i = 1; i < 13; i++)
		{
				x = 62 + 26*cos(pi/2 - i*pi/6);
				y = 37 - 26*sin(pi/2 - i*pi/6);
				snprintf(numberStr, 19, "%d", i);
				URGDrawTextInBufFast(&fontConfig, (uint8_t*)numberStr, x, y, &displayCanvas);			
		}
		//hours
		x = 64 + 12*cos(pi/2 - (Hours - Hours/12*12)*pi/6);
		y = 32 - 12*sin(pi/2 - (Hours - Hours/12*12)*pi/6);
		URGDrawLineXYInBufFast(64, 32, x, y, 3, &displayCanvas);
		//minutes
		x = 64 + 17*cos(pi/2 - Minutes*pi/30);
		y = 32 - 17*sin(pi/2 - Minutes*pi/30);
		URGDrawLineXYInBufFast(64, 32, x, y, 2, &displayCanvas);
		//seconds
		x = 64 + 20*cos(pi/2 - Seconds*pi/30);
		y = 32 - 20*sin(pi/2 - Seconds*pi/30);
		URGDrawLineXYInBufFast(64, 32, x, y, 1, &displayCanvas);
	

		//write FPS
		snprintf(numberStr, 19, "FPS=%d", _fps);
		URGDrawTextInBufFast(&fontConfig, (uint8_t*)numberStr, 0, 64, &displayCanvas);
		//text
		snprintf(numberStr, 19, "%02d:%02d:%02d", Hours, Minutes, Seconds);
		URGDrawTextInBufFast(&fontConfig, (uint8_t*)numberStr, 0, 12, &displayCanvas);
		//snprintf(numberStr, 19, "%d", frameNum);
		//MT12864DrawTextInBufFast(&fontConfig, (uint8_t*)numberStr, 20, 40, 1, &displayBuffer);
	
		//write buf to display
		D128X64WriteBuf(Display128x64, &displayBuffer);		
		
		return 0;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t Menu1_StartMenu_Events(const uint16_t frameNum, SYS_EVENTS_DATA sysEvents)
{
		RTC_TimeTypeDef sTime;
		
		//block valcoder events upd
		valcoderEventsBlock = 1;
	
		// event - button F1 click
		if ( sysEvents & EVENT_BUTTON_F1_CLICK )
		{
				color = 1;
		}
		
		// event - button F2 click
		if ( sysEvents & EVENT_BUTTON_F2_CLICK )
		{
				color = 0;
		}
		// event - button Right click
		if ( sysEvents & EVENT_BUTTON_RIGHT_CLICK )
		{
				MenuGoToNextItem(mainMenu);
		}
		// event - MENU click
		if ( sysEvents & EVENT_BUTTON_MENU_CLICK )
		{
				MenuGoToItemId(mainMenu, Menu2_MainMenu_ID);
		}
		
		// event - valcoder CCW
		if ( valcoderEvents & EVENT_VALCODER_CCW )
		{
				valcoderEvents &= ~EVENT_VALCODER_CCW;
				HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				if ( !sTime.Minutes )
				{
						if ( !sTime.Hours )
						{
								sTime.Hours = 23;
								sTime.Minutes = 59;
						} else
						{
								sTime.Hours--;
								sTime.Minutes = 59;
						}
				} else
				{
						sTime.Minutes--;						
				}
				HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		}		
		
		// event - valcoder CW
		if ( valcoderEvents & EVENT_VALCODER_CW )
		{
				valcoderEvents &= ~EVENT_VALCODER_CW;
				HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
				if ( sTime.Minutes == 59 )
				{
						if ( sTime.Hours == 23 )
						{
								sTime.Hours = 0;
								sTime.Minutes = 0;
						} else
						{
								sTime.Hours++;
								sTime.Minutes = 0;
						}
				} else
				{
						sTime.Minutes++;						
				}
				HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		}
		
		//unblock valcoder events upd
		valcoderEventsBlock = 0;
		
		return 0;
}
//---------------------------------------------------------------------------------------------------------//

//*************************************end of Menu1_StartMenu.c****************************************//
