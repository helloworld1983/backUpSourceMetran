/********************************************************************/
/*                                                                  */
/* File Name    : OperationswHandle.c                               */
/*                                                                  */
/* General       : This module contain function for init and handle
 * 				  operation switch									*/
/*                                                                  */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/06/02     Quyen Ngo(MV)  		new file        */
/*                                                                  */
/* #0001       2017/09/01  	  Quyen Ngo				modify 	        */
/*		remove assert												*/
/* #0002	   2017/09/07	  Quyen Ngo				modify			*/
/*		change function name disable_opsw to correct naming rule	*/
/* #0003  	   2017/09/21	  Quyen Ngo				modify			*/
/*		change default state of s_isAllowPush2S to true				*/
/* #0004  	   2017/11/10	  Quyen Ngo				modify			*/
/*		Using queue to store event cannot send to M4 when QUEUE full*/
/*		(Bug 2018)													*/
/********************************************************************/

#include <string.h>

#include "OperationswHandle.h"

#include "../../ipclib/IpcMsg.h"
#include "../../ipc/IpcTask.h"

#ifdef DEBUG
//#define Q_DEBUG
//#define DEBUG_OPSW_EN
#endif

#ifdef DEBUG_OPSW_EN
#define DEBUG_OPSW(...) printf(__VA_ARGS__)
#else
#define DEBUG_OPSW(...)
#endif

#define OPSW_CNT_WAIT_RELEASE -1
#define OPSW_QUEUE_LENGTH		16
static OPRATIONSW_EVENT gs_opswActiveEvent = IDLE;

static bool s_isOpswPush = false;

static bool gs_isDisableOpsw = false;

static bool s_isAllowPush2S = true;

static bool gs_currentState = SW_UNPUSH;

//Opration sw previous state
static bool gs_peviousState = SW_UNPUSH;

//counting time SW is holding in PUSH state;
static int8_t gs_swPushCount = 0;

static uint8_t gs_disableCountTime = 0;

static OPRATIONSW_EVENT gs_swMsgQueueArr[OPSW_QUEUE_LENGTH];

static int8_t gs_swHead = - 1;
static int8_t gs_swTail = - 1;

/****************************************************************************************/
/*                                                                                      */
/* Function name:  operationsw_PushRequest												*/
/*                                                                                      */
/* Details:  Push the request into the gs_swMsgQueueArr									*/
/*                                                                                      */
/* Arguments :	(I) OPRATIONSW_EVENT event							 					*/
/*                                                                                      */
/* Macros:	   		OPSW_QUEUE_LENGTH													*/
/*																			 			*/
/* ReturnValue : 																		*/
/*                                                                                      */
/* Using Global Data : (I) OPRATIONSW_EVENT gs_swMsgQueueArr							*/
/*					   (O) int8_t gs_swHead												*/
/*					   (O) int8_t gs_swTail												*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static void operationsw_PushRequest(OPRATIONSW_EVENT event)
{
	if (gs_swHead == OPSW_QUEUE_LENGTH - 1)
		DEBUGOUT("Queue Overflow \n");
	else
	{
		if (gs_swTail == - 1)
			/*If queue is initially empty */
			gs_swTail = 0;
		DEBUGOUT("Insert the element in queue : ");
		gs_swHead = gs_swHead + 1;
		gs_swMsgQueueArr[gs_swHead] = event;
		DEBUGOUT("PARAM SET: %d %d \n",gs_swHead,gs_swMsgQueueArr[gs_swTail]);
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  operationsw_PopRequest												*/
/*                                                                                      */
/* Details:  Pop the request on the gs_swMsgQueueArr									*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		OPSW_QUEUE_LENGTH													*/
/*																			 			*/
/* ReturnValue : 	OPSW_POP_REQUEST_RETURN_T											*/
/*                                                                                      */
/* Using Global Data : (I) OPRATIONSW_EVENT gs_swMsgQueueArr							*/
/*					   (O) int8_t gs_swHead												*/
/*					   (O) int8_t gs_swTail												*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static OPSW_POP_REQUEST_RETURN_T operationsw_PopRequest(void)
{
	OPSW_POP_REQUEST_RETURN_T request;
	if (gs_swTail == - 1 || gs_swTail > gs_swHead)
	{
		request.status = false;
	}
	else
	{
		request.event = gs_swMsgQueueArr[gs_swTail];
		request.status = true;
		DEBUGOUT("PARAM GET: %d %d \n",gs_swTail,gs_swMsgQueueArr[gs_swTail]);
		if(gs_swTail < OPSW_QUEUE_LENGTH - 1)
		{
			gs_swTail = gs_swTail + 1;
		}
		else
		{
			gs_swTail = -1;
			gs_swHead = -1;
		}
	}
	return request;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  opearationsw_ReturnThePreviousPop									*/
