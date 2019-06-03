#ifndef INC_OPTIONSCREEN_H_
#define INC_OPTIONSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : OptionScreen.h                               		*/
/*                                                                  */
/* General      : Option screen display		                	    */
/*                 service option	    				            */
/*				   maintenance option	      						*/
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

//Define tab index
typedef enum {
	eServiceTabId,
	eMaintenanceTabId
}E_TabIndex;

extern WM_HWIN g_optionScreen;

//Init option screen
void optionScreen_Init(void);
//Show option screen
void optionScreen_Show(void);
//Hide option screen
void optionScreen_Hide(void);
//Set tab in option screen
void optionScreen_SetTab(E_TabIndex index);

#endif /* INC_OPTIONSCREEN_H_ */
/* end of file */
