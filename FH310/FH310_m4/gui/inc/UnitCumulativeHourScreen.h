#ifndef INC_UNITCUMULATIVEHOURSCREEN_H_
#define INC_UNITCUMULATIVEHOURSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : UnitCumulativeHourScreen.h                        */
/*                                                                  */
/* General      : The UnitCumulativeHourScreen category provides    */
/* 				  the user an interface to display and              */
/*                change unit cumulative hour in system             */
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
#include "DualCoreCommon.h"

#define SIZE_BUFFER_HOUR_UNIT			6

extern WM_HWIN g_unitCumulativeHourScreen;

//Init unit Cumulative operation Hour Screen
void unitCumulativeHourScreen_Init(void);
//Show  unit Cumulative operation Hour Screen
void unitCumulativeHourScreen_Show(void);
//Show hour cumulative operation hour
void unitCumulativeHourScreen_ShowUnitHour(void);
//Hide  unit Cumulative operation Hour Screen
void unitCumulativeHourScreen_Hide(void);
//Get flag change unit total
bool unitCumulativeHourScreen_GetFlagChangeUnitTotal(void);
//Set flag change unit total
void unitCumulativeHourScreen_SetFlagChangeUnitTotal(bool flag);
//Show all item
void unitCumulativeHourScreen_ShowAllItem(void);
//Hide all item
void unitCumulativeHourScreen_HideAllItem(void);

#endif /* INC_UNITCUMULATIVEHOURSCREEN_H_ */
/* end of file */
