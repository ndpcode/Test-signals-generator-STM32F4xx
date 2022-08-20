//*********************************************************************************************************//
//UnniRamGraphicUIObjects library, header file
//draw different UI objects into raw buffer in RAM
//Created 04.09.2018
//Created by Novikov Dmitry
//*********************************************************************************************************//

#ifndef __UNNIRAMGRAPHICUIOBJECTS_H
#define __UNNIRAMGRAPHICUIOBJECTS_H

//*********************************************************************************************************//
/* includes */
#include <stdio.h>
#include <string.h>
#include "UnniRamGraphic.h"


//*********************************************************************************************************//
/* defines, constants and macro */

//*********************************************************************************************************//
/* enumeration lists */

/* flags for one Unni UI element */
typedef enum
{
		ELEMENT_VISIBLE = 1,
		ELEMENT_ENABLED = 2,
		ELEMENT_ACTIVE = 4
} UNNI_UI_ELEMENT_FLAGS;

/* flags for NumberEdit component */
typedef enum
{
		NUMBEREDIT_FRACTIONAL = 1,
		NUMBEREDIT_SIGN = 2,
		NUMBEREDIT_FOCUSED = 4
} UNNI_UI_NUMBEREDIT_FLAGS;

/* flags for ScrollList component */
typedef enum
{
		SCROLLLIST_LEFT_ALIGNED = 1,
		SCROLLLIST_CENTER_ALIGNED = 2,
		SCROLLLIST_RIGHT_ALIGNED = 4
} UNNI_UI_SCROLLLIST_FLAGS;

//*********************************************************************************************************//
/* struct declarations */

/* unni ui NumberEdit struct */
typedef struct
{
		URG_CANVAS* canvas;
		URG_FONT_CONFIG* font;
		URG_FONT_CONFIG* captionFont;
		uint8_t* captionText;
		uint32_t posX;
		uint32_t posY;
		uint8_t totalDigitCount;
		uint8_t decimalCount;
		UNNI_UI_NUMBEREDIT_FLAGS flags;
		double minValue;
		double maxValue;
		uint8_t negativeFlag;
		uint32_t integerPart;
		uint32_t fractionalPart;
		uint8_t cursorPos;	
} UNNI_UI_NUMBEREDIT;

/* struct for one unni ui ScrollList element */
typedef struct
{
		int32_t posLeft;
		int32_t posBottom;
		uint32_t width;
		uint32_t height;
		uint32_t number;
		UNNI_UI_ELEMENT_FLAGS flags;
		void* targetUIElement;
		void (*updateFunc)(void*, int32_t, int32_t, UNNI_UI_ELEMENT_FLAGS);
		void* prevItem;
		void* nextItem;
} UNNI_UI_SCROLLLIST_ITEM;

/* unni ui ScrollList struct */
typedef struct
{
		URG_CANVAS* canvas;
		int32_t posLeft;
		int32_t posBottom;
		uint32_t frameWidth;
		uint32_t frameHeight;
		uint32_t widthIndent;
		uint32_t topIndent;
		UNNI_UI_SCROLLLIST_FLAGS flags;
		uint32_t itemsCount;
		UNNI_UI_SCROLLLIST_ITEM* items;
		uint32_t itemsHeightIndent;
		uint32_t currentElementNumber;
		uint32_t scrollBarWidth;
		int32_t dX;
		int32_t dY;
} UNNI_UI_SCROLLLIST;

/* struct for one unni ui SwitchBox element */
typedef struct
{
		uint8_t* text;
		void (*handler)(uint32_t itemIndex);		
} UNNI_UI_SWITCH_BOX_ITEM;

/* unni ui SwitchBox struct */
typedef struct
{
		URG_CANVAS* canvas;
		uint32_t width;
		uint32_t height;
		URG_FONT_CONFIG* font;
		uint8_t* staticText;
		uint32_t itemsCount;
		uint32_t currentElementNumber;
		UNNI_UI_ELEMENT_FLAGS flags;
		UNNI_UI_SWITCH_BOX_ITEM* items;
		uint8_t autoRunHandlers;
} UNNI_UI_SWITCHBOX;

/* unni ui MessageBox struct */
typedef struct
{
		URG_CANVAS* canvas;
		int32_t posLeft;
		int32_t posBottom;
		uint32_t frameWidth;
		uint32_t frameHeight;
		URG_FONT_CONFIG* font;
		URG_FONT_CONFIG* captionFont;
		uint8_t* captionText;
		uint8_t* messageText;
		uint32_t framesDrawTime;
		uint8_t msgBxDataAccessLock;
} UNNI_UI_MESSAGEBOX;

//*********************************************************************************************************//
/* unni ram graphic UI functions */

/* edit number UI component */
UNNI_UI_NUMBEREDIT* UnniUiNumberEditCreate(uint32_t posX, uint32_t posY, uint8_t totalDigitCount, uint8_t decimalCount,
                                           UNNI_UI_NUMBEREDIT_FLAGS flags, double minValue, double maxValue, double startValue,
																					 URG_FONT_CONFIG* font, URG_FONT_CONFIG* captionFont, uint8_t* captionText, URG_CANVAS* canvas);
