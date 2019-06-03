/********************************************************************/
/*                                                                  */
/* File Name    : FunctionTestPageThreeScreen.h                     */
/*                                                                  */
/* General       : The FunctionTestPageThree screen allows user to  */
/*                 do some function tests to check the hardware     */
/*                 such as: Compressor motor, Cooling Fan and       */
/*                 Filter cleaning module	                        */
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

#ifndef INC_FUNCTIONTESTPAGETHREESCREEN_H_
#define INC_FUNCTIONTESTPAGETHREESCREEN_H_

#include "DualCoreCommon.h"

#define FUNCTIONTEST_PAGETHREE_X		0
#define FUNCTIONTEST_PAGETHREE_Y		0
#define FUNCTIONTEST_PAGETHREE_SIZE_X	800
#define FUNCTIONTEST_PAGETHREE_SIZE_Y	480
#define SIZE_BUFFER_RPM 				5
#define DEFAULT_RPM_TEST_STR			((int8_t*)"50")
#define DEFAULT_DUTY_TEST_STR			((int8_t*)"100")

extern WM_HWIN g_functionTestPageThreeScreen;

//Init function test page three
void functionTestPageThreeScreen_Init(void);
//Show function test page three
void functionTestPageThreeScreen_Show(void);
//Hide function test page three
void functionTestPageThreeScreen_Hide(void);
//Update monitor
void functionTestPageThreeScreen_UpdateMonitor(IPC_MSG_DATA_t data);

#endif /* INC_FUNCTIONTESTPAGETHREESCREEN_H_ */
/* end of file */
