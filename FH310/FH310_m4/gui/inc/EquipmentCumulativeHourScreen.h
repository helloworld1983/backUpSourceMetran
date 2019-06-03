#ifndef INC_EQUIPMENTCUMULATIVEHOURSCREEN_H_
#define INC_EQUIPMENTCUMULATIVEHOURSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : EquipmentCumulativeHourScreen.h                   */
/*                                                                  */
/* General       : Displaying unit cumulative hour in 				*/
/* 				   equipment information 							*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2008/08/22      Viet Le  			new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#include "WM.h"
#include "DualCoreCommon.h"

extern WM_HWIN g_equipmentCumulativeHourScreen;

//Init equipment unit cumualtive hour information screen
void equipmentCumulativeHourScreen_Init(void);
//Show equipment unit cumualtive hour information screen
void equipmentCumulativeHourScreen_Show(void);
//Hide equipment unit cumualtive hour information screen
void equipmentCumulativeHourScreen_Hide(void);
//Display cumulative operation hour
void equipmentCumulativeHourScreen_DisplayCumulative(void);

#endif /* INC_EQUIPMENTCUMULATIVEHOURSCREEN_H_ */
/* end of file */