int32_t UnniUiNumberEditGetIntValue(UNNI_UI_NUMBEREDIT* uiEditNumber);
double UnniUiNumberEditGetFloatValue(UNNI_UI_NUMBEREDIT* uiEditNumber);
URG_RESULT UnniUiNumberEditSetValue(UNNI_UI_NUMBEREDIT* uiEditNumber, double value);
int32_t UnniUiNumberEditGetFullValue(UNNI_UI_NUMBEREDIT* uiEditNumber);
URG_RESULT UnniUiNumberEditSetFullValue(UNNI_UI_NUMBEREDIT* uiEditNumber, int32_t value);																				 
URG_RESULT UnniUiNumberEditInc(UNNI_UI_NUMBEREDIT* uiEditNumber);
URG_RESULT UnniUiNumberEditDec(UNNI_UI_NUMBEREDIT* uiEditNumber);
URG_RESULT UnniUiNumberEditCursorMoveLeft(UNNI_UI_NUMBEREDIT* uiEditNumber);
URG_RESULT UnniUiNumberEditCursorMoveRight(UNNI_UI_NUMBEREDIT* uiEditNumber);
URG_RESULT UnniUiNumberEditSetFocus(UNNI_UI_NUMBEREDIT* uiEditNumber, uint8_t focusState);
URG_RESULT UnniUiNumberEditDraw(UNNI_UI_NUMBEREDIT* uiEditNumber);
void UnniUiNumberEditUpdate(void* uiEditNumber, int32_t posX, int32_t posY, UNNI_UI_ELEMENT_FLAGS flags);
																					 
/* scroll list UI component */
UNNI_UI_SCROLLLIST* UnniUiScrollListCreate(int32_t posLeft, int32_t posBottom, uint32_t frameWidth, uint32_t frameHeight,
																					 uint32_t widthIndent, uint32_t topIndent, uint32_t itemsHeightIndent, uint32_t scrollBarWidth,
																					 UNNI_UI_SCROLLLIST_FLAGS flags, URG_CANVAS* canvas);
URG_RESULT UnniUiScrollListAddItem(UNNI_UI_SCROLLLIST* scrollList, uint32_t itemWidth, uint32_t itemHeight, void* targetUIElement, void (*updateFunc)(void*, int32_t, int32_t, UNNI_UI_ELEMENT_FLAGS));
URG_RESULT UnniUiScrollListDraw(UNNI_UI_SCROLLLIST* scrollList);
URG_RESULT UnniUiScrollListCursorTo(UNNI_UI_SCROLLLIST* scrollList, uint32_t itemNumber);
URG_RESULT UnniUiScrollListUp(UNNI_UI_SCROLLLIST* scrollList);	
URG_RESULT UnniUiScrollListDown(UNNI_UI_SCROLLLIST* scrollList);

/* switch box UI component */
//UNNI_UI_SWITCHBOX* UnniUiSwitchBoxCreate(uint8_t* staticText, URG_FONT_CONFIG* font, UNNI_UI_SWITCH_BOX_ITEM* switchBoxItems,
//                                         uint32_t itemsCount, uint32_t width, uint32_t height, URG_CANVAS* canvas);
URG_RESULT UnniUiSwitchBoxPrev(UNNI_UI_SWITCHBOX* switchBox);
URG_RESULT UnniUiSwitchBoxNext(UNNI_UI_SWITCHBOX* switchBox);
URG_RESULT UnniUiSwitchBoxRunHandler(UNNI_UI_SWITCHBOX* switchBox);
URG_RESULT UnniUiSwitchBoxGetCurrent(UNNI_UI_SWITCHBOX* switchBox, uint32_t* currentElement);
URG_RESULT UnniUiSwitchBoxSetCurrent(UNNI_UI_SWITCHBOX* switchBox, uint32_t currentElement, uint8_t elementHandlerStart);
void UnniUiSwitchBoxUpdate(void* switchBox, int32_t posX, int32_t posY, UNNI_UI_ELEMENT_FLAGS flags);

/* message box UI component */
UNNI_UI_MESSAGEBOX* UnniUiMessageBoxInit(int32_t posLeft, int32_t posBottom, uint32_t frameWidth, uint32_t frameHeight,
                                         URG_FONT_CONFIG* font, URG_FONT_CONFIG* captionFont, URG_CANVAS* canvas);
URG_RESULT UnniUiMessageBoxShow(UNNI_UI_MESSAGEBOX* messageBox, const uint8_t* captionText, const uint8_t* messageText, uint32_t framesCount);
URG_RESULT UnniUiMessageBoxDraw(UNNI_UI_MESSAGEBOX* messageBox);

#endif
//************************************end of UnniRamGraphicUIObjects.h*************************************//
