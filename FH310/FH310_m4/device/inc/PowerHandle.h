#ifndef INC_POWERHANDLE_H_
#define INC_POWERHANDLE_H_

/********************************************************************/
/*                                                                  */
/* File Name    : PowerDetection.h                                  */
/*                                                                  */
/* General       : This module contain function for init and handle */
/* 				   power detection interrupt, this interrupt only   */
/* 				   init when upgrade bootloader and M0 do not init  */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*           2017/08/04      	Quyen Ngo(MV)  		new file        */
/*                                                                  */
/********************************************************************/

void powerHandle_Init(void);

void powerHandle_DisablePowerDetection(void);


#endif /* INC_POWERHANDLE_H_ */
