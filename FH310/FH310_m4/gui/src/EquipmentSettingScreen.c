/********************************************************************/
/*                                                                  */
/* File Name    : EquipmentSettingScreen.c                          */
/*                                                                  */
/* General       : Changing the equipment settings	 		        */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2008/08/22      Viet Le  			new file       	*/
/*  #0001	   2017/10/27      Viet Le 	    	        modify	    */
/*	  Edit : EDIT_SetFont()->guiTaskEDIT_SetFont; TEXT_SetFont()->  */
/*		guiTaskTEXT_SetFont() (bug 1997)							*/
/*                                                                  */
/********************************************************************/

#include "stdlib.h"
#include <EEPROM.h>
#include <StringTools.h>
#include "CreateContactDialog.h"
#include "EquipmentSettingScreen.h"
#include "MainScreen.h"
#include "WM.h"
#include "StringTable.h"
#include "EDIT.h"
#include "BUTTON.h"
#include "board.h"
#include "SettingButton.h"
#include "SettingOnOffButton.h"
#include "SettingExistNoButton.h"
#include "setting.h"
#include "TEXT.h"
#include "GuiTask.h"
#include "OperationScreen.h"
#include "Font.h"
#include "AlarmInfor.h"
#include "ServiceScreen.h"
#include "EquipmentScreen.h"
#include "OptionScreen.h"
#include "ConnectedEquipmentInfoScreen.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

//Define item id
enum{
	eTitleEquipmentSettingId,
	ePageTextEquipmentSettingId,
	eInforAlarmInEquipmentSettingId
};

WM_HWIN g_equipmentSettingScreen;

