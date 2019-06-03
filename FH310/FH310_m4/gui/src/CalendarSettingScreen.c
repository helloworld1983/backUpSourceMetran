/********************************************************************/
/*                                                                  */
/* File Name    : CalendarSettingScreen.c                           */
/*                                                                  */
/* General      :The CalendarSettingScreen allows user              */
/*                to change calendar								*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/22      Viet Le  			new file        */
/* #0001       2017/08/31	   Quyen Ngo			modify			*/
/*			Remove feed WDT, enable and disable chip RTC before		*/
/*			set RTC													*/
/*  #0002	   2017/09/28      Linh Nguyen(MV)  	    modify	    */
/*    Use new guiInterface_RTCSettime function 						*/
/*  #0003	   2017/10/27      Viet Le 	    	        modify	    */
/*	  Edit : EDIT_SetFont()->guiTaskEDIT_SetFont; TEXT_SetFont()->  */
/*		guiTaskTEXT_SetFont() (bug 1997)							*/
/*  		   2017/10/28      Linh Nguyen(MV)  	    modify	    */
/* #0004 Change RTC design  (bug #2002)								*/
/********************************************************************/

#include "stdint.h"
#include <RealTimeClock.h>
#include <StringTools.h>
#include "EDIT.h"
#include "BUTTON.h"
#include "TEXT.h"
#include "CalendarSettingScreen.h"
#include "MainScreen.h"
#include "GuiTask.h"
#include "SettingCalendarButton.h"
#include "setting.h"
#include "Font.h"
#include "AlarmInfor.h"
#include "MaintenanceScreen.h"
#include "OptionScreen.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

//Define item id for calendar setting screen
enum{
	eTimerClock1sid,
	eCloseBtnInPopupCalendarStringId,
	eTitleCalenderSettingId
};

WM_HWIN g_calendarSettingScreen;

static WM_HWIN s_popUpCalendarSetting;
static EDIT_Handle s_currentClockTime;
static EDIT_Handle s_yearBtn;
static EDIT_Handle s_monthBtn;
static EDIT_Handle s_dayBtn;
static EDIT_Handle s_hourBtn;
static EDIT_Handle s_minuteBtn;
static EDIT_Handle s_secondBtn;
static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static BUTTON_Handle s_changeClockTimeBtn;
static BUTTON_Handle s_CloseBtn;
static WM_HTIMER s_timerClock1s;
static TEXT_Handle s_titleCalendarSetting;

/****************************************************************************************/
/*                                                                                      */
/* General 	-calendarSettingScreen_HideAllItem 								            */
/*                                                                                      */
/* Details  -Hide all item when show pop up			            						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_changeClockTimeBtn : change time button                    */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_currentClockTime : display current time					*/
/*                     (O) s_yearBtn : year button						                */
/*                     (O) s_monthBtn : month button						            */
/*                     (O) s_dayBtn : day button						                */
/*                     (O) s_hourBtn : hour button						                */
/*                     (O) s_minuteBtn : minute button						            */
/*                     (O) s_secondBtn : second button						            */
/*                     (O) s_titleCalendarSetting : title calendar						*/
/*                     (O) g_calendarSettingScreen : calendar setting screen			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void calendarSettingScreen_HideAllItem(void)
{
	WM_HideWindow(s_changeClockTimeBtn);
	WM_HideWindow(s_homeBtn);
	WM_HideWindow(s_backBtn);
	WM_HideWindow(s_currentClockTime);
	WM_HideWindow(s_yearBtn);
	WM_HideWindow(s_monthBtn);
	WM_HideWindow(s_dayBtn);
	WM_HideWindow(s_hourBtn);
	WM_HideWindow(s_minuteBtn);
	WM_HideWindow(s_secondBtn);
	WM_HideWindow(s_titleCalendarSetting);
	alarmInfor_HideAlarmInfor();

	WM_Paint(g_calendarSettingScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-calendarSettingScreen_ShowAllItem 								            */
