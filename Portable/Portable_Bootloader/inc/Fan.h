#ifndef DEVICE_INC_FAN_H_
#define DEVICE_INC_FAN_H_
/********************************************************************/
/*                                                                  */
/* File Name    : Fan.h                        				        */
/*                                                                  */
/* General      : Initiallize Fan									*/
/*                Contrl FAN voltage                                */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*       December 22, 2016     Thao Ha(MV)  		new file        */
/*                                                                  */
/********************************************************************/
#include "board.h"
typedef enum
{
	OFF,
	OPERATE_5V,
	OPERATE_9V,
	OPERATE_12V
} E_FanVoltage;
void fan_Init(void);
void fan_SetVoltage(E_FanVoltage);

#endif /* DEVICE_INC_FAN_H_ */
