#ifndef INC_VALVE_CONTROL_H_
	#define INC_VALVE_CONTROL_H_

/********************************************************************/
/*                                                                  */
/* File Name    : ValveControl.h                                	*/
/*                                                                  */
/* General       : This module contain function for init and		*/
/* 					control valve									*/
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             Jun 2, 2016     Quyen Ngo(MV)  		new file        */
/*                                                                  */
/********************************************************************/

#include "gpio.h"
#include "DualCoreCommon.h"

typedef enum
{
	VAL1,
	VAL2,
	VAL3,
	VAL4,
	VAL5
} E_ValveNum;

// Config GPIO port for val control
void valve_Init(void);

E_ValStatus valve_getStatus(E_ValveNum valNum);

void valve_openVal1(void);
void valve_openVal2(void);
void valve_openVal3(void);
void valve_openVal4(void);
void valve_openVal5(void);

void valve_closeVal1(void);
void valve_closeVal2(void);
void valve_closeVal3(void);
void valve_closeVal4(void);
void valve_closeVal5(void);

#endif /* INC_VALVE_CONTROL_H_ */
