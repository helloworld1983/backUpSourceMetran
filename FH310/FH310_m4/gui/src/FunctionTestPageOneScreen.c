/********************************************************************/
/*                                                                  */
/* File Name    : FunctionTestPageOneScreen.c                       */
/*                                                                  */
/* General       : The FunctionTestPageOne screen allows user to    */
/*                 monitor the current system information in        */
/* 				   numerical and graphicaldisplay such as:          */
/* 				   Temperature (Unit internal),                     */
/*   			   Temperature (compressor outlet),                 */
/* 				   Oxygen concentration,			                */
/*                 Measurement flow rate,                           */
/*                 Voltage of Illuminance Sensor,			        */
/*                 Product tank pressure,                           */
/*                 Cannula pressure, 						        */
/*                 Compressor positive pressure,                    */
/*                 Compressor negative pressure,	                */
/*                 External flow path pressure,                     */
/*                 Compressor driver board 			                */
/*                 software version,                                */
/*                 Flow controller software version, 			    */
/*                 Boot loader software version, 					*/
/*                 Backup software software version		            */
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
/*  #0001	   2017/10/27      Viet Le 	    	        modify	    */
/*	  Edit : EDIT_SetFont()->guiTaskEDIT_SetFont(bug 1997)			*/
/********************************************************************/

#include <StringTools.h>
#include "WM.h"
#include "FunctionTestPageOneScreen.h"
#include "board.h"
#include "TEXT.h"
#include "BUTTON.h"
#include "DataDisplay.h"
#include "SettingOnOffButton.h"
#include "SettingOpenCloseButton.h"
#include "SettingButton.h"
#include "MainScreen.h"
#include "StringTable.h"
#include "TextLibrary.h"
#include "setting.h"
#include "EDIT.h"
#include "GuiTask.h"
#include "Font.h"
#include "MaintenanceScreen.h"
#include "OptionScreen.h"
#include "FunctionTestPageTwoScreen.h"
#include "AlarmInfor.h"
#include "GuiInterface.h"

//Define item id
enum{
	eCompVersionId,
	eFlowVersionId,
	eBackupSoftVersionId,
	eBootVersionId
};

WM_HWIN g_functionTestPageOneScreen;
static BUTTON_Handle s_nextBtn;
static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static EDIT_Handle s_tempUnitData;
static EDIT_Handle s_measurementFRData;
static EDIT_Handle s_OxygenConcentrationData;
static EDIT_Handle s_tempCompData;
static EDIT_Handle s_productTankPressData;
static EDIT_Handle s_cannulaPressData;
static EDIT_Handle s_IlluminanceSensorVoltageData;
static EDIT_Handle s_compPosPressData;
static EDIT_Handle s_compNegativePressData;
static EDIT_Handle s_chanelSwitchingPressData;
static EDIT_Handle s_compVersion;
static EDIT_Handle s_flowControlVersion;
static EDIT_Handle s_backupSoftwareVersion;
static EDIT_Handle s_bootVersion;

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
/* General 	-functionTestPageOneScreen_CallbackNextBtn	                                */
/*                                                                                      */
/* Details  -call back function for next button, handle event from window		        */
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
void functionTestPageOneScreen_CallbackNextBtn(WM_MESSAGE * pMsg)
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
			functionTestPageOneScreen_Hide();
			functionTestPageTwoScreen_Show();
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
/* General 	-functionTestPageOneScreen_CallbackBackBtn	                                */
/*                                                                                      */
/* Details  -call back function for back button, handle event from window		        */
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
void functionTestPageOneScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			functionTestPageOneScreen_Hide();
			settingOnOffButton_SetOffAllDevice();
			settingOpenCloseButton_SetOffLatchingValve();
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
/* General 	-functionTestPageOneScreen_CallbackHomeBtn	                                */
/*                                                                                      */
/* Details  -call back function for home button, handle event from window		        */
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
/* Using Global Data : (I) g_mainWindow : main window							        */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageOneScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			functionTestPageOneScreen_Hide();
			settingOnOffButton_SetOffAllDevice();
			settingOpenCloseButton_SetOffLatchingValve();
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
/* General 	-functionTestPageOneScreen_Callback	                                        */
/*                                                                                      */
/* Details  -Call back function for function test page one, handle event from window	*/
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
void functionTestPageOneScreen_Callback(WM_MESSAGE * pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(GUI_BLACK);
		GUI_AA_DrawRoundedRect(10,55,790,240,10);
		GUI_AA_DrawRoundedRect(10,255,790,410,10);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Function test", 400,5);

			guiTask_SetFont(&GUI_FontEng3L12B2PP);
			GUI_DispStringAt("Monitor",30,45);
			GUI_DispStringAt("Device information,Attendant software information",30,245);
			GUI_DispStringAt("Compressor driver software version:",60,270);
			GUI_DispStringAt("Flow controller software version:",87,305);
			GUI_DispStringAt("Boot loader software version:",115,340);
			GUI_DispStringAt("BT module software version:",124,375);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("機能テスト", 400,5);

			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringAt("モニタ",30,45);
			GUI_DispStringAt("デバイス情報・付帯ソフトウェア情報",30,245);

			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("コンプレッサドライバ基板ソフトウェアバージョン:",17,270);
			GUI_DispStringAt("フローコントローラソフトウェアバージョン:",69,305);
			GUI_DispStringAt("ブートローダソフトウェアバージョン:",118,340);
			GUI_DispStringAt("BTモジュールソフトウェアバージョン:",111,375);
		}
		guiTask_SetFont(&GUI_FontEng3L18B2PP);
		GUI_DispStringHCenterAt("1/4", 400,440);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-functionTestPageOneScreen_Init 							                */