/*                                                                                      */
/* Details  -Show all item when hide pop up			            						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_changeClockTimeBtn : change time button                    */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_currentClockTime : display current time					*/
/*                     (O) s_yearBtn : year button						                */
/*                     (O) s_monthBtn : month button						            */
/*                     (O) s_dayBtn : day button						                */
/*                     (O) s_hourBtn : hour button						                */
/*                     (O) s_minuteBtn : minute button						            */
/*                     (O) s_secondBtn : second button						            */
/*                     (O) s_titleCalendarSetting : title calendar						*/
/*                     (O) g_calendarSettingScreen : calendar setting screen			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void calendarSettingScreen_ShowAllItem(void)
{
	WM_ShowWindow(s_changeClockTimeBtn);
	WM_ShowWindow(s_homeBtn);
	WM_ShowWindow(s_backBtn);
	WM_ShowWindow(s_currentClockTime);
	WM_ShowWindow(s_yearBtn);
	WM_ShowWindow(s_monthBtn);
	WM_ShowWindow(s_dayBtn);
	WM_ShowWindow(s_hourBtn);
	WM_ShowWindow(s_minuteBtn);
	WM_ShowWindow(s_secondBtn);
	WM_ShowWindow(s_titleCalendarSetting);
	alarmInfor_ShowAlarmInfor();

	WM_Paint(g_calendarSettingScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-calendarSettingScreen_CallbackBackBtn 								        */
