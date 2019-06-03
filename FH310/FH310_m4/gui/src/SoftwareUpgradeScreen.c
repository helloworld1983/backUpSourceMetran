/********************************************************************/
/*                                                                  */
/* File Name    : SoftwareUpgradeScreen.c                           */
/*                                                                  */
/* General      :The SoftwareUpgrade screen allows user             */
/*                to upgrade Main-CPU								*/
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
/* #0001       2017/09/05      Viet Le              modify			*/
/*     remove #include "assert.h"                                   */
/*  #0002	   2017/09/19      Viet Le  			modify	        */
/*     -Remove extern g_m4_filename,g_m0_filename,g_boot_filename  	*/
/*			(bug 1847)												*/
/*  #0003	   2017/09/28      Viet Le  			modify	        */
/*	  -Remove #include "m4_img_ldr.h" 								*/
/*  #0004       2017/10/27	   Viet Le				modify			*/
/*	-Edit function : EDIT_SetFont()->guiTaskEDIT_SetFont().			*/
/*     TEXT_SetFont()->guiTaskTEXT_SetFont()(bug 1997)              */
/********************************************************************/

#include "string.h"
#include <StringTools.h>
#include "SoftwareUpgradeScreen.h"
#include "MainScreen.h"
#include "EDIT.h"
#include "BUTTON.h"
#include "updateFlash.h"
#include "TEXT.h"
#include "ConfirmUpgradeScreen.h"
#include "GuiTask.h"
#include "setting.h"
#include "Font.h"
#include "AlarmInfor.h"
#include "MaintenanceScreen.h"
#include "OptionScreen.h"
#include "TextLibrary.h"
#include "UsbInterface.h"
#include "GuiInterface.h"

//Define item id
enum{
	eCurrentVersionId,
	eUpgradebtnOfUpgradeId,
	eBackbtnOfUpgradeId,
	eHomebtnOfUpgradeId,
	eTitlePopUpSoftwareUpgradeId,
	eClosePopUpBtnId,
	eLabelSoftwareVersionId,
	eCurrentBootloaderVersionId
};

WM_HWIN g_softwareUpgradeScreen;

static BUTTON_Handle s_softwareUpgradeBtn;
static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static WM_HWIN s_popUpUpgradeSoftware;
static BUTTON_Handle s_closeBtn;
static EDIT_Handle s_currentVersion;
static EDIT_Handle s_currentBootloaderVersion;
static TEXT_Handle s_titlePopUp;

static bool s_isExistBoot = false;
static bool s_isExistFirm = false;

/****************************************************************************************/
/*                                                                                      */
/* General 	-softwareUpgradeScreen_HideAllItem								            */
/*                                                                                      */
/* Details  -Hide all item when show pop up						 			            */
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
/*                     (O) s_homeBtn : home button                                      */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_softwareUpgradeBtn : upgrade button						*/
/*                     (O) s_currentVersion	: display SW version				     	*/
/*                     (O) s_currentBootloaderVersion : display boot version	        */
/*                     (O) g_softwareUpgradeScreen : software upgrade screen 	        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void softwareUpgradeScreen_HideAllItem(void)
{
	WM_HideWindow(s_homeBtn);
	WM_HideWindow(s_backBtn);
	WM_HideWindow(s_softwareUpgradeBtn);
	WM_HideWindow(s_currentVersion);
	WM_HideWindow(s_currentBootloaderVersion);
	alarmInfor_HideAlarmInfor();

	WM_Paint(g_softwareUpgradeScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-softwareUpgradeScreen_ShowAllItem								            */
