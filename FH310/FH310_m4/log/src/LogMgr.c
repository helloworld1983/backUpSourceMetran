/********************************************************************/
/*                                                                  */
/* File Name    : LogMrg.c                                    		*/
/*                                                                  */
/* General      : manager of FH310 log			    				*/
/*                                                                  */
/* Product Name  : FH-310                                           */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/20/10     Linh Nguyen(MV)  	    new file        */
/* #0001       2017/09/01     Linh Nguyen	  	    modify          */
/*  Improve date of log handle										*/
/* #0002       2017/09/05     Linh Nguyen	  	    modify          */
/*  Improve bluetooth transfer mode									*/
/*  - Add the logMgr_CopyAllLogToExtRam function					*/
/*  - ADD MAX_COPY_LOG define										*/
/*  - Change the recording log algorithm							*/
/*  - Remove unsuse function										*/
/* #0003       2017/09/12     Linh Nguyen	  	    modify          */
/*  Using new macro for coding rule									*/
/* #0004       2017/09/19     Linh Nguyen	  	    modify          */
/*  Remove unuse define ID											*/
/*  Change write log design											*/
/*  Change get number of log design									*/
/*  Change send log to another tast design							*/
/*  Change get log desin 											*/
/*  Change InitLogMemory design										*/
/* #0005      2017/09/19     Linh Nguyen	  	    modify          */
/*  Clear number of saved logs when clear log						*/
/*  Fix the wrong flow rate of usage log for anypal					*/
/*  		  2017/10/07      Linh Nguyen(MV)  	    modify	    	*/
/* #0006 Fix init log memmory function for bug #1961				*/
/* #0007 Add WDT feed when init log memory(it take 1.5s when init	*/
/*		 log memory from empty case)								*/
/*  		  2017/10/28      Linh Nguyen(MV)  	    modify	    	*/
/* #0008 Do not save the log with log ID is 0xff (bug #1991)		*/
/*  		  2017/11/20      Linh Nguyen(MV)  	    modify	    	*/
/* #0009 Add {} (coverity)											*/
/********************************************************************/
/* include system files */

#include "LogMgr.h"
#include "board.h"
#include "spifilib_dev.h"
#include "spifilib_api.h"
#include "SPIMgr.h"
#include "setting.h"
#include "CBX02Handler.h"
#include "string.h"
#include "AnypalHandler.h"
#include "GuiInterface.h"
#include "LogInterface.h"
#include "BluetoothInterface.h"
/* constants, macro definition */


/* definitions of structures */

#ifdef DEBUG
#define DEBUG_LOG_EN
#endif

#ifdef DEBUG_LOG_EN
#define DEBUG_LOG(...) printf(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif

#define MAX_COPY_LOG 256
#define LOG_TASK_DELAY 50 / portTICK_PERIOD_MS

/*static global variable*/
static LOG_INFO_T gs_LogInfo[eNumberOfLogType];
extern SPIFI_HANDLE_T *pSpifi;

