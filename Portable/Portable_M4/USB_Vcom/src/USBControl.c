/********************************************************************/
/*                                                                  */
/* File Name    : USBControl.c                               		*/
/*                                                                  */
/* General      : Process different request from PC application     */
/*				  Send current system status to PC application		*/
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            April 18,2017     Thao Ha(MV)		  	    new file    */
/*                                                                  */
/********************************************************************/
#include <AudioMgr.h>
#include <DevCtrl.h>
#include <DevTask.h>
#include <DualCoreCommon.h>
#include <FlowCtrl.h>
#include <OpTime.h>
#include <PanelLed.h>
#include <PsaCtrl.h>
#include <RTC.h>
#include <TaskCommon.h>
#include <UIMgr.h>
#include <UpdateFlash.h>
#include <USBControl.h>
#include "board.h"
#include "cdc_vcom.h"
#include "Fan.h"
#include "Compressor.h"
#include "PressureSensor.h"
#include "string.h"
#include "stdlib.h"
#include "LogMgr.h"
#include "Setting.h"
#include "PressureSensor.h"
#include "UsbVcom.h"
#include "LogTask.h"
#include "USBTask.h"
#include "Gpio.h"

#define DEBUG_USB_EN
#ifdef DEBUG_USB_EN
#define DEBUG_USB(...) printf(__VA_ARGS__)
#else
#define DEBUG_USB(...)
#endif
enum
{
	ePortableSWVersion=2,
	eTotalOperationTimeContinuous=11,
	eTotalOperationTimeSynchronize=16,
	eOperationTimeFromPreviousMaintenanceContinuous=21,
	eOperationTimeFromPreviousMaintenanceSynchronize=26,
	eFlowControllerVersion=31,
	eCompressorVersion=38,
	eTriggerSetting=45,
	eVolumeSetting=46,
	eDate=47,
	eTime=55,
	eTriggerSensitivityOffset=61,
	eFlowrateOffset=65,
	eO2Offset=69,
	eDischargeVolumeOffset=73,
	ePortableBootloaderVersion =77,
	eSol1DelayOpenTimeValue = 86,
	eSol3DelayOpenTimeValue = 90,
	eTriggerSensitivityOffsetFanLow= 94,
	eTriggerSensitivityOffsetFanMid= 98,
	eTriggerSensitivityOffsetFanHigh= 102,
};

#define USER_PRO_ADDR_BOOT		0x1A000000

#define MESSAGE_ID_PCAPP 166
#define NUMBER_OF_RESEND 10

#define MONITOR_PACKET_SIZE 59
#define MONITOR_PACKET_TAG_SIZE 2
#define MONITOR_PACKET_TAG "A:"
#define MONITOR_DATA_SIZE 4

#define SYSTEM_INFO_PACKET_SIZE 107//95
#define SYSTEM_INFO_PACKET_TAG_SIZE 2
#define SYSTEM_INFO_PACKET_TAG "B:"

#define SYSTEM_INFO_PACKET_OPERATION_TIME_SIZE 27
#define SYSTEM_INFO_PACKET_OPERATION_TIME_TAG_SIZE 6
#define SYSTEM_INFO_PACKET_OPERATION_TIME_TAG "B:time"
#define SYSTEM_INFO_PACKET_OPERATION_TIME_DATA_SIZE 5


#define LOG_PACKET_SIZE 30
#define LOG_PACKET_TAG_SIZE 2
#define LOG_PACKET_TAG "C:"

#define LOG_NUMBER_PACKET_SIZE 11
#define LOG_NUMBER_PACKET_TAG_SIZE 6
#define LOG_NUMBER_PACKET_TAG "C:numb"

#define PRESSURE_VACUUM_LOG_PACKET_SIZE 24
#define PRESSURE_VACUUM_LOG_PACKET_TAG_SIZE 6
#define PRESSURE_VACUUM_LOG_PACKET_TAG "F:name"

#define PRESSURE_TANK_LOG_PACKET_SIZE 24
#define PRESSURE_TANK_LOG_PACKET_TAG_SIZE 6
#define PRESSURE_TANK_LOG_PACKET_TAG "D:name"

#define PRESSURE_TANK_PACKET_SIZE 50
#define PRESSURE_TANK_PACKET_TAG_SIZE 2
#define PRESSURE_TANK_PACKET_TAG "D:"

#define PRESSURE_SUPPLY_PACKET_SIZE 50
#define PRESSURE_SUPPLY_PACKET_TAG_SIZE 2
#define PRESSURE_SUPPLY_PACKET_TAG "G:"

#define PRESSURE_SUPPLY_LOG_PACKET_SIZE 24
#define PRESSURE_SUPPLY_LOG_PACKET_TAG_SIZE 6
#define PRESSURE_SUPPLY_LOG_PACKET_TAG "G:name"

#define PRESSURE_VACUUM_PACKET_SIZE 65
#define PRESSURE_VACUUM_PACKET_TAG_SIZE 2
#define PRESSURE_VACUUM_PACKET_TAG "F:"

#define ALARM_PACKET_SIZE 5
#define ALARM_PACKET_TAG_SIZE 2
#define ALARM_PACKET_TAG "E:"

#define NEGATIVE_VALUE '-'
#define POSITIVE_VALUE '0'

#define DEFAULT_VALUE_OF_ACK_NUMBER 11
#define ACK_PACKET_SIZE 10

#define SYSTEM_BYTE 0
#define MESSAGE_BYTE 1
#define ACK_BYTE 2
#define TRIGGER_SENSITIVITY_BYTE 2
#define VOLUME_BYTE 2
#define FIRST_OPERATION_TIME_BYTE 2
#define SECOND_OPERATION_TIME_BYTE 3
#define THIRD_OPERATION_TIME_BYTE 4
#define TRIGGER_SENSITIVITY_OFFSET_BYTE 3
#define TRIGGER_SENSITIVITY_OFFSET_SIGN_BYTE 2
#define FLOWRATE_OFFSET_BYTE 3
#define FLOWRATE_OFFSET_SIGN_BYTE 2
#define O2_OFFSET_BYTE 3
#define O2_OFFSET_SIGN_BYTE 2
#define DISCHARGE_VOLUME_OFFSET_BYTE 3
#define DISCHARGE_VOLUME_SIGN_BYTE 2
#define AC_CURRENT_LIMIT_BYTE 3
#define AC_CURRENT_LIMIT_SIGN_BYTE 2
#define O2_ALARM_DETECTION_TIME_OFFSET_BYTE 2
#define PVSA_PRESSURE_BYTE 3
#define SOL1_ON_OFFSET_TIME_BYTE 3
#define SOL3_ON_OFFSET_TIME_BYTE 3
#define SOL2_OPEN_TIME_A_BYTE 3
#define SOL2_OPEN_TIME_B_BYTE 3
#define COOLING_FAN_BYTE 3
#define COMPRESSOR_L_BYTE 3
#define COMPRESSOR_H_BYTE 2
#define FC_OUTPUT_BYTE 3
#define MAX_PSA_TIME_BYTE 3
#define YEAR_H_BYTE 4
#define YEAR_L_BYTE 5
#define MONTH_BYTE 3
#define DAY_BYTE 2
#define HOUR_BYTE 6
#define MINUTE_BYTE 7
#define SECOND_BYTE 8
#define BPM_BYTE 2
#define ALARM_TEST_TYPE_BYTE 2
#define ALARM_TEST_VALUE_H_BYTE 3
#define ALARM_TEST_VALUE_L_BYTE 4
#define PRESSURE_SCALE 10
#define SYSTEM_INFO_PACKET_TOTAL_OPERATION_TIME_SIZE 5
#define SYSTEM_INFO_PACKET_FLOW_CONTROLLER_VERSION_SIZE 7
#define SYSTEM_INFO_PACKET_COMPRESSOR_VERSION_SIZE 7
#define SYSTEM_INFO_PACKET_TRIGGER_SETTING_SIZE 1
#define SYSTEM_INFO_PACKET_VOLUME_SETTING_SIZE 1
#define SYSTEM_INFO_PACKET_DATE_SIZE 8
#define SYSTEM_INFO_PACKET_TIME_SIZE 6
#define SYSTEM_INFO_PACKET_PORTABLE_VERSION_SIZE 9
#define SYSTEM_INFO_PACKET_OFFSET_SIZE 4
#define SYSTEM_INFO_PACKET_BOOTLOADER_VERSION_SIZE 9
#define SYSTEM_INFO_PACKET_SOL_DELAY_OPEN_TIME_SIZE 4
#define SYSTEM_INFO_PACKET_OFFSET_SIZE 4

