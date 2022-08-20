
#include "bsc_controls.h"

//**********************************************************************************************************************//
//Внутренние определения вспомогательных структур и функций
//**********************************************************************************************************************//
controlsBaseStruct *_buttonsData;
controlsBaseStruct *_valcoderData;

//---------------------------------------------------------------------------------------------------------//
/*  */
static controlsBaseStruct* getLastItem(controlsBaseStruct* data)
{
		controlsBaseStruct* lastElement = data;
		if ( !data ) return 0;
		if ( !data->nextControlElement ) return data;
		while ( lastElement->nextControlElement )
		{
				lastElement = (controlsBaseStruct*)lastElement->nextControlElement;
		};
		return lastElement;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
static controlsBaseStruct* pushNewElement(controlsBaseStruct** data)
{
		//добавляем новый элемент
		controlsBaseStruct* newControl = 0;
		controlsBaseStruct* lastControl = getLastItem(*data);
		//создаем новый элемент
		newControl = (controlsBaseStruct*)STM32FX_CONTROLS_GET_MEMORY( sizeof(controlsBaseStruct) );
		//проверка и выход если память не выделена
		if ( !newControl  ) return 0;
		if ( !*data )
		{
				*data = newControl; 
		} else if ( lastControl )
		{
				lastControl->nextControlElement = newControl;
		};
		//проверка
		if ( ( !*data ) && ( !lastControl ) )
		{
			//удаляем, что-то пошло не так...
			STM32FX_CONTROLS_FREE_MEMORY(newControl);
			return 0;
		};
	return newControl;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
static controlsBaseStruct* popLastElement(controlsBaseStruct** data)
{
		//удаляем последний элемент и возвращаем указатель на предпоследний
		controlsBaseStruct* lastControl = 0;
		controlsBaseStruct* penultControl = 0;
		if ( !*data ) return 0;
		lastControl = *data;
		while ( lastControl->nextControlElement )
		{
				penultControl = lastControl;
				lastControl = (controlsBaseStruct*)lastControl->nextControlElement;
		};
		STM32FX_CONTROLS_FREE_MEMORY(lastControl);
		if ( penultControl )
		{
				penultControl->nextControlElement = 0;
		} else
		{
				*data = 0;
		};
	return penultControl;
}
//---------------------------------------------------------------------------------------------------------//

//**********************************************************************************************************************//
//Пользовательские функции для использования вне модуля
//**********************************************************************************************************************//

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t controlsDataClear(void)
{
	while ( _buttonsData )
	{
			popLastElement(&_buttonsData);
	}			
	while ( _valcoderData )
	{
			popLastElement(&_valcoderData);
	}			
	return 1;
};
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t controlsRegNewButton(char portLetter, uint8_t pinNumber, void (*controlClickHandler)(void),
                             SYS_EVENTS_DATA eventClickCode, SYS_EVENTS_DATA eventPressedCode)
{
		controlsBaseStruct* newButton = 0;
		//проверка буквы порта
		if ( !strchr("ABCDEFGHIJK", portLetter) ) return 0;
		//проверка номера пина
		if ( pinNumber > 15 ) return 0;
	
		//создаем новый элемент
		newButton = pushNewElement(&_buttonsData);
		if ( !newButton ) return 0;
	
		//заполняем данные элемента
		newButton->portAddress = GetPortAddressFromLetter(portLetter);
		newButton->pinNumber = pinNumber;
		newButton->controlClickHandler = controlClickHandler;
		newButton->eventCodeClick = eventClickCode;
		newButton->eventCodePressed = eventPressedCode;
		return 1;	
};
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
uint8_t controlsRegNewValcoder(char portLetterCCW, uint8_t pinNumberCCW,
	                             char portLetterCW, uint8_t pinNumberCW,
															 void (*ccwHandler)(void), SYS_EVENTS_DATA eventCodeCCW,
															 void (*cwHandler)(void), SYS_EVENTS_DATA eventCodeCW)
{
		controlsBaseStruct* newValcoder = 0;
		//проверка буквы порта
		if ( !strchr("ABCDEFGHIJK", portLetterCCW) ) return 0;
		if ( !strchr("ABCDEFGHIJK", portLetterCW) ) return 0;
		//проверка номера пина
		if ( ( pinNumberCCW > 15 ) || ( pinNumberCW > 15 ) ) return 0;
	
		//создаем новый элемент CCW
		newValcoder = pushNewElement(&_valcoderData);
		if ( !newValcoder ) return 0;
		//заполняем данные элемента
		newValcoder->portAddress = GetPortAddressFromLetter(portLetterCCW);
		newValcoder->pinNumber = pinNumberCCW;
		newValcoder->controlClickHandler = ccwHandler;
		newValcoder->eventCodeClick = eventCodeCCW;
		newValcoder->eventCodePressed = 0;
	
		//создаем новый элемент CW
		newValcoder = pushNewElement(&_valcoderData);
		if ( !newValcoder ) 
		{
				//удаляем предыдущий элемент и выходим
				popLastElement(&_valcoderData);
				return 0;
		};
		//заполняем данные элемента
		newValcoder->portAddress = GetPortAddressFromLetter(portLetterCW);
		newValcoder->pinNumber = pinNumberCW;
		newValcoder->controlClickHandler = cwHandler;
		newValcoder->eventCodeClick = eventCodeCW;
		newValcoder->eventCodePressed = 0;
	
		return 1;	
};
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
SYS_EVENTS_DATA controlsUpdateEvents(uint32_t params)
{
		//обработка событий и возврат результатов
		uint32_t returnedEvents = 0;
		controlsBaseStruct* controlElement = 0;
		controlsBaseStruct* controlElementCW = 0;
	
		//опрос кнопок
		controlElement = _buttonsData;
		while ( controlElement )
		{
				//проверяем низкий уровень на пин
				if ( GPIOReadPinP(controlElement->portAddress, controlElement->pinNumber) == 0 )
				{
						if ( !controlElement->controlBusyFlag )
						{
								//если кнопка нажата впервые
								//флаг
								controlElement->controlBusyFlag = 1;
								//событие по кнопке
								//если возвращаем код события
								if ( params & UPDEVENTS_GETEVENTS )
										returnedEvents |= controlElement->eventCodeClick | controlElement->eventCodePressed;
								//если вызываем обработчик события
								if ( ( params & UPDEVENTS_HANDLERS_LAUNCH ) && ( controlElement->controlClickHandler ) )
										controlElement->controlClickHandler();
						} else
						{
								//кнопка срабатывает повторно/удерживается
								//если возвращаем код события
								controlElement->controlBusyFlag = 1;
								if ( params & UPDEVENTS_GETEVENTS ) returnedEvents |= controlElement->eventCodePressed;
						};
				} else
				{
						controlElement->controlBusyFlag = 0;				
				};
				//переключаемся на следующий элемент
				controlElement = (controlsBaseStruct*)controlElement->nextControlElement;
		};
		
		//опрос валкодеров
		controlElement = _valcoderData;
		while ( controlElement )
		{
				controlElementCW = (controlsBaseStruct*)controlElement->nextControlElement;
				if ( !controlElementCW ) return VALCODER_ERROR;
				//опрос CCW - против часовой
				if ( ( GPIOReadPinP(controlElement->portAddress, controlElement->pinNumber) == 0 ) &&
				     ( GPIOReadPinP(controlElementCW->portAddress, controlElementCW->pinNumber) != 0 ) )
				{
							//если событие впервые
						if ( !controlElement->controlBusyFlag )
						{
								//устанавливаем флаги события
								controlElement->controlBusyFlag = 1;
								controlElementCW->controlBusyFlag = 1;
								//если возвращаем код события
								if ( params & UPDEVENTS_GETEVENTS ) returnedEvents |= controlElement->eventCodeClick;
								//если вызываем обработчик события
								if ( ( params & UPDEVENTS_HANDLERS_LAUNCH ) && ( controlElement->controlClickHandler ) )
										controlElement->controlClickHandler();				
						}
				//опрос CW - по часовой
				} else if ( ( GPIOReadPinP(controlElement->portAddress, controlElement->pinNumber) != 0 ) &&
		                ( GPIOReadPinP(controlElementCW->portAddress, controlElementCW->pinNumber) == 0 ) )
				{
						//если событие впервые
						if ( !controlElementCW->controlBusyFlag )
						{
								//устанавливаем флаги события
								controlElement->controlBusyFlag = 1;
								controlElementCW->controlBusyFlag = 1;
								//если возвращаем код события
								if ( params & UPDEVENTS_GETEVENTS ) returnedEvents |= controlElementCW->eventCodeClick;
								//если вызываем обработчик события
								if ( ( params & UPDEVENTS_HANDLERS_LAUNCH ) && ( controlElementCW->controlClickHandler ) )
										controlElementCW->controlClickHandler();				
						}
				//опрос для сброса флагов
				} else if ( ( GPIOReadPinP(controlElement->portAddress, controlElement->pinNumber) != 0 ) &&
				            ( GPIOReadPinP(controlElementCW->portAddress, controlElementCW->pinNumber) != 0 ) )
				{
						//сброс флагов
						controlElement->controlBusyFlag = 0;
						controlElementCW->controlBusyFlag = 0;
				};
				//переключаемся на следующий элемент
				controlElement = (controlsBaseStruct*)controlElementCW->nextControlElement;
		}
	
		return returnedEvents;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
SYS_EVENTS_DATA controlsUpdateButtons(uint32_t params)
{
		//обработка событий и возврат результатов
		uint32_t returnedEvents = 0;
		controlsBaseStruct* controlElement = 0;
	
		//опрос кнопок
		controlElement = _buttonsData;
		while ( controlElement )
		{
				//проверяем низкий уровень на пин
				if ( GPIOReadPinP(controlElement->portAddress, controlElement->pinNumber) == 0 )
				{
						if ( !controlElement->controlBusyFlag )
						{
								//если кнопка нажата впервые
								//флаг
								controlElement->controlBusyFlag = 1;
								//событие по кнопке
								//если возвращаем код события
								if ( params & UPDEVENTS_GETEVENTS )
										returnedEvents |= controlElement->eventCodeClick | controlElement->eventCodePressed;
								//если вызываем обработчик события
								if ( ( params & UPDEVENTS_HANDLERS_LAUNCH ) && ( controlElement->controlClickHandler ) )
										controlElement->controlClickHandler();
						} else
						{
								//кнопка срабатывает повторно/удерживается
								//если возвращаем код события
								controlElement->controlBusyFlag = 1;
								if ( params & UPDEVENTS_GETEVENTS ) returnedEvents |= controlElement->eventCodePressed;
						};
				} else
				{
						controlElement->controlBusyFlag = 0;				
				};
				//переключаемся на следующий элемент
				controlElement = (controlsBaseStruct*)controlElement->nextControlElement;
		};
	
		return returnedEvents;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
SYS_EVENTS_DATA controlsUpdateValcoder(uint32_t params)
{
		//обработка событий и возврат результатов
		uint32_t returnedEvents = 0;
		controlsBaseStruct* controlElement = 0;
		controlsBaseStruct* controlElementCW = 0;
		
		//опрос валкодеров
		controlElement = _valcoderData;
		while ( controlElement )
		{
				controlElementCW = (controlsBaseStruct*)controlElement->nextControlElement;
				if ( !controlElementCW ) return VALCODER_ERROR;
				//опрос CCW - против часовой
				if ( ( GPIOReadPinP(controlElement->portAddress, controlElement->pinNumber) == 0 ) &&
				     ( GPIOReadPinP(controlElementCW->portAddress, controlElementCW->pinNumber) != 0 ) )
				{
							//если событие впервые
						if ( !controlElement->controlBusyFlag )
						{
								//устанавливаем флаги события
								controlElement->controlBusyFlag = 1;
								controlElementCW->controlBusyFlag = 1;
								//если возвращаем код события
								if ( params & UPDEVENTS_GETEVENTS ) returnedEvents |= controlElement->eventCodeClick;
								//если вызываем обработчик события
								if ( ( params & UPDEVENTS_HANDLERS_LAUNCH ) && ( controlElement->controlClickHandler ) )
										controlElement->controlClickHandler();				
						}
				//опрос CW - по часовой
				} else if ( ( GPIOReadPinP(controlElement->portAddress, controlElement->pinNumber) != 0 ) &&
		                ( GPIOReadPinP(controlElementCW->portAddress, controlElementCW->pinNumber) == 0 ) )
				{
						//если событие впервые
						if ( !controlElementCW->controlBusyFlag )
						{
								//устанавливаем флаги события
								controlElement->controlBusyFlag = 1;
								controlElementCW->controlBusyFlag = 1;
								//если возвращаем код события
								if ( params & UPDEVENTS_GETEVENTS ) returnedEvents |= controlElementCW->eventCodeClick;
								//если вызываем обработчик события
								if ( ( params & UPDEVENTS_HANDLERS_LAUNCH ) && ( controlElementCW->controlClickHandler ) )
										controlElementCW->controlClickHandler();				
						}
				//опрос для сброса флагов
				} else if ( ( GPIOReadPinP(controlElement->portAddress, controlElement->pinNumber) != 0 ) &&
				            ( GPIOReadPinP(controlElementCW->portAddress, controlElementCW->pinNumber) != 0 ) )
				{
						//сброс флагов
						controlElement->controlBusyFlag = 0;
						controlElementCW->controlBusyFlag = 0;
				};
				//переключаемся на следующий элемент
				controlElement = (controlsBaseStruct*)controlElementCW->nextControlElement;
		}
		
		return returnedEvents;
}
//---------------------------------------------------------------------------------------------------------//
