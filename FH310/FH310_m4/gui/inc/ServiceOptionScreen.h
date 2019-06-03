#ifndef INC_SERVICEOPTIONSCREEN_H_
#define INC_SERVICEOPTIONSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : ServiceOptionScreen.h                             */
/*                                                                  */
/* General      : Service option screen display service option		*/
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
#include "stdbool.h"

extern WM_HWIN g_serviceOptionScreen;

//Init service option screen
void serviceOptionScreen_Init(void);
//Get flag equipment infor/setting
bool serviceOptionScreen_GetFlagEquipInfor(void);
//Disable button when system is other standby mode
void serviceOptionScreen_SetDisableBtn(void);
//Enable button when system is standby mode
void serviceOptionScreen_SetEnableBtn(void);

#endif /* INC_SERVICEOPTIONSCREEN_H_ */
/* end of file */
