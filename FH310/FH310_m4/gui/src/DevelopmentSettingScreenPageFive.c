/********************************************************************/
/*                                                                  */
/* File Name    : DevelopmentSettingScreenPageFive.c                */
/*                                                                  */
/* General       : The user is able to test watch dog				*/
/*  																*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2008/08/22      Viet Le  			new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#include "DevelopmentSettingScreenPageFive.h"
#include "MainScreen.h"
#include "BUTTON.h"
#include "setting.h"
#include "GuiTask.h"
#include "ModeMgr.h"
#include "Font.h"
#include "MaintenanceScreen.h"
#include "OptionScreen.h"
#include "DevelopmentSettingScreenPageFour.h"
#include "AlarmInfor.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

//Define item id
enum{
	eBackBtnDevelopmentPageFiveId,
	eHomeBtnDevelopmentPageFiveId,
	ePrevBtnDevelopmentPageFiveId,
	eWDTM4BtnDevelopmentPageFiveId,
	eWDTM0BtnDevelopmentPageFiveId
};

WM_HWIN g_developmentSettingScreenPageFive;

static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static BUTTON_Handle s_prevBtn;
static BUTTON_Handle s_WDTM4Btn;
static BUTTON_Handle s_WDTM0Btn;
static const GUI_POINT s_aPointPrevArrow[] = {
		{99,10},
		{40,10},
		{40,0},
		{0,59/2},
		{40,59},
		{40,59-10},
		{99,59-10}
};

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFive_CallbackBackBtn 							*/
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
void developmentSettingScreenPageFive_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			developmentSettingScreenPageFive_Hide();
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
/* General 	-developmentSettingScreenPageFive_CallbackHomeBtn 							*/
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
static void developmentSettingScreenPageFive_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			developmentSettingScreenPageFive_Hide();
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
/* General 	-developmentSettingScreenPageFive_Callback	                                */
/*                                                                                      */
/* Details  -call back function for PSA parameter page five, handle event from window	*/
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
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFive_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(GUI_BLACK);
		GUI_AA_DrawRoundedRect(10,60,380,220,10);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Development setting", 400,5);

			guiTask_SetFont(&GUI_FontEng3L12B2PP);
			GUI_DispStringAt("WDT Test",25,50);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("開発用設定", 400,5);

			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("WDT テスト",25,50);
		}

		guiTask_SetFont(&GUI_FontEng3L18B2PP);
		GUI_DispStringHCenterAt("5/5", 400,440);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFive_CallbackWDTM4tn 							*/
/*                                                                                      */
/* Details  -Call back function for WDT M4 button, handle event from 					*/
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
void developmentSettingScreenPageFive_CallbackWDTM4tn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Test performed for m4", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("m4 テスト実施", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			while(1);
		}
		break;
	case WM_TOUCH:
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFive_CallbackWDTM0tn 							*/
/*                                                                                      */
/* Details  -Call back function for WDT M0 button, handle event from 					*/
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
void developmentSettingScreenPageFive_CallbackWDTM0tn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Test performed for m0", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("m0テスト実施", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			guiTask_SendPendingEvent(eWatchDogM0ResetChangeId,0);
		}
		break;
	case WM_TOUCH:
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFive_CallbackPrevBtn 							*/
/*                                                                                      */
/* Details  -Call back function for previous button, handle event from 					*/
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
static void developmentSettingScreenPageFive_CallbackPrevBtn(WM_MESSAGE * pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_FillPolygon(&s_aPointPrevArrow[0],7,Rect.x0,Rect.y0);

		GUI_SetPenSize(2);
		GUI_SetColor(BORDER_COLOR);//Border
		GUI_DrawPolygon(&s_aPointPrevArrow[0],7,Rect.x0,Rect.y0);

		GUI_SetColor(GUI_BLACK);
		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringAt("Prev", 40,20 + 2);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringAt("前", 50,18);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			developmentSettingScreenPageFive_Hide();
			developmentSettingScreenPageFour_Show();
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
/* General 	-developmentSettingScreenPageFive_Init										*/
/*                                                                                      */
/* Details  -Initialize development setting screen page five		            		*/
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
/*                     (O) g_developmentSettingScreenPageFive : development page five   */
/*                     (O) s_WDTM0Btn : WDT M0 button						            */
/*                     (O) s_WDTM4Btn : WDT M4 button						            */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_prevBtn : previous button						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFive_Init(void)
{
	g_developmentSettingScreenPageFive = WM_CreateWindowAsChild(0, 0, 800, 480,g_maintenanceScreen,WM_CF_HIDE| WM_CF_MEMDEV | WM_CF_LATE_CLIP,developmentSettingScreenPageFive_Callback,0);

	s_WDTM0Btn = BUTTON_CreateEx(30, 80, 320, 60, g_developmentSettingScreenPageFive, WM_CF_SHOW, 0, eWDTM0BtnDevelopmentPageFiveId);
	WM_SetCallback(s_WDTM0Btn,developmentSettingScreenPageFive_CallbackWDTM0tn);

	s_WDTM4Btn = BUTTON_CreateEx(30, 150, 320, 60, g_developmentSettingScreenPageFive, WM_CF_SHOW, 0, eWDTM4BtnDevelopmentPageFiveId);
	WM_SetCallback(s_WDTM4Btn,developmentSettingScreenPageFive_CallbackWDTM4tn);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_developmentSettingScreenPageFive, WM_CF_SHOW, 0, eBackBtnDevelopmentPageFiveId);
	WM_SetCallback(s_backBtn,developmentSettingScreenPageFive_CallbackBackBtn);

	//Home button
	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_developmentSettingScreenPageFive, WM_CF_SHOW, 0, eHomeBtnDevelopmentPageFiveId);
	WM_SetCallback(s_homeBtn,developmentSettingScreenPageFive_CallbackHomeBtn);

	//Previous button
	s_prevBtn = BUTTON_CreateEx(0, 415, 102, 60, g_developmentSettingScreenPageFive, WM_CF_SHOW, 0, ePrevBtnDevelopmentPageFiveId);
	WM_SetCallback(s_prevBtn,developmentSettingScreenPageFive_CallbackPrevBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFive_Show										*/
/*                                                                                      */
/* Details  -Show development setting screen page five		            		        */
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
/*                     (O) g_developmentSettingScreenPageFive : development page five   */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFive_Show(void)
{
	alarmInfor_ShowAlarmInfor();
	WM_ShowWindow(g_developmentSettingScreenPageFive);
	WM_BringToTop(g_developmentSettingScreenPageFive);
	WM_SetFocus(g_developmentSettingScreenPageFive);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFive_Hide										*/
/*                                                                                      */
/* Details  -Hide development setting screen page five		            		        */
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
/*                     (O) g_developmentSettingScreenPageFive : development page five   */
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFive_Hide(void)
{
	WM_HideWindow(g_developmentSettingScreenPageFive);

	return;
}

/* end of files */