/*                                                                                      */
/* Details  -Initialize function test page one 			            	                */
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
/*                     (O) g_functionTestPageOneScreen : function test page one         */
/*                     (O) s_tempUnitData : display unit temperature					*/
/*                     (O) s_tempCompData : display compressor temperature              */
/*                     (O) s_OxygenConcentrationData : display O2 concentration			*/
/*                     (O) s_measurementFRData : display measurement flow               */
/*                     (O) s_IlluminanceSensorVoltageData : display Illuminance Sensor 	*/
/*                     (O) s_productTankPressData : display product tank pressure       */
/*                     (O) s_cannulaPressData : display cannula pressure				*/
/*                     (O) s_compPosPressData : display pressor sensor 1                */
/*                     (O) s_compNegativePressData : display pressor sensor 2			*/
/*                     (O) s_chanelSwitchingPressData : display ext flow path pressure  */
/*                     (O) s_compVersion : display compressor version					*/
/*                     (O) s_flowControlVersion : display flow controller version       */
/*                     (O) s_bootVersion : display bootloader version					*/
/*                     (O) s_backupSoftwareVersion : display control SW version         */
/*                     (O) s_nextBtn : next button						                */
/*                     (O) s_backBtn : back button                                      */
/*                     (O) s_homeBtn : home button						                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageOneScreen_Init(void)
{
	g_functionTestPageOneScreen = WM_CreateWindowAsChild(FUNCTIONTEST_PAGEONE_X, FUNCTIONTEST_PAGEONE_Y, FUNCTIONTEST_PAGEONE_SIZE_X,
			FUNCTIONTEST_PAGEONE_SIZE_Y,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,functionTestPageOneScreen_Callback,0);

	s_tempUnitData = EDIT_CreateEx(30, 65, 370, 40,g_functionTestPageOneScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eTemperatureUnitDataStringID,50);
	WM_SetCallback(s_tempUnitData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_tempUnitData,eValueOfTempUniFunctionTestDatatId);
	EDIT_Invalidate(s_tempUnitData);

	s_tempCompData = EDIT_CreateEx(30, 98, 370, 40,g_functionTestPageOneScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eTemperatureCompOrOutletDataStringID,50);
	WM_SetCallback(s_tempCompData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_tempCompData,eValueOfTempCompFunctionTestDataId);

	s_OxygenConcentrationData = EDIT_CreateEx(30, 131, 370, 40,g_functionTestPageOneScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eO2PercentDataStringID,50);
	WM_SetCallback(s_OxygenConcentrationData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_OxygenConcentrationData,eValueOfOxgFunctionTestDataId);

	s_measurementFRData = EDIT_CreateEx(30, 164, 370, 40,g_functionTestPageOneScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eMeasurementFRDataStringID,50);
	WM_SetCallback(s_measurementFRData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_measurementFRData,eValueOfMeasurementFRFunctionTestDataId);

	s_IlluminanceSensorVoltageData = EDIT_CreateEx(30, 197, 370, 40,g_functionTestPageOneScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eIlluminanceSensorVoltageStringID,50);
	WM_SetCallback(s_IlluminanceSensorVoltageData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_IlluminanceSensorVoltageData,eValueOfIlluminanceSensorVoltageDataId);

	s_productTankPressData = EDIT_CreateEx(418, 65, 370, 40,g_functionTestPageOneScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,ePressure1DataStringID,50);
	WM_SetCallback(s_productTankPressData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_productTankPressData,eValueOfProductTankPressFunctionTestDataId);

	s_cannulaPressData = EDIT_CreateEx(418, 98, 370, 40,g_functionTestPageOneScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,ePressure2DataStringID,50);
	WM_SetCallback(s_cannulaPressData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_cannulaPressData,eValueOfCannulaPressFunctionTestDataId);

	s_compPosPressData = EDIT_CreateEx(418, 131, 370, 40,g_functionTestPageOneScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eCompPositivePressForceStringID,50);
	WM_SetCallback(s_compPosPressData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_compPosPressData,eValueOfCompPositivePressFunctionTestDataId);

	s_compNegativePressData = EDIT_CreateEx(418, 164, 370, 40,g_functionTestPageOneScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eCompNegativePressForceStringID,50);
	WM_SetCallback(s_compNegativePressData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_compNegativePressData,eValueOfCompNegativePressFunctionTestDataId);

	s_chanelSwitchingPressData = EDIT_CreateEx(418, 197, 370, 40,g_functionTestPageOneScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eChanelSwitchingPressStringID,50);
	WM_SetCallback(s_chanelSwitchingPressData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_chanelSwitchingPressData,eValueOfChanelSwitchPressFunctionTestDataId);

	s_compVersion = EDIT_CreateEx(415,265,300,30,g_functionTestPageOneScreen,WM_CF_SHOW,0,eCompVersionId,10);
	EDIT_SetFocussable(s_compVersion,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_compVersion,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_compVersion,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_flowControlVersion = EDIT_CreateEx(415,300,300,30,g_functionTestPageOneScreen,WM_CF_SHOW,0,eFlowVersionId,10);
	EDIT_SetFocussable(s_flowControlVersion,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_flowControlVersion,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_flowControlVersion,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_bootVersion = EDIT_CreateEx(415,335,300,30,g_functionTestPageOneScreen,WM_CF_SHOW,0,eBootVersionId,50);
	EDIT_SetFocussable(s_bootVersion,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_bootVersion,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_bootVersion,GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_bootVersion,(char*)stringtools_GetBootVersion());

	s_backupSoftwareVersion = EDIT_CreateEx(415,370,300,30,g_functionTestPageOneScreen,WM_CF_SHOW,0,eBackupSoftVersionId,50);
	EDIT_SetFocussable(s_backupSoftwareVersion,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_backupSoftwareVersion,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_backupSoftwareVersion,GUI_TA_VCENTER | GUI_TA_HCENTER);

	//Next button
	s_nextBtn = BUTTON_CreateEx(700, 415, 100, 60, g_functionTestPageOneScreen, WM_CF_SHOW, 0, GUI_ID_BUTTON0);
	WM_SetCallback(s_nextBtn,functionTestPageOneScreen_CallbackNextBtn);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_functionTestPageOneScreen, WM_CF_SHOW, 0, eBackBtnFunctionTestPageOneStringId);
	WM_SetCallback(s_backBtn,functionTestPageOneScreen_CallbackBackBtn);

	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_functionTestPageOneScreen, WM_CF_SHOW, 0, eHomeBtnFunctionTestPageOneStringId);
	WM_SetCallback(s_homeBtn,functionTestPageOneScreen_CallbackHomeBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-functionTestPageOneScreen_Show 							                */