static WM_HWIN s_popUpEquipmentSetting;
static TEXT_Handle s_titleEquipSetting;
static EDIT_Handle s_settingGuidanceVolume;
static EDIT_Handle s_settingAlarmVolume;
static EDIT_Handle s_settingBuzzerVolume;
static EDIT_Handle s_settingRestingFR;
static EDIT_Handle s_settingExertionFR;
static EDIT_Handle s_settingBedtimeFR;
static EDIT_Handle s_settingLCDBackLight;
static EDIT_Handle s_settingChildLock;
static EDIT_Handle s_valuePatientCumulative;
static EDIT_Handle s_settingOxygenCylinder;
static EDIT_Handle s_valueEditContact;
static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static BUTTON_Handle s_nextBtn;
static BUTTON_Handle s_clearBtn;
static BUTTON_Handle s_createBtn;
static BUTTON_Handle s_EquipmentSettingInitializationBtn;
static BUTTON_Handle s_OKBtn;
static BUTTON_Handle s_cancelBtn;
static TEXT_Handle s_titlePopUp;
static TEXT_Handle s_pageText;
static bool s_isClearPatient = false;

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
/* General 	-equipmentSettingScreen_CallbackBackBtn 								    */
/*                                                                                      */
/* Details  -Call back function for back button, handle event from 						*/
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
static void equipmentSettingScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			equipmentSettingScreen_Hide();
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
/* General 	-equipmentSettingScreen_CallbackHomeBtn 								    */
/*                                                                                      */
/* Details  -Call back function for home button, handle event from 						*/
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
static void equipmentSettingScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			equipmentSettingScreen_Hide();
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
/* General 	-equipmentSettingScreen_CallbackNextBtn 								    */
/*                                                                                      */
/* Details  -Call back function for next button, handle event from 						*/
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
static void equipmentSettingScreen_CallbackNextBtn(WM_MESSAGE * pMsg)
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
			equipmentSettingScreen_Hide();
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
/* General 	-equipmentSettingScreen_CallbackClearBtn 								    */
/*                                                                                      */
/* Details  -Call back function for clear cumulative hours button handle, event from 	*/
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
/*                     (O) s_titlePopUp : title popup                                   */
/*                     (O) s_isClearPatient : flag clear patient		            	*/
/*                     (O) s_popUpEquipmentSetting : popup of equipment setting		    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void equipmentSettingScreen_CallbackClearBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetColor(GUI_WHITE);
		GUI_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(GUI_BLACK);
		GUI_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringAt("Clear",Rect.x0+20+5+2,Rect.y0+13+3+1);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("クリア",Rect.x0+18+5,Rect.y0+13+3);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);
				TEXT_SetText(s_titlePopUp,"Do you want to clear the patient integration time?");
			}
			else
			{
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontJapan3L16B2PP);
				TEXT_SetText(s_titlePopUp,"患者積算時間をクリアします\nよろしいですか？");
			}

			equipmentSettingScreen_HideAll();
			s_isClearPatient = true;

			WM_ShowWindow(s_popUpEquipmentSetting);
			WM_BringToTop(s_popUpEquipmentSetting);
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
/* General 	-equipmentSettingScreen_CallbackCreateBtn 								    */
/*                                                                                      */
/* Details  -Call back function for create contact button, handle event from 			*/
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
/*                     (O) g_createContactDialog : create contact dialog              	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void equipmentSettingScreen_CallbackCreateBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetColor(GUI_WHITE);
		GUI_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(GUI_BLACK);
		GUI_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringAt("Create",Rect.x0+20+5 ,Rect.y0+13+2);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("設定",Rect.x0+22+5+3,Rect.y0+13+2);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			equipmentSettingScreen_HideAll();

			createContactDialog_Show();
			WM_BringToTop(g_createContactDialog);
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
/* General 	-equipmentSettingScreen_CallbackInitializationBtn							*/
/*                                                                                      */
/* Details  -Call back function for Setting Initialization button, handle event from 	*/
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
/*                     (O) s_titlePopUp : title popup                                   */
/*                     (O) s_popUpEquipmentSetting : popup of equipment setting         */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void equipmentSettingScreen_CallbackInitializationBtn(WM_MESSAGE *pMsg)
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
			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringHCenterAt("Setting initialization", Rect.x1/2, 12+2+2);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L14B2PP);
			GUI_DispStringHCenterAt("機器設定の初期化", Rect.x1/2, 12+2+1);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);
				TEXT_SetText(s_titlePopUp,"Do you want to initialize the device settings?");
			}
			else
			{
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontJapan3L16B2PP);
				TEXT_SetText(s_titlePopUp,"機器設定を初期化します\nよろしいですか？");
			}

			WM_BringToTop(s_popUpEquipmentSetting);
			WM_ShowWindow(s_popUpEquipmentSetting);
			equipmentSettingScreen_HideAll();

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
/* General 	-equipmentSettingScreen_Callback								            */
/*                                                                                      */
/* Details  -Call back function for equipment setting screen, handle event from 	    */
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
/* Using Global Data : (I) s_popUpEquipmentSetting : popup of equipment setting         */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentSettingScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			if(!WM_IsVisible(s_popUpEquipmentSetting))
			{
				GUI_SetColor(GUI_BLACK);
				guiTask_SetFont(&GUI_FontEng3L10B2PP);
				GUI_DispStringAt("Patient cumulative operation hour",530,232);
				GUI_DispStringAt("Emergency contact number",280,322);
				GUI_SetTextAlign(GUI_TA_LEFT);
				GUI_DispStringAt("Hours",711+20,260+5);
			}
		}
		else
		{
			if(!WM_IsVisible(s_popUpEquipmentSetting))
			{
				GUI_SetColor(GUI_BLACK);
				guiTask_SetFont(&GUI_FontJapan3L12B2PP);
				GUI_DispStringAt("患者積算時間",530,230);
				GUI_DispStringAt("緊急連絡先",280,320);
				GUI_SetTextAlign(GUI_TA_LEFT);
				GUI_DispStringAt("時間",711+20,260+5);
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
/* General 	-equipmentSettingScreen_CallbackPopUp 								        */
/*                                                                                      */
/* Details  -Call back function for pop up dialog, handle event from 					*/
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
static void equipmentSettingScreen_CallbackPopUp(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(DIALOG_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,25);

		GUI_SetColor(BORDER_DIALOG_COLOR);
		GUI_AA_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,25);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentSettingScreen_CallbackOKBtn 								        */
/*                                                                                      */
/* Details  -Call back function for OK button, handle event from 						*/
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
/*                     (O) s_isClearPatient : flag clear patient cumulative    			*/
/*                     (O) s_valuePatientCumulative : display patient cumulative        */
/*                     (O) s_popUpEquipmentSetting : popup of equipment setting	        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void equipmentSettingScreen_CallbackOKBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(DIALOG_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10);

		GUI_SetColor(BORDER_COLOR);
		GUI_DrawRoundedFrame(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10,2);

		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);
		guiTask_SetFont(&GUI_FontEng3L16B2PP);
		GUI_DispStringHCenterAt("OK", Rect.x1/2, 17);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			if(s_isClearPatient == true)
			{
				EDIT_SetText(s_valuePatientCumulative,"0");
				operationScreen_ClearPatientCumulative();
			}
			else
			{
				setting_SetDefaultServiceMode();
				equipmentSettingScreen_SetDefaut();
			}

			s_isClearPatient = false;
			WM_HideWindow(s_popUpEquipmentSetting);

			equipmentSettingScreen_ShowAll();
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
/* General 	-equipmentSettingScreen_CallbackCancelBtn 								    */
/*                                                                                      */
/* Details  -Call back function for Cancel button, handle event from 					*/
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
/*                     (O) s_isClearPatient : flag clear patient cumulative             */
/*                     (O) s_popUpEquipmentSetting : popup of equipment setting			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void equipmentSettingScreen_CallbackCancelBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(DIALOG_BACKGROUND_COLOR);
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
			GUI_DispStringHCenterAt("Cancel", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("キャンセル", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_isClearPatient = false;
			WM_HideWindow(s_popUpEquipmentSetting);

			equipmentSettingScreen_ShowAll();
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
/* General 	-equipmentSettingScreen_Init								       			*/
/*                                                                                      */
/* Details  -Initialize equipment setting screen with									*/
/* 				guidance volume setting													*/
/* 				alarm volume setting													*/
/* 				buzzer volume setting													*/
/* 				flowrate setting														*/
/* 				child lock setting														*/
/* 				create contact															*/
/* 				setting default button													*/
/*																						*/
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
/*                     (O) g_equipmentSettingScreen : equipment setting screen          */
/*                     (O) s_titleEquipSetting : title equipment setting 				*/
/*                     (O) s_settingGuidanceVolume : guidance volume setting 		    */
/*                     (O) s_settingAlarmVolume : alarm volume setting 					*/
/*                     (O) s_settingBuzzerVolume : buzzer volume setting	           	*/
/*                     (O) s_settingRestingFR : resting flow rate setting				*/
/*                     (O) s_settingExertionFR : exertion flow rate setting             */
/*                     (O) s_settingBedtimeFR : bedtime flow rate setting				*/
/*                     (O) s_settingLCDBackLight : backlight setting                    */
/*                     (O) s_settingChildLock : child lock setting						*/
/*                     (O) s_clearBtn : clear button                                    */
/*                     (O) s_valuePatientCumulative : display patient cumulative		*/
/*                     (O) s_settingOxygenCylinder : oxygen cylinder setting            */
/*                     (O) s_createBtn : create contact button						    */
/*                     (O) s_valueEditContact : display contact number                  */
/*                     (O) s_EquipmentSettingInitializationBtn : initialization button	*/
/*                     (O) s_popUpEquipmentSetting : popup of equipment setting         */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button                                      */
/*                     (O) s_nextBtn : next button						                */
/*                     (O) s_pageText : page number                                     */
/*                     (O) s_OKBtn : OK button						                    */
/*                     (O) s_cancelBtn : cancel button                                  */
/*                     (O) s_titlePopUp : title popup						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentSettingScreen_Init(void)
{
	GUI_RECT Rect;

	g_equipmentSettingScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_equipmentScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,equipmentSettingScreen_Callback,0);

	s_titleEquipSetting = TEXT_CreateEx(250,5,300,60,g_equipmentSettingScreen,WM_CF_SHOW,TEXT_CF_HCENTER,eTitleEquipmentSettingId,"");

	s_settingGuidanceVolume = EDIT_CreateEx(30,50,245,70,g_equipmentSettingScreen,WM_CF_SHOW,0,eGuidanceVolumeStringId,50);
	WM_GetClientRectEx(s_settingGuidanceVolume,&Rect);
	settingButton_InitSaveLargeBtn(s_settingGuidanceVolume,eUpBtnVoiceGuidanceVolumeStringId,eDownBtnVoiceGuidanceVolumeStringId,eValueSettingGuidanceVolumeId,
			eValueOfGuidanceVolumeStringId,eGuidanceVolumeSettingId,Rect);
	WM_SetCallback(s_settingGuidanceVolume,settingButton_Callback);

	s_settingAlarmVolume = EDIT_CreateEx(280,50,245,70,g_equipmentSettingScreen,WM_CF_SHOW,0,eAlarmVolumeStringId,50);
	WM_GetClientRectEx(s_settingAlarmVolume,&Rect);
	settingButton_InitSaveLargeBtn(s_settingAlarmVolume,eUpBtnAlarmVolumeStringId,eDownBtnAlarmVolumeStringId,
			eValueSettingAlarmVolumeId,eValueOfAlarmVolumeStringId,eAlarmVolumeSettingId,Rect);
	WM_SetCallback(s_settingAlarmVolume,settingButton_Callback);

	s_settingBuzzerVolume = EDIT_CreateEx(530,50,245,70,g_equipmentSettingScreen,WM_CF_SHOW,0,eBuzzerVolumeStringId,50);
	WM_GetClientRectEx(s_settingBuzzerVolume,&Rect);
	settingButton_InitSaveLargeBtn(s_settingBuzzerVolume,eUpBtnBuzzerVolumeStringId,eDownBtnBuzzerVolumeStringId,
			eValueSettingBuzzerVolumeId,eValueOfBuzzerVolumeStringId,eBuzzerVolumeSettingId,Rect);
	WM_SetCallback(s_settingBuzzerVolume,settingButton_Callback);

	s_settingRestingFR = EDIT_CreateEx(30,140,245,70,g_equipmentSettingScreen,WM_CF_SHOW,0,eRestingFRStringId,50);
	WM_GetClientRectEx(s_settingRestingFR,&Rect);
	settingButton_InitSaveLargeBtn(s_settingRestingFR,eUpBtnRestingFRStringId,eDownBtnRestingFRStringId,
			eValueSettingRestingFRId,eValueOfRestingFRStringId,eRestingFRSettingId,Rect);
	WM_SetCallback(s_settingRestingFR,settingButton_Callback);

	s_settingExertionFR = EDIT_CreateEx(280,140,245,70,g_equipmentSettingScreen,WM_CF_SHOW,0,eExertionFRStringId,50);
	WM_GetClientRectEx(s_settingExertionFR,&Rect);
	settingButton_InitSaveLargeBtn(s_settingExertionFR,eUpBtnExertionFRStringId,eDownBtnExertionFRStringId,
			eValueSettingExertionFRId,eValueOfExertionFRStringId,eExertionFRSettingId,Rect);
	WM_SetCallback(s_settingExertionFR,settingButton_Callback);

	s_settingBedtimeFR = EDIT_CreateEx(530,140,245,70,g_equipmentSettingScreen,WM_CF_SHOW,0,eBedtimeFRStringId,50);
	WM_GetClientRectEx(s_settingBedtimeFR,&Rect);
	settingButton_InitSaveLargeBtn(s_settingBedtimeFR,eUpBtnBedtimeFRStringId,eDownBtnBedtimeFRStringId,
			eValueSettingBedtimeFRId,eValueOfBedtimeFRStringId,eBedtimeFRSettingId,Rect);
	WM_SetCallback(s_settingBedtimeFR,settingButton_Callback);

	s_settingLCDBackLight = EDIT_CreateEx(30,230,245,70,g_equipmentSettingScreen,WM_CF_SHOW,0,eLCDBrightnessStringId,50);
	WM_GetClientRectEx(s_settingLCDBackLight,&Rect);
	settingButton_InitSaveLargeBtn(s_settingLCDBackLight,eUpBtnLCDBrightnessStringId,eDownBtnLCDBrightnessStringId,
			eValueSettingLCDBackLightId,eValueOfLCDBrightnessStringId,eBrightnessSettingId,Rect);
	WM_SetCallback(s_settingLCDBackLight,settingButton_Callback);

	s_settingChildLock = EDIT_CreateEx(280,230,245,70,g_equipmentSettingScreen,WM_CF_SHOW,0,eChildlockStringID,50);
	WM_GetClientRectEx(s_settingChildLock,&Rect);
	settingOnOffLargeButton_Init(s_settingChildLock,eOnBtnChildLockStringId,eOffBtnChildLockStringId,
			eValueChildLockId,eValueOfChildLockStringId,Rect);
	WM_SetCallback(s_settingChildLock,settingOnOffButton_Callback);

	//Patients Cumulative hours
	s_clearBtn = BUTTON_CreateEx(530, 249, 100, 50, g_equipmentSettingScreen, WM_CF_SHOW, 0, eClearBtnPatientsCumulativeHoursStringId);
	WM_SetCallback(s_clearBtn,equipmentSettingScreen_CallbackClearBtn);

	s_valuePatientCumulative = EDIT_CreateEx(610+10+10,249,100,50,g_equipmentSettingScreen,WM_CF_SHOW,0,eValuePatientCumulativeHourStringId,50);
	EDIT_SetFocussable(s_valuePatientCumulative,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_valuePatientCumulative ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_valuePatientCumulative ,GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_valuePatientCumulative ,"0");

	s_settingOxygenCylinder = EDIT_CreateEx(30,320,245,70,g_equipmentSettingScreen,WM_CF_SHOW,0,eTurnOxygenCylinderStringID,50);
	WM_GetClientRectEx(s_settingOxygenCylinder,&Rect);
	settingExistNoButton_InitExistNoBtn(s_settingOxygenCylinder,eExistBtnStringId,eNoExistBtnStringId,eTurnOxygenCylinderValueStringID,Rect);
	WM_SetCallback(s_settingOxygenCylinder,settingExistNoButton_Callback);

	s_createBtn = BUTTON_CreateEx(280,339, 100, 50, g_equipmentSettingScreen, WM_CF_SHOW, 0, eCreateBtnContactStringId);
	WM_SetCallback(s_createBtn,equipmentSettingScreen_CallbackCreateBtn);

	s_valueEditContact = EDIT_CreateEx(360+20,339,190-20+10,50,g_equipmentSettingScreen,WM_CF_SHOW,0,eValueEditContactStringID,50);
	EDIT_SetFocussable(s_valueEditContact,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_valueEditContact ,&GUI_FontEng3L12B2PP);
	EDIT_SetTextAlign(s_valueEditContact ,GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_valueEditContact, (char*)setting_GetContact());

	s_EquipmentSettingInitializationBtn = BUTTON_CreateEx(570+10, 339, 210-10, 50, g_equipmentSettingScreen, WM_CF_SHOW, 0, eEquipmentSettingDefaultBtnStringId);
	WM_SetCallback(s_EquipmentSettingInitializationBtn,equipmentSettingScreen_CallbackInitializationBtn);

	s_popUpEquipmentSetting = WM_CreateWindowAsChild(50, 115, 700, 225,g_equipmentSettingScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,equipmentSettingScreen_CallbackPopUp,0);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_equipmentSettingScreen, WM_CF_SHOW, 0, eBackBtnEquipmentSettingStringId);
	WM_SetCallback(s_backBtn,equipmentSettingScreen_CallbackBackBtn);

	//Home button
	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_equipmentSettingScreen, WM_CF_SHOW, 0, eHomeBtnEquipmentSettingStringId);
	WM_SetCallback(s_homeBtn,equipmentSettingScreen_CallbackHomeBtn);

	//Next button
	s_nextBtn = BUTTON_CreateEx(700, 415, 100, 60, g_equipmentSettingScreen, WM_CF_SHOW, 0, eNexBtnEquipmentSettingStringId);
	WM_SetCallback(s_nextBtn,equipmentSettingScreen_CallbackNextBtn);

	s_pageText = TEXT_CreateEx(380,440, 50, 25,g_equipmentSettingScreen,WM_CF_SHOW,TEXT_CF_LEFT,ePageTextEquipmentSettingId,"1/2");
	guiTaskTEXT_SetFont(s_pageText, &GUI_FontEng3L18B2PP);

	//OK button
	s_OKBtn = BUTTON_CreateEx(50, 120, 280, 60, s_popUpEquipmentSetting, WM_CF_SHOW, 0, eOkBtnStringId);
	WM_SetCallback(s_OKBtn,equipmentSettingScreen_CallbackOKBtn);

	//Cancel button
	s_cancelBtn = BUTTON_CreateEx(380, 120, 280, 60, s_popUpEquipmentSetting, WM_CF_SHOW, 0, eCancelBtnStringId);
	WM_SetCallback(s_cancelBtn,equipmentSettingScreen_CallbackCancelBtn);

	//Title pop up
	s_titlePopUp = TEXT_CreateEx(0, 30, 700, 50,s_popUpEquipmentSetting,WM_CF_SHOW,TEXT_CF_RIGHT,eTitlePopUpEquipmentSettingStringID,"");
	TEXT_SetTextColor(s_titlePopUp, GUI_BLACK);
	TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
	guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);

	createContactDialog_Init();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentSettingScreen_ShowPatientHour										*/
/*                                                                                      */
/* Details  -show patients Cumulative operation Hour	            		    	    */
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
/*                     (O) s_valuePatientCumulative : display patient cumulative        */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentSettingScreen_ShowPatientHour(void)
{
	int8_t hourBuffer[SIZE_BUFFER_HOUR_EQUIP] = {'\0'};

	CumulativeHourTime time;
	time = guiInterface_GetCumulativeHour(patients);

	itoa(time.hour,(char*)hourBuffer,10);
	EDIT_SetText(s_valuePatientCumulative, (char*)hourBuffer);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentSettingScreen_Show		 								       	*/
/*                                                                                      */
/* Details  -Show equipment Setting screen with											*/
/* 				guidance volume setting													*/
/* 				alarm volume setting													*/
/* 				buzzer volume setting													*/
/* 				flowrate setting														*/
/* 				child lock setting														*/
/* 				create contact															*/
/* 				setting default button													*/
/*																						*/
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
/*                     (O) s_titleEquipSetting : title equipment setting                */
/*                     (O) s_valueEditContact : display contact number					*/
/*                     (O) s_popUpEquipmentSetting : popup of equipment setting			*/
/*                     (O) g_equipmentSettingScreen : equipment setting screen			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentSettingScreen_Show(void)
{
	alarmInfor_ShowAlarmInfor();

	int8_t buffer[SIZE_BUFFER_SETTING_EQUIP] = {'\0'};

	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		guiTaskTEXT_SetFont(s_titleEquipSetting, &GUI_FontEng3L16B2PP);
		TEXT_SetText(s_titleEquipSetting,"Equipment setting change");
	}
	else
	{
		guiTaskTEXT_SetFont(s_titleEquipSetting, &GUI_FontJapan3L18B2PP);
		TEXT_SetText(s_titleEquipSetting,"機器設定変更");
	}


	uint8_t voiceVolume = guiInterface_GetSettingValue(eGuidanceVolumeSettingId);
	itoa(voiceVolume,(char*)&buffer[0],10);
	settingButton_SetValue(eValueSettingGuidanceVolumeId,buffer);

	uint8_t alarmVolume = guiInterface_GetSettingValue(eAlarmVolumeSettingId);
	itoa(alarmVolume,(char*)&buffer[0],10);
	settingButton_SetValue(eValueSettingAlarmVolumeId,buffer);

	uint8_t buzzerVolume = guiInterface_GetSettingValue(eBuzzerVolumeSettingId);
	itoa(buzzerVolume,(char*)&buffer[0],10);
	settingButton_SetValue(eValueSettingBuzzerVolumeId,buffer);

	uint8_t restingFR = guiInterface_GetSettingValue(eRestingFRSettingId);
	double floatRestingFR = (double)(restingFR/20.0);
	sprintf((char*)buffer,"%0.2f",floatRestingFR);
	settingButton_SetValue(eValueSettingRestingFRId,buffer);

	uint8_t exertionFR = guiInterface_GetSettingValue(eExertionFRSettingId);
	double floatExertionFR = (double)(exertionFR/20.0);
	sprintf((char*)buffer,"%0.2f",floatExertionFR);
	settingButton_SetValue(eValueSettingExertionFRId,buffer);

	uint8_t bedtimeFR = guiInterface_GetSettingValue(eBedtimeFRSettingId);
	double floatBedtimeFR = (double)(bedtimeFR/20.0);
	sprintf((char*)buffer,"%0.2f",floatBedtimeFR);
	settingButton_SetValue(eValueSettingBedtimeFRId,buffer);

	uint8_t brightness = guiInterface_GetSettingValue(eBrightnessSettingId);
	itoa(brightness,(char*)&buffer[0],10);
	settingButton_SetValue(eValueSettingLCDBackLightId,buffer);

	uint8_t childlock = guiInterface_GetSettingValue(eChildLockSettingId);

	if(childlock == eOn)
	{
		settingOnOffButton_SetValue(eValueChildLockId, (int8_t *)"ON");
	}
	else
	{
		settingOnOffButton_SetValue(eValueChildLockId, (int8_t *)"OFF");
	}

	settingExistNoButton_SetValue();

	EDIT_SetText(s_valueEditContact, (char*)setting_GetContact());

	equipmentSettingScreen_ShowPatientHour();

	createContactDialog_Hide();
	WM_HideWindow(s_popUpEquipmentSetting);
	equipmentSettingScreen_ShowAll();

	WM_ShowWindow(g_equipmentSettingScreen);
	WM_BringToTop(g_equipmentSettingScreen);
	WM_SetFocus(g_equipmentSettingScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentSettingScreen_Hide		 								       	*/
/*                                                                                      */
/* Details  -Hide equipment Setting screen with											*/
/* 				guidance volume setting													*/
/* 				alarm volume setting													*/
/* 				buzzer volume setting													*/
/* 				flowrate setting														*/
/* 				child lock setting														*/
/* 				create contact															*/
/* 				setting default button													*/
/*																						*/
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
/*                     (O) g_equipmentSettingScreen : equipment setting screen      	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentSettingScreen_Hide(void)
{
	WM_HideWindow(g_equipmentSettingScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentSettingScreen_HideAll 			 								*/
/*                                                                                      */
/* Details  -Hide all child except create contact dialog								*/
/*																						*/
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
/*                     (O) s_titleEquipSetting : title equipment setting                */
/*                     (O) s_settingGuidanceVolume : guidance volume setting 		    */
/*                     (O) s_settingAlarmVolume : alarm volume setting 					*/
/*                     (O) s_settingBuzzerVolume : buzzer volume setting	           	*/
/*                     (O) s_settingRestingFR : resting flow rate setting				*/
/*                     (O) s_settingExertionFR : exertion flow rate setting             */
/*                     (O) s_settingBedtimeFR : bedtime flow rate setting				*/
/*                     (O) s_settingLCDBackLight : backlight setting                    */
/*                     (O) s_settingChildLock : child lock setting						*/
/*                     (O) s_valuePatientCumulative : display patient cumulative		*/
/*                     (O) s_settingOxygenCylinder : oxygen cylinder setting            */
/*                     (O) s_valueEditContact : display contact number                  */
/*                     (O) s_pageText : page number                                     */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button                                      */
/*                     (O) s_nextBtn : next button						                */
/*                     (O) s_clearBtn : clear button						            */
/*                     (O) s_createBtn : create contact number button					*/
/*                     (O) s_EquipmentSettingInitializationBtn : initialization button	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentSettingScreen_HideAll(void)
{
	WM_HideWindow(s_titleEquipSetting);
	WM_HideWindow(s_settingGuidanceVolume);
	WM_HideWindow(s_settingAlarmVolume);
	WM_HideWindow(s_settingBuzzerVolume);
	WM_HideWindow(s_settingRestingFR);
	WM_HideWindow(s_settingExertionFR);
	WM_HideWindow(s_settingBedtimeFR);
	WM_HideWindow(s_settingLCDBackLight);
	WM_HideWindow(s_settingChildLock);
	WM_HideWindow(s_valuePatientCumulative);
	WM_HideWindow(s_settingOxygenCylinder);
	WM_HideWindow(s_valueEditContact);
	WM_HideWindow(s_pageText);

	WM_HideWindow(s_backBtn);
	WM_HideWindow(s_homeBtn);
	WM_HideWindow(s_nextBtn);
	WM_HideWindow(s_clearBtn);
	WM_HideWindow(s_createBtn);
	WM_HideWindow(s_EquipmentSettingInitializationBtn);
	alarmInfor_HideAlarmInfor();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentSettingScreen_ShowAll			 								    */
/*                                                                                      */
/* Details  -Show all child except create contact dialog								*/
/*																						*/
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
/*                     (O) s_titleEquipSetting : title equipment setting                */
/*                     (O) s_settingGuidanceVolume : guidance volume setting 		    */
/*                     (O) s_settingAlarmVolume : alarm volume setting 					*/
/*                     (O) s_settingBuzzerVolume : buzzer volume setting	           	*/
/*                     (O) s_settingRestingFR : resting flow rate setting				*/
/*                     (O) s_settingExertionFR : exertion flow rate setting             */
/*                     (O) s_settingBedtimeFR : bedtime flow rate setting				*/
/*                     (O) s_settingLCDBackLight : backlight setting                    */
/*                     (O) s_settingChildLock : child lock setting						*/
/*                     (O) s_valuePatientCumulative : display patient cumulative		*/
/*                     (O) s_settingOxygenCylinder : oxygen cylinder setting            */
/*                     (O) s_valueEditContact : display contact number                  */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button                                      */
/*                     (O) s_nextBtn : next button						                */
/*                     (O) s_clearBtn : clear button						            */
/*                     (O) s_createBtn : create contact number button					*/
/*                     (O) s_EquipmentSettingInitializationBtn : initialization button	*/
/*                     (O) s_pageText : page number                                     */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentSettingScreen_ShowAll(void)
{
	WM_ShowWindow(s_titleEquipSetting);
	WM_ShowWindow(s_settingGuidanceVolume);
	WM_ShowWindow(s_settingAlarmVolume);
	WM_ShowWindow(s_settingBuzzerVolume);
	WM_ShowWindow(s_settingRestingFR);
	WM_ShowWindow(s_settingExertionFR);
	WM_ShowWindow(s_settingBedtimeFR);
	WM_ShowWindow(s_settingLCDBackLight);
	WM_ShowWindow(s_settingChildLock);
	WM_ShowWindow(s_valuePatientCumulative);
	WM_ShowWindow(s_settingOxygenCylinder);

	EDIT_SetText(s_valueEditContact, (char*)setting_GetContact());
	WM_ShowWindow(s_valueEditContact);

	WM_ShowWindow(s_backBtn);
	WM_ShowWindow(s_homeBtn);
	WM_ShowWindow(s_nextBtn);
	WM_ShowWindow(s_clearBtn);
	WM_ShowWindow(s_createBtn);
	WM_ShowWindow(s_EquipmentSettingInitializationBtn);
	WM_ShowWindow(s_pageText);
	alarmInfor_ShowAlarmInfor();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentSettingScreen_SetDefaut			 								*/
/*                                                                                      */
/* Details  -Set default setting														*/
/*																						*/
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
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentSettingScreen_SetDefaut(void)
{
	int8_t buffer[SIZE_BUFFER_SETTING_EQUIP] = {'\0'};

	uint8_t voiceVolume = guiInterface_GetSettingValue(eGuidanceVolumeSettingId);
	itoa(voiceVolume,(char*)&buffer[0],10);
	settingButton_SetValue(eValueSettingGuidanceVolumeId,buffer);

	uint8_t alarmVolume = guiInterface_GetSettingValue(eAlarmVolumeSettingId);
	itoa(alarmVolume,(char*)&buffer[0],10);
	settingButton_SetValue(eValueSettingAlarmVolumeId,buffer);

	uint8_t buzzerVolume = guiInterface_GetSettingValue(eBuzzerVolumeSettingId);
	itoa(buzzerVolume,(char*)&buffer[0],10);
	settingButton_SetValue(eValueSettingBuzzerVolumeId,buffer);

	uint8_t restingFR = guiInterface_GetSettingValue(eRestingFRSettingId);
	double floatRestingFR = (double)(restingFR/20.0);
	sprintf((char*)buffer,"%0.2f",floatRestingFR);
	settingButton_SetValue(eValueSettingRestingFRId,buffer);

	uint8_t exertionFR = guiInterface_GetSettingValue(eExertionFRSettingId);
	double floatExertionFR = (double)(exertionFR/20.0);
	sprintf((char*)buffer,"%0.2f",floatExertionFR);
	settingButton_SetValue(eValueSettingExertionFRId,buffer);

	uint8_t bedtimeFR = guiInterface_GetSettingValue(eBedtimeFRSettingId);
	double floatBedtimeFR = (double)(bedtimeFR/20.0);
	sprintf((char*)buffer,"%0.2f",floatBedtimeFR);
	settingButton_SetValue(eValueSettingBedtimeFRId,buffer);

	uint8_t brightness = guiInterface_GetSettingValue(eBrightnessSettingId);
	itoa(brightness,(char*)&buffer[0],10);
	settingButton_SetValue(eValueSettingLCDBackLightId,buffer);

	uint8_t childlock = guiInterface_GetSettingValue(eChildLockSettingId);

	if(childlock == eOn)
	{
		settingOnOffButton_SetValue(eValueChildLockId, (int8_t *)"ON");
	}
	else
	{
		settingOnOffButton_SetValue(eValueChildLockId, (int8_t *)"OFF");
	}

	settingExistNoButton_SetValue();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentSettingScreen_UpdateSetting			 							*/
/*                                                                                      */
/* Details  -Update monitor in service mode						 						*/
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
/* Using Global Data : (I) g_equipmentSettingScreen : equipment setting screen			*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentSettingScreen_UpdateSetting(void)
{
	if(WM_IsVisible(g_equipmentSettingScreen))
	{
		settingExistNoButton_SetValue();
	}

	return;
}

/* end of files */
