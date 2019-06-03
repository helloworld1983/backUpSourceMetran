/********************************************************************/
/*                                                                  */
/* File Name     : IlluminanceSensor.c                             	*/
/*                                                                  */
/* General       : This module contain function to read volatge		*/
/*                 value from illuminance sensor                    */
/*                                                                  */
/* Product Name  : Portable                                         */
/* 						                                            */
/* Company		 : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/23      Quyen Ngo  	new file       		    */
/*																	*/
/*                                                                  */
/********************************************************************/

#include <assert.h>
#include "IlluminanceSensor.h"
#include "Adc.h"

//#define DEBUG_ILLU_EN
#ifdef DEBUG_ILLU_EN
#define DEBUG_ILLU(...) printf(__VA_ARGS__)
#else
#define DEBUG_ILLU(...)
#endif

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	illuminance_ReadVoltValue	     			        				*/
/*                                                                                      */
/* Details: Read voltage value from illuminance sensor									*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : float				                                                    */
/*                                                                                      */
/****************************************************************************************/
float illuminance_ReadVoltValue()
{
	float illuVoltVal = adc_Read(ILLU_SENSOR_ADC_CH);
	return illuVoltVal;
}
