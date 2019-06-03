/*!
 * @brief Main function
 */
#include "ipcTask/IpcTask.h"
#include "ipcTask/IpcInterface.h"
#include "FreeRTOS.h"
#include "task.h"
#include "fsl_debug_console.h"
#include "device/DigitalIO.h"
#define IPC_TASK_STACK_SIZE (512)
TaskHandle_t s_devTaskHandle = NULL;
/*
 * This function runs every 2ms to handle IPC task
 */
static void ipc_task(void *param)
{
	ipcIf_Init();
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	for (;;)
	{
		DigitalIO_SetState(ePurgeFlow,eOnSolenoidValve);
		ipcIf_ReceiveMasterMsg();
		ipcIf_HandleMsgQueueRecv();
		DigitalIO_SetState(ePurgeFlow,eOffSolenoidValve);
		vTaskDelayUntil( &xLastWakeTime, IPC_TASK_DELAY_TIME/portTICK_PERIOD_MS );
	}
	return;
}

void ipc_Create(void)
{
	ipcIf_createQueue();
	if (xTaskCreate(ipc_task, "IPC_TASK", IPC_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &s_devTaskHandle) != pdPASS)
	{
		PRINTF("\r\nFailed to create application task\r\n");
		while (1)
			;
	}
	return;
}
