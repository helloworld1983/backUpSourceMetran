/********************************************************************/
/*                                                                  */
/* File Name    : FunctionTestPageFourScreen.c                      */
/*                                                                  */
/* General       : The FunctionTestPageFour screen allows user to   */
/*                 do some function tests to check the hardware     */
/*                 such as: Alarm LED, Voice, Buzzer                */
/*                , LCD/Touch panel 		                        */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/22      Viet Le			  	new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/
#include "WM.h"
#include "FunctionTestPageFourScreen.h"
#include "MainScreen.h"
#include "BUTTON.h"
#include "StringTable.h"
#include "EDIT.h"
#include "SettingOnOffButton.h"
#include "SettingOpenCloseButton.h"
#include "setting.h"
#include "GuiTask.h"
#include "Font.h"
#include "MaintenanceScreen.h"
#include "FunctionTestPageThreeScreen.h"
#include "OptionScreen.h"
#include "AlarmInfor.h"
#include "ScreenTest.h"
#include "TouchPanelScreen.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

WM_HWIN g_functionTestPageFourScreen;

static BUTTON_Handle s_prevBtnPageFour;
static EDIT_Handle s_alarmRedLEDBtn;
static EDIT_Handle s_alarmYellowLEDBtn;
static EDIT_Handle s_alarmPowerLEDBtn;
static EDIT_Handle s_voiceBtn;
static EDIT_Handle s_buzzerBtn;
static BUTTON_Handle s_screenTestBtn;
static BUTTON_Handle s_touchPanelTestBtn;
static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;

