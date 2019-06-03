/********************************************************************/
/*                                                                  */
/* File Name    : DataDisplay.h                                    	*/
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
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#ifndef DATADISPLAY_H_
#define DATADISPLAY_H_

#include "WM.h"
#include "TEXT.h"
#include "stdint.h"
#include "DualCoreCommon.h"

#define SIZE_BUFFER_DATA_VALUE				10

//Define value id for data display
typedef enum
{
	eNoValueDataId = 0,
	eValueOfTempUniDatatId,
	eValueOfMeasurementFRDataId,
	eValueOfOxgDataId,
	eValueOfOxgGraphDataId,
	eValueOfTempCompDataId,
	eValueOfProductTankPressDataId,
	eValueOfCannulaPressDataId,
	eValueOfIlluminanceSensorVoltageDataId,
	eValueOfCompPositivePressDataId,
	eValueOfCompNegativePressDataId,
	eValueOfChanelSwitchPressDataId,
	eValueOfProductTankPressMonitorPSADataId,
	eValueOfProductTankPressMonitorGraphDataId,
	eValueOfValve1DataId,
	eValueOfValve2DataId,
	eValueOfValve3DataId,
	eValueOfValve4DataId,
	eValueOfValve5DataId,
	eValueOfCompressorRPMDataId,
	eValueOfCoolingFANDataId,
	eValueOfPSACycleTimeDataId,
	eValueOfPSAParameterDataId,
	eValueOfPSAParameterGraphDataId,
	eValueOfTempCompMonitorPSADataId,
	eValueOfTempUnitMonitorPSADatatId,
	eValueOfMeasurementFREquipmentInforDataId,
	eValueOfOxgEquipmentInforDataId,
	eValueOfTempUniFunctionTestDatatId,
	eValueOfTempCompFunctionTestDataId,
	eValueOfOxgFunctionTestDataId,
	eValueOfMeasurementFRFunctionTestDataId,
	eValueOfProductTankPressFunctionTestDataId,
	eValueOfCannulaPressFunctionTestDataId,
	eValueOfCompPositivePressFunctionTestDataId,
	eValueOfCompNegativePressFunctionTestDataId,
	eValueOfChanelSwitchPressFunctionTestDataId,
	eMaxValueDataId
}E_ValueDataId;

//Init value fields
void dataDisplay_InitEditValue(TEXT_Handle obj, int valueId);
//Custom DataDisplay
void dataDisplay_Callback(WM_MESSAGE * pMsg);
//Set data display
void dataDisplay_SetValueData(int valueId, int16_t value);
//Set string data display
void dataDisplay_SetStringValveData(int valueId, E_ValStatus status);
//Set string data display
void dataDisplay_SetStringPsaSelectData(int valueId, E_PsaSelectName psaSelect);

#endif /* DATADISPLAY_H_ */
/* end of file */
