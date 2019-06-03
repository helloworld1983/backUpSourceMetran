/*
 * DeviceTask.c
 *
 *  Created on: Dec 3, 2018
 *      Author: qsbk0
 */
#include <deviceTask/DeviceInterface.h>
#include <deviceTask/DeviceTask.h>
#include <ipcTask/IpcInterface.h>
#include "FreeRTOS.h"
#include "task.h"
#include "fsl_uart.h"
#include "fsl_debug_console.h"
#include "Delay.h"
#include "rpmsg_lite.h"
#include "device/DigitalIO.h"
///////////////////////////////////
#include "device/I2CSensor.h"
#include "device/RS485Sensor.h"
#include "device/ADCSensor.h"
#include "board/uart.h"
/////////////////////////////
#define DEVICE_TASK_STACK_SIZE 512
#define WAIT_TIME_BETWEEN_PING 1000
static TaskHandle_t s_devTaskHandle = NULL;
/**
 * This function runs every 2ms to handle device task
 * @param param
 */
static void devTsk_loop(void *param)
{
	devIf_InitDevice();
	static uint32_t s_cnt = 0;
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	static bool s_state = false;
	for (;;)
	{
		uint8_t testData[4] = {'a', 'b', 'c', 'd'};
		PRINTF("Test uart\n");
        uart_Write(TEST_UART, (uint8_t *)testData, sizeof(testData));

		DigitalIO_SetState(eExhPressureZero,eOnSolenoidValve);
		devIf_HandleMsgQueueRecv();
		if(devIf_IsA53AppReady() && devIf_IsA53OSReady())
		{
			//						devIf_UpdateCurrentSensorValue();
			RealTimeM4ToA53* ptrSendMsg = devIf_UpdateMonitorStruct();
			//						devIf_Run(ptrSendMsg);
			ipcIf_sendRealTimeMsg(ptrSendMsg);
		}
		else
		{
			if(s_cnt == 0)
			{
				devIf_sendReadyMessage();
				//				PRINTF("Test Air Flow: %d \n",I2CSensorO2Flow_GetCurrentReading());
				//				PRINTF("Test Proxy Flow: %d\n",RS485ProxySensor_GetCurrentReading());
				//				PRINTF("Test Temperature %f \n",ADCSensorTemp_GetCurrentVolt());

			}
			s_cnt++;
			if(s_cnt>WAIT_TIME_BETWEEN_PING/DEVICE_TASK_DELAY_TIME)
				s_cnt=0;
		}
		DigitalIO_SetState(eExhPressureZero,eOffSolenoidValve);
		vTaskDelayUntil( &xLastWakeTime, DEVICE_TASK_DELAY_TIME/portTICK_PERIOD_MS );
	}
	return;
}

/*!
 * @brief ipc_create function
 */
void devTsk_Create(void)
{
	devIf_CreateQueue();
	if (xTaskCreate(devTsk_loop, "DEV_TASK", DEVICE_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &s_devTaskHandle) != pdPASS)
	{
		PRINTF("\r\nFailed to create application task\r\n");
		while (1);
	}
	return;
}
