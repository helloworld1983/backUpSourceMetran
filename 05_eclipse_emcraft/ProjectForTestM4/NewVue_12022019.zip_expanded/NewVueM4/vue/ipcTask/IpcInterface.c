/*
 * The Clear BSD License
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <board/board.h>
#include <board/clock_config.h>
#include <board/pin_mux.h>
#include <deviceTask/DeviceInterface.h>
#include <ipcTask/IpcTask.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rpmsg_lite.h"
#include "rpmsg_queue.h"
#include "rpmsg_ns.h"
#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "fsl_uart.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define RPMSG_LITE_SHMEM_BASE (0x58000000U)
#define RPMSG_LITE_LINK_ID (RL_PLATFORM_IMX8MQ_M4_USER_LINK_ID)
#define RPMSG_LITE_NS_ANNOUNCE_STRING "rpmsg-virtual-tty-channel-1"
#ifndef LOCAL_EPT_ADDR
#define LOCAL_EPT_ADDR (30)
#endif
#define IPC_RT_QUEUE_LENGTH 2
#define IPC_CM_QUEUE_LENGTH 12
#define IPC_QUEUE_WAIT_TIME 0
#define min(a,b) \
		({ __typeof__ (a) _a = (a); \
		__typeof__ (b) _b = (b); \
		_a < _b ? _a : _b; })
typedef enum
{
	WaitSTX,
	WaitETX,
	CheckSum,
	Success
} StateMachine;
#define  BUFF_SIZE sizeof(IPCMessage) + 2 + 2 // 2 byte header  + 2 byte check sum
#define MSG_SIZE  sizeof(IPCMessage)
/* Globals */
static unsigned char gs_ReceiveBuff[BUFF_SIZE];
static unsigned int gs_RecvIndex=0;
static StateMachine gs_State=WaitSTX;
IPCMessage gs_ReceiveMsg;
static char app_buf[512]; /* Each RPMSG buffer can carry less than 512 payload */
static QueueHandle_t s_ipcTskRealTimeQueue;
static QueueHandle_t s_ipcTskCommandQueue;
static volatile unsigned long remote_addr;
static struct rpmsg_lite_endpoint *volatile my_ept;
static volatile rpmsg_queue_handle my_queue;
struct rpmsg_lite_instance *volatile my_rpmsg;
static void *rx_buf;
static int result;
static void *tx_buf;
static unsigned long size;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

static void ipcIf_nameservice_isr_cb(unsigned int new_ept, const char *new_ept_name, unsigned long flags, void *user_data)
{
}
/**
 * Write to IPC
 * @param buf Pointer to buffer
 * @param len Length of buffer
 */
static void ipcIf_WriteRpmsg(int8_t* buf, int32_t len)
{
	/* Get tx buffer from RPMsg */
	tx_buf = rpmsg_lite_alloc_tx_buffer(my_rpmsg, &size, RL_BLOCK);
	assert(tx_buf);
	/* Copy string to RPMsg tx buffer */
	memcpy(tx_buf, buf, len);
	/* Echo back received message with nocopy send */
	result = rpmsg_lite_send_nocopy(my_rpmsg, my_ept, remote_addr, tx_buf, len);
	if (result != 0)
	{
		assert(false);
	}
}
/**
 * Construct message according to protocol and
 * write to IPC
 * @param msg
 */
static void ipcIf_SendMsg(IPCMessage* msg)
{
	int writtenBytes = 0;
	USHORT checkSum = 0;
	USHORT ipcSize = sizeof(IPCMessage);
	USHORT buffSize = ipcSize + 2 + 2;
	unsigned char data[buffSize]; //data to write
	data[0] = STX;
	//put ipc data to buffer
	memcpy(&data[1], msg, ipcSize);
	//calculate check sum for ipc data
	for(USHORT i = 1; i <= ipcSize; i++)
	{
		checkSum += data[i];
	}
	data[ipcSize + 1] = HIGHBYTE(checkSum);
	data[ipcSize + 2] = LOWBYTE(checkSum);
	data[ipcSize + 3] = ETX;

	//used to find out how many bytes were written
	ipcIf_WriteRpmsg(data, buffSize);
}
/**
 * Read from IPC
 * @param buf Pointer to buffer
 * @param len Length of buffer
 * @return int len Number of byte read from IPC
 */
