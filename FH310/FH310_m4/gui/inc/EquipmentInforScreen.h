#ifndef INC_EQUIPMENTINFORSCREEN_H_
#define INC_EQUIPMENTINFORSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : EquipmentInforScreen.h                            */
/*                                                                  */
/* General       : Displaying data in equipment information 		*/
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

extern WM_HWIN g_equipmentInforScreen;

//Init equipment Information screen
void equipmentInforScreen_Init(void);
//Show equipment Information screen
void equipmentInforScreen_Show(void);
//Hide equipment Information screen
void equipmentInforScreen_Hide(void);
//Update monitor
void equipmentInforScreen_UpdatePSA(IPC_MSG_DATA_t data);
//Update calendar and cumulative operation hour
void equipmentInforScreen_UpdateCalendarCumulative(void);
//Change flow rate in equipment infor
void equipmentInforScreen_ChangeFlowrate(uint8_t FRPSAValue);
//Confirm when change flow rate in childlock mode
void equipmentInforScreen_ConfirmChildLock(uint8_t FRPSAValue);

#endif /* INC_EQUIPMENTINFORSCREEN_H_ */
/* end of file */
