/********************************************************************/
/*                                                                  */
/* File Name    : DevelopmentSettingScreenPageFour.c              	*/
/*                                                                  */
/* General       : The user is able to dump file into USB memory	*/
/*  																*/
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
/* #0001       2017/09/05      Viet Le				modify			*/
/*      remove #include "assert.h"                                  */
/* #0002       2017/09/16      Viet Le				modify			*/
/*     - Add hide all item when show popup							*/
/*		(developmentSettingScreenPageFour_HideAllItem())			*/
/*     - Add show all item when hide popup							*/
/*		(developmentSettingScreenPageFour_ShowAllItem())			*/
/*  #0003	   2017/10/06      Viet Le  			modify	        */
/*		-Edit modeMgr_WatchDogReset()-> guiInterface_WatchDogReset()*/
/*			(bug 1957)												*/
/*  #0004	   2017/10/27      Viet Le 	    	        modify	    */
/*	  Edit : RADIO_SetFont()->guiTaskRADIO_SetFont; TEXT_SetFont()->*/
/*		guiTaskTEXT_SetFont() (bug 1997)							*/
/********************************************************************/
#include "math.h"
#include <EEPROM.h>
#include "DevelopmentSettingScreenPageFour.h"
#include "MainScreen.h"
#include "BUTTON.h"
#include "setting.h"
#include "GuiTask.h"
#include "TEXT.h"
#include "ModeMgr.h"
#include "Font.h"
#include "util.h"
#include "MaintenanceScreen.h"
#include "DevelopmentSettingScreenPageFive.h"
#include "DevelopmentSettingScreenPageThree.h"
#include "OptionScreen.h"
#include "AlarmInfor.h"
#include "RADIO.h"
#include "TextLibrary.h"
#include "UsbInterface.h"
#include "GuiInterface.h"

//Define item id
enum{
	eBackBtnInDevelopmentPageFourId,
	eHomeBtnInDevelopmentPageFourId,
	eNextBtnInDevelopmentPageFourId,
	ePrevBtnInDevelopmentPageFourId,
	eEEPROMBtnInDevelopmentPageFourId,
	eCBXBtnInDevelopmentPageFourId,
	eAnyPalBtnInDevelopmentPageFourId,
	eSpo2PulseBtnInDevelopmentPageFourId,
	ePageTextInDevelopmentPageFourId,
	eTitlePopUpInDevelopmentPageFourStringID,
	eOkBtnPopUpInDevelopmentPageFourStringId,
	eCancelBtnPopUpInDevelopmentPageFourStringId,
	eClosetnPopUpInDevelopmentPageFourStringId,
	eBitZeroValueZeroId,
	eBitZeroValueOneId,
	eBitOneValueZeroId,
	eBitOneValueOneId,
	eBitTwoValueZeroId,
	eBitTwoValueOneId,
	eBitThreeValueZeroId,
	eBitThreeValueOneId,
	eBitFourValueZeroId,
	eBitFourValueOneId,
	eBitFiveValueZeroId,
	eBitFiveValueOneId,
	eBitSixValueZeroId,
	eBitSixValueOneId,
	eBitSevenValueZeroId,
	eBitSevenValueOneId,
	eTest1BtnId,
	eTest2BtnId
};

//Define upload id
typedef enum{
	eNotUpLoad,
	eUpLoadEEPROM,
	eUpLoadCBX,
	eUpLoadAnypal,
	eUpLoadPulseO2
}E_Upload_File;

WM_HWIN g_developmentSettingScreenPageFour;

static bool s_uploadEEPROM = false;
static WM_HWIN s_popUp;
static bool s_resetSystem = false;
static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static BUTTON_Handle s_nextBtn;
static BUTTON_Handle s_prevBtn;
static BUTTON_Handle s_EEPROMBtn;
static BUTTON_Handle s_CBXBtn;
static BUTTON_Handle s_AnypalBtn;
static BUTTON_Handle s_Spo2PulseBtn;
static TEXT_Handle s_titlePopUp;
static BUTTON_Handle s_OKBtn;
static BUTTON_Handle s_cancelBtn;
static BUTTON_Handle s_closeBtn;
static BUTTON_Handle s_test1Btn;
static BUTTON_Handle s_test2Btn;
static TEXT_Handle s_pageText;
static E_Upload_File s_uploadFile = eNotUpLoad;

static RADIO_Handle s_bitZeroValueZero;
static RADIO_Handle s_bitZeroValueOne;

static RADIO_Handle s_bitOneValueZero;
static RADIO_Handle s_bitOneValueOne;

static RADIO_Handle s_bitTwoValueZero;
static RADIO_Handle s_bitTwoValueOne;

static RADIO_Handle s_bitThreeValueZero;
static RADIO_Handle s_bitThreeValueOne;

static RADIO_Handle s_bitFourValueZero;
static RADIO_Handle s_bitFourValueOne;

static RADIO_Handle s_bitFiveValueZero;
static RADIO_Handle s_bitFiveValueOne;

static RADIO_Handle s_bitSixValueZero;
static RADIO_Handle s_bitSixValueOne;

