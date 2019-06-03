/********************************************************************/
/*                                                                  */
/* File Name    : MaintenanceOptionScreen.c                         */
/*                                                                  */
/* General      : Maintenance option screen display 				*/
/*                 	 maintenance option						        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/14       Viet Le		  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/
#include "MaintenanceOptionScreen.h"
#include "BUTTON.h"
#include "setting.h"
#include "Font.h"
#include "MainScreen.h"
#include "GuiTask.h"
#include "OptionScreen.h"
#include "PasswordDialog.h"
#include "MaintenanceScreen.h"
#include "MonitorScreen.h"
#include "FunctionTestPageOneScreen.h"
#include "PSAParameterChangePageOneScreen.h"
#include "LogDataScreen.h"
#include "DevelopmentSettingScreen.h"
#include "SoftwareUpgradeScreen.h"
#include "CalendarSettingScreen.h"
#include "SerialNumberSettingScreen.h"
#include "UnitCumulativeHourScreen.h"
#include "OtherSettingScreen.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

//Define item id
enum{
	eMonitorBtnMaintenanceOptionScrId,
	eCalendarBtnMaintenanceOptionScrId,
	eFunctionTestBtnMaintenanceOptionScrId,
	ePsaParaBtnMaintenanceOptionScrId,
	eDevelopmentBtnMaintenanceOptionScrId,
	eLogDataBtnMaintenanceOptionScrId,
	eUpgradeBtnMaintenanceOptionScrId,
	eOtherBtnMaintenanceOptionScrId,
	eSerialNumberBtnMaintenanceOptionScrId,
	eUnitCumulativeBtnMaintenanceOptionScrId,
	eHomeBtnMaintenanceOptionScrId
};

WM_HWIN g_maintenanceOptionScreen;

static BUTTON_Handle s_monitorBtn;
static BUTTON_Handle s_calendarBtn;
static BUTTON_Handle s_functionTestBtn;
static BUTTON_Handle s_psaParaBtn;
static BUTTON_Handle s_developmentSettingBtn;
static BUTTON_Handle s_logDataBtn;
static BUTTON_Handle s_upgradeBtn;
static BUTTON_Handle s_otherBtn;
static BUTTON_Handle s_serialNumberBtn;
static BUTTON_Handle s_unitCumulativeBtn;
static BUTTON_Handle s_homeBtn;

/****************************************************************************************/
/*                                                                                      */
/* General 	-maintenanceOptionScreen_CallbackMonitorDataBtn 						    */
/*                                                                                      */
/* Details  -call back fuction for monitor button, handle event from the window 		*/
/*			  														                    */
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
void maintenanceOptionScreen_CallbackMonitorDataBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Monitor", Rect.x1/2, 12);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("モニタ", Rect.x1/2, 12);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			optionScreen_Hide();
			maintenanceScreen_Show();
			monitorScreen_Show();
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
/* General 	-maintenanceOptionScreen_CallbackFunctionTestBtn 						    */
/*                                                                                      */
/* Details  -call back fuction for function test button, handle event from the window   */
/*			  														                    */
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
void maintenanceOptionScreen_CallbackFunctionTestBtn(WM_MESSAGE *pMsg)
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
		if(WM_IsEnabled(pMsg->hWin))
		{
			GUI_SetColor(GUI_BLACK);
		}
		else
		{
			GUI_SetColor(GUI_GRAY);
		}

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Function test", Rect.x1/2, 12);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("機能テスト", Rect.x1/2, 12);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			optionScreen_Hide();
			maintenanceScreen_Show();
			functionTestPageOneScreen_Show();
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
/* General 	-maintenanceOptionScreen_CallbackPSAParemeterBtn 							*/
/*                                                                                      */
/* Details  -call back fuction for psa parameter change button,   						*/
/*			  	handle event from the window											*/
/* 																						*/
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
void maintenanceOptionScreen_CallbackPSAParemeterBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("PSA parameters change", Rect.x1/2, 12);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("PSA パラメータ変更", Rect.x1/2, 12);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			optionScreen_Hide();
			maintenanceScreen_Show();
			PSAParameterChangePageOneScreen_Show();
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
/* General 	-maintenanceOptionScreen_CallbackLogDataBtn 								*/
/*                                                                                      */
/* Details  -call back fuction for log data button,   						            */
/*			  	handle event from the window											*/
/* 																						*/
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
void maintenanceOptionScreen_CallbackLogDataBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Log data", Rect.x1/2, 12);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("ログデータ", Rect.x1/2, 12);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			optionScreen_Hide();
			maintenanceScreen_Show();
			logDataScreen_Show();
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
/* General 	-maintenanceOptionScreen_CallbackDevelopmentBtn 							*/
/*                                                                                      */
/* Details  -call back fuction for development button,   						        */
/*			  	handle event from the window											*/
/* 																						*/
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
void maintenanceOptionScreen_CallbackDevelopmentBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Development setting", Rect.x1/2, 12);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("開発用設定", Rect.x1/2, 12);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			optionScreen_Hide();
			maintenanceScreen_Show();
			developmentSettingScreen_Show();
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
/* General 	-maintenanceOptionScreen_CallbackUpgradeBtn	 								*/
/*                                                                                      */
/* Details  -call back fuction for upgrade button,  	 						        */
/*			  	handle event from the window											*/
/* 																						*/
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
void maintenanceOptionScreen_CallbackUpgradeBtn(WM_MESSAGE *pMsg)
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
		if(WM_IsEnabled(pMsg->hWin))
		{
			GUI_SetColor(GUI_BLACK);
		}
		else
		{
			GUI_SetColor(GUI_GRAY);
		}

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Software upgrade", Rect.x1/2, 12);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("ソフトウェア更新", Rect.x1/2, 12);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			optionScreen_Hide();
			maintenanceScreen_Show();
			softwareUpgradeScreen_Show();
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
/* General 	-maintenanceOptionScreen_CallbackCalendarBtn	 							*/
/*                                                                                      */
/* Details  -call back fuction for calendar button,  	 						        */
/*			  	handle event from the window											*/
/* 																						*/
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
void maintenanceOptionScreen_CallbackCalendarBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Calendar setting", Rect.x1/2, 12);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("カレンダ設定", Rect.x1/2, 12);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			optionScreen_Hide();
			maintenanceScreen_Show();
			calendarSettingScreen_Show();
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
/* General 	-maintenanceOptionScreen_CallbackSerialBtn	 								*/
/*                                                                                      */
/* Details  -call back fuction for serial number button,	  	 						*/
/*			  	handle event from the window											*/
/* 																						*/
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
void maintenanceOptionScreen_CallbackSerialBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Serial number setting", Rect.x1/2, 12);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("シリアル番号設定", Rect.x1/2, 12);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			optionScreen_Hide();
			maintenanceScreen_Show();
			serialNumberSettingScreen_Show();
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
/* General 	-maintenanceOptionScreen_CallbackUnitCumulativeBtn	 						*/
/*                                                                                      */
/* Details  -call back fuction for unit cumulative button,	  	 						*/
/*			  	handle event from the window											*/
/* 																						*/
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
void maintenanceOptionScreen_CallbackUnitCumulativeBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Unit’s total hours change", Rect.x1/2, 12);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("機器稼働時間変更", Rect.x1/2, 12);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			optionScreen_Hide();
			maintenanceScreen_Show();
			unitCumulativeHourScreen_Show();
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
/* General 	-maintenanceOptionScreen_CallbackOtherBtn	 								*/
/*                                                                                      */
/* Details  -call back fuction for other button,	  	 						        */
/*			  	handle event from the window											*/
/* 																						*/
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
void maintenanceOptionScreen_CallbackOtherBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Other setting", Rect.x1/2, 12);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("その他設定", Rect.x1/2, 12);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			optionScreen_Hide();
			maintenanceScreen_Show();
			otherSettingScreen_Show();
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
/* General 	-maintenanceOptionScreen_CallbackHomeBtn 									*/
/*                                                                                      */
/* Details  -call back fuction for home button,   						           		*/
/*			  	handle event from the window											*/
/* 																						*/
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
void maintenanceOptionScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
/* General 	-maintenanceOptionScreen_Callback	                                        */
/*                                                                                      */
/* Details  -call back function for maintenance option screen, handle event from window	*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : messenger	from window 					  		*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_monitorBtn : monitor button                                */
/*                     (O) s_logDataBtn	: log data button					            */
/*                     (O) s_calendarBtn : calendar button                              */
/*                     (O) s_upgradeBtn	: upgrade button					            */
/*                     (O) s_functionTestBtn : function test button              		*/
/*                     (O) s_unitCumulativeBtn : unit cumulative hour button            */
/*                     (O) s_serialNumberBtn : serial number button        			   	*/
/*                     (O) s_otherBtn : other button						            */
/*                     (O) s_psaParaBtn : psa parameter button               		   	*/
/*                     (O) s_developmentSettingBtn : development button			        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void maintenanceOptionScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);
		break;
	case WM_SET_FOCUS:
	{
		switch(passwordDialog_GetUserLogin())
		{
		case E_DISTRIBUTOR_USER:
			WM_ShowWindow(s_monitorBtn);
			WM_ShowWindow(s_logDataBtn);
			WM_ShowWindow(s_calendarBtn);
			WM_ShowWindow(s_upgradeBtn);

			WM_HideWindow(s_functionTestBtn);
			WM_HideWindow(s_psaParaBtn);
			WM_HideWindow(s_developmentSettingBtn);
			WM_HideWindow(s_otherBtn);
			WM_HideWindow(s_serialNumberBtn);
			WM_HideWindow(s_unitCumulativeBtn);
			break;
		case E_MANUFACTURE_INSPECTION_USER:
			WM_ShowWindow(s_monitorBtn);
			WM_ShowWindow(s_logDataBtn);
			WM_ShowWindow(s_calendarBtn);
			WM_ShowWindow(s_upgradeBtn);
			WM_ShowWindow(s_functionTestBtn);

			WM_HideWindow(s_psaParaBtn);
			WM_HideWindow(s_developmentSettingBtn);
			WM_HideWindow(s_otherBtn);
			WM_HideWindow(s_serialNumberBtn);
			WM_HideWindow(s_unitCumulativeBtn);
			break;
		case E_MANUFACTURE_USER:
			WM_ShowWindow(s_monitorBtn);
			WM_ShowWindow(s_logDataBtn);
			WM_ShowWindow(s_calendarBtn);
			WM_ShowWindow(s_upgradeBtn);
			WM_ShowWindow(s_functionTestBtn);
			WM_ShowWindow(s_unitCumulativeBtn);
			WM_ShowWindow(s_serialNumberBtn);

			WM_HideWindow(s_otherBtn);
			WM_HideWindow(s_psaParaBtn);
			WM_HideWindow(s_developmentSettingBtn);
			break;
		case E_MANUFACTURE_OPTION_USER:
			WM_ShowWindow(s_monitorBtn);
			WM_ShowWindow(s_logDataBtn);
			WM_ShowWindow(s_calendarBtn);
			WM_ShowWindow(s_upgradeBtn);
			WM_ShowWindow(s_functionTestBtn);
			WM_ShowWindow(s_unitCumulativeBtn);
			WM_ShowWindow(s_serialNumberBtn);
			WM_ShowWindow(s_otherBtn);

			WM_HideWindow(s_psaParaBtn);
			WM_HideWindow(s_developmentSettingBtn);
			break;
		case E_DEVELOPER_USER:
			WM_ShowWindow(s_monitorBtn);
			WM_ShowWindow(s_logDataBtn);
			WM_ShowWindow(s_calendarBtn);
			WM_ShowWindow(s_upgradeBtn);
			WM_ShowWindow(s_functionTestBtn);
			WM_ShowWindow(s_unitCumulativeBtn);
			WM_ShowWindow(s_serialNumberBtn);
			WM_ShowWindow(s_otherBtn);
			WM_ShowWindow(s_psaParaBtn);
			WM_ShowWindow(s_developmentSettingBtn);
			break;
		default:
			break;
		}
		break;
	}
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-maintenanceOptionScreen_Init								       		    */
/*                                                                                      */
/* Details  -Initialize maintenance option screen        							    */
/*			  																			*/
/*                                                                                      */
/* Arguments : (I) NONE					 												*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) g_maintenanceOptionScreen : maintenance option screen        */
/*                     (O) s_monitorBtn : monitor button                                */
/*                     (O) s_logDataBtn	: log data button					            */
/*                     (O) s_calendarBtn : calendar button                              */
/*                     (O) s_upgradeBtn	: upgrade button					            */
/*                     (O) s_functionTestBtn : function test button              		*/
/*                     (O) s_unitCumulativeBtn : unit cumulative hour button            */
/*                     (O) s_serialNumberBtn : serial number button        			   	*/
/*                     (O) s_otherBtn : other button						            */
/*                     (O) s_psaParaBtn : psa parameter button               		   	*/
/*                     (O) s_developmentSettingBtn : development button			        */
/*                     (O) s_homeBtn : home button                                      */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void maintenanceOptionScreen_Init(void)
{
	g_maintenanceOptionScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_optionScreen,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP, maintenanceOptionScreen_Callback, 0);

	s_monitorBtn = BUTTON_CreateEx(50,10,298,50,g_maintenanceOptionScreen,WM_CF_SHOW,0,eMonitorBtnMaintenanceOptionScrId);
	WM_SetCallback(s_monitorBtn,maintenanceOptionScreen_CallbackMonitorDataBtn);

	s_logDataBtn = BUTTON_CreateEx(50,70,298,50,g_maintenanceOptionScreen,WM_CF_SHOW,0,eLogDataBtnMaintenanceOptionScrId);
	WM_SetCallback(s_logDataBtn,maintenanceOptionScreen_CallbackLogDataBtn);

	s_calendarBtn = BUTTON_CreateEx(50,130,298,50,g_maintenanceOptionScreen,WM_CF_SHOW,0,eCalendarBtnMaintenanceOptionScrId);
	WM_SetCallback(s_calendarBtn,maintenanceOptionScreen_CallbackCalendarBtn);

	s_upgradeBtn = BUTTON_CreateEx(50,190,298,50,g_maintenanceOptionScreen,WM_CF_SHOW,0,eUpgradeBtnMaintenanceOptionScrId);
	WM_SetCallback(s_upgradeBtn,maintenanceOptionScreen_CallbackUpgradeBtn);

	s_functionTestBtn = BUTTON_CreateEx(50,250,298,50,g_maintenanceOptionScreen,WM_CF_SHOW,0,eFunctionTestBtnMaintenanceOptionScrId);
	WM_SetCallback(s_functionTestBtn,maintenanceOptionScreen_CallbackFunctionTestBtn);

	s_serialNumberBtn = BUTTON_CreateEx(452,10,298,50,g_maintenanceOptionScreen,WM_CF_SHOW,0,eSerialNumberBtnMaintenanceOptionScrId);
	WM_SetCallback(s_serialNumberBtn,maintenanceOptionScreen_CallbackSerialBtn);

	s_unitCumulativeBtn = BUTTON_CreateEx(452,70,298,50,g_maintenanceOptionScreen,WM_CF_SHOW,0,eUnitCumulativeBtnMaintenanceOptionScrId);
	WM_SetCallback(s_unitCumulativeBtn,maintenanceOptionScreen_CallbackUnitCumulativeBtn);

	s_otherBtn = BUTTON_CreateEx(452,130,298,50,g_maintenanceOptionScreen,WM_CF_SHOW,0,eOtherBtnMaintenanceOptionScrId);
	WM_SetCallback(s_otherBtn,maintenanceOptionScreen_CallbackOtherBtn);

	s_psaParaBtn = BUTTON_CreateEx(452,190,298,50,g_maintenanceOptionScreen,WM_CF_SHOW,0,ePsaParaBtnMaintenanceOptionScrId);
	WM_SetCallback(s_psaParaBtn,maintenanceOptionScreen_CallbackPSAParemeterBtn);

	s_developmentSettingBtn = BUTTON_CreateEx(452,250,298,50,g_maintenanceOptionScreen,WM_CF_SHOW,0,eDevelopmentBtnMaintenanceOptionScrId);
	WM_SetCallback(s_developmentSettingBtn,maintenanceOptionScreen_CallbackDevelopmentBtn);

	s_homeBtn = BUTTON_CreateEx(350, 365, 100, 60, g_maintenanceOptionScreen, WM_CF_SHOW, 0, eHomeBtnMaintenanceOptionScrId);
	WM_SetCallback(s_homeBtn,maintenanceOptionScreen_CallbackHomeBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-maintenanceOptionScreen_SetDisableBtn								        */
/*                                                                                      */
/* Details  -Disable function test button and software upgrade button       			*/
/*			  																			*/
/*                                                                                      */
/* Arguments : (I) NONE					 												*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_functionTestBtn : function test button                    	*/
/*                     (O) s_upgradeBtn : upgrade button						        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void maintenanceOptionScreen_SetDisableBtn(void)
{
	WM_DisableWindow(s_functionTestBtn);
	WM_DisableWindow(s_upgradeBtn);

	BUTTON_SetTextColor(s_functionTestBtn, BUTTON_CI_DISABLED,GUI_GRAY);
	BUTTON_SetTextColor(s_upgradeBtn, BUTTON_CI_DISABLED,GUI_GRAY);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-maintenanceOptionScreen_SetEnableBtn								        */
/*                                                                                      */
/* Details  -Enable function test button and software upgrade button       				*/
/*			  																			*/
/*                                                                                      */
/* Arguments : (I) NONE					 												*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_functionTestBtn : function test button                    	*/
/*                     (O) s_upgradeBtn : upgrade button						        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void maintenanceOptionScreen_SetEnableBtn(void)
{
	WM_EnableWindow(s_functionTestBtn);
	WM_EnableWindow(s_upgradeBtn);

	BUTTON_SetTextColor(s_functionTestBtn, BUTTON_CI_UNPRESSED,GUI_BLACK);
	BUTTON_SetTextColor(s_upgradeBtn, BUTTON_CI_UNPRESSED,GUI_BLACK);

	return;
}

/* end of files */
