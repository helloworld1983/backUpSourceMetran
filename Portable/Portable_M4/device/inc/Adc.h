#ifndef INC_ADC_H_
#define INC_ADC_H_
/********************************************************************/
/*                                                                  */
/* File Name    : Adc.h   		                                 	*/
/*                                                                  */
/* General      : Initialization adc driver							*/
/* 				  Read adc value                                    */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            April 18,2017     Thao Ha(MV)		  	    new file    */
/*                                                                  */
/********************************************************************/
#include <stdint.h>
#include "board.h"

#define PORTABLE_ADC_ID 				LPC_ADC0 /*ADC ID use for Portable*/
#define PROVIDE_PRESSURE_ADC_CH 		ADC_CH5 /*Productank pressure chanel*/
#define TANK_PRESSURE_ADC_CH 			ADC_CH2 /*Compressor positive pressure*/
#define VACUUM_PRESSURE_ADC_CH 			ADC_CH6 /*Cannula pressure sensor */
#define ILLU_SENSOR_ADC_CH				ADC_CH7 /*Illuminance sensor*/
#define DRV_TEMP_ADC_CH					ADC_CH1
#define KEY1_ADC_CH						ADC_CH4
#define KEY0_ADC_CH						ADC_CH3

/****************************************************************************************/
/*                                                                                      */
/* Function name:   adc_Init                                                            */
/*                                                                                      */
/* Details:        Init ADC and config ADC channel                                      */
/*                 Sample rate = 400kHz                                                 */
/*                 Set 10bits                                                           */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void adc_Init(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   adc_Read                                                            */
/*                                                                                      */
/* Details:        read adc value from chanel                                           */
/*                                                                                      */
/* Arguments:   uint8_t channel                                                         */
/*                                                                                      */
/* ReturnValue : float adc converison value from chanel                                 */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
float adc_Read(uint8_t channel);

#endif /* INC_ADC_H_ */