/*******************************************************************
 * *
The points of the arrow
 */
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
/* General 	-functionTestPageFourScreen_CallbackPrevBtn	                                */
/*                                                                                      */
/* Details  -call back function for previous button, handle event from window		    */
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
static void functionTestPageFourScreen_CallbackPrevBtn(WM_MESSAGE * pMsg)
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
			functionTestPageFourScreen_Hide();
			functionTestPageThreeScreen_Show();
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
/* General 	-functionTestPageFourScreen_CallbackScrTestBtn	                            */
/*                                                                                      */
/* Details  -call back function for screen test button, handle event from window		*/
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
void functionTestPageFourScreen_CallbackScrTestBtn(WM_MESSAGE * pMsg)
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
			guiTask_SetFont(&GUI_FontEng3L12B2PP);
			GUI_DispStringHCenterAt("Screen test", Rect.x1/2,18+2);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("スクリーンテスト", Rect.x1/2,18);
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
/* General 	-functionTestPageFourScreen_CallbackTouchTestBtn                            */
/*                                                                                      */
/* Details  -call back function for touch panel test button, handle event from window   */
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
void functionTestPageFourScreen_CallbackTouchTestBtn(WM_MESSAGE * pMsg)
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
			guiTask_SetFont(&GUI_FontEng3L12B2PP);
			GUI_DispStringHCenterAt("Touch panel test", Rect.x1 / 2,18+2);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("タッチパネルテスト", Rect.x1 / 2,18);
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
/* General 	-functionTestPageFourScreen_CallbackBackBtn	                                */
/*                                                                                      */
/* Details  -call back function for back button, handle event from window  			    */
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
void functionTestPageFourScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			functionTestPageFourScreen_Hide();
			settingOnOffButton_SetOffAllDevice();
			settingOpenCloseButton_SetOffLatchingValve();
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
/* General 	-functionTestPageFourScreen_CallbackHomeBtn	                                */
/*                                                                                      */
/* Details  -call back function for home button, handle event from window  			    */
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
void functionTestPageFourScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			functionTestPageFourScreen_Hide();
			settingOnOffButton_SetOffAllDevice();
			settingOpenCloseButton_SetOffLatchingValve();
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
/* General 	-functionTestPageFourScreen_Callback	                                    */
/*                                                                                      */
/* Details  -call back function for function test page four, handle event from window  	*/
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
void functionTestPageFourScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(GUI_BLACK);
		GUI_AA_DrawRoundedRect(10,65,263,390,10);
		GUI_AA_DrawRoundedRect(273,65,526,390,10);
		GUI_AA_DrawRoundedRect(536,65,789,390,10);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			GUI_SetColor(GUI_BLACK);
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Function test", 400,5);

			guiTask_SetFont(&GUI_FontEng3L12B2PP);
			GUI_DispStringAt("LED",30,55);
			GUI_DispStringAt("Other",293,55);
			GUI_DispStringAt("LCD/Touch panel",556,55);
		}
		else
		{
			GUI_SetColor(GUI_BLACK);
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("機能テスト", 400,5);

			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringAt("LED",30,55);
			GUI_DispStringAt("その他",293,55);
			GUI_DispStringAt("LCD /タッチパネル",556,55);
		}
		guiTask_SetFont(&GUI_FontEng3L18B2PP);
		GUI_DispStringHCenterAt("4/4", 400,440);
		break;
	case WM_NOTIFY_PARENT:
		if(WM_IsVisible(pMsg->hWin))
		{
			switch(WM_GetId(pMsg->hWinSrc))
			{
			case GUI_ID_BUTTON2:
				if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
				{
					functionTestPageFourScreen_Hide();
					screenTest_Show();
				}
				break;
			case GUI_ID_BUTTON3:
				if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
				{
					functionTestPageFourScreen_Hide();
					touchPanelScreen_Show();
				}
				break;
			default:
				break;
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
/* General 	-functionTestPageFourScreen_Init 							            	*/
/*                                                                                      */
/* Details  -Initialize function test page four 			            	            */
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
/*                     (O) g_functionTestPageFourScreen : function test page four       */
/*                     (O) s_alarmRedLEDBtn : ON/OFF red LED button						*/
/*                     (O) s_alarmYellowLEDBtn : ON/OFF yellow LED button 				*/
/*                     (O) s_alarmPowerLEDBtn : ON/OFF power LED button					*/
/*                     (O) s_voiceBtn : ON/OFF voice button						        */
/*                     (O) s_buzzerBtn : ON/OFF buzzer button						    */
/*                     (O) s_screenTestBtn : screen test button						    */
/*                     (O) s_touchPanelTestBtn : touch panel test button				*/
/*                     (O) s_prevBtnPageFour : previous button						    */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button						                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageFourScreen_Init(void)
{
	GUI_RECT Rect;

	//create a window as basic
	g_functionTestPageFourScreen = WM_CreateWindowAsChild(FUNCTIONTEST_PAGEFOUR_X, FUNCTIONTEST_PAGEFOUR_Y, FUNCTIONTEST_PAGEFOUR_SIZE_X,
			FUNCTIONTEST_PAGEFOUR_SIZE_Y,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,functionTestPageFourScreen_Callback,0);

	s_alarmRedLEDBtn = EDIT_CreateEx(30,80,220,70,g_functionTestPageFourScreen,WM_CF_SHOW,0,eTurnAlarmRedLEDStringID,50);
	WM_GetClientRectEx(s_alarmRedLEDBtn,&Rect);
	settingOnOffLargeButton_Init(s_alarmRedLEDBtn,eOnBtnAlarmRedLEDStringId,eOffBtnAlarmRedLEDStringId,
			eValueRedLedId,eValueOfRedLedStringId,Rect);
	WM_SetCallback(s_alarmRedLEDBtn,settingOnOffButton_Callback);

	s_alarmYellowLEDBtn = EDIT_CreateEx(30,170,220,70,g_functionTestPageFourScreen,WM_CF_SHOW,0,eTurnAlarmYellowLEDStringID,50);
	WM_GetClientRectEx(s_alarmYellowLEDBtn,&Rect);
	settingOnOffLargeButton_Init(s_alarmYellowLEDBtn,eOnBtnAlarmYellowLEDStringId,eOffBtnAlarmYellowLEDStringId,
			eValueYellowLedId,eValueOfYellowLedStringId,Rect);
	WM_SetCallback(s_alarmYellowLEDBtn,settingOnOffButton_Callback);

	s_alarmPowerLEDBtn = EDIT_CreateEx(30,260,220,70,g_functionTestPageFourScreen,WM_CF_SHOW,0,eTurnLEDPowerAnomaliesStringID,50);
	WM_GetClientRectEx(s_alarmPowerLEDBtn,&Rect);
	settingOnOffLargeButton_Init(s_alarmPowerLEDBtn,eOnBtnPowerLEDStringId,eOffBtnPowerLEDStringId,
			eValuePowerLedId,eValueOfPowerLedStringId,Rect);
	WM_SetCallback(s_alarmPowerLEDBtn,settingOnOffButton_Callback);

	s_voiceBtn = EDIT_CreateEx(293,80,220,70,g_functionTestPageFourScreen,WM_CF_SHOW,0,eTurnVoiceGuidanceStringID,50);
	WM_GetClientRectEx(s_voiceBtn,&Rect);
	settingOnOffLargeButton_Init(s_voiceBtn,eOnBtnVoiceStringId,eOffBtnVoiceStringId,eValueVoiceId,
			eValueOfVoiceStringId,Rect);
	WM_SetCallback(s_voiceBtn,settingOnOffButton_Callback);

	s_buzzerBtn = EDIT_CreateEx(293,170,220,70,g_functionTestPageFourScreen,WM_CF_SHOW,0,eTurnBuzzerStringID,50);
	WM_GetClientRectEx(s_buzzerBtn,&Rect);
	settingOnOffLargeButton_Init(s_buzzerBtn,eOnBtnBuzzerStringId,eOffBtnBuzzerStringId,eValueBuzzerId,
			eValueOfBuzzerStringId,Rect);
	WM_SetCallback(s_buzzerBtn,settingOnOffButton_Callback);

	s_screenTestBtn = BUTTON_CreateEx(556,100,214,60, g_functionTestPageFourScreen, WM_CF_SHOW, 0, GUI_ID_BUTTON2);
	WM_SetCallback(s_screenTestBtn,functionTestPageFourScreen_CallbackScrTestBtn);

	s_touchPanelTestBtn = BUTTON_CreateEx(556,190,214,60, g_functionTestPageFourScreen, WM_CF_SHOW, 0, GUI_ID_BUTTON3);
	WM_SetCallback(s_touchPanelTestBtn,functionTestPageFourScreen_CallbackTouchTestBtn);

	//Prev button
	s_prevBtnPageFour = BUTTON_CreateEx(0, 415, 102, 60, g_functionTestPageFourScreen, WM_CF_SHOW, 0, ePrevBtnPageFourStringId);
	WM_SetCallback(s_prevBtnPageFour,functionTestPageFourScreen_CallbackPrevBtn);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_functionTestPageFourScreen, WM_CF_SHOW, 0, eBackBtnFunctionTestPageFourStringId);
	WM_SetCallback(s_backBtn,functionTestPageFourScreen_CallbackBackBtn);

	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_functionTestPageFourScreen, WM_CF_SHOW, 0, eHomeBtnFunctionTestPageFourStringId);
	WM_SetCallback(s_homeBtn,functionTestPageFourScreen_CallbackHomeBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-functionTestPageFourScreen_Show 							            	*/
/*                                                                                      */
/* Details  -Show function test page four 			            	           			*/
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
/*                     (O) g_functionTestPageFourScreen : function test page four       */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageFourScreen_Show(void)
{
	alarmInfor_ShowAlarmInfor();
	WM_ShowWindow(g_functionTestPageFourScreen);
	WM_BringToTop(g_functionTestPageFourScreen);
	WM_SetFocus(g_functionTestPageFourScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-functionTestPageFourScreen_Hide 							                */
/*                                                                                      */
/* Details  -Hide function test page four 			            	           			*/
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
/*                     (O) g_functionTestPageFourScreen : function test page four       */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageFourScreen_Hide(void)
{
	WM_HideWindow(g_functionTestPageFourScreen);

	return;
}

/* end of files */
