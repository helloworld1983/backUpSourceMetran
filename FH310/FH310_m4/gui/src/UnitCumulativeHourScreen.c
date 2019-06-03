/********************************************************************/
/*                                                                  */
/* File Name    : UnitCumulativeHourScreen.c                        */
/*                                                                  */
/* General      : The UnitCumulativeHourScreen category provides    */
/* 				  the user an interface to display and              */
/*                change unit cumulative hour in system             */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/14       Viet Le		  	    new file        */
/* #0001       2017/10/27	   Viet Le				modify			*/
/*	-Edit function : EDIT_SetFont()->guiTaskEDIT_SetFont()(bug 1997)*/
/*                                                                  */
/********************************************************************/

#include "stdlib.h"
#include "string.h"
#include <EEPROM.h>
#include "UnitCumulativeHourScreen.h"
#include "BUTTON.h"
#include "EDIT.h"
#include "setting.h"
#include "GuiTask.h"
#include "MainScreen.h"
#include "CumulativeHourDialog.h"
#include "OperationScreen.h"
#include "Font.h"
#include "AlarmInfor.h"
#include "MaintenanceScreen.h"
#include "OptionScreen.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

//Define item id
enum{
	eCurrentUnitCumulativeHourId,
	eChangeUnitCumulativeHourBtnId,
	eBackBtnInUintCumulativeHourId,
	eHomeBtnInUintCumulativeHourId,
	eUintCumulativeHourThreeChangeStringId,
	eUintCumulativeHourTwoFiveChangeStringId,
	eUintCumulativeHourTwoChangeStringId,
	eUintCumulativeHourOneSevenFiveChangeStringId,
	eUintCumulativeHourOneFiveChangeStringId,
	eUintCumulativeHourOneTwoFiveChangeStringId,
	eUintCumulativeHourOneChangeStringId,
	eUintCumulativeHourZeroSevenFiveChangeStringId,
	eUintCumulativeHourZeroFiveChangeStringId,
	eUintCumulativeHourZeroTwoFiveChangeStringId,
	eUintCumulativeHourThreeChangeBtnStringId,
	eUintCumulativeHourTwoFiveChangeBtnStringId,
	eUintCumulativeHourTwoChangeBtnStringId,
	eUintCumulativeHourOneSevenFiveChangeBtnStringId,
	eUintCumulativeHourOneFiveChangeBtnStringId,
	eUintCumulativeHourOneTwoFiveChangeBtnStringId,
	eUintCumulativeHourOneChangeBtnStringId,
	eUintCumulativeHourZeroSevenFiveChangeBtnStringId,
	eUintCumulativeHourZeroFiveChangeBtnStringId,
	eUintCumulativeHourZeroTwoFiveChangeBtnStringId
};

WM_HWIN g_unitCumulativeHourScreen;

static EDIT_Handle s_currentUnitCumulativeHour;
static BUTTON_Handle s_changeUnitCumulativeHourBtn;
static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;

static EDIT_Handle s_ThreeEdit;
static EDIT_Handle s_TwoFiveEdit;
static EDIT_Handle s_TwoEdit;
static EDIT_Handle s_OneSevenFiveEdit;
static EDIT_Handle s_OneFiveEdit;
static EDIT_Handle s_OneTwoFiveEdit;
static EDIT_Handle s_OneEdit;
static EDIT_Handle s_ZeroSevenFiveEdit;
static EDIT_Handle s_ZeroFiveEdit;
static EDIT_Handle s_ZeroTwoFiveEdit;

static BUTTON_Handle s_ThreeBtn;
static BUTTON_Handle s_TwoFiveBtn;
static BUTTON_Handle s_TwoBtn;
static BUTTON_Handle s_OneSevenFiveBtn;
static BUTTON_Handle s_OneFiveBtn;
static BUTTON_Handle s_OneTwoFiveBtn;
static BUTTON_Handle s_OneBtn;
static BUTTON_Handle s_ZeroSevenFiveBtn;
static BUTTON_Handle s_ZeroFiveBtn;
static BUTTON_Handle s_ZeroTwoFiveBtn;
static bool s_isChangeUnitTotal = false;

