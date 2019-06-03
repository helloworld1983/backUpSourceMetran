/********************************************************************/
/*                                                                  */
/* File Name    : MonitorGraphScreen.c                 		        */
/*                                                                  */
/* General       : Display graph monitor			 		        */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2008/08/22      Viet Le  			new file       	*/
/* #0001       2017/09/05      Viet Le				modify			*/
/*     remove #include "assert.h"                                   */
/* #0002       2017/10/27	   Viet Le				modify			*/
/*	-Edit function : EDIT_SetFont()->guiTaskEDIT_SetFont()(bug 1997)*/
/********************************************************************/

#include "string.h"
#include "stdlib.h"
#include "WM.h"
#include "GRAPH.h"
#include "EDIT.h"
#include "BUTTON.h"
#include "StringTable.h"
#include "math.h"
#include "MainScreen.h"
#include "DataDisplay.h"
#include "board.h"
#include "MonitorGraphScreen.h"
#include "setting.h"
#include "GuiTask.h"
#include "PSATableMgr.h"
#include "Font.h"
#include "MaintenanceScreen.h"
#include "OptionScreen.h"
#include "MonitorScreen.h"
#include "AlarmInfor.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

//Define item id
enum{
	ePrevBtnMonitorGraphId,
	eInforAlarmInMonitorGraphId,
	eTimerUpdateGraphId
};

WM_HWIN g_monitorGraphScreen;

static GRAPH_DATA_Handle  s_ahData[SIZE_GRAPH_BUFFER]; /* Array of handles for the GRAPH_DATA objects */
static GRAPH_SCALE_Handle s_hScaleV;   /* Handle of vertical scale */
static GRAPH_SCALE_Handle s_hScaleH;   /* Handle of horizontal scale */

static GRAPH_DATA_Handle  s_ahO2Data[SIZE_GRAPH_BUFFER]; /* Array of handles for the GRAPH_DATA objects */
static GRAPH_SCALE_Handle s_hO2ScaleV;   /* Handle of vertical scale */
static GRAPH_SCALE_Handle s_hO2ScaleH;   /* Handle of horizontal scale */

static I16 s_aValue = 0;
static int16_t s_productTankPress;
static I16 s_psaPress = 0;

static I16 s_aO2Value = 0;
static int16_t s_O2percent;

static WM_HTIMER s_timerUpdateGraph;
static WM_HWIN s_pressGraph;
static WM_HWIN s_O2PercentGraph;

static EDIT_Handle s_ProductTankGraphData;
static EDIT_Handle s_FlowRateGraphSetting;
static EDIT_Handle s_PSAParameterGraphData;
static EDIT_Handle s_oxyconcentratorGraphData;

static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static BUTTON_Handle s_prevBtn;

static const GUI_POINT s_aPointPrevArrow[] = {
		{99,10},
		{40,10},
		{40,0},
		{0,59/2},
		{40,59},
		{40,59-10},
		{99,59-10}
};

static E_O2ConcentratorType s_conditionO2 = eNormalO2Type;
static E_TemperatureType s_conditionTemp = eNormalTempType;

