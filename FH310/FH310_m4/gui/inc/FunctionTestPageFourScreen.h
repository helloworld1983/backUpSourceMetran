#ifndef INC_FUNCTIONTESTPAGEFOURSCREEN_H_
#define INC_FUNCTIONTESTPAGEFOURSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : FunctionTestPageFourScreen.h                      */
/*                                                                  */
/* General       : The FunctionTestPageFour screen allows user to   */
/*                 do some function tests to check the hardware     */
/*                 such as: Alarm LED, Voice, Buzzer,               */
/*                 LCD/Touch panel 		                            */
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

#define FUNCTIONTEST_PAGEFOUR_X		0
#define FUNCTIONTEST_PAGEFOUR_Y		0
#define FUNCTIONTEST_PAGEFOUR_SIZE_X	800
#define FUNCTIONTEST_PAGEFOUR_SIZE_Y	480

extern WM_HWIN g_functionTestPageFourScreen;

//Init function test page four
void functionTestPageFourScreen_Init(void);
//Show function test page four
void functionTestPageFourScreen_Show(void);
//Hide function test page four
void functionTestPageFourScreen_Hide(void);

#endif /* INC_FUNCTIONTESTPAGEFOURSCREEN_H_ */
/* end of file */
