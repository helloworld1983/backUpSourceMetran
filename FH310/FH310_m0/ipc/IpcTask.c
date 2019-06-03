/********************************************************************/
/*                                                                  */
/* File Name     : IpcTask.c			                            */
/*                                                                  */
/* General       : This file contains function to create and handle */
/*				   operation of ipc communication between M4 and M0 */
/*				   core												*/
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
/*	#0001	   2017/08/25	   Quyen Ngo			Add task suspend*/
/*					block when acess ipc_PushMsg fucntion for		*/
/*					mutual exclusion protection						*/
/*                                                                  */
/*	#0002	   2017/09/08	   Quyen Ngo		modify				*/
/*		-Change to call power_SendPowerLostMsgToM4 instead of call	*/
/*		to power_EnterToBatteryStopMode								*/
/*	#0003	   2017/09/28	   Quyen Ngo(MV)		modify			*/
/*		-remove stack high water mark calculation					*/
/*	#0004	   2017/11/02	   Quyen Ngo(MV)		modify			*/
/*		-resend power lost msg when Ipc handshake until sucessful   */
/*		(Bug 2007)													*/
/********************************************************************/


#include <stdlib.h>

#include <IpcTask.h>
#include "DeviceController.h"
#include "DeviceInterface.h"
#include "TaskCommon.h"
#include "IpcMsg.h"
#include "PowerMgr.h"
#include "AlarmLed.h"

//#define Q_DEBUG
#define IPC_TASK_STACK_SIZE				2*256

static bool s_isM0ResetWdtTest = false;

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
/* Array of IPC Event callback functions */
//static void(*ipcex_callback_lookup[IPCEX_MAX_IDS]) (uint32_t);
static void(*ipc_CallbackLookup[IPCEX_MAX_IDS]) (IPC_MSG_DATA_t);

/* Message QUEUE used by IPC library to exchange message between cores */
static ipcex_msg_t s_ipcQueue[IPCEX_QUEUE_SZ];

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: ipc_HandleMsgSendToDeviceTask
//
//    Detail: Handle msg send from M4 core to device task
//
//    Arguments: (I) IPC_MSG_DATA_t ipcMsgData
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
static void ipc_HandleMsgSendToDeviceTask(IPC_MSG_DATA_t ipcMsgData)
{
	devInterface_SendMsg(ipcMsgData);
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
		ret = IPC_tryPopMsg(&msg);
		if ((ret == QUEUE_VALID) && (msg.id < IPCEX_MAX_IDS)) {
			if (ipc_CallbackLookup[msg.id]) {
				ipc_CallbackLookup[msg.id](msg.ipcMsgData);
#ifdef Q_DEBUG
				DEBUGOUT("M0_pop_DATA: %d, %d\n",msg.ipcMsgData.data_id,msg.ipcMsgData.data_value);
#endif
			}
		}
		vTaskDelay(IPC_TASK_DELAY_TICK);
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
	/* Start Blinky event Task */
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
	ipc_RegisterCallback(IPCEX_ID_DEVICE, ipc_HandleMsgSendToDeviceTask);
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
//						  (O)s_isM0ResetWdtTest
//
//
//    Note: None
//
/******************************************************************************/
int ipc_PushMsg(uint32_t id, IPC_MSG_DATA_t data)
{
	int8_t err = 0;
	if(s_isM0ResetWdtTest == false)
	{

		ipcex_msg_t msg;
		msg.id = id;
		msg.ipcMsgData = data;
		vTaskSuspendAll();
		err = IPC_tryPushMsg(&msg);
		xTaskResumeAll();
	}
	else
	{
		err = QUEUE_ERROR;
	}
	return err;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: ipcex_ResetM0WatdogTest
//
//    Detail: Trigger M0 watdog test
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)s_isM0ResetWdtTest
//
//
//    Note: None
//
/******************************************************************************/
void ipc_ResetM0WatdogTest(void)
{
	s_isM0ResetWdtTest = true;
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: ipcex_WaitForHandShake
//
//    Detail: Wait for a handshake message from M4 core and return it to M4 core
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
	if(power_CheckPowerWhenHandShake() == true)
	{
		msg.id = IPCEX_ID_GUI;
		msg.ipcMsgData.data_id = eHandshakePowerNormalId;
		IPC_pushMsg(&msg);
	}
	else
	{
		DEBUGOUT("Handshake Goto stop");
		while (!power_SendPowerLostMsgToM4());
	}
	while(1)
	{
		ret = IPC_tryPopMsg(&msg);
		if ((ret == QUEUE_VALID) && (msg.id < IPCEX_MAX_IDS)) {
			if(msg.id == IPCEX_ID_DEVICE)
			{
				DEBUGOUT("Handshake successfull %d \n",msg.ipcMsgData.data_id);
				return;
			}
		}
	}
	return;
}
/**
 * @}
 */
