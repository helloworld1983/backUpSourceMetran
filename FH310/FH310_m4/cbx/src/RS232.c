/********************************************************************/
/*                                                                  */
/* File Name    : RS232.c                       					*/
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
/*  Remove RS232_SendBtInfo function and use define					*/
/*	Check packet len when receive packet via RS232					*/
/*  #0002      2017/09/28       Linh Nguyen	  	    modify          */
/*  Change function name for coding rule					     	*/
/*	Handle more case of getting packet for CBX function				*/
/* 		       2017/10/12       Linh Nguyen	  	    modify          */
/*  #0003 send the error when paket over packet length (bug 1949)	*/
/********************************************************************/
/* include system files */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
/* include user files */
#include "RS232.h"
#include "Bluetooth.h"
#include "board.h"
#include "util.h"
#include "CBX02Handler.h"
/* constants, macro definition */

#define RS232_BAUD					19200
/******************************************************************************/
//        CLASS DECLARATIONS
/******************************************************************************/

/* definitions of structures */

/*global variable*/

/* Transmit and receive ring buffers */
static RINGBUFF_T s_txringRS232, s_rxringRS232;
/* Transmit and receive buffers */
static uint8_t s_rxbuff[RS232_UART_RRB_SIZE], s_txbuff[RS232_UART_SRB_SIZE];

/* external declaration */



/****************************************************************************************/
/*                                                                                      */
/* General rs232_SendPacketToCbx	                                           		    */
/*                                                                                      */
/* Details  -Handle interrupt for Uart 2		                 						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t* Packet		 					  							*/
/*             (I) uint8_t CbxPacketLength												*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
void rs232_SendPacketToCbx(uint8_t* Packet, uint16_t CbxPacketLength)
{
	Chip_UART_SendRB(LPC_USART2, &s_txringRS232, Packet, CbxPacketLength);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General Uart2 IRQ handle			                                           		    */
/*                                                                                      */
/* Details  -Handle interrupt for Uart 2		                 						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void UART2_IRQHandler(void)
{
	/* Want to handle any errors? Do it here. */
	/* Use default ring buffer handler. Override this with your own
	   code if you need more capability. */
	Chip_UART_IRQRBHandler(LPC_USART2, &s_rxringRS232, &s_txringRS232);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General rs232_Init				                                           		    */
/*                                                                                      */
/* Details  -Init communication via RS232C			             						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t* Packet		 					  							*/
/*             (I) uint8_t CbxPacketLength												*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
void rs232_Init(void)
{
	Board_UART_Init(LPC_USART2);

	/* Setup UART for 115.2K8N1 */
	Chip_UART_DeInit(LPC_USART2);
	Chip_UART_Init(LPC_USART2);
	Chip_UART_SetBaud(LPC_USART2, RS232_BAUD);
	Chip_UART_ConfigData(LPC_USART2, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(LPC_USART2, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(LPC_USART2);

	/* Before using the ring buffers, initialize them using the ring
					   buffer init function */
	RingBuffer_Init(&s_rxringRS232, s_rxbuff, 1, RS232_UART_RRB_SIZE);
	RingBuffer_Init(&s_txringRS232, s_txbuff, 1, RS232_UART_SRB_SIZE);

	/* Reset and enable FIFOs, FIFO trigger level 3 (14 chars) */
	Chip_UART_SetupFIFOS(LPC_USART2, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
			UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(LPC_USART2, (UART_IER_RBRINT | UART_IER_RLSINT));

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(USART2_IRQn, 1);
	NVIC_EnableIRQ(USART2_IRQn);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General rs232_Init				                                           		    */
/*                                                                                      */
/* Details  -Irs232_GetOnePacketFromCBX				             						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t* data		 					  							*/
/*             (I) int32_t datalen														*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
static void rs232_GetOnePacketFromCBX(uint8_t* data, int32_t datalen){

	static E_CBX02ReceivePacketState BTState = CBX_WAIT_STX;
	static uint8_t packetLen;
	static uint8_t cbxReceivePacket[PACKETIN_LENGTH];
	static uint8_t crcCount = 0;
	uint8_t *c = data;
	for (int i = 0; i < datalen; i++)
	{
		switch (BTState)
		{
		case CBX_WAIT_STX:
		{
			if(*c == STX_BYTE)
			{
				memset(cbxReceivePacket,0,PACKETIN_LENGTH);
				packetLen =0;
				cbxReceivePacket[packetLen++] = *c;
				BTState = CBX_WAIT_ETX;
			}
			break;
		}
		case CBX_WAIT_ETX:
		{
			cbxReceivePacket[packetLen++] = *c;
			if(packetLen > PACKETIN_LENGTH)
			{
				BTState = CBX_WAIT_STX;
				uint8_t mC1;
				if (cbxReceivePacket[1] == DLE_BYTE)
				{
					mC1 = cbxReceivePacket[2];
				}
				else
				{
					mC1 = cbxReceivePacket[1];
				}
				cbx_isPacketFormRS232(true);
				cbx_SendErrPacketToCbx(mC1, ERR_PARAM_LENTH);
			}
			else if(*c == STX_BYTE)
			{
				memset(cbxReceivePacket,0,PACKETIN_LENGTH);
				packetLen =0;
				cbxReceivePacket[packetLen++] = *c;
				BTState = CBX_WAIT_ETX;
			}
			else if(*c == ETX_BYTE)
			{
				crcCount = 0;
				BTState = CBX_WAIT_CRC;
			}
			break;
		}
		case CBX_WAIT_CRC:
		{
			cbxReceivePacket[packetLen++] = *c;
			crcCount++;
			if(packetLen > PACKETIN_LENGTH)
			{
				BTState = CBX_WAIT_STX;
				uint8_t mC1;
				if (cbxReceivePacket[1] == DLE_BYTE)
				{
					mC1 = cbxReceivePacket[2];
				}
				else
				{
					mC1 = cbxReceivePacket[1];
				}
				cbx_isPacketFormRS232(true);
				cbx_SendErrPacketToCbx(mC1, ERR_PARAM_LENTH);
			}
			else if(*c == STX_BYTE)
			{
				memset(cbxReceivePacket,0,PACKETIN_LENGTH);
				packetLen =0;
				cbxReceivePacket[packetLen++] = *c;
				BTState = CBX_WAIT_ETX;
			}
			else if (crcCount == 4)
			{
				DEBUGOUT("cbx packet:");
				for(int i =0; i<packetLen; i++)
				{
					DEBUGOUT(" 0x%x",cbxReceivePacket[i]);
				}
				DEBUGOUT("\n");
				BTState = CBX_WAIT_STX;
				cbx_isPacketFormRS232(true);
				cbx_ReceivePacketFromCbx(cbxReceivePacket, packetLen);
			}
			break;
		}
		default:
			break;
		}
		c++;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General rs322_ReadFromCBX		                                           		    */
/*                                                                                      */
/* Details  -Read data from CBX module				             						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t* data		 					  							*/
/*             (I) int32_t datalen														*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
void rs322_ReadFromCBX(void)
{
	uint8_t tmpBuf[PACKETIN_LENGTH]={0};
	int n = 0;

	n = Chip_UART_ReadRB(LPC_USART2, &s_rxringRS232, tmpBuf, PACKETIN_LENGTH);
	if (n>0)
	{
		rs232_GetOnePacketFromCBX(tmpBuf, n);
	}
	return;
}
