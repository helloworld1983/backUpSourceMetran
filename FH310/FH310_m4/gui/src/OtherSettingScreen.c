/********************************************************************/
/*                                                                  */
/* File Name    : OtherSettingScreen.c                              */
/*                                                                  */
/* General      : OtherSettingScreen allow user change language     */
/* 				  setting.										    */
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
/*	-Edit function : RADIO_SetFont()->guiTaskRADIO_SetFont()		*/
/*     (bug 1997)                                                   */
/*                                                                  */
/********************************************************************/
#include "OtherSettingScreen.h"
#include "BUTTON.h"
#include "MainScreen.h"
#include "RADIO.h"
#include "board.h"
#include "setting.h"
#include "GuiTask.h"
#include "Font.h"
#include "MaintenanceScreen.h"
#include "OptionScreen.h"
#include "AlarmInfor.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

//Define item id
enum{
	eBackBtnInOtherSettingId,
	eHomeBtnInOtherSettingId,
	eLanguageRadioBtnSettingId
};

WM_HWIN g_otherSettingScreen;

static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static RADIO_Handle s_languageRadio;

/****************************************************************************************/
/*                                                                                      */
/* General 	-otherSettingScreen_CallbackBackBtn                                         */
/*                                                                                      */
/* Details  -call back function for back button, handle event from window				*/
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
void otherSettingScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			otherSettingScreen_Hide();
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
/* General 	-otherSettingScreen_CallbackHomeBtn	                                        */
/*                                                                                      */
/* Details  -call back function for home button, handle event from window				*/
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
/* Using Global Data : (I) g_mainWindow : main window							        */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void otherSettingScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			otherSettingScreen_Hide();
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
/* General 	-otherSettingScreen_Callback	                                            */
/*                                                                                      */
/* Details  -call back function for other Setting Screen, handle event from window		*/
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
/*                     (O) s_backBtn : back button                                      */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_languageRadio : language radio button						*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void otherSettingScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(GUI_BLACK);
		GUI_AA_DrawRoundedRect(10,60,390,170,10);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Other setting", 400,5);

			guiTask_SetFont(&GUI_FontEng3L12B2PP);
			GUI_DispStringAt("Language selection",25,50);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("その他設定", 400,5);

			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringAt("言語選択",25,50);

		}
		break;
	case WM_NOTIFY_PARENT:
	{
		switch(WM_GetId(pMsg->hWinSrc))
		{
		case eLanguageRadioBtnSettingId:
			if(pMsg->Data.v == WM_NOTIFICATION_VALUE_CHANGED)
			{
				int valueRadioLang = RADIO_GetValue(s_languageRadio);
				if(valueRadioLang >= 0)
				{
					guiInterface_SetSettingValue(eLanguageSettingId,(E_Language)valueRadioLang);
					setting_SaveAllSettingValue();
				}

				WM_Paint(pMsg->hWin);
				WM_Paint(s_backBtn);
				WM_Paint(s_homeBtn);

				if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
				{
					guiTaskRADIO_SetFont(s_languageRadio,&GUI_FontEng3L12B2PP);
					RADIO_SetText(s_languageRadio,"Japanese",0);
					RADIO_SetText(s_languageRadio,"English",1);
				}
				else
				{
					guiTaskRADIO_SetFont(s_languageRadio,&GUI_FontJapan3L12B2PP);
					RADIO_SetText(s_languageRadio,"日本語",0);
					RADIO_SetText(s_languageRadio,"英語",1);
				}
			}
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
/* General 	-otherSettingScreen_CallbackLanguageBtn                                     */
/*                                                                                      */
/* Details  -call back function for language button, handle event from window			*/
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
void otherSettingScreen_CallbackLanguageBtn(WM_MESSAGE *pMsg)
{
	switch(pMsg->MsgId)
	{
	default:
		RADIO_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-otherSettingScreen_Init							                		*/
/*                                                                                      */
/* Details  -Initialize other Setting Screen 			            	                */
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
/*                     (O) g_otherSettingScreen : other setting screen                  */
/*                     (O) s_languageRadio : language radio button			          	*/
/*                     (O) s_backBtn : back button                                      */
/*                     (O) s_homeBtn : home button						                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void otherSettingScreen_Init(void)
{
	g_otherSettingScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,otherSettingScreen_Callback,0);

	s_languageRadio = RADIO_CreateEx(30,80,180,120,g_otherSettingScreen,WM_CF_SHOW,0,eLanguageRadioBtnSettingId,2,40);
	guiTaskRADIO_SetFont(s_languageRadio,&GUI_FontEng3L12B2PP);
	RADIO_SetText(s_languageRadio,"Japanese",eJapanese);
	RADIO_SetText(s_languageRadio,"English",eEnglish);
	RADIO_SetFocusColor(s_languageRadio,SERVICE_BACKGROUND_COLOR);
	WM_SetCallback(s_languageRadio, otherSettingScreen_CallbackLanguageBtn);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_otherSettingScreen, WM_CF_SHOW, 0, eBackBtnInOtherSettingId);
	WM_SetCallback(s_backBtn,otherSettingScreen_CallbackBackBtn);

	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_otherSettingScreen, WM_CF_SHOW, 0, eHomeBtnInOtherSettingId);
	WM_SetCallback(s_homeBtn,otherSettingScreen_CallbackHomeBtn);

	WM_HideWindow(g_otherSettingScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-otherSettingScreen_Show									                */
/*                                                                                      */
/* Details  -Show other Setting Screen 			            	                        */
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
/*                     (O) s_languageRadio : language radio button          		   	*/
/*                     (O) g_otherSettingScreen : other setting screen	                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void otherSettingScreen_Show(void)
{
	alarmInfor_ShowAlarmInfor();
	uint8_t lang = guiInterface_GetSettingValue(eLanguageSettingId);

	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		guiTaskRADIO_SetFont(s_languageRadio,&GUI_FontEng3L12B2PP);
		RADIO_SetText(s_languageRadio,"Japanese",eJapanese);
		RADIO_SetText(s_languageRadio,"English",eEnglish);
	}
	else
	{
		guiTaskRADIO_SetFont(s_languageRadio,&GUI_FontJapan3L12B2PP);
		RADIO_SetText(s_languageRadio,"日本語",eJapanese);
		RADIO_SetText(s_languageRadio,"英語",eEnglish);
	}

	if(lang <= eEnglish)
	{
		RADIO_SetValue(s_languageRadio,lang);
	}

	WM_ShowWindow(g_otherSettingScreen);
	WM_BringToTop(g_otherSettingScreen);
	WM_SetFocus(g_otherSettingScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-otherSettingScreen_Hide									                */
/*                                                                                      */
/* Details  -Hide other Setting Screen 			            	                        */
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
/*                     (O) g_otherSettingScreen : other setting screen  			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void otherSettingScreen_Hide(void)
{
	WM_HideWindow(g_otherSettingScreen);

	return;
}

/* end of files */

