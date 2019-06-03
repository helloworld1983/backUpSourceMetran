#ifndef INC_DEVELOPMENTSETTINGSCREENPAGETHREE_H_
#define INC_DEVELOPMENTSETTINGSCREENPAGETHREE_H_

/********************************************************************/
/*                                                                  */
/* File Name    : DevelopmentSettingScreenPageThree.h               */
/*                                                                  */
/* General       : The user is able to dump file into USB memory	*/
/*  																*/
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

#define GUI_DELAY_DEVELOP_PAGE_THREE						50

extern WM_HWIN g_developmentSettingScreenPageThree;

//Init development setting screen page three
void developmentSettingScreenPageThree_Init(void);
//Show development setting screen page three
void developmentSettingScreenPageThree_Show(void);
//Hide development setting screen page three
void developmentSettingScreenPageThree_Hide(void);
//Close pop up
void developmentSettingScreenPageThree_ClosePopUp(bool saveResult);
//Show clear pop up
void developmentSettingScreenPageThree_ClearPopUp(void);
//Send message to usb task
void developmentSettingScreenPageThree_SendToUSB(void);

#endif /* INC_DEVELOPMENTSETTINGSCREENPAGETHREE_H_ */
/* end of file */
