/********************************************************************/
/*                                                                  */
/* File Name    : DevelopmentSettingScreen.c                        */
/*                                                                  */
/* General       : The user is able to select the enable or			*/
/*  			   disable to each alarm functions. 				*/
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
/*     remove  #include "assert.h"                                  */
/* #0002	   2017/10/27      Viet Le 	    	        modify	    */
/*	  Edit : RADIO_SetFont()->guiTaskRADIO_SetFont(bug 1997)		*/
/* #0003	   2017/11/03      Viet Le 	    	        modify	    */
/*	  Add case eCompressorDCOverVoltageId,							*/
/*	eCompressorSoftwareOverCurrentId,eCompressorHardwareOverCurrent */
/*  in developmentSettingScreen_CheckEnable() function (bug 2014)   */
/* #0004	   2017/11/03      Viet Le 	    	        modify	    */
/*  Add case eFlowControllerCommandTimeOutId						*/
/*  in developmentSettingScreen_CheckEnable() function (bug 2015)	*/
/********************************************************************/

#include "string.h"
#include "board.h"
#include "DevelopmentSettingScreen.h"
#include "MainScreen.h"
#include "RADIO.h"
#include "BUTTON.h"
#include "setting.h"
#include "GuiTask.h"
#include "Font.h"
#include "MaintenanceScreen.h"
#include "OptionScreen.h"
#include "DevelopmentSettingScreenPageTwo.h"
#include "AlarmInfor.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

//Define item id
enum
{
	eTemperatureEnable,
	eTemperatureDisable,
	eHighPressureEnable,
	eHighPressureDisable,
	eLowPressureEnable,
	eLowPressureDisable,
	eCompressorEnable,
	eCompressorDisable,
	eCoolingFanEnable,
	eCoolingFanDisable,
	eO2ConcentrationAbnormalEnable,
	eO2ConcentrationAbnormalDisable,
	eCannulaEnable,
	eCannulaDisable,
	eO2ConcentrationLoweringEnable,
	eO2ConcentrationLoweringDisable,
	eO2ConcentrationSensorEnable,
	eO2ConcentrationSensorDisable,
	eFlowRateEnable,
	eFlowRateDisable,
	eFilterCleaningModuleEnable,
	eFilterCleaningModuleDisable,
	eSpeakerEnable,
	eSpeakerDisable,
	eAnypalEnable,
	eAnypalDisable,
	eCompressorOutletTemperatureAbnormalEnable,
	eCompressorOutletTemperatureAbnormalDisable,
	eCompressorOutletTemperatureRisingEnable,
	eCompressorOutletTemperatureRisingDisable,
	eFlowControllerEnable,
	eFlowControllerDisable,
	eNegativePressureEnable,
	eNegativePressureDisable,
	eNonvolatileMemoryEnable,
	eNonvolatileMemoryDisable,
	eExternalFlowPathPressureEnable,
	eExternalFlowPathPressureDisable,
	eBackBtnDevelopmentSetting,
	eHomeBtnDevelopmentSetting,
	eNextBtnDevelopmentSetting,
	eDisableAllAlarmSettingId
};

WM_HWIN g_developmentSettingScreen;

static RADIO_Handle s_abnormalTemperatureEnable;
static RADIO_Handle s_abnormalTemperatureDisable;

static RADIO_Handle s_abnormalHighPressureEnable;
static RADIO_Handle s_abnormalHighPressureDisable;

static RADIO_Handle s_abnormalLowPressureEnable;
static RADIO_Handle s_abnormalLowPressureDisable;

static RADIO_Handle s_abnormalCompressorEnable;
static RADIO_Handle s_abnormalCompressorDisable;

static RADIO_Handle s_abnormalCoolingFanEnable;
static RADIO_Handle s_abnormalCoolingFanDisable;

static RADIO_Handle s_abnormalO2ConcentrationAbnormalEnable;
static RADIO_Handle s_abnormalO2ConcentrationAbnormalDisable;

static RADIO_Handle s_abnormalCannulaEnable;
static RADIO_Handle s_abnormalCannulaDisable;

static RADIO_Handle s_abnormalO2ConcentrationLoweringEnable;
static RADIO_Handle s_abnormalO2ConcentrationLoweringDisable;

static RADIO_Handle s_abnormalO2ConcentrationSensorEnable;
static RADIO_Handle s_abnormalO2ConcentrationSensorDisable;

static RADIO_Handle s_abnormalFlowRateEnable;
static RADIO_Handle s_abnormalFlowRateDisable;

static RADIO_Handle s_abnormalFilterCleaningModuleEnable;
static RADIO_Handle s_abnormalFilterCleaningModuleDisable;

