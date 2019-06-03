#ifndef INC_SERVICESCREEN_H_
#define INC_SERVICESCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : ServiceScreen.h                       			*/
/*                                                                  */
/* General       : Service screen									*/
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

extern WM_HWIN g_serviceScreen;

//Init service screen
void serviceScreen_Init(void);
//Show service screen
void serviceScreen_Show(void);
//Hide service screen
void serviceScreen_Hide(void);

#endif /* INC_SERVICESCREEN_H_ */
/* end of file */
