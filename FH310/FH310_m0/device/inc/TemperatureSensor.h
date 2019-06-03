#ifndef INC_TEMPERATURE_SENSOR_H_
	#define INC_TEMPERATURE_SENSOR_H_

/********************************************************************/
/*                                                                  */
/* File Name    : TemperatureSensor.h                               */
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
/* #0001	   2017/11/02		Quyen Ngo(MV)		modify			*/
/*		-Change the return value of pressure_ReadProductTank and	*/
/*		pressure_ReadCannula to ADC_SENSOR_t for fix bug 2012		*/
/********************************************************************/

#include "adc.h"

/* read internal temperature */
ADC_SENSOR_t temperature_ReadValueOfInternal(void);

/* read compressor temperature */
float temperature_ReadValueOfCompressor(void);

#endif /* INC_TEMPERATURE_SENSOR_H_ */
