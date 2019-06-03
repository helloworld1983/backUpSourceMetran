#ifndef RS232_H_
	#define RS232_H_

/********************************************************************/
/*                                                                  */
/* File Name    : RS232.h                       					*/
/*                                                                  */
/* General      : This module contains function for communicate with*/
/*                 CBX-02 via RS232							        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/* 		       2016/12/19      Quyen Ngo(MV)  	    new file        */
/*  #0001	   2017/09/20      Linh Nguyen		  	modify		    */
/*  Rename macro and move to .h file							 	*/
/*  Remove RS232_SendBtInfo function 								*/
/* #0002       2017/09/28       Linh Nguyen	  	    modify          */
/*  Change function name for coding rule					     	*/
/********************************************************************/

#include "board.h"

#define RS232_UART_SRB_SIZE 				128	/* Send */
#define RS232_UART_RRB_SIZE 				128	/* Receive */

typedef enum {
	RS232_WAIT_FIRST_BYTE,
	RS232_WAIT_DATA
} E_RS232ProcesPacketState;

void rs322_ReadFromCBX(void);

void rs232_Init(void);

void rs232_SendPacketToCbx(uint8_t* Packet, uint16_t CbxPacketLength);
#endif /* end of file */