/*                                                                                      */
/* Details  -Call back function for back button, handle event from 			            */
/* 				the window																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : messenger from the window					 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void calendarSettingScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10);

		GUI_SetColor(BORDER_COLOR);
		GUI_DrawRoundedFrame(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10,2);

		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Back", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("戻る", Rect.x1/2, 17);
		}

		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			calendarSettingScreen_Hide();
			maintenanceScreen_Hide();
			optionScreen_SetTab(eMaintenanceTabId);
			optionScreen_Show();
		}
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-calendarSettingScreen_CallbackHomeBtn 								        */
/*                                                                                      */
/* Details  -Call back function for home button, handle event from 			            */
/* 				the window																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : messenger from the window					 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void calendarSettingScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10);

		GUI_SetColor(BORDER_COLOR);
		GUI_DrawRoundedFrame(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10,2);

		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Home", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("ホーム", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			calendarSettingScreen_Hide();
			WM_SendMessageNoPara(g_mainWindow,WM_SHOW_HOMESCREEN);
		}
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-calendarSettingScreen_Callback 								            */
/*                                                                                      */
/* Details  -Call back function for calendar Setting Screen, handle event from     	    */
/* 				the window																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : messenger from the window					 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_popUpCalendarSetting : popup for calendar					*/
/*                     (O) s_currentClockTime : display current time                    */
/*                     (O) s_timerClock1s : timer update time						    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void calendarSettingScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		if(!WM_IsVisible(s_popUpCalendarSetting))
		{
			GUI_SetColor(GUI_BLACK);
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				guiTask_SetFont(&GUI_FontEng3L14B2PP);
				GUI_DispStringHCenterAt("Current clock time", 400,60);
				GUI_DispStringHCenterAt("Change date and time", 400,160);
			}
			else
			{
				guiTask_SetFont(&GUI_FontJapan3L16B2PP);
				GUI_DispStringHCenterAt("現在の日時", 400,60);
				GUI_DispStringHCenterAt("変更日時", 400,160);
			}
		}
		break;
	case WM_TIMER:
		if(WM_IsVisible(pMsg->hWin))
		{
			if(WM_GetTimerId(pMsg->Data.v) == eTimerClock1sid)
			{
				EDIT_SetText(s_currentClockTime ,(char*)stringtools_GetDateString());
				WM_RestartTimer(s_timerClock1s, TIMER_1_SECONDS);
			}
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-calendarSettingScreen_CallbackPopUp 								        */
/*                                                                                      */
/* Details  -Call back function for pop up dialog, handle event from 					*/
/* 				the window																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : messenger from the window					 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void calendarSettingScreen_CallbackPopUp(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(DIALOG_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,25);

		GUI_SetColor(BORDER_DIALOG_COLOR);
		GUI_AA_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,25);

		GUI_SetBkColor(DIALOG_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Unable to set date", 350,50);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("設定出来ない日付です", 350,50);
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-calendarSettingScreen_CallbackChangeClockBtn 								*/
/*                                                                                      */
/* Details  -Call back function for change clock Screen, handle event from     	    	*/
/* 				the window																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : messenger from the window					 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_popUpCalendarSetting : popup for calendar                  */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void calendarSettingScreen_CallbackChangeClockBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10);

		GUI_SetColor(BORDER_COLOR);
		GUI_DrawRoundedFrame(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10,2);

		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Change", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("時刻設定", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			if(calendarSettingScreen_CheckValidDate() == false)
			{
				WM_ShowWindow(s_popUpCalendarSetting);
				WM_BringToTop(s_popUpCalendarSetting);
				calendarSettingScreen_HideAllItem();
			}
			else
			{
				Real_Time_Clock clockTime;
				clockTime.year = settingCalendarButton_GetValue(eValueYearSettingId);
				clockTime.month = settingCalendarButton_GetValue(eValueMonthSettingId);

				clockTime.mday = settingCalendarButton_GetValue(eValueDaySettingId);
				clockTime.hour = settingCalendarButton_GetValue(eValueHourSettingId);
				clockTime.min = settingCalendarButton_GetValue(eValueMinuteSettingId);
				clockTime.sec = settingCalendarButton_GetValue(eValueSecondSettingId);

				guiInterface_RTCSettime(&clockTime, E_USER_REQUEST_SET_RTC_ID);
			}
		}
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-calendarSettingScreen_CallbackCloseBtn 								    */
/*                                                                                      */
/* Details  -Call back function for OK button, handle event from 						*/
/* 				the window																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : messenger from the window					 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_popUpCalendarSetting : popup for calendar   			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void calendarSettingScreen_CallbackCloseBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10);

		GUI_SetColor(BORDER_COLOR);
		GUI_DrawRoundedFrame(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10,2);

		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Close", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("閉じる", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			WM_HideWindow(s_popUpCalendarSetting);
			calendarSettingScreen_ShowAllItem();
		}
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-calendarSettingScreen_Init								    				*/
/*                                                                                      */
/* Details  -Initialize calendar Setting Screen		            						*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_changeClockTimeBtn : change time button                    */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_currentClockTime : display current time					*/
/*                     (O) s_yearBtn : year button						                */
/*                     (O) s_monthBtn : month button						            */
/*                     (O) s_dayBtn : day button						                */
/*                     (O) s_hourBtn : hour button						                */
/*                     (O) s_minuteBtn : minute button						            */
/*                     (O) s_secondBtn : second button						            */
/*                     (O) s_titleCalendarSetting : title calendar						*/
/*                     (O) g_calendarSettingScreen : calendar setting screen			*/
/*                     (O) s_popUpCalendarSetting : popup for calendar setting screen	*/
/*                     (O) s_CloseBtn : close button									*/
/*                     (O) s_timerClock1s : timer update time							*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void calendarSettingScreen_Init(void)
{
	GUI_RECT Rect;

	g_calendarSettingScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,calendarSettingScreen_Callback,0);

	s_titleCalendarSetting = TEXT_CreateEx(250,5,300,60,g_calendarSettingScreen,WM_CF_SHOW,TEXT_CF_HCENTER,eTitleCalenderSettingId,"");

	s_currentClockTime = EDIT_CreateEx(200,90,400,50,g_calendarSettingScreen,WM_CF_SHOW,0,eCurrentClockTimeId,50);
	EDIT_SetFocussable(s_currentClockTime,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_currentClockTime ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_currentClockTime ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_yearBtn = EDIT_CreateEx(60,180,240,70,g_calendarSettingScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eYearId,50);
	EDIT_SetFocussable(s_yearBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_yearBtn,&Rect);
	settingCalendarButton_Init(s_yearBtn,eUpYearBtnId,eDownYearBtnId,eValueYearSettingId,eYearFieldId,Rect);
	WM_SetCallback(s_yearBtn,settingCalendarButton_Callback);

	s_monthBtn = EDIT_CreateEx(310,180,240,70,g_calendarSettingScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eMonthId,50);
	EDIT_SetFocussable(s_monthBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_monthBtn,&Rect);
	settingCalendarButton_Init(s_monthBtn,eUpMonthBtnId,eDownMonthBtnId,eValueMonthSettingId,eMonthFieldId,Rect);
	WM_SetCallback(s_monthBtn,settingCalendarButton_Callback);

	s_dayBtn = EDIT_CreateEx(550,180,230,70,g_calendarSettingScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eDayId,50);
	EDIT_SetFocussable(s_dayBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_dayBtn,&Rect);
	settingCalendarButton_Init(s_dayBtn,eUpDayBtnId,eDownDayBtnId,eValueDaySettingId,eDayFieldId,Rect);
	WM_SetCallback(s_dayBtn,settingCalendarButton_Callback);

	s_hourBtn = EDIT_CreateEx(60,250,240,70,g_calendarSettingScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eHourId,50);
	EDIT_SetFocussable(s_hourBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_hourBtn,&Rect);
	settingCalendarButton_Init(s_hourBtn,eUpHourBtnId,eDownHourBtnId,eValueHourSettingId,eHourFieldId,Rect);
	WM_SetCallback(s_hourBtn,settingCalendarButton_Callback);

	s_minuteBtn = EDIT_CreateEx(310,250,240,70,g_calendarSettingScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eMinuteId,50);
	EDIT_SetFocussable(s_minuteBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_minuteBtn,&Rect);
	settingCalendarButton_Init(s_minuteBtn,eUpMinuteBtnId,eDownMinuteBtnId,eValueMinuteSettingId,eMinuteFieldId,Rect);
	WM_SetCallback(s_minuteBtn,settingCalendarButton_Callback);

	s_secondBtn = EDIT_CreateEx(550,250,240,70,g_calendarSettingScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eSecondId,50);
	EDIT_SetFocussable(s_secondBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_secondBtn,&Rect);
	settingCalendarButton_Init(s_secondBtn,eUpSecondBtnId,eDownSecondBtnId,eValueSecondSettingId,eSecondFieldId,Rect);
	WM_SetCallback(s_secondBtn,settingCalendarButton_Callback);

	s_changeClockTimeBtn = BUTTON_CreateEx(275, 340, 200+50, 60, g_calendarSettingScreen, WM_CF_SHOW, 0, eChangeClockTimeBtnId);
	WM_SetCallback(s_changeClockTimeBtn,calendarSettingScreen_CallbackChangeClockBtn);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_calendarSettingScreen, WM_CF_SHOW, 0, eBackBtnCalendarId);
	WM_SetCallback(s_backBtn,calendarSettingScreen_CallbackBackBtn);

	//Home button
	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_calendarSettingScreen, WM_CF_SHOW, 0, eHomeBtnCalendarId);
	WM_SetCallback(s_homeBtn,calendarSettingScreen_CallbackHomeBtn);

	s_popUpCalendarSetting= WM_CreateWindowAsChild(50, 115, 700, 220,g_calendarSettingScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,calendarSettingScreen_CallbackPopUp,0);

	//OK button
	s_CloseBtn = BUTTON_CreateEx(210, 120, 280, 60, s_popUpCalendarSetting, WM_CF_SHOW, 0, eCloseBtnInPopupCalendarStringId);
	WM_SetCallback(s_CloseBtn,calendarSettingScreen_CallbackCloseBtn);

	s_timerClock1s = WM_CreateTimer(g_calendarSettingScreen, eTimerClock1sid, TIMER_1_SECONDS,0);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-calendarSettingScreen_Show								    				*/
