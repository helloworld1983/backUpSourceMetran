/********************************************************************/
/*                                                                  */
/* File Name    : PSAParameterChangePageOneScreen.c                 */
/*                                                                  */
/* General       : The PSAParameterChangePageOne screen allows user */
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
/* #0001       2017/10/27	   Viet Le				modify			*/
/*-Edit function : RADIO_SetFont()->guiTaskRADIO_SetFont()(bug 1997)*/
/*                                                                  */
/********************************************************************/

#include "string.h"
#include "stdlib.h"
#include <StringTools.h>
#include "PSAParameterChangePageOneScreen.h"
#include "MainScreen.h"
#include "SettingButton.h"
#include "EDIT.h"
#include "StringTable.h"
#include "board.h"
#include "setting.h"
#include "RADIO.h"
#include "BUTTON.h"
#include "PSAParaChangeDialog.h"
#include "PSATableMgr.h"
#include "GuiTask.h"
#include "ModeMgr.h"
#include "TEXT.h"
#include "Font.h"
#include "AlarmInfor.h"
#include "MaintenanceScreen.h"
#include "OptionScreen.h"
#include "PSAParameterChangePageTwoScreen.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

//Define item id
enum{
	eFlowRatePSASettingInPSAScreenId,
	eRadioBtnTempConditionId,
	eRadioBtnO2ConditionId
};

WM_HWIN g_PSAParameterChangePageOneScreen;

static EDIT_Handle s_highTemBtn;
static EDIT_Handle s_lowTemBtn;
static EDIT_Handle s_oxygenLimitBtn;
static EDIT_Handle s_flowRateBtn;

static RADIO_Handle s_tempRadio;
static RADIO_Handle s_O2Radio;

static BUTTON_Handle s_PSABtn;
static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static BUTTON_Handle s_nextBtn;

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
/* General 	-PSAParameterChangePageOneScreen_CallbackNextBtn	                        */
/*                                                                                      */
/* Details  -call back function for next button, handle event from window		    	*/
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
void PSAParameterChangePageOneScreen_CallbackNextBtn(WM_MESSAGE * pMsg)
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
			GUI_DispStringAt("Next", 20,20 + 2);//Next
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
			PSAParameterChangePageOneScreen_Hide();
			PSAParameterChangePageTwoScreen_Show();
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
/* General 	-PSAParameterChangePageOneScreen_HideAllChild	                            */
/*                                                                                      */
/* Details  -Hide all child when show psa parameter setting								*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE 					  											*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_highTemBtn : high temp setting                             */
/*                     (O) s_lowTemBtn	: low temp setting					            */
/*                     (O) s_oxygenLimitBtn : O2 limit setting                          */
/*                     (O) s_flowRateBtn : flow rate in psa						        */
/*                     (O) s_tempRadio : temp radio button                              */
/*                     (O) s_O2Radio : O2 radio button						            */
/*                     (O) s_PSABtn : psa button                                        */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button                                      */
/*                     (O) s_nextBtn : next button						                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void PSAParameterChangePageOneScreen_HideAllChild(void)
{
	WM_HideWindow(s_highTemBtn);
	WM_HideWindow(s_lowTemBtn);
	WM_HideWindow(s_oxygenLimitBtn);
	WM_HideWindow(s_flowRateBtn);
	WM_HideWindow(s_tempRadio);
	WM_HideWindow(s_O2Radio);
	WM_HideWindow(s_PSABtn);
	WM_HideWindow(s_backBtn);
	WM_HideWindow(s_homeBtn);
	WM_HideWindow(s_nextBtn);
	alarmInfor_HideAlarmInfor();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSAParameterChangePageOneScreen_ShowAllChild	                            */
/*                                                                                      */
/* Details  -Show all child when hide psa parameter setting								*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE 					  											*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_highTemBtn : high temp setting                             */
/*                     (O) s_lowTemBtn	: low temp setting					            */
/*                     (O) s_oxygenLimitBtn : O2 limit setting                          */
/*                     (O) s_flowRateBtn : flow rate in psa						        */
/*                     (O) s_tempRadio : temp radio button                              */
/*                     (O) s_O2Radio : O2 radio button						            */
/*                     (O) s_PSABtn : psa button                                        */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button                                      */
/*                     (O) s_nextBtn : next button						                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void PSAParameterChangePageOneScreen_ShowAllChild(void)
{
	WM_ShowWindow(s_highTemBtn);
	WM_ShowWindow(s_lowTemBtn);
	WM_ShowWindow(s_oxygenLimitBtn);
	WM_ShowWindow(s_flowRateBtn);
	WM_ShowWindow(s_tempRadio);
	WM_ShowWindow(s_O2Radio);
	WM_ShowWindow(s_PSABtn);
	WM_ShowWindow(s_backBtn);
	WM_ShowWindow(s_homeBtn);
	WM_ShowWindow(s_nextBtn);
	alarmInfor_ShowAlarmInfor();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSAParameterChangePageOneScreen_Callback	                                */
