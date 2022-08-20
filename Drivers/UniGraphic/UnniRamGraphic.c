//*********************************************************************************************************//
//UnniRamGraphic library, source file
//draw different grapics objects into raw buffer in RAM
//Created 04.09.2018
//Created by Novikov Dmitry
//*********************************************************************************************************//

//*********************************************************************************************************//
/* includes */
#include "UnniRamGraphic.h"

//*********************************************************************************************************//
/* unni ram graphic functions */

/* main unni ram graphic functions *************************************************************************/

//---------------------------------------------------------------------------------------------------------//
/* create unni ram graphic buffer */
URG_RESULT URGAllocRAMBuffer(uint32_t screenWidth, uint32_t screenHeight, URG_BUF_DATA_FORMAT dataFormat, URG_RAW_BUFFER** buffer)
{	
		//check input data
		//...buffer pointer
		if ( !buffer ) return URG_INPUT_DATA_ERROR;
		if ( *buffer ) return URG_INPUT_DATA_ERROR;
		//...screen resolution
		if ( (!screenWidth) || (!screenHeight) ) return URG_INPUT_DATA_ERROR;
		if ( ((dataFormat == DATA_FORMAT_1BIT_OX) || (dataFormat == DATA_FORMAT_1BIT_OY )) &&
				 ((screenWidth*screenHeight)%8) ) return URG_INVALID_CANVAS_RESOLUTION;
	
		//get memory for buffer struct
		*buffer = (URG_RAW_BUFFER*)URG_GET_MEMORY(sizeof(URG_RAW_BUFFER));
		if ( !(*buffer) ) return URG_MEMORY_ERROR;
		(*buffer)->dataFormat = dataFormat;
		(*buffer)->raw = 0;
		
		//choice buffer type
		switch (dataFormat)
		{
				case DATA_FORMAT_1BIT_OY:
						//get memory for screenWidth x screenHeight x 1bit
						(*buffer)->raw = (uint32_t*)URG_GET_MEMORY(screenWidth*screenHeight/8);
						if ( !(*buffer)->raw )
						{
								URG_FREE_MEMORY(*buffer);
								return URG_MEMORY_ERROR;
						}					
				break;
					
				case DATA_FORMAT_1BIT_OX:
						return URG_ERROR;
			
				case DATA_FORMAT_UNK:
						return URG_ERROR;	
			
				default:
						return URG_INPUT_DATA_ERROR;			
		}
		
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* unni ram graphic free buffer */
URG_RESULT URGFreeRAMBuffer(URG_RAW_BUFFER** buffer)
{
		//check input data
		//...buffer pointer
		if ( !buffer ) return URG_INPUT_DATA_ERROR;
		if ( !*buffer ) return URG_INPUT_DATA_ERROR;
	
		//free
		if ( (*buffer)->raw )
		{
				URG_FREE_MEMORY((*buffer)->raw);			
		}
		URG_FREE_MEMORY(*buffer);
		*buffer = 0;
	
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

/* 2D unni ram graphic functions ***************************************************************************/

//---------------------------------------------------------------------------------------------------------//
/**/
void URGDrawPointXYInBufFast(int32_t posX, int32_t posY, URG_CANVAS* canvas)
{		
		URG_COLOR pointColor;
	
		//check only critical data
		if (!canvas) return;
		if ( ( posX < 0 ) || ( posX >= canvas->screenWidth ) || ( posY < 0 ) || ( posY >= canvas->screenHeight ) ) return;
		
		//choice canvas buffer type
		switch (canvas->buffer->dataFormat)
		{
				case DATA_FORMAT_1BIT_OY:
						pointColor.color1B = canvas->color.color1B;
						if ( canvas->colorMode == COLOR_WB_1BIT_INVERSE )
						{
								pointColor.color1B = (~canvas->color.color1B) & 0x01;								
						}
						if ( pointColor.color1B )
						{
								//set point to 1
								*((uint8_t*)canvas->buffer->raw + (posX*8 + posY/8)) |= 1 << (posY & 0x07);
						} else
						{
								//set point to 0 (reset)
								*((uint8_t*)canvas->buffer->raw + (posX*8 + posY/8)) &= ~(1 << (posY & 0x07));		
						}							
				break;
					
				case DATA_FORMAT_1BIT_OX:
						return;
			
				case DATA_FORMAT_UNK:
						return;
			
				default:
						return;			
		}
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/**/
void URGDrawLineXYInBufFast(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t thickness, URG_CANVAS* canvas)
{
		int32_t i;
		if ( (x1 > x2) || (y1 > y2) )
		{
				i = x1;
				x1 = x2;
				x2 = i;
				i = y1;
				y1 = y2;
				y2 = i;
		}
    int32_t deltaX = abs(x2 - x1);
    int32_t deltaY = abs(y2 - y1);
    int32_t signX = x1 < x2 ? 1 : -1;
    int32_t signY = y1 < y2 ? 1 : -1;
    int32_t error = deltaX - deltaY;
		int32_t error2 = 0;
		
		URGDrawPointXYInBufFast(x2, y2, canvas);
		while (1)
		{
				if ( deltaX > deltaY )
				{
						for (i = y1 - thickness/2; i < y1; i++)
						{
								URGDrawPointXYInBufFast(x1, i, canvas);
						}
						for (i = y1; i < y1 + (thickness+1)/2; i++)
						{
								URGDrawPointXYInBufFast(x1, i, canvas);
						}						
				} else
				{
						for (i = x1 - thickness/2; i < x1; i++)
						{
								URGDrawPointXYInBufFast(i, y1, canvas);
						}
						for (i = x1; i < x1 + (thickness+1)/2; i++)
						{
								URGDrawPointXYInBufFast(i, y1, canvas);
						}
				}
				if ( (x1 == x2) && (y1 == y2) ) break;
        error2 = error * 2;
        if (error2 > -deltaY) 
        {
						error -= deltaY;
            x1 += signX;
        }
				if (error2 < deltaX)
				{
            error += deltaX;
            y1 += signY;
        }
    }
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/**/
void URGDrawRectInBufFast(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t thickness, URG_CANVAS* canvas)
{
		//draw 4 sides
		URGDrawLineXYInBufFast(x1, y1, x2, y1, thickness, canvas);
		URGDrawLineXYInBufFast(x2, y1, x2, y2, thickness, canvas);
		URGDrawLineXYInBufFast(x2, y2, x1, y2, thickness, canvas);
		URGDrawLineXYInBufFast(x1, y2, x1, y1, thickness, canvas);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/**/
void URGDrawFilledRectInBufFast(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, URG_CANVAS* canvas)
{
		int32_t x, y;
	
		//coordinates control
		if (x1 > x2)
		{
				x = x1;
				x1 = x2;
				x2 = x;
		}
		if (y1 > y2)
		{
				y = y1;
				y1 = y2;
				y2 = y;
		}
		//fill rect
		for (x = x1; x <= x2; x++)
		{
				for (y = y1; y <= y2; y++)
				{
						URGDrawPointXYInBufFast(x, y, canvas);						
				}
		}		
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/**/
void URGDrawCircleInBufFast(uint8_t posX, uint8_t posY, uint8_t radius, uint8_t thickness, URG_CANVAS* canvas)
{
		uint8_t i;
		int32_t x = 0;
    int32_t y = radius;
    int32_t delta = 2 - 2*radius;
    int32_t error = 0;
    while (y >= 0)
		{
				for (i = 0; i < thickness; i++)
				{
						URGDrawPointXYInBufFast(posX + x + i, posY + y + i, canvas);
						URGDrawPointXYInBufFast(posX + x + i, posY - y - i, canvas);
						URGDrawPointXYInBufFast(posX - x - i, posY + y + i, canvas);
						URGDrawPointXYInBufFast(posX - x - i, posY - y - i, canvas);
				}
				error = 2*(delta + y) - 1;
				if ( (delta < 0) && (error <= 0) )
				{
						++x;
            delta += 2*x + 1;
            continue;
        }
				error = 2*(delta - x) - 1;
        if ( (delta > 0) && (error > 0) )
				{
						--y;
            delta += 1 - 2*y;
            continue;
        }
        ++x;
        delta += 2*(x - y);
        --y;
    }
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/**/
void URGDrawTextInBufFast(URG_FONT_CONFIG* fontConfig, uint8_t* text, uint8_t posX, uint8_t posY, URG_CANVAS* canvas)
{
		//variables
		uint8_t x,y;
		uint32_t cX, cY, dX;
		uint8_t oneChar;
		//calculate bytes for one character column and total bytes per character
		uint8_t bytesPerColumn = fontConfig->symbolHeight / 9 + 1;
		uint8_t bytesPerCharacter, offsetPerCharacter;
		//calculate bytes per character in font raw array
		bytesPerCharacter = fontConfig->symbolWidth * bytesPerColumn + 1;
		//calculate offset per character on screen
		offsetPerCharacter = bytesPerCharacter - 1;
		//reset delta X
		dX = 0;
		while (*text)
		{
				//copy symbol to variable
				oneChar = *text;
				//set start point for standard and russian characters
				if ( oneChar < 0x80 )
				{
						//start point for standard characters
						oneChar -= 0x20;
				} else
				{
						//start point for russian charcters
						oneChar -= 0x60;
				}
				//get offset per character count, if not center alignment
				if ( !fontConfig->centerAlignment )
				{
						offsetPerCharacter = fontConfig->fontRaw[((uint32_t)fontConfig->symbolWidth * bytesPerColumn + 1)*oneChar] * bytesPerColumn;
				}
				//
				for (x = 0; x < offsetPerCharacter; x++)
				{
						//calculate point coordinate X on display
						cX = posX + dX + x / bytesPerColumn;
						for (y = 0; y < 8; y++)
						{
								//calculate point coordinate Y on display
								cY = posY - fontConfig->symbolHeight + 1 + (x % bytesPerColumn)*8 + y;
								//draw point of character if not empty
								if ( (fontConfig->fontRaw[(uint32_t)bytesPerCharacter * oneChar + x + 1]) & (1 << y) )
								{
										URGDrawPointXYInBufFast(cX, cY, canvas);								
								}
						}
				}
				//text spacing calculate
				if ( fontConfig->centerAlignment )
				{
						//if character center aligned
						dX += fontConfig->symbolWidth + fontConfig->textSpacing;
				} else
				{
						dX += fontConfig->fontRaw[(uint32_t)bytesPerCharacter * oneChar] + fontConfig->textSpacing;
				}
				text++;				
		}		
}
//---------------------------------------------------------------------------------------------------------//

//****************************************end of UnniRamGraphic.c*******************************************//
