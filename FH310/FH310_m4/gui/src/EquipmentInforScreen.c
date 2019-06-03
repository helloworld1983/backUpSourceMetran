/********************************************************************/
/*                                                                  */
/* File Name    : EquipmentInforScreen.c                            */
/*                                                                  */
/* General       : Displaying data in equipment information 		*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2008/08/22      Viet Le  	        new file        */
/*  #0001	   2017/09/07      Linh Nguyen(MV)  	    modify	    */
/*    use the new get serial number function design					*/
/*  #0002      2017/09/12       Linh Nguyen	  	    modify          */
/*  Use new save log function                            			*/
/*  #0003	   2017/10/27      Viet Le 	    	        modify	    */
/*	  Edit : EDIT_SetFont()->guiTaskEDIT_SetFont(bug 1997)			*/
/*  #0004	   2017/12/01      Viet Le 	    	        modify	    */
/*    Handle flag blinking flow rate when change flow rate in 		*/
/*     service mode with child lock mode(bug 2046)					*/
/*  #0005	   2018/01/18      Viet Le 	    	        modify	    */
/*	 When the flow rate changes, wait 1 second to play the flow rate*/
/*		(bug 2057)													*/
/*  #0006	   2018/02/01      Viet Le 	    	        modify	    */
/*   Not check current flow rate and pendding flow rate when play 	*/
/*		flow rate after 1 seconds(bug 2059)							*/
/*  #0007	   2018/02/06      Viet Le 	    	        modify	    */
/*	 Add comment(bug 2064) 											*/
/********************************************************************/

#include "stdlib.h"
#include <RealTimeClock.h>
#include <StringTools.h>
#include <StringTools.h>
#include "EquipmentInforScreen.h"
#include "MainScreen.h"
#include "WM.h"
#include "StringTable.h"
#include "EDIT.h"
#include "DataDisplay.h"
#include "BUTTON.h"
#include "board.h"
#include "setting.h"
#include "GuiTask.h"
#include "MonitorGraphScreen.h"
#include "MonitorScreen.h"
#include "AudioMgr.h"
#include "ModeMgr.h"
#include "OperationScreen.h"
#include "GuiHandler.h"
#include "Font.h"
#include "AlarmInfor.h"
#include "ServiceScreen.h"
#include "EquipmentScreen.h"
#include "OptionScreen.h"
#include "ConnectedEquipmentInfoScreen.h"
#include "TextLibrary.h"
#include "GuiInterface.h"
#include "LogInterface.h"

//Define item id
enum{
	eFlowRateSettingInEquipmentInforStringId,
	eSendDeviceTimerInEquipmentInforId,
	eTimerChildlock5sId,
	eTimerUpdateCalendarAndCumulativeId
};

WM_HWIN g_equipmentInforScreen;

static EDIT_Handle s_displaySerialNumber;
static EDIT_Handle s_displaySoftwareVersion;
static EDIT_Handle s_displayCalendar;
static EDIT_Handle s_displayPatientCumulativeHours;
static EDIT_Handle s_displayUintCumulativeHours;
static EDIT_Handle s_displayFlowrateSetting;
static EDIT_Handle s_displayMeasurementFlowRateData;
static EDIT_Handle s_displayOxyconcentratorData;

static WM_HTIMER s_timerSendDevice;
static WM_HTIMER s_timerChildLock5s;