/*                                                                                      */
/* Details  -Show function test page one 			            	                    */
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
/*                     (O) g_functionTestPageOneScreen : function test page one         */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageOneScreen_Show(void)
{
	alarmInfor_ShowAlarmInfor();
	WM_ShowWindow(g_functionTestPageOneScreen);
	WM_BringToTop(g_functionTestPageOneScreen);
	WM_SetFocus(g_functionTestPageOneScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-functionTestPageOneScreen_Hide 							                */
/*                                                                                      */
/* Details  -Hide function test page one 			            	                    */
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
/*                     (O) g_functionTestPageOneScreen : function test page one         */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageOneScreen_Hide(void)
{
	WM_HideWindow(g_functionTestPageOneScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-functionTestPageOneScreen_UpdateMonitor				 					*/
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
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageOneScreen_UpdateMonitor(IPC_MSG_DATA_t data)
{
	if(WM_IsVisible(g_functionTestPageOneScreen))
	{
		dataDisplay_SetValueData(eValueOfTempUniFunctionTestDatatId, data.stPsaMonitorPacket.temperature);
		dataDisplay_SetValueData(eValueOfTempCompFunctionTestDataId, data.stPsaMonitorPacket.compTemperature);
		dataDisplay_SetValueData(eValueOfOxgFunctionTestDataId, data.stPsaMonitorPacket.o2Concentrator);
		dataDisplay_SetValueData(eValueOfMeasurementFRFunctionTestDataId, data.stPsaMonitorPacket.flowMeasure);
		dataDisplay_SetValueData(eValueOfProductTankPressFunctionTestDataId, data.stPsaMonitorPacket.productankPress);
		dataDisplay_SetValueData(eValueOfCannulaPressFunctionTestDataId, data.stPsaMonitorPacket.cannulaPress);
		dataDisplay_SetValueData(eValueOfCompPositivePressFunctionTestDataId, data.stPsaMonitorPacket.compPosPress);
		dataDisplay_SetValueData(eValueOfCompNegativePressFunctionTestDataId, data.stPsaMonitorPacket.compNegPress);
		dataDisplay_SetValueData(eValueOfChanelSwitchPressFunctionTestDataId, data.stPsaMonitorPacket.extFlowPathPress);
		dataDisplay_SetValueData(eValueOfIlluminanceSensorVoltageDataId, data.stPsaMonitorPacket.illuminanceVoltVal);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-functionTestPageOneScreen_DisplayVersionComp			 					*/
/*                                                                                      */
/* Details  -Display version compressor				 									*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) const int8_t* version : compressor version							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_compVersion : display compressor version                   */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageOneScreen_DisplayVersionComp(const int8_t* version)
{
	EDIT_SetText(s_compVersion,(char*)version);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-functionTestPageOneScreen_DisplayVersionFlow			 					*/
/*                                                                                      */
/* Details  -Display version flow					 									*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) const int8_t* version : flow controller version						*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_flowControlVersion : display flow controller version       */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageOneScreen_DisplayVersionFlow(const int8_t* version)
{
	EDIT_SetText(s_flowControlVersion,(char*)version);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-functionTestPageOneScreen_DisplayBT				 						*/
/*                                                                                      */
/* Details  -Display version BT						 									*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) BT_FW_VERSION_T BTVersion : bluetooth version						*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_backupSoftwareVersion : display bluetooth version          */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void functionTestPageOneScreen_DisplayBT(BT_FW_VERSION_T BTVersion)
{
	int8_t buffer[SIZE_BUFFER_BT_VERSION] = {'\0'};

	sprintf((char*)buffer,"V%d.%d.%d.%d\0",BTVersion.majorNum,BTVersion.minorNum,BTVersion.patchNum,BTVersion.buildNum);

	EDIT_SetText(s_backupSoftwareVersion,(char*)buffer);

	return;
}

/* end of files */
