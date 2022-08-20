//*********************************************************************************************************//
//UnniRamGraphic library, header file
//draw different grapics objects into raw buffer in RAM
//Created 04.09.2018
//Created by Novikov Dmitry
//*********************************************************************************************************//

#ifndef __UNNIRAMGRAPHIC_H
#define __UNNIRAMGRAPHIC_H

//*********************************************************************************************************//
/* includes */
#include <stdint.h>
#include <stdlib.h>

//*********************************************************************************************************//
/* defines, constants and macro */

/* memory alloc macro set **********************************************************************************/
#define URG_GET_MEMORY(memSize) malloc(memSize)
#define URG_FREE_MEMORY(memPointer) free(memPointer)

/* base colors ********************************************************************************************/
#define URG_COLOR_1B_WHITE 1U
#define URG_COLOR_1B_BLACK 0U

//*********************************************************************************************************//
/* enumeration lists */

/* unni ram graphic format of canvas ***********************************************************************/
typedef enum
{
		DATA_FORMAT_1BIT_OY = 1,
		DATA_FORMAT_1BIT_OX,
		DATA_FORMAT_UNK
} URG_BUF_DATA_FORMAT;

/* unni ram graphic color mode *****************************************************************************/
typedef enum
{
		COLOR_WB_1BIT = 1,
		COLOR_WB_1BIT_INVERSE,
		COLOR_UNK
} URG_COLOR_MODE;

/* unni ram graphic function result enum *******************************************************************/
typedef enum
{
		URG_OK = 0,
		URG_ERROR,
		URG_INPUT_DATA_ERROR,
		URG_MEMORY_ERROR,
		URG_INVALID_CANVAS_RESOLUTION,
		URG_FUNC_NOT_AVAILABLE
} URG_RESULT;

//*********************************************************************************************************//
/* struct declarations */

/* graphic figures *****************************************************************************************/
typedef struct
{
		int32_t x,y;
} URG_POINT;

typedef struct
{
		URG_POINT startPoint, endPoint;
} URG_LINE;

typedef struct
{
		URG_POINT startPoint;
		URG_POINT endPoint;
} URG_RECT;

/* color structures ****************************************************************************************/
typedef uint8_t URG_COLOR_1B;
typedef struct
{
		uint8_t r;
		uint8_t g;
		uint8_t b;
} URG_COLOR_RGB;

/* general color struct ************************************************************************************/
typedef union
{
		URG_COLOR_1B color1B;
		URG_COLOR_RGB colorRGB;	
} URG_COLOR;

/* text struct ********************************************************************************************/
typedef struct
{
		uint32_t symbolWidth;
		uint32_t symbolHeight;
		uint32_t centerAlignment;
		uint32_t textSpacing;
		uint8_t* fontRaw;
} URG_FONT_CONFIG;

/* unni ram graphic raw buffer struct **********************************************************************/
typedef struct
{
		URG_BUF_DATA_FORMAT dataFormat;
		uint32_t* raw;
} URG_RAW_BUFFER;

/* unni ram graphic main config ****************************************************************************/
typedef struct
{
		URG_COLOR_MODE colorMode;
		URG_COLOR color;
		uint32_t screenWidth;
		uint32_t screenHeight;
		URG_RAW_BUFFER* buffer;
} URG_CANVAS;

//*********************************************************************************************************//
/* main unni ram graphic functions *************************************************************************/
URG_RESULT URGAllocRAMBuffer(uint32_t screenWidth, uint32_t screenHeight, URG_BUF_DATA_FORMAT dataFormat, URG_RAW_BUFFER** buffer); 
URG_RESULT URGFreeRAMBuffer(URG_RAW_BUFFER** buffer);

/* 2D unni ram graphic functions ***************************************************************************/
void URGDrawPointXYInBufFast(int32_t posX, int32_t posY, URG_CANVAS* canvas);
void URGDrawLineXYInBufFast(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t thickness, URG_CANVAS* canvas);
void URGDrawRectInBufFast(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t thickness, URG_CANVAS* canvas);
void URGDrawFilledRectInBufFast(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, URG_CANVAS* canvas);
void URGDrawCircleInBufFast(uint8_t posX, uint8_t posY, uint8_t radius, uint8_t thickness, URG_CANVAS* canvas);
void URGDrawTextInBufFast(URG_FONT_CONFIG* fontConfig, uint8_t* text, uint8_t posX, uint8_t posY, URG_CANVAS* canvas);

#endif
//****************************************end of UnniRamGraphic.h*******************************************//
