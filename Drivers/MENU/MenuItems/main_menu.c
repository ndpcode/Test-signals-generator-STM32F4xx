//*********************************************************************************************************//
//generator 407 main menu source file
//Created 18.08.2018
//Created by Novikov Dmitry
//*********************************************************************************************************//

//*********************************************************************************************************//
/* includes */
#include "main_menu.h"

//*********************************************************************************************************//
/* defines, constants and macro */

//*********************************************************************************************************//
/* Private variables --------------------------------------------------------------------------------------*/
D128X64_BUFFER displayBuffer;
URG_RAW_BUFFER urgBuffer = {DATA_FORMAT_1BIT_OY, 0};
URG_CANVAS displayCanvas = {COLOR_WB_1BIT, URG_COLOR_1B_BLACK, 128, 64, &urgBuffer};

//*********************************************************************************************************//
/* main menu functions */

//---------------------------------------------------------------------------------------------------------//
/* reset config global variables of menu */
void ResetCommonMenuData(void)
{
		//clear buffer array
		memset(&displayBuffer.raw[0], 0, DISPLAY_BUFFER_SIZE);
	
		//config urg ram buffer
		urgBuffer.dataFormat = DATA_FORMAT_1BIT_OY;
		urgBuffer.raw = (uint32_t*)&displayBuffer.raw[0];
	
		//config display canvas
		displayCanvas.colorMode = COLOR_WB_1BIT;
		displayCanvas.color.color1B = URG_COLOR_1B_BLACK;
		displayCanvas.screenWidth = 128;
		displayCanvas.screenHeight = 64;
		displayCanvas.buffer = &urgBuffer;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* init config of menu dialog */
void MenuDialogWNDInitConfig(MENU_DIALOG_WND* menuDialogStruct)
{
		URG_RECT menuElementsRegionRect;
		uint32_t menuElementHeight;
		uint32_t menuElementsCount;
		uint32_t index, counter;
	
		//set menu elements region position
		menuElementsRegionRect.startPoint.x = 0;
		menuElementsRegionRect.startPoint.y = menuDialogStruct->wndBorder.startPoint.y;
		menuElementsRegionRect.endPoint.x = menuDialogStruct->wndBorder.endPoint.x;
		menuElementsRegionRect.endPoint.y = menuDialogStruct->wndBorder.endPoint.y + menuDialogStruct->wndNameFont.symbolHeight + menuDialogStruct->wndBorderThickness;

		//calc height of one menu element and count of displayed menu elements at the same time
		menuElementHeight = menuDialogStruct->menuElementFont.symbolHeight + 2;
		menuDialogStruct->displayedElemCount = (menuElementsRegionRect.startPoint.y - menuElementsRegionRect.endPoint.y + 1)/menuElementHeight;
	
		//check menu elements
		index = 0;
		while (menuDialogStruct->menuElements[index].text)
		{				
				if ( index < menuDialogStruct->displayedElemCount )
				{
						menuDialogStruct->menuElements[index].elemementVisible = 1;						
				} else
				{
						menuDialogStruct->menuElements[index].elemementVisible = 0;					
				}	
				index++;				
		}
		menuDialogStruct->menuElementsCount = index;
		if ( index )
		{
				menuDialogStruct->activeMenuElementIndex = 0;				
		} else
		{
				menuDialogStruct->activeMenuElementIndex = 0xFFFFFFFF;
		}		

		//check displayedElemCount 
		if (menuDialogStruct->displayedElemCount > menuDialogStruct->menuElementsCount)
		{
				menuDialogStruct->displayedElemCount = menuDialogStruct->menuElementsCount;
		}
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* draw dialog with menu strings */
void DrawMenuDialogWND(MENU_DIALOG_WND* menuDialogStruct)
{
		URG_RECT menuElementsRegionRect;
		URG_RECT activeElementRect;
		URG_POINT textPoint;
		uint32_t menuElementHeight;
		uint32_t index, counter;
		uint32_t scrollBarHeight;
		uint8_t numberStr[20] = "";
	
		//set menu elements region position
		menuElementsRegionRect.startPoint.x = 0;
		menuElementsRegionRect.startPoint.y = menuDialogStruct->wndBorder.startPoint.y;
		menuElementsRegionRect.endPoint.x = menuDialogStruct->wndBorder.endPoint.x - MENU_SCROLL_BAR_WIDTH;
		menuElementsRegionRect.endPoint.y = menuDialogStruct->wndBorder.endPoint.y + menuDialogStruct->wndNameFont.symbolHeight + menuDialogStruct->wndBorderThickness + 1;
	
		//draw window border
		URGDrawRectInBufFast(menuDialogStruct->wndBorder.startPoint.x,
		                     menuDialogStruct->wndBorder.startPoint.y,
		                     menuDialogStruct->wndBorder.endPoint.x,
		                     menuDialogStruct->wndBorder.endPoint.y,
		                     menuDialogStruct->wndBorderThickness, menuDialogStruct->displayCanvas);
		//draw window name separator line
		URGDrawLineXYInBufFast(menuDialogStruct->wndBorder.startPoint.x,
		                       menuElementsRegionRect.endPoint.y,
		                       menuDialogStruct->wndBorder.endPoint.x,
		                       menuElementsRegionRect.endPoint.y,
		                       menuDialogStruct->wndBorderThickness, menuDialogStruct->displayCanvas);
		//draw window name text
		URGDrawTextInBufFast(&menuDialogStruct->wndNameFont, menuDialogStruct->wndName,
							 menuDialogStruct->wndBorder.startPoint.x + menuDialogStruct->wndBorderThickness + 1,
							 menuElementsRegionRect.endPoint.y - 1 - MENU_ELEM_TEXT_BORDER_INDENT,
							 menuDialogStruct->displayCanvas);
		//calc height of one menu element
		menuElementHeight = menuDialogStruct->menuElementFont.symbolHeight + MENU_ELEM_TEXT_OY_INDENT*2;
		
		//draw menu elements
		if (menuDialogStruct->menuElementsCount)
		{
				index = 0;
				counter = 0;
				while (menuDialogStruct->menuElements[index].text)
				{
						//if element visible
						if (menuDialogStruct->menuElements[index].elemementVisible)
						{
								counter++;
								//convert string
								snprintf((char*)&numberStr[0], 19, "%s", menuDialogStruct->menuElements[index].text);
								//calc text border
								textPoint.x = menuElementsRegionRect.startPoint.x + MENU_ELEM_TEXT_OX_INDENT;
								textPoint.y = menuElementsRegionRect.endPoint.y + counter*menuElementHeight + MENU_ELEM_TEXT_OY_INDENT - MENU_ELEM_TEXT_BORDER_INDENT;
								URGDrawTextInBufFast(&menuDialogStruct->menuElementFont, (uint8_t*)numberStr, textPoint.x, textPoint.y, menuDialogStruct->displayCanvas);
								//if element is active and enabled, highlight
								if ( (index == menuDialogStruct->activeMenuElementIndex) && (menuDialogStruct->menuElements[index].elementEnabled) )
								{
										URGDrawRectInBufFast(menuElementsRegionRect.startPoint.x + 2,
																				 textPoint.y + MENU_ELEM_TEXT_OY_INDENT + MENU_ELEM_TEXT_BORDER_INDENT,
																				 menuElementsRegionRect.endPoint.x - 2,
																				 textPoint.y - menuElementHeight + MENU_ELEM_TEXT_OY_INDENT + MENU_ELEM_TEXT_BORDER_INDENT,
																				 menuDialogStruct->activeMenuElementThickness, menuDialogStruct->displayCanvas);								
								}
						}
						index++;			
				}		
				//draw scroll bar
				activeElementRect.startPoint.x = menuElementsRegionRect.endPoint.x;
				activeElementRect.startPoint.y = menuElementsRegionRect.startPoint.y;
				activeElementRect.endPoint.x = menuDialogStruct->wndBorder.endPoint.x;
				activeElementRect.endPoint.y = menuElementsRegionRect.endPoint.y;
				URGDrawRectInBufFast(activeElementRect.startPoint.x, activeElementRect.startPoint.y, activeElementRect.endPoint.x, activeElementRect.endPoint.y, 1, menuDialogStruct->displayCanvas);
				scrollBarHeight = (activeElementRect.startPoint.y - activeElementRect.endPoint.y - 1)/menuDialogStruct->menuElementsCount;
				activeElementRect.startPoint.y = menuElementsRegionRect.endPoint.y + scrollBarHeight*(menuDialogStruct->activeMenuElementIndex + 1) +1;
				activeElementRect.endPoint.y = activeElementRect.startPoint.y - scrollBarHeight;
				if ( activeElementRect.endPoint.y >= activeElementRect.startPoint.y )
				{
						activeElementRect.endPoint.y = activeElementRect.startPoint.y - 1;
				}
				URGDrawFilledRectInBufFast(activeElementRect.startPoint.x, activeElementRect.startPoint.y, activeElementRect.endPoint.x, activeElementRect.endPoint.y, menuDialogStruct->displayCanvas);
		}		
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* menu dialog wnd - go to next active element */
void MenuDialogWNDGoToNext(MENU_DIALOG_WND* menuDialogStruct)
{
		uint32_t index;
	
		//check active element index and increment it
		if (menuDialogStruct->activeMenuElementIndex < (menuDialogStruct->menuElementsCount - 1))
		{
				menuDialogStruct->activeMenuElementIndex++;
				menuDialogStruct->menuElements[menuDialogStruct->activeMenuElementIndex].elemementVisible = 1;
				if ( menuDialogStruct->activeMenuElementIndex > (menuDialogStruct->displayedElemCount - 1) )
				{
						menuDialogStruct->menuElements[menuDialogStruct->activeMenuElementIndex - menuDialogStruct->displayedElemCount].elemementVisible = 0;						
				}
		} else
		{
				menuDialogStruct->activeMenuElementIndex = 0;
				for (index = menuDialogStruct->menuElementsCount - menuDialogStruct->displayedElemCount; index < menuDialogStruct->menuElementsCount; index++)
				{
						menuDialogStruct->menuElements[index].elemementVisible = 0;						
				}
				for (index = 0; index < menuDialogStruct->displayedElemCount; index++)
				{
						menuDialogStruct->menuElements[index].elemementVisible = 1;						
				}
		}
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* menu dialog wnd - go to prev active element */
void MenuDialogWNDGoToPrev(MENU_DIALOG_WND* menuDialogStruct)
{
		uint32_t index;
		
		//check active element and decrement it
		if ( menuDialogStruct->activeMenuElementIndex > 0 )
		{
				menuDialogStruct->activeMenuElementIndex--;
				menuDialogStruct->menuElements[menuDialogStruct->activeMenuElementIndex].elemementVisible = 1;
				if ( menuDialogStruct->menuElementsCount > menuDialogStruct->activeMenuElementIndex + menuDialogStruct->displayedElemCount )
				{
						menuDialogStruct->menuElements[menuDialogStruct->activeMenuElementIndex + menuDialogStruct->displayedElemCount].elemementVisible = 0;						
				}
		} else
		{
				menuDialogStruct->activeMenuElementIndex = menuDialogStruct->menuElementsCount - 1;
				for (index = 0; index < menuDialogStruct->displayedElemCount; index++)
				{
						menuDialogStruct->menuElements[index].elemementVisible = 0;						
				}
				for (index = menuDialogStruct->menuElementsCount - menuDialogStruct->displayedElemCount; index < menuDialogStruct->menuElementsCount; index++)
				{
						menuDialogStruct->menuElements[index].elemementVisible = 1;						
				}
		}
}
//---------------------------------------------------------------------------------------------------------//

//*******************************************end of main_menu.c********************************************//