/*                                                                                      */
/* Details  -Show calendar Setting Screen		            							*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_timerClock1s : timer update time                           */
/*                     (O) s_titleCalendarSetting : title calendar						*/
/*                     (O) s_popUpCalendarSetting : popup for calendar					*/
/*                     (O) s_currentClockTime : display current time					*/
/*																						*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void calendarSettingScreen_Show(void)
{
	alarmInfor_ShowAlarmInfor();
	WM_RestartTimer(s_timerClock1s, TIMER_1_SECONDS);

	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		guiTaskTEXT_SetFont(s_titleCalendarSetting, &GUI_FontEng3L16B2PP);
		TEXT_SetText(s_titleCalendarSetting,"Calendar setting");
	}
	else
	{
		guiTaskTEXT_SetFont(s_titleCalendarSetting, &GUI_FontJapan3L18B2PP);
		TEXT_SetText(s_titleCalendarSetting,"カレンダ設定");
	}


	int8_t yearBuffer[SIZE_BUFFER_YEAR] = {'\0'};
	int8_t monthBuffer[SIZE_BUFFER_MONTH] = {'\0'};
	int8_t dayBuffer[SIZE_BUFFER_DAY] = {'\0'};

	int8_t hourBuffer[SIZE_BUFFER_HOURS] = {'\0'};
	int8_t minuteBuffer[SIZE_BUFFER_MINUTE] = {'\0'};
	int8_t secondBuffer[SIZE_BUFFER_SECOND] = {'\0'};

	Real_Time_Clock clockTime = realtimeclock_GetLastTime();
	EDIT_SetText(s_currentClockTime ,(char*)stringtools_GetDateString());

	sprintf((char*)yearBuffer,"%.4d",clockTime.year);
	sprintf((char*)monthBuffer,"%.2d",clockTime.month);
	sprintf((char*)dayBuffer,"%.2d",clockTime.mday);

	sprintf((char*)hourBuffer,"%.2d",clockTime.hour);
	sprintf((char*)minuteBuffer,"%.2d",clockTime.min);
	sprintf((char*)secondBuffer,"%.2d",clockTime.sec);

	if(clockTime.year < MIN_YEAR_CALENDAR)
	{
		settingCalendarButton_SetValue(eValueYearSettingId,MIN_YEAR_STR);
	}
	else
	{
		settingCalendarButton_SetValue(eValueYearSettingId,yearBuffer);
	}

	settingCalendarButton_SetValue(eValueMonthSettingId,monthBuffer);
	settingCalendarButton_SetValue(eValueDaySettingId,dayBuffer);

	settingCalendarButton_SetValue(eValueHourSettingId,hourBuffer);
	settingCalendarButton_SetValue(eValueMinuteSettingId,minuteBuffer);
	settingCalendarButton_SetValue(eValueSecondSettingId,secondBuffer);

	WM_HideWindow(s_popUpCalendarSetting);
	calendarSettingScreen_ShowAllItem();

	WM_ShowWindow(g_calendarSettingScreen);
	WM_BringToTop(g_calendarSettingScreen);
	WM_SetFocus(g_calendarSettingScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-calendarSettingScreen_Hide								    				*/