/****************************************************************************************/
/*                                                                                      */
/* General 	-monitorGraphScreen_CallbackPrevBtn 								        */
/*                                                                                      */
/* Details  -Call back function for previous button, handle event from 					*/
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
static void monitorGraphScreen_CallbackPrevBtn(WM_MESSAGE * pMsg)
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
			monitorGraphScreen_Hide();
			monitorScreen_Show();
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
/* General 	-monitorGraphScreen_UpdateSelectPSA						                    */
/*                                                                                      */
/* Details  -Update psa select															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) PsaSelectName psaSelectName : - psa select 							*/
/*                    - range : 0-5                                                     */
/*                    - unit : NONE                                                     */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_conditionTemp : condition temperature                      */
/*                     (O) s_conditionO2 : condition O2						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void monitorGraphScreen_UpdateSelectPSA(E_PsaSelectName psaSelectName)
{
	switch(psaSelectName)
	{
	case LOW_TEMP_NORM_02:
		s_conditionTemp = eLowTempType;
		s_conditionO2 = eNormalO2Type;
		break;
	case NORM_TEMP_NORM_02:
		s_conditionTemp = eNormalTempType;
		s_conditionO2 = eNormalO2Type;
		break;
	case HIGH_TEMP_NORM_02:
		s_conditionTemp = eHighTempType;
		s_conditionO2 = eNormalO2Type;
		break;
	case LOW_TEMP_LOW_02:
		s_conditionTemp = eLowTempType;
		s_conditionO2 = eLowO2Type;
		break;
	case NORM_TEMP_LOW_02:
		s_conditionTemp = eNormalTempType;
		s_conditionO2 = eLowO2Type;
		break;
	case HIGH_TEMP_LOW_02:
		s_conditionTemp = eHighTempType;
		s_conditionO2 = eLowO2Type;
		break;
	default:
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-monitorGraphScreen_AddValues				 								*/
/*                                                                                      */
/* Details  -update graph monitor														*/
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
/*                     (O) s_aValue : product tank pressure data    				   	*/
/*                     (O) s_psaPress : psa press setting						        */
/*                     (O) s_ahO2Data : O2 data array                                   */
/*                     (O) s_ahData : product tank pressure array			            */
/*                     (O) s_O2percent : O2 data                                        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void monitorGraphScreen_AddValues(void)
{
	static I16 defaultO2 = DEFAULT_O2;

	s_aValue = (I16)(s_productTankPress / END_Y_GRAPH) - GRAPH_SCALE;
	s_psaPress = PSATableMgr_GetPSAItem(guiInterface_GetSettingValue(eFlowRateSettingId),
			s_conditionO2,s_conditionTemp,ePSAPress);

	GRAPH_DATA_YT_AddValue(s_ahData[2], s_psaPress - GRAPH_SCALE);
	GRAPH_DATA_YT_AddValue(s_ahData[3], s_psaPress - GRAPH_SCALE + 1);

	if(guiTask_GetSelectPort() == EEPROM_SELECT)
	{
		uint8_t o2LimitSetting = guiInterface_GetSettingValue(eOxyLimitSettingId);
		uint16_t o2Limit = 0;
		if(o2LimitSetting >= MIN_OXY_LIMIT_SETTING && o2LimitSetting <= MAX_OXY_LIMIT_SETTING)
		{
			o2Limit = Y_O2_GRAPH(o2LimitSetting);
		}
		GRAPH_DATA_YT_AddValue(s_ahO2Data[2], o2Limit);
		GRAPH_DATA_YT_AddValue(s_ahO2Data[3], o2Limit + 1);
	}
	else
	{
		uint16_t o2Limit = Y_O2_GRAPH(defaultO2);
		GRAPH_DATA_YT_AddValue(s_ahO2Data[2], o2Limit);
		GRAPH_DATA_YT_AddValue(s_ahO2Data[3], o2Limit + 1);
	}

	//Pressure
	GRAPH_DATA_YT_AddValue(s_ahData[0], s_aValue);
	GRAPH_DATA_YT_AddValue(s_ahData[1], s_aValue + 1);

	//O2
	s_aO2Value = (I16)(s_O2percent / END_Y_GRAPH);
	GRAPH_DATA_YT_AddValue(s_ahO2Data[0], Y_O2_GRAPH(s_aO2Value));
	GRAPH_DATA_YT_AddValue(s_ahO2Data[1], Y_O2_GRAPH(s_aO2Value) + 1);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-monitorGraphScreen_Callback				 								*/
