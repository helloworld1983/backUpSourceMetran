#ifndef INC_FTDIANYPALHOST_H_
	#define INC_FTDIANYPALHOST_H_
/********************************************************************/
/*                                                                  */
/* File Name    : FtdiAnypalHost.h                       			*/
/*                                                                  */
/* General     : Contains functions for handle FTDI module			*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/* 		       2016/11/26      Quyen Ngo(MV)  	    new file        */
/* #0001       2017/09/28       Linh Nguyen	  	    modify          */
/*  Change function name for coding rule					     	*/
/*  Remove un use code										     	*/
/********************************************************************/
#include <lpc_types.h>
#include "USB.h"

typedef enum
{
	eAnypalNotConnect,
	eAnypalDiconnecting,
	eAnypalConnecting,
	eAnypalConnectingAfterOVCR,
	eAnypalConnected,
	eAnypalRetranferForTimeOut,
	eAnypalTransferForNewCycle,
	eAnypalSleep,
	eAnypalOverCurrent,
	eAnypalAlarm,
	eAnypalWaitReconnectForTimeOut
}AnypalState;

void ftdiAnypalHost_Handle(void);

uint8_t ftdiAnypalHost_SendPacketToATP(const uint8_t* const Buffer, uint32_t size);

void ftdiAnypalHost_CreateAnypalTask(void);

AnypalState ftdiAnypalHost_GetAnypalState(void);

void ftdiAnypalHost_SetAnypalState(AnypalState anypalState);

void ftdiAnypalHost_InitAnypalTask(void);

void ftdiAnypalHost_ReconnectAfterOVCR(void);
#endif /* end of file */
