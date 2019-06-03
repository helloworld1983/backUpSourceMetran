/********************************************************************/
/*                                                                  */
/* File Name    : LogDataScreen.c                       			*/
/*                                                                  */
/* General       : Display log data information	                    */
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
/* #0001       2016/09/19     Linh Nguyen	  	    modify          */
/*  Use the new get log function									*/
/* #0002       2016/10/14      Viet Le	  	    	modify          */
/*  Delete all row of log list view before get log data and show	*/
/*		the log data screen (bug 1975)								*/
/* #0003       2017/10/27	   Viet Le				modify			*/
/*	-Edit function : LISTVIEW_SetFont()->guiTaskLISTVIEW_SetFont().	*/
/*			TEXT_SetFont()->guiTaskTEXT_SetFont()					*/
/*		EDIT_SetFont()->guiTaskEDIT_SetFont().(bug 1997)			*/
/* #0004       2018/02/28	   Viet Le				modify			*/
/* -Disable scroll bar. Remove logDataScreen_CallbackScrollBar	    */
/*		function (bug 2070)											*/
/********************************************************************/

#include "stdlib.h"
#include "string.h"
#include <RealTimeClock.h>
#include <StringTools.h>
#include "LogDataScreen.h"
#include "MainScreen.h"
#include "LISTVIEW.h"
#include "HEADER.h"
#include "board.h"
#include "SCROLLBAR.h"
#include "nand.h"
#include "GuiTask.h"
#include "BUTTON.h"
#include "setting.h"
#include "EDIT.h"
#include "TEXT.h"
#include "SLIDER.h"
#include "LogTask.h"
#include "LogMgr.h"
#include "fsusb_cfg.h"
#include "ff.h"
#include "UsbMgr.h"
#include "Font.h"
#include "MaintenanceScreen.h"
#include "OptionScreen.h"
#include "AlarmInfor.h"
#include "TextLibrary.h"
#include "GuiInterface.h"
#include "LogInterface.h"
#include "UsbInterface.h"

//Define item id
enum
{
	eBackBtnLogScreenId,
	eHomeBtnLogScreenId,
	eClearLogBtnId,
	eFieldDisplayLogId,
	eSaveLogBtnId,
	eTitlePopUpLogDataStringId,
	eOkBtnLogScreenId,
	eCancelBtnLogScreenId,
	eSaveBtnLogScreenId,
	eCloseBtnLogScreenId,
	eTitlePopUpFailureLogDataStringId,
	eIncreaseOneLineBtnId,
	eDecreaseOneLineBtnId,
	eIncreaseTenLineBtnId,
	eDecreaseTenLineBtnId,
	eTextWhiteId
};

//Define header view
typedef enum{
	eNumberView,
	eDateView,
	eLogNameView,
	eFlowRateView
}E_ViewId;

const char* g_logStringTable[] =
{
		"",
		"Operation Start",
		"Operation Stop",
		"Changing Flow rate",
		"Operation restart",
		"",
		"Abnormal power supply (power outage in in-operation)",
		"Abnormal power supply (pressed operation SW during unplug)",
		"Abnormal temperature",
		"Abnormal high pressure",
		"Abnormal low pressure",
		"Lowering O2 Concentration",
		"Abnormal O2 concentration",
		"Abnormal Cannula",
		"Abnormal O2 concentration sensor",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Abnormal MCU clock",
		"Abnormal Cooling Fan",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Abnormal flow rate",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Anypal communication abnormal",
		"",
		"Anypal battery abnormal",
		"Anypal RTC abnormal",
		"Over current is detected",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Usb Communication success",
		"Usb Communication Failure",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"PSA parameter set Low temp/Normal O2",
		"PSA parameter set Low temp/Lowering O2",
		"PSA parameter set Normal temp/Normal O2",
		"PSA parameter set Normal temp/Lowering O2",
		"PSA parameter set High temp/Normal O2",
		"PSA parameter set High temp/Lowering O2",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Compressor communication abnormal",
		"",
		"",
		"Abnormal filter cleaning module",
		"Abnormal Speaker",
		"",
		"",
		"",
		"",
		"",
		"Reset (watchdog)",
		"Rising Compressor outlet temperature",
		"Abnormal Compressor outlet temperature",
		"Use default parameter",
		"Abnormal Negative pressure",
		"Abnormal Nonvolatile memory",
		"Abnormal External flow path pressure",
		"Flow controller Pressure sensor abnormal",
		"Flow controller Ultrasonic sensor abnormal",
		"Flow controller Temperature sensor abnormal",
		"Flow controller communication abnormal",
		"Flow controller Command Checksum error",
		"Flow controller Command code error",
		"Flow controller Command Parameter error",
		"Flow controller Response Checksum error",
		"Flow controller Response code error",
		"Flow controller Response Parameter error",
		"Flow controller Response length error",
		"Flow controller No Response",
		"Compressor DC Over Voltage",
		"",
		"Compressor Software Over current",
		"Compressor Hardware Over current",
		"Compressor Board temperature abnormal",
		"",
		"Compressor RPM error",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Oxygen cylinder connection detected",
		"Oxygen cylinder no connection detected",
		"Flow controller command time out abnormal",
		"Flow controller reset notification",
		"Compressor command time out abnormal",
		"Compressor command reset notification"
};

