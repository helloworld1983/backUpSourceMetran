/********************************************************************/
/*                                                                  */
/* File Name    : DeviceController.h                                */
/*                                                                  */
/* General       : This module contain function for Create RTOS task
 * to control and monitor device operation							*/
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
/*             Jun 2, 2016     Quyen Ngo(MV)  		new file        */
/*                                                                  */
/*	#0001	   2017/09/08	   Quyen Ngo(MV)		modify			*/
/*		-Change to call power_SendPowerLostMsgToM4 instead of call	*/
/*		to power_EnterToBatteryStopMode								*/
/*	#0002	   2017/09/12	   Quyen Ngo(MV)		modify			*/
/*		-add send request to get version of flow controller after	*/
/*		reset(bug 1832)												*/
/*	#0003	   2017/09/28	   Quyen Ngo(MV)		modify			*/
/*		-remove stack high water mark calculation					*/
/*	#0004	   2017/11/02	   Quyen Ngo(MV)		modify			*/
/*		Handle the case IPC queue full when send power lost msg	by	*/
/*		send this msg until successful								*/
/*																	*/
/********************************************************************/
#include <string.h>

#include "DeviceTask.h"
#include "OperationswHandle.h"
#include "PwmController.h"
#include "PressureSensor.h"
#include "PsaControl.h"
#include "ValveControl.h"
#include "TemperatureSensor.h"
#include "ModeHandle.h"
#include "FlowController.h"
#include "DeviceController.h"
#include "DeviceInterface.h"
#include "Compressor.h"
#include <adc.h>
#include <AlarmLed.h>
#include <BatteryHandling.h>
#include "PowerMgr.h"
#include "../../encoder/inc/encoder.h"

//#define Q_DEBUG

//Device task loop function
static void devTask_Loop(void* loop);

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devTask_Loop						              				    */
/*                                                                                      */
/* Details:        Devive task loop function is call function for handle device and
 * 				   update monitor value						    						*/
/*                                                                                      */
/* Arguments:	void* loop
 *                                                                                      */
/* ReturnValue :void			                                                        */
/* Macros : 	DEVICE_TASK_DELAY_TICK													*/
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
static void devTask_Loop(void* loop) {
	TickType_t xLastWakeTime;

	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();
	if(power_CheckPowerWhenHandShake() == true)
	{
		devCtrl_UpdateMonitorInfo();
		devCtrl_SendFirstMonitorInfoToGui();
	}
	else
	{
		DEBUGOUT("Handshake Goto stop");
		while (!(power_SendPowerLostMsgToM4()));
	}
	compressor_Reset();
	flowCtrl_Reset();
	flowCtrl_RequestSendVersion();
	while(1)
	{
		// Countinuous checking device status and handle operation
		devCtrl_UpdateMonitorInfo();
		devCtrl_Handle();
		devCtrl_UpdateAlarmMonitorStruct();
		devCtrl_SendMonitorInfoToGui();
		devInterface_HandleMsgQueueReceive();
		vTaskDelayUntil( &xLastWakeTime, DEVICE_TASK_DELAY_TICK );
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devTask_Create						              				    */
/*                                                                                      */
/* Details:        Create FREERTOS task name DEVICE										*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/* Local variables : None				                          						*/
/*                                                                                      */
/****************************************************************************************/
void devTask_Create(void) {
	xTaskCreate( devTask_Loop, "DEVICE",
			DEVICE_TASK_STACK, (void *)1, TASK_PRIO_DEVICE_TASK,
			( TaskHandle_t * ) NULL );
	return;
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
/* Local variables : None				                          						*/
/*                                                                                      */
/****************************************************************************************/
void devTask_Init(void) {
	devCtrl_Init();
	devInterface_Init();
	return;
}

