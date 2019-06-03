/********************************************************************/
/*                                                                  */
/* File Name     : IpcTask.c			                            */
/*                                                                  */
/* General       : This file contains function to create and handle */
/*					operation of ipc communicatio between M4 and M0 */
/*					core											*/
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
/*             2016/10/04      Quyen Ngo  			new file  	    */
/* #0001	   2017/08/25	   Quyen Ngo			Add task suspend*/
/*					block when access ipc_PushMsg function for 		*/
/*					mutual exclusion protection, change cycle 		*/
/*					IPC_TASK_DELAY_TIME_MS to 20ms for synchronize 	*/
/*					with M0 device task loop cycle					*/					
/*                                                                  */
/* #0002	   2017/09/12		Quyen Ngo			modify			*/
/*		-Change from eGotoBatteryStopMode to ePowerLostDetected		*/
/* #0003	   2017/10/06      Viet Le  			modify	        */
/*		-Edit modeMgr_WatchDogReset()-> guiInterface_WatchDogReset()*/
/*			(bug 1957)												*/
/* #0004	   2017/12/13	   Quyen Ngo(MV)		modify			*/
/*		Add void to non parameter function to follow coding rule	*/
/********************************************************************/


#include <stdlib.h>
#include "IpcMsg.h"
#include "IpcTask.h"
#include <TaskCommon.h>
#include "GuiTask.h"
#include "ModeMgr.h"
#include "GuiInterface.h"

#define IPC_TASK_DELAY_TIME_MS 				20
#define WDT_M0_TIMEOUT 						5000/IPC_TASK_DELAY_TIME_MS //5S
#define IPC_TASK_STACK_SIZE					256 * 4

#define MS_TO_NS(x)							1000*1000*x
#define WAIT_HAND_SHAKE_LOOP_TIME_NS	    400
#define HAND_SHAKE_WAIT_1MS					MS_TO_NS(1)/WAIT_HAND_SHAKE_LOOP_TIME_NS
#define M4_MAX_TIME_WAIT_HSHAKE_CNT		    10000*(HAND_SHAKE_WAIT_1MS)

#ifdef DEBUG
//#define Q_DEBUG
#endif

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
/* Array of IPC Event callback functions */
//static void(*ipcex_callback_lookup[IPCEX_MAX_IDS]) (uint32_t);
static void(*ipc_CallbackLookup[IPCEX_MAX_IDS]) (IPC_MSG_DATA_t);

/* Message QUEUE used by IPC library to exchange message between cores */
__BSS(RAM5) static ipcex_msg_t s_ipcQueue[IPCEX_QUEUE_SZ];

static uint16_t s_wdM0Cnt = 0;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: ipcex_HandleM0WDT
//
//    Detail: Handle M0 watchdog simulate
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
static void ipc_HandleM0WDT(void)
{
	s_wdM0Cnt++;
	if(s_wdM0Cnt >= WDT_M0_TIMEOUT)
	{
		DEBUGOUT("M0 reset \n");
		NVIC_SystemReset();
	}
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: ipcex_handleMsgSendToGuiTask
//
//    Detail: Handle Msg send from M0 core to GUI task
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
static void ipc_HandleMsgSendToGuiTask(IPC_MSG_DATA_t ipcMsgData)
{
	GUI_EVENT_STRUCT event;
	event.id = eIpcDataEventId;
	event.data.ipcData = ipcMsgData;

	guiInterface_SendGuiEvent(event);
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: ipc_DispatchTask
//
//    Detail: receive the message from the other core
//			   and will invoke the appropriate callback with the message
//
//    Arguments: (I) void *loop
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
static void ipc_DispatchTask(void *loop)
{
	int ret;
	ipcex_msg_t msg;
	do {
#ifndef DEBUG
		ipc_HandleM0WDT();
#endif
		ret = IPC_tryPopMsg(&msg);
		if ((ret == QUEUE_VALID) && (msg.id < IPCEX_MAX_IDS)) {
			if (ipc_CallbackLookup[msg.id]) {
				ipc_CallbackLookup[msg.id](msg.ipcMsgData);
#ifdef Q_DEBUG
				DEBUGOUT("M4_pop_DATA: %d \n",msg.ipcMsgData.data_id);
#endif
				s_wdM0Cnt = 0;//feed M0 watchdog
			}
		}
		vTaskDelay(IPC_TASK_DELAY_TIME_MS / portTICK_PERIOD_MS);
	} while (loop);
	return;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: ipc_RegisterCallback
//
//    Detail: IPC register callback function pointer
//
//    Arguments: (I) uint32_t id
//				 (I) void (*func)(IPC_MSG_DATA_t)
//				 (O)
//
//    Return Values: int
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
int ipc_RegisterCallback(uint32_t id, void (*func)(IPC_MSG_DATA_t))
{
	if (id >= IPCEX_MAX_IDS) {
		return 0;
	}

	ipc_CallbackLookup[id] = func;
	return 1;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: ipc_CreateTasks
//
//    Detail: Create RTOS task
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
void ipc_CreateTasks(void)
{
	/* Start ipc receive msg Task */
	xTaskCreate(ipc_DispatchTask, "IPC Ex",
			IPC_TASK_STACK_SIZE, (void *) 1, TASK_PRIO_IPC_DISPATCH,
			(TaskHandle_t *) NULL);
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: ipc_Init
//
//    Detail: Initialize the IPC queue
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)s_ipcQueue
//
//
//    Note: None
//
/******************************************************************************/
void ipc_Init(void)
{
	IPC_initMsgQueue(s_ipcQueue, sizeof(ipcex_msg_t), IPCEX_QUEUE_SZ);
	ipc_RegisterCallback(IPCEX_ID_GUI, ipc_HandleMsgSendToGuiTask);
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: ipc_PushMsg
//
//    Detail: Push data on the queue
//
//    Arguments: (I) uint32_t id
//				 (I) IPC_MSG_DATA_t data
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
int ipc_PushMsg(uint32_t id, IPC_MSG_DATA_t data)
{
	int8_t err = 0;
	ipcex_msg_t msg;
	msg.id = id;
	msg.ipcMsgData = data;
	vTaskSuspendAll();
	err = IPC_tryPushMsg(&msg);
	xTaskResumeAll();
	return err;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: ipcex_WaitForHandShake
//
//    Detail: Send message to M0 core and wait for a handshake message from M0 core
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
void ipc_WaitForHandShake(void)
{
	int ret;
	ipcex_msg_t msg;
	while(1)
	{
		static uint32_t s_handshakeTimeCnt = 0;
		s_handshakeTimeCnt++;
		if(s_handshakeTimeCnt >= M4_MAX_TIME_WAIT_HSHAKE_CNT)
		{
			s_handshakeTimeCnt = 0;
			guiInterface_WatchDogReset();
		}
		ret = IPC_tryPopMsg(&msg);
		if ((ret == QUEUE_VALID) && (msg.id < IPCEX_MAX_IDS)) {
			if(msg.id == IPCEX_ID_GUI)
			{
				DEBUGOUT("Handshake successfull \n");
				if(msg.ipcMsgData.data_id == ePowerLostDetected)
				{
					DEBUGOUT("power stop hand shake \n");
					guiInterface_HandlerPowerLost();
				}
				else
				{
					msg.id = IPCEX_ID_DEVICE;
					msg.ipcMsgData.data_id = eHandshakePowerNormalId;
					IPC_pushMsg(&msg);
				}
				return;
			}
		}
	}
	return;
}
/**
 * @}
 */
