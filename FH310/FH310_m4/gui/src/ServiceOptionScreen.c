/********************************************************************/
/*                                                                  */
/* File Name    : ServiceOptionScreen.c                             */
/*                                                                  */
/* General      : Service option screen display service option		*/
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
/* #0001       														*/
/*                                                                  */
/********************************************************************/
#include "ServiceOptionScreen.h"
#include "BUTTON.h"
#include "Font.h"
#include "MainScreen.h"
#include "setting.h"
#include "OptionScreen.h"
#include "GuiTask.h"
#include "EquipmentScreen.h"
#include "EquipmentInforScreen.h"
#include "AlarmInfor.h"
#include "EquipmentSettingScreen.h"
#include "TextLibrary.h"
#include "TEXT.h"
#include "GuiInterface.h"

//Define item id
enum{
	eEquipmentInforBtnServiceOptionScrId,
	eEquipmentConfigBtnServiceOptionScrId,
	eHomeBtnServiceOptionScrId
};

WM_HWIN g_serviceOptionScreen;

static BUTTON_Handle s_equipInforBtn;
static BUTTON_Handle s_equipConfigBtn;
static BUTTON_Handle s_homeBtn;
static bool s_isEquipInfor = false;

/****************************************************************************************/
/*                                                                                      */
/* General 	-serviceOptionScreen_CallbackEquipmentInforBtn 								*/
/*                                                                                      */
/* Details  -call back fuction for equipment infor button, handle event from the window */
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
/*                     (O) s_isEquipInfor : flag visible equipment infor screen         */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serviceOptionScreen_CallbackEquipmentInforBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Equipment Information", Rect.x1/2, 20);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L20B2PP);
			GUI_DispStringHCenterAt("機器情報表示", Rect.x1/2, 18);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_isEquipInfor = true;
			optionScreen_Hide();
			equipmentScreen_Show();
			equipmentInforScreen_Show();
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
/* General 	-serviceOptionScreen_CallbackEquipmentConfigBtn 						    */
/*                                                                                      */
/* Details  -call back fuction for equipment config button, handle event from the window*/
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
/*                     (O) s_isEquipInfor : flag visible equipment infor screen        	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serviceOptionScreen_CallbackEquipmentConfigBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(BUTTON_NUMBER_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10);

		GUI_SetColor(BORDER_COLOR);
		GUI_DrawRoundedFrame(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10,2);

		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
		if(WM_IsEnabled(pMsg->hWin))
		{
			GUI_SetColor(GUI_BLACK);
		}
		else
		{
			GUI_SetColor(GUI_GRAY);
		}

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Equipment Configuration", Rect.x1/2, 20);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L20B2PP);
			GUI_DispStringHCenterAt("機器設定変更", Rect.x1/2, 18);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_isEquipInfor = false;
			optionScreen_Hide();
			equipmentScreen_Show();
			equipmentSettingScreen_Show();
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
/* General 	-serviceOptionScreen_Callback	                                            */
/*                                                                                      */
/* Details  -call back function for service option screen, handle event from window	    */
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
void serviceOptionScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
	{
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		uint8_t lang = guiInterface_GetSettingValue(eLanguageSettingId);

		GUI_SetColor(GUI_BLACK);
		if(lang == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringAt("Can not change the instrument settings while driving.",100,250);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L20B2PP);
			GUI_DispStringAt("運転中に機器設定変更はできません。",150,250);
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
/* General 	-serviceOptionScreen_CallbackHomeBtn 								        */
/*                                                                                      */
/* Details  -call back fuction for home button, handle event from the window 	        */
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
/* Using Global Data : (I) g_mainWindow : main window							        */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void serviceOptionScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
/* General 	-serviceOptionScreen_Init								       				*/
/*                                                                                      */
/* Details  -Initialize service option screen        									*/
/*			  																			*/
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
/*                     (O) g_serviceOptionScreen : service option screen   			   	*/
/*                     (O) s_equipInforBtn : equipment infor button			         	*/
/*                     (O) s_equipConfigBtn : equipment setting button	              	*/
/*                     (O) s_homeBtn : home button						                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serviceOptionScreen_Init(void)
{
	g_serviceOptionScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_optionScreen,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP, serviceOptionScreen_Callback, 0);

	s_equipInforBtn = BUTTON_CreateEx(10,150,385,60,g_serviceOptionScreen,WM_CF_SHOW,0,eEquipmentInforBtnServiceOptionScrId);
	WM_SetCallback(s_equipInforBtn,serviceOptionScreen_CallbackEquipmentInforBtn);

	s_equipConfigBtn = BUTTON_CreateEx(405,150,385,60,g_serviceOptionScreen,WM_CF_SHOW,0,eEquipmentConfigBtnServiceOptionScrId);
	WM_SetCallback(s_equipConfigBtn,serviceOptionScreen_CallbackEquipmentConfigBtn);

	s_homeBtn = BUTTON_CreateEx(350, 365, 100, 60, g_serviceOptionScreen, WM_CF_SHOW, 0, eHomeBtnServiceOptionScrId);
	WM_SetCallback(s_homeBtn,serviceOptionScreen_CallbackHomeBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-serviceOptionScreen_GetFlagEquipInfor								       	*/
/*                                                                                      */
/* Details  -Get flag equipment infor		        									*/
/*			  																			*/
/*                                                                                      */
/* Arguments : (I) NONE					 												*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_isEquipInfor : flag equipment infor			         	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool serviceOptionScreen_GetFlagEquipInfor(void)
{
	return s_isEquipInfor;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-serviceOptionScreen_SetDisableBtn								        	*/
/*                                                                                      */
/* Details  -Disable equiment setting button when system is operation mode		       	*/
/*			  																			*/
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
/*                     (O) s_equipConfigBtn : equipment config button                   */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serviceOptionScreen_SetDisableBtn(void)
{
	WM_DisableWindow(s_equipConfigBtn);
	BUTTON_SetTextColor(s_equipConfigBtn, BUTTON_CI_DISABLED,GUI_GRAY);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-serviceOptionScreen_SetEnableBtn								        	*/
/*                                                                                      */
/* Details  -Enable equiment setting button when system is standby mode		       		*/
/*			  																			*/
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
/*                     (O) s_equipConfigBtn : equipment config button                   */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void serviceOptionScreen_SetEnableBtn(void)
{
	WM_EnableWindow(s_equipConfigBtn);
	BUTTON_SetTextColor(s_equipConfigBtn, BUTTON_CI_UNPRESSED,GUI_BLACK);

	return;
}

/* end of files */

