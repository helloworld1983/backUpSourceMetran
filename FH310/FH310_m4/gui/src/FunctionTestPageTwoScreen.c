/********************************************************************/
/*                                                                  */
/* File Name    : FunctionTestPageTwoScreen.c                       */
/*                                                                  */
/* General       : The FunctionTestPageTwo screen allows user to do */
/*                 some function tests to check the hardware 		*/
/*                 such as: Valve 1, Valve 2, Valve 3, Valve 4, 	*/
/*                 Valve 5 and Latching Valve	                    */
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
#include "FunctionTestPageTwoScreen.h"
#include "BUTTON.h"
#include "board.h"
#include "MainScreen.h"
#include "SettingOnOffButton.h"
#include "SettingButton.h"
#include "StringTable.h"
#include "EDIT.h"
#include "SettingOpenCloseButton.h"
#include "setting.h"
#include "GuiTask.h"
#include "Font.h"
#include "MaintenanceScreen.h"
#include "FunctionTestPageThreeScreen.h"
#include "FunctionTestPageOneScreen.h"
#include "OptionScreen.h"
#include "AlarmInfor.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

WM_HWIN g_functionTestPageTwoScreen;

static BUTTON_Handle s_prevBtn;
static BUTTON_Handle s_nextBtn;
static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static EDIT_Handle s_valve1Btn;
static EDIT_Handle s_valve2Btn;
static EDIT_Handle s_valve3Btn;
static EDIT_Handle s_valve4Btn;
static EDIT_Handle s_valve5Btn;
static EDIT_Handle s_latchingValveBtn;

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


/*******************************************************************
 * *
The points of the arrow
 */
static const GUI_POINT s_aPointNextArrow[] = {
		{0,0+10},
		{99-40,0+10},
		{99-40,0},
		{99,59/2},
		{99-40,59},
		{99-40,59-10},
		{0,59-10}
};

