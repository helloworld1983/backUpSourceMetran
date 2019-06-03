#ifndef INC_CREATECONTACTDIALOG_H_
#define INC_CREATECONTACTDIALOG_H_

/********************************************************************/
/*                                                                  */
/* File Name    : CreateContactDialog.h                             */
/*                                                                  */
/* General       : Create contact dialog		 		            */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/22      Viet Le			  	new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/
#include "WM.h"

#define BORDER_CONTACT_COLOR 			0xECC9B1
#define SIZE_BUFFER_NUMBER_CONTACT		3

extern WM_HWIN g_createContactDialog;

//Init create contact dialog
void createContactDialog_Init(void);
//Show create contact dialog
void createContactDialog_Show(void);
//Hide create contact dialog
void createContactDialog_Hide(void);

#endif /* INC_CREATECONTACTDIALOG_H_ */
/* end of file */
