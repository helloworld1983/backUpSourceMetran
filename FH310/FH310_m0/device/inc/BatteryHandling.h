#ifndef INC_BATTERYHANDLING_H_
#define INC_BATTERYHANDLING_H_

/********************************************************************/
/*                                                                  */
/* File Name     : battery_handling.h                               */
/*                                                                  */
/* General       : NiMH Battery handling sequence                   */
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
/*             2016/08/03      Quyen Ngo  	new file       		    */
/*																	*/
/* #0001	   2017/09/08	   Quyen Ngo	modify					*/
/*		-remove battery shutdown									*/
/*                                                                  */
/********************************************************************/


//init battery
void battery_Init(void);

void battery_Handle(void);

#endif /* INC_BATTERYHANDLING_H_ */
/* end of file */