#define LOG_PACKET_NO_TAG_START_BYTE 0
#define LOG_PACKET_YEAR_START_BYTE 2
#define LOG_PACKET_MONTH_START_BYTE 6
#define LOG_PACKET_DAY_START_BYTE 8
#define LOG_PACKET_HOUR_START_BYTE 10
#define LOG_PACKET_MINUTE_START_BYTE 12
#define LOG_PACKET_SECOND_START_BYTE 14
#define LOG_PACKET_LOG_CODE_START_BYTE 16
#define LOG_PACKET_EVENT_OR_ALARM_START_BYTE 20
#define LOG_PACKET_OLD_MODE_START_BYTE 21
#define LOG_PACKET_OLD_FLOWRATE_START_BYTE 22
#define LOG_PACKET_OLD_TRIGGER_SENSITIVITY_START_BYTE 23
#define LOG_PACKET_NEW_MODE_START_BYTE 24
#define LOG_PACKET_NEW_FLOWRATE_START_BYTE 25
#define LOG_PACKET_NEW_TRIGGER_SENSITIVITY_START_BYTE 26

#define PRESSURE_LOG_PACKET_YEAR_START_BYTE 0
#define PRESSURE_LOG_PACKET_MONTH_START_BYTE 4
#define PRESSURE_LOG_PACKET_DAY_START_BYTE 6
#define PRESSURE_LOG_PACKET_HOUR_START_BYTE 8
#define PRESSURE_LOG_PACKET_MINUTE_START_BYTE 10
#define PRESSURE_LOG_PACKET_SECOND_START_BYTE 12
#define PRESSURE_LOG_PACKET_LOG_CODE_START_BYTE 14
#define NUMBER_OF_PRESSURE_PER_SEND 15
#define NUMBER_OF_SEND 300/NUMBER_OF_PRESSURE_PER_SEND
#define NUMBER_OF_BYTE_PER_PRESS 3
#define PRESSURE_LOG_PACKET_NO_TAG_SIZE 2
#define NUMBER_OF_BYTE_PER_VACUUM_PRESS 4

#define CONNECTED_CONSTANT_1 0
#define CONNECTED_CONSTANT_2 257

#define SEND_MONITOR_DATA_TO_PC_APP_LOOP_CNT 60/USB_TASK_DELAY_TIME

#define MAX_WAITING_CNT 2
#define NUMBER_H_LEFT_SHIFT_BIT 16
#define NUMBER_L_LEFT_SHIFT_BIT 8
#define CONVERT_CONSTANT 256
#define CHANGE_TO_NEGATIVE_CONST -1
#define TIME_OFFSET_CONST 100


#define START_MONTH_BIT 4
#define START_MDAY_BIT 6
#define START_MINUTE_BIT 2
#define START_SEC_BIT 4

#define VACUUM_PRESSURE_SIGN_BYTE 0
#define SEND_PRESSURE_LOG_TIME_OUT 9000/USB_TASK_DELAY_TIME
#define MAXIMUM_NUMBER_OF_RECEIVE_BYTE 10
#define ONE_SECOND_WAIT_FOR_RTC_SET_TIME	1000/USB_TASK_DELAY_TIME

enum
{
	eTankPress,
	eVacuumPress,
	eFlowrate,
	eO2Concentration,
	eFCTemp,
	eFCPress,
	eMotorSpeed,
	eMotorBoardTemp,
	eMotorTemp,
	eCannulaPress,
	eProvidePress,
	eBatteryRemainCapacity,
	eIsInhale,
	eTriggerPressure
};
enum
{
	eExhale,
	eInhale
};