/*                                                                                      */
/* Details  -Call back function for graph monitor screen, handle event from the window	*/
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
/*                     (O) s_timerUpdateGraph : timer upgrade graph               		*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void monitorGraphScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(GUI_BLACK);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Monitor", 400,5);

			guiTask_SetFont(&GUI_FontEng3L10B2PP);

			//Name flow rate setting
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
			GUI_SetTextAlign(GUI_TA_RIGHT);
			GUI_DispStringAt("Setting flow rate:",595, 17);

			//Unit flow rate setting
			GUI_SetColor(GUI_BLACK);
			GUI_DispStringAt("L/min",720,17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("モニタ", 400,5);

			guiTask_SetFont(&GUI_FontJapan3L12B2PP);

			//Name flow rate setting
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
			GUI_SetTextAlign(GUI_TA_RIGHT);
			GUI_DispStringAt("設定流量:",595, 17);

			//Unit flow rate setting
			GUI_SetColor(GUI_BLACK);
			GUI_DispStringAt("L/分",720,17);
		}

		guiTask_SetFont(&GUI_FontEng3L8B2PP);

		GUI_DispStringAt("120",10, 85);
		GUI_DispStringAt("100",10, 105);
		GUI_DispStringAt("80",18, 125);
		GUI_DispStringAt("60",18, 145);
		GUI_DispStringAt("40",18, 165);
		GUI_DispStringAt("20",18, 185);

		GUI_DispStringAt("-3'00",40, 200);
		GUI_DispStringAt("-2'50",80, 200);
		GUI_DispStringAt("-2'40",120, 200);
		GUI_DispStringAt("-2'30",160, 200);
		GUI_DispStringAt("-2'20",200, 200);
		GUI_DispStringAt("-2'10",240, 200);
		GUI_DispStringAt("-2'00",280, 200);
		GUI_DispStringAt("-1'50",320, 200);
		GUI_DispStringAt("-1'40",360, 200);
		GUI_DispStringAt("-1'30",400, 200);
		GUI_DispStringAt("-1'20",440, 200);
		GUI_DispStringAt("-1'10",480, 200);
		GUI_DispStringAt("-1'00",520, 200);
		GUI_DispStringAt("-0'50",560, 200);
		GUI_DispStringAt("-0'40",600, 200);
		GUI_DispStringAt("-0'30",640, 200);
		GUI_DispStringAt("-0'20",680, 200);
		GUI_DispStringAt("-0'10",720, 200);

		GUI_DispStringAt("95",18, 270);
		GUI_DispStringAt("90",18, 290);
		GUI_DispStringAt("85",18, 310);
		GUI_DispStringAt("80",18, 330);
		GUI_DispStringAt("75",18, 350);
		GUI_DispStringAt("70",18, 370);

		GUI_DispStringAt("-3'00",40, 385);
		GUI_DispStringAt("-2'50",80, 385);
		GUI_DispStringAt("-2'40",120, 385);
		GUI_DispStringAt("-2'30",160, 385);
		GUI_DispStringAt("-2'20",200, 385);
		GUI_DispStringAt("-2'10",240, 385);
		GUI_DispStringAt("-2'00",280, 385);
		GUI_DispStringAt("-1'50",320, 385);
		GUI_DispStringAt("-1'40",360, 385);
		GUI_DispStringAt("-1'30",400, 385);
		GUI_DispStringAt("-1'20",440, 385);
		GUI_DispStringAt("-1'10",480, 385);
		GUI_DispStringAt("-1'00",520, 385);
		GUI_DispStringAt("-0'50",560, 385);
		GUI_DispStringAt("-0'40",600, 385);
		GUI_DispStringAt("-0'30",640, 385);
		GUI_DispStringAt("-0'20",680, 385);
		GUI_DispStringAt("-0'10",720, 385);

		guiTask_SetFont(&GUI_FontEng3L18B2PP);
		GUI_DispStringHCenterAt("2/2", 400,440);
		break;
	case WM_TIMER:
		if(WM_GetTimerId(pMsg->Data.v) == eTimerUpdateGraphId)
		{
			WM_RestartTimer(s_timerUpdateGraph,TIMER_UPDATE_GRAPH);
			monitorGraphScreen_AddValues();
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
/* General 	-monitorGraphScreen_CallbackBackBtn				 							*/
/*                                                                                      */
/* Details  -Call back function for back button, handle event from the window			*/
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
void monitorGraphScreen_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			monitorGraphScreen_Hide();
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
/* General 	-monitorGraphScreen_CallbackHomeBtn				 							*/
/*                                                                                      */
/* Details  -Call back function for home button, handle event from the window			*/
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
void monitorGraphScreen_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			monitorGraphScreen_Hide();
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
/* General 	-monitorGraphScreen_Init 								       				*/
/*                                                                                      */
/* Details  -Initialize graph monitor screen											*/
/*																						*/
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
/*                     (O) g_monitorGraphScreen : monitor graph screen	 			   	*/
/*                     (O) s_ProductTankGraphData : display product tank pressure       */
/*                     (O) s_FlowRateGraphSetting : display flow rate setting           */
/*                     (O) s_PSAParameterGraphData : display psa parameter              */
/*                     (O) s_pressGraph : product tank press graph                      */
/*                     (O) s_ahData : Array of handles for the pressure GRAPH_DATA objects*/
/*                     (O) s_hScaleV : Create and add vertical scale of pressure graph  */
/*                     (O) s_hScaleH : Create and add horizontal scale of pressure graph*/
/*                     (O) s_oxyconcentratorGraphData : display O2 data				   	*/
/*                     (O) s_O2PercentGraph : O2 graph						            */
/*                     (O) s_ahO2Data : Array of handles for the O2 GRAPH_DATA objects  */
/*                     (O) s_hO2ScaleV : Create and add vertical scale of O2 graph	    */
/*                     (O) s_hO2ScaleH : Create and add horizontal scale of O2 graph    */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button                                      */
/*                     (O) s_prevBtn : previous button						            */
/*                     (O) s_timerUpdateGraph : timer update graph    	 			   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void monitorGraphScreen_Init(void)
{
	g_monitorGraphScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,monitorGraphScreen_Callback,0);

	s_ProductTankGraphData = EDIT_CreateEx(12, 45, 370, 40,g_monitorGraphScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eProductTankMonitorGraphStringId,50);
	WM_SetCallback(s_ProductTankGraphData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_ProductTankGraphData,eValueOfProductTankPressMonitorGraphDataId);
	EDIT_Invalidate(s_ProductTankGraphData);

	s_FlowRateGraphSetting = EDIT_CreateEx(595,10,125,30,g_monitorGraphScreen,WM_CF_SHOW,0,eFlowRateSettingStringID,5);
	EDIT_SetFocussable(s_FlowRateGraphSetting,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_FlowRateGraphSetting ,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_FlowRateGraphSetting ,GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_FlowRateGraphSetting ,"0");

	s_PSAParameterGraphData = EDIT_CreateEx(395, 45, 370, 40,g_monitorGraphScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,ePSAParameterDataGraphStringId,50);
	WM_SetCallback(s_PSAParameterGraphData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_PSAParameterGraphData,eValueOfPSAParameterGraphDataId);
	EDIT_Invalidate(s_PSAParameterGraphData);

	s_pressGraph = GRAPH_CreateEx(40,78,720,120,g_monitorGraphScreen,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP, 0, GUI_ID_GRAPH0);
	GRAPH_SetColor(s_pressGraph,GUI_WHITE,0);
	GRAPH_SetGridVis(s_pressGraph, 1);
	GRAPH_SetLineStyle(s_pressGraph,GUI_LS_DOT);

	s_ahData[2] = GRAPH_DATA_YT_Create(0x0078FF, 720, 0, 0);
	GRAPH_AttachData(s_pressGraph, s_ahData[2]);

	s_ahData[3] = GRAPH_DATA_YT_Create(0x0078FF, 720, 0, 0);
	GRAPH_AttachData(s_pressGraph, s_ahData[3]);

	s_ahData[0] = GRAPH_DATA_YT_Create(0xFF0000, 720, 0, 0);//Blue
	GRAPH_AttachData(s_pressGraph, s_ahData[0]);

	s_ahData[1] = GRAPH_DATA_YT_Create(0xFF0000, 720, 0, 0);//Blue
	GRAPH_AttachData(s_pressGraph, s_ahData[1]);

	/* Set graph attributes */
	GRAPH_SetBorder(s_pressGraph, 0, 0, 0, 0);
	GRAPH_SetGridDistX(s_pressGraph, 40);
	GRAPH_SetGridDistY(s_pressGraph, 20);
	GRAPH_SetGridVis(s_pressGraph, 1);
	GRAPH_SetGridFixedX(s_pressGraph, 1);

	/* Create and add vertical scale */
	s_hScaleV = GRAPH_SCALE_Create(0, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 20);
	GRAPH_SCALE_SetTextColor(s_hScaleV, GUI_TRANSPARENT);
	GRAPH_AttachScale(s_pressGraph, s_hScaleV);

	/* Create and add horizontal scale */
	s_hScaleH = GRAPH_SCALE_Create(155, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 50);
	GRAPH_SCALE_SetTextColor(s_hScaleH, GUI_DARKGREEN);
	GRAPH_AttachScale(s_pressGraph, s_hScaleH);

	s_oxyconcentratorGraphData = EDIT_CreateEx(12, 230, 370, 40,g_monitorGraphScreen,WM_CF_SHOW | WM_CF_MEMDEV,0,eO2PercentGraphDataStringID,50);
	WM_SetCallback(s_oxyconcentratorGraphData,dataDisplay_Callback);
	dataDisplay_InitEditValue(s_oxyconcentratorGraphData,eValueOfOxgGraphDataId);
	EDIT_Invalidate(s_oxyconcentratorGraphData);

	s_O2PercentGraph = GRAPH_CreateEx(40,263,720,120,g_monitorGraphScreen,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP, 0, GUI_ID_GRAPH1);
	GRAPH_SetColor(s_O2PercentGraph,GUI_WHITE,0);
	GRAPH_SetGridVis(s_O2PercentGraph, 1);
	GRAPH_SetLineStyle(s_O2PercentGraph,GUI_LS_DOT);

	s_ahO2Data[2] = GRAPH_DATA_YT_Create(0x0078FF, 720, 0, 0);
	GRAPH_AttachData(s_O2PercentGraph, s_ahO2Data[2]);

	s_ahO2Data[3] = GRAPH_DATA_YT_Create(0x0078FF, 720, 0, 0);
	GRAPH_AttachData(s_O2PercentGraph, s_ahO2Data[3]);

	s_ahO2Data[0] = GRAPH_DATA_YT_Create(0xFF0000, 720, 0, 0);//Blue
	GRAPH_AttachData(s_O2PercentGraph, s_ahO2Data[0]);

	s_ahO2Data[1] = GRAPH_DATA_YT_Create(0xFF0000, 720, 0, 0);
	GRAPH_AttachData(s_O2PercentGraph, s_ahO2Data[1]);

	/* Set graph attributes */
	GRAPH_SetBorder(s_O2PercentGraph, 0, 0, 0, 0);
	GRAPH_SetGridDistX(s_O2PercentGraph, 40);
	GRAPH_SetGridDistY(s_O2PercentGraph, 20);
	GRAPH_SetGridVis(s_O2PercentGraph, 1);
	GRAPH_SetGridFixedX(s_O2PercentGraph, 1);

	/* Create and add vertical scale */
	s_hO2ScaleV = GRAPH_SCALE_Create(0, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 20);
	GRAPH_SCALE_SetTextColor(s_hO2ScaleV, GUI_TRANSPARENT);
	GRAPH_AttachScale(s_O2PercentGraph, s_hO2ScaleV);

	/* Create and add horizontal scale */
	s_hO2ScaleH = GRAPH_SCALE_Create(155, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 50);
	GRAPH_SCALE_SetTextColor(s_hO2ScaleH, GUI_DARKGREEN);
	GRAPH_AttachScale(s_O2PercentGraph, s_hO2ScaleH);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_monitorGraphScreen, WM_CF_SHOW, 0, eBackBtnMonitorGraphStringId);
	WM_SetCallback(s_backBtn,monitorGraphScreen_CallbackBackBtn);

	s_homeBtn= BUTTON_CreateEx(445, 415, 100, 60, g_monitorGraphScreen, WM_CF_SHOW, 0, eHomeBtnMonitorGraphStringId);
	WM_SetCallback(s_homeBtn,monitorGraphScreen_CallbackHomeBtn);

	//Previous button
	s_prevBtn = BUTTON_CreateEx(0, 415, 102, 60, g_monitorGraphScreen, WM_CF_SHOW, 0, ePrevBtnMonitorGraphId);
	WM_SetCallback(s_prevBtn,monitorGraphScreen_CallbackPrevBtn);

	s_timerUpdateGraph = WM_CreateTimer(g_monitorGraphScreen, eTimerUpdateGraphId, TIMER_UPDATE_GRAPH, 0);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-monitorGraphScreen_Show 								       				*/