/****************************************************************************************/
/*                                                                                      */
/* General 	-functionTestPageTwoScreen_CallbackNextBtn                                  */
/*                                                                                      */
/* Details  -call back function for next button, handle event from window		        */
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
void functionTestPageTwoScreen_CallbackNextBtn(WM_MESSAGE * pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_FillPolygon(&s_aPointNextArrow[0],7,Rect.x0,Rect.y0);

		GUI_SetPenSize(2);
		GUI_SetColor(BORDER_COLOR);//Border
		GUI_DrawPolygon(&s_aPointNextArrow[0],7,Rect.x0,Rect.y0);

		GUI_SetColor(GUI_BLACK);
		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringAt("Next", 20,20 + 2);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringAt("次", 20,18);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			functionTestPageTwoScreen_Hide();
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
/* General 	-functionTestPageTwoScreen_CallbackPrevBtn	                                */
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
static void functionTestPageTwoScreen_CallbackPrevBtn(WM_MESSAGE * pMsg)
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
			functionTestPageTwoScreen_Hide();
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
/* General 	-functionTestPageTwoScreen_CallbackBackBtn                                  */
/*                                                                                      */
/* Details  -call back function for back button, handle event from window		        */
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
void functionTestPageTwoScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			functionTestPageTwoScreen_Hide();
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
/* General 	-functionTestPageTwoScreen_CallbackHomeBtn	                                */
/*                                                                                      */
/* Details  -call back function for home button, handle event from window		        */
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
void functionTestPageTwoScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			functionTestPageTwoScreen_Hide();
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
/* General 	-functionTestPageTwoScreen_Callback	                                        */
/*                                                                                      */
/* Details  -call back function for function test page two, handle event from window	*/
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
void functionTestPageTwoScreen_Callback(WM_MESSAGE * pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(GUI_BLACK);
		GUI_AA_DrawRoundedRect(10,60,526,390,10);
		GUI_AA_DrawRoundedRect(536,60,789,170,10);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			GUI_SetColor(GUI_BLACK);
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Function test", 400,5);

			guiTask_SetFont(&GUI_FontEng3L12B2PP);
			GUI_DispStringAt("Solenoid valve",30,50);
			GUI_DispStringAt("Flow Path switching",556,50);
		}
		else
		{
			GUI_SetColor(GUI_BLACK);
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("機能テスト", 400,5);

			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringAt("ソレノイドバルブ",30,50);
			GUI_DispStringAt("流路切替",556,50);
		}
		guiTask_SetFont(&GUI_FontEng3L18B2PP);
		GUI_DispStringHCenterAt("2/4",400,440);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-functionTestPageTwoScreen_Init 							                */
/*                                                                                      */
/* Details  -Initialize function test page two 			            	                */
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
/*                     (O) g_functionTestPageTwoScreen : function test page two         */
/*                     (O) s_valve1Btn : ON/OFF valve1 button						    */
/*                     (O) s_valve2Btn : ON/OFF valve2 button						  	*/
/*                     (O) s_valve3Btn : ON/OFF valve3 button						    */
/*                     (O) s_valve4Btn : ON/OFF valve4 button						    */
/*                     (O) s_valve5Btn : ON/OFF valve5 button						    */
/*                     (O) s_prevBtn : previous button						            */
/*                     (O) s_nextBtn : next button						                */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_latchingValveBtn : ON/OFF latching valve button			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageTwoScreen_Init(void)
{
	GUI_RECT Rect;

	g_functionTestPageTwoScreen = WM_CreateWindowAsChild(FUNCTIONTEST_PAGETWO_X, FUNCTIONTEST_PAGETWO_Y, FUNCTIONTEST_PAGETWO_SIZE_X,
			FUNCTIONTEST_PAGETWO_SIZE_Y,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,functionTestPageTwoScreen_Callback,0);

	s_valve1Btn = EDIT_CreateEx(30,75,240,70,g_functionTestPageTwoScreen,WM_CF_SHOW,0,eTurnValve1StringID,50);
	WM_GetClientRectEx(s_valve1Btn,&Rect);
	settingOnOffLargeButton_Init(s_valve1Btn,eOnBtnValve1StringId,eOffBtnValve1StringId,
			eValueValve1Id,eValueOfValve1StringId,Rect);
	WM_SetCallback(s_valve1Btn,settingOnOffButton_Callback);

	s_valve2Btn = EDIT_CreateEx(30,150,240,70,g_functionTestPageTwoScreen,WM_CF_SHOW,0,eTurnValve2StringID,50);
	WM_GetClientRectEx(s_valve2Btn,&Rect);
	settingOnOffLargeButton_Init(s_valve2Btn,eOnBtnValve2StringId,eOffBtnValve2StringId,
			eValueValve2Id,eValueOfValve2StringId,Rect);
	WM_SetCallback(s_valve2Btn,settingOnOffButton_Callback);

	s_valve3Btn = EDIT_CreateEx(30,225,240,70,g_functionTestPageTwoScreen,WM_CF_SHOW,0,eTurnValve3StringID,50);
	WM_GetClientRectEx(s_valve3Btn,&Rect);
	settingOnOffLargeButton_Init(s_valve3Btn,eOnBtnValve3StringId,eOffBtnValve3StringId,
			eValueValve3Id,eValueOfValve3StringId,Rect);
	WM_SetCallback(s_valve3Btn,settingOnOffButton_Callback);

	s_valve4Btn = EDIT_CreateEx(30,300,240,70,g_functionTestPageTwoScreen,WM_CF_SHOW,0,eTurnValve4StringID,50);
	WM_GetClientRectEx(s_valve4Btn,&Rect);
	settingOnOffLargeButton_Init(s_valve4Btn,eOnBtnValve4StringId,eOffBtnValve4StringId,
			eValueValve4Id,eValueOfValve4StringId,Rect);
	WM_SetCallback(s_valve4Btn,settingOnOffButton_Callback);

	s_valve5Btn = EDIT_CreateEx(285,75,240,70,g_functionTestPageTwoScreen,WM_CF_SHOW,0,eTurnValve5StringID,50);
	WM_GetClientRectEx(s_valve5Btn,&Rect);
	settingOnOffLargeButton_Init(s_valve5Btn,eOnBtnValve5StringId,eOffBtnValve5StringId,
			eValueValve5Id,eValueOfValve5StringId,Rect);
	WM_SetCallback(s_valve5Btn,settingOnOffButton_Callback);

	//Prev button
	s_prevBtn = BUTTON_CreateEx(0, 415, 102, 60, g_functionTestPageTwoScreen, WM_CF_SHOW, 0, GUI_ID_BUTTON1);
	WM_SetCallback(s_prevBtn,functionTestPageTwoScreen_CallbackPrevBtn);

	//Next button
	s_nextBtn = BUTTON_CreateEx(700, 415, 100, 60, g_functionTestPageTwoScreen, WM_CF_SHOW, 0,eNexBtnPageTwoStringId);
	WM_SetCallback(s_nextBtn,functionTestPageTwoScreen_CallbackNextBtn);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_functionTestPageTwoScreen, WM_CF_SHOW, 0, eBackBtnFunctionTestPageTwoStringId);
	WM_SetCallback(s_backBtn,functionTestPageTwoScreen_CallbackBackBtn);

	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_functionTestPageTwoScreen, WM_CF_SHOW, 0, eHomeBtnFunctionTestPageTwoStringId);
	WM_SetCallback(s_homeBtn,functionTestPageTwoScreen_CallbackHomeBtn);

	s_latchingValveBtn = EDIT_CreateEx(556,75,220,70,g_functionTestPageTwoScreen,WM_CF_SHOW,0,eTurnBackupSystemStringID,50);
	WM_GetClientRectEx(s_latchingValveBtn,&Rect);
	settingOpenCloseButton_Init(s_latchingValveBtn,eOpenBtnLatchingStringId,eCloseBtnLatchingStringId,eTurnLatchingValveStringID,Rect);
	WM_SetCallback(s_latchingValveBtn,settingOpenCloseButton_Callback);

	WM_HideWindow(g_functionTestPageTwoScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-functionTestPageTwoScreen_Show 							                */
/*                                                                                      */
/* Details  -Show function test page two 			            	                    */
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
/*                     (O) g_functionTestPageTwoScreen : function test page two         */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageTwoScreen_Show(void)
{
	settingOpenCloseButton_SetValue();

	alarmInfor_ShowAlarmInfor();
	WM_ShowWindow(g_functionTestPageTwoScreen);
	WM_BringToTop(g_functionTestPageTwoScreen);
	WM_SetFocus(g_functionTestPageTwoScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-functionTestPageTwoScreen_Hide 							                */
/*                                                                                      */
/* Details  -Hide function test page two 			            	                    */
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
/*                     (O) g_functionTestPageTwoScreen : function test page two         */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageTwoScreen_Hide(void)
{
	WM_HideWindow(g_functionTestPageTwoScreen);

	return;
}

/* end of files */
