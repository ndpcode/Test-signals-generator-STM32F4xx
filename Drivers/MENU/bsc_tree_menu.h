//*********************************************************************************************************//
//BSC tree menu - header
//Created 10.04.2017
//Created by Novikov Dmitry
//*********************************************************************************************************//

#ifndef BSC_TREE_MENU
#define BSC_TREE_MENU

#include <stdint.h>
#include "bsc_controls.h"

//"virtual" struct
typedef struct
{
	uint16_t MenuItemId;
	uint8_t MenuItemIndexInSubmenu;
	uint8_t (*MenuDrawFunc)(const uint8_t /*MenuDrawFrameNumber*/);
	uint8_t (*MenuEventsFunc)(const uint16_t /*MenuEventsFrameNumber*/, SYS_EVENTS_DATA /*SystemEvents*/);
	uint8_t (*MenuOnEntryFunc)(void);
	void *MenuParent;
	void *MenuChild;
	void *MenuPrevItem;
	void *MenuNextItem;
} TREE_MENU_ITEM;

typedef struct
{
	TREE_MENU_ITEM *MenuHeadItem;                   //�������(������) ������� ����
	TREE_MENU_ITEM *MenuCurrentItem;                //������� ����, �������� ������
	SYS_EVENTS_DATA (*ExternalGetEventsFunc)(void); //�������, ������������ ���� �������
	//�������, ���������� ���������� � ������� ������� MenuTransitionTimeInMS ��� �������� ����� �������� ����
	void (*MenuTransitionFunc)(const uint16_t /*DrawFrameNum*/);
	uint16_t MenuTargetDrawFPS;                     //������� �������� ��������� ���� (������ � �������)
	uint16_t MenuTargetEventsFPS;                   //������� �������� ������ ������� (������� � �������)
	uint8_t MenuCurrentDrawFrame;	                  //����� �������� ����� ��������� ����, 1..MenuTargetFPS
	uint16_t MenuCurrentEventsFrame;                //����� ������� �������� ������ �������
	uint16_t MenuMaxItemId;                         //Id ������ ���������� ������ ����
	uint32_t MenuDrawClock;                         //����� ��������� ���������, ��
	uint32_t MenuEventsClock;                       //����� ���������� ���������� �������, ��
	uint8_t MenuEditAllow;                          //���� ���������� �� ����������/��������/��������� ��������� ����
	uint8_t MenuTransitionState;                    //����, �������������� � �������� ����� ������� ���� ��� MenuTransitionTimeInMS != 0
	uint16_t MenuTransitionTimeInMS;                //�����, ��������� �� ������� ����� �������� ����
	//��� ������� ������� MenuTransitionTimeInMS, MenuTransitionFunc �� ���������� � MenuTransitionState �� ��������������
} TREE_MENU;

TREE_MENU *MenuCreate(uint8_t targetDrawFPS, uint16_t targetEventFPS, SYS_EVENTS_DATA (*_ExternalGetEventsFunc)(void),
	                    void (*_MenuTransitionFunc)(const uint16_t), uint16_t _menuTransitionTimeInMs,
											uint8_t (*HeadMenuDrawFunc)(const uint8_t), uint8_t (*HeadMenuEventsFunc)(const uint16_t, SYS_EVENTS_DATA),
											uint8_t (*HeadMenuOnEntryFunc)(void));
uint16_t MenuAddNextItem(TREE_MENU *menuHead, uint16_t parentItemId, uint8_t (*_MenuDrawFunc)(const uint8_t),
                         uint8_t (*_MenuEventsFunc)(const uint16_t, SYS_EVENTS_DATA), uint8_t (*_MenuOnEntryFunc)(void));
uint16_t MenuAddSubItem(TREE_MENU *menuHead, uint16_t parentItemId, uint8_t (*_MenuDrawFunc)(const uint8_t),
	                      uint8_t (*_MenuEventsFunc)(const uint16_t, SYS_EVENTS_DATA), uint8_t (*_MenuOnEntryFunc)(void));
uint8_t MenuGoToHeadItem(TREE_MENU *menuHead);
uint8_t MenuGoToPrevItem(TREE_MENU *menuHead);
uint8_t MenuGoToNextItem(TREE_MENU *menuHead);
uint8_t MenuGoToParentItem(TREE_MENU *menuHead);
uint8_t MenuGoToChildItem(TREE_MENU *menuHead);
uint8_t MenuGoToItemId(TREE_MENU *menuHead, uint16_t MenuItemId);
uint8_t MenuDeleteItem(TREE_MENU *menuHead, uint16_t MenuItemId);
TREE_MENU_ITEM *MenuGetItemById(TREE_MENU *menuHead, uint16_t MenuItemId);
uint8_t MenuUpdate(TREE_MENU *menuHead, uint16_t ClockSecond, uint16_t ClockMillisecond);

#endif