/*                                                                                      */
/* Details:  Return QUEUE back to previous Pop index									*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 																		*/
/*                                                                                      */
/* Using Global Data : 	(O) int8_t gs_swTail											*/
/*					  																	*/
/*					   																	*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static void opearationsw_ReturnThePreviousPop(void)
{
	if(gs_swTail >0)
	{
		gs_swTail = gs_swTail -1;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  operationsw_ProcessPendingIpcMsg										*/
/*                                                                                      */
/* Details:  Pop the pending IPC Msg QUEUE and process it, if it cannnot push Msg to IPC*/
/*			 QUEUE, return msg back to QUEUE											*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		OPSW_QUEUE_LENGTH													*/
/*																			 			*/
/* ReturnValue : 	OPSW_POP_REQUEST_RETURN_T											*/
/*                                                                                      */
/* Using Global Data : (I) OPRATIONSW_EVENT gs_swMsgQueueArr							*/
/*					   (O) int8_t gs_swHead												*/
/*					   (O) int8_t gs_swTail												*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static void operationsw_ProcessPendingIpcMsg(void)
{
	IPC_MSG_DATA_t data;
	data.data_id = eNoDeviceEventId;
	data.data_value = 0;
	memset(&data.stAlarmStatPacket, 0, sizeof(data.stAlarmStatPacket));
	memset(&data.stPsaParamPacket, 0, sizeof(data.stPsaParamPacket));
	memset(&data.stPsaMonitorPacket, 0, sizeof(data.stPsaMonitorPacket));
	memset(&data.version, 0, sizeof(data.version));
	OPSW_POP_REQUEST_RETURN_T request;
	request = operationsw_PopRequest();
	if(request.status == true)
	{
		switch (request.event) {
		case TOGGLE:
			data.data_id = eOperationswToggle;
			data.data_value = 0x00;
			if (ipc_PushMsg(IPCEX_ID_GUI, data) != QUEUE_INSERT) {
				opearationsw_ReturnThePreviousPop();
			}
			break;
		case TOGGLESHORT:
			data.data_id = eOperationswToggleShort;
			data.data_value = 0x00;
			if (ipc_PushMsg(IPCEX_ID_GUI, data) != QUEUE_INSERT) {
				opearationsw_ReturnThePreviousPop();
			}
			break;
		case TOGGLE2S:
			data.data_id = eOperationswToggle2S;
			data.data_value = 0x00;
			if (ipc_PushMsg(IPCEX_ID_GUI, data) != QUEUE_INSERT) {
				opearationsw_ReturnThePreviousPop();
			}
			break;
		default:
			break;
		}
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General Enable operation switch			                                            */
/*                                                                                      */
/* Details  -Enable opsw after 2.5s from STANDY -> PATIENT   							*/
/*													*/
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
/* local valiables   : isDisableOpsw			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void enable_opsw(void)
{
	DEBUGOUT("enable_opsw \n");
	gs_isDisableOpsw = false;
	s_isAllowPush2S = true;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General Check time for enable operation switch						                */
/*                                                                                      */
/* Details  -Check disableCountTime > 2.5S ===> Enable Opsw   						*/
/*													*/
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
/* local valiables   : disableCountTime			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void check_time_for_enable_opsw(void)
{
	gs_disableCountTime++;
	if (gs_disableCountTime >= COUNT_2_5S)
	{
		gs_disableCountTime = COUNT_2_5S;
		if(gs_swPushCount == 0)
		{
			enable_opsw();
			gs_disableCountTime = 0;
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General： operationsw_config															*/
/*                                                                                      */
/* Details：  Init and cofigured GPIO port for operation switch							*/
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : None								                             	*/
/*                    					                                            	*/
/*                     								                                 	*/
/* local valiables   : None			                                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void operationsw_config(void) {
	//Operation switch
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, OPERATIONSW_PORT_NUM, OPERATIONSW_BIT_NUM);		/* input */
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General： operationsw_CheckState														*/
/*                                                                                      */
/* Details：  Check operation switch state and update gs_opswActiveEvent is 				*/
/* TOGGLE or TOGGLE2s																	*/
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : gs_opswActiveEvent								                             	*/
/*                    					                                            	*/
/*                     								                                 	*/
/* local valiables   : None			                                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
static void operationsw_CheckState(void) {
	if (gs_isDisableOpsw == false)
	{
		gs_currentState = Chip_GPIO_GetPinState(LPC_GPIO_PORT, OPERATIONSW_PORT_NUM, OPERATIONSW_BIT_NUM);
		if (gs_currentState == SW_PUSH) {
			if (gs_peviousState == SW_UNPUSH) {
#ifdef Q_DEBUG
				DEBUGOUT("Case1: %d \n",gs_currentState);
#endif
				gs_opswActiveEvent = TOGGLE;
				s_isOpswPush = true;
			} else if (gs_peviousState == SW_PUSH) {
#ifdef Q_DEBUG
				DEBUGOUT("Case2: %d \n",gs_swPushCount);
#endif
				if(gs_swPushCount != OPSW_CNT_WAIT_RELEASE)
				{
					gs_swPushCount++;
				}
				if (gs_swPushCount >= COUNT_2S)
				{
					gs_swPushCount = OPSW_CNT_WAIT_RELEASE;
					if(s_isAllowPush2S == true)
					{
						gs_opswActiveEvent = TOGGLE2S;
					}
				}
			}
		}
		else if (gs_currentState == SW_UNPUSH) {
			if ((gs_swPushCount < COUNT_2S) && (gs_swPushCount > 0))
			{
				gs_opswActiveEvent = TOGGLESHORT;
			}
			gs_swPushCount = 0;
		}
		gs_peviousState = gs_currentState;
	}
	else
	{
		gs_currentState = Chip_GPIO_GetPinState(LPC_GPIO_PORT, OPERATIONSW_PORT_NUM, OPERATIONSW_BIT_NUM);
		if (gs_currentState == SW_UNPUSH)
		{
			gs_swPushCount = 0;
		}
		check_time_for_enable_opsw();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General：  send operation switch msg to GUI task										*/