static RADIO_Handle s_bitSevenValueZero;
static RADIO_Handle s_bitSevenValueOne;

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
/* General 	-developmentSettingScreenPageFour_HideAllItem	                            */
/*                                                                                      */
/* Details  -Hide all item when show pop up												*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															  		*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_backBtn : back button                                      */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_nextBtn : next button						                */
/*                     (O) s_prevBtn : previous button						            */
/*                     (O) s_EEPROMBtn : eeprom button						            */
/*                     (O) s_CBXBtn : CBX button						                */
/*                     (O) s_AnypalBtn : Anypal button						            */
/*                     (O) s_Spo2PulseBtn : Spo2Pulse button						    */
/*                     (O) s_pageText : page number						                */
/*                     (O) g_developmentSettingScreenPageFour : development page four	*/
/*                     (O) s_bitZeroValueZero : bit zero (0) button						*/
/*                     (O) s_bitZeroValueOne : bit zero (1)	button					    */
/*                     (O) s_bitOneValueZero :  bit one (0) button						*/
/*                     (O) s_bitOneValueOne : bit one (1) button						*/
/*                     (O) s_bitTwoValueZero : bit two (0) button						*/
/*                     (O) s_bitTwoValueOne : bit two (1) button						*/
/*                     (O) s_bitThreeValueZero : bit three (0) button					*/
/*                     (O) s_bitThreeValueOne : bit three (1) button					*/
/*                     (O) s_bitFourValueZero : bit four (0) button						*/
/*                     (O) s_bitFourValueOne : bit four (1) button						*/
/*                     (O) s_bitFiveValueZero : bit five (0) button						*/
/*                     (O) s_bitFiveValueOne : bit five (1) button						*/
/*                     (O) s_bitSixValueZero : bit six (0) button						*/
/*                     (O) s_bitSixValueOne : bit six (1) button						*/
/*                     (O) s_bitSevenValueZero : bit seven (0) button					*/
/*                     (O) s_bitSevenValueOne : bit seven (1) button					*/
/*                     (O) s_test1Btn : flow test 1 button								*/
/*                     (O) s_test2Btn : flow test 2 button								*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFour_HideAllItem(void)
{
	WM_HideWindow(s_backBtn);
	WM_HideWindow(s_homeBtn);
	WM_HideWindow(s_nextBtn);
	WM_HideWindow(s_prevBtn);

	WM_HideWindow(s_EEPROMBtn);
	WM_HideWindow(s_CBXBtn);
	WM_HideWindow(s_AnypalBtn);
	WM_HideWindow(s_Spo2PulseBtn);
	WM_HideWindow(s_pageText);

	WM_HideWindow(s_bitZeroValueZero);
	WM_HideWindow(s_bitZeroValueOne);
	WM_HideWindow(s_bitOneValueZero);
	WM_HideWindow(s_bitOneValueOne);
	WM_HideWindow(s_bitTwoValueZero);
	WM_HideWindow(s_bitTwoValueOne);
	WM_HideWindow(s_bitThreeValueZero);
	WM_HideWindow(s_bitThreeValueOne);
	WM_HideWindow(s_bitFourValueZero);
	WM_HideWindow(s_bitFourValueOne);
	WM_HideWindow(s_bitFiveValueZero);
	WM_HideWindow(s_bitFiveValueOne);
	WM_HideWindow(s_bitSixValueZero);
	WM_HideWindow(s_bitSixValueOne);
	WM_HideWindow(s_bitSevenValueZero);
	WM_HideWindow(s_bitSevenValueOne);

	WM_HideWindow(s_test1Btn);
	WM_HideWindow(s_test2Btn);

	WM_Paint(g_developmentSettingScreenPageFour);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFour_ShowAllItem	                            */
/*                                                                                      */
/* Details  -Show all item when hide pop up												*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															  		*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_backBtn : back button                                      */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_nextBtn : next button						                */
/*                     (O) s_prevBtn : previous button						            */
/*                     (O) s_EEPROMBtn : eeprom button						            */
/*                     (O) s_CBXBtn : CBX button						                */
/*                     (O) s_AnypalBtn : Anypal button						            */
/*                     (O) s_Spo2PulseBtn : Spo2Pulse button						    */
/*                     (O) s_pageText : page number						                */
/*                     (O) g_developmentSettingScreenPageFour : development page four	*/
/*                     (O) s_bitZeroValueZero : bit zero (0) button						*/
/*                     (O) s_bitZeroValueOne : bit zero (1)	button					    */
/*                     (O) s_bitOneValueZero :  bit one (0) button						*/
/*                     (O) s_bitOneValueOne : bit one (1) button						*/
/*                     (O) s_bitTwoValueZero : bit two (0) button						*/
/*                     (O) s_bitTwoValueOne : bit two (1) button						*/
/*                     (O) s_bitThreeValueZero : bit three (0) button					*/
/*                     (O) s_bitThreeValueOne : bit three (1) button					*/
/*                     (O) s_bitFourValueZero : bit four (0) button						*/
/*                     (O) s_bitFourValueOne : bit four (1) button						*/
/*                     (O) s_bitFiveValueZero : bit five (0) button						*/
/*                     (O) s_bitFiveValueOne : bit five (1) button						*/
/*                     (O) s_bitSixValueZero : bit six (0) button						*/
/*                     (O) s_bitSixValueOne : bit six (1) button						*/
/*                     (O) s_bitSevenValueZero : bit seven (0) button					*/
/*                     (O) s_bitSevenValueOne : bit seven (1) button					*/
/*                     (O) s_test1Btn : flow test 1 button								*/
/*                     (O) s_test2Btn : flow test 2 button								*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFour_ShowAllItem(void)
{
	WM_ShowWindow(s_backBtn);
	WM_ShowWindow(s_homeBtn);
	WM_ShowWindow(s_nextBtn);
	WM_ShowWindow(s_prevBtn);

	WM_ShowWindow(s_EEPROMBtn);
	WM_ShowWindow(s_CBXBtn);
	WM_ShowWindow(s_AnypalBtn);
	WM_ShowWindow(s_Spo2PulseBtn);
	WM_ShowWindow(s_pageText);

	WM_ShowWindow(s_bitZeroValueZero);
	WM_ShowWindow(s_bitZeroValueOne);
	WM_ShowWindow(s_bitOneValueZero);
	WM_ShowWindow(s_bitOneValueOne);
	WM_ShowWindow(s_bitTwoValueZero);
	WM_ShowWindow(s_bitTwoValueOne);
	WM_ShowWindow(s_bitThreeValueZero);
	WM_ShowWindow(s_bitThreeValueOne);
	WM_ShowWindow(s_bitFourValueZero);
	WM_ShowWindow(s_bitFourValueOne);
	WM_ShowWindow(s_bitFiveValueZero);
	WM_ShowWindow(s_bitFiveValueOne);
	WM_ShowWindow(s_bitSixValueZero);
	WM_ShowWindow(s_bitSixValueOne);
	WM_ShowWindow(s_bitSevenValueZero);
	WM_ShowWindow(s_bitSevenValueOne);

	WM_ShowWindow(s_test1Btn);
	WM_ShowWindow(s_test2Btn);

	WM_Paint(g_developmentSettingScreenPageFour);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFour_ShowPopup	                            	*/
