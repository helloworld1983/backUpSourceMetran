/********************************************************************/
/*                                                                  */
/* File Name    : EquipmentCumulativeHourScreen.c                   */
/*                                                                  */
/* General       : Displaying unit cumulative hour in 				*/
/* 				   equipment information 							*/
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
/*  #0001	   2017/10/27      Viet Le 	    	        modify	    */
/*	  Edit : EDIT_SetFont()->guiTaskEDIT_SetFont(bug 1997)		    */
/*                                                                  */
/********************************************************************/

#include <StringTools.h>
#include "EquipmentCumulativeHourScreen.h"
#include "WM.h"
#include "MainScreen.h"
#include "setting.h"
#include "GuiTask.h"
#include "BUTTON.h"
#include "EDIT.h"
#include "ModeMgr.h"
#include "string.h"
#include "GuiHandler.h"
#include "TEXT.h"
#include "stdlib.h"
#include "Font.h"
#include "ServiceScreen.h"
#include "EquipmentScreen.h"
#include "OptionScreen.h"
#include "ConnectedEquipmentInfoScreen.h"
#include "AlarmInfor.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

//Define item id
enum{
	eBackBtnEquipmentCumulativeHourScreenStringId,
	eHomeBtnEquipmentCumulativeHourScreenStringId,
	ePrevBtnEquipmentCumulativeHourScreenStringId,
	eUintCumulativeHourThreeStringId,
	eUintCumulativeHourTwoFiveStringId,
	eUintCumulativeHourTwoStringId,
	eUintCumulativeHourOneSevenFiveStringId,
	eUintCumulativeHourOneFiveStringId,
	eUintCumulativeHourOneTwoFiveStringId,
	eUintCumulativeHourOneStringId,
	eUintCumulativeHourZeroSevenFiveStringId,
	eUintCumulativeHourZeroFiveStringId,
	eUintCumulativeHourZeroTwoFiveStringId,
	eTimerUpdateCumulativeHourFowRateStringId
};

WM_HWIN g_equipmentCumulativeHourScreen;