/*                                                                                      */
/* Details：  Check gs_opswActiveEvent and send opsw event message to GUI task				*/
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : gs_opswActiveEvent								               	*/
/*                    					                                            	*/
/*                     								                                 	*/
/* local valiables   : IPC_MSG_DATA_t data;		                                        */
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
static void operationsw_ProcessEvent(void)
{
	IPC_MSG_DATA_t data;
	data.data_id = eNoDeviceEventId;
	data.data_value = 0;
	memset(&data.stAlarmStatPacket, 0, sizeof(data.stAlarmStatPacket));
	memset(&data.stPsaParamPacket, 0, sizeof(data.stPsaParamPacket));
	memset(&data.stPsaMonitorPacket, 0, sizeof(data.stPsaMonitorPacket));
	memset(&data.version, 0, sizeof(data.version));

	switch (gs_opswActiveEvent) {
	case TOGGLE:
		data.data_id = eOperationswToggle;
		data.data_value = 0x00;
		if (ipc_PushMsg(IPCEX_ID_GUI, data) != QUEUE_INSERT) {
			operationsw_PushRequest(gs_opswActiveEvent);
		}
		gs_opswActiveEvent = IDLE;
		DEBUG_OPSW("1\n");
		break;
	case TOGGLE2S:
		data.data_id = eOperationswToggle2S;
		data.data_value = 0x00;
		if (ipc_PushMsg(IPCEX_ID_GUI, data) != QUEUE_INSERT) {
			operationsw_PushRequest(gs_opswActiveEvent);
		}
		gs_opswActiveEvent = IDLE;
		DEBUG_OPSW("2\n");
		break;
	case TOGGLESHORT:
		data.data_id = eOperationswToggleShort;
		data.data_value = 0x00;
		if (ipc_PushMsg(IPCEX_ID_GUI, data) != QUEUE_INSERT) {
			operationsw_PushRequest(gs_opswActiveEvent);
		}
		gs_opswActiveEvent = IDLE;
		DEBUG_OPSW("3\n");
		break;
	default:
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General：  Handle operation switch														*/
/*                                                                                      */
/* Details：  Call operationsw_CheckState and operationsw_ProcessEvent						*/
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : None								                             	*/
/*                    					                                            	*/
/*                     								                                 	*/
/* local valiables   : None			                                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void operationsw_Handle(void)
{
	operationsw_CheckState();
	operationsw_ProcessEvent();
	operationsw_ProcessPendingIpcMsg();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General Disable operation switch			                                            */
/*                                                                                      */
/* Details  -Disable operation switch 2.5S when STANDY -> PATIENT   					*/
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
void operationsw_disable(void)
{
	gs_swPushCount = 1; //set sw_push_count = 1 to disable sw until release when enter to PSA
	gs_disableCountTime = 0;
	gs_isDisableOpsw = true;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name operationsw_GetStatus		                                		    */
/*                                                                                      */
/* Details  -get status of operation swith is push or not		  						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool  -true: opsw push	  	                                            */
/*					   -false: opsw not push											*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) bool s_isOpswPush			                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool operationsw_GetStatus(void)
{
	bool opswStatus;
	opswStatus = s_isOpswPush;
	s_isOpswPush = false;
	return opswStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name operationsw_DisablePush2S		                                		*/
/*                                                                                      */
/* Details  -Disable operationsw push 2S						  						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE					  	                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) bool s_isAllowPush2S			                              	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void operationsw_DisablePush2S(void)
{
	s_isAllowPush2S = false;
	return;
}

