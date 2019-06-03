#ifndef INC_PRESSURE_SENSOR_H_
	#define INC_PRESSURE_SENSOR_H_
/********************************************************************/
/*                                                                  */
/* File Name    : PressureSensor.h                                  */
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
/* #0001	   2017/11/02		Quyen Ngo(MV)		modify			*/
/*		-Change the return value of pressure_ReadProductTank and	*/
/*		pressure_ReadCannula to ADC_SENSOR_t for fix bug 2012		*/
/********************************************************************/

#include "adc.h"

/*Processing:  read product tank pressure value*/
ADC_SENSOR_t pressure_ReadProductTank(void);

/*Processing:  read compressor possitive pressure value*/
float pressure_ReadCompressorPos(void);

/*Processing:  read cannula pressure value*/
ADC_SENSOR_t pressure_ReadCannula(void);

/*Processing: compressor_neg_read_pressure*/
float pressure_ReadCompressorNeg(void);

/*Processing: ext_flow_path_read_pressure*/
float pressure_ReadExtFlowPath(void);

#endif /* INC_PRESSURE_SENSOR_H_ */
