#ifndef INC_OTHERSETTINGSCREEN_H_
#define INC_OTHERSETTINGSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : OtherSettingScreen.h                              */
/*                                                                  */
/* General      : OtherSettingScreen allow user change language     */
/* 				  setting.										    */
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

extern WM_HWIN g_otherSettingScreen;

//Init other Setting Screen
void otherSettingScreen_Init(void);
//Show other Setting Screen
void otherSettingScreen_Show(void);
//Hide other Setting Screen
void otherSettingScreen_Hide(void);

#endif /* INC_OTHERSETTINGSCREEN_H_ */
/* end of file */
