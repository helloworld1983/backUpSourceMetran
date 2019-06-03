/********************************************************************/
/*                                                                  */
/* File Name    : MonitorScreen.c                            		*/
/*                                                                  */
/* General       : Displaying data in monitor PSA					*/
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
/* #0001       2017/10/27	   Viet Le				modify			*/
/*	-Edit function : EDIT_SetFont()->guiTaskEDIT_SetFont()(bug 1997)*/
/*                                                                  */
/********************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "board.h"
#include "WM.h"
#include "MonitorScreen.h"
#include "BUTTON.h"
#include "MainScreen.h"
#include "StringTable.h"
#include "DataDisplay.h"
#include "EDIT.h"
#include "setting.h"
#include "GuiTask.h"
#include "Font.h"
#include "MaintenanceScreen.h"
#include "OptionScreen.h"
#include "MonitorGraphScreen.h"
#include "AlarmInfor.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

//Define item id
enum{
	eNextBtnMonitorPSAId,
	eSendDeviceTimerInMonitorPSAId
};

WM_HWIN g_monitorScreen;

static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static BUTTON_Handle s_nextBtn;
static EDIT_Handle s_FlowRateSetting;
static EDIT_Handle s_measurementFlowRateData;
static EDIT_Handle s_oxyconcentratorData;
static EDIT_Handle s_temperatureUnitData;
static EDIT_Handle s_temperatureCompOutletData;
static EDIT_Handle s_productTankData;
static EDIT_Handle s_cannulaPressureData;
static EDIT_Handle s_compPosPressureData;
static EDIT_Handle s_compNegativePressureData;
static EDIT_Handle s_chanelSwitchingPressureData;
static EDIT_Handle s_valve1Data;
static EDIT_Handle s_valve2Data;
static EDIT_Handle s_valve3Data;
static EDIT_Handle s_valve4Data;
static EDIT_Handle s_valve5Data;
static EDIT_Handle s_compressorMotorData;
static EDIT_Handle s_coolingFANData;
static EDIT_Handle s_psaCycleTimeData;
static EDIT_Handle s_psaParameterData;

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
/* General 	-monitorScreen_CallbackBackBtn 								                */
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
static void monitorScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			monitorScreen_Hide();
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
/* General 	-monitorScreen_CallbackHomeBtn 								                */
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
static void monitorScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			monitorScreen_Hide();
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
/* General 	-monitorScreen_Callback								                        */
/*                                                                                      */
/* Details  -Call back function for monitor screen, handle event from 					*/
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
void monitorScreen_Callback(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Monitor", 400,5);

			guiTask_SetFont(&GUI_FontEng3L10B2PP);

			//Name flow rate setting
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
			GUI_SetTextAlign(GUI_TA_RIGHT);
			GUI_DispStringAt("Setting flow rate:",200, 52);

			//Unit flow rate setting
			GUI_SetColor(GUI_BLACK);
			GUI_DispStringAt("L/min",326,52);
		}
		else
		{
			GUI_SetColor(GUI_BLACK);
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("モニタ", 400,5);

			guiTask_SetFont(&GUI_FontJapan3L12B2PP);

			//Name flow rate setting
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
			GUI_SetTextAlign(GUI_TA_RIGHT);
			GUI_DispStringAt("設定流量:",200, 52);

			//Unit flow rate setting
			GUI_SetColor(GUI_BLACK);
			GUI_DispStringAt("L/分",326,52);
		}

		guiTask_SetFont(&GUI_FontEng3L18B2PP);
		GUI_DispStringHCenterAt("1/2", 400,440);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-monitorScreen_CallbackNextBtn 								                */
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
void monitorScreen_CallbackNextBtn(WM_MESSAGE * pMsg)
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
			monitorScreen_Hide();
			monitorGraphScreen_Show();
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
/* General 	-monitorScreen_Init				 								    		*/
/*                                                                                      */
/* Details  -Initialize monitor screen				 									*/
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
/*                     (O) g_monitorScreen : monitor screen                             */
/*                     (O) s_FlowRateSetting : display flow rate setting	            */
/*                     (O) s_measurementFlowRateData : display measurement flow			*/
/*                     (O) s_oxyconcentratorData : display O2 percent		        	*/
/*                     (O) s_temperatureUnitData : display unit temperature            	*/
/*                     (O) s_temperatureCompOutletData : display compressor outlet temperature*/
/*                     (O) s_productTankData : display product tank pressure	        */
/*                     (O) s_cannulaPressureData : display cannula pressure		        */
/*                     (O) s_compPosPressureData : display pressure sensor 1	        */
/*                     (O) s_compNegativePressureData : display pressure sensor 2	    */
/*                     (O) s_chanelSwitchingPressureData : display ext flow path pressure*/
/*                     (O) s_valve1Data : display status valve 1				     	*/
/*                     (O) s_valve2Data : display status valve 2				     	*/
/*                     (O) s_valve3Data : display status valve 3				     	*/
/*                     (O) s_valve4Data : display status valve 4				     	*/
/*                     (O) s_valve5Data : display status valve 5				     	*/
/*                     (O) s_compressorMotorData : display compressor motor	data		*/
/*                     (O) s_coolingFANData : display cooling fan data			        */
/*                     (O) s_psaCycleTimeData : display psa cycle time		           	*/
/*                     (O) s_psaParameterData : display psa parameter                   */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_nextBtn : next button						                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void monitorScreen_Init(void)
{
	g_monitorScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,monitorScreen_Callback,0);

	s_FlowRateSetting = EDIT_CreateEx(200,45,125,30,g_monitorScreen,WM_CF_SHOW,0,eFlowRateSettingStringID,5);
	EDIT_SetFocussable(s_FlowRateSetting,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_FlowRateSetting ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_FlowRateSetting ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_measurementFlowRateData = EDIT_CreateEx(0, 80, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,eMeasurementFRDataStringID,50);
	EDIT_SetFocussable(s_measurementFlowRateData,EDIT_CI_DISABLED);
	WM_SetCallback(s_measurementFlowRateData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_measurementFlowRateData,eValueOfMeasurementFRDataId);
	EDIT_Invalidate(s_measurementFlowRateData);

	s_oxyconcentratorData = EDIT_CreateEx(0, 115, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,eO2PercentDataStringID,50);
	EDIT_SetFocussable(s_oxyconcentratorData,EDIT_CI_DISABLED);
	WM_SetCallback(s_oxyconcentratorData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_oxyconcentratorData,eValueOfOxgDataId);
	EDIT_Invalidate(s_oxyconcentratorData);

	s_temperatureUnitData = EDIT_CreateEx(0, 150, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,eTemperatureUnitDataStringID,50);
	EDIT_SetFocussable(s_temperatureUnitData,EDIT_CI_DISABLED);
	WM_SetCallback(s_temperatureUnitData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_temperatureUnitData,eValueOfTempUnitMonitorPSADatatId);
	EDIT_Invalidate(s_temperatureUnitData);

	s_temperatureCompOutletData =  EDIT_CreateEx(0, 185, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,eTemperatureCompOrOutletDataStringID,50);
	EDIT_SetFocussable(s_temperatureCompOutletData,EDIT_CI_DISABLED);
	WM_SetCallback(s_temperatureCompOutletData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_temperatureCompOutletData,eValueOfTempCompMonitorPSADataId);
	EDIT_Invalidate(s_temperatureCompOutletData);

	s_productTankData = EDIT_CreateEx(0, 220, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,eProductTankMonitorPSAStringId,50);
	EDIT_SetFocussable(s_productTankData,EDIT_CI_DISABLED);
	WM_SetCallback(s_productTankData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_productTankData,eValueOfProductTankPressMonitorPSADataId);
	EDIT_Invalidate(s_productTankData);

	s_cannulaPressureData = EDIT_CreateEx(0, 255, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,ePressure2DataStringID,50);
	EDIT_SetFocussable(s_cannulaPressureData,EDIT_CI_DISABLED);
	WM_SetCallback(s_cannulaPressureData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_cannulaPressureData,eValueOfCannulaPressDataId);
	EDIT_Invalidate(s_cannulaPressureData);

	s_compPosPressureData = EDIT_CreateEx(0, 290, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,eCompPositivePressForceStringID,50);
	EDIT_SetFocussable(s_compPosPressureData,EDIT_CI_DISABLED);
	WM_SetCallback(s_compPosPressureData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_compPosPressureData,eValueOfCompPositivePressDataId);
	EDIT_Invalidate(s_compPosPressureData);

	s_compNegativePressureData = EDIT_CreateEx(0, 325, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,eCompNegativePressForceStringID,50);
	EDIT_SetFocussable(s_compNegativePressureData,EDIT_CI_DISABLED);
	WM_SetCallback(s_compNegativePressureData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_compNegativePressureData,eValueOfCompNegativePressDataId);
	EDIT_Invalidate(s_compNegativePressureData);

	s_chanelSwitchingPressureData = EDIT_CreateEx(0, 360, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,eChanelSwitchingPressStringID,50);
	EDIT_SetFocussable(s_chanelSwitchingPressureData,EDIT_CI_DISABLED);
	WM_SetCallback(s_chanelSwitchingPressureData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_chanelSwitchingPressureData,eValueOfChanelSwitchPressDataId);
	EDIT_Invalidate(s_chanelSwitchingPressureData);

	s_valve1Data = EDIT_CreateEx(400, 45, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,eValve1DataStringId,50);
	EDIT_SetFocussable(s_valve1Data,EDIT_CI_DISABLED);
	WM_SetCallback(s_valve1Data,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_valve1Data,eValueOfValve1DataId);
	EDIT_Invalidate(s_valve1Data);

	s_valve2Data = EDIT_CreateEx(400, 80, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,eValve2DataStringId,50);
	EDIT_SetFocussable(s_valve2Data,EDIT_CI_DISABLED);
	WM_SetCallback(s_valve2Data,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_valve2Data,eValueOfValve2DataId);
	EDIT_Invalidate(s_valve2Data);

	s_valve3Data = EDIT_CreateEx(400, 115, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,eValve3DataStringId,50);
	EDIT_SetFocussable(s_valve3Data,EDIT_CI_DISABLED);
	WM_SetCallback(s_valve3Data,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_valve3Data,eValueOfValve3DataId);
	EDIT_Invalidate(s_valve3Data);

	s_valve4Data = EDIT_CreateEx(400, 150, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,eValve4DataStringId,50);
	EDIT_SetFocussable(s_valve4Data,EDIT_CI_DISABLED);
	WM_SetCallback(s_valve4Data,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_valve4Data,eValueOfValve4DataId);
	EDIT_Invalidate(s_valve4Data);

	s_valve5Data = EDIT_CreateEx(400, 185, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,eValve5DataStringId,50);
	EDIT_SetFocussable(s_valve5Data,EDIT_CI_DISABLED);
	WM_SetCallback(s_valve5Data,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_valve5Data,eValueOfValve5DataId);
	EDIT_Invalidate(s_valve5Data);

	s_compressorMotorData = EDIT_CreateEx(400, 220, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,eCompressorRPMDataStringId,50);
	EDIT_SetFocussable(s_compressorMotorData,EDIT_CI_DISABLED);
	WM_SetCallback(s_compressorMotorData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_compressorMotorData,eValueOfCompressorRPMDataId);
	EDIT_Invalidate(s_compressorMotorData);

	s_coolingFANData = EDIT_CreateEx(400, 255, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,eCoolingFANDataStringId,50);
	EDIT_SetFocussable(s_coolingFANData,EDIT_CI_DISABLED);
	WM_SetCallback(s_coolingFANData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_coolingFANData,eValueOfCoolingFANDataId);
	EDIT_Invalidate(s_coolingFANData);

	s_psaCycleTimeData = EDIT_CreateEx(400, 290, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,ePSACycleTimeStringId,50);
	EDIT_SetFocussable(s_psaCycleTimeData,EDIT_CI_DISABLED);
	WM_SetCallback(s_psaCycleTimeData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_psaCycleTimeData,eValueOfPSACycleTimeDataId);
	EDIT_Invalidate(s_psaCycleTimeData);

	s_psaParameterData = EDIT_CreateEx(400, 325, 370, 40,g_monitorScreen,WM_CF_SHOW | WM_CF_MEMDEV_ON_REDRAW,0,ePSAParameterDataStringId,50);
	EDIT_SetFocussable(s_psaParameterData,EDIT_CI_DISABLED);
	WM_SetCallback(s_psaParameterData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_psaParameterData,eValueOfPSAParameterDataId);
	EDIT_Invalidate(s_psaParameterData);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_monitorScreen, WM_CF_SHOW, 0, eBackBtnMonitorPSAStringId);
	WM_SetCallback(s_backBtn,monitorScreen_CallbackBackBtn);

	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_monitorScreen, WM_CF_SHOW, 0, eHomeBtnMonitorPSAStringId);
	WM_SetCallback(s_homeBtn,monitorScreen_CallbackHomeBtn);

	//Next button
	s_nextBtn = BUTTON_CreateEx(700, 415, 100, 60, g_monitorScreen, WM_CF_SHOW, 0, eNextBtnMonitorPSAId);
	WM_SetCallback(s_nextBtn,monitorScreen_CallbackNextBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-monitorScreen_Show				 								    		*/
/*                                                                                      */
/* Details  -Show monitor screen				 										*/
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
/*                     (O) s_FlowRateSetting : display flow rate setting 			   	*/
/*                     (O) g_monitorScreen : monitor screen						        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void monitorScreen_Show(void)
{
	alarmInfor_ShowAlarmInfor();

	int8_t buffer[SIZE_BUFFER_FLOW_RATE] = {'\0'};
	double floatValue = (double)guiInterface_GetSettingValue(eFlowRateSettingId)/FLOWRATE_SCALE;
	sprintf((char*)buffer,"%0.2f",floatValue);
	EDIT_SetText(s_FlowRateSetting ,(char*)buffer);

	WM_ShowWindow(g_monitorScreen);
	WM_BringToTop(g_monitorScreen);
	WM_SetFocus(g_monitorScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-monitorScreen_Hide				 								    		*/
/*                                                                                      */
/* Details  -Hide monitor screen				 										*/
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
/*                     (O) g_monitorScreen : monitor screen                             */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void monitorScreen_Hide(void)
{
	WM_HideWindow(g_monitorScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-monitorScreen_UpdateValve				 								    */
/*                                                                                      */
/* Details  -Update monitor valve in maintenance mode				 					*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) ValStatus status : - current status of valve							*/
/*						- range : 0-2													*/
/*                      - unit : NONE                                                   */
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
void monitorScreen_UpdateValve(E_ValStatus status)
{
	dataDisplay_SetStringValveData(eValueOfValve1DataId, status);
	dataDisplay_SetStringValveData(eValueOfValve2DataId, status);
	dataDisplay_SetStringValveData(eValueOfValve3DataId, status);
	dataDisplay_SetStringValveData(eValueOfValve4DataId, status);
	dataDisplay_SetStringValveData(eValueOfValve5DataId, status);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-monitorScreen_UpdatePsaPara				 								*/
/*                                                                                      */
/* Details  -Update monitor valve in maintenance mode				 					*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) PsaSelectName psaSelect :  - current select psa					    */
/*						- range : 0-5													*/
/*                      - unit : NONE                                                   */
/*																						*/
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
void monitorScreen_UpdatePsaPara(E_PsaSelectName psaSelect)
{
	dataDisplay_SetStringPsaSelectData(eValueOfPSAParameterDataId, psaSelect);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-monitorScreen_UpdatePSA			 								    	*/
/*                                                                                      */
/* Details  -Update monitor PSA in maintenance mode				 						*/
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
/* Using Global Data : (I) g_monitorScreen : monitor screen							    */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void monitorScreen_UpdatePSA(IPC_MSG_DATA_t data)
{
	if(WM_IsVisible(g_monitorScreen))
	{
		dataDisplay_SetValueData(eValueOfProductTankPressMonitorPSADataId, data.stPsaMonitorPacket.productankPress);
		dataDisplay_SetValueData(eValueOfCompPositivePressDataId, data.stPsaMonitorPacket.compPosPress);
		dataDisplay_SetValueData(eValueOfCompNegativePressDataId, data.stPsaMonitorPacket.compNegPress);
		dataDisplay_SetValueData(eValueOfChanelSwitchPressDataId, data.stPsaMonitorPacket.extFlowPathPress);
		dataDisplay_SetValueData(eValueOfCannulaPressDataId, data.stPsaMonitorPacket.cannulaPress);
		dataDisplay_SetValueData(eValueOfTempCompMonitorPSADataId, data.stPsaMonitorPacket.compTemperature);
		dataDisplay_SetValueData(eValueOfMeasurementFRDataId, data.stPsaMonitorPacket.flowMeasure);
		dataDisplay_SetValueData(eValueOfOxgDataId, data.stPsaMonitorPacket.o2Concentrator);
		dataDisplay_SetValueData(eValueOfTempUnitMonitorPSADatatId,data.stPsaMonitorPacket.temperature);

		dataDisplay_SetStringValveData(eValueOfValve1DataId, data.stPsaMonitorPacket.val1Status);
		dataDisplay_SetStringValveData(eValueOfValve2DataId, data.stPsaMonitorPacket.val2Status);
		dataDisplay_SetStringValveData(eValueOfValve3DataId, data.stPsaMonitorPacket.val3Status);
		dataDisplay_SetStringValveData(eValueOfValve4DataId, data.stPsaMonitorPacket.val4Status);
		dataDisplay_SetStringValveData(eValueOfValve5DataId, data.stPsaMonitorPacket.val5Status);

		dataDisplay_SetValueData(eValueOfCompressorRPMDataId,data.stPsaMonitorPacket.compessorRpm);
		dataDisplay_SetValueData(eValueOfCoolingFANDataId,data.stPsaMonitorPacket.fanDuty);

		dataDisplay_SetValueData(eValueOfPSACycleTimeDataId,(uint16_t)(data.stPsaMonitorPacket.psaCycleTime/10));

		dataDisplay_SetStringPsaSelectData(eValueOfPSAParameterDataId, data.stPsaMonitorPacket.psaParamSelect);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-monitorScreen_ChangeFlowrate 								       			*/
/*                                                                                      */
/* Details  -Change flow rate by encoder							   					*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) uint8_t FRPSAValue : - flow rate change					 			*/
/*						- range : 5-60													*/
/*                      - unit : NONE                                        			*/
/*																						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_monitorScreen : monitor screen							    */
/*                     (O) s_FlowRateSetting : display flow rate setting         		*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void monitorScreen_ChangeFlowrate(uint8_t FRPSAValue)
{
	if(WM_IsVisible(g_monitorScreen))
	{
		int8_t buffer[SIZE_BUFFER_FLOW_RATE] = {'\0'};
		double floatValue = (double)FRPSAValue/FLOWRATE_SCALE;

		sprintf((char*)buffer,"%0.2f",floatValue);
		EDIT_SetText(s_FlowRateSetting ,(char*)buffer);
	}

	return;
}

/* end of files */
