/********************************************************************/
/*                                                                  */
/* File Name    : ConfirmUpgradeScreen.c                           	*/
/*                                                                  */
/* General      :The ConfirmUpgradeScreen screen allows user        */
/*                to confirm upgrade Main-CPU					    */
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
/*  #0001	   2017/8/25       Linh Nguyen(MV)  	modify	        */
/*     use watch dog reset instead of NVIC_SystemReset              */
/*  #0002	   2017/09/19      Viet Le  			modify	        */
/*     -Remove extern g_m4_filename,g_m0_filename,g_boot_filename  	*/
/*			(bug 1847)												*/
/*	   -Edit get file name bootloader(usb_getFirmwareFilesBoot),	*/
/*			 file name firmware(usb_getFirmwareFilesControlSW) in   */
/*			confirmUpgradeScreen_Show() function (bug 1847)			*/
/*  #0003	   2017/09/27      Viet Le  			modify	        */
/*		- Edit get file name bootloader, firmware in 				*/
/*			confirmUpgradeScreen_Show() function (bug 1847)			*/
/*  #0004	   2017/09/28      Viet Le  			modify	        */
/*		- Use true/false for this flag upgrade (bug 1914)			*/
/*  #0005	   2017/10/06      Viet Le  			modify	        */
/*		-Edit modeMgr_WatchDogReset()-> guiInterface_WatchDogReset()*/
/*			(bug 1957)												*/
/*  #0006	   2017/10/27      Viet Le 	    	        modify	    */
/*	  Edit : EDIT_SetFont()->guiTaskEDIT_SetFont(bug 1997)			*/
/*  #0007	   2017/11/23      Viet Le 	    	        modify	    */
/*  -Edit : check exist control SW , bootloader in 					*/
/*		confirmUpgradeScreen_Show() function (bug 2042)				*/
/********************************************************************/

#include <StringTools.h>
#include "SoftwareUpgradeScreen.h"
#include "ConfirmUpgradeScreen.h"
#include "EDIT.h"
#include "BUTTON.h"
#include "updateFlash.h"
#include "setting.h"
#include "GuiTask.h"
#include "ModeMgr.h"
#include "Font.h"
#include "MainScreen.h"
#include "TextLibrary.h"
#include "AlarmInfor.h"
#include "GuiInterface.h"
#include "UsbInterface.h"

//Define item id for confirm upgrade screen
enum{
	eCurrentVersionConfirmId,
	eNextVersionConfirmId,
	eOKBtnConfirmId,
	eCancelBtnConfirmId,
	eCurrentBootVersionConfirmId,
	eNextBootVersionConfirmId,
};

WM_HWIN g_confirmUpgradeScreen;

static int8_t s_fileNameVersion[SIZE_BUFFER_FIRMWARE];
static int8_t s_fileNameBootVersion[SIZE_BUFFER_FIRMWARE];

static BUTTON_Handle s_OKBtn;
static BUTTON_Handle s_cancelBtn;
static EDIT_Handle s_currentVersion;
static EDIT_Handle s_nextVersion;
static EDIT_Handle s_currentBootloaderVersion;
static EDIT_Handle s_nextBootloaderVersion;

