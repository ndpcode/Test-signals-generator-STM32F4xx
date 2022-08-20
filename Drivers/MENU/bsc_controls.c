
#include "bsc_controls.h"

//**********************************************************************************************************************//
//���������� ����������� ��������������� �������� � �������
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
		//��������� ����� �������
		controlsBaseStruct* newControl = 0;
		controlsBaseStruct* lastControl = getLastItem(*data);
		//������� ����� �������
		newControl = (controlsBaseStruct*)STM32FX_CONTROLS_GET_MEMORY( sizeof(controlsBaseStruct) );
		//�������� � ����� ���� ������ �� ��������
		if ( !newControl  ) return 0;
		if ( !*data )
		{
				*data = newControl; 
		} else if ( lastControl )
		{
				lastControl->nextControlElement = newControl;
		};
		//��������
		if ( ( !*data ) && ( !lastControl ) )
		{
			//�������, ���-�� ����� �� ���...
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
		//������� ��������� ������� � ���������� ��������� �� �������������
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
//���������������� ������� ��� ������������� ��� ������
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
		//�������� ����� �����
		if ( !strchr("ABCDEFGHIJK", portLetter) ) return 0;
		//�������� ������ ����
		if ( pinNumber > 15 ) return 0;
	
		//������� ����� �������
		newButton = pushNewElement(&_buttonsData);
		if ( !newButton ) return 0;
	
		//��������� ������ ��������
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
		//�������� ����� �����
		if ( !strchr("ABCDEFGHIJK", portLetterCCW) ) return 0;
		if ( !strchr("ABCDEFGHIJK", portLetterCW) ) return 0;
		//�������� ������ ����
		if ( ( pinNumberCCW > 15 ) || ( pinNumberCW > 15 ) ) return 0;
	
		//������� ����� ������� CCW
		newValcoder = pushNewElement(&_valcoderData);
		if ( !newValcoder ) return 0;
		//��������� ������ ��������
		newValcoder->portAddress = GetPortAddressFromLetter(portLetterCCW);
		newValcoder->pinNumber = pinNumberCCW;
		newValcoder->controlClickHandler = ccwHandler;
		newValcoder->eventCodeClick = eventCodeCCW;
		newValcoder->eventCodePressed = 0;
	
		//������� ����� ������� CW
		newValcoder = pushNewElement(&_valcoderData);
		if ( !newValcoder ) 
		{
				//������� ���������� ������� � �������
				popLastElement(&_valcoderData);
				return 0;
		};
		//��������� ������ ��������
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
		//��������� ������� � ������� �����������
		uint32_t returnedEvents = 0;
		controlsBaseStruct* controlElement = 0;
		controlsBaseStruct* controlElementCW = 0;
	
		//����� ������
		controlElement = _buttonsData;
		while ( controlElement )
		{
				//��������� ������ ������� �� ���
				if ( GPIOReadPinP(controlElement->portAddress, controlElement->pinNumber) == 0 )
				{
						if ( !controlElement->controlBusyFlag )
						{
								//���� ������ ������ �������
								//����
								controlElement->controlBusyFlag = 1;
								//������� �� ������
								//���� ���������� ��� �������
								if ( params & UPDEVENTS_GETEVENTS )
										returnedEvents |= controlElement->eventCodeClick | controlElement->eventCodePressed;
								//���� �������� ���������� �������
								if ( ( params & UPDEVENTS_HANDLERS_LAUNCH ) && ( controlElement->controlClickHandler ) )
										controlElement->controlClickHandler();
						} else
						{
								//������ ����������� ��������/������������
								//���� ���������� ��� �������
								controlElement->controlBusyFlag = 1;
								if ( params & UPDEVENTS_GETEVENTS ) returnedEvents |= controlElement->eventCodePressed;
						};
				} else
				{
						controlElement->controlBusyFlag = 0;				
				};
				//������������� �� ��������� �������
				controlElement = (controlsBaseStruct*)controlElement->nextControlElement;
		};
		
		//����� ����������
		controlElement = _valcoderData;
		while ( controlElement )
		{
				controlElementCW = (controlsBaseStruct*)controlElement->nextControlElement;
				if ( !controlElementCW ) return VALCODER_ERROR;
				//����� CCW - ������ �������
				if ( ( GPIOReadPinP(controlElement->portAddress, controlElement->pinNumber) == 0 ) &&
				     ( GPIOReadPinP(controlElementCW->portAddress, controlElementCW->pinNumber) != 0 ) )
				{
							//���� ������� �������
						if ( !controlElement->controlBusyFlag )
						{
								//������������� ����� �������
								controlElement->controlBusyFlag = 1;
								controlElementCW->controlBusyFlag = 1;
								//���� ���������� ��� �������
								if ( params & UPDEVENTS_GETEVENTS ) returnedEvents |= controlElement->eventCodeClick;
								//���� �������� ���������� �������
								if ( ( params & UPDEVENTS_HANDLERS_LAUNCH ) && ( controlElement->controlClickHandler ) )
										controlElement->controlClickHandler();				
						}
				//����� CW - �� �������
				} else if ( ( GPIOReadPinP(controlElement->portAddress, controlElement->pinNumber) != 0 ) &&
		                ( GPIOReadPinP(controlElementCW->portAddress, controlElementCW->pinNumber) == 0 ) )
				{
						//���� ������� �������
						if ( !controlElementCW->controlBusyFlag )
						{
								//������������� ����� �������
								controlElement->controlBusyFlag = 1;
								controlElementCW->controlBusyFlag = 1;
								//���� ���������� ��� �������
								if ( params & UPDEVENTS_GETEVENTS ) returnedEvents |= controlElementCW->eventCodeClick;
								//���� �������� ���������� �������
								if ( ( params & UPDEVENTS_HANDLERS_LAUNCH ) && ( controlElementCW->controlClickHandler ) )
										controlElementCW->controlClickHandler();				
						}
				//����� ��� ������ ������
				} else if ( ( GPIOReadPinP(controlElement->portAddress, controlElement->pinNumber) != 0 ) &&
				            ( GPIOReadPinP(controlElementCW->portAddress, controlElementCW->pinNumber) != 0 ) )
				{
						//����� ������
						controlElement->controlBusyFlag = 0;
						controlElementCW->controlBusyFlag = 0;
				};
				//������������� �� ��������� �������
				controlElement = (controlsBaseStruct*)controlElementCW->nextControlElement;
		}
	
		return returnedEvents;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
SYS_EVENTS_DATA controlsUpdateButtons(uint32_t params)
{
		//��������� ������� � ������� �����������
		uint32_t returnedEvents = 0;
		controlsBaseStruct* controlElement = 0;
	
		//����� ������
		controlElement = _buttonsData;
		while ( controlElement )
		{
				//��������� ������ ������� �� ���
				if ( GPIOReadPinP(controlElement->portAddress, controlElement->pinNumber) == 0 )
				{
						if ( !controlElement->controlBusyFlag )
						{
								//���� ������ ������ �������
								//����
								controlElement->controlBusyFlag = 1;
								//������� �� ������
								//���� ���������� ��� �������
								if ( params & UPDEVENTS_GETEVENTS )
										returnedEvents |= controlElement->eventCodeClick | controlElement->eventCodePressed;
								//���� �������� ���������� �������
								if ( ( params & UPDEVENTS_HANDLERS_LAUNCH ) && ( controlElement->controlClickHandler ) )
										controlElement->controlClickHandler();
						} else
						{
								//������ ����������� ��������/������������
								//���� ���������� ��� �������
								controlElement->controlBusyFlag = 1;
								if ( params & UPDEVENTS_GETEVENTS ) returnedEvents |= controlElement->eventCodePressed;
						};
				} else
				{
						controlElement->controlBusyFlag = 0;				
				};
				//������������� �� ��������� �������
				controlElement = (controlsBaseStruct*)controlElement->nextControlElement;
		};
	
		return returnedEvents;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/*  */
SYS_EVENTS_DATA controlsUpdateValcoder(uint32_t params)
{
		//��������� ������� � ������� �����������
		uint32_t returnedEvents = 0;
		controlsBaseStruct* controlElement = 0;
		controlsBaseStruct* controlElementCW = 0;
		
		//����� ����������
		controlElement = _valcoderData;
		while ( controlElement )
		{
				controlElementCW = (controlsBaseStruct*)controlElement->nextControlElement;
				if ( !controlElementCW ) return VALCODER_ERROR;
				//����� CCW - ������ �������
				if ( ( GPIOReadPinP(controlElement->portAddress, controlElement->pinNumber) == 0 ) &&
				     ( GPIOReadPinP(controlElementCW->portAddress, controlElementCW->pinNumber) != 0 ) )
				{
							//���� ������� �������
						if ( !controlElement->controlBusyFlag )
						{
								//������������� ����� �������
								controlElement->controlBusyFlag = 1;
								controlElementCW->controlBusyFlag = 1;
								//���� ���������� ��� �������
								if ( params & UPDEVENTS_GETEVENTS ) returnedEvents |= controlElement->eventCodeClick;
								//���� �������� ���������� �������
								if ( ( params & UPDEVENTS_HANDLERS_LAUNCH ) && ( controlElement->controlClickHandler ) )
										controlElement->controlClickHandler();				
						}
				//����� CW - �� �������
				} else if ( ( GPIOReadPinP(controlElement->portAddress, controlElement->pinNumber) != 0 ) &&
		                ( GPIOReadPinP(controlElementCW->portAddress, controlElementCW->pinNumber) == 0 ) )
				{
						//���� ������� �������
						if ( !controlElementCW->controlBusyFlag )
						{
								//������������� ����� �������
								controlElement->controlBusyFlag = 1;
								controlElementCW->controlBusyFlag = 1;
								//���� ���������� ��� �������
								if ( params & UPDEVENTS_GETEVENTS ) returnedEvents |= controlElementCW->eventCodeClick;
								//���� �������� ���������� �������
								if ( ( params & UPDEVENTS_HANDLERS_LAUNCH ) && ( controlElementCW->controlClickHandler ) )
										controlElementCW->controlClickHandler();				
						}
				//����� ��� ������ ������
				} else if ( ( GPIOReadPinP(controlElement->portAddress, controlElement->pinNumber) != 0 ) &&
				            ( GPIOReadPinP(controlElementCW->portAddress, controlElementCW->pinNumber) != 0 ) )
				{
						//����� ������
						controlElement->controlBusyFlag = 0;
						controlElementCW->controlBusyFlag = 0;
				};
				//������������� �� ��������� �������
				controlElement = (controlsBaseStruct*)controlElementCW->nextControlElement;
		}
		
		return returnedEvents;
}
//---------------------------------------------------------------------------------------------------------//
