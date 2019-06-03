#ifndef TOUCHPANELSCREEN_H_
#define TOUCHPANELSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : TouchPanelScreen.h                       			*/
/*                                                                  */
/* General       : ScreenTest allow user to test touch panel		*/
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

#define X_LIST_SIZE 		20
#define Y_LIST_SIZE 		12
#define DIV_40				40

//Init touch panel test
void touchPanelScreen_Init(void);
//Show touch panel test
void touchPanelScreen_Show(void);
//Hide touch panel test
void touchPanelScreen_Hide(void);

#endif /* TOUCHPANELSCREEN_H_ */
/* end of file */
