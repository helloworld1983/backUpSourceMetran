/********************************************************************/
/*                                                                  */
/* File Name    : SettingOpenCloseButton.h                          */
/*                                                                  */
/* General      : The SettingOpenCloseButton category provides      */
/* 				  the user an interface to display and              */
/*                change setting open/close in system               */
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
/* #0001       2017/11/03       Viet Le				modify			*/
/*    Remove settingOpenCloseButton_GetValue() function (not use)   */
/********************************************************************/

#ifndef SETTINGOPENCLOSEBUTTON_H_
#define SETTINGOPENCLOSEBUTTON_H_

#include "WM.h"
#include "EDIT.h"
#include "stdbool.h"
#include "GuiTask.h"

//Init setting open/close button
void settingOpenCloseButton_Init(EDIT_Handle obj, int openBtnId,int closeBtnId, int valueId, GUI_RECT Rect);
//Custom setting open/close button
void settingOpenCloseButton_Callback(WM_MESSAGE *pMsg);
//Set value for setting
void settingOpenCloseButton_SetValue(void);
//Set off latchinh valve
void settingOpenCloseButton_SetOffLatchingValve(void);

#endif /* SETTINGOPENCLOSEBUTTON_H_ */
/* end of file */