/****************************************************************************************/
/*                                                                                      */
/* General 	-unitCumulativeHourScreen_ShowAllItem	 									*/
/*                                                                                      */
/* Details  -Show all item when hide pop up							 			    	*/
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
/*                     (O) s_backBtn : back screen                                      */
/*                     (O) s_homeBtn : home screen						                */
/*                     (O) g_unitCumulativeHourScreen : unit cumulative hour screen     */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void unitCumulativeHourScreen_ShowAllItem(void)
{
	WM_ShowWindow(s_backBtn);
	WM_ShowWindow(s_homeBtn);
	alarmInfor_ShowAlarmInfor();
	WM_Paint(g_unitCumulativeHourScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-unitCumulativeHourScreen_HideAllItem	 									*/
/*                                                                                      */
/* Details  -Hide all item when show pop up							 			    	*/
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
/*                     (O) s_backBtn : back screen                                      */
/*                     (O) s_homeBtn : home screen						                */
/*                     (O) g_unitCumulativeHourScreen : unit cumulative hour screen     */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void unitCumulativeHourScreen_HideAllItem(void)
{
	WM_HideWindow(s_backBtn);
	WM_HideWindow(s_homeBtn);
	alarmInfor_HideAlarmInfor();
	WM_Paint(g_unitCumulativeHourScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-unitCumulativeHourScreen_DisplayHourCumulativeFR	 						*/
/*                                                                                      */
/* Details  -Display unit cumualtive hour for each flow rate		 			    	*/
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
/*                     (O) s_ThreeEdit : display flow rate 3L                           */
/*                     (O) s_TwoFiveEdit : : display flow rate 2.5L						*/
/*                     (O) s_TwoEdit : : display flow rate 2L                           */
/*                     (O) s_OneSevenFiveEdit : display flow rate 1.75L					*/
/*                     (O) s_OneFiveEdit : display flow rate 1.5L                       */
/*                     (O) s_OneTwoFiveEdit	: display flow rate 1.25L					*/
/*                     (O) s_OneEdit : display flow rate 1L                             */
/*                     (O) s_ZeroSevenFiveEdit	: display flow rate 0.75L				*/
/*                     (O) s_ZeroFiveEdit : display flow rate 0.5L                      */
/*                     (O) s_ZeroTwoFiveEdit : display flow rate 0.25L					*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void unitCumulativeHourScreen_DisplayHourCumulativeFR(void)
{
	int8_t hourBuffer[SIZE_BUFFER_HOUR_UNIT] = {'\0'};

	//3L
	CumulativeHourTime time_300;
	time_300 = guiInterface_GetCumulativeHour(uints_300);
	itoa(time_300.hour,(char*)hourBuffer,10);
	EDIT_SetText(s_ThreeEdit, (char*)&hourBuffer[0]);

	//2.5
	CumulativeHourTime time_250;
	time_250 = guiInterface_GetCumulativeHour(units_250);
	itoa(time_250.hour,(char*)hourBuffer,10);
	EDIT_SetText(s_TwoFiveEdit, (char*)&hourBuffer[0]);

	//2L
	CumulativeHourTime time_200;
	time_200 = guiInterface_GetCumulativeHour(uints_200);
	itoa(time_200.hour,(char*)hourBuffer,10);
	EDIT_SetText(s_TwoEdit, (char*)&hourBuffer[0]);

	//1.75
	CumulativeHourTime time_175;
	time_175 = guiInterface_GetCumulativeHour(uints_175);
	itoa(time_175.hour,(char*)hourBuffer,10);
	EDIT_SetText(s_OneSevenFiveEdit, (char*)&hourBuffer[0]);

	//1.5L
	CumulativeHourTime time_150;
	time_150 = guiInterface_GetCumulativeHour(uints_150);
	itoa(time_150.hour,(char*)hourBuffer,10);
	EDIT_SetText(s_OneFiveEdit, (char*)&hourBuffer[0]);

	//1.25
	CumulativeHourTime time_125;
	time_125 = guiInterface_GetCumulativeHour(uints_125);
	itoa(time_125.hour,(char*)hourBuffer,10);
	EDIT_SetText(s_OneTwoFiveEdit, (char*)&hourBuffer[0]);

	//1L
	CumulativeHourTime time_100;
	time_100 = guiInterface_GetCumulativeHour(uints_100);
	itoa(time_100.hour,(char*)hourBuffer,10);
	EDIT_SetText(s_OneEdit, (char*)&hourBuffer[0]);

	//0.75L
	CumulativeHourTime time_075;
	time_075 = guiInterface_GetCumulativeHour(uints_075);
	itoa(time_075.hour,(char*)hourBuffer,10);
	EDIT_SetText(s_ZeroSevenFiveEdit, (char*)&hourBuffer[0]);

	//0.5L
	CumulativeHourTime time_050;
	time_050 = guiInterface_GetCumulativeHour(uints_050);
	itoa(time_050.hour,(char*)hourBuffer,10);
	EDIT_SetText(s_ZeroFiveEdit, (char*)&hourBuffer[0]);

	//0.25L
	CumulativeHourTime time_025;
	time_025 = guiInterface_GetCumulativeHour(uints_025);
	itoa(time_025.hour,(char*)hourBuffer,10);
	EDIT_SetText(s_ZeroTwoFiveEdit, (char*)&hourBuffer[0]);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-unitCumulativeHourScreen_Callback 								            */
/*                                                                                      */
/* Details  -Call back function for unit cumulative operation Screen, handle event from */
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
void unitCumulativeHourScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(GUI_BLACK);

		if(!WM_IsVisible(g_cumulativeHourDialog))
		{
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				guiTask_SetFont(&GUI_FontEng3L16B2PP);
				GUI_DispStringHCenterAt("Unit’s cumulative operation hours change", 400,5);

				guiTask_SetFont(&GUI_FontEng3L12B2PP);
				GUI_DispStringHCenterAt("Current total hours", 100,51);

				GUI_AA_DrawRoundedRect(10,105,790,400,10);
				guiTask_SetFont(&GUI_FontEng3L12B2PP);
				GUI_DispStringAt("Unit's cumulative time for each flow rate",30,95);
			}
			else
			{
				guiTask_SetFont(&GUI_FontJapan3L18B2PP);
				GUI_DispStringHCenterAt("機器稼働時間変更", 400,5);

				guiTask_SetFont(&GUI_FontJapan3L14B2PP);
				GUI_DispStringHCenterAt("機器稼働時間", 121,51);

				GUI_AA_DrawRoundedRect(10,105,790,400,10);
				guiTask_SetFont(&GUI_FontJapan3L16B2PP);
				GUI_DispStringAt("流量別稼働時間",30,95);
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
/* General 	-unitCumulativeHourScreen_CallbackChangeUnitCumulativeBtn					*/
/*                                                                                      */
/* Details  -Call back function for change unit cumulative operation button, handle 	*/
/* 				event from the window													*/
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
/*                     (O) s_isChangeUnitTotal : flag change unit hour total   			*/
/*                     (O) g_cumulativeHourDialog : cumulative hour screen	            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void unitCumulativeHourScreen_CallbackChangeUnitCumulativeBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(BUTTON_NUMBER_BACKGROUND_COLOR);
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
			GUI_DispStringHCenterAt("Total hours change", Rect.x1/2, 15);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("機器稼働時間変更", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_isChangeUnitTotal = true;

			cumulativeHourDialog_Show(0);
			WM_BringToTop(g_cumulativeHourDialog);

			unitCumulativeHourScreen_HideAllItem();
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
/* General 	-unitCumulativeHourScreen_CallbackBackBtn 								    */
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
void unitCumulativeHourScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			unitCumulativeHourScreen_Hide();
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
/* General 	-unitCumulativeHourScreen_CallbackHomeBtn 								    */
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
/* Using Global Data : (I) g_mainWindow : main window							        */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void unitCumulativeHourScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			unitCumulativeHourScreen_Hide();
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
/* General 	-unitCumulativeHourScreen_CallbackFlowRateHour								*/
/*                                                                                      */
/* Details  -Call back function for Flow Rate hour button, handle event from 			*/
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
/*                     (O) g_cumulativeHourDialog : cumulative hour dialog         		*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void unitCumulativeHourScreen_CallbackFlowRateHour(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(BUTTON_NUMBER_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10);

		GUI_SetColor(BORDER_COLOR);
		GUI_DrawRoundedFrame(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10,2);

		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);
		guiTask_SetFont(&GUI_FontJapan3L16B2PP);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			switch(WM_GetId(pMsg->hWin))
			{
			case eUintCumulativeHourThreeChangeBtnStringId:
				GUI_DispStringHCenterAt("3.00L/min", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourTwoFiveChangeBtnStringId:
				GUI_DispStringHCenterAt("2.50L/min", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourTwoChangeBtnStringId:
				GUI_DispStringHCenterAt("2.00L/min", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourOneSevenFiveChangeBtnStringId:
				GUI_DispStringHCenterAt("1.75L/min", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourOneFiveChangeBtnStringId:
				GUI_DispStringHCenterAt("1.50L/min", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourOneTwoFiveChangeBtnStringId:
				GUI_DispStringHCenterAt("1.25L/min", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourOneChangeBtnStringId:
				GUI_DispStringHCenterAt("1.00L/min", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourZeroSevenFiveChangeBtnStringId:
				GUI_DispStringHCenterAt("0.75L/min", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourZeroFiveChangeBtnStringId:
				GUI_DispStringHCenterAt("0.50L/min", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourZeroTwoFiveChangeBtnStringId:
				GUI_DispStringHCenterAt("0.25L/min", Rect.x1/2, 10);
				break;
			default:
				break;
			}
		}
		else
		{
			switch(WM_GetId(pMsg->hWin))
			{
			case eUintCumulativeHourThreeChangeBtnStringId:
				GUI_DispStringHCenterAt("3.00L/分", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourTwoFiveChangeBtnStringId:
				GUI_DispStringHCenterAt("2.50L/分", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourTwoChangeBtnStringId:
				GUI_DispStringHCenterAt("2.00L/分", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourOneSevenFiveChangeBtnStringId:
				GUI_DispStringHCenterAt("1.75L/分", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourOneFiveChangeBtnStringId:
				GUI_DispStringHCenterAt("1.50L/分", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourOneTwoFiveChangeBtnStringId:
				GUI_DispStringHCenterAt("1.25L/分", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourOneChangeBtnStringId:
				GUI_DispStringHCenterAt("1.00L/分", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourZeroSevenFiveChangeBtnStringId:
				GUI_DispStringHCenterAt("0.75L/分", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourZeroFiveChangeBtnStringId:
				GUI_DispStringHCenterAt("0.50L/分", Rect.x1/2, 10);
				break;
			case eUintCumulativeHourZeroTwoFiveChangeBtnStringId:
				GUI_DispStringHCenterAt("0.25L/分", Rect.x1/2, 10);
				break;
			default:
				break;
			}
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_isChangeUnitTotal = false;
			uint8_t valueFR = 0;

			switch(WM_GetId(pMsg->hWin))
			{
			case eUintCumulativeHourThreeChangeBtnStringId:
				valueFR = SETTING_FR_300L;
				break;
			case eUintCumulativeHourTwoFiveChangeBtnStringId:
				valueFR = SETTING_FR_250L;
				break;
			case eUintCumulativeHourTwoChangeBtnStringId:
				valueFR = SETTING_FR_200L;
				break;
			case eUintCumulativeHourOneSevenFiveChangeBtnStringId:
				valueFR = SETTING_FR_175L;
				break;
			case eUintCumulativeHourOneFiveChangeBtnStringId:
				valueFR = SETTING_FR_150L;
				break;
			case eUintCumulativeHourOneTwoFiveChangeBtnStringId:
				valueFR = SETTING_FR_125L;
				break;
			case eUintCumulativeHourOneChangeBtnStringId:
				valueFR = SETTING_FR_100L;
				break;
			case eUintCumulativeHourZeroSevenFiveChangeBtnStringId:
				valueFR = SETTING_FR_075L;
				break;
			case eUintCumulativeHourZeroFiveChangeBtnStringId:
				valueFR = SETTING_FR_050L;
				break;
			case eUintCumulativeHourZeroTwoFiveChangeBtnStringId:
				valueFR = SETTING_FR_025L;
				break;
			default:
				break;
			}

			cumulativeHourDialog_Show(valueFR);
			WM_BringToTop(g_cumulativeHourDialog);
			unitCumulativeHourScreen_HideAllItem();
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
/* General 	-unitCumulativeHourScreen_Init												*/
/*                                                                                      */
/* Details  -Initialize unit Cumulative operation Hour Screen	            		    */
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
/* Using Global Data : (I) g_maintenanceScreen : maintenance screen			         	*/
/*                     (O) g_unitCumulativeHourScreen : unit cumulative hour screen     */
/*                     (O) s_currentUnitCumulativeHour :display unit cumulative hour    */
/*                     (O) s_changeUnitCumulativeHourBtn : change button	            */
/*                     (O) s_ThreeEdit : display flow rate 3L                           */
/*                     (O) s_TwoFiveEdit : : display flow rate 2.5L						*/
/*                     (O) s_TwoEdit : : display flow rate 2L                           */
/*                     (O) s_OneSevenFiveEdit : display flow rate 1.75L					*/
/*                     (O) s_OneFiveEdit : display flow rate 1.5L                       */
/*                     (O) s_OneTwoFiveEdit	: display flow rate 1.25L					*/
/*                     (O) s_OneEdit : display flow rate 1L                             */
/*                     (O) s_ZeroSevenFiveEdit	: display flow rate 0.75L				*/
/*                     (O) s_ZeroFiveEdit : display flow rate 0.5L                      */
/*                     (O) s_ZeroTwoFiveEdit : display flow rate 0.25L					*/
/*                     (O) s_ThreeBtn :  flow rate 3L button                            */
/*                     (O) s_TwoFiveBtn : : flow rate 2.5L button						*/
/*                     (O) s_TwoBtn : : flow rate 2L button                          	*/
/*                     (O) s_OneSevenFiveBtn :  flow rate 1.75L	button					*/
/*                     (O) s_OneFiveBtn : flow rate 1.5L button                      	*/
/*                     (O) s_OneTwoFiveBtn : flow rate 1.25L button						*/
/*                     (O) s_OneBtn : flow rate 1L button                            	*/
/*                     (O) s_ZeroSevenFiveBtn : flow rate 0.75L	button					*/
/*                     (O) s_ZeroFiveBtn : flow rate 0.5L button                     	*/
/*                     (O) s_ZeroTwoFiveBtn : flow rate 0.25L button					*/
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button						                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void unitCumulativeHourScreen_Init(void)
{
	g_unitCumulativeHourScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,unitCumulativeHourScreen_Callback,0);

	s_currentUnitCumulativeHour = EDIT_CreateEx(200,40,190,50,g_unitCumulativeHourScreen,WM_CF_SHOW,0,eCurrentUnitCumulativeHourId,50);
	EDIT_SetFocussable(s_currentUnitCumulativeHour,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_currentUnitCumulativeHour ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_currentUnitCumulativeHour ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_changeUnitCumulativeHourBtn = BUTTON_CreateEx(420, 40, 250, 50, g_unitCumulativeHourScreen, WM_CF_SHOW, 0, eChangeUnitCumulativeHourBtnId);
	WM_SetCallback(s_changeUnitCumulativeHourBtn,unitCumulativeHourScreen_CallbackChangeUnitCumulativeBtn);

	s_ThreeEdit = EDIT_CreateEx(165,125,190,40,g_unitCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourThreeChangeStringId,15);
	EDIT_SetFocussable(s_ThreeEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_ThreeEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_ThreeEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_ThreeBtn = BUTTON_CreateEx(15, 125, 140, 40, g_unitCumulativeHourScreen, WM_CF_SHOW, 0, eUintCumulativeHourThreeChangeBtnStringId);
	WM_SetCallback(s_ThreeBtn,unitCumulativeHourScreen_CallbackFlowRateHour);

	s_TwoFiveEdit = EDIT_CreateEx(165,175,190,40,g_unitCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourTwoFiveChangeStringId,15);
	EDIT_SetFocussable(s_TwoFiveEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_TwoFiveEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_TwoFiveEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_TwoFiveBtn = BUTTON_CreateEx(15, 175, 140, 40, g_unitCumulativeHourScreen, WM_CF_SHOW, 0, eUintCumulativeHourTwoFiveChangeBtnStringId);
	WM_SetCallback(s_TwoFiveBtn,unitCumulativeHourScreen_CallbackFlowRateHour);

	s_TwoEdit = EDIT_CreateEx(165,225,190,40,g_unitCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourTwoChangeStringId,15);
	EDIT_SetFocussable(s_TwoEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_TwoEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_TwoEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_TwoBtn = BUTTON_CreateEx(15, 225, 140, 40, g_unitCumulativeHourScreen, WM_CF_SHOW, 0, eUintCumulativeHourTwoChangeBtnStringId);
	WM_SetCallback(s_TwoBtn,unitCumulativeHourScreen_CallbackFlowRateHour);

	s_OneSevenFiveEdit = EDIT_CreateEx(165,275,190,40,g_unitCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourOneSevenFiveChangeStringId,15);
	EDIT_SetFocussable(s_OneSevenFiveEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_OneSevenFiveEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_OneSevenFiveEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_OneSevenFiveBtn = BUTTON_CreateEx(15, 275, 140, 40, g_unitCumulativeHourScreen, WM_CF_SHOW, 0, eUintCumulativeHourOneSevenFiveChangeBtnStringId);
	WM_SetCallback(s_OneSevenFiveBtn,unitCumulativeHourScreen_CallbackFlowRateHour);

	s_OneFiveEdit = EDIT_CreateEx(165,325,190,40,g_unitCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourOneFiveChangeStringId,15);
	EDIT_SetFocussable(s_OneFiveEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_OneFiveEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_OneFiveEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_OneFiveBtn = BUTTON_CreateEx(15, 325, 140, 40, g_unitCumulativeHourScreen, WM_CF_SHOW, 0, eUintCumulativeHourOneFiveChangeBtnStringId);
	WM_SetCallback(s_OneFiveBtn,unitCumulativeHourScreen_CallbackFlowRateHour);

	s_OneTwoFiveEdit = EDIT_CreateEx(535,125,190,40,g_unitCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourOneTwoFiveChangeStringId,15);
	EDIT_SetFocussable(s_OneTwoFiveEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_OneTwoFiveEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_OneTwoFiveEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_OneTwoFiveBtn = BUTTON_CreateEx(385,125, 140, 40, g_unitCumulativeHourScreen, WM_CF_SHOW, 0, eUintCumulativeHourOneTwoFiveChangeBtnStringId);
	WM_SetCallback(s_OneTwoFiveBtn,unitCumulativeHourScreen_CallbackFlowRateHour);

	s_OneEdit = EDIT_CreateEx(535,175,190,40,g_unitCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourOneChangeStringId,15);
	EDIT_SetFocussable(s_OneEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_OneEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_OneEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_OneBtn = BUTTON_CreateEx(385, 175, 140, 40, g_unitCumulativeHourScreen, WM_CF_SHOW, 0, eUintCumulativeHourOneChangeBtnStringId);
	WM_SetCallback(s_OneBtn,unitCumulativeHourScreen_CallbackFlowRateHour);

	s_ZeroSevenFiveEdit = EDIT_CreateEx(535,225,190,40,g_unitCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourZeroSevenFiveChangeStringId,15);
	EDIT_SetFocussable(s_ZeroSevenFiveEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_ZeroSevenFiveEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_ZeroSevenFiveEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_ZeroSevenFiveBtn = BUTTON_CreateEx(385, 225, 140, 40, g_unitCumulativeHourScreen, WM_CF_SHOW, 0, eUintCumulativeHourZeroSevenFiveChangeBtnStringId);
	WM_SetCallback(s_ZeroSevenFiveBtn,unitCumulativeHourScreen_CallbackFlowRateHour);

	s_ZeroFiveEdit = EDIT_CreateEx(535,275,190,40,g_unitCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourZeroFiveChangeStringId,15);
	EDIT_SetFocussable(s_ZeroFiveEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_ZeroFiveEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_ZeroFiveEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_ZeroFiveBtn = BUTTON_CreateEx(385, 275, 140, 40, g_unitCumulativeHourScreen, WM_CF_SHOW, 0, eUintCumulativeHourZeroFiveChangeBtnStringId);
	WM_SetCallback(s_ZeroFiveBtn,unitCumulativeHourScreen_CallbackFlowRateHour);

	s_ZeroTwoFiveEdit = EDIT_CreateEx(535,325,190,40,g_unitCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourZeroTwoFiveChangeStringId,15);
	EDIT_SetFocussable(s_ZeroTwoFiveEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_ZeroTwoFiveEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_ZeroTwoFiveEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_ZeroTwoFiveBtn = BUTTON_CreateEx(385,325, 140, 40, g_unitCumulativeHourScreen, WM_CF_SHOW, 0, eUintCumulativeHourZeroTwoFiveChangeBtnStringId);
	WM_SetCallback(s_ZeroTwoFiveBtn,unitCumulativeHourScreen_CallbackFlowRateHour);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_unitCumulativeHourScreen, WM_CF_SHOW, 0, eBackBtnInUintCumulativeHourId);
	WM_SetCallback(s_backBtn,unitCumulativeHourScreen_CallbackBackBtn);

	//Home button
	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_unitCumulativeHourScreen, WM_CF_SHOW, 0, eHomeBtnInUintCumulativeHourId);
	WM_SetCallback(s_homeBtn,unitCumulativeHourScreen_CallbackHomeBtn);

	cumulativeHourDialog_Init();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-unitCumulativeHourScreen_Show												*/
/*                                                                                      */
/* Details  -show unit Cumulative operation Hour Screen	            		    		*/
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
/*                     (O) g_unitCumulativeHourScreen : unit cumulative hour screen    	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void unitCumulativeHourScreen_Show(void)
{
	alarmInfor_ShowAlarmInfor();
	unitCumulativeHourScreen_ShowUnitHour();

	cumulativeHourDialog_Hide();
	unitCumulativeHourScreen_ShowAllItem();

	WM_ShowWindow(g_unitCumulativeHourScreen);
	WM_BringToTop(g_unitCumulativeHourScreen);
	WM_SetFocus(g_unitCumulativeHourScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-unitCumulativeHourScreen_Hide												*/
/*                                                                                      */
/* Details  -Hide unit Cumulative operation Hour Screen	            		    		*/
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
/*                     (O) g_unitCumulativeHourScreen : unit cumulative hour screen     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void unitCumulativeHourScreen_Hide(void)
{
	WM_HideWindow(g_unitCumulativeHourScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-unitCumulativeHourScreen_ShowUnitHour										*/
/*                                                                                      */
/* Details  -show unit Cumulative operation Hour	            		    			*/
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
/*                     (O) s_currentUnitCumulativeHour : display unit cumulative hour   */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void unitCumulativeHourScreen_ShowUnitHour(void)
{
	int8_t hourBuffer[SIZE_BUFFER_HOUR_UNIT] = {'\0'};

	CumulativeHourTime time;
	time = guiInterface_GetCumulativeHour(uints);

	itoa(time.hour,(char*)hourBuffer,10);
	EDIT_SetText(s_currentUnitCumulativeHour, (char*)&hourBuffer[0]);

	unitCumulativeHourScreen_DisplayHourCumulativeFR();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-unitCumulativeHourScreen_GetFlagChangeUnitTotal							*/
/*                                                                                      */
/* Details  -Get flag change unit total	            		    						*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE								 									*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_isChangeUnitTotal : flag change unit total cumulative hour	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool unitCumulativeHourScreen_GetFlagChangeUnitTotal(void)
{
	return s_isChangeUnitTotal;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-unitCumulativeHourScreen_SetFlagChangeUnitTotal							*/
/*                                                                                      */
/* Details  -Set flag change unit total	            		    						*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool flag								 						    */
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_isChangeUnitTotal : flag change unit total cumulative hour */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void unitCumulativeHourScreen_SetFlagChangeUnitTotal(bool flag)
{
	s_isChangeUnitTotal = flag;

	return;
}

/* end of files */