const char* g_logStringTableJP[] =
{
		"",
		"運転開始",
		"運転停止",
		"設定流量変更",
		"運転再開",
		"",
		"電源異常(運転中停電)",
		"電源異常（未通電時運転SW押下）",
		"温度異常",
		"高圧異常",
		"低圧異常",
		"酸素濃度低下",
		"酸素濃度異常",
		"カニューラ異常",
		"酸素センサ異常",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"クロック異常",
		"ブロワ異常",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"流量異常",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Anypal異常(通信異常)",
		"",
		"Anypal異常(バッテリー異常)",
		"Anypal異常(RTC異常)",
		"オーバーカレント検出",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Usb Communication success",
		"Usb Communication Failure",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"PSAパラメータ設定(低温/濃度正常)",
		"PSAパラメータ設定(低温/濃度低下)",
		"PSAパラメータ設定(常温/濃度正常)",
		"PSAパラメータ設定(常温/濃度低下)",
		"PSAパラメータ設定(高温/濃度正常)",
		"PSAパラメータ設定(高温/濃度低下)",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"コンプレッサドライバ基板異常(通信異常)",
		"",
		"",
		"フィルタ掃除機構異常",
		"スピーカ異常",
		"",
		"",
		"",
		"",
		"",
		"リセット（ウォッチドッグ）",
		"コンプレッサ出口温度上昇",
		"コンプレッサ出口温度異常",
		"デフォルトパラメータ使用",
		"陰圧異常",
		"不揮発性メモリ異常",
		"外部流路圧力異常",
		"フローコントローラ異常(圧力センサ異常)",
		"フローコントローラ異常(超音波センサ異常)",
		"フローコントローラ異常(温度センサ異常)",
		"フローコントローラ異常(通信異常)",
		"フローコントローラ通信異常(コマンドチェックサム)",
		"フローコントローラ通信異常(コマンドコード)",
		"フローコントローラ通信異常(コマンドパラメータ)",
		"フローコントローラ通信異常(レスポンスチェックサム)",
		"フローコントローラ通信異常(レスポンスコード)",
		"フローコントローラ通信異常(レスポンスパラメータ)",
		"フローコントローラ通信異常(レスポンス電文サイズ)",
		"フローコントローラ通信異常(レスポンス無)",
		"コンプレッサドライバ基板異常(直流過電圧)",
		"",
		"コンプレッサドライバ基板異常(ソフトウェア過電流)",
		"コンプレッサドライバ基板異常(ハードウェア過電流)",
		"コンプレッサドライバ基板異常(基板温度異常)",
		"",
		"コンプレッサドライバ基板異常(回転数エラー)",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"酸素ボンベ接続検出",
		"酸素ボンベ未接続検出",
		"フローコントローラ異常(コマンドタイムアウト異常)",
		"フローコントローラリセット通知",
		"コンプレッサドライバ基板異常(コマンドタイムアウト異常)",
		"コンプレッサドライバ基板リセット通知"
};

WM_HWIN g_logDataScreen;

static WM_HWIN s_popUpLogData;
static TEXT_Handle s_titlePopUp;
static LISTVIEW_Handle s_listView;
static HEADER_Handle s_headerView;
static SCROLLBAR_Handle s_scrollBar;
static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static BUTTON_Handle s_clearLogBtn;
static BUTTON_Handle s_saveLogBtn;
static EDIT_Handle s_valueLog;
static BUTTON_Handle s_OKBtn;
static BUTTON_Handle s_cancelBtn;
static BUTTON_Handle s_saveBtn;
static BUTTON_Handle s_closeBtn;
static BUTTON_Handle s_increaseOneBtn;
static BUTTON_Handle s_decreaseOneBtn;
static BUTTON_Handle s_increaseTenBtn;
static BUTTON_Handle s_decreaseTenBtn;
static TEXT_Handle s_textWhite;

/*******************************************************************
 * *
The points of the arrow
 */
static const GUI_POINT s_aPointUpArrow[] = {
		{39/2,5},
		{5,34},
		{39-5,39-5}
};

static const GUI_POINT s_aPointDownArrow[] = {
		{5,5},
		{34,5},
		{39/2,34}
};

