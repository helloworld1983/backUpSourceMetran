#ifndef INC_PSAPARACHANGEDIALOG_H_
#define INC_PSAPARACHANGEDIALOG_H_

/********************************************************************/
/*                                                                  */
/* File Name    : PSAParaChangeDialog.h                             */
/*                                                                  */
/* General      : PSAParaChangeDialog allow user change psa 		*/
/* 				  parameter.										*/
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

#define BORDER_PSA_CHANGE_DIALOG_COLOR 0xDAB094
#define BUFFER_VALUE_PSA 			   10
#define PSA_VALUE_SCALE				   10
#define FC_OUTPUT_SCALE				   0.2
#define SIZE_BUFFER_TITLE_PSA		   100
#define SIZE_BUFFER_TEXT_PSA		   40

extern WM_HWIN g_PSAParaChangeDialog;

//Init PopUp PSA parameter change screen
void PSAParaChangeDialog_Init(void);
//Show PopUp PSA parameter change screen
void PSAParaChangeDialog_Show(void);
//Hide PopUp PSA parameter change screen
void PSAParaChangeDialog_Hide(void);

#endif /* INC_PSAPARACHANGEDIALOG_H_ */
/* end of file */
