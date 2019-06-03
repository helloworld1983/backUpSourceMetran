#ifndef INC_USBHOSTTASK_H_
	#define INC_USBHOSTTASK_H_
/********************************************************************/
/*                                                                  */
/* File Name    : USBHostTask.h                       				*/
/*                                                                  */
/* General      : This file contains the function for create and run*/
/*      USB host freeRTOS task, which is responsible for process    */
/*              USB host event and feed Watchdog                    */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/* #0001       2016/12/25      Quyen Ngo(MV)  	    new file        */
/* #0002       2017/08/24       Linh Nguyen	  	    modify          */
/*  Add get and set flag function to handle upgrade bootloader 		*/
/*                  failure                                   		*/
/********************************************************************/

#include "board.h"

void usbHostTask_Create(void);

bool GetUpdateResult(void);

void SetUpdateResult(bool value);

#endif /* end of file */
