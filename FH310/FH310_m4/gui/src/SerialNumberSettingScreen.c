/********************************************************************/
/*                                                                  */
/* File Name    : SerialNumberSettingScreen.c                       */
/*                                                                  */
/* General      : The SerialNumberSettingScreen category provides   */
/* 				  the user an interface to display and              */
/*                change serial number setting in system            */
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
/*  #0001	   2017/09/07      Linh Nguyen(MV)  	    modify	    */
/*    use the new get serial number function design					*/
/*  #0002	   2017/10/06      Viet Le  	    	modify	        */
/*	  Edit save serial number when press decision button (bug 1954)	*/
/*  #0003       2017/10/27	   Viet Le				modify			*/
/*	-Edit function : EDIT_SetFont()->guiTaskEDIT_SetFont()(bug 1997)*/
/*  #0004       2017/11/17	   Viet Le				modify			*/
/*	-Remove s_lengSerial (coverity)									*/
/********************************************************************/

#include "string.h"
#include "stdlib.h"
#include <EEPROM.h>
#include "SerialNumberSettingScreen.h"
#include "EDIT.h"
#include "BUTTON.h"
#include "MainScreen.h"
#include "GuiTask.h"
#include "setting.h"
#include "Font.h"
#include "TEXT.h"
#include "AlarmInfor.h"
#include "MaintenanceScreen.h"
#include "OptionScreen.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

//Define item id
enum{
	eCurrentSerialNumberId,
	eChangeSerialNumberBtnId,
	eBackBtnSerialNumberId,
	eHomeBtnSerialNumberId,
	eEditFieldSerialNumberId,
	eCloseDialogSerialId,
	eClearDialogSerialId,
	eDecisionDialogSerialId,
	eEditFieldEightSerialNumberId
};

WM_HWIN g_serialNumberSettingScreen;

static WM_HWIN s_popUpSerialSetting;
static EDIT_Handle s_currentSerialNumber;
static BUTTON_Handle s_changeSerialNumberBtn;
static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static EDIT_Handle s_editSerial;
static EDIT_Handle s_editSerialEight;
static BUTTON_Handle s_clearBtn;
static BUTTON_Handle s_decisionBtn;
static BUTTON_Handle s_closeContactBtn;