/*                                                                                      */
/* Details  -Hide calendar Setting Screen		            							*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) g_calendarSettingScreen : calendar setting screen            */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void calendarSettingScreen_Hide(void)
{
	WM_HideWindow(g_calendarSettingScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-calendarSettingScreen_CheckValidDate										*/
/*                                                                                      */
/* Details  -Check valid date					            							*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool calendarSettingScreen_CheckValidDate(void)
{
	int day = settingCalendarButton_GetValue(eValueDaySettingId);
	int month = settingCalendarButton_GetValue(eValueMonthSettingId);
	int year = settingCalendarButton_GetValue(eValueYearSettingId);

	if ((year > 0) && (month > 0 && month <= DECEMBER))
	{
		if ((month == APRIL) || (month == JUNE) || (month == SEPTEMBER) || (month == NOVEMBER))
		{
			if (day > 0 && day <= DAY_30)
				return true;
			else
				return false;
		}
		else if (month == FEBRUARY)
		{
			if (year % YEAR_DIV_4 == 0)
			{
				if (day > 0 && day <= DAY_29)
					return true;
				else
					return false;
			}
			else
			{
				if (day > 0 && day <= DAY_28)
					return true;
				else
					return false;
			}
		}
		else
		{
			if (day > 0 && day <= DAY_31)
				return true;
			else
				return false;
		}
	}
	else
	{
		return false;
	}
}

/* end of files */
