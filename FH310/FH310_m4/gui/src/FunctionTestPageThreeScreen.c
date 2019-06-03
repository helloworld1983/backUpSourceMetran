/********************************************************************/
/*                                                                  */
/* File Name    : FunctionTestPageThreeScreen.c                     */
/*                                                                  */
/* General       : The FunctionTestPageThree screen allows user to  */
/*                 do some function tests to check the hardware     */
/*                 such as: Compressor motor, Cooling Fan and       */
/*                 Filter cleaning module	                        */
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
/*  #0001	   2017/10/27      Viet Le 	    	        modify	    */
/*	  Edit : EDIT_SetFont()->guiTaskEDIT_SetFont(bug 1997)			*/
/*                                                                  */
/********************************************************************/

#include "WM.h"
#include "FunctionTestPageThreeScreen.h"
#include "MainScreen.h"
#include "EDIT.h"
#include "StringTable.h"
#include "SettingOnOffButton.h"
#include "SettingOpenCloseButton.h"
#include "SettingButton.h"
#include "BUTTON.h"
#include "stdlib.h"
#include "setting.h"
#include "GuiTask.h"
#include "Font.h"
#include "MaintenanceScreen.h"
#include "FunctionTestPageTwoScreen.h"
#include "FunctionTestPageFourScreen.h"
#include "OptionScreen.h"
#include "AlarmInfor.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

WM_HWIN g_functionTestPageThreeScreen;

static EDIT_Handle s_motorCompressorBtn;
static EDIT_Handle s_settingRPMBtn;
static EDIT_Handle s_motorCoolingBtn;
static EDIT_Handle s_settingDutyBtn;
static EDIT_Handle s_motorFilterBtn;
static EDIT_Handle s_fanErrorEdit;
static EDIT_Handle s_photoSensorEdit;
static BUTTON_Handle s_prevBtn;
static BUTTON_Handle s_nextBtn;
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
/* General 	-functionTestPageThreeScreen_CallbackNextBtn	                            */
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
void functionTestPageThreeScreen_CallbackNextBtn(WM_MESSAGE * pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_FillPolygon(&s_aPointNextArrow[0],7,Rect.x0,Rect.y0);

		GUI_SetPenSize(2);
		GUI_SetColor(BORDER_COLOR);
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
			functionTestPageThreeScreen_Hide();
			functionTestPageFourScreen_Show();
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
/* General 	-functionTestPageThreeScreen_CallbackPrevBtn                                */
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
static void functionTestPageThreeScreen_CallbackPrevBtn(WM_MESSAGE * pMsg)
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
			functionTestPageThreeScreen_Hide();
			functionTestPageTwoScreen_Show();
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
/* General 	-functionTestPageThreeScreen_CallbackBackBtn	                            */
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
void functionTestPageThreeScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			functionTestPageThreeScreen_Hide();
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
/* General 	-functionTestPageThreeScreen_CallbackHomeBtn                                */
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
void functionTestPageThreeScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			functionTestPageThreeScreen_Hide();
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
/* General 	-functionTestPageThreeScreen_Callback	                                    */
/*                                                                                      */
/* Details  -call back function for function test page three, handle event from window  */
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
void functionTestPageThreeScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(GUI_BLACK);
		GUI_AA_DrawRoundedRect(10,60,263,390,10);
		GUI_AA_DrawRoundedRect(273,60,526,390,10);
		GUI_AA_DrawRoundedRect(536,60,789,390,10);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			GUI_SetColor(GUI_BLACK);
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Function test", 400,5);
			guiTask_SetFont(&GUI_FontEng3L12B2PP);
			GUI_DispStringAt("Compressor",30,50);
			GUI_DispStringAt("Cooling FAN",293,50);
			GUI_DispStringAt("Filter cleaning",556,50);

			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringAt("Fan error",383,250);
			GUI_DispStringAt("Photo sensor",646,162);
		}
		else
		{
			GUI_SetColor(GUI_BLACK);
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("機能テスト", 400,5);
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringAt("コンプレッサ",30,50);
			GUI_DispStringAt("冷却ファン",293,50);
			GUI_DispStringAt("掃除機構",556,50);

			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("ファンエラー",383,250);
			GUI_DispStringAt("フォトセンサ",646,162);
		}
		guiTask_SetFont(&GUI_FontEng3L18B2PP);
		GUI_DispStringHCenterAt("3/4",400,440);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-functionTestPageThreeScreen_Init 							            	*/