/*                                                                                      */
/* Details  -Show all item when hide pop up						 			            */
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
/*                     (O) s_isExistBoot : flag exist file upgrade boot		 	        */
/*                     (O) s_isExistFirm : flag exist file upgrade SW		 	        */
/*                     (O) s_homeBtn : home button                                      */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_softwareUpgradeBtn : upgrade button						*/
/*                     (O) s_currentVersion	: display SW version				     	*/
/*                     (O) s_currentBootloaderVersion : display boot version	        */
/*                     (O) g_softwareUpgradeScreen : software upgrade screen 	        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void softwareUpgradeScreen_ShowAllItem(void)
{
	s_isExistBoot = false;
	s_isExistFirm = false;

	WM_ShowWindow(s_homeBtn);
	WM_ShowWindow(s_backBtn);
	WM_ShowWindow(s_softwareUpgradeBtn);
	WM_ShowWindow(s_currentVersion);
	WM_ShowWindow(s_currentBootloaderVersion);
	alarmInfor_ShowAlarmInfor();

	WM_Paint(g_softwareUpgradeScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-softwareUpgradeScreen_CallbackBackBtn								        */
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
void softwareUpgradeScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			softwareUpgradeScreen_Hide();
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
/* General 	-softwareUpgradeScreen_CallbackClosePopUpBtn 								*/
/*                                                                                      */
/* Details  -Call back function for close popup button, handle event from 			    */
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
/*                     (O) s_popUpUpgradeSoftware : popup                               */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void softwareUpgradeScreen_CallbackClosePopUpBtn(WM_MESSAGE *pMsg)
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
		GUI_SetColor(GUI_BLACK);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Close", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("閉じ る", Rect.x1/2, 19);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			WM_HideWindow(s_popUpUpgradeSoftware);
			softwareUpgradeScreen_ShowAllItem();
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
/* General 	-softwareUpgradeScreen_CallbackHomeBtn 								        */
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
static void softwareUpgradeScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			softwareUpgradeScreen_Hide();
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
/* General 	-softwareUpgradeScreen_CallbackSoftwareUpgradeBtn 							*/
/*                                                                                      */
/* Details  -Call back function for software upgrade button, handle event from 			*/
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
/*                     (O) s_closeBtn : close button						            */
/*                     (O) s_popUpUpgradeSoftware : popup						        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void softwareUpgradeScreen_CallbackSoftwareUpgradeBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Software Upgrade", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("ソフトウェア更新", Rect.x1/2, 19);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			WM_MoveChildTo(s_titlePopUp,0,85);

			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);
				TEXT_SetText(s_titlePopUp,"Checking the update file.\nPlease wait");
			}
			else
			{
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontJapan3L16B2PP);
				TEXT_SetText(s_titlePopUp,"更新ファイルを確認しています。\nお待ちください。");
			}

			WM_HideWindow(s_closeBtn);

			WM_ShowWindow(s_popUpUpgradeSoftware);
			WM_BringToTop(s_popUpUpgradeSoftware);
			softwareUpgradeScreen_HideAllItem();
			GUI_Delay(GUI_DELAY_SOFTWARE_UPGRADE);

			if(WM_IsVisible(s_popUpUpgradeSoftware))
			{
				usb_SendEvent(eRequestUpdate);
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
/* General 	-softwareUpgradeScreen_CallbackPopUp 								        */
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
static void softwareUpgradeScreen_CallbackPopUp(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_Clear();

		GUI_GetClientRect(&Rect);

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
/* General 	-softwareUpgradeScreen_Callback 								            */
/*                                                                                      */
/* Details  -Call back function for soft software upgrade screen, handle event from     */
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
void softwareUpgradeScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		if(!WM_IsVisible(s_popUpUpgradeSoftware))
		{
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				GUI_SetColor(GUI_BLACK);
				guiTask_SetFont(&GUI_FontEng3L16B2PP);
				GUI_DispStringHCenterAt("Software Upgrade", 400,5);

				guiTask_SetFont(&GUI_FontEng3L14B2PP);
				GUI_DispStringHCenterAt("Current Version", 400,60);
				GUI_DispStringAt("FH-310 control software", 60,105);
				GUI_DispStringAt("FH-310 boot loader", 110,165);
				GUI_DispStringHCenterAt("After you connect the USB memory\nPress the upgrade button", 400,230);
			}
			else
			{
				GUI_SetColor(GUI_BLACK);

				guiTask_SetFont(&GUI_FontJapan3L18B2PP);;
				GUI_DispStringHCenterAt("ソフトウェア更新", 400,5);

				guiTask_SetFont(&GUI_FontJapan3L16B2PP);
				GUI_DispStringHCenterAt("現在のバージョン", 400,60);
				GUI_DispStringAt("FH-310 ソフトウェア", 90,105);
				GUI_DispStringAt("FH-310 ブートローダー", 90,165);
				GUI_DispStringHCenterAt("USBメモリーを接続してから\n更新ボタンを押して下さい", 400,230);
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
/* General 	-softwareUpgradeScreen_Init 								    			*/
/*                                                                                      */
/* Details  -Initialize software upgrade screen			            					*/
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
/*                     (O) g_softwareUpgradeScreen : software upgrade screen         	*/
/*                     (O) s_currentVersion : display SW version			            */
/*                     (O) s_currentBootloaderVersion : display boot version    		*/
/*                     (O) s_softwareUpgradeBtn : software upgrade button		        */
/*                     (O) s_popUpUpgradeSoftware : popup                               */
/*                     (O) s_titlePopUp : title popup						            */
/*                     (O) s_closeBtn : close button                                    */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button                                      */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void softwareUpgradeScreen_Init(void)
{
	g_softwareUpgradeScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,softwareUpgradeScreen_Callback,0);

	s_currentVersion = EDIT_CreateEx(350,90,400,50,g_softwareUpgradeScreen,WM_CF_SHOW,0,eCurrentVersionId,50);
	EDIT_SetFocussable(s_currentVersion,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_currentVersion ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_currentVersion ,GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_currentVersion ,(char*)stringtools_GetVersion());

	s_currentBootloaderVersion = EDIT_CreateEx(350,150,400,50,g_softwareUpgradeScreen,WM_CF_SHOW,0,eCurrentBootloaderVersionId,50);
	EDIT_SetFocussable(s_currentBootloaderVersion,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_currentBootloaderVersion ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_currentBootloaderVersion ,GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_currentBootloaderVersion ,(char*)stringtools_GetBootVersion());

	s_softwareUpgradeBtn = BUTTON_CreateEx(275, 300, 250, 60, g_softwareUpgradeScreen, WM_CF_SHOW, 0, eUpgradebtnOfUpgradeId);
	WM_SetCallback(s_softwareUpgradeBtn,softwareUpgradeScreen_CallbackSoftwareUpgradeBtn);

	s_popUpUpgradeSoftware = WM_CreateWindowAsChild(50, 115, 700, 220,g_softwareUpgradeScreen,WM_CF_HIDE| WM_CF_MEMDEV | WM_CF_LATE_CLIP,softwareUpgradeScreen_CallbackPopUp,0);

	//Title pop up
	s_titlePopUp = TEXT_CreateEx(0, 30, 700, 100,s_popUpUpgradeSoftware,WM_CF_SHOW,TEXT_CF_RIGHT,eTitlePopUpSoftwareUpgradeId,"");
	TEXT_SetTextColor(s_titlePopUp, GUI_BLACK);
	TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
	guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);

	s_closeBtn = BUTTON_CreateEx(210, 140, 280, 60, s_popUpUpgradeSoftware, WM_CF_SHOW, 0, eClosePopUpBtnId);
	WM_SetCallback(s_closeBtn,softwareUpgradeScreen_CallbackClosePopUpBtn);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_softwareUpgradeScreen, WM_CF_SHOW, 0, eBackbtnOfUpgradeId);
	WM_SetCallback(s_backBtn,softwareUpgradeScreen_CallbackBackBtn);

	//Home button
	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_softwareUpgradeScreen, WM_CF_SHOW, 0, eHomebtnOfUpgradeId);
	WM_SetCallback(s_homeBtn,softwareUpgradeScreen_CallbackHomeBtn);

	confirmUpgradeScreen_Init();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-softwareUpgradeScreen_Show								    				*/
