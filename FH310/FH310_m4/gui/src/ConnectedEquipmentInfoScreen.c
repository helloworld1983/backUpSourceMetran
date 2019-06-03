/********************************************************************/
/*                                                                  */
/* File Name    : ConnectedEquipmentInfoScreen.c                    */
/*                                                                  */
/* General       : Displaying data in connected 					*/
/*                 equipment information				            */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2008/08/22      Viet Le  	        new file        */
/* #0001       2017/08/30	   Viet Le				modify			*/
/*  Fix : coding is accepted on langage C  							*/
/*     (connectedEquipmentInforScreen_Append)                       */
/* #0002       2017/09/19      Viet Le				modify			*/
/*     Edit int8_t-> uint8_t in 									*/
/*		connectedEquipmentInforScreen_DisplayBTAddress() function 	*/
/*		connectedEquipmentInforScreen_Append() function				*/
/*		(bug 1860)													*/
/*  #0003	   2017/10/27      Viet Le 	    	        modify	    */
/*	  Edit : EDIT_SetFont()->guiTaskEDIT_SetFont; TEXT_SetFont()->  */
/*		guiTaskTEXT_SetFont() (bug 1997)							*/
/********************************************************************/

#include "string.h"
#include "stdlib.h"
#include <EEPROM.h>
#include "ConnectedEquipmentInfoScreen.h"
#include "MainScreen.h"
#include "WM.h"
#include "StringTable.h"
#include "EDIT.h"
#include "DataDisplay.h"
#include "BUTTON.h"
#include "board.h"
#include "setting.h"
#include "GuiTask.h"
#include "SettingOnOffButton.h"
#include "CompositionMgr.h"
#include "Font.h"
#include "AlarmInfor.h"
#include "ServiceScreen.h"
#include "EquipmentScreen.h"
#include "ServiceOptionScreen.h"
#include "OptionScreen.h"
#include "EquipmentInforScreen.h"
#include "EquipmentCumulativeHourScreen.h"
#include "EquipmentSettingScreen.h"
#include "BluetoothTask.h"
#include "TextLibrary.h"
#include "GuiInterface.h"
#include "LogInterface.h"
#include "AnypalInterface.h"

//Define item id
enum{
	eOkBtnStringConnectedSettingId,
	eCancelBtnStringConnectedSettingId,
	ePageTextConnectedEquipmentSettingId,
	eNexBtnConnectedInforStringId,
	eBluetoothAddressStringId
};


WM_HWIN g_connectedEquipmentInforScreen;