/****************************************************************************************/
/*                                                                                      */
/* General 	-logDataScreen_GetStringLogName  	 								        */
/*                                                                                      */
/* Details  -Get string name of log														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) LogId ID	: id of log													*/
/*						- range : 0-255													*/
/*						- unit : NONE													*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int8_t* : log string table		                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_logStringTable : log string table				          	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
int8_t* logDataScreen_GetStringLogName(LogId ID)
{
	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		return (int8_t*)g_logStringTable[ID];
	}
	else
	{
		return (int8_t*)g_logStringTableJP[ID];
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-logDataScreen_getNumberOfLog  	 								        	*/
/*                                                                                      */
/* Details  -Get number of log															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint32_t : number of log		                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_listView : list view							            */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
uint32_t logDataScreen_getNumberOfLog(void)
{
	return LISTVIEW_GetNumRows(s_listView);
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-logDataScreen_DisableAllBtn 	 								            */
/*                                                                                      */
/* Details  -Disable all button														    */
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
/*                     (O) s_saveLogBtn : save log button                               */
/*                     (O) s_clearLogBtn : clear log button						        */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_increaseOneBtn : increase one log button					*/
/*                     (O) s_increaseTenBtn : increase ten log button					*/
/*                     (O) s_decreaseOneBtn : decrease one log button					*/
/*                     (O) s_decreaseTenBtn : decrease ten log button					*/
/*                     (O) s_headerView : header view						            */
/*                     (O) s_listView : list view						                */
/*                     (O) s_valueLog : display number of log				         	*/
/*                     (O) s_textWhite : text white						                */
/*                     (O) g_logDataScreen : log data screen						    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_DisableAllBtn(void)
{
	WM_HideWindow(s_saveLogBtn);
	WM_HideWindow(s_clearLogBtn);
	WM_HideWindow(s_homeBtn);
	WM_HideWindow(s_backBtn);
	WM_HideWindow(s_increaseOneBtn);
	WM_HideWindow(s_increaseTenBtn);
	WM_HideWindow(s_decreaseOneBtn);
	WM_HideWindow(s_decreaseTenBtn);
	WM_HideWindow(s_headerView);
	WM_HideWindow(s_listView);
	WM_HideWindow(s_valueLog);
	WM_HideWindow(s_textWhite);

	WM_Paint(g_logDataScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-logDataScreen_EnableAllBtn  	 								            */
/*                                                                                      */
/* Details  -Enable all button														    */
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
/*                     (O) s_saveLogBtn : save log button                               */
/*                     (O) s_clearLogBtn : clear log button						        */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_increaseOneBtn : increase one log button					*/
/*                     (O) s_increaseTenBtn : increase ten log button					*/
/*                     (O) s_decreaseOneBtn : decrease one log button					*/
/*                     (O) s_decreaseTenBtn : decrease ten log button					*/
/*                     (O) s_headerView : header view						            */
/*                     (O) s_listView : list view						                */
/*                     (O) s_valueLog : display number of log				         	*/
/*                     (O) g_logDataScreen : log data screen						    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_EnableAllBtn(void)
{
	WM_ShowWindow(s_saveLogBtn);
	WM_ShowWindow(s_clearLogBtn);
	WM_ShowWindow(s_homeBtn);
	WM_ShowWindow(s_backBtn);
	WM_ShowWindow(s_increaseOneBtn);
	WM_ShowWindow(s_increaseTenBtn);
	WM_ShowWindow(s_decreaseOneBtn);
	WM_ShowWindow(s_decreaseTenBtn);
	WM_ShowWindow(s_headerView);
	WM_ShowWindow(s_listView);
	WM_ShowWindow(s_valueLog);

	WM_Paint(g_logDataScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-logDataScreen_CallbackBackBtn 								                */
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
static void logDataScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			logDataScreen_Hide();
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
/* General 	-logDataScreen_CallbackOKBtn 								                */
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
/*                     (O) s_valueLog : display number of log                           */
/*                     (O) s_listView : list view						                */
/*                     (O) s_popUpLogData : popup						                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_CallbackOKBtn(WM_MESSAGE *pMsg)
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
			logInterface_DeleteLog(eDeleteMachineLogId);

			EDIT_SetText(s_valueLog, "0");
			LISTVIEW_DeleteAllRows(s_listView);

			WM_HideWindow(s_popUpLogData);
			logDataScreen_EnableAllBtn();

			alarmInfor_ShowAlarmInfor();
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
/* General 	-logDataScreen_CallbackCancelBtn 								            */
/*                                                                                      */
/* Details  -Call back function for cancel button, handle event from 					*/
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
/*                     (O) s_popUpLogData : popup                                       */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_CallbackCancelBtn(WM_MESSAGE *pMsg)
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
			WM_HideWindow(s_popUpLogData);
			logDataScreen_EnableAllBtn();

			alarmInfor_ShowAlarmInfor();
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
/* General 	-logDataScreen_CallbackCloseBtn 								            */
/*                                                                                      */
/* Details  -Call back function for close button, handle event from 					*/
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
/*                     (O) s_popUpLogData : popup                                       */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_CallbackCloseBtn(WM_MESSAGE *pMsg)
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
			WM_HideWindow(s_popUpLogData);
			logDataScreen_EnableAllBtn();

			alarmInfor_ShowAlarmInfor();
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
/* General 	-logDataScreen_ClosePopUp								                    */
/*                                                                                      */
/* Details  -Display popup with close button						 					*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool saveResult : - save result					 					*/
/*					 - range : 0-1														*/
/*                   - unit : NONE                                                      */
/*																						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_titlePopUp : title popup                                   */
/*                     (O) s_closeBtn : close button						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_ClosePopUp(bool saveResult)
{
	if(saveResult == false)
	{
		WM_MoveChildTo(s_titlePopUp,0,50);
		TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTaskTEXT_SetFont(s_titlePopUp, &GUI_FontEng3L16B2PP);
			TEXT_SetText(s_titlePopUp,"I was not able to save the log data");
		}
		else
		{
			guiTaskTEXT_SetFont(s_titlePopUp, &GUI_FontJapan3L16B2PP);
			TEXT_SetText(s_titlePopUp,"ログデータを保存出来ませんでした");
		}
		WM_ShowWindow(s_closeBtn);
	}
	else
	{
		WM_MoveChildTo(s_titlePopUp,0,50);
		TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTaskTEXT_SetFont(s_titlePopUp, &GUI_FontEng3L16B2PP);
			TEXT_SetText(s_titlePopUp,"I have to saved the log data");
		}
		else
		{
			guiTaskTEXT_SetFont(s_titlePopUp, &GUI_FontJapan3L16B2PP);
			TEXT_SetText(s_titlePopUp,"ログデータを保存しました");
		}
		WM_ShowWindow(s_closeBtn);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-logDataScreen_CallbackSaveBtn								                */
/*                                                                                      */
/* Details  -Call back function for save button, handle event from 						*/
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
/*                     (O) s_closeBtn : close button                                    */
/*                     (O) s_saveBtn : save button						                */
/*                     (O) s_cancelBtn : cancel button						            */
/*                     (O) s_OKBtn : OK button						                    */
/*                     (O) s_titlePopUp : title button						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_CallbackSaveBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Save", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("保存", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			WM_HideWindow(s_closeBtn);
			WM_HideWindow(s_saveBtn);
			WM_HideWindow(s_cancelBtn);
			WM_HideWindow(s_OKBtn);

			WM_MoveChildTo(s_titlePopUp,0,70);
			TEXT_SetTextAlign(s_titlePopUp, GUI_TA_VCENTER | GUI_TA_HCENTER);

			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);
				TEXT_SetText(s_titlePopUp,"Saving log data");
			}
			else
			{
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontJapan3L16B2PP);
				TEXT_SetText(s_titlePopUp,"ログデータを保存しています");
			}

			GUI_Delay(GUI_DELAY_LOG_DATA);
			usb_SendEvent(eRequestSaveLog);
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
/* General 	-logDataScreen_CallbackSaveLogBtn 								            */
/*                                                                                      */
/* Details  -Call back function for save button, handle event from 						*/
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
/*                     (O) s_closeBtn : close button						            */
/*                     (O) s_saveBtn : save button						                */
/*                     (O) s_cancelBtn : cancel button						            */
/*                     (O) s_titlePopUp : title popup						            */
/*                     (O) s_popUpLogData : popup						                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_CallbackSaveLogBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Save of log data", Rect.x1/2, 10);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("ログデータの保存", Rect.x1/2, 10);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			WM_HideWindow(s_OKBtn);
			WM_HideWindow(s_closeBtn);

			WM_ShowWindow(s_saveBtn);
			WM_ShowWindow(s_cancelBtn);

			WM_MoveChildTo(s_titlePopUp,0,10);
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);
				TEXT_SetText(s_titlePopUp,"The log data will be saved\nAfter you connect the USB memory\nPress the Save button");
			}
			else
			{
				TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontJapan3L16B2PP);
				TEXT_SetText(s_titlePopUp,"ログデータを保存します\nUSBメモリーを接続してから\n保存ボタンを押して下さい");
			}

			WM_ShowWindow(s_popUpLogData);
			WM_BringToTop(s_popUpLogData);

			logDataScreen_DisableAllBtn();
			alarmInfor_HideAlarmInfor();
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
/* General 	-logDataScreen_CallbackClearBtn 								            */
/*                                                                                      */
/* Details  -Call back function for clear button, handle event from 					*/
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
/*                     (O) s_saveBtn : save button                                      */
/*                     (O) s_closeBtn : close button						            */
/*                     (O) s_OKBtn : OK button						                    */
/*                     (O) s_cancelBtn : cancel button						            */
/*                     (O) s_titlePopUp : title popup						            */
/*                     (O) s_popUpLogData : popup						                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_CallbackClearBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(GUI_WHITE);
		GUI_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);

		GUI_SetColor(GUI_BLACK);
		GUI_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);

		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(GUI_BLACK);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Clear", Rect.x1/2, 10);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("クリ ア", Rect.x1/2, 10);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			WM_HideWindow(s_saveBtn);
			WM_HideWindow(s_closeBtn);

			WM_ShowWindow(s_OKBtn);
			WM_ShowWindow(s_cancelBtn);

			WM_MoveChildTo(s_titlePopUp,0,10);
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);
				TEXT_SetText(s_titlePopUp,"I will clear the log data\nWould you like?");
			}
			else
			{
				TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontJapan3L16B2PP);
				TEXT_SetText(s_titlePopUp,"ログデータをクリアします\nよろしいですか？");
			}

			WM_ShowWindow(s_popUpLogData);
			WM_BringToTop(s_popUpLogData);

			logDataScreen_DisableAllBtn();
			alarmInfor_HideAlarmInfor();
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
/* General 	-logDataScreen_CallbackHomeBtn 								                */
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
static void logDataScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			logDataScreen_Hide();
			WM_SendMessageNoPara(g_mainWindow,WM_SHOW_HOMESCREEN);
		}
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/** >>#0004
	Remove logDataScreen_CallbackScrollBar
#0004 >>**/

