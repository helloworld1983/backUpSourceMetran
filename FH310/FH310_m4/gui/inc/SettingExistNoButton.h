#ifndef INC_SETTINGEXISTNOBUTTON_H_
#define INC_SETTINGEXISTNOBUTTON_H_

/********************************************************************/
/*                                                                  */
/* File Name    : SettingExistNoButton.h                            */
/*                                                                  */
/* General      : The SettingExistNoButton category provides        */
/* 				  the user an interface to display and              */
/*                change setting O2 cynlinder in system             */
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
#include "EDIT.h"
#include "stdbool.h"

//Init setting Exist/No button
void settingExistNoButton_InitExistNoBtn(EDIT_Handle obj,int existBtnId,int noBtnId, int valueId, GUI_RECT Rect);
//Custom setting Exist/No button
void settingExistNoButton_Callback(WM_MESSAGE *pMsg);
//Get value setting Exist/No button
bool settingExistNoButton_GetValue(void);
//Set value for setting
void settingExistNoButton_SetValue(void);

#endif /* INC_SETTINGEXISTNOBUTTON_H_ */
/* end of file */
