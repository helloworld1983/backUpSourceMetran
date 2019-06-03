/********************************************************************/
/*                                                                  */
/* File Name    : DataDisplay.c                                    	*/
/*                                                                  */
/* General      : The DataDisplay is used to display         		*/
/*                the monitored machine data						*/
/* 								                                    */
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
/* #0001       2017/09/05		Viet Le				modify			*/
/*    remove #include "assert.h"                                    */
/* #0002	   2017/10/27      Viet Le 	    	        modify	    */
/*	  Edit : EDIT_SetFont()->guiTaskEDIT_SetFont(bug 1997)			*/
/********************************************************************/

#include "stdlib.h"
#include "WM.h"
#include "board.h"
#include "TEXT.h"
#include "GUI.h"
#include "EDIT.h"
#include "BUTTON.h"
#include "DataDisplay.h"
#include "TextLibrary.h"
#include "setting.h"
#include "GuiTask.h"
#include "Font.h"
#include "MainScreen.h"
#include "DualCoreCommon.h"
#include "GuiInterface.h"

static EDIT_Handle s_editValueData[eMaxValueDataId];

/****************************************************************************************/
/*                                                                                      */
/* General 	-dataDisplay_SetNameData		                                            */
/*                                                                                      */
/* Details  -Set name by assigned id          											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object for data					 			    */
/*				   GUI_RECT Rect   : Rect for data										*/
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
static void dataDisplay_SetNameData(EDIT_Handle obj, GUI_RECT Rect)
{
	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		//get name
		const int8_t* nameStr = (int8_t*)textLibrary_GetString((E_StringID)WM_GetId(obj));
		guiTask_SetFont(&GUI_FontEng3L10B2PP);

		GUI_SetColor(GUI_BLACK);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_SetTextAlign(GUI_TA_RIGHT);
		GUI_DispStringAt((char*)nameStr,Rect.x1 - 170, Rect.y0 + 7);
	}
	else
	{
		guiTask_SetFont(&GUI_FontJapan3L12B2PP);
		GUI_SetColor(GUI_BLACK);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_SetTextAlign(GUI_TA_RIGHT);
		switch(WM_GetId(obj))
		{
		case eMeasurementFRDataStringID:
			GUI_DispStringAt("測定流量:",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case eO2PercentDataStringID:
		case eO2PercentGraphDataStringID:
			GUI_DispStringAt("酸素濃度:",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case eTemperatureUnitDataStringID:
			GUI_DispStringAt("温度(機器内部):",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case eTemperatureCompOrOutletDataStringID:
			GUI_DispStringAt("温度(コンプレッサ出口):",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case ePressure1DataStringID:
		case eProductTankMonitorPSAStringId:
		case eProductTankMonitorGraphStringId:
			GUI_DispStringAt("製品タンク圧力:",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case ePressure2DataStringID:
			GUI_DispStringAt("カニューラ圧力:",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case eCompPositivePressForceStringID:
			GUI_DispStringAt("圧力センサ1:",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case eCompNegativePressForceStringID:
			GUI_DispStringAt("圧力センサ2:",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case eChanelSwitchingPressStringID:
			GUI_DispStringAt("外部流路圧力:",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case eValve1DataStringId:
			GUI_DispStringAt("バルブ1動作:",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case eValve2DataStringId:
			GUI_DispStringAt("バルブ2動作:",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case eValve3DataStringId:
			GUI_DispStringAt("バルブ3動作:",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case eValve4DataStringId:
			GUI_DispStringAt("バルブ4動作:",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case eValve5DataStringId:
			GUI_DispStringAt("バルブ5動作:",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case eCompressorRPMDataStringId:
			GUI_DispStringAt("コンプレッサ回転数:",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case eCoolingFANDataStringId:
			GUI_DispStringAt("冷却ファンデューティ:",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case ePSACycleTimeStringId:
			GUI_DispStringAt("PSA サイクルタイム:",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case ePSAParameterDataStringId:
		case ePSAParameterDataGraphStringId:
			GUI_DispStringAt("PSA パラメータ:",Rect.x1 - 170, Rect.y0 + 7);
			break;
		case eIlluminanceSensorVoltageStringID:
			GUI_DispStringAt("照度センサ電圧:",Rect.x1 - 170, Rect.y0 + 7);
			break;
		default:
			break;
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-dataDisplay_SetUnitData		                                            */
/*                                                                                      */
/* Details  -Set uint by assigned id          											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : obj for data				 					  	*/
/*				   GUI_RECT Rect   : Rect for data										*/
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
void dataDisplay_SetUnitData(EDIT_Handle obj, GUI_RECT Rect)
{
	//get Unit
	const int8_t* unitStr = (int8_t*)textLibrary_GetString(eNoStringID);

	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		guiTask_SetFont(&GUI_FontEng3L10B2PP);
	}
	else
	{
		guiTask_SetFont(&GUI_FontJapan3L12B2PP);
	}
	switch(WM_GetId(obj))
	{
	case eTemperatureUnitDataStringID:
		unitStr = (int8_t*)"℃";
		break;
	case eMeasurementFRDataStringID:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			unitStr = (int8_t*)textLibrary_GetString(eUnitLPMinStringId);
		}
		else
		{
			unitStr = (int8_t*)"L/分";
		}
		break;
	case eO2PercentDataStringID:
	case eCoolingFANDataStringId:
	case eO2PercentGraphDataStringID:
	case eCompressorRPMDataStringId:
		unitStr = (int8_t*)textLibrary_GetString(eUnitPercentStringId);
		break;
	case eTemperatureCompOrOutletDataStringID:
		unitStr = (int8_t*)"℃";
		break;
	case ePressure1DataStringID:
	case ePressure2DataStringID:
	case eCompPositivePressForceStringID:
	case eCompNegativePressForceStringID:
	case eChanelSwitchingPressStringID:
	case eProductTankMonitorPSAStringId:
	case eProductTankMonitorGraphStringId:
		unitStr = (int8_t*)textLibrary_GetString(eUnitkPaStringId);
		break;
	case eIlluminanceSensorVoltageStringID:
		unitStr = (int8_t*)textLibrary_GetString(eUnitVoltageStringId);
		break;
	case ePSACycleTimeStringId:
		unitStr = (int8_t*)textLibrary_GetString(eUnitSecStringId);
		break;
	default:
		break;
	}

	GUI_SetColor(GUI_BLACK);
	GUI_DispStringAt((char*)unitStr,Rect.x1 - 43,Rect.y1 - 30);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-dataDisplay_SetValueData		                                            */
/*                                                                                      */
/* Details  -Set value by assigned id          											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int valueId : id for data			 					  			*/
/*				   int16_t value : value for data										*/
/*				   																		*/
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
void dataDisplay_SetValueData(int valueId, int16_t value)
{
	double floatValue = 0.0;
	int8_t buffer[SIZE_BUFFER_DATA_VALUE] = {'\0'};

	floatValue = (double)value/100.0;

	switch(valueId)
	{
	case eValueOfMeasurementFRDataId:
	case eValueOfMeasurementFREquipmentInforDataId:
	case eValueOfProductTankPressMonitorPSADataId:
	case eValueOfProductTankPressMonitorGraphDataId:
	case eValueOfCompPositivePressDataId:
	case eValueOfChanelSwitchPressDataId:
	case eValueOfCannulaPressDataId:
	case eValueOfMeasurementFRFunctionTestDataId:
	case eValueOfProductTankPressFunctionTestDataId:
	case eValueOfCannulaPressFunctionTestDataId:
	case eValueOfCompPositivePressFunctionTestDataId:
	case eValueOfChanelSwitchPressFunctionTestDataId:
		sprintf((char*)buffer,"%0.2f",floatValue);
		break;
	case eValueOfCompNegativePressFunctionTestDataId:
	case eValueOfCompNegativePressDataId:
	case eValueOfIlluminanceSensorVoltageDataId:
		sprintf((char*)buffer,"%0.2f",floatValue);
		break;
	case eValueOfOxgDataId:
	case eValueOfOxgGraphDataId:
	case eValueOfTempUnitMonitorPSADatatId:
	case eValueOfTempUniFunctionTestDatatId:
	case eValueOfTempCompMonitorPSADataId:
	case eValueOfTempCompFunctionTestDataId:
	case eValueOfPSACycleTimeDataId:
	case eValueOfOxgEquipmentInforDataId:
	case eValueOfOxgFunctionTestDataId:
		sprintf((char*)buffer,"%0.1f",floatValue);
		break;
	case eValueOfCoolingFANDataId:
	case eValueOfCompressorRPMDataId:
		floatValue = (double)value;
		sprintf((char*)buffer,"%0.0f",floatValue);
		break;
	default:
		break;
	}

	EDIT_SetText(s_editValueData[valueId] ,(char*)buffer);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-dataDisplay_SetStringValveData		                                        */
/*                                                                                      */
/* Details  -Set string value by assigned id          									*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int valueId  : - id of data											*/
/*				   -range : 0-37														*/
/*                 -unit : NONE                                                         */
/*                                                                                      */
/*                 ValStatus status : - status valve      								*/
/*				   -range : 0-2															*/
/*                 -unit : NONE                                                         */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_editValueData : array contain all value data            	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void dataDisplay_SetStringValveData(int valueId, E_ValStatus status)
{
	if(status == VAL_ON)
	{
		EDIT_SetText(s_editValueData[valueId] ,"ON");
	}
	else
	{
		EDIT_SetText(s_editValueData[valueId] ,"OFF");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-dataDisplay_SetStringPsaSelectData		                                    */
/*                                                                                      */
/* Details  -Set string value by assigned id          									*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int valueId  : - id of data				 					  		*/
/*				   -range : 0-37														*/
/*                 -unit : NONE                                                         */
/*																						*/
/*                 PsaSelectName psaSelect : - psa select valve      					*/
/*				   -range : 0-5															*/
/*                 -unit : NONE                                                         */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_editValueData : array contain all value data               */
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void dataDisplay_SetStringPsaSelectData(int valueId, E_PsaSelectName psaSelect)
{
	switch(psaSelect)
	{
	case LOW_TEMP_NORM_02:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTaskEDIT_SetFont(s_editValueData[valueId] ,&GUI_FontEng3L5B2PP);
			EDIT_SetText(s_editValueData[valueId] ,"Low temp/Normal O2");
		}
		else
		{
			guiTaskEDIT_SetFont(s_editValueData[valueId] ,&GUI_FontJapan3L12B2PP);
			EDIT_SetText(s_editValueData[valueId] ,"低温/濃度正常");
		}
		break;
	case NORM_TEMP_NORM_02:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTaskEDIT_SetFont(s_editValueData[valueId] ,&GUI_FontEng3L5B2PP);
			EDIT_SetText(s_editValueData[valueId] ,"Normal temp/Normal O2");
		}
		else
		{
			guiTaskEDIT_SetFont(s_editValueData[valueId] ,&GUI_FontJapan3L12B2PP);
			EDIT_SetText(s_editValueData[valueId] ,"常温/濃度正常");
		}
		break;
	case HIGH_TEMP_NORM_02:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTaskEDIT_SetFont(s_editValueData[valueId] ,&GUI_FontEng3L5B2PP);
			EDIT_SetText(s_editValueData[valueId] ,"High temp/Normal O2");
		}
		else
		{
			guiTaskEDIT_SetFont(s_editValueData[valueId] ,&GUI_FontJapan3L12B2PP);
			EDIT_SetText(s_editValueData[valueId] ,"高温/濃度正常");
		}
		break;
	case LOW_TEMP_LOW_02:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTaskEDIT_SetFont(s_editValueData[valueId] ,&GUI_FontEng3L5B2PP);
			EDIT_SetText(s_editValueData[valueId] ,"Low temp/Low O2");
		}
		else
		{
			guiTaskEDIT_SetFont(s_editValueData[valueId] ,&GUI_FontJapan3L12B2PP);
			EDIT_SetText(s_editValueData[valueId] ,"低温/濃度低下");
		}
		break;
	case NORM_TEMP_LOW_02:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTaskEDIT_SetFont(s_editValueData[valueId] ,&GUI_FontEng3L5B2PP);
			EDIT_SetText(s_editValueData[valueId] ,"Normal temp/Low O2");
		}
		else
		{
			guiTaskEDIT_SetFont(s_editValueData[valueId] ,&GUI_FontJapan3L12B2PP);
			EDIT_SetText(s_editValueData[valueId] ,"常温/濃度低下");
		}
		break;
	case HIGH_TEMP_LOW_02:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTaskEDIT_SetFont(s_editValueData[valueId] ,&GUI_FontEng3L5B2PP);
			EDIT_SetText(s_editValueData[valueId] ,"High temp/Low O2");
		}
		else
		{
			guiTaskEDIT_SetFont(s_editValueData[valueId] ,&GUI_FontJapan3L12B2PP);
			EDIT_SetText(s_editValueData[valueId] ,"高温/濃度低下");
		}
		break;
	default:
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-dataDisplay_InitEditValue	                                                */
/*                                                                                      */
/* Details  -Initialize value fields													*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : - object for data				  					*/
/*                                                                                      */
/*                 int valueId  : - id for data                                         */
/*				   -range : 0-37														*/
/*                 -unit : NONE                                                         */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_editValueData : array contain all value data               */
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void dataDisplay_InitEditValue(EDIT_Handle obj, int valueId)
{
	s_editValueData[valueId] = EDIT_CreateEx(200,0,125,30,obj,WM_CF_SHOW,0,valueId,50);
	EDIT_SetFocussable(s_editValueData[valueId],EDIT_CI_DISABLED);
	if(valueId != eValueOfPSAParameterDataId && valueId != eValueOfPSAParameterGraphDataId)
	{
		guiTaskEDIT_SetFont(s_editValueData[valueId] ,&GUI_FontEng3L14B2PP);
	}
	else
	{
		guiTaskEDIT_SetFont(s_editValueData[valueId] ,&GUI_FontEng3L5B2PP);
	}
	EDIT_SetTextAlign(s_editValueData[valueId] ,GUI_TA_VCENTER | GUI_TA_HCENTER);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-dataDisplay_Callback	                                            		*/
/*                                                                                      */
/* Details  -Handle event from window													*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE * pMsg : messenger form window			  				*/
/*                                                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void dataDisplay_Callback(WM_MESSAGE * pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		dataDisplay_SetNameData(pMsg->hWin,Rect);
		dataDisplay_SetUnitData(pMsg->hWin,Rect);
		break;
	case WM_TOUCH:
		break;
	case WM_TOUCH_CHILD:
		break;
	default:
		EDIT_Callback(pMsg);
		break;
	}

	return;
}

/* end of files */

