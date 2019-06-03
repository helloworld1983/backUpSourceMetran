/********************************************************************/
/*                                                                  */
/* File Name    : LogTask.c                                    		*/
/*                                                                  */
/* General      : LogTask module define a RTOS task for write	    */
/*                and read FH310 log						     	*/
/*                                                                  */
/* Product Name  : FH-310                                           */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/20     Linh Nguyen(MV)  	    new file        */
/*  #0001	   2017/8/28      Linh Nguyen(MV)  	    modify	        */
/*           check condition when record overcurrent log            */
/* #0002       2017/09/12     Linh Nguyen	  	    modify          */
/*  Using new macro for coding rule									*/
/*  Remove getting flow rate when receive a log                     */
/* #0003       2017/09/22     Linh Nguyen	  	    modify          */
/*  Change the log queue send for another task						*/
/* #0003       2017/09/28     Linh Nguyen	  	    modify          */
/*  Change the log queue leng										*/
/* 		       2017/09/28     Linh Nguyen	  	    modify          */
/*  #0004 Record all log in the log queue on one cycle				*/
/* 		       2017/11/03     Linh Nguyen	  	    modify          */
/*  #0005 Add missing log ID (bug 2014)								*/
/********************************************************************/

/* include system files */
#include <LogTask.h>
#include <LogMgr.h>
#include "stdint.h"
#include "spifilib_dev.h"
#include "spifilib_api.h"
#include "GuiInterface.h"
#include "LogInterface.h"

/* include user files */

/* constants, macro definition */
#define LOG_TASK_DELAY 50 / portTICK_PERIOD_MS
#define LOG_TASK_SIZE (4*1024)

/* definitions of structures */

/*global variable*/

/* external declaration */
QueueHandle_t logQueueSendForGUI;
QueueHandle_t logQueueSendForCBX;
QueueHandle_t logQueueSendForAnypal;
QueueHandle_t logQueueSendForUSB;

QueueHandle_t logQueueRecive;
QueueHandle_t LogRequestQueue;

extern SPIFI_HANDLE_T *pSpifi;
/* Body of functions */

