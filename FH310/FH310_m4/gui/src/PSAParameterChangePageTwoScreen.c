/********************************************************************/
/*                                                                  */
/* File Name    : PSAParameterChangePageTwoScreen.c                 */
/*                                                                  */
/* General       : The PSAParameterChangePageTwo screen allows user */
/*                 to change psa parameter                          */
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

#include "board.h"
#include <StringTools.h>
#include "PSAParameterChangePageTwoScreen.h"
#include "MainScreen.h"
#include "SettingButton.h"
#include "EDIT.h"
#include "StringTable.h"
#include "setting.h"
#include "RADIO.h"
#include "BUTTON.h"
#include "PSATableMgr.h"
#include "GuiTask.h"
#include "Font.h"
#include "MaintenanceScreen.h"
#include "OptionScreen.h"
#include "AlarmInfor.h"
#include "PSAParameterChangePageOneScreen.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

WM_HWIN g_PSAParameterChangePageTwoScreen;

static EDIT_Handle s_solenoidOpenTimeOnEndBtn;
static BUTTON_Handle s_prevBtn;
static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;

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
/* General 	-PSAParameterChangePageTwoScreen_CallbackPrevBtn                            */
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
static void PSAParameterChangePageTwoScreen_CallbackPrevBtn(WM_MESSAGE * pMsg)
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
			PSAParameterChangePageTwoScreen_Hide();
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
/* General 	-PSAParameterChangePageTwoScreen_Callback                                   */
/*                                                                                      */
/* Details  -call back function for PSA parameter page two, handle event from window	*/
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
void PSAParameterChangePageTwoScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			GUI_SetColor(GUI_BLACK);
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("PSA parameter change", 400,5);

			GUI_AA_DrawRoundedRect(10,55,390,410,10);
			guiTask_SetFont(&GUI_FontEng3L12B2PP);
			GUI_DispStringAt("PSA end sequence",25,45);
		}
		else
		{
			GUI_SetColor(GUI_BLACK);
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("PSA パラメータ変更", 400,5);

			GUI_AA_DrawRoundedRect(10,55,390,410,10);
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringAt("PSA 停止シーケンス",25,45);
		}

		guiTask_SetFont(&GUI_FontEng3L18B2PP);
		GUI_DispStringHCenterAt("2/2", 400,440);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSAParameterChangePageTwoScreen_CallbackBackBtn	                        */
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
void PSAParameterChangePageTwoScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			PSAParameterChangePageTwoScreen_Hide();
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
/* General 	-PSAParameterChangePageTwoScreen_CallbackHomeBtn	                        */
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
void PSAParameterChangePageTwoScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			PSAParameterChangePageTwoScreen_Hide();
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
/* General 	-PSAParameterChangePageTwoScreen_Init 							            */
/*                                                                                      */
/* Details  -Initialize PSA parameter page two 			            	                */
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
/*                     (O) g_PSAParameterChangePageTwoScreen : psa parameter change page two screen*/
/*                     (O) s_solenoidOpenTimeOnEndBtn : solenoid open time on end setting*/
/*                     (O) s_backBtn : back button                                      */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_prevBtn : previous button                                  */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void PSAParameterChangePageTwoScreen_Init(void)
{
	GUI_RECT Rect;

	g_PSAParameterChangePageTwoScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,PSAParameterChangePageTwoScreen_Callback,0);

	s_solenoidOpenTimeOnEndBtn = EDIT_CreateEx(30,70,250,70,g_PSAParameterChangePageTwoScreen,WM_CF_SHOW,0,eSolenoidOpenTimeOnEndStringId,50);
	WM_GetClientRectEx(s_solenoidOpenTimeOnEndBtn,&Rect);
	settingButton_InitNoSaveLargeBtn(s_solenoidOpenTimeOnEndBtn,eUpBtnSolenoidOpenTimeOnEndStringId,eDownBtnSolenoidOpenTimeOnEndStringId,eValueSettingSolenoidOpenTimeOnEndId,eValueOfSolenoidOpenTimeOnEndStringId,Rect);
	WM_SetCallback(s_solenoidOpenTimeOnEndBtn,settingButton_Callback);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_PSAParameterChangePageTwoScreen, WM_CF_SHOW, 0, eBackBtnPSAParameterStringId);
	WM_SetCallback(s_backBtn,PSAParameterChangePageTwoScreen_CallbackBackBtn);

	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_PSAParameterChangePageTwoScreen, WM_CF_SHOW, 0, eHomeBtnPSAParameterStringId);
	WM_SetCallback(s_homeBtn,PSAParameterChangePageTwoScreen_CallbackHomeBtn);

	//Prev button
	s_prevBtn = BUTTON_CreateEx(0, 415, 102, 60, g_PSAParameterChangePageTwoScreen, WM_CF_SHOW, 0, GUI_ID_BUTTON1);
	WM_SetCallback(s_prevBtn,PSAParameterChangePageTwoScreen_CallbackPrevBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSAParameterChangePageTwoScreen_Show 							           	*/
/*                                                                                      */
/* Details  -Show PSA parameter page two 			            	                    */
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
/*                     (O) g_PSAParameterChangePageTwoScreen : psa parameter page two screen*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void PSAParameterChangePageTwoScreen_Show(void)
{
	alarmInfor_ShowAlarmInfor();

	int8_t buffer[BUFFER_VALUE_PSA_PAGE_TWO] = {'\0'};
	double floatValue = 0.0;

	uint8_t flowValue = (uint8_t)(settingButton_GetValue(eValueSettingFRPSAId) * FC_OUTPUT_SCALE_PAGE_TWO);

	E_O2ConcentratorType conditionO2 = eNormalO2Type;
	E_TemperatureType conditionTemp = eHighTempType;

	int valueO2Radio = PSAParameterChangePageOneScreen_GetConditionO2();
	int valueTempRadio = PSAParameterChangePageOneScreen_GetConditionTemp();

	if(valueO2Radio >= 0)
	{
		conditionO2 = (E_O2ConcentratorType)valueO2Radio;
	}

	if(valueTempRadio >= 0)
	{
		conditionTemp = (E_TemperatureType)valueTempRadio;
	}

	floatValue = (double)PSATableMgr_GetPSAItem(flowValue,conditionO2,conditionTemp,eSolenoidOpenTimeOnEnd) / PSA_VALUE_SCALE_PAGE_TWO;
	sprintf((char*)buffer,"%0.0f",floatValue);
	settingButton_SetValue(eValueSettingSolenoidOpenTimeOnEndId,buffer);

	WM_ShowWindow(g_PSAParameterChangePageTwoScreen);
	WM_BringToTop(g_PSAParameterChangePageTwoScreen);
	WM_SetFocus(g_PSAParameterChangePageTwoScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSAParameterChangePageTwoScreen_Hide 							           	*/
/*                                                                                      */
/* Details  -Hide PSA parameter page two 			            	                    */
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
/*                     (O) g_PSAParameterChangePageTwoScreen : psa parameter page two screen*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void PSAParameterChangePageTwoScreen_Hide(void)
{
	WM_HideWindow(g_PSAParameterChangePageTwoScreen);

	return;
}

/* end of files */

