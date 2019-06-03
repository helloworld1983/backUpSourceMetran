/********************************************************************/
/*                                                                  */
/* File Name     : LatchValve.c		                               	*/
/*                                                                  */
/* General       : Control latching valve				            */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/* 						                                            */
/* Company		 : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/16      Quyen Ngo  	new file       		    */
/*																	*/
/* #0001	   2017/09/01	   Quyen Ngo	modify		 	        */
/*		-remove assert												*/
/*																	*/
/* #0002	   2017/09/07	   Quyen Ngo(MV)	modify				*/
/*	 - Change the name element in enum E_LatchValSeq			   	*/
/*	 - Fix send message to M4 after finish request, current does not*/
/*	   correct for OpenClose request								*/
/* #0003	   2017/09/07	   Quyen Ngo(MV)	modify				*/
/*	 - Add new function for handle request control latch valve		*/
/*	 All request to control latch valve will call these function.	*/
/*	 If there is a new request to control latch valve, the previous */
/*	 will be cancel													*/
/* #0004	   2017/11/02	   Quyen Ngo(MV)	modify				*/
/*	 - Handle the case that IPC queue is full when send LatchValve	*/
/*	 Close finish message (Bug 2007)								*/
/********************************************************************/

/* include system files */
/* include user files */
#include "LatchValve.h"
#include "gpio.h"
#include "TaskCommon.h"
#include "timers.h"
#include "DualCoreCommon.h"
#include "string.h"

#include "../../ipclib/IpcMsg.h"
#include "../../ipc/IpcTask.h"

/* constants, macro definition */

#ifdef DEBUG
//#define DEBUG_LATCH_EN
#endif


#ifdef DEBUG_LATCH_EN
#define DEBUG_LATCH(...) printf(__VA_ARGS__)
#else
#define DEBUG_LATCH(...)
#endif

#define LATCH_WAIT_TIMER_ID 1
#define LATCH_INT_TIMER_ID 2

#ifndef pdMS_TO_TICKS
#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
#endif

/* definitions of structures */

/*global variable*/
const uint8_t g_latchWaitTime = 100; //100ms
const uint8_t g_latchIntTime = 75; //75ms
const uint8_t g_latchExtTime = 75; //75ms

static TimerHandle_t s_latchValWaitTimer;
static TimerHandle_t s_latchValIntTimer;
static E_LatchValSeq s_latchValSeq = eLatchValveCloseSeq;
static IPC_MSG_DATA_t gs_ipcLatchValveMsgData;
/* external declaration */

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: latchValve_Open
//
//    Detail: Start latching valve open sequence, switch flow path to external
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
static void latchValve_Open(void)
{
	s_latchValSeq = eLatchValveOpenSeq;
	latch1Close();
	latch2Close();
	if (xTimerStart(s_latchValWaitTimer,0) != pdPASS) {
		DEBUGOUT("ERR: Failed to start latch Valve wait timer \n");
	}
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: latchValve_Close
//
//    Detail: Start latching valve close sequence, switch flow path to internal
//	          when goto AC operation (start psa sequence)
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
static void latchValve_Close(void)
{
	s_latchValSeq = eLatchValveCloseSeq;
	latch1Close();
	latch2Close();
	if (xTimerStart(s_latchValWaitTimer,0) != pdPASS) {
		DEBUGOUT("ERR: Failed to start latch Valve wait timer \n");
	}
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: latchValveOpenAndClose
//
//    Detail: Open and close valve for switch flow path back to Internal and prevent valve
//			  sticking
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
static void latchValveOpenAndClose(void)
{
	s_latchValSeq = eLatchValveOpenCloseSeq;
	latch1Close();
	latch2Close();
	if (xTimerStart(s_latchValWaitTimer,0) != pdPASS) {
		DEBUGOUT("ERR: Failed to start latch Valve wait timer \n");
	}
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: latchValve_TimerCallback
//
//    Detail: Timer call back funtion call when timer time out
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
static void latchValve_TimerCallback( TimerHandle_t xTimer )
{
	uint32_t timerId =  ( uint32_t ) pvTimerGetTimerID( xTimer );
	switch (timerId) {
	// Psa start sequence
	case LATCH_WAIT_TIMER_ID:
		if(s_latchValSeq == eLatchValveOpenSeq)
		{
			latch2Open();
		}
		else if (s_latchValSeq == eLatchValveCloseSeq)
		{
			latch1Open();
		}
		else if (s_latchValSeq == eLatchValveOpenCloseSeq)
		{
			latch2Open();
		}
		if (xTimerStart(s_latchValIntTimer,0) != pdPASS) {
			DEBUGOUT("ERR: Failed to start latch Valve Int timer \n");
		}
		break;
	case LATCH_INT_TIMER_ID:
	{
		latch2Close();
		latch1Close();
		if (s_latchValSeq == eLatchValveOpenCloseSeq)
		{
			latchValve_Close();
		}
		else //Finish open or close or open close request
		{
			gs_ipcLatchValveMsgData.data_id = eLatchValveCloseFinish;
			gs_ipcLatchValveMsgData.data_value = 0x00;
			memset(&gs_ipcLatchValveMsgData.stAlarmStatPacket, 0, sizeof(gs_ipcLatchValveMsgData.stAlarmStatPacket));
			memset(&gs_ipcLatchValveMsgData.stPsaParamPacket, 0, sizeof(gs_ipcLatchValveMsgData.stPsaParamPacket));
			memset(&gs_ipcLatchValveMsgData.stPsaMonitorPacket, 0, sizeof(gs_ipcLatchValveMsgData.stPsaMonitorPacket));
			memset(&gs_ipcLatchValveMsgData.version, 0, sizeof(gs_ipcLatchValveMsgData.version));

			if (ipc_PushMsg(IPCEX_ID_GUI, gs_ipcLatchValveMsgData) == QUEUE_INSERT) {
				DEBUGOUT("M0 Push data: %d\n",gs_ipcLatchValveMsgData.data_id);
				gs_ipcLatchValveMsgData.data_id = eNoDeviceEventId;
				gs_ipcLatchValveMsgData.data_value = 0x00;
			}
		}
	}
	break;
	default:
		DEBUGOUT("Latch valve invalid timer id \n");
		break;
	}
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: latchValve_Init
//
//    Detail: Init and config latching valve 1,2
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
void latchValve_Init(void)
{
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,LATCH1_PORT_NUM, LATCH1_BIT_NUM);		/* output */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,LATCH2_PORT_NUM, LATCH2_BIT_NUM);		/* output */
	// Init value for out put pin
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,LATCH1_PORT_NUM, LATCH1_BIT_NUM);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,LATCH2_PORT_NUM, LATCH2_BIT_NUM);
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: latchValve_CreateTimer
//
//    Detail: Start latching valve open sequence, switch flow path to external
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
void latchValve_CreateTimer(void) {
	s_latchValWaitTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
		                      kernel. */
					"lchWaitTm",
					/* The timer period in ticks, must be
		                      greater than 0. */
					pdMS_TO_TICKS(g_latchWaitTime),
					/* The timers will auto-reload themselves
		                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
		                      number of times the timer has expired, which
		                      is initialised to 0. */
					( void * ) LATCH_WAIT_TIMER_ID,
					/* Each timer calls the same callback when
		                      it expires. */
					latchValve_TimerCallback
			);

	s_latchValIntTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
		                      kernel. */
					"lchIntTm",
					/* The timer period in ticks, must be
		                      greater than 0. */
					pdMS_TO_TICKS(g_latchIntTime),
					/* The timers will auto-reload themselves
		                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
		                      number of times the timer has expired, which
		                      is initialised to 0. */
					( void * ) LATCH_INT_TIMER_ID,
					/* Each timer calls the same callback when
		                      it expires. */
					latchValve_TimerCallback
			);
	return;
}



