#ifndef INC_PASSWORDDIALOG_H_
#define INC_PASSWORDDIALOG_H_

/********************************************************************/
/*                                                                  */
/* File Name    : PasswordDialog.h                                 	*/
/*                                                                  */
/* General      : Password dialog provides the virtual keyboard 	*/
/*                to enter your user password.						*/
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
/* #0001       2017/10/19       Viet Le				modify			*/
/*  Add passwordDialog_DisplayFlowRate() function  display flow rate*/
/*	 in password dialog when change flow rate (bug 1986)			*/
/********************************************************************/

#include "WM.h"
#include "board.h"

#define BORDER_PASS_COLOR 	0xECC9B1
#define TIMER_HIDE_PASS	  	10000//10000ms = 10s
#define SIZE_BUFFER_PASS  	10
#define GUI_DELAY_IN_PASS 	10
#define SIZE_NUMBER_BUFFER 	3

//Define User login to serice mode
typedef enum
{
	E_NO_USER,
	E_DISTRIBUTOR_USER,
	E_MANUFACTURE_INSPECTION_USER,
	E_MANUFACTURE_USER,
	E_MANUFACTURE_OPTION_USER,
	E_DEVELOPER_USER
}E_UserMaintenance;

extern WM_HWIN g_passwordDialog;

//Init password dialog
void passwordDialog_Init(void);
//Show password dialog
void passwordDialog_Show(void);
//Hide password dialog
void passwordDialog_Hide(void);
//Check file in usb
void passwordDialog_CheckFile(bool isExistFile);
//Get user login
E_UserMaintenance passwordDialog_GetUserLogin(void);
//Set background color for password dialog
void passwordDialog_SetBackgroundColor(GUI_COLOR color);
//Display flow rate
void passwordDialog_DisplayFlowRate(uint8_t FRPSAValue);

#endif /* INC_PASSWORDDIALOG_H_ */
/* end of file */