/*                                                                                      */
/* Details  -Show graph monitor screen													*/
/*																						*/
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
/*                     (O) s_FlowRateGraphSetting : display flow rate setting      		*/
/*                     (O) g_monitorGraphScreen : monitor graph screen	             	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void monitorGraphScreen_Show(void)
{
	alarmInfor_ShowAlarmInfor();

	int8_t buffer[SIZE_BUFFER_FLOW_RATE] = {'\0'};
	double floatValue = (double)guiInterface_GetSettingValue(eFlowRateSettingId)/FLOWRATE_SCALE;
	sprintf((char*)buffer,"%0.2f",floatValue);
	EDIT_SetText(s_FlowRateGraphSetting ,(char*)buffer);

	WM_ShowWindow(g_monitorGraphScreen);
	WM_BringToTop(g_monitorGraphScreen);
	WM_SetFocus(g_monitorGraphScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-monitorGraphScreen_Hide 								       				*/
/*                                                                                      */
/* Details  -Hide graph monitor screen													*/
/*																						*/
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
/*                     (O) g_monitorGraphScreen : monitor graph screen              	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void monitorGraphScreen_Hide(void)
{
	WM_HideWindow(g_monitorGraphScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-monitorGraphScreen_UpdatePSA				 								*/
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
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_productTankPress : product tank pressure				   	*/
/*                     (O) s_O2percent : O2 percent						                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void monitorGraphScreen_UpdatePSA(IPC_MSG_DATA_t data)
{
	s_productTankPress = data.stPsaMonitorPacket.productankPress;
	s_O2percent = data.stPsaMonitorPacket.o2Concentrator;

	if(WM_IsVisible(g_monitorGraphScreen))
	{
		monitorGraphScreen_UpdateSelectPSA(data.stPsaMonitorPacket.psaParamSelect);
		dataDisplay_SetValueData(eValueOfProductTankPressMonitorGraphDataId, data.stPsaMonitorPacket.productankPress);
		dataDisplay_SetValueData(eValueOfOxgGraphDataId, data.stPsaMonitorPacket.o2Concentrator);
		dataDisplay_SetStringPsaSelectData(eValueOfPSAParameterGraphDataId, data.stPsaMonitorPacket.psaParamSelect);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-monitorGraphScreen_ChangeFlowrate 								       		*/
/*                                                                                      */
/* Details  -Change flow rate by encoder							   					*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) uint8_t FRPSAValue :  - flow rate change					 			*/
/*						- range : 5-60													*/
/*                      - unit : LPM                                                    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_monitorGraphScreen : monitor graph screen		           	*/
/*                     (O) s_FlowRateGraphSetting : display flow rate setting         	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void monitorGraphScreen_ChangeFlowrate(uint8_t FRPSAValue)
{
	if(WM_IsVisible(g_monitorGraphScreen))
	{
		int8_t buffer[SIZE_BUFFER_FLOW_RATE] = {'\0'};
		double floatValue = (double)FRPSAValue/FLOWRATE_SCALE;

		sprintf((char*)buffer,"%0.2f",floatValue);
		EDIT_SetText(s_FlowRateGraphSetting ,(char*)buffer);
	}

	return;
}

/* end of files */