/*                                                                                      */
/* Details  -Show pop up when press button												*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															  		*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_titlePopUp : title pop up                                  */
/*                     (O) s_closeBtn : close button						            */
/*                     (O) s_OKBtn : OK button						                    */
/*                     (O) s_cancelBtn : cancel button						            */
/*                     (O) s_popUp : pop up						                        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFour_ShowPopup(void)
{
	WM_MoveChildTo(s_titlePopUp,0,15);

	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);
		TEXT_SetText(s_titlePopUp,"Read the log data\nAfter connecting the USB memory\nPlease press the load button");
	}
	else
	{
		guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontJapan3L16B2PP);
		TEXT_SetText(s_titlePopUp,"ログデータを読み込みます\nUSBメモリーを接続してから\n読み込みボタンを押してください");
	}

	WM_HideWindow(s_closeBtn);

	WM_ShowWindow(s_OKBtn);
	WM_ShowWindow(s_cancelBtn);

	WM_ShowWindow(s_popUp);
	WM_BringToTop(s_popUp);

	developmentSettingScreenPageFour_HideAllItem();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFour_Callback	                                */
/*                                                                                      */
/* Details  -call back function for PSA parameter page Four, handle event from window	*/
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
void developmentSettingScreenPageFour_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		if(!WM_IsVisible(s_popUp))
		{
			GUI_SetColor(GUI_BLACK);
			GUI_AA_DrawRoundedRect(10,60,320,360,10);
			GUI_AA_DrawRoundedRect(350,60,780,390,10);

			guiTask_SetFont(&GUI_FontEng3L12B2PP);
			GUI_DispStringAt("DATA",400,80);
			GUI_DispStringAt("0",390,100);
			GUI_DispStringAt("1",450,100);
			GUI_DispStringAt("bit0",480,130);
			GUI_DispStringAt("bit1",480,160);
			GUI_DispStringAt("bit2",480,190);
			GUI_DispStringAt("bit3",480,220);
			GUI_DispStringAt("bit4",480,250);
			GUI_DispStringAt("bit5",480,280);
			GUI_DispStringAt("bit6",480,310);
			GUI_DispStringAt("bit7",480,340);
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				guiTask_SetFont(&GUI_FontEng3L16B2PP);
				GUI_DispStringHCenterAt("Development setting", 400,5);
				guiTask_SetFont(&GUI_FontEng3L12B2PP);
				GUI_DispStringAt("Memory upload",25,50);
				GUI_DispStringAt("Flow controller",510,50);
			}
			else
			{
				guiTask_SetFont(&GUI_FontJapan3L18B2PP);
				GUI_DispStringHCenterAt("開発用設定", 400,5);
				guiTask_SetFont(&GUI_FontJapan3L12B2PP);
				GUI_DispStringAt("メモリアップロード",25,50);
				GUI_DispStringAt("フローコントローラ",510,50);
			}
		}
		break;
	case WM_TOUCH:
	case WM_TOUCH_CHILD:
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFour_CallbackBackBtn	                        */
/*                                                                                      */
/* Details  -call back function for back button											*/
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
void developmentSettingScreenPageFour_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			developmentSettingScreenPageFour_Hide();
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
/* General 	-developmentSettingScreenPageFour_CallbackHomeBtn 						    */
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
void developmentSettingScreenPageFour_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			developmentSettingScreenPageFour_Hide();
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
/* General 	-developmentSettingScreenPageFour_ValueFowController 						*/
/*                                                                                      */
/* Details  -Calculator value flow send to M0											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I)	NONE					 											*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint8_t : value send to flow controller		                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_bitZeroValueZero : bit zero button							*/
/*                     (I) s_bitOneValueZero : bit one button                           */
/*                     (I) s_bitTwoValueZero : bit two button                           */
/*                     (I) s_bitThreeValueZero : bit three button                       */
/*                     (I) s_bitFourValueZero : bit four button                         */
/*                     (I) s_bitFiveValueZero : bit five button                         */
/*                     (I) s_bitSixValueZero : bit six button                           */
/*                     (I) s_bitSevenValueZero : bit seven button                       */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
uint8_t developmentSettingScreenPageFour_ValueFowController(void)
{
	uint8_t result = 0;

	if(RADIO_GetValue(s_bitZeroValueZero) >= 0)
	{
		result = result + VALUE_0 * pow(BASIS,BIT_0);
	}
	else
	{
		result = result + VALUE_1 * pow(BASIS,BIT_0);
	}

	if(RADIO_GetValue(s_bitOneValueZero) >= 0)
	{
		result = result + VALUE_0 * pow(BASIS,BIT_1);
	}
	else
	{
		result = result + VALUE_1 * pow(BASIS,BIT_1);
	}

	if(RADIO_GetValue(s_bitTwoValueZero) >= 0)
	{
		result = result + VALUE_0 * pow(BASIS,BIT_2);
	}
	else
	{
		result = result + VALUE_1 * pow(BASIS,BIT_2);
	}

	if(RADIO_GetValue(s_bitThreeValueZero) >= 0)
	{
		result = result + VALUE_0 * pow(BASIS,BIT_3);
	}
	else
	{
		result = result + VALUE_1 * pow(BASIS,BIT_3);
	}

	if(RADIO_GetValue(s_bitFourValueZero) >= 0)
	{
		result = result + VALUE_0 * pow(BASIS,BIT_4);
	}
	else
	{
		result = result + VALUE_1 * pow(BASIS,BIT_4);
	}

	if(RADIO_GetValue(s_bitFiveValueZero) >= 0)
	{
		result = result + VALUE_0 * pow(BASIS,BIT_5);
	}
	else
	{
		result = result + VALUE_1 * pow(BASIS,BIT_5);
	}

	if(RADIO_GetValue(s_bitSixValueZero) >= 0)
	{
		result = result + VALUE_0 * pow(BASIS,BIT_6);
	}
	else
	{
		result = result + VALUE_1 * pow(BASIS,BIT_6);
	}

	if(RADIO_GetValue(s_bitSevenValueZero) >= 0)
	{
		result = result + VALUE_0 * pow(BASIS,BIT_7);
	}
	else
	{
		result = result + VALUE_1 * pow(BASIS,BIT_7);
	}

	return result;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFour_CallbackTest1Btn 						    */
/*                                                                                      */
/* Details  -Call back function for test1 button, handle event from 			        */
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
void developmentSettingScreenPageFour_CallbackTest1Btn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Test1", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("テスト1", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			guiTask_SendPendingEvent(eFlowControllerTest1Id,(uint16_t)developmentSettingScreenPageFour_ValueFowController());
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
/* General 	-developmentSettingScreenPageFour_CallbackTest2Btn 						    */
/*                                                                                      */
/* Details  -Call back function for test2 button, handle event from 			        */
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
void developmentSettingScreenPageFour_CallbackTest2Btn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Test2", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("テスト2", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			guiTask_SendPendingEvent(eFlowControllerTest2Id,(uint16_t)developmentSettingScreenPageFour_ValueFowController());
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
/* General 	-developmentSettingScreenPageFour_CallbackNextBtn 							*/
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
void developmentSettingScreenPageFour_CallbackNextBtn(WM_MESSAGE *pMsg)
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
			developmentSettingScreenPageFour_Hide();
			developmentSettingScreenPageFive_Show();
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
/* General 	-developmentSettingScreenPageFour_CallbackPrevBtn 							*/
/*                                                                                      */
/* Details  -Call back function for prev button, handle event from 			            */
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
void developmentSettingScreenPageFour_CallbackPrevBtn(WM_MESSAGE *pMsg)
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
			developmentSettingScreenPageFour_Hide();
			developmentSettingScreenPageThree_Show();
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
/* General 	-developmentSettingScreenPageFour_CallbackEEPROMBtn 						*/
/*                                                                                      */
/* Details  -Call back function for EEPROM button, handle event from 			        */
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
/*                     (O) s_uploadFile : current upload file                           */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFour_CallbackEEPROMBtn(WM_MESSAGE *pMsg)
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

		guiTask_SetFont(&GUI_FontEng3L16B2PP);
		GUI_DispStringHCenterAt("EEPROM", Rect.x1/2, 19);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_uploadFile = eUpLoadEEPROM;
			developmentSettingScreenPageFour_ShowPopup();
		}
		break;
	case WM_TOUCH:
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFour_CallbackCBXBtn 							*/
/*                                                                                      */
/* Details  -Call back function for CBX button, handle event from 			       		*/
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
/*                     (O) s_uploadFile : current upload file                           */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFour_CallbackCBXBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Usage log (CBX)", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("使用ログ(CBX)", Rect.x1/2, 19);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_uploadFile = eUpLoadCBX;
			developmentSettingScreenPageFour_ShowPopup();
		}
		break;
	case WM_TOUCH:
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFour_CallbackAnypalBtn 						*/
/*                                                                                      */
/* Details  -Call back function for Anypal button, handle event from 			        */
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
/*                     (O) s_uploadFile : current upload file                           */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFour_CallbackAnypalBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Usage log (Anypal)", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("使用ログ(Anypal)", Rect.x1/2, 19);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_uploadFile = eUpLoadAnypal;
			developmentSettingScreenPageFour_ShowPopup();
		}
		break;
	case WM_TOUCH:
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFour_CallbackSpo2PulseBtn 						*/
/*                                                                                      */
/* Details  -Call back function for Spo2 pulse button, handle event from 			    */
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
/*                     (O) s_uploadFile : current upload file                           */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFour_CallbackSpo2PulseBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Spo2/Pulse", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("血中酸素/脈拍", Rect.x1/2, 19);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_uploadFile = eUpLoadPulseO2;
			developmentSettingScreenPageFour_ShowPopup();
		}
		break;
	case WM_TOUCH:
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFour_CallbackPopUp 							*/
/*                                                                                      */
/* Details  -Call back function for pop up, handle event from 							*/
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
void developmentSettingScreenPageFour_CallbackPopUp(WM_MESSAGE *pMsg)
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
/* General 	-developmentSettingScreenPageFour_CallbackOKBtn 							*/
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
/*                     (O) s_OKBtn : OK button                                          */
/*                     (O) s_cancelBtn : cancel button						            */
/*                     (O) s_closeBtn : close button						            */
/*                     (O) s_titlePopUp : title pop up						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFour_CallbackOKBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Load", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("読み込み", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			WM_HideWindow(s_OKBtn);
			WM_HideWindow(s_cancelBtn);
			WM_HideWindow(s_closeBtn);

			WM_MoveChildTo(s_titlePopUp,0,85);
			TEXT_SetTextAlign(s_titlePopUp, GUI_TA_VCENTER | GUI_TA_HCENTER);

			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);
				TEXT_SetText(s_titlePopUp,"Loading log data");
			}
			else
			{
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontJapan3L16B2PP);
				TEXT_SetText(s_titlePopUp,"ログデータを読み込んでいます");
			}
			GUI_Delay(GUI_DELAY_DEVELOP_PAGE_FOUR);
			developmentSettingScreenPageFour_UploadToUSB();
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
/* General 	-developmentSettingScreenPageFour_CallbackCancelBtn 						*/
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
/*                     (O) s_popUp : pop up                                           	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFour_CallbackCancelBtn(WM_MESSAGE *pMsg)
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
			WM_HideWindow(s_popUp);

			developmentSettingScreenPageFour_ShowAllItem();
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
/* General 	-developmentSettingScreenPageFour_CallbackCloseBtn 							*/
/*                                                                                      */
/* Details  -Call back function for Close button, handle event from 					*/
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
/* Using Global Data : (I) s_uploadEEPROM : flag upload EEPROM							*/
/*					   (I) s_resetSystem : flag reset system							*/
/*                     (O) s_popUp : pop up                                           	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFour_CallbackCloseBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Close", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("閉じる", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			if(s_uploadEEPROM == true)
			{
				if(s_resetSystem == true)
				{
					setting_RestoreSetting();
					guiInterface_WatchDogReset();
				}
			}

			WM_HideWindow(s_popUp);
			developmentSettingScreenPageFour_ShowAllItem();
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
/* General 	-developmentSettingScreenPageFour_Init										*/
/*                                                                                      */
/* Details  -Initialize development setting screen page Four	            		    */
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
/*                     (O) g_developmentSettingScreenPageFour : development page four   */
/*                     (O) s_EEPROMBtn : eeprom button						            */
/*                     (O) s_CBXBtn : CBX button						            	*/
/*                     (O) s_AnypalBtn : Anypal button						            */
/*                     (O) s_Spo2PulseBtn : Spo2Pulse button						    */
/*                     (O) s_bitZeroValueZero : bit zero (0) button						*/
/*                     (O) s_bitZeroValueOne : bit zero (1)	button					    */
/*                     (O) s_bitOneValueZero :  bit one (0) button						*/
/*                     (O) s_bitOneValueOne : bit one (1) button						*/
/*                     (O) s_bitTwoValueZero : bit two (0) button						*/
/*                     (O) s_bitTwoValueOne : bit two (1) button						*/
/*                     (O) s_bitThreeValueZero : bit three (0) button					*/
/*                     (O) s_bitThreeValueOne : bit three (1) button					*/
/*                     (O) s_bitFourValueZero : bit four (0) button						*/
/*                     (O) s_bitFourValueOne : bit four (1) button						*/
/*                     (O) s_bitFiveValueZero : bit five (0) button						*/
/*                     (O) s_bitFiveValueOne : bit five (1) button						*/
/*                     (O) s_bitSixValueZero : bit six (0) button						*/
/*                     (O) s_bitSixValueOne : bit six (1) button						*/
/*                     (O) s_bitSevenValueZero : bit seven (0) button					*/
/*                     (O) s_bitSevenValueOne : bit seven (1) button					*/
/*                     (O) s_test1Btn : flow test 1 button								*/
/*                     (O) s_test2Btn : flow test 2 button								*/
/*                     (O) s_backBtn : back button										*/
/*                     (O) s_homeBtn : home button										*/
/*                     (O) s_nextBtn : next button										*/
/*                     (O) s_prevBtn : previous button									*/
/*                     (O) s_pageText : page number										*/
/*                     (O) s_popUp : pop up												*/
/*                     (O) s_titlePopUp : title pop up									*/
/*                     (O) s_OKBtn : OK button											*/
/*                     (O) s_cancelBtn : cancel button									*/
/*                     (O) s_closeBtn : close button									*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFour_Init(void)
{
	g_developmentSettingScreenPageFour = WM_CreateWindowAsChild(0, 0, 800, 480,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,developmentSettingScreenPageFour_Callback,0);

	s_EEPROMBtn = BUTTON_CreateEx(30, 80, 250, 60, g_developmentSettingScreenPageFour, WM_CF_SHOW, 0, eEEPROMBtnInDevelopmentPageFourId);
	WM_SetCallback(s_EEPROMBtn,developmentSettingScreenPageFour_CallbackEEPROMBtn);

	s_CBXBtn = BUTTON_CreateEx(30, 150, 250, 60, g_developmentSettingScreenPageFour, WM_CF_SHOW, 0, eCBXBtnInDevelopmentPageFourId);
	WM_SetCallback(s_CBXBtn,developmentSettingScreenPageFour_CallbackCBXBtn);

	s_AnypalBtn = BUTTON_CreateEx(30, 220, 250, 60, g_developmentSettingScreenPageFour, WM_CF_SHOW, 0, eAnyPalBtnInDevelopmentPageFourId);
	WM_SetCallback(s_AnypalBtn,developmentSettingScreenPageFour_CallbackAnypalBtn);

	s_Spo2PulseBtn = BUTTON_CreateEx(30, 290, 250, 60, g_developmentSettingScreenPageFour, WM_CF_SHOW, 0, eSpo2PulseBtnInDevelopmentPageFourId);
	WM_SetCallback(s_Spo2PulseBtn,developmentSettingScreenPageFour_CallbackSpo2PulseBtn);

	//Bit zero
	s_bitZeroValueZero = RADIO_CreateEx(388,130,50,30,g_developmentSettingScreenPageFour,WM_CF_SHOW,0,eBitZeroValueZeroId,1,35);
	guiTaskRADIO_SetFont(s_bitZeroValueZero,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_bitZeroValueZero,"",0);
	RADIO_SetFocusColor(s_bitZeroValueZero,SERVICE_BACKGROUND_COLOR);

	s_bitZeroValueOne = RADIO_CreateEx(448,130,50,30,g_developmentSettingScreenPageFour,WM_CF_SHOW,0,eBitZeroValueOneId,1,35);
	guiTaskRADIO_SetFont(s_bitZeroValueOne,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_bitZeroValueOne,"",0);
	RADIO_SetFocusColor(s_bitZeroValueOne,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_bitZeroValueZero, 1);
	RADIO_SetGroupId(s_bitZeroValueOne, 1);

	//Bit One
	s_bitOneValueZero = RADIO_CreateEx(388,160,50,30,g_developmentSettingScreenPageFour,WM_CF_SHOW,0,eBitOneValueZeroId,1,35);
	guiTaskRADIO_SetFont(s_bitOneValueZero,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_bitOneValueZero,"",0);
	RADIO_SetFocusColor(s_bitOneValueZero,SERVICE_BACKGROUND_COLOR);

	s_bitOneValueOne = RADIO_CreateEx(448,160,50,30,g_developmentSettingScreenPageFour,WM_CF_SHOW,0,eBitOneValueOneId,1,35);
	guiTaskRADIO_SetFont(s_bitOneValueOne,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_bitOneValueOne,"",0);
	RADIO_SetFocusColor(s_bitOneValueOne,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_bitOneValueZero, 2);
	RADIO_SetGroupId(s_bitOneValueOne, 2);

	//Bit two
	s_bitTwoValueZero = RADIO_CreateEx(388,190,50,30,g_developmentSettingScreenPageFour,WM_CF_SHOW,0,eBitTwoValueZeroId,1,35);
	guiTaskRADIO_SetFont(s_bitTwoValueZero,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_bitTwoValueZero,"",0);
	RADIO_SetFocusColor(s_bitTwoValueZero,SERVICE_BACKGROUND_COLOR);

	s_bitTwoValueOne = RADIO_CreateEx(448,190,50,30,g_developmentSettingScreenPageFour,WM_CF_SHOW,0,eBitTwoValueOneId,1,35);
	guiTaskRADIO_SetFont(s_bitTwoValueOne,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_bitTwoValueOne,"",0);
	RADIO_SetFocusColor(s_bitTwoValueOne,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_bitTwoValueZero, 3);
	RADIO_SetGroupId(s_bitTwoValueOne, 3);

	//Bit three
	s_bitThreeValueZero = RADIO_CreateEx(388,220,50,30,g_developmentSettingScreenPageFour,WM_CF_SHOW,0,eBitThreeValueZeroId,1,35);
	guiTaskRADIO_SetFont(s_bitThreeValueZero,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_bitThreeValueZero,"",0);
	RADIO_SetFocusColor(s_bitThreeValueZero,SERVICE_BACKGROUND_COLOR);

	s_bitThreeValueOne = RADIO_CreateEx(448,220,50,30,g_developmentSettingScreenPageFour,WM_CF_SHOW,0,eBitThreeValueOneId,1,35);
	guiTaskRADIO_SetFont(s_bitThreeValueOne,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_bitThreeValueOne,"",0);
	RADIO_SetFocusColor(s_bitThreeValueOne,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_bitThreeValueZero, 4);
	RADIO_SetGroupId(s_bitThreeValueOne, 4);

	//Bit four
	s_bitFourValueZero = RADIO_CreateEx(388,250,50,30,g_developmentSettingScreenPageFour,WM_CF_SHOW,0,eBitFourValueZeroId,1,35);
	guiTaskRADIO_SetFont(s_bitFourValueZero,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_bitFourValueZero,"",0);
	RADIO_SetFocusColor(s_bitFourValueZero,SERVICE_BACKGROUND_COLOR);

	s_bitFourValueOne = RADIO_CreateEx(448,250,50,30,g_developmentSettingScreenPageFour,WM_CF_SHOW,0,eBitFourValueOneId,1,35);
	guiTaskRADIO_SetFont(s_bitFourValueOne,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_bitFourValueOne,"",0);
	RADIO_SetFocusColor(s_bitFourValueOne,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_bitFourValueZero, 5);
	RADIO_SetGroupId(s_bitFourValueOne, 5);

	//Bit five
	s_bitFiveValueZero = RADIO_CreateEx(388,280,50,30,g_developmentSettingScreenPageFour,WM_CF_SHOW,0,eBitFiveValueZeroId,1,35);
	guiTaskRADIO_SetFont(s_bitFiveValueZero,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_bitFiveValueZero,"",0);
	RADIO_SetFocusColor(s_bitFiveValueZero,SERVICE_BACKGROUND_COLOR);

	s_bitFiveValueOne = RADIO_CreateEx(448,280,50,30,g_developmentSettingScreenPageFour,WM_CF_SHOW,0,eBitFiveValueOneId,1,35);
	guiTaskRADIO_SetFont(s_bitFiveValueOne,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_bitFiveValueOne,"",0);
	RADIO_SetFocusColor(s_bitFiveValueOne,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_bitFiveValueZero, 6);
	RADIO_SetGroupId(s_bitFiveValueOne, 6);

	//Bit six
	s_bitSixValueZero = RADIO_CreateEx(388,310,50,30,g_developmentSettingScreenPageFour,WM_CF_SHOW,0,eBitSixValueZeroId,1,35);
	guiTaskRADIO_SetFont(s_bitSixValueZero,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_bitSixValueZero,"",0);
	RADIO_SetFocusColor(s_bitSixValueZero,SERVICE_BACKGROUND_COLOR);

	s_bitSixValueOne = RADIO_CreateEx(448,310,50,30,g_developmentSettingScreenPageFour,WM_CF_SHOW,0,eBitSixValueOneId,1,35);
	guiTaskRADIO_SetFont(s_bitSixValueOne,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_bitSixValueOne,"",0);
	RADIO_SetFocusColor(s_bitSixValueOne,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_bitSixValueZero, 7);
	RADIO_SetGroupId(s_bitSixValueOne, 7);

	//Bit seven
	s_bitSevenValueZero = RADIO_CreateEx(388,340,50,30,g_developmentSettingScreenPageFour,WM_CF_SHOW,0,eBitSevenValueZeroId,1,35);
	guiTaskRADIO_SetFont(s_bitSevenValueZero,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_bitSevenValueZero,"",0);
	RADIO_SetFocusColor(s_bitSevenValueZero,SERVICE_BACKGROUND_COLOR);

	s_bitSevenValueOne = RADIO_CreateEx(448,340,50,30,g_developmentSettingScreenPageFour,WM_CF_SHOW,0,eBitSevenValueOneId,1,35);
	guiTaskRADIO_SetFont(s_bitSevenValueOne,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_bitSevenValueOne,"",0);
	RADIO_SetFocusColor(s_bitSevenValueOne,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_bitSevenValueZero, 8);
	RADIO_SetGroupId(s_bitSevenValueOne, 8);

	s_test1Btn = BUTTON_CreateEx(610, 145, 120, 60, g_developmentSettingScreenPageFour, WM_CF_SHOW, 0, eTest1BtnId);
	WM_SetCallback(s_test1Btn,developmentSettingScreenPageFour_CallbackTest1Btn);

	s_test2Btn = BUTTON_CreateEx(610, 240, 120, 60, g_developmentSettingScreenPageFour, WM_CF_SHOW, 0, eTest2BtnId);
	WM_SetCallback(s_test2Btn,developmentSettingScreenPageFour_CallbackTest2Btn);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_developmentSettingScreenPageFour, WM_CF_SHOW, 0, eBackBtnInDevelopmentPageFourId);
	WM_SetCallback(s_backBtn,developmentSettingScreenPageFour_CallbackBackBtn);

	//Home button
	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_developmentSettingScreenPageFour, WM_CF_SHOW, 0, eHomeBtnInDevelopmentPageFourId);
	WM_SetCallback(s_homeBtn,developmentSettingScreenPageFour_CallbackHomeBtn);

	//Next button
	s_nextBtn = BUTTON_CreateEx(700, 415, 100, 60, g_developmentSettingScreenPageFour, WM_CF_SHOW, 0, eNextBtnInDevelopmentPageFourId);
	WM_SetCallback(s_nextBtn,developmentSettingScreenPageFour_CallbackNextBtn);

	//Previous button
	s_prevBtn = BUTTON_CreateEx(0, 415, 102, 60, g_developmentSettingScreenPageFour, WM_CF_SHOW, 0, ePrevBtnInDevelopmentPageFourId);
	WM_SetCallback(s_prevBtn,developmentSettingScreenPageFour_CallbackPrevBtn);

	s_pageText = TEXT_CreateEx(380,440, 50, 25,g_developmentSettingScreenPageFour,WM_CF_SHOW,TEXT_CF_LEFT,ePageTextInDevelopmentPageFourId,"4/5");
	guiTaskTEXT_SetFont(s_pageText, &GUI_FontEng3L18B2PP);

	s_popUp = WM_CreateWindowAsChild(50, 115, 700, 225,g_developmentSettingScreenPageFour,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,developmentSettingScreenPageFour_CallbackPopUp,0);

	//Title pop up
	s_titlePopUp = TEXT_CreateEx(0, 30, 700, 70,s_popUp,WM_CF_SHOW,TEXT_CF_RIGHT,eTitlePopUpInDevelopmentPageFourStringID,"");
	TEXT_SetTextColor(s_titlePopUp, GUI_BLACK);
	TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
	guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);

	//OK button
	s_OKBtn = BUTTON_CreateEx(50, 120, 280, 60, s_popUp, WM_CF_SHOW, 0, eOkBtnPopUpInDevelopmentPageFourStringId);
	WM_SetCallback(s_OKBtn,developmentSettingScreenPageFour_CallbackOKBtn);

	//Cancel button
	s_cancelBtn = BUTTON_CreateEx(380, 120, 280, 60, s_popUp, WM_CF_SHOW, 0, eCancelBtnPopUpInDevelopmentPageFourStringId);
	WM_SetCallback(s_cancelBtn,developmentSettingScreenPageFour_CallbackCancelBtn);

	s_closeBtn = BUTTON_CreateEx(210, 120, 280, 60, s_popUp, WM_CF_SHOW, 0, eClosetnPopUpInDevelopmentPageFourStringId);
	WM_SetCallback(s_closeBtn,developmentSettingScreenPageFour_CallbackCloseBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFour_Show										*/
