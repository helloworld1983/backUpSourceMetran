#ifndef INC_ADC_H_
	#define INC_ADC_H_
/********************************************************************/
/*                                                                  */
/* File Name     : Adc.h			                                */
/*                                                                  */
/* General       : This module contain function for init and 		*/
/* 					read adc										*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/* 						                                            */
/* Company		 : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/09/28      Quyen Ngo  			new file  	    */
/*																	*/
/* #0001	   2017/11/02	   Quyen Ngo			modify			*/
/*		-Add ADC_VAL_t struct, change return value of adc_Read() 	*/
/*		to  ADC_VAL_t(Bug 2012)										*/
/*                                                                  */
/* #0002	   2017/11/08	   Quyen Ngo			modify			*/
/*		-Move ADC_SENSOR_t struct from DualCoreCommon.h to adc.h	*/
/********************************************************************/
#include <stdint.h>
#include "board.h"

#define FH310_ADC_ID 				LPC_ADC0 /*ADC ID use for FH310*/
#define PRODUCTANK_ADC_CHANNEL 		ADC_CH5 /*Productank pressure chanel*/
#define COMP_POS_PRESSURE 			ADC_CH1 /*Compressor positive pressure*/
#define CANNULA_PRESSURE 			ADC_CH2 /*Cannula pressure sensor */
#define COMP_NEG_PRESSURE 			ADC_CH6 /*Compressor negative pressure sensor*/
#define EXT_FLOWPATH_PRESSURE 		ADC_CH3 /*External flow path pressure sensor*/
#define INTERNAL_TEMPERATURE		ADC_CH7 /*Internal temperature sensor*/
#define COMP_TEMPERATURE 			ADC_CH4 /*Compressor outlet temperature sensor*/
#define ILLU_SENSOR_CHANNEL			ADC_CH0 /*Illuminance sensor*/

typedef struct
{
	uint16_t adVal;//adc value 10 bit
	float 	 voltVal;//voltage value
}ADC_VAL_t;

typedef struct
{
	uint16_t AdVal; //Adc value
	float	sensorVal; // sensor converted value
}ADC_SENSOR_t;

/*
 * Init adc and config adc chanel
 * +Sample rate = 400kHz
 * +Set 10bit
 */
void adc_Init(void);

/*
 * read adc value from chanel
 * @param: uint8_t channel chanel need to be read
 * @return: float adc converison value from chanel
 */
ADC_VAL_t adc_Read(ADC_CHANNEL_T channel);

#endif /* end of file */