static int32_t ipcIf_ReadRpmsg(uint8_t* buf, int len)
{
	/* Get RPMsg rx buffer with message */
	result = rpmsg_queue_recv_nocopy(my_rpmsg, my_queue, (unsigned long *)&remote_addr, (char **)&rx_buf, &len,
			RL_DONT_BLOCK);
	//	PRINTF("result1 %d\n",result);
	//	if (result != 0)
	//	{
	//		assert(false);
	//	}
	if((result == 0) && (len > 0))
	{
		/* Copy string from RPMsg rx buffer */
		assert(len < sizeof(app_buf));
		memcpy(app_buf, rx_buf, len);
		app_buf[len] = 0; /* End string by '\0' */

		//		if ((len == 2) && (app_buf[0] == 0xd) && (app_buf[1] == 0xa))
		//			PRINTF("Get New Line From Master Side\r\n");
		//		else
		//			PRINTF("Get Message From Master Side : \"%s\" [len : %d]\r\n", app_buf, len);
		memcpy(buf, rx_buf, len);
		/* Release held RPMsg rx buffer */
		result = rpmsg_queue_nocopy_free(my_rpmsg, rx_buf);
		//		PRINTF("result2 %d\n",result);
		if (result != 0)
		{
			assert(false);
		}
		return len;
	}
	return 0;
}

static void ipcIf_ProcessStateSuccess()
{
	memcpy(&gs_ReceiveMsg, &gs_ReceiveBuff[1], MSG_SIZE);
	gs_State = WaitSTX;
	memset(gs_ReceiveBuff, 0, BUFF_SIZE);
	gs_RecvIndex = 0;
}
static void ipcIf_ProcessStateCheckSum()
{
	USHORT sum = 0;
	for(unsigned int i = 1; i < MSG_SIZE + 1; ++i)
	{
		sum += gs_ReceiveBuff[i];
	}

	if((LOWBYTE(sum) == gs_ReceiveBuff[MSG_SIZE + 2]) && (HIGHBYTE(sum) == gs_ReceiveBuff[MSG_SIZE + 1]))
	{
		gs_State = Success;
	}
	else
	{
		gs_State = WaitSTX;
	}
}
static void ipcIf_ProcessStateWaitETX()
{
	if(gs_ReceiveBuff[BUFF_SIZE - 1] == ETX)
	{
		//if reach ETX, change state to check sum state
		gs_State = CheckSum;
	}
	else if(gs_RecvIndex >= BUFF_SIZE)
	{
		//Receive buffer is full but can't find ETX.
		//This message is invalid, so that we ignore and reset state to WaitSTX.
		gs_State = WaitSTX;
	}
	else
	{
		UCHAR buff[BUFF_SIZE] = {0};
		unsigned int bytes = ipcIf_ReadRpmsg(buff, BUFF_SIZE);
		//return if read nothing
		if(bytes < 1)
			return;
		//copy data from buff to Receive buff
		gs_RecvIndex %= BUFF_SIZE;
		memcpy(&gs_ReceiveBuff[gs_RecvIndex], buff, min(bytes, BUFF_SIZE - gs_RecvIndex + 1));
		gs_RecvIndex = gs_RecvIndex + min(bytes, BUFF_SIZE - gs_RecvIndex + 1);
	}
}
static bool ipcIf_ProcessStateWaitSTX()
{
	UCHAR buff[BUFF_SIZE] = {0};
	int bytes = ipcIf_ReadRpmsg(buff, BUFF_SIZE);
	//ignore if read nothing
	if(bytes < 1)
		return false;

	//if receive full package at first time
	if((bytes == BUFF_SIZE) && (buff[0] == STX) && (buff[BUFF_SIZE - 1] == ETX))
	{
		memcpy(gs_ReceiveBuff, buff, BUFF_SIZE);
		gs_RecvIndex = BUFF_SIZE;
		ipcIf_ProcessStateCheckSum();
		if(gs_State == Success)
		{
			ipcIf_ProcessStateSuccess();
			return true;
		}
		else
			return false;

	}
	//process for short read
	for(int i = 0; i < bytes; ++i)
	{
		if(buff[i] == STX)
		{
			//if reach STX, copy data from buffer to Receive buffer
			//and change state to Wait ETX
			memcpy(gs_ReceiveBuff, &buff[i], bytes - i);
			gs_State = WaitETX;
			gs_RecvIndex = bytes - i;
			break;
		}
	}
	return false;
}
static bool ipcIf_Receive()
{
	switch (gs_State)
	{
	case WaitSTX:
		return ipcIf_ProcessStateWaitSTX();
	case WaitETX:
		ipcIf_ProcessStateWaitETX();
		break;
	case CheckSum:
		ipcIf_ProcessStateCheckSum();
		break;
	case Success:
		ipcIf_ProcessStateSuccess();
		return true;
	}

	return false;
}
/**
 * Get message from IPC and send to device task
 */