/****************************************************************************************/
/*                                                                                      */
/* General 	-logDataScreen_CallbackHeaderView	                                        */
/*                                                                                      */
/* Details  -call back function for header view, handle event from window		        */
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
void logDataScreen_CallbackHeaderView(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(HEADER_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(GUI_BLACK);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L10B2PP);

			GUI_DrawLine(Rect.x0 + 50,Rect.y0,Rect.x0 + 50, Rect.y0 + 20);
			GUI_DispStringHCenterAt("No.",Rect.x0 + 25,Rect.y0);

			GUI_DrawLine(Rect.x0 + 50 + 180,Rect.y0,Rect.x0 + 50 + 180, Rect.y0 + 20);
			GUI_DispStringHCenterAt("Date and Time",Rect.x0 + 125,Rect.y0);

			GUI_DrawLine(Rect.x0 + 50 + 180 + 420,Rect.y0,Rect.x0 + 50 + 180 + 420, Rect.y0 + 20);
			GUI_DispStringHCenterAt("Log name",Rect.x0 + 450,Rect.y0);

			GUI_DrawLine(Rect.x0 + 50 + 180 + 420 + 90,Rect.y0,Rect.x0 + 50 + 180 + 420 + 90, Rect.y0 + 20);
			GUI_DispStringHCenterAt("Flow Rate",Rect.x0 + 50 + 280 + 320 + 50,Rect.y0);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);

			GUI_DrawLine(Rect.x0 + 50,Rect.y0,Rect.x0 + 50, Rect.y0 + 20);
			GUI_DispStringHCenterAt("No.",Rect.x0 + 25,Rect.y0 + 2);

			GUI_DrawLine(Rect.x0 + 50 + 180,Rect.y0,Rect.x0 + 50 + 180, Rect.y0 + 20);
			GUI_DispStringHCenterAt("日時",Rect.x0 + 125,Rect.y0);

			GUI_DrawLine(Rect.x0 + 50 + 180 + 420,Rect.y0,Rect.x0 + 50 + 180 + 420, Rect.y0 + 20);
			GUI_DispStringHCenterAt("ログ名称",Rect.x0 + 450,Rect.y0);

			GUI_DrawLine(Rect.x0 + 50 + 180 + 420 + 90,Rect.y0,Rect.x0 + 50 + 180 + 420 + 90, Rect.y0 + 20);
			GUI_DispStringHCenterAt("設定流量",Rect.x0 + 50 + 280 + 320 + 50,Rect.y0);
		}
		break;
	case WM_TOUCH:
	case WM_PID_STATE_CHANGED:
		break;
	default:
		HEADER_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-logDataScreen_CallbackListView	                                            */
