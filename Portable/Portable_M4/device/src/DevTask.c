/********************************************************************/
/*                                                                  */
/* File Name    : DeviceTask.c		                                */
/*                                                                  */
/* General       : This module contain function for Create RTOS task*/
/*                 to control and monitor device operation		    */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*       December 22, 2016     Thao Ha(MV)  		new file        */
/*                                                                  */
/********************************************************************/
#include <AlarmMsgHandler.h>
#include <AudioTask.h>
#include <DevCtrl.h>
#include <DevTask.h>
#include <EEPRom.h>
#include <FlowCtrl.h>
#include <Gpio.h>
#include <OpTime.h>
#include "Setting.h"
#include "task.h"
#include "TaskCommon.h"
#include "LogTask.h"
#include "Battery.h"
#include "AlarmTask.h"
#include "Compressor.h"

QueueHandle_t queue_deviceQueue;
QueueHandle_t queue_AlarmDataQueue;


//#define CHECK_REMAINING_STACK_SIZE

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devTask_Loop						              				    */
/*                                                                                      */
/* Details:        Devive task loop function is call function for handle device and		*/
/* 				   update monitor every 20ms						    				*/
/*                                                                                      */
/* Arguments:	void* loop																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void devTask_Loop(void* loop) {
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	DEV_EVENT_STRUCT deviceQueue;
	ALARM_STAT_T  deviceQueueForAlarm;
	bool isShutDown = false;
	Chip_WWDT_Start(LPC_WWDT);
	EEPROM_SetMode(eResetWatchdogTimer);
	uint32_t cnt = 0;

#ifdef CHECK_REMAINING_STACK_SIZE
	UBaseType_t uxHighWaterMark;
	uint32_t cnt = 0;
#endif
	while(1)
	{
		if(xQueueReceive(queue_deviceQueue, &deviceQueue, 0) == pdTRUE)
		{
			switch(deviceQueue.id)
			{
			case eFromUsbTask:
				devCtrl_ProcessUSBQueue(deviceQueue);
				break;
			case eFromLogTask:
				devCtrl_ProcessLogPacket(deviceQueue);
				break;
			case eFromAudioTask:
				devCtrl_ProcessAudioPacket(deviceQueue);
				break;
            case eFromAlarmTask:
            	devCtrl_ProcessAlarmPacket(deviceQueue);
                break;
			default:
				break;
			}
		}
		devCtrl_Handle();
		if(isShutDown==false)
		{
			devCtrl_UpdateMonitorInfo();
			devCtrl_UpdateAlarmMonitorStruct();
			if(xQueueReceive(queue_AlarmDataQueue, &deviceQueueForAlarm, 0))
			{
				alarmMsgHandler_HandleAlarm(deviceQueueForAlarm);
			}
			alarmMsgHandler_HandleKeyWhenAlarmOccur();
			alarmMsgHandler_BatteryInvalidHandle();
		}
#ifdef CHECK_REMAINING_STACK_SIZE
		uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
		if(cnt>=1000/DEVICE_TASK_DELAY_TIME)
		{
			DEBUGOUT("Device Task %d\n",uxHighWaterMark);
			cnt = 0;
		}
		else
		{
			cnt++;
		}
#endif

		if(cnt>=1000/DEVICE_TASK_DELAY_TIME)
		{
			cnt = 0;
		}
		else
		{
			cnt++;
		}

		vTaskDelayUntil( &xLastWakeTime, DEVICE_TASK_DELAY_TICK );
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devTask_Create						              				    */
/*                                                                                      */
/* Details:        Create FREERTOS task name devTask									*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void devTask_CreateTask(void) {
	xTaskCreate( devTask_Loop, "DEVICE task",
			DEVICE_TASK_SIZE, (void *)1, TASK_PRIO_DEVICE_TASK,
			( TaskHandle_t * ) NULL );
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devTask_Init						              				    */
/*                                                                                      */
/* Details:        Init device task by call devCtrl_Init function for init all devices  */
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void devTask_Init(void) {
	queue_deviceQueue = xQueueCreate(DEVICE_QUEUE_LENGTH, sizeof(DEV_EVENT_STRUCT));
	queue_AlarmDataQueue = xQueueCreate( ALARM_QUEUE_LENGTH, sizeof( struct ALARM_STAT_T * ) );// truong add
	devCtrl_Init();
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	SendToDeviceQueue					              				    */
/*                                                                                      */
/* Details:        Send data to Device Queue											*/
/*                                                                                      */
/* Arguments:	E_DEVICEQUEUE pDeviceQueueData								 		*/
/*              TickType_t xTicksToWait                                                 */
/*                                                                                      */
/* ReturnValue : true if it sent successfully                                           */
/*  			 false if it fails to sent                                              */
/*                                                                                      */
/****************************************************************************************/
bool devTask_SendToDeviceQueue(DEV_EVENT_STRUCT pDeviceQueueData,TickType_t xTicksToWait )
{
	bool status = false;
	DEV_EVENT_STRUCT pDeviceQueueDataSend = pDeviceQueueData;
	//send Data to Device Queue
	if(!xQueueSend(queue_deviceQueue, &pDeviceQueueDataSend, xTicksToWait)){
		//return false if it fails to sent
		status = false;
	}else{
		//return true if it sent successfully
		status = true;
	}

	return status;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmCtrl_SendToAlarmQueue							              	*/
/*                                                                                      */
/* Details: send data to alarm queue													*/
/*                                                                                      */
/* Arguments:	ALARM_STAT_T alarmQueueData, TickType_t xTicksToWait					*/
/*                                                                                      */
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/****************************************************************************************/
bool devTask_SendToAlarmQueue(ALARM_STAT_T alarmQueue, TickType_t xTicksToWait )
{
	bool status = false;
	ALARM_STAT_T alarmQueueSend = alarmQueue;
	if(!xQueueSend(queue_AlarmDataQueue, &alarmQueueSend, xTicksToWait)){
	  //return false if it fails to sent
	 	 status = false;
	}else{
	  //return true if it sent successfully
	  status = true;
	}
	 return status;
}