/****************************************************************************************/
/*                                                                                      */
/* Function name:  latchValve_RequestOpen												*/
/*                                                                                      */
/* Details:  cancel previous request and start new request								*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																						*/
/* ReturnValue : 		 																*/
/*                                                                                      */
/* Using Global Data : (O) TimerHandle_t s_latchValWaitTimer							*/
/*					   (O) TimerHandle_t s_latchValIntTimer								*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void latchValve_RequestOpen(void)
{
	//cancel previous request processing
	if (xTimerStop(s_latchValWaitTimer,0) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop latch Valve wait timer \n");
	}
	if (xTimerStop(s_latchValIntTimer,0) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop latch Valve int timer \n");
	}
	//process new request
	latchValve_Open();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  latchValve_RequestClose												*/
/*                                                                                      */
/* Details:  cancel previous request and start new request								*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																						*/
/* ReturnValue : 		 																*/
/*                                                                                      */
/* Using Global Data : (O) TimerHandle_t s_latchValWaitTimer							*/
/*					   (O) TimerHandle_t s_latchValIntTimer								*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void latchValve_RequestClose(void)
{
	//cancel previous request processing
	if (xTimerStop(s_latchValWaitTimer,0) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop latch Valve wait timer \n");
	}
	if (xTimerStop(s_latchValIntTimer,0) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop latch Valve int timer \n");
	}
	//process new request
	latchValve_Close();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  latchValve_RequestOpenAndClose										*/
/*                                                                                      */
/* Details:  cancel previous request and start new request								*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																						*/
/* ReturnValue : 		 																*/
/*                                                                                      */
/* Using Global Data : (O) TimerHandle_t s_latchValWaitTimer							*/
/*					   (O) TimerHandle_t s_latchValIntTimer								*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void latchValve_RequestOpenAndClose(void)
{
	//cancel previous request processing
	if (xTimerStop(s_latchValWaitTimer,0) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop latch Valve wait timer \n");
	}
	if (xTimerStop(s_latchValIntTimer,0) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop latch Valve int timer \n");
	}
	//process new request
	latchValveOpenAndClose();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  latchValve_ProcessPendingIpcMsg										*/
/*                                                                                      */
/* Details:  Resends latch valve close finish  message if previous send is failed		*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																						*/
/* ReturnValue : 		 																*/
/*                                                                                      */
/* Using Global Data : 	(I)	IPC_MSG_DATA_t gs_ipcLatchValveMsgData						*/
/*					   																	*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void latchValve_ProcessPendingIpcMsg(void)
{
	if(gs_ipcLatchValveMsgData.data_id == eLatchValveCloseFinish)
	{
		if (ipc_PushMsg(IPCEX_ID_GUI, gs_ipcLatchValveMsgData) != QUEUE_INSERT) {
			DEBUGOUT("M0 Push data Err: %d\n",gs_ipcLatchValveMsgData.data_id);
		}
		else
		{
			gs_ipcLatchValveMsgData.data_id = eNoDeviceEventId;
			gs_ipcLatchValveMsgData.data_value = 0x00;
		}
	}
	return;
}

/* end of file */
