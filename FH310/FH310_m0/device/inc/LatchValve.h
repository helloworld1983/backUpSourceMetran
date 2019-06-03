#ifndef INC_LATCHVALVE_H_
	#define INC_LATCHVALVE_H_

/********************************************************************/
/*                                                                  */
/* File Name    : LatchValve.h			                            */
/*                                                                  */
/* General       : This file contains functions for control 		*/
/* 				   latchvalve										*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/16      Quyen Ngo(MV)  	new file        	*/
/* #0001	   2017/08/30	   Quyen Ngo(MV)	modify				*/
/*	 - Change the name element in enum E_LatchValSeq			   	*/
/*	 - add new functions to handle request control latch valve		*/
/*																	*/
/* #0002	   2017/09/15	   Quyen Ngo(MV)	modify				*/
/*	 - Change the name element in enum E_LatchValSeq to avoid 	  	*/
/*	 conflict with E_LatchValveRequest item name					*/
/* #0003	   2017/11/02	   Quyen Ngo(MV)    modify				*/
/* 	 - Add latchValve_ProcessPendingIpcMsg to handle the case that  */
/*	 IPC queue is full(Bug 2007)									*/
/********************************************************************/

#include "gpio.h"

#define latch1Open(void) Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,LATCH1_PORT_NUM, LATCH1_BIT_NUM)
#define	latch1Close(void) Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,LATCH1_PORT_NUM, LATCH1_BIT_NUM)
#define latch2Open(void) Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,LATCH2_PORT_NUM, LATCH2_BIT_NUM)
#define latch2Close(void) Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,LATCH2_PORT_NUM, LATCH2_BIT_NUM)

typedef enum
{
	eLatchValveOpenSeq,
	eLatchValveCloseSeq,
	eLatchValveOpenCloseSeq
} E_LatchValSeq;

void latchValve_Init(void);

void latchValve_CreateTimer(void);

void latchValve_RequestOpen(void);

void latchValve_RequestClose(void);

void latchValve_RequestOpenAndClose(void);

void latchValve_ProcessPendingIpcMsg(void);

#endif /* end of file */