/*                                                                                      */
/* Details  -call back function for PSA parameter page one, handle event from window	*/
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
void PSAParameterChangePageOneScreen_Callback(WM_MESSAGE *pMsg)
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
			GUI_DispStringAt("PSA parameter change conditions",25,45);

			GUI_AA_DrawRoundedRect(410,55,790,410,10);
			GUI_DispStringAt("Flow rate by PSA parameters",425,45);
		}
		else
		{
			GUI_SetColor(GUI_BLACK);
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("PSA パラメータ変更", 400,5);

			GUI_AA_DrawRoundedRect(10,55,390,410,10);
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringAt("PSA パラメータ変更条件",25,45);

			GUI_AA_DrawRoundedRect(410,55,790,410,10);
			GUI_DispStringAt("流量別PSA パラメータ",425,45);
		}

		guiTask_SetFont(&GUI_FontEng3L18B2PP);
		GUI_DispStringHCenterAt("1/2", 400,440);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSAParameterChangePageOneScreen_CallbackBackBtn	                        */
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
void PSAParameterChangePageOneScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			PSAParameterChangePageOneScreen_Hide();
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
/* General 	-PSAParameterChangePageOneScreen_CallbackHomeBtn	                        */
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
void PSAParameterChangePageOneScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			if(modeMgr_GetCurrentMode() == eServiceMode)
			{
				uint8_t flowrateSetting = guiInterface_GetSettingValue(eFlowRateSettingId);

				if(flowrateSetting >= SETTING_FR_025L && flowrateSetting <= SETTING_FR_300L)
				{
					guiTask_SendPSAParaEvent(ePacketPSAParaChangeId,flowrateSetting,
							guiTask_GetConditionO2(),guiTask_GetConditionTemp());
				}
			}
			PSAParameterChangePageOneScreen_Hide();
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
/* General 	-PSAParameterChangePageOneScreen_CallbackRadioTempBtn                       */
/*                                                                                      */
/* Details  -call back function for radio temp button, handle event from window			*/
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
void PSAParameterChangePageOneScreen_CallbackRadioTempBtn(WM_MESSAGE *pMsg)
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
/* General 	-PSAParameterChangePageOneScreen_CallbackRadioO2Btn	                        */
/*                                                                                      */
/* Details  -call back function for radio O2 button, handle event from window			*/
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
void PSAParameterChangePageOneScreen_CallbackRadioO2Btn(WM_MESSAGE *pMsg)
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
/* General 	-PSAParameterChangePageOneScreen_CallbackPSABtn	                            */
/*                                                                                      */
/* Details  -call back function for PSA parameter button, handle event from window		*/
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
void PSAParameterChangePageOneScreen_CallbackPSABtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Parameter change", Rect.x1/2, 12);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("パラメータ変更", Rect.x1/2, 14);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			PSATableMgr_SetEEPROMSelectPort();
			PSAParameterChangePageOneScreen_HideAllChild();
			PSAParaChangeDialog_Show();
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
/* General 	-PSAParameterChangePageOneScreen_Init 							            */
/*                                                                                      */
/* Details  -Initialize PSA parameter page one 			            	                */
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
/*                     (O) g_PSAParameterChangePageOneScreen : psa parameter change page one screen*/
/*                     (O) s_highTemBtn : high temp setting                             */
/*                     (O) s_lowTemBtn	: low temp setting					            */
/*                     (O) s_oxygenLimitBtn : O2 limit setting                          */
/*                     (O) s_flowRateBtn : flow rate in psa						        */
/*                     (O) s_tempRadio : temp radio button                              */
/*                     (O) s_O2Radio : O2 radio button						            */
/*                     (O) s_PSABtn : psa button                                        */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button                                      */
/*                     (O) s_nextBtn : next button						                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void PSAParameterChangePageOneScreen_Init(void)
{
	GUI_RECT Rect;

	g_PSAParameterChangePageOneScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,PSAParameterChangePageOneScreen_Callback,0);

	s_highTemBtn = EDIT_CreateEx(30,80,265,70,g_PSAParameterChangePageOneScreen,WM_CF_SHOW,0,eHighTemperatureStringID,50);
	EDIT_SetFocussable(s_highTemBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_highTemBtn,&Rect);
	settingButton_InitSaveLargeBtn(s_highTemBtn,eUpBtnHighTempStringId,eDownBtnHighTempStringId,eValueHightempSettingId,
			eValueOfHighTempStringId,eHightempSettingId,Rect);
	WM_SetCallback(s_highTemBtn,settingButton_Callback);

	s_lowTemBtn = EDIT_CreateEx(30,180,265,70,g_PSAParameterChangePageOneScreen,WM_CF_SHOW,0,eLowTemperatureStringID,50);
	EDIT_SetFocussable(s_lowTemBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_lowTemBtn,&Rect);
	settingButton_InitSaveLargeBtn(s_lowTemBtn,eUpBtnLowTempStringId,eDownBtnLowTempStringId,eValueLowTempSettingId,
			eValueOfLowTempStringId,eLowTempSettingId,Rect);
	WM_SetCallback(s_lowTemBtn,settingButton_Callback);

	s_oxygenLimitBtn = EDIT_CreateEx(30,280,300,70,g_PSAParameterChangePageOneScreen,WM_CF_SHOW,0,eLimitOxygenConcentratorStringID,50);
	EDIT_SetFocussable(s_oxygenLimitBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_oxygenLimitBtn,&Rect);
	settingButton_InitSaveLargeBtn(s_oxygenLimitBtn,eUpBtnO2LimitStringId,eDownBtnO2LimitStringId,
			eValueOxyLimitSettingId,eValueOfO2LimitTempStringId,eOxyLimitSettingId,Rect);
	WM_SetCallback(s_oxygenLimitBtn,settingButton_Callback);

	s_flowRateBtn = EDIT_CreateEx(430,80,265,70,g_PSAParameterChangePageOneScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eFlowRatePSAStringID,50);
	EDIT_SetFocussable(s_flowRateBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_flowRateBtn,&Rect);
	settingButton_InitNoSaveLargeBtn(s_flowRateBtn,eUpBtnFRPSAStringId,eDownBtnFRPSAStringId,eValueSettingFRPSAId,
			eValueOfFRPSAStringId,Rect);
	WM_SetCallback(s_flowRateBtn,settingButton_Callback);
	settingButton_SetValue(eValueSettingFRPSAId,(int8_t*)"0.25");

	s_tempRadio = RADIO_CreateEx(430,170,180,120,g_PSAParameterChangePageOneScreen,WM_CF_SHOW,0,eRadioBtnTempConditionId,3,40);
	guiTaskRADIO_SetFont(s_tempRadio,&GUI_FontEng3L12B2PP);

	RADIO_SetText(s_tempRadio,"High temp",0);
	RADIO_SetText(s_tempRadio,"Normal temp",1);
	RADIO_SetText(s_tempRadio,"Low temp",2);

	RADIO_SetFocusColor(s_tempRadio,SERVICE_BACKGROUND_COLOR);
	WM_SetCallback(s_tempRadio,PSAParameterChangePageOneScreen_CallbackRadioTempBtn);

	s_O2Radio = RADIO_CreateEx(610,170,180,120,g_PSAParameterChangePageOneScreen,WM_CF_SHOW,0,eRadioBtnO2ConditionId,2,40);
	guiTaskRADIO_SetFont(s_O2Radio,&GUI_FontEng3L12B2PP);
	RADIO_SetText(s_O2Radio,"Normal O2",0);
	RADIO_SetText(s_O2Radio,"Lower O2",1);
	RADIO_SetFocusColor(s_O2Radio,SERVICE_BACKGROUND_COLOR);
	WM_SetCallback(s_O2Radio,PSAParameterChangePageOneScreen_CallbackRadioO2Btn);

	s_PSABtn = BUTTON_CreateEx(480,330,250,50, g_PSAParameterChangePageOneScreen, WM_CF_SHOW, 0, GUI_ID_BUTTON8);
	WM_SetCallback(s_PSABtn,PSAParameterChangePageOneScreen_CallbackPSABtn);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_PSAParameterChangePageOneScreen, WM_CF_SHOW, 0, eBackBtnPSAParameterStringId);
	WM_SetCallback(s_backBtn,PSAParameterChangePageOneScreen_CallbackBackBtn);

	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_PSAParameterChangePageOneScreen, WM_CF_SHOW, 0, eHomeBtnPSAParameterStringId);
	WM_SetCallback(s_homeBtn,PSAParameterChangePageOneScreen_CallbackHomeBtn);

	//Next button
	s_nextBtn = BUTTON_CreateEx(700, 415, 100, 60, g_PSAParameterChangePageOneScreen, WM_CF_SHOW, 0,eNexBtnPageTwoStringId);
	WM_SetCallback(s_nextBtn,PSAParameterChangePageOneScreen_CallbackNextBtn);

	PSAParaChangeDialog_Init();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSAParameterChangePageOneScreen_Show							           	*/
