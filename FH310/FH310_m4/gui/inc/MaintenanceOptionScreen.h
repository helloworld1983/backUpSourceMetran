#ifndef INC_MAINTENANCEOPTIONSCREEN_H_
#define INC_MAINTENANCEOPTIONSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : MaintenanceOptionScreen.h                         */
/*                                                                  */
/* General      : Maintenance option screen display 				*/
/*                 	 maintenance option						        */
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

extern WM_HWIN g_maintenanceOptionScreen;

//Init maintenance option screen
void maintenanceOptionScreen_Init(void);
//Disable button when system is other standby mode
void maintenanceOptionScreen_SetDisableBtn(void);
//Enable button when system is standby mode
void maintenanceOptionScreen_SetEnableBtn(void);

#endif /* INC_MAINTENANCEOPTIONSCREEN_H_ */
/* end of file */