/*                                                                                      */
/* Details  -Initialize function test page three 			            	            */
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
/*                     (O) g_functionTestPageThreeScreen : function test page three     */
/*                     (O) s_motorCompressorBtn : ON/OFF compressor button				*/
/*                     (O) s_settingRPMBtn : compressor RPM setting                     */
/*                     (O) s_motorCoolingBtn : ON/OFF cooling fan button				*/
/*                     (O) s_settingDutyBtn : cooling fan duty setting                  */
/*                     (O) s_fanErrorEdit : display fan error						    */
/*                     (O) s_motorFilterBtn : ON/OFF filter cleaning button             */
/*                     (O) s_photoSensorEdit : display photo sensor						*/
/*                     (O) s_prevBtn : previous button                                  */
/*                     (O) s_nextBtn  : next button						                */
/*                     (O) s_backBtn : back button                                      */
/*                     (O) s_homeBtn : home button						                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageThreeScreen_Init(void)
{
	GUI_RECT Rect;

	g_functionTestPageThreeScreen = WM_CreateWindowAsChild(FUNCTIONTEST_PAGETHREE_X, FUNCTIONTEST_PAGETHREE_Y,
			FUNCTIONTEST_PAGETHREE_SIZE_X, FUNCTIONTEST_PAGETHREE_SIZE_Y,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,functionTestPageThreeScreen_Callback,0);

	s_motorCompressorBtn = EDIT_CreateEx(20,75,220,70,g_functionTestPageThreeScreen,WM_CF_SHOW,0,eTurnMotorCompressorStringID,50);
	WM_GetClientRectEx(s_motorCompressorBtn,&Rect);
	settingOnOffLargeButton_Init(s_motorCompressorBtn,eOnBtnMotorCompressorStringId,eOffBtnMotorCompressorStringId,
			eValueMotorCompressorId,eValueOfMotorCompressorStringId,Rect);
	WM_SetCallback(s_motorCompressorBtn,settingOnOffButton_Callback);

	s_settingRPMBtn = EDIT_CreateEx(20,165,235,70,g_functionTestPageThreeScreen,WM_CF_SHOW,0,eMotorRPMFunctionTestStringID,50);
	WM_GetClientRectEx(s_settingRPMBtn,&Rect);
	settingButton_InitNoSaveLargeBtn(s_settingRPMBtn,eUpBtnCompressorStringId,eDownBtnCompressorStringId,
			eValueSettingRPMId,eValueOfSettingRPMStringId,Rect);
	settingButton_SetValue(eValueSettingRPMId,DEFAULT_RPM_TEST_STR);
	WM_SetCallback(s_settingRPMBtn,settingButton_Callback);

	s_motorCoolingBtn = EDIT_CreateEx(283,75,220,70,g_functionTestPageThreeScreen,WM_CF_SHOW,0,eTurnMotorCoolingFANStringID,50);
	WM_GetClientRectEx(s_motorCoolingBtn,&Rect);
	settingOnOffLargeButton_Init(s_motorCoolingBtn,eOnBtnMotorCoolingStringId,eOffBtnMotorCoolingStringId,
			eValueMotorCoolingId,eValueOfMotorCoolingStringId,Rect);
	WM_SetCallback(s_motorCoolingBtn,settingOnOffButton_Callback);

	s_settingDutyBtn = EDIT_CreateEx(283,165,240,70,g_functionTestPageThreeScreen,WM_CF_SHOW,0,eCoolingFanFunctiontestStringID,50);
	WM_GetClientRectEx(s_settingDutyBtn,&Rect);
	settingButton_InitNoSaveLargeBtn(s_settingDutyBtn,eUpBtnCoolingStringId,eDownBtnCoolingStringId,
			eValueSettingDutyId,eValueOfSettingDutyStringId,Rect);
	settingButton_SetValue(eValueSettingDutyId,DEFAULT_DUTY_TEST_STR);
	WM_SetCallback(s_settingDutyBtn,settingButton_Callback);

	s_fanErrorEdit = EDIT_CreateEx(383,268,100,50,g_functionTestPageThreeScreen,WM_CF_SHOW,0,GUI_ID_EDIT0,5);
	EDIT_SetFocussable(s_fanErrorEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_fanErrorEdit,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_fanErrorEdit,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_motorFilterBtn = EDIT_CreateEx(546,75,220,70,g_functionTestPageThreeScreen,WM_CF_SHOW,0,eTurnMotorFilterCleaningStringID,50);
	WM_GetClientRectEx(s_motorFilterBtn,&Rect);
	settingOnOffLargeButton_Init(s_motorFilterBtn,eOnBtnMotorFilterStringId,eOffBtnMotorFilterStringId,
			eValueMotorFilterId,eValueOfMotorFilterStringId,Rect);
	WM_SetCallback(s_motorFilterBtn,settingOnOffButton_Callback);

	s_photoSensorEdit = EDIT_CreateEx(646,180,100,50,g_functionTestPageThreeScreen,WM_CF_SHOW,0,GUI_ID_EDIT1,5);
	EDIT_SetFocussable(s_photoSensorEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_photoSensorEdit,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_photoSensorEdit,GUI_TA_VCENTER | GUI_TA_HCENTER);

	//Prev button
	s_prevBtn = BUTTON_CreateEx(0, 415, 102, 60, g_functionTestPageThreeScreen, WM_CF_SHOW, 0, ePrevBtnPageThreeStringId);
	WM_SetCallback(s_prevBtn,functionTestPageThreeScreen_CallbackPrevBtn);

	//Next button
	s_nextBtn= BUTTON_CreateEx(700, 415, 100, 60, g_functionTestPageThreeScreen, WM_CF_SHOW, 0,eNexBtnPageThreeStringId);
	WM_SetCallback(s_nextBtn,functionTestPageThreeScreen_CallbackNextBtn);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_functionTestPageThreeScreen, WM_CF_SHOW, 0, eBackBtnFunctionTestPageThreeStringId);
	WM_SetCallback(s_backBtn,functionTestPageThreeScreen_CallbackBackBtn);

	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_functionTestPageThreeScreen, WM_CF_SHOW, 0, eHomeBtnFunctionTestPageThreeStringId);
	WM_SetCallback(s_homeBtn,functionTestPageThreeScreen_CallbackHomeBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-functionTestPageThreeScreen_Show 							                */
