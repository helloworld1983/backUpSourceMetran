#ifndef WDTHANDLE_H_
#define WDTHANDLE_H_
/********************************************************************/
/*                                                                  */
/* File Name    : WdtHandle.hS                          				*/
/*                                                                  */
/* General      : Contains functions for int and feed wdt in early	*/
/*				  state when RTOS not running						*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/09/21       Quyen Ngo(MV)	    new file        */
/*																	*/
/********************************************************************/

void wdt_InitWDT(void);

void wdt_SwitchToTaskObservation();

#endif /* end of file */
