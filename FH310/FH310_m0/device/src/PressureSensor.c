/********************************************************************/
/*                                                                  */
/* File Name    : PressureSensor.c                                  */
/*                                                                  */
/* General       : This module contain function for read pressure
 * 				   sensor											*/
/*                                                                  */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             Jul 31, 2016     Quyen Ngo(MV)  		new file        */
/*                                                                  */
/*	#0001	   2017/11/02		Quyen Ngo(MV)		modify			*/
/*		-Change return of Productank, Canula, Temperate function to */
/*		ADC_SENSOR_t(Bug 2012)										*/
/********************************************************************/
#include "PressureSensor.h"

#ifdef DEBUG
//#define Q_DEBUG
#endif

#define VDDA 3.3

const float a1 = 0.004091;
const float a2 = 0.008182;
const float a3 = -0.008182;
const float b1 = 0.09091;
const float b2 = 0.09091;
const float b3 = 0.09091;


//*****************************************************************************/
//    Operation Name: volt_to_kpa()
//
//    Processing:  Convert volt value to kpa value
//
//    Input Parameters: float volt
//
//    Output Parameters:
//
//    Return Values:  float kpa_val
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
static float pressure_ConvertVoltToKpa(float volt, uint8_t channelIndex)
{
	float result = 0.0;
	switch (channelIndex) {
		case PRODUCTANK_ADC_CHANNEL:
			result = (1/a1) * ((volt / VDDA) -b1);
			break;
		case COMP_POS_PRESSURE:
			result = (1/a1) * ((volt / VDDA) -b1);
			break;
		case CANNULA_PRESSURE:
			result = (1/a2) * ((volt / VDDA) -b2);
			break;
		case COMP_NEG_PRESSURE:
			result = (1/a3) * ((volt / VDDA) -b3);
			break;
		case EXT_FLOWPATH_PRESSURE:
			result = (1/a1) * ((volt / VDDA) -b1);
			break;
		default:
			break;
	}
	return result;
}

//*****************************************************************************/
//    Operation Name: productank_read_pressure()
//
//    Processing:  read product tank pressure value
//
//    Input Parameters: NONE
//
//    Output Parameters: float kpa_val
//
//    Return Values:  ADC_SENSOR_t
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
ADC_SENSOR_t pressure_ReadProductTank(void)
{
	ADC_VAL_t stAdcVal;
	ADC_SENSOR_t stAdcSenSor;
	float voltVal;
	float kpaVal;
	stAdcVal = adc_Read(PRODUCTANK_ADC_CHANNEL);
	voltVal = stAdcVal.voltVal;
	kpaVal = pressure_ConvertVoltToKpa(voltVal,PRODUCTANK_ADC_CHANNEL);
#ifdef Q_DEBUG
	DEBUGOUT("pressure volt val: %f \n",voltVal);
	DEBUGOUT("pressure kpa val: %f \n",kpaVal);
#endif
	stAdcSenSor.AdVal = stAdcVal.adVal;
	stAdcSenSor.sensorVal = kpaVal;
	return stAdcSenSor;
}

//*****************************************************************************/
//    Operation Name: compressor_pos_read_pressure()
//
//    Processing:  read compressor positive pressure value
//
//    Input Parameters: NONE
//
//    Output Parameters: float kpaVal
//
//    Return Values:  float kpaVal
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
float pressure_ReadCompressorPos(void)
{
	ADC_VAL_t stAdcVal;
	float voltVal;
	float kpaVal;
	stAdcVal = adc_Read(COMP_POS_PRESSURE);
	voltVal = stAdcVal.voltVal;
	kpaVal = pressure_ConvertVoltToKpa(voltVal,COMP_POS_PRESSURE);
#ifdef Q_DEBUG
	DEBUGOUT("compressor pos pressure volt val: %f \n",voltVal);
	DEBUGOUT("compressor pos pressure kpa val: %f \n",kpaVal);
#endif
	return kpaVal;
}

//*****************************************************************************/
//    Operation Name: cannula_read_pressure()
//
//    Processing:  read cannula pressure value
//
//    Input Parameters: NONE
//
//    Output Parameters: float kpa_val
//
//    Return Values:  float kpa_val
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
ADC_SENSOR_t pressure_ReadCannula(void)
{
	ADC_VAL_t stAdcVal;
	ADC_SENSOR_t stAdcSenSor;
	float voltVal;
	float kpaVal;
	stAdcVal = adc_Read(CANNULA_PRESSURE);
	voltVal = stAdcVal.voltVal;
	kpaVal = pressure_ConvertVoltToKpa(voltVal,CANNULA_PRESSURE);
#ifdef Q_DEBUG
	DEBUGOUT("cannula pressure volt val: %f \n",voltVal);
	DEBUGOUT("cannula pressure kpa val: %f \n",kpaVal);
#endif
	stAdcSenSor.AdVal = stAdcVal.adVal;
	stAdcSenSor.sensorVal = kpaVal;
	return stAdcSenSor;
}

//*****************************************************************************/
//    Operation Name: compressor_neg_read_pressure()
//
//    Processing:  read compressor negative pressure value
//
//    Input Parameters: NONE
//
//    Output Parameters: float kpa_val
//
//    Return Values:  float kpa_val
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
float pressure_ReadCompressorNeg(void)
{
	ADC_VAL_t stAdcVal;
	float voltVal;
	float kpaVal;
	stAdcVal = adc_Read(COMP_NEG_PRESSURE);
	voltVal = stAdcVal.voltVal;
	kpaVal = pressure_ConvertVoltToKpa(voltVal,COMP_NEG_PRESSURE);
#ifdef Q_DEBUG
	DEBUGOUT("compressor neg pressure volt val: %f \n",voltVal);
	DEBUGOUT("compressor neg pressure kpa val: %f \n",kpaVal);
#endif
	return kpaVal;
}

//*****************************************************************************/
//    Operation Name: ext_flow_path_read_pressure()
//
//    Processing:  read external flow path pressure value
//
//    Input Parameters: NONE
//
//    Output Parameters: float kpa_val
//
//    Return Values:  float kpa_val
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
float pressure_ReadExtFlowPath(void)
{
	ADC_VAL_t stAdcVal;
	float voltVal;
	float kpaVal;
	stAdcVal = adc_Read(EXT_FLOWPATH_PRESSURE);
	voltVal = stAdcVal.voltVal;
	kpaVal = pressure_ConvertVoltToKpa(voltVal,EXT_FLOWPATH_PRESSURE);
#ifdef Q_DEBUG
	DEBUGOUT("ext flow path pressure volt val: %f \n",voltVal);
	DEBUGOUT("ext flow path pressure kpa val: %f \n",kpaVal);
#endif
	return kpaVal;
}