/*                                                                                      */
/* Details  -Show function test page three 			            	                    */
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
/*                     (O) g_functionTestPageThreeScreen : function test page three     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageThreeScreen_Show(void)
{
	alarmInfor_ShowAlarmInfor();

	settingButton_SetValue(eValueSettingRPMId, DEFAULT_RPM_TEST_STR);
	settingButton_SetValue(eValueSettingDutyId, DEFAULT_DUTY_TEST_STR);

	WM_ShowWindow(g_functionTestPageThreeScreen);
	WM_BringToTop(g_functionTestPageThreeScreen);
	WM_SetFocus(g_functionTestPageThreeScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-functionTestPageThreeScreen_Hide 							                */
/*                                                                                      */
/* Details  -Hide function test page three 			            	                    */
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
/*                     (O) g_functionTestPageThreeScreen : function test page three     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageThreeScreen_Hide(void)
{
	WM_HideWindow(g_functionTestPageThreeScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-functionTestPageThreeScreen_UpdateMonitor				 					*/
/*                                                                                      */
/* Details  -Update monitor in function test mode						 			    */
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) IPC_MSG_DATA_t data : data receive from device						*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_fanErrorEdit : display fan error                           */
/*                     (O) s_photoSensorEdit : display photo sensor						*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageThreeScreen_UpdateMonitor(IPC_MSG_DATA_t data)
{
	if(WM_IsVisible(g_functionTestPageThreeScreen))
	{
		if(data.stPsaMonitorPacket.coolingFanErrState == false)
		{
			EDIT_SetText(s_fanErrorEdit,"Low");
		}
		else
		{
			EDIT_SetText(s_fanErrorEdit,"High");
		}

		if(data.stPsaMonitorPacket.filterPhotoErrStatus == false)
		{
			EDIT_SetText(s_photoSensorEdit,"Low");
		}
		else
		{
			EDIT_SetText(s_photoSensorEdit,"High");
		}
	}

	return;
}

/* end of files */