/*                                                                                      */
/* Details  -Show software upgrade screen			            						*/
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
/*                     (O) g_softwareUpgradeScreen : software upgrade screen         	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void softwareUpgradeScreen_Show(void)
{
	alarmInfor_ShowAlarmInfor();
	WM_ShowWindow(g_softwareUpgradeScreen);
	WM_BringToTop(g_softwareUpgradeScreen);
	WM_SetFocus(g_softwareUpgradeScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-softwareUpgradeScreen_Hide 								    			*/
/*                                                                                      */
/* Details  -Hide software upgrade screen			            						*/
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
/*                     (O) g_softwareUpgradeScreen : software upgrade screen    		*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void softwareUpgradeScreen_Hide(void)
{
	WM_HideWindow(g_softwareUpgradeScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-softwareUpgradeScreen_HidePopup 								    		*/
/*                                                                                      */
/* Details  -Hide pop up software upgrade screen			            				*/
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
/*                     (O) g_softwareUpgradeScreen : software upgrade screen       		*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void softwareUpgradeScreen_HidePopup(void)
{
	WM_HideWindow(s_popUpUpgradeSoftware);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-softwareUpgradeScreen_UpgradeFW 								    		*/
/*                                                                                      */
/* Details  -Show confirm upgrade if check Result is no error		            		*/
/* 			-Show popup error if check Result is error									*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) CheckFWFileResult checkResult : - check result 						*/
/*						- range : 0-2													*/
/*                      - unit : NONE                                                   */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_isExistBoot : flag exist flie bootloader upgrade      		*/
/*                     (O) s_isExistFirm : flag exist flie SW upgrade				    */
/*                     (O) s_popUpUpgradeSoftware : popup			    				*/
/*                     (O) s_titlePopUp : title popup				    				*/
/*                     (O) s_closeBtn : close button				    				*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void softwareUpgradeScreen_UpgradeFW(CheckFWFileResult checkResult)
{
	switch (checkResult)
	{
	case eExistBootAndCTrlSW:
	{
		s_isExistBoot = true;
		s_isExistFirm = true;

		WM_HideWindow(s_popUpUpgradeSoftware);
		confirmUpgradeScreen_Show();
		break;
	}
	case eExistControlSW:
	{
		s_isExistBoot = false;
		s_isExistFirm = true;

		WM_HideWindow(s_popUpUpgradeSoftware);
		confirmUpgradeScreen_Show();
		break;
	}
	case eErrorFWFile:
	{
		WM_MoveChildTo(s_titlePopUp,0,30);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);
			TEXT_SetText(s_titlePopUp,"Upgrade failures\nCheck the connection\nPlease upgrade again");
		}
		else
		{
			guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontJapan3L16B2PP);
			TEXT_SetText(s_titlePopUp,"更新に失敗しました\n接続を確認して\n再度更新を行ってください");
		}
		WM_ShowWindow(s_closeBtn);
		softwareUpgradeScreen_HideAllItem();
		break;
	}
	default:
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-softwareUpgradeScreen_GetExistBootFile 								    */
/*                                                                                      */
/* Details  -Get flag exist boot file										            */
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_isExistBoot : flag exist file bootloader upgrade	        */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool softwareUpgradeScreen_GetExistBootFile(void)
{
	return s_isExistBoot;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-softwareUpgradeScreen_GetExistFirmFile 								    */
/*                                                                                      */
/* Details  -Get flag exist firmware file										        */
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : if exist is true, else is false		            			    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_isExistFirm : flag exist file SW upgrade			        */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool softwareUpgradeScreen_GetExistFirmFile(void)
{
	return s_isExistFirm;
}

/* end of files */