/*                                                                                      */
/* Details  -Show development setting screen page Four	            		    		*/
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
/*                     (O) s_popUp : pop up                                           	*/
/*                     (O) g_developmentSettingScreenPageFour : development page four	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFour_Show(void)
{
	alarmInfor_ShowAlarmInfor();
	WM_HideWindow(s_popUp);
	developmentSettingScreenPageFour_ShowAllItem();
	WM_ShowWindow(g_developmentSettingScreenPageFour);
	WM_BringToTop(g_developmentSettingScreenPageFour);
	WM_SetFocus(g_developmentSettingScreenPageFour);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFour_Hide										*/
/*                                                                                      */
/* Details  -Hide development setting screen page Four	            		    		*/
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
/*                     (O) g_developmentSettingScreenPageFour : development page four   */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFour_Hide(void)
{
	WM_HideWindow(g_developmentSettingScreenPageFour);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageThree_ClosePopUp								*/
/*                                                                                      */
/* Details  -Close pop up								            		    		*/
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
/* Using Global Data : (I) s_uploadEEPROM : flag upload eeprom							*/
/*                     (O) s_titlePopUp : title pop up                                  */
/*                     (O) s_closeBtn : close button						            */
/*                     (O) s_resetSystem : flag reset system						    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFour_ClosePopUp(bool saveResult)
{
	if(saveResult == false)
	{
		WM_MoveChildTo(s_titlePopUp,0,50);
		TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTaskTEXT_SetFont(s_titlePopUp, &GUI_FontEng3L16B2PP);
			TEXT_SetText(s_titlePopUp,"The log data could not be loaded");
		}
		else
		{
			guiTaskTEXT_SetFont(s_titlePopUp, &GUI_FontJapan3L16B2PP);
			TEXT_SetText(s_titlePopUp,"ログデータを読み込めませんでした");
		}
		WM_ShowWindow(s_closeBtn);

		if(s_uploadEEPROM == true)
		{
			s_resetSystem = false;
		}
	}
	else
	{
		WM_MoveChildTo(s_titlePopUp,0,50);
		TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTaskTEXT_SetFont(s_titlePopUp, &GUI_FontEng3L16B2PP);
			TEXT_SetText(s_titlePopUp,"Log data loaded");
		}
		else
		{
			guiTaskTEXT_SetFont(s_titlePopUp, &GUI_FontJapan3L16B2PP);
			TEXT_SetText(s_titlePopUp,"ログデータを読み込みました");
		}
		WM_ShowWindow(s_closeBtn);

		if(s_uploadEEPROM == true)
		{
			s_resetSystem = true;
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageFour_UploadToUSB								*/
/*                                                                                      */
/* Details  -upload to usb task								            		    	*/
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
/*                     (O) s_uploadEEPROM : flag upload eeprom                          */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageFour_UploadToUSB(void)
{
	switch(s_uploadFile)
	{
	case eUpLoadEEPROM:
		DEBUGOUT("\n eUpLoadEEPROM");
		usb_SendEvent(eRequestEEPRomUpload);
		s_uploadEEPROM = true;
		break;
	case eUpLoadCBX:
		DEBUGOUT("\n eUpLoadCBX");
		usb_SendEvent(eRequestCBXLogUpload);
		break;
	case eUpLoadAnypal:
		DEBUGOUT("\n eUpLoadAnypal");
		usb_SendEvent(eRequestAnypalUpload);
		break;
	case eUpLoadPulseO2:
		DEBUGOUT("\n eUpLoadPulseO2");
		usb_SendEvent(eRequestSpO2PulseUpload);
		break;
	default:
		DEBUGOUT("\n upload default");
		break;
	}

	return;
}

/* end of files */