/*                                                                                      */
/* Details  -call back function for list view, handle event from window		            */
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
void logDataScreen_CallbackListView(WM_MESSAGE *pMsg)
{
	switch (pMsg->MsgId)
	{
	case WM_TOUCH:
		break;
	default:
		LISTVIEW_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-logDataScreen_AddLogToTable	                                            */
/*                                                                                      */
/* Details  -Add log string to log table 			    								*/
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
/*                     (O) s_titlePopUp : title popup                                   */
/*                     (O) s_listView : list view						                */
/*                     (O) s_OKBtn : OK button							                */
/*                     (O) s_cancelBtn : cancel button						            */
/*                     (O) s_saveBtn : save button						                */
/*                     (O) s_closeBtn : close button						            */
/*                     (O) s_popUpLogData : popup						                */
/*                     (O) s_valueLog : display number of log						    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_AddLogToTable(void)
{
	LISTVIEW_DeleteAllRows(s_listView);
	uint32_t cout=0;
	int8_t buffer[SIZE_BUFFER_NUMBER_LOG] = {'\0'};

	alarmInfor_HideAlarmInfor();
	WM_MoveChildTo(s_titlePopUp,0,70);
	TEXT_SetTextAlign(s_titlePopUp, GUI_TA_VCENTER | GUI_TA_HCENTER);
	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		guiTaskLISTVIEW_SetFont(s_listView,&GUI_FontEng3L10B2PP);
		guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);
		TEXT_SetText(s_titlePopUp,"Loading");
	}
	else
	{
		guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontJapan3L16B2PP);
		TEXT_SetText(s_titlePopUp,"ログデータを読込み中です");
		guiTaskLISTVIEW_SetFont(s_listView,&GUI_FontJapan3L10B2PP);
	}
	WM_HideWindow(s_OKBtn);
	WM_HideWindow(s_cancelBtn);
	WM_HideWindow(s_saveBtn);
	WM_HideWindow(s_closeBtn);
	alarmInfor_HideAlarmInfor();

	WM_ShowWindow(s_popUpLogData);
	WM_BringToTop(s_popUpLogData);
	logDataScreen_DisableAllBtn();

	GUI_Delay(GUI_DELAY_LOG_DATA);

	int32_t numLog = (int32_t)logInterface_GetNumberOfLog(eMachineLog);

	itoa(numLog,(char*)&buffer[0], 10);
	EDIT_SetText(s_valueLog,(char*)buffer);
	FH310_LOG_T log[MAX_LOG_GUI_RQ];
	FH310_LOG_T logGet[MAX_LOG_GUI_RQ];
	uint8_t NoLogGet;
	LOG_REQUEST_t request;
	request.id = eGuiGetMachineLogId;
	while(numLog>0)
	{
		guiInterface_WWDTFeed(LPC_WWDT);
		if(numLog > MAX_LOG_GUI_RQ)
		{
			request.beginLog =  numLog-MAX_LOG_GUI_RQ+1;
			request.numberOfLog = MAX_LOG_GUI_RQ;
		}
		else
		{
			request.beginLog =  FIRST_LOG_INDEX;
			request.numberOfLog = numLog;
		}
		if (logInterface_ReceiveLogForGUI(request, logGet, &NoLogGet) == eSuccess)
		{
			for(int j =0; j< NoLogGet; j++)
			{
				log[j] = logGet[NoLogGet -j-1];
				int8_t array_1[5];
				itoa(cout+1,(char*)array_1,10);
				int8_t array_2[18];
				itoa((log[j].YYYY%100),(char*)array_2,10);
				sprintf((char*)array_2,"%.2d",(log[j].YYYY%100));
				array_2[2]= '/';

				sprintf((char*)&array_2[3],"%.2d",log[j].MM);

				array_2[5]='/';

				sprintf((char*)&array_2[6],"%.2d",log[j].DD);

				array_2[8]= ' ';

				sprintf((char*)&array_2[9],"%.2d",log[j].hh);

				array_2[11]=':';

				sprintf((char*)&array_2[12],"%.2d",log[j].min);

				array_2[14]=':';

				sprintf((char*)&array_2[15],"%.2d",log[j].ss);

				int8_t *array_3;
				array_3 = logDataScreen_GetStringLogName(log[j].ID);
				int8_t array_4[5] = {'\0'};
				float flowRate = log[j].flowRateSetting / FLOWRATE_SCALE;
				sprintf((char*)array_4,"%.2f",flowRate);

				int8_t * pText[5] = {array_1,array_2,array_3,array_4,(int8_t *)""};

				LISTVIEW_AddRow(s_listView, (GUI_ConstString*)pText);
				cout++;
			}
		}
		else
		{
			break;
		}
		numLog -= NoLogGet;
	}



	WM_HideWindow(s_popUpLogData);
	logDataScreen_EnableAllBtn();
	alarmInfor_ShowAlarmInfor();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-logDataScreen_Callback	                                            		*/
/*                                                                                      */
/* Details  -call back function for log data screen, handle event from window		    */
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
void logDataScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		if(!WM_IsVisible(s_popUpLogData))
		{
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				GUI_SetColor(GUI_BLACK);
				guiTask_SetFont(&GUI_FontEng3L16B2PP);
				GUI_DispStringHCenterAt("Log data", 400,5);
				guiTask_SetFont(&GUI_FontEng3L15B2PP);
				GUI_DispStringAt("Number of log",10,43);
				GUI_DispStringAt("Event",233,79);
			}
			else
			{
				GUI_SetColor(GUI_BLACK);
				guiTask_SetFont(&GUI_FontJapan3L18B2PP);
				GUI_DispStringHCenterAt("ログデータ", 400,5);
				guiTask_SetFont(&GUI_FontJapan3L16B2PP);
				GUI_DispStringAt("ログ件数",10,44);
				GUI_DispStringAt("件",233,79);
			}
		}
		break;
	case WM_SET_FOCUS:
	{
		logDataScreen_AddLogToTable();
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
/* General 	-logDataScreen_CallbackPopUp	                                            */
/*                                                                                      */
/* Details  -call back function for pop up log data screen, handle event from window	*/
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
void logDataScreen_CallbackPopUp(WM_MESSAGE *pMsg)
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
/* General 	-logDataScreen_CallbackIncreaseTenLine 									    */
/*                                                                                      */
/* Details  -Call back function increase ten line button 			            	    */
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
/*                     (O) s_scrollBar : scroll bar                                     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_CallbackIncreaseTenLine(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(GUI_WHITE);
		GUI_ClearRectEx(&Rect);

		GUI_FillRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1);

		GUI_SetColor(GUI_BLACK);
		GUI_DrawRect(Rect.x0+1,Rect.y0,Rect.x1,Rect.y1);
		GUI_FillPolygon(&s_aPointUpArrow[0],3,Rect.x0,Rect.y0);

		GUI_SetPenSize(2);
		GUI_DrawLine(Rect.x0 + 5, Rect.y0+5,Rect.x1 - 5,Rect.y0+5);
		break;
	case WM_PID_STATE_CHANGED:
		if(SCROLLBAR_GetValue(s_scrollBar) > 0)
		{
			if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
			{
				SCROLLBAR_SetValue(s_scrollBar,SCROLLBAR_GetValue(s_scrollBar) - MAX_ROW_OF_PAGE);
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
/* General 	-logDataScreen_CallbackIncreaseOneLine									    */
/*                                                                                      */
/* Details  -Call back function increase one line button 			            	    */
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
/*                     (O) s_scrollBar : scroll bar                                     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_CallbackIncreaseOneLine(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(GUI_WHITE);
		GUI_ClearRectEx(&Rect);

		GUI_FillRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1);
		GUI_SetColor(GUI_BLACK);
		GUI_DrawRect(Rect.x0+1,Rect.y0,Rect.x1,Rect.y1);

		GUI_FillPolygon(&s_aPointUpArrow[0],3,Rect.x0,Rect.y0);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			SCROLLBAR_Dec(s_scrollBar);
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
/* General 	-logDataScreen_CallbackDecreaseOneLine									    */
/*                                                                                      */
/* Details  -Call back function decrease one line button 			            	    */
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
/*                     (O) s_scrollBar : scroll bar                                     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_CallbackDecreaseOneLine(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(GUI_WHITE);
		GUI_ClearRectEx(&Rect);

		GUI_FillRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1);
		GUI_SetColor(GUI_BLACK);
		GUI_DrawRect(Rect.x0+1,Rect.y0,Rect.x1,Rect.y1);

		GUI_FillPolygon(&s_aPointDownArrow[0],3,Rect.x0,Rect.y0);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			SCROLLBAR_Inc(s_scrollBar);
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
/* General 	-logDataScreen_CallbackDecreaseTenLine 									    */
/*                                                                                      */
/* Details  -Call back function decrease ten line button 			            	    */
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
/*                     (O) s_scrollBar : scroll bar                                     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_CallbackDecreaseTenLine(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(GUI_WHITE);
		GUI_ClearRectEx(&Rect);

		GUI_FillRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1);
		GUI_SetColor(GUI_BLACK);
		GUI_DrawRect(Rect.x0+1,Rect.y0,Rect.x1,Rect.y1);

		GUI_FillPolygon(&s_aPointDownArrow[0],3,Rect.x0,Rect.y0);

		GUI_SetPenSize(2);
		GUI_DrawLine(Rect.x0 + 5, Rect.y0+35,Rect.x1 - 5,Rect.y0+35);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			SCROLLBAR_SetValue(s_scrollBar,SCROLLBAR_GetValue(s_scrollBar) + MAX_ROW_OF_PAGE);
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
/* General 	-logDataScreen_CallbackTextWhite 									        */
/*                                                                                      */
/* Details  -Call back function text white							            	    */
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
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_CallbackTextWhite(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(GUI_WHITE);
		GUI_ClearRectEx(&Rect);

		GUI_FillRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1);
		GUI_SetColor(GUI_BLACK);
		GUI_DrawRect(Rect.x0+1,Rect.y0,Rect.x1,Rect.y1);
		break;
	default:
		TEXT_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-logDataScreen_Init 							                			*/
/*                                                                                      */
/* Details  -Initialize log data screen 			            	                	*/
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
/*                     (O) g_logDataScreen : log data screen                            */
/*                     (O) s_clearLogBtn : clear button						            */
/*                     (O) s_valueLog : display number of log						    */
/*                     (O) s_saveLogBtn : save log button						        */
/*                     (O) s_listView : list view						                */
/*                     (O) s_scrollBar : scroll bar						                */
/*                     (O) s_increaseTenBtn : increase ten log button		            */
/*                     (O) s_increaseOneBtn	: increase one log button					*/
/*                     (O) s_textWhite : text white						                */
/*                     (O) s_decreaseOneBtn : decrease one log button 					*/
/*                     (O) s_decreaseTenBtn : decrease ten log button		            */
/*                     (O) s_headerView : header view						            */
/*                     (O) s_popUpLogData : popup						                */
/*                     (O) s_titlePopUp : title popup						            */
/*                     (O) s_OKBtn : OK button						                    */
/*                     (O) s_cancelBtn : cancel button						            */
/*                     (O) s_saveBtn : save button						                */
/*                     (O) s_closeBtn : close button						            */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button						                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_Init(void)
{
	g_logDataScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,logDataScreen_Callback,0);

	s_clearLogBtn = BUTTON_CreateEx(10, 69, 110, 40, g_logDataScreen, WM_CF_SHOW, 0, eClearLogBtnId);
	WM_SetCallback(s_clearLogBtn,logDataScreen_CallbackClearBtn);

	s_valueLog = EDIT_CreateEx(120,69,110,40,g_logDataScreen,WM_CF_SHOW,0,eFieldDisplayLogId,50);
	EDIT_SetFocussable(s_valueLog,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_valueLog ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_valueLog ,GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_valueLog, "0");

	s_saveLogBtn = BUTTON_CreateEx(580, 69, 210, 40, g_logDataScreen, WM_CF_SHOW, 0, eSaveLogBtnId);
	WM_SetCallback(s_saveLogBtn,logDataScreen_CallbackSaveLogBtn);

	s_listView = LISTVIEW_CreateEx(10,112,780,300,g_logDataScreen,WM_CF_SHOW,0,GUI_ID_LISTVIEW0);
	WM_SetCallback(s_listView,logDataScreen_CallbackListView);
	guiTaskLISTVIEW_SetFont(s_listView,&GUI_FontEng3L10B2PP);
	LISTVIEW_SetHeaderHeight(s_listView,20);
	LISTVIEW_SetGridVis(s_listView,1);
	LISTVIEW_DisableSort(s_listView);
	LISTVIEW_DisableRow(s_listView,0);
	WM_DisableWindow(s_listView);

	s_scrollBar = SCROLLBAR_CreateAttached(s_listView,SCROLLBAR_CF_VERTICAL);
	SCROLLBAR_SetWidth(s_scrollBar, 40);
	WM_MoveChildTo(s_scrollBar,740, 60);
	WM_SetSize(s_scrollBar,40, 200);
	/** >>#0004
		Disable scroll bar
	#0004 >>**/
	WM_DisableWindow(s_scrollBar);

	s_increaseTenBtn = BUTTON_CreateEx(750, 132, 40, 40, g_logDataScreen, WM_CF_SHOW, 0, eIncreaseTenLineBtnId);
	WM_SetCallback(s_increaseTenBtn,logDataScreen_CallbackIncreaseTenLine);

	s_increaseOneBtn = BUTTON_CreateEx(750, 172, 40, 40, g_logDataScreen, WM_CF_SHOW, 0, eIncreaseOneLineBtnId);
	WM_SetCallback(s_increaseOneBtn,logDataScreen_CallbackIncreaseOneLine);

	s_textWhite = TEXT_CreateEx(750, 212, 40, 140,g_logDataScreen,WM_CF_HIDE,TEXT_CF_HCENTER,eTextWhiteId,"");
	WM_SetCallback(s_textWhite,logDataScreen_CallbackTextWhite);

	s_decreaseOneBtn = BUTTON_CreateEx(750, 332, 40, 40, g_logDataScreen, WM_CF_SHOW, 0, eDecreaseOneLineBtnId);
	WM_SetCallback(s_decreaseOneBtn,logDataScreen_CallbackDecreaseOneLine);

	s_decreaseTenBtn = BUTTON_CreateEx(750, 372, 40, 40, g_logDataScreen, WM_CF_SHOW, 0, eDecreaseTenLineBtnId);
	WM_SetCallback(s_decreaseTenBtn,logDataScreen_CallbackDecreaseTenLine);

	LISTVIEW_AddColumn(s_listView,50,"No.",GUI_TA_VCENTER | GUI_TA_HCENTER);
	LISTVIEW_AddColumn(s_listView,180,"Date and Time",GUI_TA_VCENTER | GUI_TA_HCENTER);
	LISTVIEW_AddColumn(s_listView,420,"Log name",GUI_TA_VCENTER | GUI_TA_HCENTER);
	LISTVIEW_AddColumn(s_listView,90,"Flow Rate",GUI_TA_VCENTER | GUI_TA_HCENTER);
	LISTVIEW_AddColumn(s_listView,40,"",GUI_TA_VCENTER | GUI_TA_HCENTER);

	LISTVIEW_SetRowHeight(s_listView,28);

	s_headerView = LISTVIEW_GetHeader(s_listView);
	WM_SetCallback(s_headerView,logDataScreen_CallbackHeaderView);
	WM_InvalidateWindow(s_headerView);
	WM_BringToBottom(s_headerView);

	s_popUpLogData = WM_CreateWindowAsChild(50, 115, 700, 220,g_logDataScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,logDataScreen_CallbackPopUp,0);

	//Title pop up
	s_titlePopUp = TEXT_CreateEx(0, 10, 700, 70,s_popUpLogData,WM_CF_SHOW,TEXT_CF_RIGHT,eTitlePopUpLogDataStringId,"");
	TEXT_SetTextColor(s_titlePopUp, GUI_BLACK);
	TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
	guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);

	//OK button
	s_OKBtn = BUTTON_CreateEx(50, 120, 280, 60, s_popUpLogData, WM_CF_SHOW, 0, eOkBtnLogScreenId);
	WM_SetCallback(s_OKBtn,logDataScreen_CallbackOKBtn);

	//Cancel button
	s_cancelBtn = BUTTON_CreateEx(380, 120, 280, 60, s_popUpLogData, WM_CF_SHOW, 0, eCancelBtnLogScreenId);
	WM_SetCallback(s_cancelBtn,logDataScreen_CallbackCancelBtn);

	s_saveBtn = BUTTON_CreateEx(50, 120, 280, 60, s_popUpLogData, WM_CF_SHOW, 0, eSaveBtnLogScreenId);
	WM_SetCallback(s_saveBtn,logDataScreen_CallbackSaveBtn);

	s_closeBtn = BUTTON_CreateEx(210, 120, 280, 60, s_popUpLogData, WM_CF_SHOW, 0, eCloseBtnLogScreenId);
	WM_SetCallback(s_closeBtn,logDataScreen_CallbackCloseBtn);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_logDataScreen, WM_CF_SHOW, 0, eBackBtnLogScreenId);
	WM_SetCallback(s_backBtn,logDataScreen_CallbackBackBtn);

	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_logDataScreen, WM_CF_SHOW, 0, eHomeBtnLogScreenId);
	WM_SetCallback(s_homeBtn,logDataScreen_CallbackHomeBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-logDataScreen_Show 							                			*/
/*                                                                                      */
/* Details  -Show log data screen 			            	                			*/
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
/*                     (O) g_logDataScreen : log data screen                            */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_Show(void)
{
	WM_ShowWindow(g_logDataScreen);
	WM_BringToTop(g_logDataScreen);
	WM_SetFocus(g_logDataScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-logDataScreen_Hide 							                			*/
/*                                                                                      */
/* Details  -Hide log data screen 			            	                			*/
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
/*                     (O) s_listView : list view                                       */
/*                     (O) g_logDataScreen : log data screen						    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void logDataScreen_Hide(void)
{
	LISTVIEW_DeleteAllRows(s_listView);
	WM_HideWindow(g_logDataScreen);

	return;
}

/* end of files */
