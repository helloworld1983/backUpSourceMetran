#ifndef INC_DEVELOPMENTSETTINGSCREEN_H_
#define INC_DEVELOPMENTSETTINGSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : DevelopmentSettingScreen.h                        */
/*                                                                  */
/* General       : The user is able to select the enable or			*/
/*  			   disable to each alarm functions. 				*/
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

extern WM_HWIN g_developmentSettingScreen;

//Init development setting screen
void developmentSettingScreen_Init(void);
//Show development setting screen
void developmentSettingScreen_Show(void);
//Hide development setting screen
void developmentSettingScreen_Hide(void);
//Check enable alarm
bool developmentSettingScreen_CheckEnable(E_AlarmId alarmId);

#endif /* INC_DEVELOPMENTSETTINGSCREEN_H_ */
/* end of file */
