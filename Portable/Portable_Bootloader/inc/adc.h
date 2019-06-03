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
/* Product Name  : Portable                                            */
/* 						                                            */
/* Company		 : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/09/28      Quyen Ngo  			new file  	    */
/*																	*/
/*                                                                  */
/********************************************************************/
#include <stdint.h>
#include "board.h"

#define PORTABLE_ADC_ID 				LPC_ADC0 /*ADC ID use for FH310*/
#define PROVIDE_PRESSURE_ADC_CH 		ADC_CH5 /*Productank pressure chanel*/
#define TANK_PRESSURE_ADC_CH 			ADC_CH2 /*Compressor positive pressure*/
#define VACUUM_PRESSURE_ADC_CH 			ADC_CH6 /*Cannula pressure sensor */
#define ILLU_SENSOR_ADC_CH				ADC_CH7 /*Illuminance sensor*/
#define DRV_TEMP_ADC_CH					ADC_CH1
#define KEY1_ADC_CH						ADC_CH4
#define KEY0_ADC_CH						ADC_CH3
/*fcv
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
float adc_Read(uint8_t channel);

#endif /* INC_ADC_H_ */
