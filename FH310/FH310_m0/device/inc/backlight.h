#ifndef INC_BACKLIGHT_H_
	#define INC_BACKLIGHT_H_

/********************************************************************/
/*                                                                  */
/* File Name     : backlight.h			                            */
/*                                                                  */
/* General       : This file contain function for control backlight */
/*                 depend on the brightness condition of the 		*/
/*				   environment and step value of backlight setting  */
/*                                                                  */
/* Product Name  : FH310                                            */
/* 						                                            */
/* Company		 : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/07      Quyen Ngo  			new file  	    */
/*																	*/
/*                                                                  */
/********************************************************************/
#include "PwmController.h"

void backlight_Init(void);

void backlight_Setduty(uint8_t dutyCycle);

void backlight_Handle(void);

void backlight_SetStep(uint8_t step);

#endif /* INC_BACKLIGHT_H_ */
/* end of file */
