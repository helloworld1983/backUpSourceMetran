#ifndef INC_COOLINGFAN_H_
	#define INC_COOLINGFAN_H_

/********************************************************************/
/*                                                                  */
/* File Name     : CoolingFan.h		                                */
/*                                                                  */
/* General       : This file contains the functions for init,	    */
/*				  control cooling fan and detect error              */
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
/*             2016/10/07      Quyen Ngo  	new file       		    */
/*																	*/
/*                                                                  */
/********************************************************************/
#include "PwmController.h"

void coolingFan_Init(void);

void coolingFan_Setduty(uint8_t dutyCycle);

uint8_t coolingFan_getDuty(void);

bool coolingFan_GerErr(void);

#endif /* INC_COOLINGFAN_H_ */ /* end of file */
