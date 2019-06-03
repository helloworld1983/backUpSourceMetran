/********************************************************************/
/*                                                                  */
/* File Name    : DeviceInterface.c                                 */
/*                                                                  */
/* General       : This file contains the function for interface
 * 				   with device module 		                        */
/*                 	 						                     */
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            2016/09/29      Quyen Ngo(MV)  			new file 	*/
/*                                                                  */
/* #0001	  2017/09/02	  Quyen Ngo(MV)				modify		*/
/*		change s_ipcMsgData to static								*/
/* #0002	  2017/09/08	  Quyen Ngo(MV)				modify		*/
/*		delete call to set O2 cylinder status						*/
/* #0003	  2017/09/20	  Quyen Ngo(MV)				modify		*/
/*		handle disable and enable encoder for all medium/low alarm ID*/
/* #0004	  2017/09/19	  Viet Le					modify		*/
/*    remove handle case eNegativePressureId						*/
/* #0005	  2017/09/27	  Quyen Ngo(MV)				modify		*/
/*	    only call to enable encoder if alarm which is allowed to 	*/
/*		enable encoder is activated									*/
/* #0006	  2017/11/07	  Viet Le				    modify		*/
/*	  Add case eCompressorDCOverVoltageId,							*/
/*	eCompressorSoftwareOverCurrentId,eCompressorHardwareOverCurrent,*/
/*		eFlowControllerCommandTimeOutId								*/
/*  in devInterface_AlarmProcessGuiRequest() function (bug 2014)    */
/********************************************************************/

/* include system files */
#include <string.h>
/* include user files */
#include <DeviceInterface.h>
#include "DeviceController.h"
#include "AlarmTask.h"
#include "ModeHandle.h"
#include "FilterCleaningModule.h"
#include "IpcMsg.h"
#include "IpcTask.h"
#include "EncoderInterface.h"

/* constants, macro definition */
/* definitions of structures */

/*global variable*/
static QueueHandle_t s_deviceTskQueue;
static IPC_MSG_DATA_t s_ipcMsgData;
/* external declaration */
SemaphoreHandle_t g_alarmMonitorMutex;