static bool gs_isSendSystemInfor = false;
static uint32_t gs_numberOfLogSend=0;
static bool gs_isWaitingPressureLog = false;
static bool gs_errorSendLogToPCApp = false;
static bool gs_isSendNumberOfLog = false;
static PCAPP_MONITOR_t gs_currentValue;
static uint32_t gs_CntOneSecondSetTime  = 0;

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USB_sendData					                               		*/
/*                                                                                      */
/* Details		: 	Send data to PC application and waiting for ACK message				*/
/*																						*/
/* Arguments:	(I) uint8_t* buff														*/
/*				(I) uint8_t size														*/
/*				(I) uint8_t ACKID														*/
/*				(I) uint16_t number														*/
/* ReturnValue : bool	                                                         		*/
/*                                                                                      */
/****************************************************************************************/
static bool USB_sendData(uint8_t* buff,uint8_t size,uint8_t ACKID,uint16_t number)
{
	uint8_t cnt = 0;
	uint8_t numberOfSend = 0;
	bool error = false;
	uint8_t receivedACK[ACK_PACKET_SIZE];
	vcom_write(buff,size);
	while(numberOfSend < NUMBER_OF_RESEND)
	{
		vTaskDelay(DELAY_TICK_5MS);
		memset(receivedACK, DEFAULT_VALUE_OF_ACK_NUMBER, ACK_PACKET_SIZE);
		vcom_bread(&receivedACK[0], ACK_PACKET_SIZE);
		if((receivedACK[SYSTEM_BYTE] == MESSAGE_ID_PCAPP)&&(receivedACK[MESSAGE_BYTE] == ACKID)&&(receivedACK[ACK_BYTE] == number))
		{
			return error;
		}
		if(cnt >= MAX_WAITING_CNT)
		{
			vcom_write(buff,size);
			cnt=0;
		}
		cnt++;
		numberOfSend++;
	}
	error = true;
	DEBUG_USB("Error send \n");
	return error;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USB_GetLogHandler				                               		*/
/*                                                                                      */
/* Details		: 	Handle get log request from PC application							*/
/*																						*/
/* Arguments:	(I) None																*/
/*				(O) None																*/
/*																						*/
/* ReturnValue : void	                                                         		*/
/*                                                                                      */
/****************************************************************************************/
static void USB_GetLogHandler()
{
	gs_errorSendLogToPCApp = false;
	gs_numberOfLogSend = gs_currentValue.numberOfLog;
	if(gs_numberOfLogSend>0)
	{
	    USBTask_ResetLogQueueSendForPCApp();
		E_logQueueReceive logQueue;
		logQueue.id = eRequestSendLogForPCApp;
		logQueue.data[0] = 0;
		if(!logTask_SendToLogQueueReceive(logQueue, 10)){
			DEBUG_TASK("Failed to send Item to logQueueRecive from USB_GetLogHandler");
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USB_MessageHandle				                               		*/
/*                                                                                      */
/* Details		: 	Process request sent from PC application							*/
/*																						*/
/* Arguments:	(I) uint8_t* buffer														*/
/*				(O) None																*/
/*																						*/
/* ReturnValue : void	                                                         		*/
/*                                                                                      */
/****************************************************************************************/
static void USB_MessageHandle(uint8_t* buffer)
{
	Real_Time_Clock datetime;
	uint32_t firstByte;
	uint32_t SecondByte;
	uint32_t ThirdByte;
	uint32_t operationTime;
	E_logQueueReceive logQueue;
	for(int i = 0; i < 20; i++)
		logQueue.data[i] = 0;
	DEV_EVENT_STRUCT deviceQueue;
	deviceQueue.id = eFromUsbTask;
	if(buffer[SYSTEM_BYTE]==MESSAGE_ID_PCAPP)
	{
		switch(buffer[MESSAGE_BYTE])
		{
		case eMessageIDGetLog:
			USB_GetLogHandler();
			break;

		case eMessageIDSetTriggerSensitivity:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetTriggerSensitivity;
			deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_BYTE];

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			DEBUG_USB("Trigger Sensitivity Set %d\n",buffer[TRIGGER_SENSITIVITY_BYTE]);
			break;

		case eMessageIDSetVolume:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetVolume;
			deviceQueue.msgData.usbMsg.requestData = buffer[VOLUME_BYTE];

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			DEBUG_USB("Sound Volume Set %d\n",buffer[VOLUME_BYTE]);
			break;

		case eMessageIDSetOperationTimeFromPreviousMaintenanceContinuousReset:
			firstByte = buffer[FIRST_OPERATION_TIME_BYTE];
			SecondByte = buffer[SECOND_OPERATION_TIME_BYTE];
			ThirdByte = buffer[THIRD_OPERATION_TIME_BYTE];
			operationTime = (firstByte<<NUMBER_H_LEFT_SHIFT_BIT)|(SecondByte<<NUMBER_L_LEFT_SHIFT_BIT)|(ThirdByte);
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetOperationTimeFromPreviousMaintenanceContinuousReset;
			deviceQueue.msgData.usbMsg.requestData = operationTime;

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			DEBUG_USB("Operation Time from previous maintenance continuous Set %d\n",operationTime);
			break;

		case eMessageIDSetOperationTimeFromPreviousMaintenanceSynchronizeReset:
			firstByte = buffer[FIRST_OPERATION_TIME_BYTE];
			SecondByte = buffer[SECOND_OPERATION_TIME_BYTE];
			ThirdByte = buffer[THIRD_OPERATION_TIME_BYTE];
			operationTime = (firstByte<<NUMBER_H_LEFT_SHIFT_BIT)|(SecondByte<<NUMBER_L_LEFT_SHIFT_BIT)|(ThirdByte);
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetOperationTimeFromPreviousMaintenanceSynchronizeReset;
			deviceQueue.msgData.usbMsg.requestData = operationTime;

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			DEBUG_USB("Operation Time from previous maintenance synchronize Set %d\n",operationTime);
			break;

		case eMessageIDClearHistory:
			logQueue.id = eRequestDeleteLog;

			if(!logTask_SendToLogQueueReceive(logQueue, 10)){
				DEBUG_TASK("Failed to send Item to logQueueRecive from USB_MessageHandle");
			}
			DEBUG_USB("Clear History\n");
			break;

		case eMessageIDClearPressureLog:
			logQueue.id = eRequestDeletePressureLog;

			if(!logTask_SendToLogQueueReceive(logQueue, 10)){
				DEBUG_TASK("Failed to send Item to logQueueRecive from USB_MessageHandle");
			}
			DEBUG_USB("Clear Pressure Log\n");
			break;

		case eMessageIDSetDateAndTime:
			if (gs_CntOneSecondSetTime>= ONE_SECOND_WAIT_FOR_RTC_SET_TIME)
			{
				datetime.year = buffer[YEAR_H_BYTE]*CONVERT_CONSTANT+buffer[YEAR_L_BYTE];
				datetime.month = buffer[MONTH_BYTE];
				datetime.mday = buffer[DAY_BYTE];
				datetime.wday = 0;
				datetime.hour = buffer[HOUR_BYTE];
				datetime.min = buffer[MINUTE_BYTE];
				datetime.sec = buffer[SECOND_BYTE];
				DEBUG_USB("Set Date and Time %d %d %d %d %d %d\n",datetime.mday,datetime.month,datetime.year,datetime.hour,datetime.min,datetime.sec);
				taskENTER_CRITICAL();
				realtimeclock_settime(&datetime);
				taskEXIT_CRITICAL();
				gs_CntOneSecondSetTime = 0;
			}
			break;

		case eMessageIDGetSystemInformation:
			gs_isSendSystemInfor = true;
			break;
		case eMessageIDGetPressureLog:
		    USBTask_ResetLogQueueSendForPCApp();
			logQueue.id = eRequestSendPressureLogForPCApp;

			if(!logTask_SendToLogQueueReceive(logQueue, 10)){
				DEBUG_TASK("Failed to send Item to logQueueRecive from USB_MessageHandle");
			}
			gs_isWaitingPressureLog = true;
			break;

		case eMessageIDTestAudioVolume:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDTestAudioVolume;
			deviceQueue.msgData.usbMsg.requestData = buffer[VOLUME_BYTE];

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			DEBUG_USB("Test Volume %d\n",buffer[VOLUME_BYTE]);
			break;

		case eMessageIDSetAllLEDOn:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetAllLEDOn;
			deviceQueue.msgData.usbMsg.requestData = 0;

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}

			DEBUG_USB("Set all LEDs on\n");
			break;

		case eMessageIDSetAllLEDOff:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetAllLEDOff;
			deviceQueue.msgData.usbMsg.requestData = 0;

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			DEBUG_USB("Set all LEDs off\n");
			break;

		case eMessageIDSetTotalOperationTimeContinuousReset:
			firstByte = buffer[FIRST_OPERATION_TIME_BYTE];
			SecondByte = buffer[SECOND_OPERATION_TIME_BYTE];
			ThirdByte = buffer[THIRD_OPERATION_TIME_BYTE];
			operationTime = (firstByte<<NUMBER_H_LEFT_SHIFT_BIT)|(SecondByte<<NUMBER_L_LEFT_SHIFT_BIT)|(ThirdByte);
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetTotalOperationTimeContinuousReset;
			deviceQueue.msgData.usbMsg.requestData = operationTime;
			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			DEBUG_USB("Total Operation Time continuous Set %d\n",operationTime);
			break;

		case eMessageIDSetTotalOperationTimeSynchronizeReset:
			firstByte = buffer[FIRST_OPERATION_TIME_BYTE];
			SecondByte = buffer[SECOND_OPERATION_TIME_BYTE];
			ThirdByte = buffer[THIRD_OPERATION_TIME_BYTE];
			operationTime = (firstByte<<NUMBER_H_LEFT_SHIFT_BIT)|(SecondByte<<NUMBER_L_LEFT_SHIFT_BIT)|(ThirdByte);
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetTotalOperationTimeSynchronizeReset;
			deviceQueue.msgData.usbMsg.requestData = operationTime;

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			DEBUG_USB("Total Operation Time synchronize Set %d\n",operationTime);
			break;

		case eMessageIDSetTriggerSensitivityOffset:
			deviceQueue.msgData.usbMsg.requestId  = eMessageIDSetTriggerSensitivityOffset;
			if(buffer[TRIGGER_SENSITIVITY_OFFSET_SIGN_BYTE]==NEGATIVE_VALUE)
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE]*CHANGE_TO_NEGATIVE_CONST;
			}
			else
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE];
			}

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			DEBUG_USB("Set Trigger Sensitivity Offset %d\n",buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE]);
			DEBUG_USB("Trigger Sensitivity Offset.......... %d\n",deviceQueue.msgData.usbMsg.requestData);
			break;

		case eMessageIDSetTriggerSensitivityOffsetFanLow:
			deviceQueue.msgData.usbMsg.requestId  = eMessageIDSetTriggerSensitivityOffsetFanLow;
			if(buffer[TRIGGER_SENSITIVITY_OFFSET_SIGN_BYTE]==NEGATIVE_VALUE)
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE]*CHANGE_TO_NEGATIVE_CONST;
			}
			else
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE];
			}

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			DEBUG_USB("Set Trigger Sensitivity Offset %d\n",buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE]);
//			DEBUGOUT("Trigger Sensitivity Offset FAN LOW.......... %d\n",deviceQueue.msgData.usbMsg.requestData);
			break;

		case eMessageIDSetTriggerSensitivityOffsetFanMid:
			deviceQueue.msgData.usbMsg.requestId  = eMessageIDSetTriggerSensitivityOffsetFanMid;
			if(buffer[TRIGGER_SENSITIVITY_OFFSET_SIGN_BYTE]==NEGATIVE_VALUE)
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE]*CHANGE_TO_NEGATIVE_CONST;
			}
			else
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE];
			}

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			DEBUG_USB("Set Trigger Sensitivity Offset %d\n",buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE]);
//			DEBUGOUT("Trigger Sensitivity Offset FAN MID.......... %d\n",deviceQueue.msgData.usbMsg.requestData);
			break;

		case eMessageIDSetTriggerSensitivityOffsetFanHigh:
			deviceQueue.msgData.usbMsg.requestId  = eMessageIDSetTriggerSensitivityOffsetFanHigh;
			if(buffer[TRIGGER_SENSITIVITY_OFFSET_SIGN_BYTE]==NEGATIVE_VALUE)
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE]*CHANGE_TO_NEGATIVE_CONST;
			}
			else
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE];
			}

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			DEBUG_USB("Set Trigger Sensitivity Offset %d\n",buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE]);
//			DEBUGOUT("Trigger Sensitivity Offset FAN HIGH.......... %d\n",deviceQueue.msgData.usbMsg.requestData);
			break;

		case eMessageIDSetFlowrateOffset:
			deviceQueue.msgData.usbMsg.requestId  = eMessageIDSetFlowrateOffset;
			if(buffer[FLOWRATE_OFFSET_SIGN_BYTE] == NEGATIVE_VALUE)
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[FLOWRATE_OFFSET_BYTE]*CHANGE_TO_NEGATIVE_CONST;
			}
			else
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[FLOWRATE_OFFSET_BYTE];
			}

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			DEBUG_USB("Set Flow rate Offset %d\n",buffer[FLOWRATE_OFFSET_BYTE]);

			break;
		case eMessageIDSetO2Offset:
			deviceQueue.msgData.usbMsg.requestId  = eMessageIDSetO2Offset;
			if(buffer[O2_OFFSET_SIGN_BYTE]==NEGATIVE_VALUE)
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[O2_OFFSET_BYTE]*CHANGE_TO_NEGATIVE_CONST;
			}
			else
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[O2_OFFSET_BYTE];
			}

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			DEBUG_USB("Set O2 Offset %d\n",buffer[O2_OFFSET_BYTE]);
			break;
		case eMessageIDSetDischargeVolumeOffset:
			deviceQueue.msgData.usbMsg.requestId  = eMessageIDSetDischargeVolumeOffset;
			if(buffer[DISCHARGE_VOLUME_SIGN_BYTE]==NEGATIVE_VALUE)
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[DISCHARGE_VOLUME_OFFSET_BYTE]*CHANGE_TO_NEGATIVE_CONST;
			}
			else
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[DISCHARGE_VOLUME_OFFSET_BYTE];
			}

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			DEBUG_USB("Set Discharge Volume Offset %d\n",buffer[DISCHARGE_VOLUME_OFFSET_BYTE]);
			break;

		case eMessageIDSetPVSAPress:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetPVSAPress;
			deviceQueue.msgData.usbMsg.requestData = buffer[PVSA_PRESSURE_BYTE];

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			break;
		case eMessageIDSetSol1OnOffsetTime:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetSol1OnOffsetTime;
			deviceQueue.msgData.usbMsg.requestData = buffer[SOL1_ON_OFFSET_TIME_BYTE]*TIME_OFFSET_CONST;

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			break;
		case eMessageIDSetSol3OnOffsetTime:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetSol3OnOffsetTime;
			deviceQueue.msgData.usbMsg.requestData = buffer[SOL3_ON_OFFSET_TIME_BYTE]*TIME_OFFSET_CONST;

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			break;
		case eMessageIDSetSol2OpenTimeA:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetSol2OpenTimeA;
			deviceQueue.msgData.usbMsg.requestData = buffer[SOL2_OPEN_TIME_A_BYTE]*TIME_OFFSET_CONST;

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			break;
		case eMessageIDSetSol2OpenTimeB:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetSol2OpenTimeB;
			deviceQueue.msgData.usbMsg.requestData = buffer[SOL2_OPEN_TIME_B_BYTE]*TIME_OFFSET_CONST;

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			break;
		case eMessageIDSetSol1DelayOpenTime:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetSol1DelayOpenTime;
			deviceQueue.msgData.usbMsg.requestData = buffer[3]*TIME_OFFSET_CONST;

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			break;
		case eMessageIDSetSol3DelayOpenTime:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetSol3DelayOpenTime;
			deviceQueue.msgData.usbMsg.requestData = buffer[3]*TIME_OFFSET_CONST;

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			break;
		case eMessageIDSetCoolingFan:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetCoolingFan;
			deviceQueue.msgData.usbMsg.requestData = buffer[COOLING_FAN_BYTE];

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			break;
		case eMessageIDSetFCOutput:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetFCOutput;
			deviceQueue.msgData.usbMsg.requestData = buffer[FC_OUTPUT_BYTE];

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			break;
		case eMessageIDSetCompressor:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetCompressor;
			deviceQueue.msgData.usbMsg.requestData = buffer[COMPRESSOR_H_BYTE]*CONVERT_CONSTANT+buffer[COMPRESSOR_L_BYTE];

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			break;
		case eMessageIDSetMaxPSATime:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetMaxPSATime;
			deviceQueue.msgData.usbMsg.requestData = buffer[MAX_PSA_TIME_BYTE];

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			break;
		case eMessageIDClosePort:
			vcom_disconnected();
			break;
		case eMessageIDSetTestSynchronizeMode:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetTestSynchronizeMode;
			deviceQueue.msgData.usbMsg.requestData = buffer[BPM_BYTE];

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			break;
		case eMessageIDIncreaseFlowrate:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDIncreaseFlowrate;
			deviceQueue.msgData.usbMsg.requestData = 0;

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			break;
		case eMessageIDDecreaseFlowrate:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDDecreaseFlowrate;
			deviceQueue.msgData.usbMsg.requestData = 0;

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			break;
		case eMessageIDChangeMode:

			deviceQueue.msgData.usbMsg.requestId = eMessageIDChangeMode;
			deviceQueue.msgData.usbMsg.requestData = 0;

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			break;
		case eMessageIDTestAlarmValue:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDTestAlarmValue;
			deviceQueue.msgData.usbMsg.requestTestAlarmType =buffer[ALARM_TEST_TYPE_BYTE];
			deviceQueue.msgData.usbMsg.requestTestAlarmData =buffer[ALARM_TEST_VALUE_H_BYTE]*CONVERT_CONSTANT+buffer[ALARM_TEST_VALUE_L_BYTE];

			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			break;

		case eMessageIDUpgrade:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDUpgrade;
			deviceQueue.msgData.usbMsg.requestData = 1;
			if(!devTask_SendToDeviceQueue(deviceQueue,10)){
				DEBUG_TASK("Failed to send Item to deviceQueue from USB_MessageHandle");
			}
			DEBUG_USB("Upgrade Bootloader Set \n");
			break;;
		default:
			break;
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USB_sendSystemInformation		                               		*/
/*                                                                                      */
/* Details		: 	Send system information to PC Application							*/
/*																						*/
/* Arguments:	(I) None																*/
/*				(O) None																*/
/*																						*/
/* ReturnValue : void	                                                         		*/
/*                                                                                      */
/****************************************************************************************/
static void USB_sendSystemInformation()
{
	char buffer[9] = PORTABLE_SW_VERSION;
	uint8_t systemInfor[SYSTEM_INFO_PACKET_SIZE] = SYSTEM_INFO_PACKET_TAG;
	memcpy((char*)&systemInfor[ePortableSWVersion],buffer,SYSTEM_INFO_PACKET_PORTABLE_VERSION_SIZE);

	uint32_t totalOperationTimeContinuous = gs_currentValue.totalOperationTimeContinuous;
	sprintf (buffer,"%.5d",totalOperationTimeContinuous);
	memcpy((char*)&systemInfor[eTotalOperationTimeContinuous],buffer,SYSTEM_INFO_PACKET_TOTAL_OPERATION_TIME_SIZE);

	uint32_t totalOperationTimeSynchronize = gs_currentValue.totalOperationTimeSynchronize;
	sprintf (buffer,"%.5d",totalOperationTimeSynchronize);
	memcpy((char*)&systemInfor[eTotalOperationTimeSynchronize],buffer,SYSTEM_INFO_PACKET_TOTAL_OPERATION_TIME_SIZE);

	uint32_t operationTimeContinuous = gs_currentValue.operationTimeContinuous;
	sprintf (buffer,"%.5d",operationTimeContinuous);
	memcpy((char*)&systemInfor[eOperationTimeFromPreviousMaintenanceContinuous],buffer,SYSTEM_INFO_PACKET_TOTAL_OPERATION_TIME_SIZE);

	uint32_t operationTimeSynchronize = gs_currentValue.operationTimeSynchronize;
	sprintf (buffer,"%.5d",operationTimeSynchronize);
	memcpy((char*)&systemInfor[eOperationTimeFromPreviousMaintenanceSynchronize],buffer,SYSTEM_INFO_PACKET_TOTAL_OPERATION_TIME_SIZE);

	memcpy((char*)&systemInfor[eFlowControllerVersion],gs_currentValue.flowControllerVersion,SYSTEM_INFO_PACKET_FLOW_CONTROLLER_VERSION_SIZE);

	memcpy((char*)&systemInfor[eCompressorVersion],gs_currentValue.compressorVersion,SYSTEM_INFO_PACKET_COMPRESSOR_VERSION_SIZE);

	uint8_t triggerSetting = gs_currentValue.currentTriggerSensitivity;
	sprintf (buffer,"%.1d",triggerSetting);
	memcpy((char*)&systemInfor[eTriggerSetting],buffer,SYSTEM_INFO_PACKET_TRIGGER_SETTING_SIZE);
	uint8_t volumeSetting = gs_currentValue.currentAudioVolume;
	sprintf (buffer,"%.1d",volumeSetting);
	memcpy((char*)&systemInfor[eVolumeSetting],buffer,SYSTEM_INFO_PACKET_VOLUME_SETTING_SIZE);
	Real_Time_Clock clock;
	realtimeclock_gettime(&clock);
	sprintf (buffer,"%.4d",clock.year);
	sprintf (&buffer[START_MONTH_BIT],"%.2d",clock.month);
	sprintf(&buffer[START_MDAY_BIT],"%2d",clock.mday);
	memcpy((char*)&systemInfor[eDate],buffer,SYSTEM_INFO_PACKET_DATE_SIZE);
	sprintf (buffer,"%.2d",clock.hour);
	sprintf (&buffer[START_MINUTE_BIT],"%.2d",clock.min);
	sprintf(&buffer[START_SEC_BIT],"%2d",clock.sec);
	memcpy((char*)&systemInfor[eTime],buffer,SYSTEM_INFO_PACKET_TIME_SIZE);

	int triggerValue =(int)(gs_currentValue.triggerSensitivityOffset*100.0);
	if(triggerValue<0)
		sprintf (buffer,"%.3d",triggerValue);
	else
		sprintf (buffer,"%.4d",triggerValue);
	memcpy((char*)&systemInfor[eTriggerSensitivityOffset],buffer,SYSTEM_INFO_PACKET_OFFSET_SIZE);

	triggerValue =(int)(gs_currentValue.triggerSensitivityOffsetFanLow*100.0);
	if(triggerValue<0)
		sprintf (buffer,"%.3d",triggerValue);
	else
		sprintf (buffer,"%.4d",triggerValue);
	memcpy((char*)&systemInfor[eTriggerSensitivityOffsetFanLow],buffer,SYSTEM_INFO_PACKET_OFFSET_SIZE);

	triggerValue =(int)(gs_currentValue.triggerSensitivityOffsetFanMid*100.0);
	if(triggerValue<0)
		sprintf (buffer,"%.3d",triggerValue);
	else
		sprintf (buffer,"%.4d",triggerValue);
	memcpy((char*)&systemInfor[eTriggerSensitivityOffsetFanMid],buffer,SYSTEM_INFO_PACKET_OFFSET_SIZE);

	triggerValue =(int)(gs_currentValue.triggerSensitivityOffsetFanHigh*100.0);
	if(triggerValue<0)
		sprintf (buffer,"%.3d",triggerValue);
	else
		sprintf (buffer,"%.4d",triggerValue);
	memcpy((char*)&systemInfor[eTriggerSensitivityOffsetFanHigh],buffer,SYSTEM_INFO_PACKET_OFFSET_SIZE);


	triggerValue = (int)(gs_currentValue.flowrateOffset*100.0);
	if(triggerValue<0)
		sprintf (buffer,"%.3d",triggerValue);
	else
		sprintf (buffer,"%.4d",triggerValue);
	memcpy((char*)&systemInfor[eFlowrateOffset],buffer,SYSTEM_INFO_PACKET_OFFSET_SIZE);
	triggerValue = (int)(gs_currentValue.O2Offset*10.0);
	if(triggerValue<0)
		sprintf (buffer,"%.3d",triggerValue);
	else
		sprintf (buffer,"%.4d",triggerValue);
	memcpy((char*)&systemInfor[eO2Offset],buffer,SYSTEM_INFO_PACKET_OFFSET_SIZE);
	triggerValue = (int)(gs_currentValue.dischargeVolumeOffset);
	if(triggerValue<0)
		sprintf (buffer,"%.3d",triggerValue);
	else
		sprintf (buffer,"%.4d",triggerValue);
	memcpy((char*)&systemInfor[eDischargeVolumeOffset], buffer, SYSTEM_INFO_PACKET_OFFSET_SIZE);


	triggerValue = (int)(gs_currentValue.sol1DelayOpenTimeValue);
	if(triggerValue < 0)
		sprintf (buffer,"%.3d", triggerValue);
	else
		sprintf (buffer,"%.4d", triggerValue);
	memcpy((char*)&systemInfor[eSol1DelayOpenTimeValue], buffer, SYSTEM_INFO_PACKET_SOL_DELAY_OPEN_TIME_SIZE);

	triggerValue = (int)(gs_currentValue.sol3DelayOpenTimeValue);
	if(triggerValue<0)
		sprintf (buffer,"%.3d",triggerValue);
	else
		sprintf (buffer,"%.4d",triggerValue);
	memcpy((char*)&systemInfor[eSol3DelayOpenTimeValue], buffer, SYSTEM_INFO_PACKET_SOL_DELAY_OPEN_TIME_SIZE);

	uint8_t bootVersionL = 0;
	uint8_t bootVersionH = 0;
	char bufferBootVerion[9] = "00-00-000";
	update_getBootloaderVersion(USER_PRO_ADDR_BOOT, &bootVersionH, &bootVersionL);
	sprintf (buffer,"%.2d", bootVersionH);
	memcpy((char*)&bufferBootVerion[3], buffer, 2);
	sprintf (buffer,"%.3d", bootVersionL);
	memcpy((char*)&bufferBootVerion[6], buffer, 3);
	memcpy((char*)&systemInfor[ePortableBootloaderVersion], bufferBootVerion, SYSTEM_INFO_PACKET_BOOTLOADER_VERSION_SIZE);

	systemInfor[SYSTEM_INFO_PACKET_SIZE-1]='\0';
	USB_sendData(systemInfor,SYSTEM_INFO_PACKET_SIZE,eMessageIDGetSystemInformationACK,0);

	gs_isSendSystemInfor = false;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USB_sendMonitorData				                               		*/
/*                                                                                      */
/* Details		: 	Send data to display on monitor screen of PC applicaiton			*/
/*																						*/
/* Arguments:	(I) None																*/
/*				(O) None																*/
/*																						*/
/* ReturnValue : void	                                                         		*/
/*                                                                                      */
/****************************************************************************************/
static void USB_sendMonitorData()
{
	char buffer[4] = {'\0'};
	uint8_t sendData[MONITOR_PACKET_SIZE] = MONITOR_PACKET_TAG;

	float tankPress = gs_currentValue.tankPress;
	sprintf(buffer,"%.4f",tankPress);
	memcpy((char*)&sendData[MONITOR_PACKET_TAG_SIZE + eTankPress*MONITOR_DATA_SIZE], buffer, MONITOR_DATA_SIZE);

	float vacuumPress = gs_currentValue.vacuumPress*PRESSURE_SCALE;
	sprintf(buffer,"%.4f",vacuumPress);
	memcpy((char*)&sendData[MONITOR_PACKET_TAG_SIZE + eVacuumPress*MONITOR_DATA_SIZE], buffer, MONITOR_DATA_SIZE);

	uint16_t flowValue = gs_currentValue.flowrate ;
	sprintf(buffer,"%.4d",flowValue);
	memcpy((char*)&sendData[MONITOR_PACKET_TAG_SIZE + eFlowrate*MONITOR_DATA_SIZE], buffer, MONITOR_DATA_SIZE);

	uint16_t O2Value = gs_currentValue.o2Concentrator ;
	sprintf(buffer,"%.4d",O2Value);
	memcpy((char*)&sendData[MONITOR_PACKET_TAG_SIZE + eO2Concentration*MONITOR_DATA_SIZE], buffer, MONITOR_DATA_SIZE);

	uint16_t flowTemperature = gs_currentValue.flowTemp;
	sprintf(buffer,"%.4d",flowTemperature);
	memcpy((char*)&sendData[MONITOR_PACKET_TAG_SIZE + eFCTemp*MONITOR_DATA_SIZE], buffer, MONITOR_DATA_SIZE);

	uint16_t flowPressure = gs_currentValue.flowPress;
	sprintf(buffer,"%.4d",flowPressure);
	memcpy((char*)&sendData[MONITOR_PACKET_TAG_SIZE + eFCPress*MONITOR_DATA_SIZE], buffer, MONITOR_DATA_SIZE);

	uint16_t motorSpeed = gs_currentValue.motorSpeed;
	sprintf(buffer,"%.4d",motorSpeed);
	memcpy((char*)&sendData[MONITOR_PACKET_TAG_SIZE + eMotorSpeed*MONITOR_DATA_SIZE], buffer, MONITOR_DATA_SIZE);

	uint16_t motorBoardTemp = gs_currentValue.motorBoardTemp;
	sprintf(buffer,"%.4d",motorBoardTemp);
	memcpy((char*)&sendData[MONITOR_PACKET_TAG_SIZE + eMotorBoardTemp*MONITOR_DATA_SIZE], buffer, MONITOR_DATA_SIZE);

	uint16_t motorTemp = gs_currentValue.motorTemp;
	sprintf(buffer,"%.4d",motorTemp);
	memcpy((char*)&sendData[MONITOR_PACKET_TAG_SIZE + eMotorTemp*MONITOR_DATA_SIZE], buffer, MONITOR_DATA_SIZE);

	float cannualaPress = gs_currentValue.cannualaPress*PRESSURE_SCALE;
	sprintf(buffer,"%.4f",cannualaPress);
	memcpy((char*)&sendData[MONITOR_PACKET_TAG_SIZE + eCannulaPress*MONITOR_DATA_SIZE], buffer, MONITOR_DATA_SIZE);

	float providePress = gs_currentValue.providePress;
	sprintf(buffer,"%.4f",providePress);
	memcpy((char*)&sendData[MONITOR_PACKET_TAG_SIZE + eProvidePress*MONITOR_DATA_SIZE], buffer, MONITOR_DATA_SIZE);

	uint16_t remainCap = gs_currentValue.batteryRemainCap;
	sprintf(buffer,"%.4d",remainCap);
	memcpy((char*)&sendData[MONITOR_PACKET_TAG_SIZE + eBatteryRemainCapacity*MONITOR_DATA_SIZE], buffer, MONITOR_DATA_SIZE);

	bool isInhale = gs_currentValue.isInhale;
	if(gs_currentValue.currentMode==eSynchronizedMode)
	{
		if(isInhale == false)
		{
			sprintf(buffer,"%.4d",eExhale);
		}
		else
		{
			sprintf(buffer,"%.4d",eInhale);
		}
	}
	else
	{
		sprintf(buffer,"%.4d",eExhale);
	}
	memcpy((char*)&sendData[MONITOR_PACKET_TAG_SIZE + eIsInhale*MONITOR_DATA_SIZE], buffer, MONITOR_DATA_SIZE);
	float triggerPressure = gs_currentValue.triggerPressure*10;
	if(isInhale == true)
	{
		sprintf(buffer,"%.4f",triggerPressure);
		memcpy((char*)&sendData[MONITOR_PACKET_TAG_SIZE + eTriggerPressure*MONITOR_DATA_SIZE], buffer, MONITOR_DATA_SIZE);
	}
	else
	{
		float triggerPressure = 0;
		sprintf(buffer,"%.4f",triggerPressure);
		memcpy((char*)&sendData[MONITOR_PACKET_TAG_SIZE+eTriggerPressure*MONITOR_DATA_SIZE], buffer, MONITOR_DATA_SIZE);

	}
	sendData[MONITOR_PACKET_SIZE-1] = '\0';
	vcom_write(sendData,MONITOR_PACKET_SIZE);
	vTaskDelay(DELAY_TICK_3MS);
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USB_sendLogToPCApp				                               		*/
/*                                                                                      */
/* Details		: 	Send log in SPIF to PC application for displaying					*/
/*																						*/
/* Arguments:	(I) None																*/
/*				(O) None																*/
/*																						*/
/* ReturnValue : void	                                                         		*/
/*                                                                                      */
/****************************************************************************************/
static void USB_sendLogToPCApp()
{
	POR_LOG_T log[NUMBER_OF_LOG_QUEUE_SEND_FOR_PC_APP];
	for (int i = 0; i < NUMBER_OF_LOG_QUEUE_SEND_FOR_PC_APP; i++)
	{
		log[i].year = 0;
		log[i].month = 0;
		log[i].day = 0;
		log[i].hour = 0;
		log[i].minute = 0;
		log[i].second = 0;
		log[i].logCode = 0;
		log[i].EventOrAlarm = 0;
		log[i].oldValue.mode = 0;
		log[i].oldValue.flowRate = 0;
		log[i].oldValue.triggerSensitivity = 0;
		log[i].newValue.mode = 0;
		log[i].newValue.flowRate = 0;
		log[i].newValue.triggerSensitivity = 0;
	}
	if(USBTask_ReceiveLogQueueSendForApp(log) && (gs_errorSendLogToPCApp == false))
	{
		if(gs_isSendNumberOfLog == false)
		{
			uint8_t logNumber[LOG_NUMBER_PACKET_SIZE+1] = LOG_NUMBER_PACKET_TAG;
			gs_numberOfLogSend = logMgr_GetNumberOfLog();
			char buff[5]={'\0'};
			sprintf (buff, "%.4d", gs_numberOfLogSend);
			strncat((char*)&logNumber[LOG_NUMBER_PACKET_TAG_SIZE], buff, sizeof(buff));
			logNumber[LOG_NUMBER_PACKET_SIZE-1] = '\0';
			USB_sendData(logNumber,LOG_NUMBER_PACKET_SIZE, eMessageIDGetLogACK, 0);
			gs_isSendNumberOfLog = true;
		}
		for(int i = 0; i<10; i++)
		{
			if(log[i].logCode==eFirsLogId)
			{
				return;
			}
			gs_numberOfLogSend--;
			char buff[28]={'\0'};
			uint8_t s_log[LOG_PACKET_SIZE]=LOG_PACKET_TAG;
			sprintf (&buff[LOG_PACKET_NO_TAG_START_BYTE],"%2.d",i);
			sprintf (&buff[LOG_PACKET_YEAR_START_BYTE], "%.4d", log[i].year);
			sprintf (&buff[LOG_PACKET_MONTH_START_BYTE], "%.2d", log[i].month);
			sprintf (&buff[LOG_PACKET_DAY_START_BYTE], "%.2d", log[i].day);
			sprintf (&buff[LOG_PACKET_HOUR_START_BYTE], "%.2d", log[i].hour);
			sprintf (&buff[LOG_PACKET_MINUTE_START_BYTE], "%.2d", log[i].minute);
			sprintf (&buff[LOG_PACKET_SECOND_START_BYTE], "%.2d", log[i].second);
			sprintf (&buff[LOG_PACKET_LOG_CODE_START_BYTE], "%.4d", log[i].logCode);
			sprintf(&buff[LOG_PACKET_EVENT_OR_ALARM_START_BYTE],"%.1d",log[i].EventOrAlarm);
			sprintf(&buff[LOG_PACKET_OLD_MODE_START_BYTE],"%.1d",log[i].oldValue.mode);
			sprintf(&buff[LOG_PACKET_OLD_FLOWRATE_START_BYTE],"%.1d",log[i].oldValue.flowRate);
			sprintf(&buff[LOG_PACKET_OLD_TRIGGER_SENSITIVITY_START_BYTE],"%.1d",log[i].oldValue.triggerSensitivity);
			sprintf(&buff[LOG_PACKET_NEW_MODE_START_BYTE],"%.1d",log[i].newValue.mode);
			sprintf(&buff[LOG_PACKET_NEW_FLOWRATE_START_BYTE],"%.1d",log[i].newValue.flowRate);
			sprintf(&buff[LOG_PACKET_NEW_TRIGGER_SENSITIVITY_START_BYTE],"%.1d",log[i].newValue.triggerSensitivity);
			DEBUG_USB("%s\n",buff);
			memcpy((char*)&s_log[LOG_PACKET_TAG_SIZE],buff,sizeof(buff));
			s_log[LOG_PACKET_SIZE-1]='\0';
			DEBUG_USB("%d : %s\n",i,s_log);
			if(USB_sendData(s_log,LOG_PACKET_SIZE,eMessageIDGetLogACK,i)==true)
			{
				gs_numberOfLogSend = 0;
				gs_isSendNumberOfLog = false;
				gs_errorSendLogToPCApp = true;
				break;
			}
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USB_sendTankPressureLogToPCApp		                               	*/
/*                                                                                      */
/* Details		: 	Send Pressure log in SPIF to PC application for displaying			*/
/*																						*/
/* Arguments:	(I) None																*/
/*				(O) None																*/
/*																						*/
/* ReturnValue : void	                                                         		*/
/*                                                                                      */
/****************************************************************************************/
static void USB_sendTankPressureLogToPCApp(POR_PRESS_LOG_T* pressLog)
{
	char buff[20] = {'\0'};
	uint8_t s_logPressName[PRESSURE_TANK_LOG_PACKET_SIZE]=PRESSURE_TANK_LOG_PACKET_TAG;
	sprintf (&buff[PRESSURE_LOG_PACKET_YEAR_START_BYTE], "%.4d", pressLog->year);
	sprintf (&buff[PRESSURE_LOG_PACKET_MONTH_START_BYTE], "%.2d", pressLog->month);
	sprintf (&buff[PRESSURE_LOG_PACKET_DAY_START_BYTE], "%.2d", pressLog->day);
	sprintf (&buff[PRESSURE_LOG_PACKET_HOUR_START_BYTE], "%.2d", pressLog->hour);
	sprintf (&buff[PRESSURE_LOG_PACKET_MINUTE_START_BYTE], "%.2d", pressLog->minute);
	sprintf (&buff[PRESSURE_LOG_PACKET_SECOND_START_BYTE], "%.2d", pressLog->second);
	sprintf (&buff[PRESSURE_LOG_PACKET_LOG_CODE_START_BYTE], "%.3d", pressLog->logCode);
	memcpy((char*)&s_logPressName[PRESSURE_TANK_LOG_PACKET_TAG_SIZE],buff,PRESSURE_TANK_LOG_PACKET_SIZE-PRESSURE_TANK_LOG_PACKET_TAG_SIZE);
	s_logPressName[PRESSURE_TANK_LOG_PACKET_SIZE-1]='\0';
	if(USB_sendData(s_logPressName,PRESSURE_TANK_LOG_PACKET_SIZE,eMessageIDGetPressureLogACK,0)==true)
	{
		return;
	}
	uint8_t logPress[PRESSURE_TANK_PACKET_SIZE] = PRESSURE_TANK_PACKET_TAG;
	if(pressLog->logCode != '\0')
	{
		for(int i = 0;i < NUMBER_OF_SEND; i++)
		{
			uint8_t cnt = 0;
			sprintf (&buff[0], "%.2d", i);
			memcpy((char*)&logPress[PRESSURE_TANK_PACKET_TAG_SIZE],buff,PRESSURE_LOG_PACKET_NO_TAG_SIZE);
			for(int k = i*NUMBER_OF_PRESSURE_PER_SEND;k<(i*NUMBER_OF_PRESSURE_PER_SEND)+NUMBER_OF_PRESSURE_PER_SEND;k++)
			{
				sprintf (&buff[0], "%.3d", pressLog->PressData[i*NUMBER_OF_PRESSURE_PER_SEND+cnt]);
				memcpy((char*)&logPress[PRESSURE_TANK_PACKET_TAG_SIZE+PRESSURE_LOG_PACKET_NO_TAG_SIZE+cnt*NUMBER_OF_BYTE_PER_PRESS],buff,NUMBER_OF_BYTE_PER_PRESS);
				cnt++;
			}
			logPress[PRESSURE_TANK_PACKET_SIZE-1]='\0';
			if(USB_sendData(logPress,PRESSURE_TANK_PACKET_SIZE,eMessageIDGetPressureLogACK,i)==true)
			{
				break;
			}
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USB_sendSupplyPressureLogToPCApp	                               	*/
/*                                                                                      */
/* Details		: 	Send Pressure log in SPIF to PC application for displaying			*/
/*																						*/
/* Arguments:	(I) None																*/
/*				(O) None																*/
/*																						*/
/* ReturnValue : void	                                                         		*/
/*                                                                                      */
/****************************************************************************************/
static void USB_sendSupplyPressureLogToPCApp(POR_PRESS_LOG_T* pressLog)
{
	char buff[20] = {'\0'};
	uint8_t s_logPressName[PRESSURE_SUPPLY_LOG_PACKET_SIZE]=PRESSURE_SUPPLY_LOG_PACKET_TAG;
	sprintf (&buff[PRESSURE_LOG_PACKET_YEAR_START_BYTE], "%.4d", pressLog->year);
	sprintf (&buff[PRESSURE_LOG_PACKET_MONTH_START_BYTE], "%.2d", pressLog->month);
	sprintf (&buff[PRESSURE_LOG_PACKET_DAY_START_BYTE], "%.2d", pressLog->day);
	sprintf (&buff[PRESSURE_LOG_PACKET_HOUR_START_BYTE], "%.2d", pressLog->hour);
	sprintf (&buff[PRESSURE_LOG_PACKET_MINUTE_START_BYTE], "%.2d", pressLog->minute);
	sprintf (&buff[PRESSURE_LOG_PACKET_SECOND_START_BYTE], "%.2d", pressLog->second);
	sprintf (&buff[PRESSURE_LOG_PACKET_LOG_CODE_START_BYTE], "%.3d", pressLog->logCode);
	memcpy((char*)&s_logPressName[PRESSURE_SUPPLY_LOG_PACKET_TAG_SIZE],buff,PRESSURE_SUPPLY_LOG_PACKET_SIZE-PRESSURE_SUPPLY_LOG_PACKET_TAG_SIZE);
	s_logPressName[PRESSURE_SUPPLY_LOG_PACKET_SIZE-1]='\0';
	if(USB_sendData(s_logPressName,PRESSURE_SUPPLY_LOG_PACKET_SIZE,eMessageIDGetPressureLogACK,0)==true)
	{
		return;
	}
	uint8_t logPress[PRESSURE_SUPPLY_PACKET_SIZE] = PRESSURE_SUPPLY_PACKET_TAG;
	if(pressLog->logCode != '\0')
	{
		for(int i =0;i<NUMBER_OF_SEND;i++)
		{
			uint8_t cnt = 0;
			sprintf (&buff[0], "%.2d", i);
			memcpy((char*)&logPress[PRESSURE_SUPPLY_PACKET_TAG_SIZE],buff,PRESSURE_LOG_PACKET_NO_TAG_SIZE);
			for(int k = i*NUMBER_OF_PRESSURE_PER_SEND;k<(i*NUMBER_OF_PRESSURE_PER_SEND)+NUMBER_OF_PRESSURE_PER_SEND;k++)
			{
				sprintf (&buff[0], "%.3d", pressLog->PressData[i*NUMBER_OF_PRESSURE_PER_SEND+cnt]);
				memcpy((char*)&logPress[PRESSURE_SUPPLY_PACKET_TAG_SIZE+PRESSURE_LOG_PACKET_NO_TAG_SIZE+cnt*NUMBER_OF_BYTE_PER_PRESS],buff,NUMBER_OF_BYTE_PER_PRESS);
				cnt++;
			}
			logPress[PRESSURE_SUPPLY_PACKET_SIZE-1]='\0';
			if(USB_sendData(logPress,PRESSURE_SUPPLY_PACKET_SIZE,eMessageIDGetPressureLogACK,i)==true)
			{
				break;
			}
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USB_sendVacuumPressureLogToPCApp		                            */
/*                                                                                      */
/* Details		: 	Send Pressure log in SPIF to PC application for displaying			*/
/*																						*/
/* Arguments:	(I) None																*/
/*				(O) None																*/
/*																						*/
/* ReturnValue : void	                                                         		*/
/*                                                                                      */
/****************************************************************************************/
static void USB_sendVacuumPressureLogToPCApp(POR_PRESS_LOG_T* pressLog)
{
	char buff[20]={'\0'};
	uint8_t s_logPressName[PRESSURE_VACUUM_LOG_PACKET_SIZE]=PRESSURE_VACUUM_LOG_PACKET_TAG;
	sprintf (&buff[PRESSURE_LOG_PACKET_YEAR_START_BYTE], "%.4d", pressLog->year);
	sprintf (&buff[PRESSURE_LOG_PACKET_MONTH_START_BYTE], "%.2d", pressLog->month);
	sprintf (&buff[PRESSURE_LOG_PACKET_DAY_START_BYTE], "%.2d", pressLog->day);
	sprintf (&buff[PRESSURE_LOG_PACKET_HOUR_START_BYTE], "%.2d", pressLog->hour);
	sprintf (&buff[PRESSURE_LOG_PACKET_MINUTE_START_BYTE], "%.2d", pressLog->minute);
	sprintf (&buff[PRESSURE_LOG_PACKET_SECOND_START_BYTE], "%.2d", pressLog->second);
	sprintf (&buff[PRESSURE_LOG_PACKET_LOG_CODE_START_BYTE], "%.3d", pressLog->logCode);
	memcpy((char*)&s_logPressName[PRESSURE_VACUUM_LOG_PACKET_TAG_SIZE],buff,PRESSURE_VACUUM_LOG_PACKET_SIZE-PRESSURE_VACUUM_LOG_PACKET_TAG_SIZE);
	s_logPressName[PRESSURE_VACUUM_LOG_PACKET_SIZE-1]='\0';
	if(USB_sendData(s_logPressName,PRESSURE_VACUUM_LOG_PACKET_SIZE,eMessageIDGetPressureLogACK,0)==true)
	{
		return;
	}
	uint8_t logPress[PRESSURE_VACUUM_PACKET_SIZE] = PRESSURE_VACUUM_PACKET_TAG;
	if(pressLog->logCode != '\0')
	{
		for(int i =0;i<NUMBER_OF_SEND;i++)
		{
			uint8_t cnt = 0;
			sprintf (&buff[0], "%.2d", i);
			memcpy((char*)&logPress[PRESSURE_VACUUM_PACKET_TAG_SIZE],buff,PRESSURE_LOG_PACKET_NO_TAG_SIZE);
			for(int k = i*NUMBER_OF_PRESSURE_PER_SEND;k<(i*NUMBER_OF_PRESSURE_PER_SEND)+NUMBER_OF_PRESSURE_PER_SEND;k++)
			{
				if(pressLog->PressData[i*NUMBER_OF_PRESSURE_PER_SEND+cnt]<0)
				{
					buff[VACUUM_PRESSURE_SIGN_BYTE]=NEGATIVE_VALUE;
				}
				else
				{
					buff[VACUUM_PRESSURE_SIGN_BYTE]=POSITIVE_VALUE;
				}
				sprintf (&buff[1], "%.3d", abs(pressLog->PressData[i*NUMBER_OF_PRESSURE_PER_SEND+cnt]));
				memcpy((char*)&logPress[PRESSURE_VACUUM_PACKET_TAG_SIZE+PRESSURE_LOG_PACKET_NO_TAG_SIZE+cnt*NUMBER_OF_BYTE_PER_VACUUM_PRESS],buff,NUMBER_OF_BYTE_PER_VACUUM_PRESS);
				cnt++;
			}
			logPress[PRESSURE_VACUUM_PACKET_SIZE-1]='\0';
			if(USB_sendData(logPress,PRESSURE_VACUUM_PACKET_SIZE,eMessageIDGetPressureLogACK,i)==true)
			{
				break;
			}
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USB_sendErrorCode				                               		*/
/*                                                                                      */
/* Details		: 	Send Error code to PC application for displaying in monitor screen	*/
/*																						*/
/* Arguments:	(I) E_ID_ERRORCODE code													*/
/*				(O) None																*/
/*																						*/
/* ReturnValue : void	                                                         		*/
/*                                                                                      */
/****************************************************************************************/
static void USB_sendErrorCode(E_ID_ERRORCODE code)
{
	uint8_t alarm[ALARM_PACKET_SIZE] = ALARM_PACKET_TAG;
	char buff[3]={'\0'};
	sprintf (&buff[0], "%.2d", code);
	memcpy((char*)&alarm[ALARM_PACKET_TAG_SIZE],buff,sizeof(buff));
	alarm[ALARM_PACKET_SIZE-1]='\0';
	vcom_write(alarm,ALARM_PACKET_SIZE);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USB_sendOperationTime				                               	*/
/*                                                                                      */
/* Details		: 	Send Operation Time to PC Application for displaying				*/
/*																						*/
/* Arguments:	(I) None																*/
/*				(O) None																*/
/*																						*/
/* ReturnValue : void	                                                         		*/
/*                                                                                      */
/****************************************************************************************/
static void USB_sendOperationTime()
{
	char buffer[9]={'\0'};
	uint8_t systemInfor[SYSTEM_INFO_PACKET_OPERATION_TIME_SIZE]=SYSTEM_INFO_PACKET_OPERATION_TIME_TAG;

	uint32_t totalOperationTimeContinuous = OperationTime_GetTotalOperationTimeContinuous();
	sprintf (buffer,"%.5d",totalOperationTimeContinuous);
	memcpy((char*)&systemInfor[SYSTEM_INFO_PACKET_OPERATION_TIME_TAG_SIZE
							   + SYSTEM_INFO_PACKET_OPERATION_TIME_DATA_SIZE*0],buffer,SYSTEM_INFO_PACKET_TOTAL_OPERATION_TIME_SIZE);

	uint32_t totalOperationTimeSynchronize = OperationTime_GetTotalOperationTimeSynchronize();
	sprintf (buffer,"%.5d",totalOperationTimeSynchronize);
	memcpy((char*)&systemInfor[SYSTEM_INFO_PACKET_OPERATION_TIME_TAG_SIZE
							   + SYSTEM_INFO_PACKET_OPERATION_TIME_DATA_SIZE*1],buffer,SYSTEM_INFO_PACKET_TOTAL_OPERATION_TIME_SIZE);

	uint32_t operationTimeContinuous = OperationTime_GetOperationTimeAfterMaintenanceContinuous();
	sprintf (buffer,"%.5d",operationTimeContinuous);
	memcpy((char*)&systemInfor[SYSTEM_INFO_PACKET_OPERATION_TIME_TAG_SIZE
							   + SYSTEM_INFO_PACKET_OPERATION_TIME_DATA_SIZE*2],buffer,SYSTEM_INFO_PACKET_TOTAL_OPERATION_TIME_SIZE);

	uint32_t operationTimeSynchronize = OperationTime_GetOperationTimeAfterMaintenanceSynchronize();
	sprintf (buffer,"%.5d",operationTimeSynchronize);
	memcpy((char*)&systemInfor[SYSTEM_INFO_PACKET_OPERATION_TIME_TAG_SIZE
							   + SYSTEM_INFO_PACKET_OPERATION_TIME_DATA_SIZE*3],buffer,SYSTEM_INFO_PACKET_TOTAL_OPERATION_TIME_SIZE);

	systemInfor[SYSTEM_INFO_PACKET_OPERATION_TIME_SIZE-1]='\0';
	USB_sendData(systemInfor,SYSTEM_INFO_PACKET_OPERATION_TIME_SIZE,eMessageIDSendOperationTimeACK,0);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USB_SendLogHandle					                               	*/
/*                                                                                      */
/* Details		: 	This function checks whether log send is requested from PC App. 	*/
/*					If there is request, this one will handle sending operation			*/
/* Arguments:	(I) None																*/
/*				(O) None																*/
/*																						*/
/* ReturnValue : void	                                                         		*/
/*                                                                                      */
/****************************************************************************************/
static void USB_SendLogHandle()
{
	if(gs_numberOfLogSend>0)
	{
		USB_sendLogToPCApp();
	}
	else
	{
		gs_isSendNumberOfLog=false;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USB_SendPressureLogHandle			                               	*/
/*                                                                                      */
/* Details		: 	This function checks whether log pressure send is requested from PC */
/*					App. If there is request, this one will handle sending operation	*/
/* Arguments:	(I) None																*/
/*				(O) None																*/
/*																						*/
/* ReturnValue : void	                                                         		*/
/*                                                                                      */
/****************************************************************************************/
static void USB_SendPressureLogHandle()
{
	static uint16_t s_cnt = 0;
	if(gs_isWaitingPressureLog == true)
	{
		s_cnt++;
		if(s_cnt>SEND_PRESSURE_LOG_TIME_OUT)
		{
			s_cnt = 0;
			gs_isWaitingPressureLog = false;
		}
		POR_PRESS_LOG_T pressLog = {'\0'};
		if(USBTask_ReceiveLogQueueSendPressureForPCApp(&pressLog))
		{
			switch(pressLog.logCode)
			{
			case eTankPressureLog:
				USB_sendTankPressureLogToPCApp(&pressLog);
				break;
			case eVacuumPressureLog:
				USB_sendVacuumPressureLogToPCApp(&pressLog);
				break;
			case eSupplyPressureLog:
				USB_sendSupplyPressureLogToPCApp(&pressLog);
				gs_isWaitingPressureLog = false;
			default:
				break;
			}
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USB_SendSystemInformationHandle		                               	*/
/*                                                                                      */
/* Details		: 	This function checks whether system information send is requested 	*/
/*					from PC App. If there is request, this one will handle sending 		*/
/*	 				operation															*/
/* Arguments:	(I) None																*/
/*				(O) None																*/
/*																						*/
/* ReturnValue : void	                                                         		*/
/*                                                                                      */
/****************************************************************************************/
static void USB_SendSystemInformationHandle()
{
	if(gs_isSendSystemInfor)
	{
		USB_sendSystemInformation();
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USB_init					                               			*/
/*                                                                                      */
/* Details		: 	Init usb vcom														*/
/*																						*/
/* Arguments:	(I) None																*/
/*				(O) None																*/
/*																						*/
/* ReturnValue : void	                                                         		*/
/*                                                                                      */
/****************************************************************************************/
void USB_init()
{
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, USB_DETECT_5V_PORT_NUM, USB_DETECT_5V_BIT_NUM);
	USBVcom_init();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USB_Handle							                               	*/
/*                                                                                      */
/* Details		: 	This function is called every 30ms to read Vcom buffer to check		*/
/*					whether the request from PC application exists to process, this also*/
/*					send message to PC application for updating current status on		*/
/* 					monitor screen. 													*/
/* Arguments:	(I) None																*/
/*				(O) None																*/
/*																						*/
/* ReturnValue : void	                                                         		*/
/*                                                                                      */
/****************************************************************************************/
void USB_Handle()
{
	static uint32_t rdCnt = 0;
	static uint8_t s_rxBuff[MAXIMUM_NUMBER_OF_RECEIVE_BYTE];
	static E_USBQUEUE usbQueue ;
	rdCnt = vcom_bread(&s_rxBuff[0], MAXIMUM_NUMBER_OF_RECEIVE_BYTE);
	if (rdCnt)
	{
		uint8_t buffer[MAXIMUM_NUMBER_OF_RECEIVE_BYTE]={"\0"};
		memcpy(buffer,s_rxBuff,sizeof(buffer));
		uint8_t CRC = vcom_Checksum(0x0000,(unsigned char*)buffer,MAXIMUM_NUMBER_OF_RECEIVE_BYTE-1);
		if(CRC==buffer[MAXIMUM_NUMBER_OF_RECEIVE_BYTE-1])
		{
			DEBUG_USB("Data receive %d %d %d %d %d\n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4]);
			USB_MessageHandle(buffer);
		}
	}
	USB_SendLogHandle();
	USB_SendPressureLogHandle();
	USB_SendSystemInformationHandle();
	if((vcom_connected() != CONNECTED_CONSTANT_1) && (vcom_connected() != CONNECTED_CONSTANT_2)
			&& Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,USB_DETECT_5V_PORT_NUM,USB_DETECT_5V_BIT_NUM)==false)
	{
		devCtrl_GetPCAppMonitorStruct(&gs_currentValue);
		USB_sendMonitorData();
	}
	if(USBTask_ReceiveUSBQueue(&usbQueue))
	{
		if((vcom_connected() != CONNECTED_CONSTANT_1) && (vcom_connected() != CONNECTED_CONSTANT_2))
		{
			if(usbQueue.ID == eErrorCode)
				USB_sendErrorCode((E_ID_ERRORCODE)usbQueue.data);
			else if(usbQueue.ID == eOperationTimeUpdate)
			{
				USB_sendOperationTime();
				DEBUG_USB("Send new Operation Time\n");
			}
		}
	}
	if(gs_CntOneSecondSetTime <= ONE_SECOND_WAIT_FOR_RTC_SET_TIME)
		gs_CntOneSecondSetTime++;
}
