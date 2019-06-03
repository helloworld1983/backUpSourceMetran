/********************************************************************/
/*                                                                  */
/* File Name    : PasswordDialog.c                                 	*/
/*                                                                  */
/* General      : Password dialog provides the virtual keyboard 	*/
/*                to enter your user password.						*/
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
/* #0001       2017/09/28       Viet Le				modify			*/
/*        Remove #include "common.h"                                */
/* #0002       2017/10/19       Viet Le				modify			*/
/*  Add passwordDialog_DisplayFlowRate() function  display flow rate*/
/*	 in password dialog when change flow rate (bug 1986)			*/
/* #0003       2017/10/19       Viet Le				modify			*/
/*	Add passwordDialog_DisableAllItem() function disable item when 	*/
/*    popup error password is show, passwordDialog_EnableAllItem()  */
/*	  function enable item when popup error is hide(bug 1987)		*/
/* #0004       2017/10/27	   Viet Le				modify			*/
/*	-Edit function : EDIT_SetFont()->guiTaskEDIT_SetFont().			*/
/*     TEXT_SetFont()->guiTaskTEXT_SetFont()(bug 1997)              */
/********************************************************************/

#include "string.h"
#include "board.h"
#include "MainScreen.h"
#include "PasswordDialog.h"
#include "OperationScreen.h"
#include "EDIT.h"
#include "BUTTON.h"
#include "StringTable.h"
#include "MainScreen.h"
#include "ModeMgr.h"
#include "GuiTask.h"
#include "setting.h"
#include "TEXT.h"
#include "Font.h"
#include "HighAlarmScreen.h"
#include "MediumAlarmScreen.h"
#include "LowAlarmScreen.h"
#include "AlarmScreenMgr.h"
#include "StandbyScreen.h"
#include "UsbInterface.h"
#include "GuiInterface.h"

//Define item id
enum
{
	eOkBtnErrorPassStringId,
	eFlowRateSettingInPassId,
	eUintFlowRateSettingInPassId,
	eTimer10SecondsId,
	eEditPasswordId
};

WM_HWIN g_passwordDialog;

static E_UserMaintenance s_userLogin = E_NO_USER;
static EDIT_Handle s_editPassword;
static BUTTON_Handle s_zeroBtn;
static BUTTON_Handle s_oneBtn;
static BUTTON_Handle s_twoBtn;
static BUTTON_Handle s_threeBtn;
static BUTTON_Handle s_fourBtn;
static BUTTON_Handle s_fineBtn;
static BUTTON_Handle s_sixBtn;
static BUTTON_Handle s_sevenBtn;
static BUTTON_Handle s_eightBtn;
static BUTTON_Handle s_nineBtn;
static BUTTON_Handle s_enterPassBtn;
static BUTTON_Handle s_clearPassBtn;
static BUTTON_Handle s_closePassBtn;
static BUTTON_Handle s_OKBtn;
static TEXT_Handle s_flowRateSetting;
static TEXT_Handle s_uintFlowRateSetting;
static WM_HWIN s_popUpErrorPass;
static int8_t s_passStr[SIZE_BUFFER_PASS] = {'\0'};
static int8_t s_passCheckStr[SIZE_BUFFER_PASS] = {'\0'};
static WM_HTIMER s_timer10Seconds;
static GUI_COLOR s_bkColor = GUI_WHITE;