/* Body of functions */

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devInterface_Init					              				    */
/*                                                                                      */
/* Details:         Init devInterface by create mutex				    				*/
/*                                                                                      */
/* Arguments:																			*/
/*																						*/
/* Macros:			DEVICE_QUEUE_LENGTH													*/
/*                                                                                      */
/* ReturnValue : 				                                                        */
/*                                                                                      */
/* Using Global Data : (O) QueueHandle_t s_deviceTskQueue        						*/
/*					   (O) SemaphoreHandle_t g_alarmMonitorMutex						*/
/*					   (O) IPC_MSG_DATA_t s_ipcMsgData									*/
/*                                                                                      */
/****************************************************************************************/
void devInterface_Init(void)
{
	s_deviceTskQueue = xQueueCreate(DEVICE_QUEUE_LENGTH, sizeof(IPC_MSG_DATA_t));
	g_alarmMonitorMutex = xSemaphoreCreateMutex();

	s_ipcMsgData.data_id = eNoDeviceEventId;
	s_ipcMsgData.data_value = 0x00;
	memset(&s_ipcMsgData.stAlarmStatPacket, 0, sizeof(s_ipcMsgData.stAlarmStatPacket));
	memset(&s_ipcMsgData.stPsaParamPacket, 0, sizeof(s_ipcMsgData.stPsaParamPacket));
	memset(&s_ipcMsgData.stPsaMonitorPacket, 0, sizeof(s_ipcMsgData.stPsaMonitorPacket));
	memset(&s_ipcMsgData.version, 0, sizeof(s_ipcMsgData.version));
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-devInterface_SendMsg	                				    				*/
/*                                                                                      */
/* Details  -Send Msg to device task												    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) IPC_MSG_DATA_t msg													*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* Macros:			DEVICE_QUEUE_WAIT_TIME												*/
/*                                                                                      */
/* ReturnValue : bool			                                                    	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) QueueHandle_t s_deviceTskQueue        						*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool devInterface_SendMsg(IPC_MSG_DATA_t msg)
{
	if(xQueueSend(s_deviceTskQueue, &msg,DEVICE_QUEUE_WAIT_TIME) == pdFALSE)
	{
		DEBUGOUT("\n Failure send");
		return false;
	}
	return true;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devInterface_ReceiveMsg						             		   */
/*                                                                                      */
/* Details:         hanle message receive from other task(IPC task)	    				*/
/*                                                                                      */
/*                                                                                      */
/* ReturnValue : NONE                                                    			    */
/*                                                                                      */
/* Using Global Data : (I) QueueHandle_t s_deviceTskQueue        						*/
/*                                                                                      */
/****************************************************************************************/
void devInterface_HandleMsgQueueReceive(void)
{
	if(xQueueReceive(s_deviceTskQueue, &s_ipcMsgData, 0) == pdTRUE)
	{
		devCtrl_HandleGuiMsg(s_ipcMsgData);
	}
	return;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmInterface_GetAlarmMonitor						                */
/*                                                                                      */
/* Details:         return current alarm monitor for alarm task		    				*/
/*                                                                                      */
/*                                                                                      */
/* ReturnValue : ALARM_MONITOR_t                                                        */
/*                                                                                      */
/* Using Global Data : (I) alarmMonitor                           						*/
/*                                                                                      */
/****************************************************************************************/
ALARM_MONITOR_t devInterface_GetAlarmMonitor(void)
{
	return devCtrl_GetAlarmMonitorStruct();
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devInterface_AlarmProcessGuiRequest	                				*/
/*                                                                                      */
/* Details:         Handle message from GUI task for process alarm behaviour			*/
/*                                                                                      */
/* Arguments:		(I) ALARM_STAT_T alarmStat                              			*/
/*                                                                                      */
/* ReturnValue : 				                                                        */
/*                                                                                      */
/* Using Global Data : (I) alarmMonitor                           						*/
/*                                                                                      */
/****************************************************************************************/
void devInterface_AlarmProcessGuiRequest(ALARM_STAT_T alarmStat)
{
	switch (alarmStat.ID) {
	case eTemperatureId:                               //2
	case eHighPressureId:                              //3
	case eLowPressureId:                               //4
	case eCompressorDCOverVoltageId:
	case eCompressorSoftwareOverCurrentId:
	case eCompressorHardwareOverCurrent:
	case eCompressorBoardTemperatureId:
	case eCompressorRPMId:
	case eCoolingFanId:                                //14
	case eO2ConcentrationId:                           //15
	case eFlowRateId:
	case eCompressorOutletTemperatureId:               //26
	case eFlowControllerCommunicationId:               //28
	case eFlowControllerPressureSensorId:              //29
	case eFlowControllerUltrasonicSensorId:            //30
	case eFlowControllerTemperatureSensorId:           //31
	case eFlowControllerCommandTimeOutId:
		if(alarmStat.currentStatus == eActive)
		{
			//Disable flow rate change and continue normal operation
			DEBUGOUT("\nhigh alarm");
			encoderInterface_SetEnable(false);
		}
		break;

	case eLoweringO2ConcentrationId:                   //18
	case eO2ConcentrationSensorId:                     //19
	case eRisingCompressorOutletTemperatureId:         //27
		if(alarmStat.currentStatus == eActive)
		{
			//Disable flow rate change and continue normal operation
			DEBUGOUT("\nmedium alarm active");
			encoderInterface_SetEnable(false);
		}
		else
		{
			DEBUGOUT("\nmedium alarm not active");
			//Enable flow rate change
			encoderInterface_SetEnable(true);
		}
		break;
	case eCannulaUnder27sId:
	case eCannulaMore27sId:
	case eExternalFlowPathpressureId:
		if(alarmStat.currentStatus == eActive)
		{
			encoderInterface_SetEnable(true);
		}
		break;
	case eFilterCleaningId:
		if(alarmStat.currentStatus == eActive)
		{
			encoderInterface_SetEnable(true);
			filter_Stop();
		}
		break;
	default:
		break;
	}
	return;
}
/* end of files */