/****************************************************************************************/
/*                                                                                      */
/* General 	-serialNumberSettingScreen_HideAllItem 								        */
/*                                                                                      */
/* Details  -Hide all item when show pop up									     	    */
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
/*                     (O) s_backBtn : back button                                      */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) g_serialNumberSettingScreen : serial number setting screen	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serialNumberSettingScreen_HideAllItem(void)
{
	WM_HideWindow(s_backBtn);
	WM_HideWindow(s_homeBtn);
	alarmInfor_HideAlarmInfor();
	WM_Paint(g_serialNumberSettingScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-serialNumberSettingScreen_ShowAllItem 								        */
/*                                                                                      */
/* Details  -Show all item when hide pop up									     	    */
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
/*                     (O) s_backBtn : back button                                      */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) g_serialNumberSettingScreen : serial number setting screen	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serialNumberSettingScreen_ShowAllItem(void)
{
	WM_ShowWindow(s_backBtn);
	WM_ShowWindow(s_homeBtn);
	alarmInfor_ShowAlarmInfor();
	WM_Paint(g_serialNumberSettingScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-serialNumberSettingScreen_Callback 								        */
/*                                                                                      */
/* Details  -Call back function for serial number Screen, handle event from     	    */
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
void serialNumberSettingScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		if(!WM_IsVisible(s_popUpSerialSetting))
		{
			GUI_SetColor(GUI_BLACK);
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				guiTask_SetFont(&GUI_FontEng3L16B2PP);
				GUI_DispStringHCenterAt("Serial number setting", 400,5);

				guiTask_SetFont(&GUI_FontEng3L14B2PP);
				GUI_DispStringHCenterAt("Current serial number", 400,60);
			}
			else
			{
				guiTask_SetFont(&GUI_FontJapan3L18B2PP);
				GUI_DispStringHCenterAt("シリアル番号設定", 400,5);

				guiTask_SetFont(&GUI_FontJapan3L16B2PP);
				GUI_DispStringHCenterAt("シリアル番号", 400,60);

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
/* General 	-serialNumberSettingScreen_CallbackChangeSerialBtn 							*/
/*                                                                                      */
/* Details  -Call back function for change clock Screen, handle event from     	    	*/
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
/*                     (O) s_editSerial : display full serial number            	  	*/
/*                     (O) s_editSerialEight : display eight string of serial number    */
/*                     (O) s_popUpSerialSetting : popup								    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serialNumberSettingScreen_CallbackChangeSerialBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Serial change", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("シリアル番号変更", Rect.x1/2, 19);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			int8_t serialNumber[SERIAL_NUMBER_BUFFER_SIZE];
			guiInterface_GetSerialNumber(serialNumber);
			EDIT_SetText(s_editSerial, (char*)serialNumber);
			EDIT_SetText(s_editSerialEight, (char*)serialNumber);
			WM_ShowWindow(s_popUpSerialSetting);

			serialNumberSettingScreen_HideAllItem();
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
/* General 	-serialNumberSettingScreen_CallbackBackBtn 								    */
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
void serialNumberSettingScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			serialNumberSettingScreen_Hide();
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
/* General 	-serialNumberSettingScreen_CallbackHomeBtn 								    */
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
static void serialNumberSettingScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			serialNumberSettingScreen_Hide();
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
/* General 	-serialNumberSettingScreen_CallbackPopUp 								    */
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
static void serialNumberSettingScreen_CallbackPopUp(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(BUTTON_DIALOG_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,50);

		GUI_SetColor(BORDER_SERIAL_COLOR);
		GUI_AA_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,50);

		GUI_SetBkColor(BUTTON_DIALOG_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Please enter the serial number", 390,10);
			GUI_DispStringHCenterAt("Connect a barcode reader\nPlease read the barcode", 390,150);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L20B2PP);
			GUI_DispStringHCenterAt("シリアル番号を入力してください", 390,10);
			GUI_DispStringHCenterAt("バーコードリーダーを接続して\nバーコードを読み込んで下さい", 390,150);
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
/* General 	-serialNumberSettingScreen_callBackClearBtn 								*/
/*                                                                                      */
/* Details  -call back fuction for clear button, handle event from the window 			*/
/*			  														                    */
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
/*                     (O) s_editSerial : display full serial number            	  	*/
/*                     (O) s_editSerialEight : display eight string of serial number    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serialNumberSettingScreen_callBackClearBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(BUTTON_DIALOG_BACKGROUND_COLOR);
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
			GUI_DispStringHCenterAt("Clear", Rect.x1/2, 22);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("クリア", Rect.x1/2, 22);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			EDIT_SetText(s_editSerial, "");
			EDIT_SetText(s_editSerialEight, "");
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
/* General 	-serialNumberSettingScreen_Reverse							        		*/
/*                                                                                      */
/* Details  -Reverses a string 'str' of length 'len'								    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t *str : pointer string 										*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int8_t : reverse string		                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
int8_t serialNumberSettingScreen_Reverse(int8_t s[])
{
	int8_t c = 0;
	int len=strlen((char*)s);
	for(int i=0; i<len/2;i++)
	{
		c=s[i];
		s[i]=s[len-1-i];
		s[len-1-i]=c;
	}
	return (c);
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-serialNumberSettingScreen_CallbackDecisionBtn 								*/
/*                                                                                      */
/* Details  -call back fuction for Decision button, handle event from the window 	    */
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : messenger from the window					 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_editSerial : serial number							        */
/*                     (O) s_currentSerialNumber : display serial number    			*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serialNumberSettingScreen_CallbackDecisionBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(BUTTON_DIALOG_BACKGROUND_COLOR);
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
			GUI_DispStringHCenterAt("Decision", Rect.x1/2, 22);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("決定", Rect.x1/2, 22);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			int8_t serialNumber[SERIAL_NUMBER_BUFFER_SIZE] = {0};

			EDIT_GetText(s_editSerialEight, (char*)serialNumber,SERIAL_NUMBER_BUFFER_SIZE);

			if(strlen((char*)serialNumber) >= MAX_LENGTH_SERIAL_NUMBER)
			{
				guiInterface_SetSerialNumber(serialNumber);
				guiInterface_EEPROMWrite(EEPROM_SERIAL_NUMBER_OFFSET, EEPROM_SERIAL_NUMBER_PAGE,(uint32_t*)serialNumber, SERIAL_NUMBER_BUFFER_SIZE);
				setting_RecordChecksum();

				EDIT_SetText(s_currentSerialNumber, (char*)serialNumber);
			}

			WM_HideWindow(s_popUpSerialSetting);
			serialNumberSettingScreen_ShowAllItem();
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
/* General 	-serialNumberSettingScreen_CallbackCloseBtn 								*/
/*                                                                                      */
/* Details  -call back fuction for close button, handle event from the window 			*/
/*			  														                    */
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
/*                     (O) s_currentSerialNumber : display serial number 			   	*/
/*                     (O) s_popUpSerialSetting : popup						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serialNumberSettingScreen_CallbackCloseBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(BUTTON_DIALOG_BACKGROUND_COLOR);
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
			GUI_DispStringHCenterAt("Close", Rect.x1/2, 22);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("閉じる", Rect.x1/2, 22);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			WM_HideWindow(s_popUpSerialSetting);
			int8_t serialNumber[SERIAL_NUMBER_BUFFER_SIZE];
			guiInterface_GetSerialNumber(serialNumber);
			EDIT_SetText(s_currentSerialNumber, (char*)serialNumber);

			serialNumberSettingScreen_ShowAllItem();
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
/* General 	-serialNumberSettingScreen_Init							        			*/
/*                                                                                      */
/* Details  -Initialize serial number Setting Screen	            				    */
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
/*                     (O) g_serialNumberSettingScreen : serial numer setting screen    */
/*                     (O) s_currentSerialNumber : display serial number	            */
/*                     (O) s_changeSerialNumberBtn : change serial button			    */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_popUpSerialSetting : popup						            */
/*                     (O) s_editSerial : serial number full string			        	*/
/*                     (O) s_editSerialEight : eight string of serial number	        */
/*                     (O) s_clearBtn : clear button						            */
/*                     (O) s_decisionBtn : decision button						        */
/*                     (O) s_closeContactBtn : close button						        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serialNumberSettingScreen_Init(void)
{
	g_serialNumberSettingScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,serialNumberSettingScreen_Callback,0);

	s_currentSerialNumber = EDIT_CreateEx(200,90,400,50,g_serialNumberSettingScreen,WM_CF_SHOW,0,eCurrentSerialNumberId,50);
	EDIT_SetFocussable(s_currentSerialNumber,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_currentSerialNumber ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_currentSerialNumber ,GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_currentSerialNumber ,"00000000");

	s_changeSerialNumberBtn = BUTTON_CreateEx(300, 200, 200, 60, g_serialNumberSettingScreen, WM_CF_SHOW, 0, eChangeSerialNumberBtnId);
	WM_SetCallback(s_changeSerialNumberBtn,serialNumberSettingScreen_CallbackChangeSerialBtn);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_serialNumberSettingScreen, WM_CF_SHOW, 0, eBackBtnSerialNumberId);
	WM_SetCallback(s_backBtn,serialNumberSettingScreen_CallbackBackBtn);

	//Home button
	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_serialNumberSettingScreen, WM_CF_SHOW, 0, eHomeBtnSerialNumberId);
	WM_SetCallback(s_homeBtn,serialNumberSettingScreen_CallbackHomeBtn);

	s_popUpSerialSetting = WM_CreateWindowAsChild(10, 10, 780, 460,g_serialNumberSettingScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,serialNumberSettingScreen_CallbackPopUp,0);

	s_editSerial = EDIT_CreateEx(220,40,340,50,s_popUpSerialSetting,WM_CF_HIDE,0,eEditFieldSerialNumberId,50);
	guiTaskEDIT_SetFont (s_editSerial, &GUI_FontEng3L16B2PP);
	EDIT_SetTextColor(s_editSerial,EDIT_CI_ENABLED ,GUI_BLACK);
	EDIT_SetTextAlign(s_editSerial,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_editSerialEight = EDIT_CreateEx(220,40,340,50,s_popUpSerialSetting,WM_CF_SHOW,0,eEditFieldEightSerialNumberId,50);
	guiTaskEDIT_SetFont (s_editSerialEight, &GUI_FontEng3L16B2PP);
	EDIT_SetTextColor(s_editSerialEight,EDIT_CI_ENABLED ,GUI_BLACK);
	EDIT_SetTextAlign(s_editSerialEight,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_clearBtn = BUTTON_CreateEx(585,300,120,70,s_popUpSerialSetting,WM_CF_SHOW,0,eClearDialogSerialId);
	WM_SetCallback(s_clearBtn,serialNumberSettingScreen_callBackClearBtn);

	s_decisionBtn = BUTTON_CreateEx(585,380,120,70,s_popUpSerialSetting,WM_CF_SHOW,0,eDecisionDialogSerialId);
	WM_SetCallback(s_decisionBtn,serialNumberSettingScreen_CallbackDecisionBtn);

	s_closeContactBtn = BUTTON_CreateEx(75,380,120,70,s_popUpSerialSetting,WM_CF_SHOW,0,eCloseDialogSerialId);
	WM_SetCallback(s_closeContactBtn,serialNumberSettingScreen_CallbackCloseBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-serialNumberSettingScreen_Show							        			*/
/*                                                                                      */
/* Details  -Show serial number Setting Screen	            				   			*/
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
/*                     (O) s_currentSerialNumber : display serial number			   	*/
/*                     (O) s_popUpSerialSetting : popup						            */
/*                     (O) g_serialNumberSettingScreen : serial number setting screen   */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serialNumberSettingScreen_Show(void)
{
	alarmInfor_ShowAlarmInfor();
	int8_t serialNumber[SERIAL_NUMBER_BUFFER_SIZE];
	guiInterface_GetSerialNumber(serialNumber);
	EDIT_SetText(s_currentSerialNumber ,(char*)serialNumber);

	WM_HideWindow(s_popUpSerialSetting);
	serialNumberSettingScreen_ShowAllItem();

	WM_ShowWindow(g_serialNumberSettingScreen);
	WM_BringToTop(g_serialNumberSettingScreen);
	WM_SetFocus(g_serialNumberSettingScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-serialNumberSettingScreen_Hide							        			*/
/*                                                                                      */
/* Details  -Hide serial number Setting Screen	            				   			*/
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
/*                     (O) g_serialNumberSettingScreen : serial number setting screen 	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serialNumberSettingScreen_Hide(void)
{
	WM_HideWindow(g_serialNumberSettingScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-serialNumberSettingScreen_DisplaySerialNumber							    */
/*                                                                                      */
/* Details  -Display serial number				            				   			*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t length : - length of serial number string 					*/
/*                      - range : 0-255                                                 */
/*                      - unit : NONE                                                   */
/*																						*/
/*                const int8_t* serialStr : serial number string                        */
/*                                                                                      */
/*                                                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_editSerial : full string serial number				        */
/*                     (O) s_editSerialEight : eight string of serial number	        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serialNumberSettingScreen_DisplaySerialNumber(uint8_t length, const int8_t* serialStr)
{
	if(length > 0)
	{
		int8_t bufferserial[length];

		EDIT_SetText(s_editSerial, (char*)serialStr);
		EDIT_GetText(s_editSerial,(char*)bufferserial,length);
		EDIT_SetText(s_editSerial, (char*)bufferserial);

		int8_t bufferEightStr[SERIAL_NUMBER_BUFFER_SIZE] = {'\0'};
		serialNumberSettingScreen_Reverse(bufferserial);
		memcpy(bufferEightStr,bufferserial,MAX_LENGTH_SERIAL_NUMBER);
		serialNumberSettingScreen_Reverse(bufferEightStr);
		DEBUGOUT("\n bufferEightStr = %s",bufferEightStr);
		EDIT_SetText(s_editSerialEight, (char*)bufferEightStr);
	}

	return;
}

/* end of files */