/****************************************************************************************/
/*                                                                                      */
/* General 	-passwordDialog_Callback								    				*/
/*                                                                                      */
/* Details  -call back fuction for password dialog, handle event from the window 		*/
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
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void passwordDialog_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(s_bkColor);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(BUTTON_DIALOG_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,50);

		GUI_SetColor(BORDER_PASS_COLOR);
		GUI_AA_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,50);

		GUI_SetBkColor(BUTTON_DIALOG_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Please enter password", 400,5);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L20B2PP);
			GUI_DispStringHCenterAt("パスワードを入力してください", 400,5);
		}
		break;
	case WM_TIMER:
	{
		switch(WM_GetTimerId(pMsg->Data.v))
		{
		case eTimer10SecondsId:
			if(WM_IsVisible(pMsg->hWin))
			{
				passwordDialog_Hide();
				standbyScreen_RestartTimerBlackScreen();
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
/* General 	-passwordDialog_CallbackPopUp 												*/
/*                                                                                      */
/* Details  -call back fuction for error password dialog, handle event from the window  */
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
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void passwordDialog_CallbackPopUp(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Login Failure \n Incorrect Password", 350,40);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("ログイン失敗\n正しくないパスワードです", 350,40);
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
/* General 	-passwordDialog_CallbackNumberBtn 								        	*/
/*                                                                                      */
/* Details  -call back fuction for number button, handle event from the window 			*/
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
/*                     (O) s_editPassword : display password                            */
/*                     (O) s_passCheckStr : password check						        */
/*                     (O) s_timer10Seconds : timer 10 seconds hide password dialog		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void passwordDialog_CallbackNumberBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	int8_t buffer[SIZE_BUFFER_PASS] = {'\0'};
	int8_t numberBuffer[SIZE_NUMBER_BUFFER] = {'\0'};

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(BUTTON_NUMBER_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,0);

		GUI_SetColor(BORDER_COLOR);
		GUI_DrawRoundedFrame(Rect.x0,Rect.y0,Rect.x1,Rect.y1,0,2);

		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);
		guiTask_SetFont(&GUI_FontEng3L16B2PP);

		BUTTON_GetText(pMsg->hWin,(char*)&numberBuffer[0],3);
		GUI_DispStringHCenterAt((char*)numberBuffer, Rect.x1/2, 22);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 1)
		{
			if(strlen((char*)&s_passStr[0]) < SIZE_BUFFER_PASS)
			{
				strcat((char*)&s_passStr[0],"o");
				EDIT_SetText(s_editPassword,(char*)s_passStr);
				BUTTON_GetText(pMsg->hWin,(char*)&buffer[0],10);
				strncat((char*)&s_passCheckStr[0],(char*)&buffer[0],SIZE_BUFFER_PASS - 1);
			}

			WM_RestartTimer(s_timer10Seconds,TIMER_HIDE_PASS);
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
/* General 	-passwordDialog_DisableAllItem 								        		*/
/*                                                                                      */
/* Details  -Disable item when show popup password error							 	*/
/*			  														                    */
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
/*                     (O) s_editPassword : display password                            */
/*                     (O) s_zeroBtn : zero number button						        */
/*                     (O) s_oneBtn : one number button                                 */
/*                     (O) s_twoBtn	: two number button					                */
/*                     (O) s_threeBtn : three number button                             */
/*                     (O) s_fourBtn : four number button						        */
/*                     (O) s_fineBtn : five number button                               */
/*                     (O) s_sixBtn	: six number button					                */
/*                     (O) s_sevenBtn : seven number button                             */
/*                     (O) s_eightBtn : eight number button						        */
/*                     (O) s_nineBtn : nine number button                               */
/*                     (O) s_clearPassBtn : clear button						        */
/*                     (O) s_enterPassBtn : enter button                                */
/*                     (O) s_closePassBtn : close button						        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void passwordDialog_DisableAllItem(void)
{
	WM_DisableWindow(s_editPassword);
	EDIT_SetBkColor(s_editPassword,EDIT_CI_DISABLED,GUI_WHITE);

	WM_DisableWindow(s_zeroBtn);
	WM_DisableWindow(s_oneBtn);
	WM_DisableWindow(s_twoBtn);
	WM_DisableWindow(s_threeBtn);
	WM_DisableWindow(s_fourBtn);
	WM_DisableWindow(s_fineBtn);
	WM_DisableWindow(s_sixBtn);
	WM_DisableWindow(s_sevenBtn);
	WM_DisableWindow(s_eightBtn);
	WM_DisableWindow(s_nineBtn);
	WM_DisableWindow(s_enterPassBtn);
	WM_DisableWindow(s_clearPassBtn);
	WM_DisableWindow(s_closePassBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-passwordDialog_DisableAllItem 								        		*/
/*                                                                                      */
/* Details  -Enable item when hide popup password error							 		*/
/*			  														                    */
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
/*                     (O) s_editPassword : display password                            */
/*                     (O) s_zeroBtn : zero number button						        */
/*                     (O) s_oneBtn : one number button                                 */
/*                     (O) s_twoBtn	: two number button					                */
/*                     (O) s_threeBtn : three number button                             */
/*                     (O) s_fourBtn : four number button						        */
/*                     (O) s_fineBtn : five number button                               */
/*                     (O) s_sixBtn	: six number button					                */
/*                     (O) s_sevenBtn : seven number button                             */
/*                     (O) s_eightBtn : eight number button						        */
/*                     (O) s_nineBtn : nine number button                               */
/*                     (O) s_clearPassBtn : clear button						        */
/*                     (O) s_enterPassBtn : enter button                                */
/*                     (O) s_closePassBtn : close button						        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void passwordDialog_EnableAllItem(void)
{
	WM_EnableWindow(s_editPassword);
	EDIT_SetBkColor(s_editPassword,EDIT_CI_ENABELD,GUI_WHITE);

	WM_EnableWindow(s_zeroBtn);
	WM_EnableWindow(s_oneBtn);
	WM_EnableWindow(s_twoBtn);
	WM_EnableWindow(s_threeBtn);
	WM_EnableWindow(s_fourBtn);
	WM_EnableWindow(s_fineBtn);
	WM_EnableWindow(s_sixBtn);
	WM_EnableWindow(s_sevenBtn);
	WM_EnableWindow(s_eightBtn);
	WM_EnableWindow(s_nineBtn);
	WM_EnableWindow(s_enterPassBtn);
	WM_EnableWindow(s_clearPassBtn);
	WM_EnableWindow(s_closePassBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-passwordDialog_CallbackEnterBtn 								        	*/
/*                                                                                      */
/* Details  -call back fuction for enter button, handle event from the window 			*/
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
/* Using Global Data : (I) g_operationScreen : operation screen					    	*/
/*                     (I) s_passCheckStr : password check     				            */
/*                     (I) g_mainWindow : main window     				            	*/
/*                     (O) s_popUpErrorPass : error popup                               */
/*                     (O) s_timer10Seconds : timer 10 seconds hide password dialog	    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void passwordDialog_CallbackEnterBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10);

		GUI_SetColor(BORDER_COLOR);
		GUI_DrawRoundedFrame(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10,2);

		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Enter", Rect.x1/2, 22);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("決定", Rect.x1/2, 22);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 1)
		{
			int compareValueOne = strcmp((char*)s_passCheckStr,"3100");
			int compareValueTwo = strcmp((char*)s_passCheckStr,"3101");
			int compareValueThree = strcmp((char*)s_passCheckStr,"3102");
			int compareValueFour = strcmp((char*)s_passCheckStr,"3103");
			int compareValueFive = strcmp((char*)s_passCheckStr,"31042572");

			if(compareValueOne == 0)
			{
				passwordDialog_Hide();

				if(WM_IsVisible(g_operationScreen))
				{
					operationScreen_Hide();
				}
				else if(alarmScreenMgr_GetFlagAlarmScreen() == true)
				{
					highAlarmScreen_Hide();
					mediumAlarmScreen_Hide();
					lowAlarmScreen_Hide();
					alarmScreenMgr_SetFlagAlarmScreen(false);
				}
				s_userLogin = E_DISTRIBUTOR_USER;
				WM_SendMessageNoPara(g_mainWindow,WM_SHOW_SERVICE_TAB_SCREEN);
			}
			else if (compareValueTwo == 0)
			{
				passwordDialog_Hide();
				if(WM_IsVisible(g_operationScreen))
				{
					operationScreen_Hide();
				}
				else if(alarmScreenMgr_GetFlagAlarmScreen() == true)
				{
					highAlarmScreen_Hide();
					mediumAlarmScreen_Hide();
					lowAlarmScreen_Hide();
					alarmScreenMgr_SetFlagAlarmScreen(false);
				}

				s_userLogin = E_MANUFACTURE_INSPECTION_USER;
				WM_SendMessageNoPara(g_mainWindow,WM_SHOW_SERVICE_TAB_SCREEN);

			}
			else if (compareValueThree == 0)
			{
				passwordDialog_Hide();
				if(WM_IsVisible(g_operationScreen))
				{
					operationScreen_Hide();
				}
				else if(alarmScreenMgr_GetFlagAlarmScreen() == true)
				{
					highAlarmScreen_Hide();
					mediumAlarmScreen_Hide();
					lowAlarmScreen_Hide();
					alarmScreenMgr_SetFlagAlarmScreen(false);
				}
				s_userLogin = E_MANUFACTURE_USER;

				WM_SendMessageNoPara(g_mainWindow,WM_SHOW_SERVICE_TAB_SCREEN);

			}
			else if (compareValueFour == 0)
			{
				passwordDialog_Hide();
				if(WM_IsVisible(g_operationScreen))
				{
					operationScreen_Hide();
				}
				else if(alarmScreenMgr_GetFlagAlarmScreen() == true)
				{
					highAlarmScreen_Hide();
					mediumAlarmScreen_Hide();
					lowAlarmScreen_Hide();
					alarmScreenMgr_SetFlagAlarmScreen(false);
				}
				s_userLogin = E_MANUFACTURE_OPTION_USER;

				WM_SendMessageNoPara(g_mainWindow,WM_SHOW_SERVICE_TAB_SCREEN);
			}
			else if (compareValueFive == 0)
			{
				usb_SendEvent(eRequestPassword);
			}
			else
			{
				WM_ShowWindow(s_popUpErrorPass);
				WM_BringToTop(s_popUpErrorPass);
				WM_RestartTimer(s_timer10Seconds,TIMER_HIDE_PASS);

				//Disable item
				passwordDialog_DisableAllItem();
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
/* General 	-passwordDialog_CallbackClearBtn 								        	*/
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
/*                     (O) s_editPassword : display password                            */
/*                     (O) s_passStr : password string						            */
/*                     (O) s_passCheckStr : password check						        */
/*                     (O) s_timer10Seconds : timer 10 seconds hide password dialog		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void passwordDialog_CallbackClearBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);

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
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 1)
		{
			EDIT_SetText(s_editPassword, "");
			for(int i = 0; i < SIZE_BUFFER_PASS; i++)
			{
				s_passStr[i] = '\0';
				s_passCheckStr[i] = '\0';
			}

			WM_RestartTimer(s_timer10Seconds,TIMER_HIDE_PASS);
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
/* General 	-passwordDialog_CallbackCloseBtn 								        	*/
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
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void passwordDialog_CallbackCloseBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);

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
			GUI_DispStringHCenterAt("閉じ る", Rect.x1/2, 22);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 1)
		{
			passwordDialog_Hide();
			standbyScreen_RestartTimerBlackScreen();
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
/* General 	-passwordDialog_CallbackOKBtn	 								        	*/
/*                                                                                      */
/* Details  -call back fuction for ok button, handle event from the window 				*/
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
/*                     (O) s_editPassword : display password                            */
/*                     (O) s_passStr : password string						            */
/*                     (O) s_passCheckStr : password check						        */
/*                     (O) s_popUpErrorPass : error popup								*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void passwordDialog_CallbackOKBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("閉じ る", Rect.x1/2, 22);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 1)
		{
			EDIT_SetText(s_editPassword, "");
			for(int i = 0; i < SIZE_BUFFER_PASS; i++)
			{
				s_passStr[i] = '\0';
				s_passCheckStr[i] = '\0';
			}
			WM_HideWindow(s_popUpErrorPass);

			//Enable item
			passwordDialog_EnableAllItem();
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
/* General 	-passwordDialog_Init 								       					*/
/*                                                                                      */
/* Details  -Initialize password dialog with virtual keyborad							*/
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
/*                     (O) g_passwordDialog : password dialog                           */
/*                     (O) s_popUpErrorPass : error popup						        */
/*                     (O) s_editPassword : display password                            */
/*                     (O) s_zeroBtn : zero number button						        */
/*                     (O) s_oneBtn : one number button                                 */
/*                     (O) s_twoBtn	: two number button					                */
/*                     (O) s_threeBtn : three number button                             */
/*                     (O) s_fourBtn : four number button						        */
/*                     (O) s_fineBtn : five number button                               */
/*                     (O) s_sixBtn	: six number button					                */
/*                     (O) s_sevenBtn : seven number button                             */
/*                     (O) s_eightBtn : eight number button						        */
/*                     (O) s_nineBtn : nine number button                               */
/*                     (O) s_clearPassBtn : clear button						        */
/*                     (O) s_enterPassBtn : enter button                                */
/*                     (O) s_closePassBtn : close button						        */
/*                     (O) s_flowRateSetting : display flow rate				       	*/
/*                     (O) s_uintFlowRateSetting : display unit flow rate		        */
/*                     (O) s_OKBtn : OK button						                    */
/*                     (O) s_timer10Seconds	: timer 10 seconds hide password dialog		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void passwordDialog_Init(void)
{
	g_passwordDialog = WM_CreateWindowAsChild(10, 10, 780, 460,g_mainWindow,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,passwordDialog_Callback,0);

	s_popUpErrorPass = WM_CreateWindowAsChild(40, 115, 700, 220,g_passwordDialog,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,passwordDialog_CallbackPopUp,0);

	s_editPassword = EDIT_CreateEx(220,40,340,50,g_passwordDialog,WM_CF_HIDE,0,eEditPasswordId,10);
	guiTaskEDIT_SetFont (s_editPassword, &GUI_FontSpecial22B);
	EDIT_SetTextColor(s_editPassword,EDIT_CI_ENABLED ,GUI_BLACK);
	EDIT_SetTextAlign(s_editPassword,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_zeroBtn = BUTTON_CreateEx(345,320,90,70,g_passwordDialog,WM_CF_SHOW,0,eZeroBtnStringId);
	BUTTON_SetText(s_zeroBtn,"0");
	WM_SetCallback(s_zeroBtn,passwordDialog_CallbackNumberBtn);

	s_oneBtn = BUTTON_CreateEx(245,245,90,70,g_passwordDialog,WM_CF_SHOW,0,eOneBtnStringId);
	BUTTON_SetText(s_oneBtn,"1");
	WM_SetCallback(s_oneBtn,passwordDialog_CallbackNumberBtn);

	s_twoBtn = BUTTON_CreateEx(345,245,90,70,g_passwordDialog,WM_CF_SHOW,0,eTwoBtnStringId);
	BUTTON_SetText(s_twoBtn,"2");
	WM_SetCallback(s_twoBtn,passwordDialog_CallbackNumberBtn);

	s_threeBtn = BUTTON_CreateEx(445,245,90,70,g_passwordDialog,WM_CF_SHOW,0,eThreeBtnStringId);
	BUTTON_SetText(s_threeBtn,"3");
	WM_SetCallback(s_threeBtn,passwordDialog_CallbackNumberBtn);

	s_fourBtn = BUTTON_CreateEx(245,170,90,70,g_passwordDialog,WM_CF_SHOW,0,eFourBtnStringId);
	BUTTON_SetText(s_fourBtn,"4");
	WM_SetCallback(s_fourBtn,passwordDialog_CallbackNumberBtn);

	s_fineBtn = BUTTON_CreateEx(345,170,90,70,g_passwordDialog,WM_CF_SHOW,0,eFineBtnStringId);
	BUTTON_SetText(s_fineBtn,"5");
	WM_SetCallback(s_fineBtn,passwordDialog_CallbackNumberBtn);

	s_sixBtn = BUTTON_CreateEx(445,170,90,70,g_passwordDialog,WM_CF_SHOW,0,eSixBtnStringId);
	BUTTON_SetText(s_sixBtn,"6");
	WM_SetCallback(s_sixBtn,passwordDialog_CallbackNumberBtn);

	s_sevenBtn = BUTTON_CreateEx(245,95,90,70,g_passwordDialog,WM_CF_SHOW,0,eSevenBtnStringId);
	BUTTON_SetText(s_sevenBtn,"7");
	WM_SetCallback(s_sevenBtn,passwordDialog_CallbackNumberBtn);

	s_eightBtn = BUTTON_CreateEx(345,95,90,70,g_passwordDialog,WM_CF_SHOW,0,eEightBtnStringId);
	BUTTON_SetText(s_eightBtn,"8");
	WM_SetCallback(s_eightBtn,passwordDialog_CallbackNumberBtn);

	s_nineBtn = BUTTON_CreateEx(445,95,90,70,g_passwordDialog,WM_CF_SHOW,0,eNineBtnStringId);
	BUTTON_SetText(s_nineBtn,"9");
	WM_SetCallback(s_nineBtn,passwordDialog_CallbackNumberBtn);

	s_clearPassBtn = BUTTON_CreateEx(585,300,120,70,g_passwordDialog,WM_CF_SHOW,0,eClearPassBtnStringId);
	BUTTON_SetText(s_clearPassBtn,"Clear");
	WM_SetCallback(s_clearPassBtn,passwordDialog_CallbackClearBtn);

	s_enterPassBtn = BUTTON_CreateEx(585,380,120,70,g_passwordDialog,WM_CF_SHOW,0,eEnterPassBtnStringId);
	BUTTON_SetText(s_enterPassBtn,"Enter");
	WM_SetCallback(s_enterPassBtn,passwordDialog_CallbackEnterBtn);

	s_closePassBtn = BUTTON_CreateEx(75,380,120,70,g_passwordDialog,WM_CF_SHOW,0,eClosePopUpPassStringId);
	BUTTON_SetText(s_closePassBtn,"Close");
	WM_SetCallback(s_closePassBtn,passwordDialog_CallbackCloseBtn);

	s_flowRateSetting = TEXT_CreateEx(325,420,70,40,g_passwordDialog,WM_CF_HIDE,TEXT_CF_RIGHT,eFlowRateSettingInPassId,"");
	guiTaskTEXT_SetFont(s_flowRateSetting,&GUI_FontEng3L16B2PP);

	s_uintFlowRateSetting = TEXT_CreateEx(395,420,70,40,g_passwordDialog,WM_CF_HIDE,TEXT_CF_RIGHT,eUintFlowRateSettingInPassId,"");
	guiTaskTEXT_SetFont(s_uintFlowRateSetting,&GUI_FontEng3L16B2PP);

	//OK button
	s_OKBtn = BUTTON_CreateEx(210, 140, 280, 60, s_popUpErrorPass, WM_CF_SHOW, 0, eOkBtnErrorPassStringId);
	WM_SetCallback(s_OKBtn,passwordDialog_CallbackOKBtn);

	s_timer10Seconds = WM_CreateTimer(g_passwordDialog, eTimer10SecondsId, TIMER_100_MS, 0);

	WM_HideWindow(s_popUpErrorPass);
	WM_HideWindow(g_passwordDialog);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-passwordDialog_Show		 								       			*/
/*                                                                                      */
/* Details  -Show password dialog with virtual keyborad									*/
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
/*                     (O) g_passwordDialog : password dialog            			   	*/
/*                     (O) s_editPassword : display password					     	*/
/*                     (O) s_passStr : password string							     	*/
/*                     (O) s_passCheckStr : password check						     	*/
/*                     (O) s_flowRateSetting : display flow rate					    */
/*                     (O) s_uintFlowRateSetting : display flow rate unit				*/
/*                     (O) s_timer10Seconds : timer 10 seconds hide password dialog		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void passwordDialog_Show(void)
{
	WM_ShowWindow(g_passwordDialog);
	WM_BringToTop(g_passwordDialog);
	WM_SetFocus(g_passwordDialog);

	EDIT_SetText(s_editPassword, "");
	for(int i = 0; i < SIZE_BUFFER_PASS; i++)
	{
		s_passStr[i] = '\0';
		s_passCheckStr[i] = '\0';
	}

	if(modeMgr_GetCurrentMode() == ePatientMode)
	{
		WM_ShowWindow(s_flowRateSetting);
		WM_ShowWindow(s_uintFlowRateSetting);
		int8_t buffer[SIZE_BUFFER_FLOW_RATE] = {'\0'};
		uint8_t flowRateSetting = guiInterface_GetSettingValue(eFlowRateSettingId);
		double floatValue = (double)flowRateSetting/FLOWRATE_SCALE;
		sprintf((char*)buffer,"%0.2f",floatValue);
		TEXT_SetText(s_flowRateSetting, (char*)buffer);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTaskTEXT_SetFont(s_uintFlowRateSetting,&GUI_FontEng3L16B2PP);
			TEXT_SetText(s_uintFlowRateSetting, "L/min");
		}
		else
		{
			guiTaskTEXT_SetFont(s_uintFlowRateSetting,&GUI_FontJapan3L16B2PP);
			TEXT_SetText(s_uintFlowRateSetting, "L/分");
		}
	}
	else
	{
		WM_HideWindow(s_flowRateSetting);
		WM_HideWindow(s_uintFlowRateSetting);
	}

	GUI_Delay(GUI_DELAY_IN_PASS);
	WM_ShowWindow(s_editPassword);
	WM_RestartTimer(s_timer10Seconds,TIMER_HIDE_PASS);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-passwordDialog_Hide		 								       			*/
/*                                                                                      */
/* Details  -Hide password dialog with virtual keyborad									*/
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
/*                     (O) s_editPassword : display password                            */
/*                     (O) s_passStr : password string						            */
/*                     (O) s_passCheckStr : password check                              */
/*                     (O) g_passwordDialog : password dialog					     	*/
/*                     (O) s_popUpErrorPass : error popup                               */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void passwordDialog_Hide(void)
{
	EDIT_SetText(s_editPassword, "");

	s_passStr[0] = '\0';
	s_passCheckStr[0] = '\0';

	WM_HideWindow(g_passwordDialog);
	WM_HideWindow(s_editPassword);
	WM_HideWindow(s_popUpErrorPass);

	//Enable item
	passwordDialog_EnableAllItem();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-passwordDialog_CheckFile													*/
/*                                                        	                            */
/* Details  -check existence of breathtest_technology file								*/
/*                                                                                      */
/* Arguments : (I) bool isExistFile : - check exist file								*/
/*					 - range : 0-1														*/
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_operationScreen : operation screen					      	*/
/*                     (I) g_mainWindow : main window                                   */
/*                     (O) s_popUpErrorPass : error popup                               */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void passwordDialog_CheckFile(bool isExistFile)
{
	if(isExistFile == true)
	{
		passwordDialog_Hide();

		if(WM_IsVisible(g_operationScreen))
		{
			operationScreen_Hide();
		}
		else if(alarmScreenMgr_GetFlagAlarmScreen() == true)
		{
			highAlarmScreen_Hide();
			mediumAlarmScreen_Hide();
			lowAlarmScreen_Hide();
			alarmScreenMgr_SetFlagAlarmScreen(false);
		}
		s_userLogin = E_DEVELOPER_USER;

		WM_SendMessageNoPara(g_mainWindow,WM_SHOW_SERVICE_TAB_SCREEN);
	}
	else
	{
		WM_ShowWindow(s_popUpErrorPass);
		WM_BringToTop(s_popUpErrorPass);

		//Disable item
		passwordDialog_DisableAllItem();
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-passwordDialog_GetUserLogin											    */
/*                                                        	                            */
/* Details  -Get current user															*/
/*                                                                                      */
/* Arguments : (I) NONE					 												*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : E_UserMaintenance : user login	                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_userLogin : user login							            */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
E_UserMaintenance passwordDialog_GetUserLogin(void)
{
	return s_userLogin;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-passwordDialog_SetBackgroundColor											*/
/*                                                        	                            */
/* Details  -Set background color for password dialog									*/
/*                                                                                      */
/* Arguments : (I) GUI_COLOR color					 									*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void					                                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_bkColor : background color of password dialog    			*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void passwordDialog_SetBackgroundColor(GUI_COLOR color)
{
	s_bkColor = color;

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-passwordDialog_DisplayFlowRate								       			*/
/*                                                                                      */
/* Details  -Display flow rate in password dialog							   			*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) uint8_t FRPSAValue : - flow rate change					 			*/
/*					- range : 5-60														*/
/*                  - unit : LPM                                                        */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_passwordDialog : password dialog							*/
/*                     (O) s_flowRateSetting : flow rate setting						*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void passwordDialog_DisplayFlowRate(uint8_t FRPSAValue)
{
	if(WM_IsVisible(g_passwordDialog))
	{
		int8_t buffer[SIZE_BUFFER_FLOW_RATE] = {'\0'};
		double floatValue = (double)FRPSAValue/FLOWRATE_SCALE;
		sprintf((char*)buffer,"%0.2f",floatValue);
		TEXT_SetText(s_flowRateSetting, (char*)buffer);
	}

	return;
}

/* end of files */