void ipcIf_ReceiveMasterMsg(void)
{
	bool status = ipcIf_Receive();
	while(status)
	{
		if(status)
		{
			if(gs_ReceiveMsg.A53ToM4.nCmd==0)
			{
				devIf_SendSettingMsg(&gs_ReceiveMsg.A53ToM4.data.setting);
			}
			else
			{
				for(int i = 0;i<gs_ReceiveMsg.A53ToM4.nCmd;i++)
				{
					devIf_SendCommandMsg(&gs_ReceiveMsg.A53ToM4.data.command[i]);
				}
			}

		}
		status = ipcIf_Receive();
	}
}
/**
 * Get message from ipc queue (command and real time message)
 *  and send to A53 application via IPC
 */
void ipcIf_HandleMsgQueueRecv(void)
{
	IPCMessage	msgM4ToA53;
	RealTimeM4ToA53 ipcMsgRealTimeData;
	Command cmd;
	int cmdCnt = 0;
	msgM4ToA53.M4ToA53.isUpdateRealTime = false;
	if(xQueueReceive(s_ipcTskRealTimeQueue, &ipcMsgRealTimeData, 0) == pdTRUE)
	{
		msgM4ToA53.M4ToA53.isUpdateRealTime = true;
		msgM4ToA53.M4ToA53.realTime = ipcMsgRealTimeData;
	}

	while(xQueueReceive(s_ipcTskCommandQueue, &cmd,0) == pdTRUE)
	{
		msgM4ToA53.M4ToA53.command[cmdCnt]=cmd;
		cmdCnt++;
		if(cmdCnt>=MAX_CMD_M4_TO_A53)
			break;
	}
	msgM4ToA53.M4ToA53.nOfCmd = cmdCnt;
	if(cmdCnt>0 || msgM4ToA53.M4ToA53.isUpdateRealTime==true)
	{
		ipcIf_SendMsg(&msgM4ToA53);
	}
	return;
}
/**
 * Send message to device task to let it know A53 OS is ready
 */
static void ipcIf_sendA53OSReadyMessage()
{
	Command A53OSReadyMessage;
	A53OSReadyMessage.id = A53OSReady;
	devIf_SendCommandMsg(&A53OSReadyMessage);
}
/*
 * Initialize IPC task by waiting for A53 OS to be ready
 * and send A53 OS ready message to device task after that
 */
void ipcIf_Init()
{
	/* Print the initial banner */
	PRINTF("\r\nRPMSG String Echo FreeRTOS RTOS API Demo...\r\n");

#ifdef MCMGR_USED
	uint32_t startupData;

	/* Get the startup data */
	MCMGR_GetStartupData(kMCMGR_Core1, &startupData);

	my_rpmsg = rpmsg_lite_remote_init((void *)startupData, RPMSG_LITE_LINK_ID, RL_NO_FLAGS);

	/* Signal the other core we are ready */
	MCMGR_SignalReady(kMCMGR_Core1);
#else
	my_rpmsg = rpmsg_lite_remote_init((void *)RPMSG_LITE_SHMEM_BASE, RPMSG_LITE_LINK_ID, RL_NO_FLAGS);
#endif /* MCMGR_USED */

	while (!rpmsg_lite_is_link_up(my_rpmsg))
	{
		PRINTF("Wait for A53 OS\n");
		vTaskDelay(1000/portTICK_PERIOD_MS );
	}
	ipcIf_sendA53OSReadyMessage();

	my_queue = rpmsg_queue_create(my_rpmsg);
	my_ept = rpmsg_lite_create_ept(my_rpmsg, LOCAL_EPT_ADDR, rpmsg_queue_rx_cb, my_queue);
	rpmsg_ns_bind(my_rpmsg, ipcIf_nameservice_isr_cb, NULL);
	rpmsg_ns_announce(my_rpmsg, my_ept, RPMSG_LITE_NS_ANNOUNCE_STRING, RL_NS_CREATE);

	PRINTF("\r\nNameservice sent, ready for incoming messages...\r\n");
}
/*
 * Send real time message to IPC queue
 */
bool ipcIf_sendRealTimeMsg(RealTimeM4ToA53* msg)
{
	if(xQueueSend(s_ipcTskRealTimeQueue, msg,IPC_QUEUE_WAIT_TIME) == pdFALSE)
	{
		//		PRINTF("\n ipc_sendMsg Failure send");
		return false;
	}
	return true;
}
/*
 * Send command message to IPC queue
 */
bool ipcIf_sendCommandMsg(Command* msg)
{
	if(xQueueSend(s_ipcTskCommandQueue, msg,IPC_QUEUE_WAIT_TIME) == pdFALSE)
	{
		//		PRINTF("\n ipc_sendMsg Failure send");
		return false;
	}
	return true;
}
/**
 * Create IPC task's queues
 */
void ipcIf_createQueue()
{
	s_ipcTskRealTimeQueue = xQueueCreate(IPC_RT_QUEUE_LENGTH, sizeof(RealTimeM4ToA53));
	s_ipcTskCommandQueue = xQueueCreate(IPC_CM_QUEUE_LENGTH, sizeof(Command));
}