static RADIO_Handle s_abnormalSpeakerEnable;
static RADIO_Handle s_abnormalSpeakerDisable;

static RADIO_Handle s_abnormalAnypalEnable;
static RADIO_Handle s_abnormalAnypalDisable;

static RADIO_Handle s_abnormalCompressorOutletTemperatureAbnormalEnable;
static RADIO_Handle s_abnormalCompressorOutletTemperatureAbnormalDisable;

static RADIO_Handle s_abnormalCompressorOutletTemperatureRisingEnable;
static RADIO_Handle s_abnormalCompressorOutletTemperatureRisingDisable;

static RADIO_Handle s_abnormalFlowControllerEnable;
static RADIO_Handle s_abnormalFlowControllerDisable;

static RADIO_Handle s_abnormalNonvolatileMemoryEnable;
static RADIO_Handle s_abnormalNonvolatileMemoryDisable;

static RADIO_Handle s_abnormalExternalFlowPathPressureEnable;
static RADIO_Handle s_abnormalExternalFlowPathPressureDisable;

static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static BUTTON_Handle s_nextBtn;
static BUTTON_Handle s_disableAllAlarmBtn;

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
/* General 	-developmentSettingScreen_Callback	                                        */
/*                                                                                      */
/* Details  -call back function for PSA parameter page one, handle event from window	*/
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
void developmentSettingScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(GUI_BLACK);
		GUI_AA_DrawRoundedRect(10,60,780,410,10);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Development setting", 400,5);

			guiTask_SetFont(&GUI_FontEng3L12B2PP);
			GUI_DispStringAt("Alarm function",25,50);
			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringAt("Enable",30,70);
			GUI_DispStringAt("Disable",120,70);
			GUI_DispStringAt("Enable",460,70);
			GUI_DispStringAt("Disable",550,70);

			GUI_DispStringAt("Temperature", 150,90);
			GUI_DispStringAt("High pressure", 150,120);
			GUI_DispStringAt("Low pressure", 150,150);
			GUI_DispStringAt("Compressor", 150,180);
			GUI_DispStringAt("Cooling Fan", 150,210);
			GUI_DispStringAt("O2 concentration (Abnormal)", 150,240);
			GUI_DispStringAt("Cannula", 150,270);
			GUI_DispStringAt("O2 concentration (Lowering)",150,300);
			GUI_DispStringAt("O2 concentration sensor", 150,330);
			GUI_DispStringAt("Flow rate", 150,360);

			GUI_DispStringAt("Filter cleaning module", 580,90);
			GUI_DispStringAt("Speaker", 580,120);
			GUI_DispStringAt("Anypal", 580,150);
			GUI_DispStringAt("Comp outlet temp (A)", 580,180);
			GUI_DispStringAt("Comp outlet temp (R)", 580,210);
			GUI_DispStringAt("Flow controller", 580,240);
			GUI_DispStringAt("Nonvolatile memory", 580,270);
			GUI_DispStringAt("External flow path", 580,300);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("開発用設定", 400,5);

			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("アラーム機能",25,50);
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("有効",30,70);
			GUI_DispStringAt("無効",120,70);
			GUI_DispStringAt("有効",460,70);
			GUI_DispStringAt("無効",550,70);

			GUI_DispStringAt("温度異常", 150,90);
			GUI_DispStringAt("高圧異常", 150,120);
			GUI_DispStringAt("低圧異常", 150,150);
			GUI_DispStringAt("コンプレッサ異常", 150,180);
			GUI_DispStringAt("ブロワ異常", 150,210);
			GUI_DispStringAt("酸素濃度異常", 150,240);
			GUI_DispStringAt("カニューラ異常", 150,270);
			GUI_DispStringAt("酸素濃度低下",150,300);
			GUI_DispStringAt("酸素センサ異常", 150,330);
			GUI_DispStringAt("流量異常", 150,360);

			GUI_DispStringAt("掃除機構異常", 580,90);
			GUI_DispStringAt("スピーカ異常", 580,120);
			GUI_DispStringAt("エニィパル異常", 580,150);
			GUI_DispStringAt("コンプレッサ出口温度異常", 580,180);
			GUI_DispStringAt("コンプレッサ出口温度上昇", 580,210);
			GUI_DispStringAt("フローコントローラ異常", 580,240);
			GUI_DispStringAt("不揮発性メモリ異常", 580,270);
			GUI_DispStringAt("外部流路圧力異常", 580,300);
		}

		guiTask_SetFont(&GUI_FontEng3L18B2PP);
		GUI_DispStringHCenterAt("1/5", 400,440);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreen_CallbackBackBtn 								    */
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
void developmentSettingScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			developmentSettingScreen_Hide();
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
/* General 	-developmentSettingScreen_CallbackNextBtn 								    */
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
void developmentSettingScreen_CallbackNextBtn(WM_MESSAGE *pMsg)
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
			developmentSettingScreen_Hide();
			developmentSettingScreenPageTwo_Show();
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
/* General 	-developmentSettingScreen_CallbackHomeBtn 								    */
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
static void developmentSettingScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			developmentSettingScreen_Hide();
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
/* General 	-developmentSettingScreen_CallbackRadio								        */
/*                                                                                      */
/* Details  -Call back function for radio button, handle event from 			        */
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
void developmentSettingScreen_CallbackRadio(WM_MESSAGE *pMsg)
{
	switch(pMsg->MsgId)
	{
	default:
		RADIO_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreen_CallbackDisaleAllAlarmBtn 						*/
/*                                                                                      */
/* Details  -Call back function for disable all alarm button, handle event from 		*/
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
/*                     (O) s_abnormalTemperatureEnable : enable alarm temp              */
/*                     (O) s_abnormalHighPressureEnable : enable alarm high pressure	*/
/*                     (O) s_abnormalLowPressureEnable : enable alarm low pressure		*/
/*                     (O) s_abnormalCompressorEnable : enable alarm compressor         */
/*                     (O) s_abnormalCoolingFanEnable : enable alarm cooling            */
/*                     (O) s_abnormalO2ConcentrationAbnormalEnable : enable alarm O2	*/
/*                     (O) s_abnormalCannulaEnable : enable alarm cannula	          	*/
/*                     (O) s_abnormalO2ConcentrationLoweringEnable:enable alarm O2 lower*/
/*                     (O) s_abnormalO2ConcentrationSensorEnable:enable alarm O2 sensor */
/*                     (O) s_abnormalFlowRateEnable : enable alarm flow rate			*/
/*                     (O) s_abnormalFilterCleaningModuleEnable : enable alarm filter	*/
/*                     (O) s_abnormalSpeakerEnable : enable alarm speaker	         	*/
/*                     (O) s_abnormalAnypalEnable : enable alarm Anypal					*/
/*                     (O) s_abnormalCompressorOutletTemperatureAbnormalEnable : enable */
/*                                   alarm comp outlet temperature                      */
/*                     (O) s_abnormalCompressorOutletTemperatureRisingEnable : enable	*/
/*                         			 alarm comp outlet rising                           */
/*                     (O) s_abnormalFlowControllerEnable : enable alarm flow controller*/
/*                     (O) s_abnormalNonvolatileMemoryEnable : anable alarm NVRAM		*/
/*                     (O) s_abnormalExternalFlowPathPressureEnable : anable ext flow	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreen_CallbackDisaleAllAlarmBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Disable All alarms", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("全アラーム無効", Rect.x1/2, 18);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			//Set disable all alarm
			RADIO_SetValue(s_abnormalTemperatureEnable,1);
			RADIO_SetValue(s_abnormalHighPressureEnable,1);
			RADIO_SetValue(s_abnormalLowPressureEnable,1);
			RADIO_SetValue(s_abnormalCompressorEnable,1);
			RADIO_SetValue(s_abnormalCoolingFanEnable,1);
			RADIO_SetValue(s_abnormalO2ConcentrationAbnormalEnable,1);
			RADIO_SetValue(s_abnormalCannulaEnable,1);
			RADIO_SetValue(s_abnormalO2ConcentrationLoweringEnable,1);
			RADIO_SetValue(s_abnormalO2ConcentrationSensorEnable,1);
			RADIO_SetValue(s_abnormalFlowRateEnable,1);
			RADIO_SetValue(s_abnormalFilterCleaningModuleEnable,1);
			RADIO_SetValue(s_abnormalSpeakerEnable,1);
			RADIO_SetValue(s_abnormalAnypalEnable,1);
			RADIO_SetValue(s_abnormalCompressorOutletTemperatureAbnormalEnable,1);
			RADIO_SetValue(s_abnormalCompressorOutletTemperatureRisingEnable,1);
			RADIO_SetValue(s_abnormalFlowControllerEnable,1);
			RADIO_SetValue(s_abnormalNonvolatileMemoryEnable,1);
			RADIO_SetValue(s_abnormalExternalFlowPathPressureEnable,1);
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
/* General 	-developmentSettingScreen_Init								   				*/
/*                                                                                      */
/* Details  -Initialize development setting screen 			            				*/
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
/*                     (O) g_developmentSettingScreen : development setting page one 	*/
/*                     (O) s_abnormalTemperatureEnable : enable alarm temperature		*/
/*                     (O) s_abnormalTemperatureDisable : disable alarm temperature 	*/
/*                     (O) s_abnormalHighPressureEnable : enable alarm high pressure 	*/
/*                     (O) s_abnormalHighPressureDisable : disable alarm high pressure 	*/
/*                     (O) s_abnormalLowPressureEnable : enable alarm low pressure		*/
/*                     (O) s_abnormalLowPressureDisable : disable alarm low pressure 	*/
/*                     (O) s_abnormalCompressorEnable : enable alarm compressor 		*/
/*                     (O) s_abnormalCompressorDisable : disable alarm compressor 		*/
/*                     (O) s_abnormalCoolingFanEnable : enable alarm cooling fan		*/
/*                     (O) s_abnormalCoolingFanDisable : disable alarm cooling fan 		*/
/*                     (O) s_abnormalO2ConcentrationAbnormalEnable : enable alarm O2 	*/
/*                     (O) s_abnormalO2ConcentrationAbnormalDisable : disable alarm O2 	*/
/*                     (O) s_abnormalCannulaEnable : enable alarm cannula 				*/
/*                     (O) s_abnormalCannulaDisable : disable alarm cannula		 		*/
/*                     (O) s_abnormalO2ConcentrationLoweringEnable : enable alarm O2 low*/
/*                     (O) s_abnormalO2ConcentrationLoweringDisable:disable alarm O2 low*/
/*                     (O) s_abnormalO2ConcentrationSensorEnable:enable alarm O2 sensor	*/
/*                     (O) s_abnormalO2ConcentrationSensorDisable:disable alarm O2 sensor*/
/*                     (O) s_abnormalFlowRateEnable : enable alarm flow rate			*/
/*                     (O) s_abnormalFlowRateDisable : disable alarm flow rate	 		*/
/*                     (O) s_abnormalFilterCleaningModuleEnable : enable alarm filter	*/
/*                     (O) s_abnormalFilterCleaningModuleDisable : disable alarm filter	*/
/*                     (O) s_abnormalSpeakerEnable : enable alarm speaker				*/
/*                     (O) s_abnormalSpeakerDisable : disable alarm speaker 			*/
/*                     (O) s_abnormalAnypalEnable : enable alarm Anypal					*/
/*                     (O) s_abnormalAnypalDisable : disable alarm Anypal		 		*/
/*                     (O) s_abnormalCompressorOutletTemperatureAbnormalEnable : enable */
/*                                     alarm comp outlet temp                           */
/*                     (O) s_abnormalCompressorOutletTemperatureAbnormalDisable : disable*/
/*                                     alarm comp outlet temp                           */
/*                     (O) s_abnormalCompressorOutletTemperatureRisingEnable : enable  	*/
/*                                     alarm comp outlet rising                         */
/*                     (O) s_abnormalCompressorOutletTemperatureRisingDisable : disable */
/*                                     alarm comp outlet rising                         */
/*                     (O) s_abnormalFlowControllerEnable : enable alarm flow controller*/
/*                     (O) s_abnormalFlowControllerDisable : disable alarm flow controller*/
/*                     (O) s_abnormalNonvolatileMemoryEnable : enable alarm NVRAM		*/
/*                     (O) s_abnormalNonvolatileMemoryDisable : disable alarm NVRAM 	*/
/*                     (O) s_abnormalExternalFlowPathPressureEnable : enable alarm ext flow*/
/*                     (O) s_abnormalExternalFlowPathPressureDisable : disable alarm ext flow*/
/*                     (O) s_nextBtn : next button										*/
/*                     (O) s_backBtn : back button									 	*/
/*                     (O) s_homeBtn : home button									 	*/
/*                     (O) s_disableAllAlarmBtn : disable all alarm button			 	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreen_Init(void)
{
	g_developmentSettingScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,developmentSettingScreen_Callback,0);

	/*************************************************************************************************************/
	s_abnormalTemperatureEnable = RADIO_CreateEx(35,90,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eTemperatureEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalTemperatureEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalTemperatureEnable,"",0);
	RADIO_SetFocusColor(s_abnormalTemperatureEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalTemperatureDisable = RADIO_CreateEx(125,90,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eTemperatureDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalTemperatureDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalTemperatureDisable,"",0);
	RADIO_SetFocusColor(s_abnormalTemperatureDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalTemperatureEnable, 1);
	RADIO_SetGroupId(s_abnormalTemperatureDisable, 1);

	s_abnormalHighPressureEnable = RADIO_CreateEx(35,120,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eHighPressureEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalHighPressureEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalHighPressureEnable,"",0);
	RADIO_SetFocusColor(s_abnormalHighPressureEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalHighPressureDisable = RADIO_CreateEx(125,120,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eHighPressureDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalHighPressureDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalHighPressureDisable,"",0);
	RADIO_SetFocusColor(s_abnormalHighPressureDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalHighPressureEnable, 2);
	RADIO_SetGroupId(s_abnormalHighPressureDisable, 2);

	s_abnormalLowPressureEnable = RADIO_CreateEx(35,150,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eLowPressureEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalLowPressureEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalLowPressureEnable,"",0);
	RADIO_SetFocusColor(s_abnormalLowPressureEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalLowPressureDisable = RADIO_CreateEx(125,150,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eLowPressureDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalLowPressureDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalLowPressureDisable,"",0);
	RADIO_SetFocusColor(s_abnormalLowPressureDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalLowPressureEnable, 3);
	RADIO_SetGroupId(s_abnormalLowPressureDisable, 3);

	s_abnormalCompressorEnable = RADIO_CreateEx(35,180,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eCompressorEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalCompressorEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalCompressorEnable,"",0);
	RADIO_SetFocusColor(s_abnormalCompressorEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalCompressorDisable = RADIO_CreateEx(125,180,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eCompressorDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalCompressorDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalCompressorDisable,"",0);
	RADIO_SetFocusColor(s_abnormalCompressorDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalCompressorEnable, 4);
	RADIO_SetGroupId(s_abnormalCompressorDisable, 4);

	s_abnormalCoolingFanEnable = RADIO_CreateEx(35,210,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eCoolingFanEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalCoolingFanEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalCoolingFanEnable,"",0);
	RADIO_SetFocusColor(s_abnormalCoolingFanEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalCoolingFanDisable = RADIO_CreateEx(125,210,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eCoolingFanDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalCoolingFanDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalCoolingFanDisable,"",0);
	RADIO_SetFocusColor(s_abnormalCoolingFanDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalCoolingFanEnable, 5);
	RADIO_SetGroupId(s_abnormalCoolingFanDisable, 5);

	s_abnormalO2ConcentrationAbnormalEnable = RADIO_CreateEx(35,240,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eO2ConcentrationAbnormalEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalO2ConcentrationAbnormalEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalO2ConcentrationAbnormalEnable,"",0);
	RADIO_SetFocusColor(s_abnormalO2ConcentrationAbnormalEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalO2ConcentrationAbnormalDisable = RADIO_CreateEx(125,240,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eO2ConcentrationAbnormalDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalO2ConcentrationAbnormalDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalO2ConcentrationAbnormalDisable,"",0);
	RADIO_SetFocusColor(s_abnormalO2ConcentrationAbnormalDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalO2ConcentrationAbnormalEnable, 6);
	RADIO_SetGroupId(s_abnormalO2ConcentrationAbnormalDisable, 6);

	s_abnormalCannulaEnable = RADIO_CreateEx(35,270,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eCannulaEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalCannulaEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalCannulaEnable,"",0);
	RADIO_SetFocusColor(s_abnormalCannulaEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalCannulaDisable = RADIO_CreateEx(125,270,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eCannulaDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalCannulaDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalCannulaDisable,"",0);
	RADIO_SetFocusColor(s_abnormalCannulaDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalCannulaEnable, 7);
	RADIO_SetGroupId(s_abnormalCannulaDisable, 7);

	s_abnormalO2ConcentrationLoweringEnable = RADIO_CreateEx(35,300,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eO2ConcentrationLoweringEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalO2ConcentrationLoweringEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalO2ConcentrationLoweringEnable,"",0);
	RADIO_SetFocusColor(s_abnormalO2ConcentrationLoweringEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalO2ConcentrationLoweringDisable = RADIO_CreateEx(125,300,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eO2ConcentrationLoweringDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalO2ConcentrationLoweringDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalO2ConcentrationLoweringDisable,"",0);
	RADIO_SetFocusColor(s_abnormalO2ConcentrationLoweringDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalO2ConcentrationLoweringEnable, 8);
	RADIO_SetGroupId(s_abnormalO2ConcentrationLoweringDisable, 8);

	s_abnormalO2ConcentrationSensorEnable = RADIO_CreateEx(35,330,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eO2ConcentrationSensorEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalO2ConcentrationSensorEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalO2ConcentrationSensorEnable,"",0);
	RADIO_SetFocusColor(s_abnormalO2ConcentrationSensorEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalO2ConcentrationSensorDisable = RADIO_CreateEx(125,330,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eO2ConcentrationSensorDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalO2ConcentrationSensorDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalO2ConcentrationSensorDisable,"",0);
	RADIO_SetFocusColor(s_abnormalO2ConcentrationSensorDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalO2ConcentrationSensorEnable, 9);
	RADIO_SetGroupId(s_abnormalO2ConcentrationSensorDisable, 9);

	s_abnormalFlowRateEnable = RADIO_CreateEx(35,360,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eFlowRateEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalFlowRateEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalFlowRateEnable,"",0);
	RADIO_SetFocusColor(s_abnormalFlowRateEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalFlowRateDisable = RADIO_CreateEx(125,360,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eFlowRateDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalFlowRateDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalFlowRateDisable,"",0);
	RADIO_SetFocusColor(s_abnormalFlowRateDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalFlowRateEnable, 10);
	RADIO_SetGroupId(s_abnormalFlowRateDisable, 10);
	/*************************************************************************************************************/

	/*************************************************************************************************************/
	s_abnormalFilterCleaningModuleEnable = RADIO_CreateEx(465,90,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eFilterCleaningModuleEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalFilterCleaningModuleEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalFilterCleaningModuleEnable,"",0);
	RADIO_SetFocusColor(s_abnormalFilterCleaningModuleEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalFilterCleaningModuleDisable = RADIO_CreateEx(555,90,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eFilterCleaningModuleDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalFilterCleaningModuleDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalFilterCleaningModuleDisable,"",0);
	RADIO_SetFocusColor(s_abnormalFilterCleaningModuleDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalFilterCleaningModuleEnable, 11);
	RADIO_SetGroupId(s_abnormalFilterCleaningModuleDisable, 11);

	s_abnormalSpeakerEnable = RADIO_CreateEx(465,120,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eSpeakerEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalSpeakerEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalSpeakerEnable,"",0);
	RADIO_SetFocusColor(s_abnormalSpeakerEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalSpeakerDisable = RADIO_CreateEx(555,120,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eSpeakerDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalSpeakerDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalSpeakerDisable,"",0);
	RADIO_SetFocusColor(s_abnormalSpeakerDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalSpeakerEnable, 12);
	RADIO_SetGroupId(s_abnormalSpeakerDisable, 12);

	s_abnormalAnypalEnable = RADIO_CreateEx(465,150,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eAnypalEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalAnypalEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalAnypalEnable,"",0);
	RADIO_SetFocusColor(s_abnormalAnypalEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalAnypalDisable= RADIO_CreateEx(555,150,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eAnypalDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalAnypalDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalAnypalDisable,"",0);
	RADIO_SetFocusColor(s_abnormalAnypalDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalAnypalEnable, 13);
	RADIO_SetGroupId(s_abnormalAnypalDisable, 13);

	s_abnormalCompressorOutletTemperatureAbnormalEnable = RADIO_CreateEx(465,180,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eCompressorOutletTemperatureAbnormalEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalCompressorOutletTemperatureAbnormalEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalCompressorOutletTemperatureAbnormalEnable,"",0);
	RADIO_SetFocusColor(s_abnormalCompressorOutletTemperatureAbnormalEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalCompressorOutletTemperatureAbnormalDisable = RADIO_CreateEx(555,180,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eCompressorOutletTemperatureAbnormalDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalCompressorOutletTemperatureAbnormalDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalCompressorOutletTemperatureAbnormalDisable,"",0);
	RADIO_SetFocusColor(s_abnormalCompressorOutletTemperatureAbnormalDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalCompressorOutletTemperatureAbnormalEnable, 14);
	RADIO_SetGroupId(s_abnormalCompressorOutletTemperatureAbnormalDisable, 14);

	s_abnormalCompressorOutletTemperatureRisingEnable = RADIO_CreateEx(465,210,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eCompressorOutletTemperatureRisingEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalCompressorOutletTemperatureRisingEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalCompressorOutletTemperatureRisingEnable,"",0);
	RADIO_SetFocusColor(s_abnormalCompressorOutletTemperatureRisingEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalCompressorOutletTemperatureRisingDisable = RADIO_CreateEx(555,210,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eCompressorOutletTemperatureRisingDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalCompressorOutletTemperatureRisingDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalCompressorOutletTemperatureRisingDisable,"",0);
	RADIO_SetFocusColor(s_abnormalCompressorOutletTemperatureRisingDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalCompressorOutletTemperatureRisingEnable, 15);
	RADIO_SetGroupId(s_abnormalCompressorOutletTemperatureRisingDisable, 15);

	s_abnormalFlowControllerEnable = RADIO_CreateEx(465,240,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eFlowControllerEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalFlowControllerEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalFlowControllerEnable,"",0);
	RADIO_SetFocusColor(s_abnormalFlowControllerEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalFlowControllerDisable = RADIO_CreateEx(555,240,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eFlowControllerDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalFlowControllerDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalFlowControllerDisable,"",0);
	RADIO_SetFocusColor(s_abnormalFlowControllerDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalFlowControllerEnable, 16);
	RADIO_SetGroupId(s_abnormalFlowControllerDisable, 16);

	s_abnormalNonvolatileMemoryEnable = RADIO_CreateEx(465,270,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eNonvolatileMemoryEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalNonvolatileMemoryEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalNonvolatileMemoryEnable,"",0);
	RADIO_SetFocusColor(s_abnormalNonvolatileMemoryEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalNonvolatileMemoryDisable = RADIO_CreateEx(555,270,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eNonvolatileMemoryDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalNonvolatileMemoryDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalNonvolatileMemoryDisable,"",0);
	RADIO_SetFocusColor(s_abnormalNonvolatileMemoryDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalNonvolatileMemoryEnable, 18);
	RADIO_SetGroupId(s_abnormalNonvolatileMemoryDisable, 18);

	s_abnormalExternalFlowPathPressureEnable = RADIO_CreateEx(465,300,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eExternalFlowPathPressureEnable,1,35);
	guiTaskRADIO_SetFont(s_abnormalExternalFlowPathPressureEnable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalExternalFlowPathPressureEnable,"",0);
	RADIO_SetFocusColor(s_abnormalExternalFlowPathPressureEnable,SERVICE_BACKGROUND_COLOR);

	s_abnormalExternalFlowPathPressureDisable = RADIO_CreateEx(555,300,50,30,g_developmentSettingScreen,WM_CF_SHOW,0,eExternalFlowPathPressureDisable,1,35);
	guiTaskRADIO_SetFont(s_abnormalExternalFlowPathPressureDisable,&GUI_FontEng3L10B2PP);
	RADIO_SetText(s_abnormalExternalFlowPathPressureDisable,"",0);
	RADIO_SetFocusColor(s_abnormalExternalFlowPathPressureDisable,SERVICE_BACKGROUND_COLOR);

	RADIO_SetGroupId(s_abnormalExternalFlowPathPressureEnable, 19);
	RADIO_SetGroupId(s_abnormalExternalFlowPathPressureDisable, 19);

	RADIO_SetValue(s_abnormalTemperatureEnable,0);
	RADIO_SetValue(s_abnormalHighPressureEnable,0);
	RADIO_SetValue(s_abnormalLowPressureEnable,0);
	RADIO_SetValue(s_abnormalCompressorEnable,0);
	RADIO_SetValue(s_abnormalCoolingFanEnable,0);
	RADIO_SetValue(s_abnormalO2ConcentrationAbnormalEnable,0);
	RADIO_SetValue(s_abnormalCannulaEnable,0);
	RADIO_SetValue(s_abnormalO2ConcentrationLoweringEnable,0);
	RADIO_SetValue(s_abnormalO2ConcentrationSensorEnable,0);
	RADIO_SetValue(s_abnormalFlowRateEnable,0);
	RADIO_SetValue(s_abnormalFilterCleaningModuleEnable,0);
	RADIO_SetValue(s_abnormalSpeakerEnable,0);
	RADIO_SetValue(s_abnormalAnypalEnable,0);
	RADIO_SetValue(s_abnormalCompressorOutletTemperatureAbnormalEnable,0);
	RADIO_SetValue(s_abnormalCompressorOutletTemperatureRisingEnable,0);
	RADIO_SetValue(s_abnormalFlowControllerEnable,0);
	RADIO_SetValue(s_abnormalNonvolatileMemoryEnable,0);
	RADIO_SetValue(s_abnormalExternalFlowPathPressureEnable,0);

	//Next button
	s_nextBtn = BUTTON_CreateEx(700, 415, 100, 60, g_developmentSettingScreen, WM_CF_SHOW, 0, eNextBtnDevelopmentSetting);
	WM_SetCallback(s_nextBtn,developmentSettingScreen_CallbackNextBtn);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_developmentSettingScreen, WM_CF_SHOW, 0, eBackBtnDevelopmentSetting);
	WM_SetCallback(s_backBtn,developmentSettingScreen_CallbackBackBtn);

	//Home button
	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_developmentSettingScreen, WM_CF_SHOW, 0, eHomeBtnDevelopmentSetting);
	WM_SetCallback(s_homeBtn,developmentSettingScreen_CallbackHomeBtn);

	s_disableAllAlarmBtn = BUTTON_CreateEx(460, 366-20, 230, 60, g_developmentSettingScreen, WM_CF_SHOW, 0, eDisableAllAlarmSettingId);
	WM_SetCallback(s_disableAllAlarmBtn,developmentSettingScreen_CallbackDisaleAllAlarmBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreen_Show 								   				*/
/*                                                                                      */
/* Details  -Show development setting screen 			            					*/
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
/*                     (O) g_developmentSettingScreen : development page one screen     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreen_Show(void)
{
	alarmInfor_ShowAlarmInfor();
	WM_ShowWindow(g_developmentSettingScreen);
	WM_BringToTop(g_developmentSettingScreen);
	WM_SetFocus(g_developmentSettingScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreen_Hide 								   				*/
/*                                                                                      */
/* Details  -Hide development setting screen 			            					*/
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
/*                     (O) g_developmentSettingScreen : development page one screen     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreen_Hide(void)
{
	WM_HideWindow(g_developmentSettingScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreen_CheckEnable				 						*/
/*                                                                                      */
/* Details  -Check enable alarm when alarm activate 			            		    */
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_AlarmId alarmId : - alarm need check								*/
/*				   -range : 0-31														*/
/*                 -unit : NONE                                                         */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : - bool : true if alarm is enable, false if alarm is disable		    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool developmentSettingScreen_CheckEnable(E_AlarmId alarmId)
{
	switch(alarmId)
	{
	case eTemperatureId:
		if(RADIO_GetValue(s_abnormalTemperatureEnable) >= 0)
		{
			return true;
		}
		break;
	case eHighPressureId:
		if(RADIO_GetValue(s_abnormalHighPressureEnable) >= 0)
		{
			return true;
		}
		break;
	case eLowPressureId:
		if(RADIO_GetValue(s_abnormalLowPressureEnable) >= 0)
		{
			return true;
		}
		break;
	case eCompressorDCOverVoltageId:
	case eCompressorSoftwareOverCurrentId:
	case eCompressorBoardTemperatureId:
	case eCompressorRPMId:
	case eCompressorHardwareOverCurrent:
		if(RADIO_GetValue(s_abnormalCompressorEnable) >= 0)
		{
			return true;
		}
		break;
	case eCoolingFanId:
		if(RADIO_GetValue(s_abnormalCoolingFanEnable) >= 0)
		{
			return true;
		}
		break;
	case eO2ConcentrationId:
		if(RADIO_GetValue(s_abnormalO2ConcentrationAbnormalEnable) >= 0)
		{
			return true;
		}
		break;
	case eFlowRateId:
		if(RADIO_GetValue(s_abnormalFlowRateEnable) >= 0)
		{
			return true;
		}
		break;
	case eCompressorOutletTemperatureId:
		if(RADIO_GetValue(s_abnormalCompressorOutletTemperatureAbnormalEnable) >= 0)
		{
			return true;
		}
		break;
	case eFlowControllerCommunicationId:
	case eFlowControllerPressureSensorId:
	case eFlowControllerUltrasonicSensorId:
	case eFlowControllerTemperatureSensorId:
	case eFlowControllerCommandTimeOutId:
		if(RADIO_GetValue(s_abnormalFlowControllerEnable) >= 0)
		{
			return true;
		}
		break;
	case eNonvolatileMemoryId:
		if(RADIO_GetValue(s_abnormalNonvolatileMemoryEnable) >= 0)
		{
			return true;
		}
		break;
	case eCannulaUnder27sId:
	case eCannulaMore27sId:
		if(RADIO_GetValue(s_abnormalCannulaEnable) >= 0)
		{
			return true;
		}
		break;
	case eLoweringO2ConcentrationId:
		if(RADIO_GetValue(s_abnormalO2ConcentrationLoweringEnable) >= 0)
		{
			return true;
		}
		break;
	case eO2ConcentrationSensorId:
		if(RADIO_GetValue(s_abnormalO2ConcentrationSensorEnable) >= 0)
		{
			return true;
		}
		break;
	case eRisingCompressorOutletTemperatureId:
		if(RADIO_GetValue(s_abnormalCompressorOutletTemperatureRisingEnable) >= 0)
		{
			return true;
		}
		break;
	case eFilterCleaningId:
		if(RADIO_GetValue(s_abnormalFilterCleaningModuleEnable) >= 0)
		{
			return true;
		}
		break;
	case eSpeakerId:
		if(RADIO_GetValue(s_abnormalSpeakerEnable) >= 0)
		{
			return true;
		}
		break;
	case eAnypalBatterId:
	case eAnypalRTCId:
	case eAnypalCommunicationId:
	case eOverCurrentId:
		if(RADIO_GetValue(s_abnormalAnypalEnable) >= 0)
		{
			return true;
		}
		break;
	case eExternalFlowPathpressureId:
		if(RADIO_GetValue(s_abnormalExternalFlowPathPressureEnable) >= 0)
		{
			return true;
		}
		break;
	default:
		break;
	}

	return false;
}

/* end of files */