/****************************************************************************************/
/*                                                                                      */
/* Function name： LogTask				                                                */
/*                                                                                      */
/* Details：     Handle all of Log task processing           								*/
/* Using Global Data : (I) 								                              	*/
/*                     (O)                                              				*/
/* local variables   : (I)             			              							*/
/*                                                                                      */
/****************************************************************************************/
static void LogTask(void)
{
	logMgr_InitLogMemory();
	FH310_LOG_T log;
	LOG_REQUEST_t request;
	log.ID = eNoneLog;
	log.SpO2 =0;
	log.pulse =0;
	log.flowRateSetting = 0;
	log.YYYY = 0;
	log.DD = 0;
	log.MM = 0;
	log.hh = 0;
	log.min = 0;
	log.ss = 0;
	request.id = eNotGetID;
	request.beginLog =0;
	bool isChagneMode = true;

	while(1)
	{
		while(xQueueReceive(logQueueRecive, &log, 0))
		{			
			Real_Time_Clock clock;
			guiInterface_RTCGettime(&clock);

			if(log.ID != eAnypalLogId)
			{
				log.YYYY = clock.year;//1991;//get
				log.DD = clock.mday;//13;//get
				log.MM = clock.month;//6;//get
				log.hh = clock.hour;//10;//get
				log.min = clock.min;//18;//get
				log.ss = clock.sec;//8;//get
			}
			switch (log.ID)
			{
			case eOperationStartLogId:
			case eOperationStopLogId:
				isChagneMode = true;
			case eChangingFlowRateLogId:
				logMgr_RecordLog(log, eMachineLog);
				logMgr_RecordLog(log, eServerLog);
				logMgr_RecordLog(log, eUsageLog);
				break;

			case eAbnormalpoweroutageininoperationLogId:
			{
				logMgr_RecordLog(log, eMachineLog);

				GUI_EVENT_STRUCT event;
				event.id = eResetWDTEventId;
				event.data.charData = true;
				guiInterface_SendGuiEvent(event);
				break;
			}
			case eOperationRestartLogId:
			case eAbnormalpressedoperationswduringunplugLogId:
			case eAbnormalTemperatureLogId:
			case eAbnormalHighPressureLogId:
			case eAbnormalLowPressureLogId:
			case eAbnormalLoweringO2ConcentrationLogId:
			case eAbnormalO2ConcentrationLogId:
			case eAbnormalCannulaLogId:
			case eAbnormalO2ConcentrationSensorLogId:
			case eAbnormalCoolingFanLogId:
			case eAbnormalFlowRateLogId:
			case eAbnormalAnypalCommunicationLogId:
			case eAbnormalAnypalBatteryLogId:
			case eAbnormalAnypalRtcLogId:
			case eUsbCommunicationsuccessLogId:
			case eUsbCommunicationFailureLogId:
			case ePsaParameterSetLowNormalO2ConcentrationLogId:
			case ePsaParameterSetLowLoweringO2ConcentrationLogId:
			case ePsaParameterSetNormalNormalO2ConcentrationLogId:
			case ePsaParameterSetNormalLoweringO2ConcentrationLogId:
			case ePsaParameterSetHighNormalO2ConcentrationLogId:
			case ePsaParameterSetHighLoweringO2ConcentrationLogId:
			case eAbnormalCompressorCommunicationLogId:
			case eAbnormalFilterCleaningModuleLogId:
			case eAbnormalSpeakerLogId:
			case eResetWatchdogLogId:
			case eAbnormalRisingCompressorOutletTemperatureLogId:
			case eAbnormalCompressorOutletTemperatureLogId:
			case eUseDefaultParameterLogId:
			case eAbnormalNegativePressureLogId:
			case eAbnormalNonvolatileMemoryLogId:
			case eAbnormalExternalFlowPathPressureLogId:
			case eAbnormalFlowControllerPressureSensorLogId:
			case eAbnormalUltrasonicControllerPressureSensorLogId:
			case eAbnormalTemperatureControllerPressureSensorLogId:
			case eAbnormalFlowControllerCommunicationLogId:
			case eFlowControllerCommandChecksumErrorLogId:
			case eFlowControllerCommandCodeErrorLogId:
			case eFlowControllerCommandParameterErrorLogId:
			case eFlowControllerResponseChecksumErrorLogId:
			case eFlowControllerResponseCodeErrorLogId:
			case eFlowControllerResponseParameterErrorLogId:
			case eFlowControllerResponseLengthErrorLogId:
			case eFlowControllerNoResponseLogId:
			case eAbnormalCompressorDCOverVoltageLogId:
			case eAbnormalCompressorSoftwareOverCurrentLogId:
			case eAbnormalCompressorHardwareOverCurrentLogId:
			case eAbnormalCompressorBoardTemperatureLogId:
			case eCompressorRpmErrorLogId:
			case eOxygenCylinderConnectionDetectedLogId:
			case eOxygenCylinderNoConnectionDetectedLogId:
			case eFlowControllerCommandTimeOutAbnormalId:
			case eFlowControllerResetNotificationId:
			case eCompressorCommandTimeOutAbnormalId:
			case eCompressorCommandResetNotificationId:
				logMgr_RecordLog(log, eMachineLog);
				break;

			case eOverCurrentIsDetectedLogId:
			{
				if (isChagneMode)
				{
					isChagneMode = false;
					logMgr_RecordLog(log, eMachineLog);
				}
				break;
			}

			case eAnypalLogId:
				logMgr_RecordLog(log, eAnypalLog);
				break;

			case eAnypalInsertLogId:
			case eAnypalPutOutLogId:
			case eChangeHourLogId:
			case eChangeDayLogId:
				logMgr_RecordLog(log, eUsageLog);
				break;

			default:
				break;
			}
		}

		if(xQueueReceive(LogRequestQueue, &request, 0))
		{
			logMgr_RequsetLogEvent(request);
		}

		vTaskDelay(LOG_TASK_DELAY);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name： logTask_CreateLogTask				                              	*/
/*                                                                                      */
/* Details		：  Create a RTOS task             										*/
/*                                                                                      */
/****************************************************************************************/
void logTask_CreateLogTask(void)
{
	//DEBUGOUT("create log task\n");
	xTaskCreate((TaskFunction_t)LogTask, "Log task", LOG_TASK_SIZE, (void *)1, TASK_PRIO_LOG_TASK, ( TaskHandle_t * ) NULL);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name： logTask_InitLogTask											    */
/*                                                                                      */
/* Details  -init the log task queue									 				*/
/*                                                                                      */
/* Using Global Data : (O) alarmQueue							                        */
/*                                                                                      */
/****************************************************************************************/
void logTask_InitLogTask(void)
{
	logQueueSendForGUI = xQueueCreate(LOG_QUEUE_SEND_LENGTH, sizeof(FH310_LOG_T)*MAX_LOG_GUI_RQ);
	logQueueSendForUSB = xQueueCreate(LOG_QUEUE_SEND_LENGTH, sizeof(FH310_LOG_T)*MAX_LOG_USB_RQ);
	logQueueSendForCBX = xQueueCreate(LOG_QUEUE_SEND_LENGTH, sizeof(FH310_LOG_T)*MAX_LOG_CBX_RQ);
	logQueueSendForAnypal = xQueueCreate(LOG_QUEUE_SEND_LENGTH, sizeof(FH310_LOG_T)*MAX_LOG_ATP_RQ);

	logQueueRecive = xQueueCreate(LOG_QUEUE_RECEIVE_LENGTH, sizeof(FH310_LOG_T));
	LogRequestQueue = xQueueCreate(LOG_QUEUE_REQUEST_LENGTH, sizeof(LOG_REQUEST_t));
	return;
}
/* end of files */