static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static BUTTON_Handle s_prevBtn;
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
/* General 	-equipmentCumulativeHourScreen_DisplayCumulative	 						*/
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
/* Using Global Data : (I) g_equipmentCumulativeHourScreen : EQ cumulative hour screen	*/
/*                     (O) s_ThreeEdit : display cumulative hour for 3L                 */
/*                     (O) s_TwoFiveEdit : display cumulative hour for 2.5L			    */
/*                     (O) s_TwoEdit : display cumulative hour for 2L					*/
/*                     (O) s_OneSevenFiveEdit : display cumulative hour for 1.75L 		*/
/*                     (O) s_OneFiveEdit : display cumulative hour for 1.5L				*/
/*                     (O) s_OneTwoFiveEdit : display cumulative hour for 1.25L 		*/
/*                     (O) s_OneEdit : display cumulative hour for 1L 					*/
/*                     (O) s_ZeroSevenFiveEdit : display cumulative hour for 0.75L		*/
/*                     (O) s_ZeroFiveEdit : display cumulative hour for 0.5L			*/
/*                     (O) s_ZeroTwoFiveEdit : display cumulative hour for 0.25L		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentCumulativeHourScreen_DisplayCumulative(void)
{
	if(WM_IsVisible(g_equipmentCumulativeHourScreen))
	{
		if(guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_300L)
		{
			EDIT_SetText(s_ThreeEdit, (char*)stringtools_GetCumlativeString(uints_300));
		}
		else if (guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_250L)
		{
			EDIT_SetText(s_TwoFiveEdit, (char*)stringtools_GetCumlativeString(units_250));
		}
		else if (guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_200L)
		{
			EDIT_SetText(s_TwoEdit, (char*)stringtools_GetCumlativeString(uints_200));
		}
		else if (guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_175L)
		{
			EDIT_SetText(s_OneSevenFiveEdit, (char*)stringtools_GetCumlativeString(uints_175));
		}
		else if (guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_150L)
		{
			EDIT_SetText(s_OneFiveEdit, (char*)stringtools_GetCumlativeString(uints_150));
		}
		else if (guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_125L)
		{
			EDIT_SetText(s_OneTwoFiveEdit, (char*)stringtools_GetCumlativeString(uints_125));
		}
		else if (guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_100L)
		{
			EDIT_SetText(s_OneEdit, (char*)stringtools_GetCumlativeString(uints_100));
		}
		else if (guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_075L)
		{
			EDIT_SetText(s_ZeroSevenFiveEdit, (char*)stringtools_GetCumlativeString(uints_075));
		}
		else if (guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_050L)
		{
			EDIT_SetText(s_ZeroFiveEdit, (char*)stringtools_GetCumlativeString(uints_050));
		}
		else if (guiInterface_GetSettingValue(eFlowRateSettingId) == SETTING_FR_025L)
		{
			EDIT_SetText(s_ZeroTwoFiveEdit, (char*)stringtools_GetCumlativeString(uints_025));
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentCumulativeHourScreen_Callback 								    */
/*                                                                                      */
/* Details  -Call back function for equipment cumualtive hour infor screen, 			*/
/* 			 handle event from the window												*/
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
void equipmentCumulativeHourScreen_Callback(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Equipment information", 400,5);

			GUI_AA_DrawRoundedRect(10,55,790,380,10);
			guiTask_SetFont(&GUI_FontEng3L12B2PP);
			GUI_DispStringAt("Unit's cumulative time for each flow rate",30,45);

			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringAt("3.00L/min:",22,85);
			GUI_DispStringAt("2.50L/min:",22,145);
			GUI_DispStringAt("2.00L/min:",22,205);
			GUI_DispStringAt("1.75L/min:",22,265);
			GUI_DispStringAt("1.50L/min:",22,325);

			GUI_DispStringAt("1.25L/min:",402,85);
			GUI_DispStringAt("1.00L/min:",402,145);
			GUI_DispStringAt("0.75L/min:",402,205);
			GUI_DispStringAt("0.50L/min:",402,265);
			GUI_DispStringAt("0.25L/min:",402,325);
		}
		else
		{
			GUI_SetColor(GUI_BLACK);
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("機器情報表示", 400,5);

			GUI_AA_DrawRoundedRect(10,55,790,380,10);
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringAt("流量別稼働時間",30,45);

			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringAt("3.00L/分:",40,85);
			GUI_DispStringAt("2.50L/分:",40,145);
			GUI_DispStringAt("2.00L/分:",40,205);
			GUI_DispStringAt("1.75L/分:",40,265);
			GUI_DispStringAt("1.50L/分:",40,325);

			GUI_DispStringAt("1.25L/分:",420,85);
			GUI_DispStringAt("1.00L/分:",420,145);
			GUI_DispStringAt("0.75L/分:",420,205);
			GUI_DispStringAt("0.50L/分:",420,265);
			GUI_DispStringAt("0.25L/分:",420,325);
		}

		guiTask_SetFont(&GUI_FontEng3L18B2PP);
		GUI_DispStringHCenterAt("3/3", 400,440);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentCumulativeHourScreen_CallbackPrevBtn 								*/
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
static void equipmentCumulativeHourScreen_CallbackPrevBtn(WM_MESSAGE * pMsg)
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
			equipmentCumulativeHourScreen_Hide();
			connectedEquipmentInforScreen_Show();
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
/* General 	-equipmentCumulativeHourScreen_CallbackBackBtn 								*/
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
void equipmentCumulativeHourScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			equipmentCumulativeHourScreen_Hide();
			equipmentScreen_Hide();

			optionScreen_SetTab(eServiceTabId);
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
/* General 	-equipmentCumulativeHourScreen_CallbackHomeBtn 								*/
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
static void equipmentCumulativeHourScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			equipmentCumulativeHourScreen_Hide();
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
/* General 	-equipmentCumulativeHourScreen_Init											*/
/*                                                                                      */
/* Details  -Initialize equipment unit cumualtive hour information screen 			    */
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
/*                     (O) g_equipmentCumulativeHourScreen : EQ cumulative hour screen  */
/*                     (O) s_ThreeEdit : display cumulative hour for 3L                 */
/*                     (O) s_TwoFiveEdit : display cumulative hour for 2.5L			    */
/*                     (O) s_TwoEdit : display cumulative hour for 2L					*/
/*                     (O) s_OneSevenFiveEdit : display cumulative hour for 1.75L 		*/
/*                     (O) s_OneFiveEdit : display cumulative hour for 1.5L				*/
/*                     (O) s_OneTwoFiveEdit : display cumulative hour for 1.25L 		*/
/*                     (O) s_OneEdit : display cumulative hour for 1L 					*/
/*                     (O) s_ZeroSevenFiveEdit : display cumulative hour for 0.75L		*/
/*                     (O) s_ZeroFiveEdit : display cumulative hour for 0.5L			*/
/*                     (O) s_ZeroTwoFiveEdit : display cumulative hour for 0.25L		*/
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_prevBtn : previous button						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentCumulativeHourScreen_Init(void)
{
	g_equipmentCumulativeHourScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_equipmentScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,equipmentCumulativeHourScreen_Callback,0);

	s_ThreeEdit = EDIT_CreateEx(155,80,190,40,g_equipmentCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourThreeStringId,15);
	EDIT_SetFocussable(s_ThreeEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_ThreeEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_ThreeEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_TwoFiveEdit = EDIT_CreateEx(155,140,190,40,g_equipmentCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourTwoFiveStringId,15);
	EDIT_SetFocussable(s_TwoFiveEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_TwoFiveEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_TwoFiveEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_TwoEdit = EDIT_CreateEx(155,200,190,40,g_equipmentCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourTwoStringId,15);
	EDIT_SetFocussable(s_TwoEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_TwoEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_TwoEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_OneSevenFiveEdit = EDIT_CreateEx(155,260,190,40,g_equipmentCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourOneSevenFiveStringId,15);
	EDIT_SetFocussable(s_OneSevenFiveEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_OneSevenFiveEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_OneSevenFiveEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_OneFiveEdit = EDIT_CreateEx(155,320,190,40,g_equipmentCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourOneFiveStringId,15);
	EDIT_SetFocussable(s_OneFiveEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_OneFiveEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_OneFiveEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_OneTwoFiveEdit = EDIT_CreateEx(535,80,190,40,g_equipmentCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourOneTwoFiveStringId,15);
	EDIT_SetFocussable(s_OneTwoFiveEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_OneTwoFiveEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_OneTwoFiveEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_OneEdit = EDIT_CreateEx(535,140,190,40,g_equipmentCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourOneStringId,15);
	EDIT_SetFocussable(s_OneEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_OneEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_OneEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_ZeroSevenFiveEdit = EDIT_CreateEx(535,200,190,40,g_equipmentCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourZeroSevenFiveStringId,15);
	EDIT_SetFocussable(s_ZeroSevenFiveEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_ZeroSevenFiveEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_ZeroSevenFiveEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_ZeroFiveEdit = EDIT_CreateEx(535,260,190,40,g_equipmentCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourZeroFiveStringId,15);
	EDIT_SetFocussable(s_ZeroFiveEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_ZeroFiveEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_ZeroFiveEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_ZeroTwoFiveEdit = EDIT_CreateEx(535,320,190,40,g_equipmentCumulativeHourScreen,WM_CF_SHOW,0,eUintCumulativeHourZeroTwoFiveStringId,15);
	EDIT_SetFocussable(s_ZeroTwoFiveEdit,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_ZeroTwoFiveEdit ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_ZeroTwoFiveEdit ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_equipmentCumulativeHourScreen, WM_CF_SHOW, 0, eBackBtnEquipmentCumulativeHourScreenStringId);
	WM_SetCallback(s_backBtn,equipmentCumulativeHourScreen_CallbackBackBtn);

	//Home button
	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_equipmentCumulativeHourScreen, WM_CF_SHOW, 0, eHomeBtnEquipmentCumulativeHourScreenStringId);
	WM_SetCallback(s_homeBtn,equipmentCumulativeHourScreen_CallbackHomeBtn);

	//Previous button
	s_prevBtn = BUTTON_CreateEx(0, 415, 102, 60, g_equipmentCumulativeHourScreen, WM_CF_SHOW, 0, ePrevBtnEquipmentCumulativeHourScreenStringId);
	WM_SetCallback(s_prevBtn,equipmentCumulativeHourScreen_CallbackPrevBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentCumulativeHourScreen_Show											*/
/*                                                                                      */
/* Details  -Show equipment unit cumualtive hour information screen 			    	*/
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
/*                     (O) s_ThreeEdit : display cumulative hour for 3L                 */
/*                     (O) s_TwoFiveEdit : display cumulative hour for 2.5L			    */
/*                     (O) s_TwoEdit : display cumulative hour for 2L					*/
/*                     (O) s_OneSevenFiveEdit : display cumulative hour for 1.75L 		*/
/*                     (O) s_OneFiveEdit : display cumulative hour for 1.5L				*/
/*                     (O) s_OneTwoFiveEdit : display cumulative hour for 1.25L 		*/
/*                     (O) s_OneEdit : display cumulative hour for 1L 					*/
/*                     (O) s_ZeroSevenFiveEdit : display cumulative hour for 0.75L		*/
/*                     (O) s_ZeroFiveEdit : display cumulative hour for 0.5L			*/
/*                     (O) s_ZeroTwoFiveEdit : display cumulative hour for 0.25L		*/
/*                     (O) g_equipmentCumulativeHourScreen : EQ cumulative hour screen  */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentCumulativeHourScreen_Show(void)
{
	alarmInfor_ShowAlarmInfor();

	EDIT_SetText(s_ThreeEdit, (char*)stringtools_GetCumlativeString(uints_300));
	EDIT_SetText(s_TwoFiveEdit, (char*)stringtools_GetCumlativeString(units_250));
	EDIT_SetText(s_TwoEdit, (char*)stringtools_GetCumlativeString(uints_200));
	EDIT_SetText(s_OneSevenFiveEdit, (char*)stringtools_GetCumlativeString(uints_175));
	EDIT_SetText(s_OneFiveEdit, (char*)stringtools_GetCumlativeString(uints_150));
	EDIT_SetText(s_OneTwoFiveEdit, (char*)stringtools_GetCumlativeString(uints_125));
	EDIT_SetText(s_OneEdit, (char*)stringtools_GetCumlativeString(uints_100));
	EDIT_SetText(s_ZeroSevenFiveEdit, (char*)stringtools_GetCumlativeString(uints_075));
	EDIT_SetText(s_ZeroFiveEdit, (char*)stringtools_GetCumlativeString(uints_050));
	EDIT_SetText(s_ZeroTwoFiveEdit, (char*)stringtools_GetCumlativeString(uints_025));

	guihandler_StartTimerUpdateEquipInfor();

	WM_ShowWindow(g_equipmentCumulativeHourScreen);
	WM_BringToTop(g_equipmentCumulativeHourScreen);
	WM_SetFocus(g_equipmentCumulativeHourScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentCumulativeHourScreen_Hide											*/
/*                                                                                      */
/* Details  -Hide equipment unit cumualtive hour information screen 			    	*/
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
/*                     (O) g_equipmentCumulativeHourScreen : EQ cumulative hour screen  */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentCumulativeHourScreen_Hide(void)
{
	guihandler_StopTimerUpdateEquipInfor();
	WM_HideWindow(g_equipmentCumulativeHourScreen);

	return;
}

/* end of files */

