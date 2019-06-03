#ifndef INC_EQUIPMENTSCREEN_H_
#define INC_EQUIPMENTSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : EquipmentScreen.h		                            */
/*                                                                  */
/* General      : Equipment screen include :						*/
/* 					-Equipment information screen				    */
/* 					-Equipment configure screen 					*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/14       Viet Le		  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/
#include "WM.h"

extern WM_HWIN g_equipmentScreen;

//Init equipment screen
void equipmentScreen_Init(void);
//Show equipment screen
void equipmentScreen_Show(void);
//Hide equipment screen
void equipmentScreen_Hide(void);

#endif /* INC_EQUIPMENTSCREEN_H_ */
/* end of file */
