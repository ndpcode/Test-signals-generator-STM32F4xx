//*********************************************************************************************************//
//UnniRamGraphicUIObjects library, source file
//draw different UI objects into raw buffer in RAM
//Created 04.09.2018
//Created by Novikov Dmitry
//*********************************************************************************************************//

//*********************************************************************************************************//
/* includes */
#include "UnniRamGraphicUIObjects.h"

//*********************************************************************************************************//
/* private const and var */
static const uint32_t degreesOfTen[10] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

//*********************************************************************************************************//
/* unni ram graphic UI functions */

//---------------------------------------------------------------------------------------------------------//
/* edit number UI component */

//---------------------------------------------------------------------------------------------------------//
/* create and config UI NumberEdit component*/
UNNI_UI_NUMBEREDIT* UnniUiNumberEditCreate(uint32_t posX, uint32_t posY, uint8_t totalDigitCount, uint8_t decimalCount,
                                           UNNI_UI_NUMBEREDIT_FLAGS flags, double minValue, double maxValue, double startValue,
																					 URG_FONT_CONFIG* font, URG_FONT_CONFIG* captionFont, uint8_t* captionText, URG_CANVAS* canvas)
{
		UNNI_UI_NUMBEREDIT* newNumberEdit;
		
		//check input data
		if (!totalDigitCount) return 0;
		if (decimalCount > totalDigitCount) return 0;
		if (!font) return 0;
		if ((captionText) && (!captionFont)) return 0;
		if (!canvas) return 0;
	
		//allocate memory for struct
		newNumberEdit = 0;
		newNumberEdit = (UNNI_UI_NUMBEREDIT*)URG_GET_MEMORY(sizeof(UNNI_UI_NUMBEREDIT));
		if (!newNumberEdit) return 0;
	
		//init config
		newNumberEdit->canvas = canvas;
		newNumberEdit->font = font;
		newNumberEdit->captionFont = captionFont;
		newNumberEdit->captionText = captionText;
		newNumberEdit->posX = posX;
		newNumberEdit->posY = posY;
		newNumberEdit->totalDigitCount = totalDigitCount;
		newNumberEdit->decimalCount = decimalCount;
		newNumberEdit->flags = flags;
		newNumberEdit->minValue = minValue;
		newNumberEdit->maxValue = maxValue;
		newNumberEdit->negativeFlag = (startValue >= (double)0)? 0 : 1;
		if (newNumberEdit->negativeFlag)
		{
				startValue *= -1;
		}
		UnniUiNumberEditSetValue(newNumberEdit, startValue);
		if (decimalCount)
		{
				newNumberEdit->cursorPos = decimalCount + 1;				
		} else
		{
				newNumberEdit->cursorPos = 1;
		}
		//config font
		//newNumberEdit->font->textSpacing = 2;
		//newNumberEdit->font->centerAlignment = 1;

		//return
		return newNumberEdit;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/**/
int32_t UnniUiNumberEditGetIntValue(UNNI_UI_NUMBEREDIT* uiEditNumber)
{
		//get integer value
		uint32_t integerPart = uiEditNumber->integerPart;
		//control negative number
		if ( (uiEditNumber->flags & NUMBEREDIT_SIGN) && (uiEditNumber->negativeFlag) )
		{
				integerPart *= -1;
		}	
		//calc current value
		return integerPart;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/**/
double UnniUiNumberEditGetFloatValue(UNNI_UI_NUMBEREDIT* uiEditNumber)
{
		//get float value
		double value = (double)uiEditNumber->integerPart + (double)uiEditNumber->fractionalPart/degreesOfTen[uiEditNumber->decimalCount];
		//control negative number
		if ( (uiEditNumber->flags & NUMBEREDIT_SIGN) && (uiEditNumber->negativeFlag) )
		{
				value *= -1;
		}	
		//calc current value
		return value;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/**/
URG_RESULT UnniUiNumberEditSetValue(UNNI_UI_NUMBEREDIT* uiEditNumber, double value)
{
		uint32_t intCount;
	
		//control negative number
		if ( (uiEditNumber->flags & NUMBEREDIT_SIGN) && (value < 0.0) )
		{
				uiEditNumber->negativeFlag = 1;
				value = -value;
		} else
		{
				uiEditNumber->negativeFlag = 0;
		}	
		//get size of integer part
		intCount = uiEditNumber->totalDigitCount - uiEditNumber->decimalCount;
		//get integer part
		uiEditNumber->integerPart = (uint32_t)value - ((uint32_t)value/degreesOfTen[intCount])*degreesOfTen[intCount];
		//get fractional part
		value = (uint32_t)((double)value*degreesOfTen[uiEditNumber->decimalCount]);
		uiEditNumber->fractionalPart = (uint32_t)value - ((uint32_t)value/degreesOfTen[uiEditNumber->decimalCount])*degreesOfTen[uiEditNumber->decimalCount];
		
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/**/
int32_t UnniUiNumberEditGetFullValue(UNNI_UI_NUMBEREDIT* uiEditNumber)
{
		//get full value
		int32_t fullValue = uiEditNumber->integerPart*degreesOfTen[uiEditNumber->decimalCount] + uiEditNumber->fractionalPart;
		//control negative number
		if ( (uiEditNumber->flags & NUMBEREDIT_SIGN) && (uiEditNumber->negativeFlag) )
		{
				fullValue *= -1;
		}	
		return fullValue;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/**/
URG_RESULT UnniUiNumberEditSetFullValue(UNNI_UI_NUMBEREDIT* uiEditNumber, int32_t value)
{
		//control negative number
		if ( (uiEditNumber->flags & NUMBEREDIT_SIGN) && (value < 0) )
		{
				uiEditNumber->negativeFlag = 1;
				value = -value;
		} else
		{
				uiEditNumber->negativeFlag = 0;
		}
		value = abs(value);
		//get integer part
		uiEditNumber->integerPart = (uint32_t)value/ degreesOfTen[uiEditNumber->decimalCount];
		//get fractional part
		uiEditNumber->fractionalPart = value - uiEditNumber->integerPart * degreesOfTen[uiEditNumber->decimalCount];
		
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/**/
URG_RESULT UnniUiNumberEditInc(UNNI_UI_NUMBEREDIT* uiEditNumber)
{
		double numberEditDelta = 0;
		double numberEditValue = 0;
	
		//check number edit focus
		if (!(uiEditNumber->flags & NUMBEREDIT_FOCUSED)) return URG_FUNC_NOT_AVAILABLE;
	
		//calc current value
		numberEditValue = (double)uiEditNumber->integerPart + (double)uiEditNumber->fractionalPart/degreesOfTen[uiEditNumber->decimalCount];
		//control negative number
		if ( (uiEditNumber->flags & NUMBEREDIT_SIGN) && (uiEditNumber->negativeFlag) )
		{
				numberEditValue *= -1;
		}		
	
		//get delta for number edit value
		if ( (uiEditNumber->flags & NUMBEREDIT_FRACTIONAL) && (uiEditNumber->cursorPos <= uiEditNumber->decimalCount) )
		{
				//float
				numberEditDelta = (double)1 / degreesOfTen[uiEditNumber->decimalCount - uiEditNumber->cursorPos + 1];
		} else
		{
				//integer
				numberEditDelta = (double)degreesOfTen[uiEditNumber->cursorPos - uiEditNumber->decimalCount - 1];
		}
		
		//check max and calc new value
		if ((uiEditNumber->maxValue - numberEditValue) >= numberEditDelta)
		{
				numberEditValue += numberEditDelta + 0.0000000001;
				//save new value
				UnniUiNumberEditSetValue(uiEditNumber, numberEditValue);
		}

		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/**/
URG_RESULT UnniUiNumberEditDec(UNNI_UI_NUMBEREDIT* uiEditNumber)
{
		double numberEditDelta = 0;
		double numberEditValue;
	
		//check number edit focus
		if (!(uiEditNumber->flags & NUMBEREDIT_FOCUSED)) return URG_FUNC_NOT_AVAILABLE;
	
		//calc current value
		numberEditValue = (double)uiEditNumber->integerPart + (double)uiEditNumber->fractionalPart/degreesOfTen[uiEditNumber->decimalCount];
		//control negative number
		if ( (uiEditNumber->flags & NUMBEREDIT_SIGN) && (uiEditNumber->negativeFlag) )
		{
				numberEditValue *= -1;
		}		
	
		//get delta for number edit value
		if ( (uiEditNumber->flags & NUMBEREDIT_FRACTIONAL) && (uiEditNumber->cursorPos <= uiEditNumber->decimalCount) )
		{
				//float
				numberEditDelta = (double)1 / degreesOfTen[uiEditNumber->decimalCount - uiEditNumber->cursorPos + 1];
		} else
		{
				//integer
				numberEditDelta = (double)degreesOfTen[uiEditNumber->cursorPos - uiEditNumber->decimalCount - 1];
		}
		
		//check min and calc new value
		if ((numberEditValue - uiEditNumber->minValue) >= numberEditDelta)
		{
				numberEditValue -= numberEditDelta;
				//save new value
				UnniUiNumberEditSetValue(uiEditNumber, numberEditValue);
		}

		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* move left cursor of NumberEdit component */
URG_RESULT UnniUiNumberEditCursorMoveLeft(UNNI_UI_NUMBEREDIT* uiEditNumber)
{
		//check number edit focus
		if (!(uiEditNumber->flags & NUMBEREDIT_FOCUSED)) return URG_FUNC_NOT_AVAILABLE;
		
		//inc cursor
		if (uiEditNumber->cursorPos < uiEditNumber->totalDigitCount)
		{
				uiEditNumber->cursorPos++;
		}
		
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* move right cursor of NumberEdit component */
URG_RESULT UnniUiNumberEditCursorMoveRight(UNNI_UI_NUMBEREDIT* uiEditNumber)
{
		//check number edit focus
		if (!(uiEditNumber->flags & NUMBEREDIT_FOCUSED)) return URG_FUNC_NOT_AVAILABLE;
		
		//dec cursor
		if (uiEditNumber->cursorPos > 1)
		{
				uiEditNumber->cursorPos--;
		}
		
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* move right cursor of NumberEdit component */
URG_RESULT UnniUiNumberEditSetFocus(UNNI_UI_NUMBEREDIT* uiEditNumber, uint8_t focusState)
{
		if (focusState)
		{
				uiEditNumber->flags |= NUMBEREDIT_FOCUSED;				
		} else
		{
				uiEditNumber->flags &= ~NUMBEREDIT_FOCUSED;
		}
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* draw NumberEdit component on canvas*/
URG_RESULT UnniUiNumberEditDraw(UNNI_UI_NUMBEREDIT* uiEditNumber)
{
		char formatStr[20] = "";
		char numberStr[20] = "";
		int32_t intVal;
		int32_t posX, posY;
		uint8_t charCount;
	
		//check input data
		if (!uiEditNumber) return URG_INPUT_DATA_ERROR;
	
		//draw digit
		charCount = 0;
		//integer part
		//check sign of integer and correct integer part value
		if ( (uiEditNumber->flags & NUMBEREDIT_SIGN) && (uiEditNumber->negativeFlag) )
		{
				intVal = -uiEditNumber->integerPart;
				charCount++;
		} else
		{
				intVal = uiEditNumber->integerPart;
		}
		//calc coordinates
		charCount += uiEditNumber->totalDigitCount - uiEditNumber->decimalCount;
		posX = uiEditNumber->posX;
		posY = uiEditNumber->posY;
		//format string
		snprintf(formatStr, 19, "%s%d%s", "%0", charCount, "d");
		snprintf(numberStr, 19, formatStr, intVal);
		//draw integer part
		URGDrawTextInBufFast(uiEditNumber->font, (uint8_t*)numberStr, posX, posY, uiEditNumber->canvas);
		
		//fractional part
		if (uiEditNumber->flags & NUMBEREDIT_FRACTIONAL)
		{
				//point
				//get string
				snprintf(numberStr, 19, ".");
				//calc coordinates
				posX = uiEditNumber->posX + charCount*(uiEditNumber->font->symbolWidth + uiEditNumber->font->textSpacing);
				posY = uiEditNumber->posY;
				//draw point and inc counter
				URGDrawTextInBufFast(uiEditNumber->font, (uint8_t*)numberStr, posX, posY, uiEditNumber->canvas);
				charCount += 1;
				
				//fractional part
				intVal = uiEditNumber->fractionalPart;
				//format string
				snprintf(formatStr, 19, "%s%d%s", "%0", uiEditNumber->decimalCount, "d");
				snprintf(numberStr, 19, formatStr, intVal);	
				//calc coordinates
				posX += uiEditNumber->font->symbolWidth + uiEditNumber->font->textSpacing;	
				//draw fractional part and inc counter
				URGDrawTextInBufFast(uiEditNumber->font, (uint8_t*)numberStr, posX, posY, uiEditNumber->canvas);			
		}
		
		//draw caption text if it available
		if (uiEditNumber->captionText)
		{
				//calc coordinates
				charCount += uiEditNumber->decimalCount;
				posX = uiEditNumber->posX + charCount*(uiEditNumber->font->symbolWidth + uiEditNumber->font->textSpacing);
				//draw text
				URGDrawTextInBufFast(uiEditNumber->captionFont, uiEditNumber->captionText, posX, posY, uiEditNumber->canvas);				
		}
		
		//draw cursor if it available
		if (uiEditNumber->flags & NUMBEREDIT_FOCUSED)
		{
				//get char count for calc cursor shift
				charCount = uiEditNumber->totalDigitCount - uiEditNumber->cursorPos;
				//get value of number at focus
				if ( (uiEditNumber->flags & NUMBEREDIT_FRACTIONAL) && (uiEditNumber->cursorPos <= uiEditNumber->decimalCount) )
				{
						charCount++;
						intVal = uiEditNumber->fractionalPart/degreesOfTen[uiEditNumber->cursorPos - 1];
				} else
				{
						intVal = uiEditNumber->integerPart/degreesOfTen[uiEditNumber->cursorPos - uiEditNumber->decimalCount - 1];
				}
				intVal -= (intVal/10)*10;
				if (uiEditNumber->negativeFlag)
				{
						charCount++;
				}
				//calc coordinates
				posX = uiEditNumber->posX + charCount*(uiEditNumber->font->symbolWidth + uiEditNumber->font->textSpacing);
				//draw filled rect
				URGDrawFilledRectInBufFast(posX - 1, posY + 1, posX + uiEditNumber->font->symbolWidth, posY - uiEditNumber->font->symbolHeight, uiEditNumber->canvas);
				//format string
				snprintf(numberStr, 19, "%d", intVal);
				//change color mode and draw
				uiEditNumber->canvas->colorMode = COLOR_WB_1BIT_INVERSE;
				URGDrawTextInBufFast(uiEditNumber->font, (uint8_t*)numberStr, posX, posY, uiEditNumber->canvas);
				uiEditNumber->canvas->colorMode = COLOR_WB_1BIT;	
		}
		
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* draw NumberEdit component on canvas with extended parameters*/
void UnniUiNumberEditUpdate(void* uiEditNumber, int32_t posX, int32_t posY, UNNI_UI_ELEMENT_FLAGS flags)
{
		//check input data
		if (!uiEditNumber) return;
		if ((posX < 0) || (posY < 0)) return;
	
		//check active or not
		if ( (flags & ELEMENT_ENABLED) && (flags & ELEMENT_ACTIVE) )
		{
				//set focus
				((UNNI_UI_NUMBEREDIT*)uiEditNumber)->flags |= NUMBEREDIT_FOCUSED;					
		} else
		{
				//reset focus
				((UNNI_UI_NUMBEREDIT*)uiEditNumber)->flags &= ~NUMBEREDIT_FOCUSED;						
		}
		
		//check flags
		if (flags & ELEMENT_VISIBLE)
		{
				//set X,Y
				((UNNI_UI_NUMBEREDIT*)uiEditNumber)->posX = posX;
				((UNNI_UI_NUMBEREDIT*)uiEditNumber)->posY = posY;
				//draw
				UnniUiNumberEditDraw(uiEditNumber);
		}
}
//---------------------------------------------------------------------------------------------------------//


//---------------------------------------------------------------------------------------------------------//
/* scroll list UI component */

//---------------------------------------------------------------------------------------------------------//
/* util function: get ScrollList item */
static UNNI_UI_SCROLLLIST_ITEM* getScrollListItem(UNNI_UI_SCROLLLIST* scrollList, uint32_t itemIndex)
{
		UNNI_UI_SCROLLLIST_ITEM* lastScrollListItem;
	
		lastScrollListItem = scrollList->items;
		while (lastScrollListItem)
		{
				if (lastScrollListItem->number == itemIndex)
				{
						return lastScrollListItem;
				}
				lastScrollListItem = lastScrollListItem->nextItem;
		}
		
		return 0;	
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* util function: update elements visible flag */
static void updateScrollListItemsState(UNNI_UI_SCROLLLIST* scrollList, uint32_t startItemIndex, uint8_t inverseDirection)
{	
		UNNI_UI_SCROLLLIST_ITEM* scrollListItem;
		int32_t posX, posY;
	
		//set all items as invisible
		scrollListItem = scrollList->items;
		while (scrollListItem)
		{
				scrollListItem->flags &= ~ELEMENT_VISIBLE;
				scrollListItem = scrollListItem->nextItem;
		}
	
		//get item by index
		scrollListItem = getScrollListItem(scrollList, startItemIndex);
		if (!scrollListItem) return;		
		if (!inverseDirection)
		{
				//direction - top down
				//set correction delta dX,dY
				scrollList->dX = 0;
				scrollList->dY = 0;
				//set start X,Y
				posY = scrollList->posBottom - scrollList->frameHeight + scrollList->topIndent + 1;
				while (scrollListItem)
				{
						//check frame range
						if ((posY + scrollListItem->height - 1) <= scrollList->posBottom)
						{
								//calc X
								if (scrollList->flags & SCROLLLIST_LEFT_ALIGNED)
								{
										posX = scrollList->posLeft + scrollList->widthIndent;
								} else if (scrollList->flags & SCROLLLIST_CENTER_ALIGNED)
								{
										posX = scrollList->posLeft + (scrollList->frameWidth - scrollList->scrollBarWidth)/2 - scrollListItem->width/2;
								} else if (scrollList->flags & SCROLLLIST_RIGHT_ALIGNED)
								{
										posX = scrollList->posLeft + scrollList->frameWidth - scrollList->scrollBarWidth - scrollList->widthIndent - scrollListItem->width - 1;
								}
								//calc Y
								posY += scrollListItem->height - 1;
								//save X,Y
								scrollListItem->posLeft = posX;
								scrollListItem->posBottom = posY;
								//set flag
								scrollListItem->flags |= ELEMENT_VISIBLE;
								//add height indent + 1
								posY += scrollList->itemsHeightIndent + 1;
						}			
						//go to next item
						scrollListItem = scrollListItem->nextItem;
				}				
		} else
		{
				//direction - down up
				//set correction delta dX,dY
				scrollList->dX = 0;
				scrollList->dY = 0;
				//set start X,Y
				posY = scrollList->posBottom;
				while (scrollListItem)
				{
						//check frame range
						if ((posY - scrollListItem->height + 1) >= (scrollList->posBottom - scrollList->frameHeight + scrollList->topIndent + 1))
						{
								//calc X
								if (scrollList->flags & SCROLLLIST_LEFT_ALIGNED)
								{
										posX = scrollList->posLeft + scrollList->widthIndent;
								} else if (scrollList->flags & SCROLLLIST_CENTER_ALIGNED)
								{
										posX = scrollList->posLeft + (scrollList->frameWidth - scrollList->scrollBarWidth)/2 - scrollListItem->width/2;
								} else if (scrollList->flags & SCROLLLIST_RIGHT_ALIGNED)
								{
										posX = scrollList->posLeft + scrollList->frameWidth - scrollList->scrollBarWidth - scrollList->widthIndent - scrollListItem->width - 1;
								}
								//save X,Y
								scrollListItem->posLeft = posX;
								scrollListItem->posBottom = posY;
								//set flag
								scrollListItem->flags |= ELEMENT_VISIBLE;
								//calc Y, add height indent
								posY -= scrollListItem->height + scrollList->itemsHeightIndent;
								//calc dY
								scrollList->dY = (scrollListItem->posBottom - scrollListItem->height + 1) - (scrollList->posBottom - scrollList->frameHeight + scrollList->topIndent + 1);
						}
						//go to prev item
						scrollListItem = scrollListItem->prevItem;
				}
		}		
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* create unni ui ScrollList */
UNNI_UI_SCROLLLIST* UnniUiScrollListCreate(int32_t posLeft, int32_t posBottom, uint32_t frameWidth, uint32_t frameHeight,
																					 uint32_t widthIndent, uint32_t topIndent, uint32_t itemsHeightIndent, uint32_t scrollBarWidth,
																					 UNNI_UI_SCROLLLIST_FLAGS flags, URG_CANVAS* canvas)
{
		UNNI_UI_SCROLLLIST* newScrollList;
	
		//check input data
		if (!canvas) return 0;
		if ((frameWidth == 0) || (frameHeight == 0)) return 0;
	
		//allocate memory for new component and check
		newScrollList = (UNNI_UI_SCROLLLIST*)URG_GET_MEMORY(sizeof(UNNI_UI_SCROLLLIST));
		if (!newScrollList) return 0;

		//set config
		newScrollList->canvas = canvas;
		newScrollList->posLeft = posLeft;
		newScrollList->posBottom = posBottom;
		newScrollList->frameWidth = frameWidth;
		newScrollList->frameHeight = frameHeight;
		newScrollList->widthIndent = widthIndent;
		newScrollList->topIndent = topIndent;
		newScrollList->flags = flags;
		newScrollList->itemsCount = 0;
		newScrollList->items = 0;
		newScrollList->itemsHeightIndent = itemsHeightIndent;
		newScrollList->currentElementNumber = 0;
		newScrollList->scrollBarWidth = scrollBarWidth;
		newScrollList->dX = 0;
		newScrollList->dY = 0;
		//return
		return newScrollList;		
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* add item to ScrollList*/
URG_RESULT UnniUiScrollListAddItem(UNNI_UI_SCROLLLIST* scrollList, uint32_t itemWidth, uint32_t itemHeight, void* targetUIElement, void (*updateFunc)(void*, int32_t, int32_t, UNNI_UI_ELEMENT_FLAGS))
{
		UNNI_UI_SCROLLLIST_ITEM* newScrollListItem;
		UNNI_UI_SCROLLLIST_ITEM* lastScrollListItem;
		
		//check input data
		if (!scrollList) return URG_INPUT_DATA_ERROR;
		if (!updateFunc) return URG_INPUT_DATA_ERROR;
	
		//allocate memory for new item and check
		newScrollListItem = (UNNI_UI_SCROLLLIST_ITEM*)URG_GET_MEMORY(sizeof(UNNI_UI_SCROLLLIST_ITEM));
		if (!newScrollListItem) return URG_MEMORY_ERROR;
	
		//set config for new item
		newScrollListItem->posLeft = 0;
		newScrollListItem->posBottom = 0;
		newScrollListItem->width = itemWidth;
		newScrollListItem->height = itemHeight;
		newScrollListItem->flags = ELEMENT_ENABLED | ELEMENT_VISIBLE;
		newScrollListItem->targetUIElement = targetUIElement;
		newScrollListItem->updateFunc = updateFunc;
		newScrollListItem->prevItem = 0;
		newScrollListItem->nextItem = 0;	
		//check prev item and config new
		if (scrollList->items)
		{
				//go to last item
				lastScrollListItem = scrollList->items;
				while (lastScrollListItem->nextItem)
				{
						lastScrollListItem = lastScrollListItem->nextItem;										
				}
				//set next item
				lastScrollListItem->nextItem = newScrollListItem;
				//config new item
				newScrollListItem->prevItem = lastScrollListItem;
				newScrollListItem->number = lastScrollListItem->number + 1;
		} else
		{
				//set first item
				scrollList->items = newScrollListItem;
				scrollList->currentElementNumber = 1;
				//config new item
				newScrollListItem->number = 1;
				//set first as active
				newScrollListItem->flags |= ELEMENT_ACTIVE;
		}
		//set count
		scrollList->itemsCount = newScrollListItem->number;
		//update all items state
		updateScrollListItemsState(scrollList, scrollList->currentElementNumber, 0);
		//return
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* draw unni ui ScrollList */
URG_RESULT UnniUiScrollListDraw(UNNI_UI_SCROLLLIST* scrollList)
{
		UNNI_UI_SCROLLLIST_ITEM* lastScrollListItem;
		URG_RECT scrollBarRect;
		uint32_t scrollBarHeight;
		
		//check input data
		if (!scrollList) return URG_INPUT_DATA_ERROR;
	
		//update items
		//get first item
		lastScrollListItem = scrollList->items;
		while (lastScrollListItem)
		{
				//update and draw
				if (lastScrollListItem->targetUIElement)
				{
						lastScrollListItem->updateFunc(lastScrollListItem->targetUIElement, lastScrollListItem->posLeft + scrollList->dX,
							                             lastScrollListItem->posBottom - scrollList->dY, lastScrollListItem->flags);
				} else
				{
						lastScrollListItem->updateFunc((void*)0, lastScrollListItem->posLeft + scrollList->dX,
							                             lastScrollListItem->posBottom - scrollList->dY, lastScrollListItem->flags);
				}		
				//go to next item
				lastScrollListItem = lastScrollListItem->nextItem;
		}
		
		//draw scroll bar
		scrollBarRect.startPoint.x = scrollList->posLeft + scrollList->frameWidth - scrollList->scrollBarWidth - 1;
		scrollBarRect.startPoint.y = scrollList->posBottom;
		scrollBarRect.endPoint.x = scrollList->posLeft + scrollList->frameWidth - 1;
		scrollBarRect.endPoint.y = scrollList->posBottom - scrollList->frameHeight + 1;
		URGDrawRectInBufFast(scrollBarRect.startPoint.x, scrollBarRect.startPoint.y, scrollBarRect.endPoint.x, scrollBarRect.endPoint.y, 1, scrollList->canvas);
		scrollBarHeight = (scrollBarRect.startPoint.y - scrollBarRect.endPoint.y - 1)/scrollList->itemsCount;
		scrollBarRect.startPoint.y = scrollBarRect.endPoint.y + scrollBarHeight*scrollList->currentElementNumber + 1;
		scrollBarRect.endPoint.y = scrollBarRect.startPoint.y - scrollBarHeight + 1;
		if ( scrollBarRect.endPoint.y >= scrollBarRect.startPoint.y )
		{
				scrollBarRect.endPoint.y = scrollBarRect.startPoint.y - 1;
		}
		URGDrawFilledRectInBufFast(scrollBarRect.startPoint.x, scrollBarRect.startPoint.y, scrollBarRect.endPoint.x, scrollBarRect.endPoint.y, scrollList->canvas);		
		//return
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* ScrollList step up */
URG_RESULT UnniUiScrollListUp(UNNI_UI_SCROLLLIST* scrollList)
{
		UNNI_UI_SCROLLLIST_ITEM* scrollListItem;
		UNNI_UI_SCROLLLIST_ITEM* lastScrollListItem;
	
		//check input data
		if (!scrollList) return URG_INPUT_DATA_ERROR;
	
		//get current item
		lastScrollListItem = getScrollListItem(scrollList, scrollList->currentElementNumber);	
		//check - first element or not
		if (scrollList->currentElementNumber > 1)
		{
				//dec and get item
				scrollList->currentElementNumber--;
				scrollListItem = getScrollListItem(scrollList, scrollList->currentElementNumber);
				if (lastScrollListItem && scrollListItem)
				{
						//reset active flag
						lastScrollListItem->flags &= ~ELEMENT_ACTIVE;
						//element visible check
						if (scrollListItem->flags & ELEMENT_VISIBLE)
						{
								//element already visible, no changes required
								scrollListItem->flags |= ELEMENT_ACTIVE;
						} else
						{
								//element not visible, need changes
								scrollListItem->flags |= ELEMENT_ACTIVE | ELEMENT_VISIBLE;
								//update all elements
								updateScrollListItemsState(scrollList, scrollList->currentElementNumber, 0);
						}						
				} else
				{
						return URG_ERROR;
				}				
		} else
		{
				//go back/down
				scrollList->currentElementNumber = scrollList->itemsCount;
				//get item
				scrollListItem = getScrollListItem(scrollList, scrollList->currentElementNumber);
				if (lastScrollListItem && scrollListItem)
				{
						//reset active flag
						lastScrollListItem->flags &= ~ELEMENT_ACTIVE;
						//element visible check
						if (scrollListItem->flags & ELEMENT_VISIBLE)
						{
								//element already visible, no changes required
								scrollListItem->flags |= ELEMENT_ACTIVE;
						} else
						{
								//element not visible, need changes
								scrollListItem->flags |= ELEMENT_ACTIVE | ELEMENT_VISIBLE;
								//update all elements
								updateScrollListItemsState(scrollList, scrollList->currentElementNumber, 1);
						}						
				} else
				{
						return URG_ERROR;
				}
		}	
		//return
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* ScrollList step down */
URG_RESULT UnniUiScrollListDown(UNNI_UI_SCROLLLIST* scrollList)
{
		UNNI_UI_SCROLLLIST_ITEM* scrollListItem;
		UNNI_UI_SCROLLLIST_ITEM* lastScrollListItem;
	
		//check input data
		if (!scrollList) return URG_INPUT_DATA_ERROR;
	
		//get current item
		lastScrollListItem = getScrollListItem(scrollList, scrollList->currentElementNumber);	
		//check - last element or not
		if (scrollList->currentElementNumber < scrollList->itemsCount)
		{
				//inc and get item
				scrollList->currentElementNumber++;
				scrollListItem = getScrollListItem(scrollList, scrollList->currentElementNumber);
				if (lastScrollListItem && scrollListItem)
				{
						//reset active flag
						lastScrollListItem->flags &= ~ELEMENT_ACTIVE;
						//element visible check
						if (scrollListItem->flags & ELEMENT_VISIBLE)
						{
								//element already visible, no changes required
								scrollListItem->flags |= ELEMENT_ACTIVE;
						} else
						{
								//element not visible, need changes
								scrollListItem->flags |= ELEMENT_ACTIVE | ELEMENT_VISIBLE;
								//update all elements
								updateScrollListItemsState(scrollList, scrollList->currentElementNumber, 1);
						}						
				} else
				{
						return URG_ERROR;
				}				
		} else
		{
				//go forward/up
				scrollList->currentElementNumber = 1;
				//get item
				scrollListItem = getScrollListItem(scrollList, scrollList->currentElementNumber);
				if (lastScrollListItem && scrollListItem)
				{
						//reset active flag
						lastScrollListItem->flags &= ~ELEMENT_ACTIVE;
						//element visible check
						if (scrollListItem->flags & ELEMENT_VISIBLE)
						{
								//element already visible, no changes required
								scrollListItem->flags |= ELEMENT_ACTIVE;
						} else
						{
								//element not visible, need changes
								scrollListItem->flags |= ELEMENT_ACTIVE | ELEMENT_VISIBLE;
								//update all elements
								updateScrollListItemsState(scrollList, scrollList->currentElementNumber, 0);
						}						
				} else
				{
						return URG_ERROR;
				}
		}	
		//return
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* ScrollList go to item # */
URG_RESULT UnniUiScrollListCursorTo(UNNI_UI_SCROLLLIST* scrollList, uint32_t elementNumber)
{
		UNNI_UI_SCROLLLIST_ITEM* scrollListItem;
	
		//check input data
		if (!scrollList) return URG_INPUT_DATA_ERROR;
	
		//get item
		scrollListItem = getScrollListItem(scrollList, elementNumber);
		if (scrollListItem)
		{
				//element visible check
				if (scrollListItem->flags & ELEMENT_VISIBLE)
				{
						//element already visible, no changes required
						scrollListItem->flags |= ELEMENT_ACTIVE;
				} else
				{
						//element not visible, need changes
						scrollListItem->flags |= ELEMENT_ACTIVE | ELEMENT_VISIBLE;
						//update all elements
						if (scrollList->currentElementNumber < elementNumber)
						{
								//update all items
								updateScrollListItemsState(scrollList, scrollList->currentElementNumber, 1);
						} else
						{
								//update all items
								updateScrollListItemsState(scrollList, scrollList->currentElementNumber, 0);
						}						
				}	
				//set current element in cursor
				scrollList->currentElementNumber = elementNumber;
		} else
		{
				return URG_ERROR;
		}
		//return
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* switch box UI component */

//---------------------------------------------------------------------------------------------------------//
/* switch box go to prev element */
URG_RESULT UnniUiSwitchBoxPrev(UNNI_UI_SWITCHBOX* switchBox)
{
		//check input data
		if (!switchBox) return URG_INPUT_DATA_ERROR;

		//check flags
		if (!( (switchBox->flags & ELEMENT_VISIBLE) &&
		       (switchBox->flags & ELEMENT_ENABLED) &&
		       (switchBox->flags & ELEMENT_ACTIVE) )) return URG_FUNC_NOT_AVAILABLE;	
		//go to prev
		if (switchBox->currentElementNumber > 1)
		{
				//dec cursor
				switchBox->currentElementNumber--;
				//check items
				if ( (switchBox->items) && (switchBox->autoRunHandlers) )
				{
						//start handler, if exists
						if (switchBox->items[switchBox->currentElementNumber - 1].handler)
						{
								switchBox->items[switchBox->currentElementNumber - 1].handler(switchBox->currentElementNumber);
						}
				}				
		}		
		//return
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* switch box go to next element */
URG_RESULT UnniUiSwitchBoxNext(UNNI_UI_SWITCHBOX* switchBox)
{
		//check input data
		if (!switchBox) return URG_INPUT_DATA_ERROR;

		//check flags
		if (!( (switchBox->flags & ELEMENT_VISIBLE) &&
		       (switchBox->flags & ELEMENT_ENABLED) &&
		       (switchBox->flags & ELEMENT_ACTIVE) )) return URG_FUNC_NOT_AVAILABLE;	
		//go to next
		if (switchBox->currentElementNumber < switchBox->itemsCount)
		{
				//inc cursor
				switchBox->currentElementNumber++;
				//check items
				if ( (switchBox->items) && (switchBox->autoRunHandlers) )
				{
						//start handler, if exists
						if (switchBox->items[switchBox->currentElementNumber - 1].handler)
						{
								switchBox->items[switchBox->currentElementNumber - 1].handler(switchBox->currentElementNumber);
						}
				}				
		}		
		//return
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* switch box run handler for current element */
URG_RESULT UnniUiSwitchBoxRunHandler(UNNI_UI_SWITCHBOX* switchBox)
{
		//check input data
		if (!switchBox) return URG_INPUT_DATA_ERROR;

		//check flags
		if (!( (switchBox->flags & ELEMENT_VISIBLE) &&
		       (switchBox->flags & ELEMENT_ENABLED) &&
		       (switchBox->flags & ELEMENT_ACTIVE) )) return URG_FUNC_NOT_AVAILABLE;
		
		//check items
		if (switchBox->items)
		{
				//start handler, if exists
				if (switchBox->items[switchBox->currentElementNumber - 1].handler)
				{
						switchBox->items[switchBox->currentElementNumber - 1].handler(switchBox->currentElementNumber);
				}
		}		
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* switch box get current element number */
URG_RESULT UnniUiSwitchBoxGetCurrent(UNNI_UI_SWITCHBOX* switchBox, uint32_t* currentElement)
{
		//check input data
		if (!switchBox) return URG_INPUT_DATA_ERROR;
		if (!currentElement) return URG_INPUT_DATA_ERROR;

		*currentElement = switchBox->currentElementNumber;
	
		//return
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* switch box set current element number */
URG_RESULT UnniUiSwitchBoxSetCurrent(UNNI_UI_SWITCHBOX* switchBox, uint32_t currentElement, uint8_t elementHandlerStart)
{
		//check input data
		if (!switchBox) return URG_INPUT_DATA_ERROR;
		if ( (currentElement == 0) || (currentElement > switchBox->itemsCount) ) return URG_INPUT_DATA_ERROR;
	
		//set new current element
		switchBox->currentElementNumber = currentElement;
	
		//check items
		if ( (elementHandlerStart) && (switchBox->items) )
		{
				//start handler, if exists
				if (switchBox->items[switchBox->currentElementNumber - 1].handler)
				{
						switchBox->items[switchBox->currentElementNumber - 1].handler(switchBox->currentElementNumber);
				}
		}
		
		//return
		return URG_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* switch box draw */
void UnniUiSwitchBoxUpdate(void* switchBox, int32_t posX, int32_t posY, UNNI_UI_ELEMENT_FLAGS flags)
{
		UNNI_UI_SWITCHBOX* swBox;
		char formatStr[65] = "";
	
		//check input data
		if (!switchBox) return;
	
		//get struct pointer
		swBox = (UNNI_UI_SWITCHBOX*)switchBox;
		//import flags
		swBox->flags = flags;
		
		//check visible
		if (flags & ELEMENT_VISIBLE)
		{
				//format string
				snprintf(formatStr, 64, "%s%s", swBox->staticText, swBox->items[swBox->currentElementNumber - 1].text);
				//draw text
				URGDrawTextInBufFast(swBox->font, (uint8_t*)formatStr, posX + 2, posY - 1, swBox->canvas);
				//draw rect, if active
				if ((flags & ELEMENT_ENABLED) && (flags & ELEMENT_ACTIVE))
				{
						URGDrawRectInBufFast(posX, posY, posX + swBox->width - 1, posY - swBox->height + 1, 1, swBox->canvas);								
				}
		}
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* message box UI component */

//---------------------------------------------------------------------------------------------------------//
/* message box struct init */
UNNI_UI_MESSAGEBOX* UnniUiMessageBoxInit(int32_t posLeft, int32_t posBottom, uint32_t frameWidth, uint32_t frameHeight,
                                         URG_FONT_CONFIG* font, URG_FONT_CONFIG* captionFont, URG_CANVAS* canvas)
{
		UNNI_UI_MESSAGEBOX* newMessageBox;
	
		//check input data
		if ( (!font) || (!captionFont) || (!canvas) ) return 0;
		if ((frameWidth == 0) || (frameHeight == 0)) return 0;
	
		//allocate memory for new component and check
		newMessageBox = (UNNI_UI_MESSAGEBOX*)URG_GET_MEMORY(sizeof(UNNI_UI_MESSAGEBOX));
		if (!newMessageBox) return 0;
	
		//set config for new component
		newMessageBox->canvas = canvas;
		newMessageBox->posLeft = posLeft;
		newMessageBox->posBottom = posBottom;
		newMessageBox->frameWidth = frameWidth;
		newMessageBox->frameHeight = frameHeight;
		newMessageBox->font = font;
		newMessageBox->captionFont = captionFont;
		newMessageBox->captionText = 0;
		newMessageBox->messageText = 0;
		newMessageBox->framesDrawTime = 0;
		newMessageBox->msgBxDataAccessLock = 0;
	
		//return
		return newMessageBox;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* message box show */
URG_RESULT UnniUiMessageBoxShow(UNNI_UI_MESSAGEBOX* messageBox, const uint8_t* captionText, const uint8_t* messageText, uint32_t framesCount)
{
		uint32_t textLen;
		
		//check input data
		if (!messageBox) return URG_INPUT_DATA_ERROR;
		//check message box data access
		if (messageBox->msgBxDataAccessLock) return URG_FUNC_NOT_AVAILABLE;
	
		//lock message box data access
		messageBox->msgBxDataAccessLock = 1;
	
		//clear messagebox data
		//caption text
		if (messageBox->captionText)
		{
				//free memory
				URG_FREE_MEMORY(messageBox->captionText);
				messageBox->captionText = 0;
		}
		//message text
		if (messageBox->messageText)
		{
				//free memory
				URG_FREE_MEMORY(messageBox->messageText);
				messageBox->messageText = 0;
		}
		messageBox->framesDrawTime = 0;
	
		//copy captionText
		if (captionText)
		{
				textLen = strlen((const char*)captionText);
				if (textLen)
				{
						//get memory
						messageBox->captionText = (uint8_t*)URG_GET_MEMORY(textLen + 1);
						if (!messageBox->captionText)
						{
								//unlock message box data access
								messageBox->msgBxDataAccessLock = 0;
								//exit
								return URG_MEMORY_ERROR;
						}
						memcpy(messageBox->captionText, captionText, textLen);
						messageBox->captionText[textLen] = '\0';
				}
		}
		//copy messageText
		if (messageText)
		{
				textLen = strlen((const char*)messageText);
				if (textLen)
				{
						//get memory
						messageBox->messageText = (uint8_t*)URG_GET_MEMORY(textLen + 1);
						if (!messageBox->messageText)
						{
								//unlock message box data access
								messageBox->msgBxDataAccessLock = 0;
								//exit
								return URG_MEMORY_ERROR;
						}
						memcpy(messageBox->messageText, messageText, textLen);
						messageBox->messageText[textLen] = '\0';
				}
		}
		//copy frames count
		messageBox->framesDrawTime = framesCount;
		
		//unlock message box data access
		messageBox->msgBxDataAccessLock = 0;

		//return
		return URG_OK;	
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* message box draw function */
URG_RESULT UnniUiMessageBoxDraw(UNNI_UI_MESSAGEBOX* messageBox)
{
		uint8_t lastColorMode;
		int32_t posVarX, posVarY;
	
		//check input data
		if (!messageBox) return URG_INPUT_DATA_ERROR;
		//check message box data access
		if (messageBox->msgBxDataAccessLock) return URG_FUNC_NOT_AVAILABLE;
	
		//check draw counter (time)
		if (!messageBox->framesDrawTime)
		{
				//clear messagebox data
				//caption text
				if (messageBox->captionText)
				{
						//free memory
						URG_FREE_MEMORY(messageBox->captionText);
						messageBox->captionText = 0;
				}
				//message text
				if (messageBox->messageText)
				{
						//free memory
						URG_FREE_MEMORY(messageBox->messageText);
						messageBox->messageText = 0;
				}
				//exit
				return URG_OK;				
		}
		
		//dec counter
		messageBox->framesDrawTime--;
	
		//draw main filled rect
		URGDrawFilledRectInBufFast(messageBox->posLeft, messageBox->posBottom,
		                           messageBox->posLeft + messageBox->frameWidth - 1, messageBox->posBottom - messageBox->frameHeight + 1,
		                           messageBox->canvas);
		
		//inverse color
		lastColorMode = messageBox->canvas->colorMode;
		if (lastColorMode == COLOR_WB_1BIT)
		{
				messageBox->canvas->colorMode = COLOR_WB_1BIT_INVERSE;
		} else
		{
				messageBox->canvas->colorMode = COLOR_WB_1BIT;
		}
		
		//draw separator line
		posVarY = messageBox->posBottom - messageBox->frameHeight + messageBox->captionFont->symbolHeight + 4;
		URGDrawLineXYInBufFast(messageBox->posLeft + 1, posVarY, messageBox->posLeft + messageBox->frameWidth - 2, posVarY, 1, messageBox->canvas);
		
		//draw caption text
		if (messageBox->captionText)
		{
				URGDrawTextInBufFast(messageBox->captionFont, messageBox->captionText, messageBox->posLeft + 2, posVarY - 2, messageBox->canvas);
		}
		
		//draw message text
		if (messageBox->messageText)
		{
				//calc str center pos X
				posVarX = (messageBox->frameWidth - 4 - strlen((const char*)messageBox->messageText) * messageBox->font->symbolWidth) / 2;
				posVarX = messageBox->posLeft + 1 + posVarX;
				//cal str center pos Y
				posVarY = messageBox->posBottom - (messageBox->posBottom - posVarY - messageBox->font->symbolHeight) / 2;
				URGDrawTextInBufFast(messageBox->font, messageBox->messageText, posVarX, posVarY, messageBox->canvas);
		}
		
		//return color mode
		messageBox->canvas->colorMode = lastColorMode;
	
		//return
		return URG_OK;	
}
//---------------------------------------------------------------------------------------------------------//

//************************************end of UnniRamGraphicUIObjects.c*************************************//
