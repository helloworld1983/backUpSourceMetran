#ifndef DEVICE_INC_DEVTASK_H_
#define DEVICE_INC_DEVTASK_H_

/********************************************************************/
/*                                                                  */
/* File Name    : DeviceTask.h		                                */
/*                                                                  */
/* General      : This module contain function for Create RTOS task */
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

#include "DualCoreCommon.h"


typedef struct
{
	uint8_t requestId;
	int16_t requestData;
	int16_t requestTestAlarmType;
	int16_t requestTestAlarmData;
}USB_MSG_TO_DEV;


typedef struct
{
	uint16_t numberOfLog;
}LOG_MSG_TO_DEV;


typedef struct
{
	uint16_t deviceId;
}ALARM_MSG_TO_DEV;


typedef struct
{
	bool isSpeakerConnected;
}AUDIO_MSG_TO_DEV;

typedef union
{
	USB_MSG_TO_DEV 		usbMsg;
	LOG_MSG_TO_DEV 		logMgs;
	ALARM_MSG_TO_DEV	alarmMsg;
	AUDIO_MSG_TO_DEV 	audioMsg;
}DEV_MSG_DATA;

typedef struct{
	uint8_t id;
	DEV_MSG_DATA msgData;
}DEV_EVENT_STRUCT;




/****************************************************************************************/
/*                                                                                      */
/* Function name:   devTask_Create                                                      */
/*                                                                                      */
/* Details:        Create FREERTOS task name devTask                                    */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void devTask_CreateTask();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   devTask_Init                                                        */
/*                                                                                      */
/* Details:        Init device task by call devCtrl_Init function for init all devices  */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void devTask_Init();


/****************************************************************************************/
/*                                                                                      */
/* Function name:   SendToDeviceQueue                                                   */
/*                                                                                      */
/* Details:        Send data to Device Queue                                            */
/*                                                                                      */
/* Arguments:   E_DEVICEQUEUE pDeviceQueueData                               	        */
/*              TickType_t xTicksToWait                                                 */
/*                                                                                      */
/* ReturnValue : true if it sent successfully                                           */
/*               false if it fails to sent                                              */
/*                                                                                      */
/****************************************************************************************/
bool devTask_SendToDeviceQueue(DEV_EVENT_STRUCT pDeviceQueueData,TickType_t xTicksToWait);


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
bool devTask_SendToAlarmQueue(ALARM_STAT_T alarmQueueData, TickType_t xTicksToWait );
#endif /* DEVICE_INC_DEVTASK_H_ */
