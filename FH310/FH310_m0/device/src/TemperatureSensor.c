/********************************************************************/
/*                                                                  */
/* File Name    : TemperatureSensor.c                               */
/*                                                                  */
/* General       : This module contain function for read temperature
 * 				   sensor value										*/
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
/*             Aug 1, 2016     Quyen Ngo(MV)  		new file        */
/*                                                                  */
/********************************************************************/
#include <math.h>
#include "TemperatureSensor.h"

//*****************************************************************************/
//    Operation Name: temperature_ConvertVoltToDegC()
//
//    Processing:  Convert volt value to DegC value
//
//    Input Parameters: float volt, uint8_t channelIndex
//
//    Local variables: float result: degC value
//					   float Rth
//
//    Return Values:  float
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
static float temperature_ConvertVoltToDegC(float volt, uint8_t channelIndex)
{
	float result = 0.0;
	switch (channelIndex) {
	case INTERNAL_TEMPERATURE:
		result = (volt - 0.5) / 0.01; // formula for convert volt value to degC value of internal temperature sensor
		break;
	case COMP_TEMPERATURE:
	{
		float Rth = 0.0;
		Rth = 3300*volt/(3.3 - volt);
		result = 1/ ((log(Rth/ 10000)) / 3435 + (1/298.15)) - 273.15;
	}
		break;
	default:
		break;
	}
	return result;
}

//*****************************************************************************/
//    Operation Name: temperature_ReadInternalValue()
//
//    Processing:  read Unit internal temperature
//
//    Input Parameters: NONE
//
//    Local variable use: float voltVal
//						  float tempVal
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
ADC_SENSOR_t temperature_ReadValueOfInternal(void)
{
	ADC_VAL_t stAdcVal;
	ADC_SENSOR_t stAdcSenSor;
	float voltVal;
	float tempVal;
	stAdcVal = adc_Read(INTERNAL_TEMPERATURE);
	voltVal = stAdcVal.voltVal;
	tempVal = temperature_ConvertVoltToDegC(voltVal,INTERNAL_TEMPERATURE);
#ifdef Q_DEBUG
	DEBUGOUT("internal temperature volt val: %f \n",voltVal);
	DEBUGOUT("internal temperature kpa val: %f \n",tempVal);
#endif
	stAdcSenSor.AdVal = stAdcVal.adVal;
	stAdcSenSor.sensorVal = tempVal;
	return stAdcSenSor;
}

//*****************************************************************************/
//    Operation Name: temperature_ReadValueOfCompressor()
//
//    Processing:  compressor temperature read value
//
//    Input Parameters: NONE
//
//    Local variable use: float voltVal
//						  float tempVal
//
//    Return Values:  float
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
float temperature_ReadValueOfCompressor(void)
{
	ADC_VAL_t stAdcVal;
	float voltVal;
	float tempVal;
	stAdcVal = adc_Read(COMP_TEMPERATURE);
	voltVal = stAdcVal.voltVal;
	tempVal = temperature_ConvertVoltToDegC(voltVal,COMP_TEMPERATURE);
#ifdef Q_DEBUG
	DEBUGOUT("compressor temperature volt val: %f \n",voltVal);
	DEBUGOUT("compressor temperature kpa val: %f \n",tempVal);
#endif
	return tempVal;
}