/*                                                                                      */
/* Details  -Show PSA parameter page one 			            	                    */
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
/*                     (O) s_tempRadio : temperature radio button                       */
/*                     (O) s_O2Radio : O2 radio button						            */
/*                     (O) g_PSAParameterChangePageOneScreen : psa parameter change page one screen*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void PSAParameterChangePageOneScreen_Show(void)
{
	alarmInfor_ShowAlarmInfor();

	int8_t buffer[BUFFER_SIZE_VALUE_PSA_PAGE_ONE] = {'\0'};

	uint8_t highTemp = guiInterface_GetSettingValue(eHightempSettingId);
	uint8_t lowTemp = guiInterface_GetSettingValue(eLowTempSettingId);
	uint8_t o2Limit = guiInterface_GetSettingValue(eOxyLimitSettingId);

	if(highTemp >= MIN_HIGHTEMP_SETTING && highTemp <= MAX_HIGHTEMP_SETTING)
	{
		itoa(highTemp,(char*)buffer,10);
		settingButton_SetValue(eValueHightempSettingId,buffer);
	}

	if(lowTemp <= MAX_LOWTEMP_SETTING)
	{
		itoa(lowTemp,(char*)buffer,10);
		settingButton_SetValue(eValueLowTempSettingId,buffer);
	}

	if(o2Limit >= MIN_OXY_LIMIT_SETTING && o2Limit <= MAX_OXY_LIMIT_SETTING)
	{
		itoa(o2Limit,(char*)buffer,10);
		settingButton_SetValue(eValueOxyLimitSettingId,buffer);
	}

	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		guiTaskRADIO_SetFont(s_tempRadio,&GUI_FontEng3L12B2PP);

		RADIO_SetText(s_tempRadio,"High temp",0);
		RADIO_SetText(s_tempRadio,"Normal temp",1);
		RADIO_SetText(s_tempRadio,"Low temp",2);

		guiTaskRADIO_SetFont(s_O2Radio,&GUI_FontEng3L12B2PP);
		RADIO_SetText(s_O2Radio,"Normal O2",0);
		RADIO_SetText(s_O2Radio,"Lower O2",1);
	}
	else
	{
		guiTaskRADIO_SetFont(s_tempRadio,&GUI_FontJapan3L12B2PP);

		RADIO_SetText(s_tempRadio,"高温",0);
		RADIO_SetText(s_tempRadio,"常温",1);
		RADIO_SetText(s_tempRadio,"低温",2);

		guiTaskRADIO_SetFont(s_O2Radio,&GUI_FontJapan3L12B2PP);
		RADIO_SetText(s_O2Radio,"酸素濃度正常",0);
		RADIO_SetText(s_O2Radio,"酸素濃度低下",1);
	}

	RADIO_SetValue(s_tempRadio,eNormalTempType);
	RADIO_SetValue(s_O2Radio,eNormalO2Type);

	PSAParaChangeDialog_Hide();
	PSAParameterChangePageOneScreen_ShowAllChild();

	WM_ShowWindow(g_PSAParameterChangePageOneScreen);
	WM_BringToTop(g_PSAParameterChangePageOneScreen);
	WM_SetFocus(g_PSAParameterChangePageOneScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSAParameterChangePageOneScreen_Hide 							           	*/
/*                                                                                      */
/* Details  -Hide PSA parameter page one 			            	                    */
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
/*                     (O) g_PSAParameterChangePageOneScreen : psa parameter change page one screen*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void PSAParameterChangePageOneScreen_Hide(void)
{
	WM_HideWindow(g_PSAParameterChangePageOneScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSAParameterChangePageOneScreen_GetConditionTemp 							*/
/*                                                                                      */
/* Details  -Get condition temperature setting			            	                */
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_tempRadio : temperature radio button 					   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
int PSAParameterChangePageOneScreen_GetConditionTemp(void)
{
	return RADIO_GetValue(s_tempRadio);
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSAParameterChangePageOneScreen_GetConditionO2 							*/
/*                                                                                      */
/* Details  -Get condition O2 setting			            	                		*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_O2Radio : O2 radio button                                  */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
int PSAParameterChangePageOneScreen_GetConditionO2(void)
{
	return RADIO_GetValue(s_O2Radio);
}

/* end of files */