extern QueueHandle_t logQueueSendForGUI;
extern QueueHandle_t logQueueSendForUSB;
extern QueueHandle_t logQueueSendForAnypal;
extern QueueHandle_t logQueueSendForCBX;

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_GetNLog			                                        	*/
/*                                                                                      */
/* Details		: 	get n FH310 logs from spi flash									*/
/*                                                                                      */
/* Arguments 	: 	(O)  FH310_LOG_T* 	stLog                                  			*/
/*					(I)	 uint32_t 		address											*/
/* 																						*/
/*  ReturnValue :															*/
/*                                                                                      */
/* Using Global Data : (I) gs_LogInfo			                              	*/
/*                                                                                      */
/****************************************************************************************/
static void logMgr_GetNLog(FH310_LOG_T* stLog, uint32_t beginLog, LogType logType, uint32_t numberOfLog, LOG_INFO_T *logInfo)
{
	//check valid of number of get log request
	if((beginLog + numberOfLog -1) > logMgr_GetNumberOfLog(logType))
	{
		numberOfLog = logMgr_GetNumberOfLog(logType) - beginLog + 1;
	}
	uint32_t addressOfBeginLog;
	//number of log from current log to begin log.
	uint32_t numLogTWBeginLog = logMgr_GetNumberOfLog(logType) - beginLog;
	//number of log from current address to first address.
	uint32_t numLogTWFirstAddLog = (logInfo[logType].currentAdd - logInfo[logType].firstAdd)/LOG_LENGTH;

	if(numLogTWBeginLog > numLogTWFirstAddLog)
	{
		//case begin log was save in higher block than current address
		//number of log from begin log to last address
		uint32_t remainLog = logMgr_GetNumberOfLog(logType) - numLogTWFirstAddLog - beginLog +1;
		addressOfBeginLog = gs_LogInfo[logType].lastAdd - (remainLog*LOG_LENGTH);
		if(remainLog> numberOfLog)
		{
			remainLog = numberOfLog;
			spiMgr_spifiRead(pSpifi, addressOfBeginLog, (uint32_t*)stLog, remainLog*LOG_LENGTH);
		}
		else
		{
			//half part of get logs were saved in last block and rest were save in first block
			spiMgr_spifiRead(pSpifi, addressOfBeginLog, (uint32_t*)stLog, remainLog*LOG_LENGTH);
			spiMgr_spifiRead(pSpifi, gs_LogInfo[logType].firstAdd, (uint32_t*)&stLog[remainLog], (numberOfLog-remainLog)*LOG_LENGTH);
		}
	}
	else
	{
		//case begin log was save in lower block than current address
		addressOfBeginLog = logInfo[logType].currentAdd - ((logMgr_GetNumberOfLog(logType) - beginLog+1)*LOG_LENGTH);
		spiMgr_spifiRead(pSpifi, addressOfBeginLog, (uint32_t*)stLog, numberOfLog*LOG_LENGTH);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: logMgr_CopyAllLogToExtRam	            		                  		*/
/*                                                                                      */
/* Details:  copy machine, CBX and SpO2/Pluse to external ram							*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint8_t		                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local variables   : NONE			                                                  	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
static NUM_OF_LOG_t logMgr_CopyAllLogToExtRam(void)
{
	FH310_LOG_T log[MAX_COPY_LOG];
	for(int i= eFirstLogTypeID; i< eNumberOfLogType; i++)
	{
		uint16_t logCout=0;
		uint16_t NoLogGet;
		uint16_t NoLog = logMgr_GetNumberOfLog(i);
		while(logCout < NoLog)
		{
			if(NoLog - logCout > MAX_COPY_LOG)
			{
				NoLogGet = MAX_COPY_LOG;
			}
			else
			{
				NoLogGet = NoLog - logCout;
			}
			logMgr_GetNLog(log, logCout +1, i, NoLogGet, gs_LogInfo);
			BTInterface_CopyLogToEXTRam(logCout, log, NoLog, i);
			logCout += NoLogGet;
		}
	}

	NUM_OF_LOG_t result;
	result.NoMachineLog = logMgr_GetNumberOfLog(eMachineLog);
	result.NoCBXLog = logMgr_GetNumberOfLog(eServerLog);
	result.NoATPLog = logMgr_GetNumberOfLog(eAnypalLog);
	return result;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_GetNumberOfLog	                                        	*/
/*                                                                                      */
/* Details		: 	get number of the fh310 log											*/
/*                                                                                      */
/* ReturnValue 	: uint32_t                                                         		*/
/*                                                                                      */
/* Using Global Data : (I) gs_LogInfo			                              	*/
/*                                                                                      */
/****************************************************************************************/
uint32_t logMgr_GetNumberOfLog(LogType logType)
{
	if (gs_LogInfo[logType].numLogSaved > gs_LogInfo[logType].maxLog)
	{
		return gs_LogInfo[logType].maxLog;
	}
	else
	{
		return gs_LogInfo[logType].numLogSaved;
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_Delete4096OldestLogs		                                    */
/*                                                                                      */
/* Details		: 	Delete 4096 Oldes 	tLogs									     	*/
/*                                                                                      */
/* Arguments : (I)  LogType 		logType	       		                               	*/
/*                                                                                      */
/****************************************************************************************/
static uint32_t logMgr_Delete4096OldestLogs(LogType logType)
{
	uint32_t nextBlock = spifiGetBlockFromAddr(pSpifi, gs_LogInfo[logType].currentAdd - LOG_LENGTH) +1;
	if (nextBlock > gs_LogInfo[logType].lastBlock)
	{
		nextBlock = gs_LogInfo[logType].firstBlock;
	}
	spiMgr_spifiErase(pSpifi, nextBlock,1);
	DEBUGOUT("Next block\n");
	return spifiGetAddrFromBlock(pSpifi, nextBlock);
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_ClearLog					                                    */
/*                                                                                      */
/* Details		: 	Clear all log base on log type		  						     	*/
/*                                                                                      */
/* Arguments : (I)  LogType 		logType	       		                               	*/
/*                                                                                      */
/****************************************************************************************/
void logMgr_ClearLog(LogType logType)
{
	spiMgr_spifiErase(pSpifi, gs_LogInfo[logType].firstBlock, gs_LogInfo[logType].numBlock);
	gs_LogInfo[logType].currentAdd = gs_LogInfo[logType].firstAdd;
	gs_LogInfo[logType].numLogSaved =0;
	if (logType == eUsageLog)
	{
		guiInterface_SetNumberOfLogStored(0);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_WriteLogToSpiFlash		                                  	*/
/*                                                                                      */
/* Details		: 	Write a log to SPI Flash									     	*/
/*                                                                                      */
/* Arguments 	: 	(I)  FH310_LOG_T 	log                                 			*/
/*                  (I)  LogType 		logType                                         */
/*                                                                                      */
/* Using Global Data : (I) gs_LogInfo			                              	*/
/*                                                                                      */
/****************************************************************************************/
void logMgr_WriteLogToSpiFlash(FH310_LOG_T log, LogType logType)
{
	log.header = LOG_HEADER;	
	if(logType == eAnypalLog)
	{
		log.ID = eAnypalLogId;
	}

	if(log.ID == eNoneLog)
	{
		return;
	}

	spiMgr_spifiProgram(pSpifi,gs_LogInfo[logType].currentAdd,(uint32_t*)&log, LOG_LENGTH);
	if ((logType == eUsageLog) && (logMgr_GetNumberOfLog(eUsageLog) == ANYPAL_USAGE_MAX_LOG))
	{
		uint32_t numLog = guiInterface_GetNumberOfLogStored();
		if(numLog >0)
		{
			guiInterface_SetNumberOfLogStored(numLog -1);
		}
	}

	gs_LogInfo[logType].currentAdd += LOG_LENGTH;
	gs_LogInfo[logType].numLogSaved++;
	if((gs_LogInfo[logType].currentAdd% BLOCK_SIZE) == 0)
	{
		gs_LogInfo[logType].currentAdd = logMgr_Delete4096OldestLogs(logType);
		if(gs_LogInfo[logType].numLogSaved == (gs_LogInfo[logType].numBlock * LOGS_1_BLOCK))
		{
			gs_LogInfo[logType].numLogSaved -= LOGS_1_BLOCK;
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_RecordLog		                                        	*/
/*                                                                                      */
/* Details		: 	Record a log 												     	*/
/*                                                                                      */
/* Arguments 	: 	(I)  FH310_LOG_T 	log		                                  		*/
/*                  (I)  LogType 		logType                                         */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void logMgr_RecordLog(FH310_LOG_T log, LogType logType)
{
	static FH310_LOG_T logRecord;
	static bool isFirstTime = true;
	static bool isAnypalInsert = false; //=get anypal state

	if(isFirstTime == true)
	{
		isFirstTime = false;
		logRecord.ID = eNoneLog;
	}

	switch (logType)
	{
	case eMachineLog:
	case eServerLog:
	case eAnypalLog:
		logInterface_WriteLogToSpiFlash(log, logType);
		break;

	case eUsageLog:
	{
		//sub switch
		switch (log.ID)
		{
		case eOperationStartLogId:
			logRecord = log;
			break;

		case eChangingFlowRateLogId:
		{
			logInterface_WriteLogToSpiFlash(logRecord, logType);
			uint8_t oldFLR = logRecord.flowRateSetting;
			logRecord = log;
			log.flowRateSetting = oldFLR;
			logInterface_WriteLogToSpiFlash(log, logType);
			break;
		}
		case eOperationStopLogId:
			logInterface_WriteLogToSpiFlash(logRecord, logType);
			logInterface_WriteLogToSpiFlash(log, logType);
			logRecord.ID = eNoneLog;
			break;

		case eAnypalInsertLogId:
			isAnypalInsert = true;
			if (logRecord.ID != eNoneLog)
			{
				logInterface_WriteLogToSpiFlash(logRecord, logType);
				logRecord = log;
				logRecord.ID = eChangingFlowRateLogId;
				logInterface_WriteLogToSpiFlash(logRecord, logType);
			}
			break;

		case eAnypalPutOutLogId:
			isAnypalInsert = false;
			break;

		case eChangeHourLogId:
			if((isAnypalInsert == true)&&(logRecord.ID != eNoneLog))
			{
				logInterface_WriteLogToSpiFlash(logRecord, logType);
				logRecord = log;
				logRecord.ID = eChangingFlowRateLogId;
				logInterface_WriteLogToSpiFlash(logRecord, logType);
			}
			break;

		case eChangeDayLogId:
			if((isAnypalInsert == false)&&(logRecord.ID != eNoneLog))
			{
				logInterface_WriteLogToSpiFlash(logRecord, logType);
				logRecord = log;
				logRecord.ID = eChangingFlowRateLogId;
				logInterface_WriteLogToSpiFlash(logRecord, logType);
			}
			break;
		default:
			break;
		}
		break;
	}
	//end sub switch
	default:
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_InitLogMemory	                                        	*/
/*                                                                                      */
/* Details		: 	init memory for FH310 log											*/
/*                                                                                      */
/* ReturnValue : uint32_t                                                         		*/
/*                                                                                      */
/* Using Global Data : (I) gs_LogInfo			                              	*/
/*                                                                                      */
/****************************************************************************************/
void logMgr_InitLogMemory(void)
{
	gs_LogInfo[eMachineLog].firstAdd = FISRT_ADDRESS_FOR_MACHINE_LOG;
	gs_LogInfo[eServerLog].firstAdd = FISRT_ADDRESS_FOR_SERVER_LOG;
	gs_LogInfo[eAnypalLog].firstAdd = FISRT_ADDRESS_FOR_ANYPAL_LOG;
	gs_LogInfo[eUsageLog].firstAdd = FISRT_ADDRESS_FOR_USAGE_LOG;

	gs_LogInfo[eMachineLog].firstBlock = FISRT_BLOCK_FOR_MACHINE_LOG;
	gs_LogInfo[eServerLog].firstBlock = FISRT_BLOCK_FOR_SERVER_LOG;
	gs_LogInfo[eAnypalLog].firstBlock = FISRT_BLOCK_FOR_ANYPAL_LOG;
	gs_LogInfo[eUsageLog].firstBlock = FISRT_BLOCK_FOR_USAGE_LOG;

	gs_LogInfo[eMachineLog].numBlock = NUMBER_OF_BLOCK_FOR_MACHINE_LOG;
	gs_LogInfo[eServerLog].numBlock = NUMBER_OF_BLOCK_FOR_SERVER_LOG;
	gs_LogInfo[eAnypalLog].numBlock = NUMBER_OF_BLOCK_FOR_ANYPAL_LOG;
	gs_LogInfo[eUsageLog].numBlock = NUMBER_OF_BLOCK_FOR_USAGE_LOG;

	gs_LogInfo[eMachineLog].lastBlock 	= LAST_BLOCK_FOR_MACHINE_LOG;
	gs_LogInfo[eServerLog].lastBlock 	= LAST_BLOCK_FOR_SERVER_LOG;
	gs_LogInfo[eAnypalLog].lastBlock 	= LAST_BLOCK_FOR_ANYPAL_LOG;
	gs_LogInfo[eUsageLog].lastBlock 	= LAST_BLOCK_FOR_USAGE_LOG;

	gs_LogInfo[eMachineLog].maxLog = MACHINE_MAX_LOG;
	gs_LogInfo[eServerLog].maxLog = CBX_USAGE_MAX_LOG;
	gs_LogInfo[eAnypalLog].maxLog = ANYPAL_MAX_LOG;
	gs_LogInfo[eUsageLog].maxLog = ANYPAL_USAGE_MAX_LOG;

	gs_LogInfo[eMachineLog].lastAdd = LAST_ADDRESS_FOR_MACHINE_LOG;
	gs_LogInfo[eServerLog].lastAdd = LAST_ADDRESS_FOR_SERVER_LOG;
	gs_LogInfo[eAnypalLog].lastAdd = LAST_ADDRESS_FOR_ANYPAL_LOG;
	gs_LogInfo[eUsageLog].lastAdd = LAST_ADDRESS_FOR_USAGE_LOG;

	guiInterface_WWDTFeed(LPC_WWDT);

	for(int i= eFirstLogTypeID; i< eNumberOfLogType; i++)
	{

		gs_LogInfo[i].numLogSaved = 0;
		gs_LogInfo[i].currentAdd = gs_LogInfo[i].firstAdd;
		FH310_LOG_T log;
		for(int j= gs_LogInfo[i].firstBlock; j <= gs_LogInfo[i].lastBlock; j++)
		{
			const uint32_t firstadd = spifiGetAddrFromBlock(pSpifi, j);
			log.header =0;
			//get last log of this block
			spiMgr_spifiRead(pSpifi, firstadd + BLOCK_SIZE - LOG_LENGTH, (uint32_t*)&log, LOG_LENGTH);
			// If the last log is not empty, add 4096 to number Of log
			if(log.header != LOG_HEADER)
			{
				uint32_t currentAdd = firstadd;
				for (int k= 0; k < LOGS_1_BLOCK;k++)
				{
					log.header =0;
					spiMgr_spifiRead(pSpifi, currentAdd, (uint32_t*)&log, LOG_LENGTH);
					if (log.header != LOG_HEADER)
					{
						spiMgr_spifiRead(pSpifi, gs_LogInfo[i].currentAdd, (uint32_t*)&log, LOG_LENGTH);
						if(log.header == LOG_HEADER)
						{
							gs_LogInfo[i].currentAdd = currentAdd;
						}
						break;
					}
					else
					{
						currentAdd += LOG_LENGTH;
						gs_LogInfo[i].numLogSaved++;
						gs_LogInfo[i].currentAdd = currentAdd;
					}
				}

			}
			else
			{
				gs_LogInfo[i].numLogSaved += LOGS_1_BLOCK;
			}
		}
		if(gs_LogInfo[i].numLogSaved ==0 || gs_LogInfo[i].numLogSaved == (LOGS_1_BLOCK*gs_LogInfo[i].numBlock))
		{
			spiMgr_spifiErase(pSpifi, gs_LogInfo[i].firstBlock, gs_LogInfo[i].numBlock);
			gs_LogInfo[i].currentAdd = gs_LogInfo[i].firstAdd;
			gs_LogInfo[i].numLogSaved = 0;
			if(i==eUsageLog)
			{
				guiInterface_SetNumberOfLogStored(0);
			}
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_SendLogToGUI				 									*/
/*                                                                                      */
/* Details:         send FH310 logs to GUI task					 						*/
/*                                                                                      */
/* Arguments : (I)  FH310_LOG_T *log			        								*/
/*                                                                                      */
/****************************************************************************************/
static void logMgr_SendLogToGUI(const FH310_LOG_T *log)
{
	if(!xQueueSend(logQueueSendForGUI, log, QUEUE_WAITING*2))
	{
		DEBUGOUT("Failed to send item to queue within QUEUE_WAITING 1000ms");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_SendLogToUSB				 									*/
/*                                                                                      */
/* Details:         send FH310 logs to USB task					 						*/
/*                                                                                      */
/* Arguments : (I)  FH310_LOG_T *log			        								*/
/*                                                                                      */
/****************************************************************************************/
static void logMgr_SendLogToUSB(const FH310_LOG_T *log)
{
	if(!xQueueSend(logQueueSendForUSB, log, QUEUE_WAITING*2))
	{
		DEBUGOUT("Failed to send item to queue within QUEUE_WAITING 1000ms");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_SendLogToCBX				 									*/
/*                                                                                      */
/* Details:         send FH310 logs to CBX task					 						*/
/*                                                                                      */
/* Arguments : (I)  FH310_LOG_T *log		        									*/
/*                                                                                      */
/****************************************************************************************/
static void logMgr_SendLogToCBX(const FH310_LOG_T *log)
{
	if(!xQueueSend(logQueueSendForCBX, log, QUEUE_WAITING))
	{
		DEBUGOUT("Failed to send item to queue within QUEUE_WAITINGms");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_SendLogToAnypal				 							*/
/*                                                                                      */
/* Details:         send 10 FH310 logs to Anypal task					 				*/
/*                                                                                      */
/* Arguments : (I)  FH310_LOG_T *log		        									*/
/*                                                                                      */
/****************************************************************************************/
static void logMgr_SendLogToAnypal(const FH310_LOG_T *log) //isOperationSWPushed
{
	if(!xQueueSend(logQueueSendForAnypal, log, QUEUE_WAITING))
	{
		DEBUGOUT("Failed to send item to queue within QUEUE_WAITINGms");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_SendAllLog	                                        		*/
/*                                                                                      */
/* Details		: 	Send all of FH310 log to Gui										*/
/*                                                                                      */
/* ReturnValue : None	                                                         		*/
/*                                                                                      */
/* Using Global Data : (I) None							                              	*/
/*                                                                                      */
/****************************************************************************************/
void logMgr_RequsetLogEvent(LOG_REQUEST_t LogRequest)
{
	switch (LogRequest.id)
	{
	case eGuiGetMachineLogId:
	{
		FH310_LOG_T logGet[MAX_LOG_GUI_RQ];
		logMgr_GetNLog(logGet, LogRequest.beginLog, eMachineLog, LogRequest.numberOfLog, gs_LogInfo);
		if (LogRequest.numberOfLog < MAX_LOG_GUI_RQ)
		{
			logGet[LogRequest.numberOfLog].ID = eNoneLog;
		}
		logMgr_SendLogToGUI(logGet);
		break;
	}
	case eUSBGetServerLogId:
	{
		FH310_LOG_T logGet[MAX_LOG_USB_RQ];
		logMgr_GetNLog(logGet, LogRequest.beginLog, eServerLog, LogRequest.numberOfLog, gs_LogInfo);
		if (LogRequest.numberOfLog < MAX_LOG_USB_RQ)
		{
			logGet[LogRequest.numberOfLog].ID = eNoneLog;
		}
		logMgr_SendLogToUSB(logGet);
		break;
	}
	case eUSBGetUsageLogId:
	{
		FH310_LOG_T logGet[MAX_LOG_USB_RQ];
		logMgr_GetNLog(logGet, LogRequest.beginLog, eUsageLog, LogRequest.numberOfLog, gs_LogInfo);
		if (LogRequest.numberOfLog < MAX_LOG_USB_RQ)
		{
			logGet[LogRequest.numberOfLog].ID = eNoneLog;
		}
		logMgr_SendLogToUSB(logGet);
		break;
	}
	case eUSBGetAnypalLogId:
	{
		FH310_LOG_T logGet[MAX_LOG_USB_RQ];
		logMgr_GetNLog(logGet, LogRequest.beginLog, eAnypalLog, LogRequest.numberOfLog, gs_LogInfo);
		if (LogRequest.numberOfLog < MAX_LOG_USB_RQ)
		{
			logGet[LogRequest.numberOfLog].ID = eNoneLog;
		}
		logMgr_SendLogToUSB(logGet);
		break;
	}
	case eUSBGetMachineLogId:
	{
		FH310_LOG_T logGet[MAX_LOG_ATP_RQ];
		logMgr_GetNLog(logGet, LogRequest.beginLog, eMachineLog, LogRequest.numberOfLog, gs_LogInfo);
		if (LogRequest.numberOfLog < MAX_LOG_ATP_RQ)
		{
			logGet[LogRequest.numberOfLog].ID = eNoneLog;
		}
		logMgr_SendLogToUSB(logGet);
		break;
	}
	case eAnypalGetUsageLogId:
	{
		FH310_LOG_T logGet[MAX_LOG_ATP_RQ];
		logMgr_GetNLog(logGet, LogRequest.beginLog, eUsageLog, LogRequest.numberOfLog, gs_LogInfo);
		if (LogRequest.numberOfLog < MAX_LOG_ATP_RQ)
		{
			logGet[LogRequest.numberOfLog].ID = eNoneLog;
		}
		logMgr_SendLogToAnypal(logGet);
		break;
	}
	case eAnypalGetAnypalLogId:
	{
		FH310_LOG_T logGet[MAX_LOG_ATP_RQ];
		logMgr_GetNLog(logGet, LogRequest.beginLog, eAnypalLog, LogRequest.numberOfLog, gs_LogInfo);
		if (LogRequest.numberOfLog < MAX_LOG_ATP_RQ)
		{
			logGet[LogRequest.numberOfLog].ID = eNoneLog;
		}
		logMgr_SendLogToAnypal(logGet);
		break;
	}
	case eCBXGetMachineLogId:
	{
		FH310_LOG_T logGet[MAX_LOG_CBX_RQ];
		logMgr_GetNLog(logGet, LogRequest.beginLog, eMachineLog, LogRequest.numberOfLog, gs_LogInfo);
		logMgr_SendLogToCBX(logGet);
		break;
	}
	case eCBXGetServerLogId:
	{
		FH310_LOG_T logGet[MAX_LOG_CBX_RQ];
		logMgr_GetNLog(logGet, LogRequest.beginLog, eServerLog, LogRequest.numberOfLog, gs_LogInfo);
		logMgr_SendLogToCBX(logGet);
		break;
	}
	case eCBXGetAnypalLogId:
	{
		FH310_LOG_T logGet[MAX_LOG_CBX_RQ];
		logMgr_GetNLog(logGet, LogRequest.beginLog, eAnypalLog, LogRequest.numberOfLog, gs_LogInfo);
		logMgr_SendLogToCBX(logGet);
		break;
	}
	case eDeleteMachineLogId:
	{
		logMgr_ClearLog(eMachineLog);
		break;
	}
	case eDeleteServerLogId:
	{
		logMgr_ClearLog(eServerLog);
		break;
	}
	case eDeleteAnypalLogId:
	{
		logMgr_ClearLog(eAnypalLog);
		break;
	}
	case eDeleteUsageLogId:
	{
		logMgr_ClearLog(eUsageLog);
		break;
	}
	case eDeleteAllLogid:
	{
		logMgr_ClearLog(eMachineLog);
		logMgr_ClearLog(eServerLog);
		logMgr_ClearLog(eAnypalLog);
		logMgr_ClearLog(eUsageLog);
		break;
	}
	case eCopyLogToEXTRamid:
	{
		BT_MSG_t mgs;
		mgs.id = eOnBluetoothTranferId;
		mgs.mgs = logMgr_CopyAllLogToExtRam();
		BTTask_SendMgs(mgs);
		break;
	}
	default:
		break;
	}
	return;
}