/****************************************************************************************/
/*                                                                                      */
/* General 	-confirmUpgradeScreen_CallbackOKBtn 								        */
/*                                                                                      */
/* Details  -Call back function for OK button, handle event from 			        	*/
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
void confirmUpgradeScreen_CallbackOKBtn(WM_MESSAGE *pMsg)
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

		guiTask_SetFont(&GUI_FontEng3L16B2PP);
		GUI_DispStringHCenterAt("OK", Rect.x1/2, 17);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			if(softwareUpgradeScreen_GetExistBootFile() == false && softwareUpgradeScreen_GetExistFirmFile() == true)
			{
				__disable_irq();
				Chip_IAP_PreSectorForReadWrite(START_SECTOR, END_SECTOR,  IAP_FLASH_BANK_B);
				Chip_IAP_EraseSector(START_SECTOR, END_SECTOR, IAP_FLASH_BANK_B);
				__enable_irq();
			}

			if(softwareUpgradeScreen_GetExistBootFile() == true && softwareUpgradeScreen_GetExistFirmFile() == true)
			{
				//write EEPROM
				guiInterface_SetSettingValue(eUpgradeSettingId,true);
				guiInterface_SetSettingValue(eUpdateCtrlSWId,true);
			}

			guiInterface_WatchDogReset();
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
/* General 	-confirmUpgradeScreen_CallbackCancelBtn 								    */
/*                                                                                      */
/* Details  -Call back function for close button, handle event from 			        */
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
/*                     (O) s_fileNameVersion : file name upgrade control SW             */
/*                     (O) s_fileNameBootVersion : file name upgrade bootloader			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void confirmUpgradeScreen_CallbackCancelBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Cancel", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("キャンセル", Rect.x1/2, 19);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			confirmUpgradeScreen_Hide();

			for(int  i = 0; i < SIZE_BUFFER_FIRMWARE; i++)
			{
				s_fileNameVersion[i] = '\0';
				s_fileNameBootVersion[i] = '\0';
			}

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
/* General 	-confirmUpgradeScreen_Callback 								                */
/*                                                                                      */
/* Details  -Call back function for soft confirm upgrade screen, handle event from      */
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
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void confirmUpgradeScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(BUTTON_DIALOG_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,50);

		GUI_SetColor(BORDER_CONFIRM_UPGRADE_COLOR);
		GUI_AA_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,50);

		GUI_SetBkColor(BUTTON_DIALOG_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Upgrade confirmation", 400,5);
			GUI_DispStringHCenterAt("Is it OK?", 400,340);

			guiTask_SetFont(&GUI_FontEng3L14B2PP);
			GUI_DispStringAt("FH-310 control software", 60,105);
			GUI_DispStringAt("FH-310 boot loader", 110,215);
			GUI_DispStringAt("Current Version", 350,60);
			GUI_DispStringAt("Next Version", 570,60);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("ソフトウェア更新確認", 400,5);
			GUI_DispStringHCenterAt("よろしいですか？", 400,340);

			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringAt("FH-310 ソフトウェア", 92,105);
			GUI_DispStringAt("FH-310 ブートローダー", 90,215);
			GUI_DispStringAt("現在のバージョン", 350,60);
			GUI_DispStringAt("更新後のバージョン", 570,60);
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
/* General 	-confirmUpgradeScreen_Init 								    				*/
/*                                                                                      */
/* Details  -Initialize confirm upgrade screen			            					*/
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
/*                     (O) g_confirmUpgradeScreen : confirm upgrade screen              */
/*                     (O) s_currentVersion : display current version of control SW 	*/
/*                     (O) s_nextVersion : display next version of control SW           */
/*                     (O) s_currentBootloaderVersion : display current version         */
/*                                                      of bootloader                   */
/*                     (O) s_nextBootloaderVersion : display next version of bootloader */
/*                     (O) s_OKBtn : OK button           								*/
/*                     (O) s_cancelBtn : cancel button           						*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void confirmUpgradeScreen_Init(void)
{
	g_confirmUpgradeScreen = WM_CreateWindowAsChild(10, 10, 780, 460,g_softwareUpgradeScreen,WM_CF_HIDE| WM_CF_MEMDEV | WM_CF_LATE_CLIP,confirmUpgradeScreen_Callback,0);

	s_currentVersion = EDIT_CreateEx(350,90,180,50,g_confirmUpgradeScreen,WM_CF_SHOW,0,eCurrentVersionConfirmId,50);
	EDIT_SetFocussable(s_currentVersion,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_currentVersion ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_currentVersion ,GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_currentVersion,(char*)stringtools_GetVersionNotDate());

	s_nextVersion = EDIT_CreateEx(570,90,180,50,g_confirmUpgradeScreen,WM_CF_SHOW,0,eNextVersionConfirmId,50);
	EDIT_SetFocussable(s_nextVersion,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_nextVersion ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_nextVersion ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_currentBootloaderVersion = EDIT_CreateEx(350,200,180,50,g_confirmUpgradeScreen,WM_CF_SHOW,0,eCurrentBootVersionConfirmId,50);
	EDIT_SetFocussable(s_currentBootloaderVersion,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_currentBootloaderVersion ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_currentBootloaderVersion ,GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_currentBootloaderVersion,(char*)stringtools_GetBootVersionNotDate());

	s_nextBootloaderVersion = EDIT_CreateEx(570,200,180,50,g_confirmUpgradeScreen,WM_CF_SHOW,0,eNextBootVersionConfirmId,50);
	EDIT_SetFocussable(s_nextBootloaderVersion,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_nextBootloaderVersion ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_nextBootloaderVersion ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_OKBtn = BUTTON_CreateEx(100, 390, 210, 60, g_confirmUpgradeScreen, WM_CF_SHOW, 0, eOKBtnConfirmId);
	WM_SetCallback(s_OKBtn,confirmUpgradeScreen_CallbackOKBtn);

	s_cancelBtn = BUTTON_CreateEx(470, 390, 210, 60, g_confirmUpgradeScreen, WM_CF_SHOW, 0, eCancelBtnConfirmId);
	WM_SetCallback(s_cancelBtn,confirmUpgradeScreen_CallbackCancelBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-confirmUpgradeScreen_Show 								    				*/
/*                                                                                      */
/* Details  -Show confirm upgrade screen			            						*/
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
/* Using Global Data : (I) s_fileNameVersion : file name upgrade control SW				*/
/*					   (I) s_fileNameBootVersion : file name upgrade bootloader			*/
/*                     (O) s_nextBootloaderVersion : display next version bootloader    */
/*                     (O) s_nextVersion : display next version control SW				*/
/*                     (O) g_confirmUpgradeScreen : confirm upgrade screen				*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void confirmUpgradeScreen_Show(void)
{
	int8_t firstVersion[SIZE_FIRST_VERSION_BUFFER] = {'\0'};
	int8_t lastVersion[SIZE_LAST_VERSION_BUFFER] = {'\0'};

	int8_t firstBootVersion[SIZE_FIRST_VERSION_BUFFER] = {'\0'};
	int8_t lastBootVersion[SIZE_LAST_VERSION_BUFFER] = {'\0'};

	int8_t bootFilename[4*SIZE_BUFFER_FIRMWARE] = {0};
	int8_t firmwareFilenameM4[4*SIZE_BUFFER_FIRMWARE] = {0};
	int8_t firmwareFilenameM0[4*SIZE_BUFFER_FIRMWARE] = {0};

	if((usb_getFirmwareFilesM0(firmwareFilenameM0) == true) &&
			(usb_getFirmwareFilesM4(firmwareFilenameM4) == true))
	{
		//M4
		if(softwareUpgradeScreen_GetExistFirmFile() == true)
		{
			for(int i = START_POSITION_FIRST_VERSION_CONTROL; i < END_POSITION_FIRST_VERSION_CONTROL; i++)
			{
				firstVersion[i - START_POSITION_FIRST_VERSION_CONTROL] = firmwareFilenameM4[i];
			}

			for(int i = START_POSITION_LAST_VERSION_CONTROL; i < END_POSITION_LAST_VERSION_CONTROL; i++)
			{
				lastVersion[i - START_POSITION_LAST_VERSION_CONTROL] = firmwareFilenameM4[i];
			}

			strncat((char*)s_fileNameVersion, (char*)firstVersion,SIZE_FIRST_VERSION_BUFFER);
			strcat((char*)s_fileNameVersion, " ");
			strncat((char*)s_fileNameVersion, (char*)lastVersion,SIZE_LAST_VERSION_BUFFER);

			EDIT_SetText(s_nextVersion ,(char*)s_fileNameVersion);
		}

		//Boot
		if(usb_getFirmwareFilesBoot(bootFilename) == true)
		{
			if(softwareUpgradeScreen_GetExistBootFile() == true)
			{
				for(int i = START_POSITION_FIRST_VERSION_BOOT; i < END_POSITION_FIRST_VERSION_BOOT; i++)
				{
					firstBootVersion[i - START_POSITION_FIRST_VERSION_BOOT] = bootFilename[i];
				}

				for(int i = START_POSITION_LAST_VERSION_BOOT; i < END_POSITION_LAST_VERSION_BOOT; i++)
				{
					lastBootVersion[i - START_POSITION_LAST_VERSION_BOOT] = bootFilename[i];
				}

				strncat((char*)s_fileNameBootVersion, (char*)firstBootVersion,SIZE_FIRST_VERSION_BUFFER);
				strcat((char*)s_fileNameBootVersion, " ");
				strncat((char*)s_fileNameBootVersion, (char*)lastBootVersion,SIZE_LAST_VERSION_BUFFER);

				EDIT_SetText(s_nextBootloaderVersion,(char*)s_fileNameBootVersion);
			}
		}

		WM_ShowWindow(g_confirmUpgradeScreen);
		WM_SetFocus(g_confirmUpgradeScreen);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-confirmUpgradeScreen_Hide 								    				*/
/*                                                                                      */
/* Details  -Hide confirm upgrade screen			            						*/
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
/*                     (O) s_nextBootloaderVersion : display next version bootloader    */
/*                     (O) g_confirmUpgradeScreen : confirm upgrade screen 				*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void confirmUpgradeScreen_Hide(void)
{
	EDIT_SetText(s_nextBootloaderVersion,"");
	WM_HideWindow(g_confirmUpgradeScreen);

	return;
}

/* end of files */