static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static BUTTON_Handle s_nextBtn;

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
/* General 	-equipmentInforScreen_Callback 								                */
/*                                                                                      */
/* Details  -Call back function for equipment infor screen, handle event from 			*/
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
/* Using Global Data : (I) g_equipmentInforScreen : equipment infor screen				*/
/*                     (O) s_displayFlowrateSetting : display flow rate setting         */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentInforScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			GUI_SetColor(GUI_BLACK);
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Equipment information", 400,5);

			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);

			//Serial number
			GUI_SetTextAlign(GUI_TA_RIGHT);
			GUI_DispStringAt("Serial number:",300, 57);

			//Software version
			GUI_SetTextAlign(GUI_TA_RIGHT);
			GUI_DispStringAt("Software version:",300, 97);

			//Calendar
			GUI_SetTextAlign(GUI_TA_RIGHT);
			GUI_DispStringAt("Calendar:",300, 137);

			//Patient’s cumulative operation hours
			GUI_SetTextAlign(GUI_TA_RIGHT);
			GUI_DispStringAt("Patient’s cumulative operation hours:",300, 177);

			//Unit’s cumulative operation hours
			GUI_SetTextAlign(GUI_TA_RIGHT);
			GUI_DispStringAt("Unit’s cumulative operation hours:",300, 217);

			//FlowRate
			GUI_SetTextAlign(GUI_TA_RIGHT);
			GUI_DispStringAt("Setting flow rates:",300, 257);

			//Uint FlowRate
			GUI_SetTextAlign(GUI_TA_RIGHT);
			GUI_DispStringAt("L/min",470, 257);
		}
		else
		{
			GUI_SetColor(GUI_BLACK);
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("機器情報表示", 400,5);

			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);

			//Serial number
			GUI_SetTextAlign(GUI_TA_RIGHT);
			GUI_DispStringAt("シリアル番号:",300, 57);

			//Software version
			GUI_SetTextAlign(GUI_TA_RIGHT);
			GUI_DispStringAt("ソフトウェアバージョン:",300, 97);

			//Calendar
			GUI_SetTextAlign(GUI_TA_RIGHT);
			GUI_DispStringAt("カレンダ:",300, 137);

			//Patient’s cumulative operation hours
			GUI_SetTextAlign(GUI_TA_RIGHT);
			GUI_DispStringAt("患者積算時間:",300, 177);

			//Unit’s cumulative operation hours
			GUI_SetTextAlign(GUI_TA_RIGHT);
			GUI_DispStringAt("機器稼働時間:",300, 217);

			//FlowRate
			GUI_SetTextAlign(GUI_TA_RIGHT);
			GUI_DispStringAt("設定流量:",300, 257);

			//Uint FlowRate
			GUI_DispStringAt("L/分",426, 257);
		}

		guiTask_SetFont(&GUI_FontEng3L18B2PP);
		GUI_DispStringHCenterAt("1/3", 400,440);
		break;
	case WM_TIMER:
	{
		switch(WM_GetTimerId(pMsg->Data.v))
		{
		case eSendDeviceTimerInEquipmentInforId:
		{
			uint8_t flowRate = guiTask_GetFlowrate();
			uint8_t penddingFlowRate = guiTask_GetPenddingFlowRate();
			uint8_t flowrateSetting = 0;
			if(flowRate != penddingFlowRate)
			{
				guiTask_SetPenddingFowRate(flowRate);

				if(guiInterface_GetSettingValue(eChildLockSettingId) == eOff)
				{
					guiInterface_SetSettingValue(eFlowRateSettingId,guiTask_GetFlowrate());
					setting_SaveAllSettingValue();
					logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eChangingFlowRateLogId);
				}

				flowrateSetting = guiInterface_GetSettingValue(eFlowRateSettingId);

				if(flowrateSetting >= SETTING_FR_025L && flowrateSetting <= SETTING_FR_300L)
				{
					guiTask_SendPSAParaEvent(ePacketPSAParaChangeId,flowrateSetting,
							guiTask_GetConditionO2(),guiTask_GetConditionTemp());
				}
			}

			/** >>#0006
				Not check current flow rate and pendding flow rate when play flow rate
			#0006 >>**/
			if(modeMgr_GetCurrentMode() == eServiceMode)
			{
				audioMgr_PlayFlowRate(flowRate);
			}
			break;
		}
		case eTimerChildlock5sId:
		{
			uint8_t flowRate = guiTask_GetFlowrate();
			uint8_t penddingFlowRate = guiTask_GetPenddingFlowRate();
			double floatValue = 0.0;
			guiTask_SetBlinkingFR(false);
			if(flowRate != penddingFlowRate)
			{
				int8_t buffer[SIZE_BUFFER_FLOW_RATE] = {'\0'};
				guiTask_SetFowRate(guiTask_GetPenddingFlowRate());
				floatValue = (double)guiTask_GetFlowrate()/FLOWRATE_SCALE;
				sprintf((char*)buffer,"%0.2f",floatValue);
				if(WM_IsVisible(g_equipmentInforScreen))
				{
					EDIT_SetText(s_displayFlowrateSetting ,(char*)buffer);
				}
				monitorScreen_ChangeFlowrate(guiTask_GetFlowrate());
				monitorGraphScreen_ChangeFlowrate(guiTask_GetFlowrate());
			}
			break;
		}
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
/* General 	-equipmentInforScreen_CallbackBackBtn 								        */
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
void equipmentInforScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		WM_GetClientRect(&Rect);
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
			equipmentInforScreen_Hide();
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
/* General 	-equipmentInforScreen_CallbackHomeBtn 								        */
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
static void equipmentInforScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		WM_GetClientRect(&Rect);
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
			equipmentInforScreen_Hide();
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
/* General 	-equipmentInforScreen_CallbackNextBtn 								        */
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
void equipmentInforScreen_CallbackNextBtn(WM_MESSAGE * pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		WM_GetClientRect(&Rect);

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
			equipmentInforScreen_Hide();
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
/* General 	-equipmentInforScreen_Init								    				*/
/*                                                                                      */
/* Details  -Initialize equipment information screen 			            			*/
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
/*                     (O) g_equipmentInforScreen : equipment infor screen              */
/*                     (O) s_displaySerialNumber : display serial number	            */
/*                     (O) s_displaySoftwareVersion : display control SW version        */
/*                     (O) s_displayCalendar : display calendar						    */
/*                     (O) s_displayPatientCumulativeHours : display patient cumulative */
/*                     (O) s_displayUintCumulativeHours : display unit cumulative       */
/*                     (O) s_displayFlowrateSetting : display flow rate setting    		*/
/*                     (O) s_displayMeasurementFlowRateData : display measurement flow	*/
/*                     (O) s_displayOxyconcentratorData : display O2 concentrator       */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button                                      */
/*                     (O) s_nextBtn : next button						                */
/*                     (O) s_timerSendDevice : timer 1 seconds send flow setting to device*/
/*                     (O) s_timerChildLock5s : timer 5 seconds when change FR in child lock*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentInforScreen_Init(void)
{
	g_equipmentInforScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_equipmentScreen,WM_CF_HIDE| WM_CF_MEMDEV | WM_CF_LATE_CLIP,equipmentInforScreen_Callback,0);

	s_displaySerialNumber = EDIT_CreateEx(300,50,320,30,g_equipmentInforScreen,WM_CF_SHOW,0,eSerialNumberStringId,15);
	EDIT_SetFocussable(s_displaySerialNumber,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_displaySerialNumber ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_displaySerialNumber ,GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_displaySerialNumber ,"0000000");

	s_displaySoftwareVersion = EDIT_CreateEx(300,90,320,30,g_equipmentInforScreen,WM_CF_SHOW,0,eSoftwareVersionStringId,50);
	EDIT_SetFocussable(s_displaySoftwareVersion,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_displaySoftwareVersion ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_displaySoftwareVersion ,GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_displaySoftwareVersion ,(char*)stringtools_GetVersion());

	s_displayCalendar = EDIT_CreateEx(300,130,320,30,g_equipmentInforScreen,WM_CF_SHOW,0,eCalendarStringId,50);
	EDIT_SetFocussable(s_displayCalendar,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_displayCalendar ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_displayCalendar ,GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_displayCalendar ,"2016/08/24 00:00:00");

	s_displayPatientCumulativeHours = EDIT_CreateEx(300,170,280,30,g_equipmentInforScreen,WM_CF_SHOW,0,ePatientCumulativeHourStringId,15);
	EDIT_SetFocussable(s_displayPatientCumulativeHours,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_displayPatientCumulativeHours ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_displayPatientCumulativeHours ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_displayUintCumulativeHours = EDIT_CreateEx(300,210,280,30,g_equipmentInforScreen,WM_CF_SHOW,0,eUintCumulativeHourStringId,15);
	EDIT_SetFocussable(s_displayUintCumulativeHours,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_displayUintCumulativeHours ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_displayUintCumulativeHours ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_displayFlowrateSetting = EDIT_CreateEx(300,250,125,30,g_equipmentInforScreen,WM_CF_SHOW,0,eFlowRateSettingInEquipmentInforStringId,5);
	EDIT_SetFocussable(s_displayFlowrateSetting,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_displayFlowrateSetting ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_displayFlowrateSetting ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_displayMeasurementFlowRateData = EDIT_CreateEx(100, 290, 370, 40,g_equipmentInforScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eMeasurementFRDataStringID,50);
	WM_SetCallback(s_displayMeasurementFlowRateData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_displayMeasurementFlowRateData,eValueOfMeasurementFREquipmentInforDataId);

	s_displayOxyconcentratorData = EDIT_CreateEx(100, 330, 370, 40,g_equipmentInforScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eO2PercentDataStringID,50);
	WM_SetCallback(s_displayOxyconcentratorData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_displayOxyconcentratorData,eValueOfOxgEquipmentInforDataId);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_equipmentInforScreen, WM_CF_SHOW, 0, eBackBtnEquipmentInforStringId);
	WM_SetCallback(s_backBtn,equipmentInforScreen_CallbackBackBtn);

	//Home button
	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_equipmentInforScreen, WM_CF_SHOW, 0, eHomeBtnEquipmentInforStringId);
	WM_SetCallback(s_homeBtn,equipmentInforScreen_CallbackHomeBtn);

	//Next button
	s_nextBtn = BUTTON_CreateEx(700, 415, 100, 60, g_equipmentInforScreen, WM_CF_SHOW, 0, eNexBtnEquipmentInforStringId);
	WM_SetCallback(s_nextBtn,equipmentInforScreen_CallbackNextBtn);

	s_timerSendDevice = WM_CreateTimer(g_equipmentInforScreen, eSendDeviceTimerInEquipmentInforId, TIMER_1_SECONDS, 0);
	s_timerChildLock5s = WM_CreateTimer(g_equipmentInforScreen, eTimerChildlock5sId, TIMER_100_MS, 0);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentInforScreen_Show 								    				*/
/*                                                                                      */
/* Details  -Show equipment information screen 			            					*/
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
/*                     (O) s_displayPatientCumulativeHours : display patient cumulative */
/*                     (O) s_displayUintCumulativeHours : display unit cumulative       */
/*                     (O) s_displaySerialNumber : display serial number		        */
/*                     (O) s_displayCalendar : display calendar					        */
/*                     (O) s_displayFlowrateSetting : display flow rate setting	        */
/*                     (O) g_equipmentInforScreen : equipment infor screen		        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentInforScreen_Show(void)
{
	alarmInfor_ShowAlarmInfor();

	EDIT_SetText(s_displayPatientCumulativeHours ,(char*)stringtools_GetCumlativeString(patients));
	EDIT_SetText(s_displayUintCumulativeHours ,(char*)stringtools_GetCumlativeString(uints));
	int8_t serialNumber[SERIAL_NUMBER_BUFFER_SIZE];
	guiInterface_GetSerialNumber(serialNumber);
	EDIT_SetText(s_displaySerialNumber, (char*)serialNumber);
	EDIT_SetText(s_displayCalendar,(char*)stringtools_GetDateString());

	guihandler_StartTimerUpdateEquipInfor();

	int8_t buffer[SIZE_BUFFER_FLOW_RATE] = {0};
	double floatValue = (double)guiInterface_GetSettingValue(eFlowRateSettingId)/FLOWRATE_SCALE;

	sprintf((char*)buffer,"%0.2f",floatValue);
	EDIT_SetText(s_displayFlowrateSetting ,(char*)buffer);

	WM_ShowWindow(g_equipmentInforScreen);
	WM_BringToTop(g_equipmentInforScreen);
	WM_SetFocus(g_equipmentInforScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentInforScreen_Hide 								    				*/
/*                                                                                      */
/* Details  -Hide equipment information screen 			            					*/
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
/*                     (O) g_equipmentInforScreen : equipment infor screen        		*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentInforScreen_Hide(void)
{
	guihandler_StopTimerUpdateEquipInfor();
	WM_HideWindow(g_equipmentInforScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentInforScreen_UpdatePSA				 								*/
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
/* Using Global Data : (I) g_equipmentInforScreen : equipment infor screen				*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentInforScreen_UpdatePSA(IPC_MSG_DATA_t data)
{
	if(WM_IsVisible(g_equipmentInforScreen))
	{
		dataDisplay_SetValueData(eValueOfMeasurementFREquipmentInforDataId, data.stPsaMonitorPacket.flowMeasure);
		dataDisplay_SetValueData(eValueOfOxgEquipmentInforDataId, data.stPsaMonitorPacket.o2Concentrator);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentInforScreen_UpdateCalendarCumulative				 				*/
/*                                                                                      */
/* Details  -Update calendar and cumulative hour in service mode						*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : NONE																		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_equipmentInforScreen : equipment infor screen				*/
/*                     (O) s_displayPatientCumulativeHours : display patient cumulative */
/*                     (O) s_displayUintCumulativeHours : display unit cumulative 		*/
/*                     (O) s_displayCalendar : display calendar					 		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentInforScreen_UpdateCalendarCumulative(void)
{
	if(WM_IsVisible(g_equipmentInforScreen))
	{
		if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
		{
			EDIT_SetText(s_displayPatientCumulativeHours ,(char*)stringtools_GetCumlativeString(patients));
			EDIT_SetText(s_displayUintCumulativeHours ,(char*)stringtools_GetCumlativeString(uints));
		}
		EDIT_SetText(s_displayCalendar,(char*)stringtools_GetDateString());
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentInforScreen_ConfirmChildLock 								       	*/
/*                                                                                      */
/* Details  -Confirm flow rate when child lock on							   			*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) uint8_t FRPSAValue : - flow rate change					 		    */
/*					- range : 5-60														*/
/*                  - unit : LPM                                                        */
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
void equipmentInforScreen_ConfirmChildLock(uint8_t FRPSAValue)
{
	guiTask_SetBlinkingFR(false);

	if(guiInterface_GetSettingValue(eChildLockSettingId) == eOn)
	{
		if(guiTask_GetFlagServiceScreen() == true)
		{
			uint8_t flowRate = guiTask_GetFlowrate();
			uint8_t penddingFlowRate = guiTask_GetPenddingFlowRate();
			uint8_t flowrateSetting = 0;
			if(flowRate != penddingFlowRate)
			{
				guiInterface_SetSettingValue(eFlowRateSettingId,guiTask_GetFlowrate());
				setting_SaveAllSettingValue();

				flowrateSetting = guiInterface_GetSettingValue(eFlowRateSettingId);

				if(flowrateSetting >= SETTING_FR_025L && flowrateSetting <= SETTING_FR_300L)
				{
					guiTask_SendPSAParaEvent(ePacketPSAParaChangeId,flowrateSetting,
							guiTask_GetConditionO2(),guiTask_GetConditionTemp());
				}
				logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eChangingFlowRateLogId);
			}
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-equipmentInforScreen_ChangeFlowrate								       	*/
/*                                                                                      */
/* Details  -Change flow rate by encoder							   					*/
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
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_timerChildLock5s : timer 5 seconds when change flow in child lock*/
/*                     (O) s_timerSendDevice : timer 1 seconds send flow to device      */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void equipmentInforScreen_ChangeFlowrate(uint8_t FRPSAValue)
{
	if(guiTask_GetFlagServiceScreen() == true)
	{
		int8_t buffer[SIZE_BUFFER_FLOW_RATE];
		double floatValue = (double)FRPSAValue/FLOWRATE_SCALE;

		sprintf((char*)buffer,"%0.2f",floatValue);
		if(WM_IsVisible(g_equipmentInforScreen))
		{
			EDIT_SetText(s_displayFlowrateSetting ,(char*)buffer);
		}
		monitorScreen_ChangeFlowrate(FRPSAValue);
		monitorGraphScreen_ChangeFlowrate(FRPSAValue);
		if(guiInterface_GetSettingValue(eChildLockSettingId) == eOn)
		{
			WM_RestartTimer(s_timerChildLock5s,(5*TIMER_1_SECONDS));
			guiTask_SetBlinkingFR(true);
		}
		else
		{
			/** >>#0007
				Remove play flow rate audio immediately when change flow rate. Play flow rate audio after 1 seconds.
			#0007 >>**/
			WM_RestartTimer(s_timerSendDevice,TIMER_1_SECONDS);
		}
	}

	return;
}

/* end of files */
