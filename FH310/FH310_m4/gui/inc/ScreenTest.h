#ifndef SCREENTEST_H_
#define SCREENTEST_H_

/********************************************************************/
/*                                                                  */
/* File Name    : ScreenTest.h                       			    */
/*                                                                  */
/* General       : ScreenTest allow user to test screen				*/
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

#define PIXEL_INCREASE 		16

//Define state for screen test
typedef enum {
	eTestBlackScreen,
	eTestWhiteScreen,
	eTestBlackWhiteScreen,
	eEndTestScreen
}E_TestScreen;

//Init screen test
void screenTest_Init(void);
//Show screen test
void screenTest_Show(void);
//Hide screen test
void screenTest_Hide(void);

#endif /* SCREENTEST_H_ */
/* end of file */

