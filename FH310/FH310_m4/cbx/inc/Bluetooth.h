#ifndef INC_BLUETOOTH_H_
	#define INC_BLUETOOTH_H_
/********************************************************************/
/*                                                                  */
/* File Name    : Bluetooth.h                       				*/
/*                                                                  */
/* General      : Contains function for init and communicate with	*/
/*                 Bluetooth BT121 module using dumo_bglib          */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/11/09      Quyen Ngo(MV)  	    new file        */
/*  #0001	   2017/09/15      Linh Nguyen		  	modify		    */
/*  modify the interface of BT send packet function				 	*/
/*  #0002	   2017/09/20      Linh Nguyen		  	modify		    */
/*  Rename macro and move to .h file							 	*/
/*  #0003	   2018/03/13      Quyen Ngo		  	modify		    */
/*  Add new function because the BT reset sequence specification 	*/
/*	change							 								*/
/********************************************************************/
#include "board.h"

#define BT_UART_SRB_SIZE 				1024*1024	/* Send */
#define BT_UART_RRB_SIZE 				1024*1024	/* Receive */

typedef enum
{
	DISCOVER_MODE,
	UNDISCOVER_MODE
} E_BtMode;

typedef enum {
	BT_IDLE,
	BT_WAIT_HEADER,
	BT_WAIT_DATA
} E_BTProcesPacketState;

#define BT_ADD_LENGTH				13

void bluetooth_Init(void);
int8_t* bluetooth_GetAddr(void);
void bluetooth_HandleResponse(void);
void bluetooth_ProceesPacketByBglib(uint8_t byteData);
void bluetooth_SendPacket(uint8_t* packet, uint32_t packetLen);
void bluetooth_Setmode(E_BtMode mode);
/** >>#003
	Add new function because the BT reset sequence specification change
#003 >>**/
void bluetooth_ChangeMode(E_BtMode mode);
void bluetooth_Reset(void);
#endif /* end of file */
