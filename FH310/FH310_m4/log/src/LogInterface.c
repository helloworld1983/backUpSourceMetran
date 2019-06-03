/********************************************************************/
/*                                                                  */
/* File Name    : LogInterface.c                              		*/
/*                                                                  */
/* General      : Contains functions which are Log interface		*/
/*                                                                  */
/* Product Name  : FH-310                                           */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/07/22     Linh Nguyen(MV)  	    new file        */
/* #0001       2016/09/12     Linh Nguyen	  	    modify          */
/*  Add the flow rate into save log message							*/
/* #0002       2017/09/19     Linh Nguyen	  	    modify          */
/*  Add receive log function for another task						*/
/********************************************************************/
#include "LogInterface.h"
#include "TaskCommon.h"
#include "LogMgr.h"
#include "GuiInterface.h"

extern QueueHandle_t LogRequestQueue;
extern QueueHandle_t logQueueRecive;

extern QueueHandle_t logQueueSendForGUI;
extern QueueHandle_t logQueueSendForUSB;
extern QueueHandle_t logQueueSendForAnypal;
extern QueueHandle_t logQueueSendForCBX;

#define WRITE_LOG_SEMAPHORE_WAITING 	2/portTICK_PERIOD_MS


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logInterface_WriteLog		                                  		*/
/*                                                                                      */
/* Details		: 	Write a log to spi flash interface									*/
/*                                                                                      */
/* Arguments 	: 	(I)  FH310_LOG_T 	log                                 			*/
/*                  (I)  FH310_LOG_T 	logType                                         */
/*                                                                                      */
/****************************************************************************************/
void logInterface_WriteLogToSpiFlash(FH310_LOG_T log, LogType logType)
{
	vTaskSuspendAll();
	logMgr_WriteLogToSpiFlash(log, logType);
	xTaskResumeAll();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logInterface_WriteSpO2Log		                                  	*/
/*                                                                                      */
/* Details		: 	Write a SpO2 log interface										    */
/*                                                                                      */
/* Arguments 	: 	(I)  FH310_LOG_T 	log                                 			*/
/*                                                                                      */
/****************************************************************************************/
void logInterface_WriteSpO2Log(FH310_LOG_T log)
{
	if(!xQueueSend(logQueueRecive, &log, QUEUE_WAITING))
	{
		DEBUGOUT("Failed to send item to queue within 500ms");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logInterface_SendLogRequestEvent                                  	*/
/*                                                                                      */
/* Details		: 	Send a Log Request Event									     	*/
/*                                                                                      */
/* Arguments 	: 	(I)  LOG_REQUEST_t event	                            			*/
/*                                                                                      */
/* Using Global Data : (I) LogRequestQueue				                              	*/
/*                                                                                      */
/****************************************************************************************/
void logInterface_SendLogRequestEvent(LOG_REQUEST_t event)
{
	if(!xQueueSend(LogRequestQueue, &event, QUEUE_WAITING))
	{
		DEBUGOUT("Failed to send item to queue within 500ms");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logInterface_ReceiveLogForCBX	                                  	*/
/*                                                                                      */
/* Details		: 	Receive Log For CBX											     	*/
/*                                                                                      */
/* Arguments 	: 	(I)  LOG_REQUEST_t request                               			*/
/* 					(I)  FH310_LOG_T *stlog		                               			*/
/* 					(I)  uint8_t *num			                               			*/
/*                                                                                      */
/* ReturnValue : GetLogResult                                                           */
/*                                                                                      */
/* Using Global Data : (I) LogRequestQueue				                              	*/
/*                     (I) logQueueSendForCBX                                           */
/****************************************************************************************/
GetLogResult logInterface_ReceiveLogForCBX(LOG_REQUEST_t request, FH310_LOG_T *stlog, uint8_t *num)
{
	if(!xQueueSend(LogRequestQueue, &request, QUEUE_WAITING))
	{
		DEBUGOUT("Failed to send ReceiveLogForGUI within 500ms");
		return eTimeout;
	}

	if(!xQueueReceive(logQueueSendForCBX, stlog, QUEUE_WAITING))
	{
		DEBUGOUT("Failed to Receive ReceiveLogForGUI within 500ms");
		return eTimeout;
	}

	uint8_t logCout = 0;
	while(logCout < MAX_LOG_GUI_RQ)
	{
		if (stlog[logCout].ID == eNoneLog)
		{
			break;
		}
		logCout++;
	}
	*num = logCout;
	return eSuccess;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logInterface_ReceiveLogForGUI	                                  	*/
/*                                                                                      */
/* Details		: 	Receive Log For GUI											     	*/
/*                                                                                      */
/* Arguments 	: 	(I)  LOG_REQUEST_t request                               			*/
/* 					(I)  FH310_LOG_T *stlog		                               			*/
/* 					(I)  uint8_t *num			                               			*/
/*                                                                                      */
/* ReturnValue : GetLogResult                                                           */
/*                                                                                      */
/* Using Global Data : (I) LogRequestQueue				                              	*/
/*                     (I) logQueueSendForGUI                                           */
/****************************************************************************************/
GetLogResult logInterface_ReceiveLogForGUI(LOG_REQUEST_t request, FH310_LOG_T *stlog, uint8_t *num)
{
	if(!xQueueSend(LogRequestQueue, &request, QUEUE_WAITING))
	{
		DEBUGOUT("Failed to send ReceiveLogForGUI within 500ms");
		return eTimeout;
	}

	if(!xQueueReceive(logQueueSendForGUI, stlog, QUEUE_WAITING))
	{
		DEBUGOUT("Failed to Receive ReceiveLogForGUI within 500ms");
		return eTimeout;
	}

	uint8_t logCout = 0;
	while(logCout < MAX_LOG_GUI_RQ)
	{
		if (stlog[logCout].ID == eNoneLog)
		{
			break;
		}
		logCout++;
	}
	*num = logCout;
	return eSuccess;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logInterface_ReceiveLogForUSB 	                                  	*/
/*                                                                                      */
/* Details		: 	Receive Log For USB 										     	*/
/*                                                                                      */
/* Arguments 	: 	(I)  LOG_REQUEST_t request                               			*/
/* 					(I)  FH310_LOG_T *stlog		                               			*/
/* 					(I)  uint8_t *num			                               			*/
/*                                                                                      */
/* ReturnValue : GetLogResult                                                           */
/*                                                                                      */
/* Using Global Data : (I) LogRequestQueue				                              	*/
/*                     (I) logQueueSendForUSB                                           */
/****************************************************************************************/
GetLogResult logInterface_ReceiveLogForUSB(LOG_REQUEST_t request, FH310_LOG_T *stlog, uint8_t *num)
{
	if(!xQueueSend(LogRequestQueue, &request, QUEUE_WAITING))
	{
		DEBUGOUT("Failed to send ReceiveLogForGUI within 500ms");
		return eTimeout;
	}

	if(!xQueueReceive(logQueueSendForUSB, stlog, QUEUE_WAITING))
	{
		DEBUGOUT("Failed to Receive ReceiveLogForGUI within 500ms");
		return eTimeout;
	}

	uint8_t logCout = 0;
	while(logCout < MAX_LOG_USB_RQ)
	{
		if (stlog[logCout].ID == eNoneLog)
		{
			break;
		}
		logCout++;
	}
	*num = logCout;
	return eSuccess;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logInterface_ReceiveLogForAnypal                                  	*/
/*                                                                                      */
/* Details		: 	Receive Log For Anypal 										     	*/
/*                                                                                      */
/* Arguments 	: 	(I)  LOG_REQUEST_t request                               			*/
/* 					(I)  FH310_LOG_T *stlog		                               			*/
/* 					(I)  uint8_t *num			                               			*/
/*                                                                                      */
/* ReturnValue : GetLogResult                                                           */
/*                                                                                      */
/* Using Global Data : (I) LogRequestQueue				                              	*/
/*                     (I) logQueueSendForAnypal                                        */
/****************************************************************************************/
GetLogResult logInterface_ReceiveLogForAnypal(LOG_REQUEST_t request, FH310_LOG_T *stlog, uint8_t *num)
{
	if(!xQueueSend(LogRequestQueue, &request, QUEUE_WAITING))
	{
		DEBUGOUT("Failed to send ReceiveLogForGUI within 500ms");
		return eTimeout;
	}

	if(!xQueueReceive(logQueueSendForAnypal, stlog, QUEUE_WAITING))
	{
		DEBUGOUT("Failed to Receive ReceiveLogForGUI within 500ms");
		return eTimeout;
	}

	uint8_t logCout = 0;
	while(logCout < MAX_LOG_ATP_RQ)
	{
		if (stlog[logCout].ID == eNoneLog)
		{
			break;
		}
		logCout++;
	}
	*num = logCout;
	return eSuccess;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-logInterface_WriteMacineLog							                           	    */
/*                                                                                      */
/* Details  -Write log data by logId													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) LogId ID : log id						 							*/
/*                                                                                      */
/*             (I) uint8_t flowRateST													*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) logQueueRecive				                              	*/
/*                     (I) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
void logInterface_WriteMacineLog(uint8_t flowRateST, LogId ID)
{
	FH310_LOG_T log;
	log.ID = ID;
	log.flowRateSetting = flowRateST;
	log.YYYY = 0;
	log.MM = 0;
	log.DD = 0;
	log.hh = 0;
	log.min = 0;
	log.ss = 0;
	log.SpO2 = 0;
	log.pulse = 0;

	if(!xQueueSend(logQueueRecive, &log, QUEUE_WAITING))
	{
		DEBUGOUT("Failed to send item to queue within 500ms");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logInterface_DeleteLog                                        		*/
/*                                                                                      */
/* Details		: 	Delete Log 															*/
/*                                                                                      */
/* ReturnValue 	: none	                                                         		*/
/*                                                                                      */
/* Using Global Data : (I) LogRequestID id		          		                    	*/
/*                                                                                      */
/****************************************************************************************/
void logInterface_DeleteLog(LogRequestID id)
{
	LOG_REQUEST_t getlog;
	getlog.id = id;
	if(!xQueueSend(LogRequestQueue, &getlog, QUEUE_WAITING))
	{
		DEBUGOUT("Failed to send item to queue within 500ms");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logInterface_GetNumberOfLog                                     	*/
/*                                                                                      */
/* Details		: 	get number of the fh310 log											*/
/*                                                                                      */
/* ReturnValue 	: uint32_t                                                         		*/
/*                                                                                      */
/* Using Global Data : (I) LogType logtype		                        			  	*/
/*                                                                                      */
/****************************************************************************************/
uint32_t logInterface_GetNumberOfLog(LogType logtype)
{

	vTaskSuspendAll();
	uint32_t numLog = logMgr_GetNumberOfLog(logtype);
	xTaskResumeAll();
	return numLog;
}




