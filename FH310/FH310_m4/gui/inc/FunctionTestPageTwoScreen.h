#ifndef INC_FUNCTIONTESTPAGETWOSCREEN_H_
#define INC_FUNCTIONTESTPAGETWOSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : FunctionTestPageTwoScreen.h                       */
/*                                                                  */
/* General       : The FunctionTestPageTwo screen allows user to do */
/*                 some function tests to check the hardware 		*/
/*                 such as: Valve 1, Valve 2, Valve 3, Valve 4, 	*/
/*                 Valve 5 and Latching Valve	                    */
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

#define FUNCTIONTEST_PAGETWO_X		0
#define FUNCTIONTEST_PAGETWO_Y		0
#define FUNCTIONTEST_PAGETWO_SIZE_X	800
#define FUNCTIONTEST_PAGETWO_SIZE_Y	480

extern WM_HWIN g_functionTestPageTwoScreen;

//Init function test page two
void functionTestPageTwoScreen_Init(void);
//Show function test page two
void functionTestPageTwoScreen_Show(void);
//Hide function test page two
void functionTestPageTwoScreen_Hide(void);

#endif /* INC_FUNCTIONTESTPAGETWOSCREEN_H_ */
/* end of file */
