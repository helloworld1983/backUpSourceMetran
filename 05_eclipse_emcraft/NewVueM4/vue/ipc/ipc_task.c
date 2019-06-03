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
#include "ipc_task.h"
#include "device/DeviceInterface.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define RPMSG_LITE_SHMEM_BASE (0x58000000U)
#define RPMSG_LITE_LINK_ID (RL_PLATFORM_IMX8MQ_M4_USER_LINK_ID)
#define RPMSG_LITE_NS_ANNOUNCE_STRING "rpmsg-virtual-tty-channel-1"
#define APP_TASK_STACK_SIZE (256)
#ifndef LOCAL_EPT_ADDR
#define LOCAL_EPT_ADDR (30)
#endif
#define IPC_QUEUE_LENGTH 8
#define IPC_QUEUE_WAIT_TIME 100

/* Globals */
static char app_buf[512]; /* Each RPMSG buffer can carry less than 512 payload */
static QueueHandle_t s_ipcTskQueue;

static volatile unsigned long remote_addr;
static struct rpmsg_lite_endpoint *volatile my_ept;
static volatile rpmsg_queue_handle my_queue;
static struct rpmsg_lite_instance *volatile my_rpmsg;
static void *rx_buf;
static int result;
static void *tx_buf;
static int len;
static unsigned long size;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
TaskHandle_t s_devTaskHandle = NULL;

static void ipc_nameservice_isr_cb(unsigned int new_ept, const char *new_ept_name, unsigned long flags, void *user_data)
{
}

static void ipc_WriteRpmsg(int8_t* buf, int32_t len)
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

static void ipc_SendMsg(IpcMessage* msg)
{
	int writtenBytes = 0;
	USHORT checkSum = 0;
	USHORT ipcSize = sizeof(IpcMessage);
	USHORT buffSize = ipcSize + 2 + 2;
	unsigned char data[buffSize]; //data to write
	data[0] = STX;

	//put ipc data to buffer
	memcpy(&data[1], &msg, ipcSize);

	//calculate check sum for ipc data
	for(USHORT i = 1; i <= ipcSize; i++)
	{
		checkSum += data[i];
	}

	data[ipcSize + 1] = HIGHBYTE(checkSum);
	data[ipcSize + 2] = LOWBYTE(checkSum);
	data[ipcSize + 3] = ETX;

	//used to find out how many bytes were written
	ipc_WriteRpmsg(data, buffSize);
}

static int32_t ipc_ReadRpmsg(uint8_t* buf, int len)
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

		if ((len == 2) && (app_buf[0] == 0xd) && (app_buf[1] == 0xa))
			PRINTF("Get New Line From Master Side\r\n");
		else
			PRINTF("Get Message From Master Side : \"%s\" [len : %d]\r\n", app_buf, len);
		memcpy(buf, rx_buf, len);
		/* Release held RPMsg rx buffer */
		result = rpmsg_queue_nocopy_free(my_rpmsg, rx_buf);
		PRINTF("result2 %d\n",result);
		if (result != 0)
		{
			assert(false);
		}
		return len;
	}
	return 0;
}

static IpcMessage ipc_ReceiveMasterMsg(void)
{
	IpcMessage recv;
	USHORT size = sizeof(IpcMessage) + 2 + 2; // 2 byte header  + 2 byte check sum
	unsigned char buff[size];
	int n = ipc_ReadRpmsg(buff, size);
	if(n > 0)
	{
		for(int i =0; i< n; i++)
		{
			PRINTF("%d\n",buff[i]);
		}
		memcpy(&recv, buff, sizeof(IpcMessage));
		devIf_sendMsg(&recv);
	}
	//	USHORT size = sizeof(IpcMessage) + 2 + 2; // 2 byte header  + 2 byte check sum
	//	unsigned char buff[size];
	//	unsigned char tmp[size];
	//	const unsigned char* result = tmp;
	//	bool isDone = false;
	//	USHORT checkSum = 0;
	//
	//	while (!isDone)
	//	{
	//		int n = ipc_ReadRpmsg(buff, size);
	//
	//		USHORT i = 0;
	//		for (i = 0; i < size; i++) {
	//			if (buff[i] == STX)
	//				break;
	//		}
	//
	//		if (i == 0) {
	//			memcpy(tmp, buff, size);
	//			break;
	//		}
	//
	//		if (i != size) {
	//			memcpy(tmp, &buff[i], size - i);
	//			tmp += size - i;
	//			size = i;
	//			continue;
	//		}
	//
	//		if (n < size) {
	//			//short read
	//			tmp += n;
	//			size -= n;
	//			continue;
	//		}
	//		//successful read of a whole message
	//		memcpy(tmp, buff, size);
	//		//check sum is correct or not
	//		for(USHORT i = 1; i <= sizeof(IpcMessage); i++)
	//			checkSum += result[i];
	//
	//		if((LOWBYTE(checkSum) == result[sizeof(IpcMessage)+2])
	//				&& (HIGHBYTE(checkSum) == result[sizeof(IpcMessage)+1]))
	//			isDone = true;
	//	}
	//
	//	memcpy(&recv, &result[1], sizeof(IpcMessage));
	return recv;
}

static void ipc_HandleMsgQueueRecv(void)
{
	static IpcMessage s_ipcMsgData;
	if(xQueueReceive(s_ipcTskQueue, &s_ipcMsgData, 0) == pdTRUE)
	{
		ipc_SendMsg(&s_ipcMsgData);
	}
	return;
}

static void ipc_task(void *param)
{
	/* Print the initial banner */
//	PRINTF("\r\nRPMSG String Echo FreeRTOS RTOS API Demo...\r\n");

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
		;

	my_queue = rpmsg_queue_create(my_rpmsg);
	my_ept = rpmsg_lite_create_ept(my_rpmsg, LOCAL_EPT_ADDR, rpmsg_queue_rx_cb, my_queue);
	rpmsg_ns_bind(my_rpmsg, ipc_nameservice_isr_cb, NULL);
	rpmsg_ns_announce(my_rpmsg, my_ept, RPMSG_LITE_NS_ANNOUNCE_STRING, RL_NS_CREATE);

	PRINTF("\r\nNameservice sent, ready for incoming messages...\r\n");

	for (;;)
	{
		ipc_ReceiveMasterMsg();
		ipc_HandleMsgQueueRecv();
		//		PRINTF("ipc tsk\n");
		//		ipc_ReadRpmsg();
		//		ipc_WriteRpmsg("hello\n",6);
		vTaskDelay(2);
	}
	return;
}

bool ipc_sendMsg(IpcMessage* msg)
{
	if(xQueueSend(s_ipcTskQueue, msg,IPC_QUEUE_WAIT_TIME) == pdFALSE)
	{
		PRINTF("\n Failure send");
		return false;
	}
	return true;
}



/*!
 * @brief Main function
 */
void ipc_create(void)
{
	s_ipcTskQueue = xQueueCreate(IPC_QUEUE_LENGTH, sizeof(IpcMessage));
	if (xTaskCreate(ipc_task, "IPC_TASK", APP_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &s_devTaskHandle) != pdPASS)
	{
		PRINTF("\r\nFailed to create application task\r\n");
		while (1)
			;
	}
	return;
}