static WM_HWIN s_popUpConnectedEquipmentSetting;
static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static BUTTON_Handle s_prevBtn;
static BUTTON_Handle s_deleteExternalRegisterBtn;
static BUTTON_Handle s_OKBtn;
static BUTTON_Handle s_cancelBtn;
static BUTTON_Handle s_nextBtn;
static EDIT_Handle s_displayEquipmentName;
static EDIT_Handle s_displayCommodityCode;
static EDIT_Handle s_displayVersion;
static EDIT_Handle s_displayIDSerialNumber;
static EDIT_Handle s_settingBluetoothOnOffBtn;
static EDIT_Handle s_settingBluetoothTransferBtn;
static EDIT_Handle s_displayBTAddress;
static TEXT_Handle s_pageText;
static bool s_isDeleteEx = false;

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
/* General 	-connectedEquipmentInforScreen_CallbackPrevBtn 								*/
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
static void connectedEquipmentInforScreen_CallbackPrevBtn(WM_MESSAGE * pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_FillPolygon(&s_aPointPrevArrow[0],7,Rect.x0,Rect.y0);

		GUI_SetPenSize(2);
		GUI_SetColor(BORDER_COLOR);
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
			connectedEquipmentInforScreen_Hide();
			if(serviceOptionScreen_GetFlagEquipInfor() == true)
			{
				equipmentInforScreen_Show();
			}
			else
			{
				equipmentSettingScreen_Show();
			}
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
/* General 	-connectedEquipmentInforScreen_CallbackBackBtn 								*/
/*                                                                                      */
/* Details  -Call back function for back button, handle event from 					    */
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
static void connectedEquipmentInforScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			settingOnOffButton_SetValue(eValueBluetoothId, (const int8_t *)"OFF");
			BTTask_SendEvent(eOffBluetoothId);

			connectedEquipmentInforScreen_Hide();
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
/* General 	-connectedEquipmentInforScreen_CallbackHomeBtn 								*/
/*                                                                                      */
/* Details  -Call back function for home button, handle event from 					    */
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
static void connectedEquipmentInforScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			settingOnOffButton_SetValue(eValueBluetoothId, (const int8_t *)"OFF");
			BTTask_SendEvent(eOffBluetoothId);

			connectedEquipmentInforScreen_Hide();
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
/* General 	-connectedEquipmentInforScreen_Callback 								    */
/*                                                                                      */
/* Details  -Call back function for connected equipment infor, handle event from 	    */
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
void connectedEquipmentInforScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		if(s_isDeleteEx == false)
		{
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				GUI_SetColor(GUI_BLACK);
				guiTask_SetFont(&GUI_FontEng3L16B2PP);
				if(serviceOptionScreen_GetFlagEquipInfor() == true)
				{
					GUI_DispStringHCenterAt("Equipment information", 400,5);
				}
				else
				{
					GUI_DispStringHCenterAt("Equipment setting", 400,5);
				}

				guiTask_SetFont(&GUI_FontEng3L12B2PP);
				GUI_AA_DrawRoundedRect(10,60,790,265,10);
				GUI_DispStringAt("Connected equipment information",30,50);
				GUI_DispStringAt("Connected equipment 1",320,80);

				GUI_SetTextAlign(GUI_TA_RIGHT);
				GUI_DispStringAt("Equipment Name:",300, 117);

				GUI_SetTextAlign(GUI_TA_RIGHT);
				GUI_DispStringAt("Commodity code:",300, 157);

				GUI_SetTextAlign(GUI_TA_RIGHT);
				GUI_DispStringAt("Version:",300, 197);

				GUI_SetTextAlign(GUI_TA_RIGHT);
				GUI_DispStringAt("ID (Serial) number:",300, 237);

				GUI_SetTextAlign(GUI_TA_RIGHT);
				GUI_DispStringAt("Bluetooth address:",350, 282);
			}
			else
			{
				GUI_SetColor(GUI_BLACK);
				guiTask_SetFont(&GUI_FontJapan3L18B2PP);

				if(serviceOptionScreen_GetFlagEquipInfor() == true)
				{
					GUI_DispStringHCenterAt("機器情報表示", 400,5);
				}
				else
				{
					GUI_DispStringHCenterAt("機器設定変更", 400,5);
				}

				guiTask_SetFont(&GUI_FontJapan3L16B2PP);
				GUI_AA_DrawRoundedRect(10,60,790,265,10);
				GUI_DispStringAt("接続機器情報",30,50);
				GUI_DispStringAt("接続機器1",320,80);

				GUI_SetTextAlign(GUI_TA_RIGHT);
				GUI_DispStringAt("機器名称:",300, 117);

				GUI_SetTextAlign(GUI_TA_RIGHT);
				GUI_DispStringAt("品目コード:",300, 157);

				GUI_SetTextAlign(GUI_TA_RIGHT);
				GUI_DispStringAt("バージョン:",300, 197);

				GUI_SetTextAlign(GUI_TA_RIGHT);
				GUI_DispStringAt("ID(シリアル)番号:",300, 237);

				GUI_SetTextAlign(GUI_TA_RIGHT);
				GUI_DispStringAt("Bluetooth アドレス:",350, 282);
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
/* General 	-connectedEquipmentInforScreen_CallbackPopUp 								*/
/*                                                                                      */
/* Details  -Call back function for pop up									 			*/
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
void connectedEquipmentInforScreen_CallbackPopUp(WM_MESSAGE *pMsg)
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

		GUI_SetBkColor(DIALOG_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Remove the connected equipment\nAre you sure?", 350,20);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("接続機器を削除します\nよろしいですか?", 350,20);
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
/* General 	-connectedEquipmentInforScreen_CallbackDeleteExternalBtn 					*/
/*                                                                                      */
/* Details  -Call back function for delete external button handle event from 			*/
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
/*                     (O) s_popUpConnectedEquipmentSetting : pop up of connected screen*/
/*                     (O) g_connectedEquipmentInforScreen : connected external screen	*/
/*                     (O) s_isDeleteEx : flag delete external communication			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void connectedEquipmentInforScreen_CallbackDeleteExternalBtn(WM_MESSAGE *pMsg)
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

		GUI_SetTextMode(GUI_TM_TRANS);
		GUI_SetColor(GUI_BLACK);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringHCenterAt("Delete external", Rect.x1/2, 12+3+2);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L14B2PP);
			GUI_DispStringHCenterAt("接続機器の削除", Rect.x1/2, 12+3);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_isDeleteEx = true;
			connectedEquipmentInforScreen_HideAllItem();

			WM_ShowWindow(s_popUpConnectedEquipmentSetting);
			WM_BringToTop(s_popUpConnectedEquipmentSetting);

			WM_Paint(g_connectedEquipmentInforScreen);
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
/* General 	-connectedEquipmentInforScreen_CallbackOKBtn 								*/
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
/*                     (O) s_displayEquipmentName : display EQ name                     */
/*                     (O) s_displayCommodityCode : display commodity code				*/
/*                     (O) s_displayVersion : display version external communication	*/
/*                     (O) s_displayIDSerialNumber : display ID serial number			*/
/*                     (O) s_popUpConnectedEquipmentSetting : pop up of connected screen*/
/*                     (O) s_isDeleteEx : flag delete external communication			*/
/*                     (O) g_connectedEquipmentInforScreen : connected external screen	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void connectedEquipmentInforScreen_CallbackOKBtn(WM_MESSAGE *pMsg)
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
			guiInterface_SetAnypalInfo((int8_t*)"",(int8_t*)"",(int8_t*)"",(int8_t*)"");

			EDIT_SetText(s_displayEquipmentName, "");
			EDIT_SetText(s_displayCommodityCode, "");
			EDIT_SetText(s_displayVersion, "");
			EDIT_SetText(s_displayIDSerialNumber, "");
			logInterface_DeleteLog(eDeleteAnypalLogId);
			logInterface_DeleteLog(eDeleteServerLogId);
			logInterface_DeleteLog(eDeleteUsageLogId);
			anypal_ResetAllAnypalState(eResetAllAnypalState);
			guiInterface_SetNumberOfLogStored(0);
			BTTask_SendEvent(eDeleteCompositionId);
			setting_SaveAnypalInfo();
			setting_RecordChecksum();
			WM_HideWindow(s_popUpConnectedEquipmentSetting);

			connectedEquipmentInforScreen_ShowAllItem();

			s_isDeleteEx = false;

			WM_Paint(g_connectedEquipmentInforScreen);
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
/* General 	-connectedEquipmentInforScreen_CallbackCancelBtn 							*/
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
/*                     (O) s_isDeleteEx : flag delete external communication            */
/*                     (O) s_popUpConnectedEquipmentSetting : pop up of connected screen*/
/*                     (O) g_connectedEquipmentInforScreen : connected external screen  */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void connectedEquipmentInforScreen_CallbackCancelBtn(WM_MESSAGE *pMsg)
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
			WM_HideWindow(s_popUpConnectedEquipmentSetting);
			connectedEquipmentInforScreen_ShowAllItem();

			s_isDeleteEx = false;

			WM_Paint(g_connectedEquipmentInforScreen);
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
/* General 	-connectedEquipmentInforScreen_CallbackNextBtn 								*/
/*                                                                                      */
/* Details  -Call back function for next button, handle event from 			            */
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
void connectedEquipmentInforScreen_CallbackNextBtn(WM_MESSAGE *pMsg)
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
			connectedEquipmentInforScreen_Hide();
			equipmentCumulativeHourScreen_Show();
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
/* General 	-connectedEquipmentInforScreen_Init 										*/
/*                                                                                      */
/* Details  -Initialize connected equipment information screen 			            	*/
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
/*                     (O) g_connectedEquipmentInforScreen : connected external screen  */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_prevBtn : previous button						            */
/*                     (O) s_displayEquipmentName : display EQ name						*/
/*                     (O) s_displayCommodityCode : display commodity code				*/
/*                     (O) s_displayVersion : display version external					*/
/*                     (O) s_displayIDSerialNumber : display ID serial number			*/
/*                     (O) s_displayBTAddress : display bluetooth address				*/
/*                     (O) s_settingBluetoothOnOffBtn : ON/OFF bluetooth button			*/
/*                     (O) s_settingBluetoothTransferBtn : ON/OFF blue transfer button	*/
/*                     (O) s_deleteExternalRegisterBtn : delete external button			*/
/*                     (O) s_pageText : display number page								*/
/*                     (O) s_popUpConnectedEquipmentSetting : pop up connected screen	*/
/*                     (O) s_OKBtn : OK button											*/
/*                     (O) s_cancelBtn : cancel button									*/
/*                     (O) s_nextBtn : next button										*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void connectedEquipmentInforScreen_Init(void)
{
	GUI_RECT Rect;

	g_connectedEquipmentInforScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_equipmentScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,connectedEquipmentInforScreen_Callback,0);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_connectedEquipmentInforScreen, WM_CF_SHOW, 0, eBackBtnEquipmentInforExtStringId);
	WM_SetCallback(s_backBtn,connectedEquipmentInforScreen_CallbackBackBtn);

	//Home button
	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_connectedEquipmentInforScreen, WM_CF_SHOW, 0, eHomeBtnEquipmentInforExtStringId);
	WM_SetCallback(s_homeBtn,connectedEquipmentInforScreen_CallbackHomeBtn);

	//Previous button
	s_prevBtn = BUTTON_CreateEx(0, 415, 102, 60, g_connectedEquipmentInforScreen, WM_CF_SHOW, 0, ePrevBtnEquipmentInforExtStringId);
	WM_SetCallback(s_prevBtn,connectedEquipmentInforScreen_CallbackPrevBtn);

	s_displayEquipmentName = EDIT_CreateEx(300,110,320,30,g_connectedEquipmentInforScreen,WM_CF_SHOW,0,eEquipmentConnectedNameStringId,50);
	EDIT_SetFocussable(s_displayEquipmentName,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_displayEquipmentName ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_displayEquipmentName ,GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_displayEquipmentName ,"ATP-02");

	s_displayCommodityCode = EDIT_CreateEx(300,150,320,30,g_connectedEquipmentInforScreen,WM_CF_SHOW,0,eEquipmentConnectedCommodityCodeStringId,50);
	EDIT_SetFocussable(s_displayCommodityCode,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_displayCommodityCode ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_displayCommodityCode ,GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_displayCommodityCode ,"123456789");

	s_displayVersion = EDIT_CreateEx(300,190,320,30,g_connectedEquipmentInforScreen,WM_CF_SHOW,0,eEquipmentConnectedVersionStringId,50);
	EDIT_SetFocussable(s_displayVersion,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_displayVersion ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_displayVersion ,GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_displayVersion ,"V01-01 001 2016/08/24");

	s_displayIDSerialNumber = EDIT_CreateEx(300,230,320,30,g_connectedEquipmentInforScreen,WM_CF_SHOW,0,eEquipmentConnectedIDSerialNumberStringId,50);
	EDIT_SetFocussable(s_displayIDSerialNumber,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_displayIDSerialNumber ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_displayIDSerialNumber ,GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_displayIDSerialNumber ,"0123456789");

	s_displayBTAddress = EDIT_CreateEx(350,275,320,30,g_connectedEquipmentInforScreen,WM_CF_SHOW,0,eBluetoothAddressStringId,50);
	EDIT_SetFocussable(s_displayBTAddress,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_displayBTAddress ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_displayBTAddress ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	//Bluetooth
	s_settingBluetoothOnOffBtn = EDIT_CreateEx(580,335 - 25 + 10,245,70,g_connectedEquipmentInforScreen,WM_CF_SHOW,0,eTurnBluetoothStringID,50);
	WM_GetClientRectEx(s_settingBluetoothOnOffBtn,&Rect);
	settingOnOffLargeButton_Init(s_settingBluetoothOnOffBtn,eOnBtnBluetoothStringId,eOffBtnBluetoothStringId,
			eValueBluetoothId,eValueOfBluetoothStringId,Rect);
	WM_SetCallback(s_settingBluetoothOnOffBtn,settingOnOffButton_Callback);

	//Bluetooth tranfer
	s_settingBluetoothTransferBtn = EDIT_CreateEx(315,335 - 25 + 10,245,70,g_connectedEquipmentInforScreen,WM_CF_SHOW,0,eTurnBluetoothTranferStringID,50);
	WM_GetClientRectEx(s_settingBluetoothTransferBtn,&Rect);
	settingOnOffLargeButton_Init(s_settingBluetoothTransferBtn,eOnBtnBluetoothTransferStringId,eOffBtnBluetoothTransferStringId,
			eValueBluetoothTransferId,eValueOfBluetoothTransferStringId,Rect);
	WM_SetCallback(s_settingBluetoothTransferBtn,settingOnOffButton_Callback);

	//Delete button
	s_deleteExternalRegisterBtn = BUTTON_CreateEx(25, 345, 210, 50, g_connectedEquipmentInforScreen, WM_CF_SHOW, 0, eDeleteExternalRegisterStringId);
	WM_SetCallback(s_deleteExternalRegisterBtn,connectedEquipmentInforScreen_CallbackDeleteExternalBtn);

	s_pageText = TEXT_CreateEx(380,440, 50, 25,g_connectedEquipmentInforScreen,WM_CF_SHOW,TEXT_CF_LEFT,ePageTextConnectedEquipmentSettingId,"2/2");
	guiTaskTEXT_SetFont(s_pageText, &GUI_FontEng3L18B2PP);

	s_popUpConnectedEquipmentSetting = WM_CreateWindowAsChild(50, 115, 700, 220,g_connectedEquipmentInforScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,connectedEquipmentInforScreen_CallbackPopUp,0);

	//OK button
	s_OKBtn = BUTTON_CreateEx(50, 120, 280, 60, s_popUpConnectedEquipmentSetting, WM_CF_SHOW, 0, eOkBtnStringConnectedSettingId);
	WM_SetCallback(s_OKBtn,connectedEquipmentInforScreen_CallbackOKBtn);

	//Cancel button
	s_cancelBtn = BUTTON_CreateEx(380, 120, 280, 60, s_popUpConnectedEquipmentSetting, WM_CF_SHOW, 0, eCancelBtnStringConnectedSettingId);
	WM_SetCallback(s_cancelBtn,connectedEquipmentInforScreen_CallbackCancelBtn);

	//Next button
	s_nextBtn = BUTTON_CreateEx(700, 415, 100, 60, g_connectedEquipmentInforScreen, WM_CF_HIDE, 0, eNexBtnConnectedInforStringId);
	WM_SetCallback(s_nextBtn,connectedEquipmentInforScreen_CallbackNextBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-connectedEquipmentInforScreen_Show 							    		*/
/*                                                                                      */
/* Details  -Show connected equipment information screen 			            	    */
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
/*                     (O) s_isDeleteEx : flag delete external communication            */
/*                     (O) s_popUpConnectedEquipmentSetting : pop up of connected screen*/
/*                     (O) g_connectedEquipmentInforScreen : connected EQ screen		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void connectedEquipmentInforScreen_Show(void)
{
	alarmInfor_ShowAlarmInfor();

	connectedEquipmentInforScreen_ShowAllItem();

	if(serviceOptionScreen_GetFlagEquipInfor() == true)
	{
		connectedEquipmentInforScreen_ShowItemInConnectInfor();
	}
	else
	{
		connectedEquipmentInforScreen_ShowItemInConnectSetting();
	}

	s_isDeleteEx = false;
	WM_HideWindow(s_popUpConnectedEquipmentSetting);

	WM_ShowWindow(g_connectedEquipmentInforScreen);
	WM_BringToTop(g_connectedEquipmentInforScreen);
	WM_SetFocus(g_connectedEquipmentInforScreen);

	connectedEquipmentInforScreen_UpdateAnypal();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-connectedEquipmentInforScreen_Hide 							    		*/
/*                                                                                      */
/* Details  -Hide connected equipment information screen 			            	    */
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
/*                     (O) g_connectedEquipmentInforScreen : connected EQ screen        */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void connectedEquipmentInforScreen_Hide(void)
{
	WM_HideWindow(g_connectedEquipmentInforScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-connectedEquipmentInforScreen_UpdateAnypal				 					*/
/*                                                                                      */
/* Details  -Update infor anypal						 								*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_connectedEquipmentInforScreen : connected EQ screen		*/
/*                     (O) s_displayEquipmentName : display EQ name                     */
/*                     (O) s_displayCommodityCode : display commodity code				*/
/*                     (O) s_displayVersion : display version external					*/
/*                     (O) s_displayIDSerialNumber : display ID serial number			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void connectedEquipmentInforScreen_UpdateAnypal(void)
{
	if(WM_IsVisible(g_connectedEquipmentInforScreen))
	{
		int8_t version[SIZE_BUFFER_VERSION_ANYPAL] = {'\0'};
		AnypalInfor anypalInfo = guiInterface_GetAnypalInfo();
		if(anypalInfo.version[0] != '\0')
		{
			strcat((char*)version, "V");
			version[1] = anypalInfo.version[0];
			version[2] = anypalInfo.version[1];
			strcat((char*)version, "-");
			version[4] = anypalInfo.version[2];
			version[5] = anypalInfo.version[3];
			strcat((char*)version, " ");
			version[7] = anypalInfo.version[4];
			version[8] = anypalInfo.version[5];
			version[9] = anypalInfo.version[6];
			strcat((char*)version, " ");
			version[11] = anypalInfo.version[7];
			version[12] = anypalInfo.version[8];
			version[13] = anypalInfo.version[9];
			version[14] = anypalInfo.version[10];
			strcat((char*)version, "/");
			version[16] = anypalInfo.version[11];
			version[17] = anypalInfo.version[12];
			strcat((char*)version, "/");
			version[19] = anypalInfo.version[13];
			version[20] = anypalInfo.version[14];
		}

		EDIT_SetText(s_displayEquipmentName,(char*)anypalInfo.equipmentName);
		EDIT_SetText(s_displayCommodityCode,(char*)anypalInfo.commodityCode);
		EDIT_SetText(s_displayVersion ,(char*)version);
		int8_t numberID[NUMBER_ID_SIZE + 1]={'\0'};
		memcpy((char*)numberID,anypalInfo.numberID,NUMBER_ID_SIZE);
		EDIT_SetText(s_displayIDSerialNumber,(char*)anypalInfo.numberID);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-connectedEquipmentInforScreen_ShowItemInConnectInfor 						*/
/*                                                                                      */
/* Details  -Show item in connected equipment information screen			            */
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
/*                     (O) s_deleteExternalRegisterBtn : delete external button         */
/*                     (O) s_settingBluetoothOnOffBtn : ON/OFF bluetooth button			*/
/*                     (O) s_settingBluetoothTransferBtn : ON/OFF blue transfer button	*/
/*                     (O) s_popUpConnectedEquipmentSetting : pop up connected screen	*/
/*                     (O) s_nextBtn : next button										*/
/*                     (O) s_pageText : page number										*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void connectedEquipmentInforScreen_ShowItemInConnectInfor(void)
{
	WM_HideWindow(s_deleteExternalRegisterBtn);
	WM_HideWindow(s_settingBluetoothOnOffBtn);
	WM_HideWindow(s_settingBluetoothTransferBtn);
	WM_HideWindow(s_popUpConnectedEquipmentSetting);
	WM_HideWindow(s_settingBluetoothTransferBtn);

	WM_ShowWindow(s_nextBtn);

	TEXT_SetText(s_pageText,"2/3");

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-connectedEquipmentInforScreen_ShowItemInConnectSetting 					*/
/*                                                                                      */
/* Details  -Show item in connected equipment setting screen				            */
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
/*                     (O) s_nextBtn : next button                                      */
/*                     (O) s_deleteExternalRegisterBtn : delete external button			*/
/*                     (O) s_settingBluetoothOnOffBtn : ON/OFF bluetooth button			*/
/*                     (O) s_settingBluetoothTransferBtn : ON/OFF blue transfer button  */
/*                     (O) s_pageText : page number  									*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void connectedEquipmentInforScreen_ShowItemInConnectSetting(void)
{
	WM_HideWindow(s_nextBtn);

	WM_ShowWindow(s_deleteExternalRegisterBtn);
	WM_ShowWindow(s_settingBluetoothOnOffBtn);
	WM_ShowWindow(s_settingBluetoothTransferBtn);

	TEXT_SetText(s_pageText,"2/2");

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-connectedEquipmentInforScreen_HideAllItem 									*/
/*                                                                                      */
/* Details  -Hide all item in connected equipment setting screen				        */
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
/*                     (O) s_pageText : page number                                     */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_deleteExternalRegisterBtn : delete external button			*/
/*                     (O) s_prevBtn : previous button									*/
/*                     (O) s_displayEquipmentName : display EQ name					    */
/*                     (O) s_displayCommodityCode : display commodity code				*/
/*                     (O) s_displayVersion : display version of external				*/
/*                     (O) s_displayIDSerialNumber : display ID serial number			*/
/*                     (O) s_settingBluetoothOnOffBtn : ON/OFF buletooth button			*/
/*                     (O) s_settingBluetoothTransferBtn : ON/OFF bule transfer button	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void connectedEquipmentInforScreen_HideAllItem(void)
{
	WM_HideWindow(s_pageText);
	WM_HideWindow(s_backBtn);
	WM_HideWindow(s_homeBtn);
	WM_HideWindow(s_deleteExternalRegisterBtn);
	WM_HideWindow(s_prevBtn);
	WM_HideWindow(s_displayEquipmentName);
	WM_HideWindow(s_displayCommodityCode);
	WM_HideWindow(s_displayVersion);
	WM_HideWindow(s_displayIDSerialNumber);
	WM_HideWindow(s_settingBluetoothOnOffBtn);
	WM_HideWindow(s_settingBluetoothTransferBtn);
	alarmInfor_HideAlarmInfor();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-connectedEquipmentInforScreen_ShowAllItem 									*/
/*                                                                                      */
/* Details  -Show all item in connected equipment setting screen				        */
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
/*                     (O) s_pageText : page number                                     */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_deleteExternalRegisterBtn : delete external button			*/
/*                     (O) s_prevBtn : previous button									*/
/*                     (O) s_displayEquipmentName : display EQ name					    */
/*                     (O) s_displayCommodityCode : display commodity code				*/
/*                     (O) s_displayVersion : display version of external				*/
/*                     (O) s_displayIDSerialNumber : display ID serial number			*/
/*                     (O) s_settingBluetoothOnOffBtn : ON/OFF buletooth button			*/
/*                     (O) s_settingBluetoothTransferBtn : ON/OFF bule transfer button	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void connectedEquipmentInforScreen_ShowAllItem(void)
{
	WM_ShowWindow(s_pageText);
	WM_ShowWindow(s_backBtn);
	WM_ShowWindow(s_homeBtn);
	WM_ShowWindow(s_deleteExternalRegisterBtn);
	WM_ShowWindow(s_prevBtn);
	WM_ShowWindow(s_displayEquipmentName);
	WM_ShowWindow(s_displayCommodityCode);
	WM_ShowWindow(s_displayVersion);
	WM_ShowWindow(s_displayIDSerialNumber);
	WM_ShowWindow(s_settingBluetoothOnOffBtn);
	WM_ShowWindow(s_settingBluetoothTransferBtn);
	alarmInfor_ShowAlarmInfor();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-connectedEquipmentInforScreen_Append 										*/
/*                                                                                      */
/* Details  -inserts into subject[] at position pos				        				*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t subject[] : subject	insert								    */
/*				   const uint8_t insert[] : char insert									*/
/*                 int pos : position insert                                            */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void connectedEquipmentInforScreen_Append(uint8_t subject[], const uint8_t insert[], int pos)
{
	uint8_t buf[SIZE_BUFFER_INSERT_STR] = {0};

    strncpy((char*)buf, (char*)subject, pos); // copy at most first pos characters
    int len = strlen((char*)buf);
    strncpy((char*)(buf+len), (char*)insert,strlen((char*)insert)); // copy all of insert[] at the end
    len += strlen((char*)insert);  // increase the length by length of insert[]
    strncpy((char*)(buf+len), (char*)(subject+pos),strlen((char*)(subject+pos))); // copy the rest

    strncpy((char*)subject, (char*)buf,strlen((char*)buf));   // copy it back to subject

    return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-connectedEquipmentInforScreen_DisplayBTAddress 							*/
/*                                                                                      */
/* Details  -Display bluetooth address											        */
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t* btAddr : bluetooth address									*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_displayBTAddress : display bluetooth address               */
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void connectedEquipmentInforScreen_DisplayBTAddress(uint8_t* btAddr)
{
	uint8_t buffer[SIZE_BUFFER_BT_ADDR] = {'\0'};

	for(int i = 0; i < strlen((char*)btAddr); i++)
	{
		buffer[i] = btAddr[i];
	}


	int  j = 0;
	while(j < strlen((char*)buffer))
	{
		j++;

		if((j >= DIV_3) && (j % DIV_3 == 0))
		{
			connectedEquipmentInforScreen_Append(buffer,(const uint8_t*)":",j-1);
		}
	}

	EDIT_SetText(s_displayBTAddress,(char*)buffer);

	return;
}

/* end of files */
