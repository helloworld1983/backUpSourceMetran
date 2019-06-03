/*
 * DeviceInterface.c
 *
 *  Created on: Dec 25, 2018
 *      Author: qsbk0
 */
#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "DeviceInterface.h"
#include "ipc/ipc_task.h"


#define DEVICE_QUEUE_WAIT_TIME 10
#define DEV_QUEUE_LENGTH 8

static QueueHandle_t s_devTskQueue;

bool devIf_sendMsg(IpcMessage* msg)
{
	if(xQueueSend(s_devTskQueue, msg,DEVICE_QUEUE_WAIT_TIME) == pdFALSE)
	{
		PRINTF("\n Failure send");
		return false;
	}
	return true;
}

void devIf_HandleMsgQueueRecv(void)
{
	static IpcMessage s_ipcMsgData;
	if(xQueueReceive(s_devTskQueue, &s_ipcMsgData, 0) == pdTRUE)
	{
		PRINTF("%s\n",s_ipcMsgData);
		ipc_sendMsg(&s_ipcMsgData);
	}
	return;
}

void devIf_Init(void)
{
	s_devTskQueue = xQueueCreate(DEV_QUEUE_LENGTH, sizeof(IpcMessage));
}
