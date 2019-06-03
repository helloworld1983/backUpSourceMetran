/********************************************************************/
/*                                                                  */
/* File Name    : LogTask.c		                                	*/
/*                                                                  */
/* General       : This module contain function for Create RTOS task*/
/* to control Log													*/
/*                                                                  */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable	                                        */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            April 18, 2016    Thao Ha(MV)  		new file    	*/
/*                                                                  */
/********************************************************************/
#include <TaskCommon.h>
#include "LogTask.h"
#include <DevTask.h>
#include <RTC.h>
#include "queue.h"
#include "Setting.h"
#include "string.h"


#define DEBUG_LOG_EN
#ifdef DEBUG_LOG_EN
#define DEBUG_LOG(...) printf(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif

QueueHandle_t logQueueReceive;
TaskHandle_t gs_logTaskHandle;
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logTask_InitLogTask					              				    */
/*                                                                                      */
/* Details:        Init Log task by create logQueueRecive, logQueueSend					*/
/*				logQueueSendForPCApp, logQueueSendPressureForPCApp, logQueueDelete	    */
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void logTask_InitLogTask()
{
    logQueueReceive = xQueueCreate(LOG_QUEUE_RECEIVE_LENGTH, sizeof(E_logQueueReceive));
    logMgr_InitLogMemory();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logTask_CreateCRC						              				*/
/*                                                                                      */
/* Details:        Create CRC for Portable Log											*/
/*                                                                                      */
/* Arguments:	(I) POR_LOG_T* log														*/
/*                                                                                      */
/* ReturnValue : unsigned char	                                                        */
/*                                                                                     	*/
/****************************************************************************************/
static unsigned char logTask_CreateCRC(POR_LOG_T* log)
{
    unsigned char data[LOG_LENGTH]={'\0'};
    memcpy(data,log,sizeof(POR_LOG_T));
    unsigned char sum = 0;
    for(int i = 0; i < (sizeof(POR_LOG_T)-2); i ++)
    {
        sum ^= data[i];
    }
    return sum;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logTask_CreateCRC						              				*/
/*                                                                                      */
/* Details:        Create CRC for Portable Pressure Log									*/
/*                                                                                      */
/* Arguments:	(I) POR_PRESS_LOG_T* pressLog											*/
/*                                                                                      */
/* ReturnValue : unsigned char	                                                        */
/*                                                                                     	*/
/****************************************************************************************/
static unsigned char logTask_CreatePressureLogCRC(POR_PRESS_LOG_T* pressLog)
{
    unsigned char data[LOG_PRESS_LENGTH]={'\0'};
    memcpy(data,pressLog,sizeof(POR_PRESS_LOG_T));
    unsigned char sum = 0;
    for(uint16_t i = 0; i < (sizeof(POR_PRESS_LOG_T)-2); i ++)
    {
        sum ^= data[i];
    }
    return sum;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	LogTask_Loop						              				    */
/*                                                                                      */
/* Details:        Log task loop function is call function for handle log function		*/
/* 					every 40ms															*/
/*                                                                                      */
/* Arguments:	(I) void *pvParameters													*/
/*                                                                                      */
/* ReturnValue : NONE			                                                        */
/*                                                                                     	*/
/****************************************************************************************/
static void LogTask_Loop(void *pvParameters)
{
    E_logQueueReceive receivedLogQueue;
    POR_LOG_T log;
    log.logCode = 0;
    POR_PRESS_LOG_T pressLog;
    DEV_EVENT_STRUCT deviceQueue;
    deviceQueue.id = eFromLogTask;
#ifdef CHECK_REMAINING_STACK_SIZE
    UBaseType_t uxHighWaterMark;
    uint32_t cnt = 0;
#endif
	Chip_SCU_PinMuxSet(1, 14, SCU_MODE_FUNC0);//new add
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 1, 7);//new add
    while(1)
    {

        if(xQueueReceive(logQueueReceive,&receivedLogQueue,0))
        {
            switch (receivedLogQueue.id)
            {
                case eRequestSendLogForPCApp:
                    DEBUG_LOG("Send all Log\n");
                    logMgr_SendAllLog();
                    break;

                case eRequestSendPressureLogForPCApp:
                    DEBUG_LOG("Send pressure Log\n");
                    logMgr_SendTankPressureLog();
                    vTaskDelay(DELAY_TICK_20MS);
                    logMgr_SendVacuumPressureLog();
                    vTaskDelay(DELAY_TICK_20MS);
                    logMgr_SendSupplyPressureLog();
                    break;

                case eRequestWriteLog:
                    DEBUG_LOG("write Log\n");
                    memcpy(&log.year,receivedLogQueue.data,sizeof(POR_LOG_T));
                    if(log.logCode != eFirsLogId)
                    {
                        Real_Time_Clock clock;
                        realtimeclock_gettime(&clock);
                        log.year = clock.year;
                        log.month = clock.month;
                        log.day = clock.mday;
                        log.hour = clock.hour;
                        log.minute = clock.min;
                        log.second = clock.sec;
                        log.CRC = logTask_CreateCRC(&log);
                        logMgr_RecordLog(log);
                        if (log.logCode == eBootloaderUpdateModeSetId)
                        {
                            keyPanel_PowerButtonToggle2S();
                        }
                        else if((log.logCode==ePowerLost)||(log.logCode==eResetPowerSwitching))
                        {
                            vTaskSuspend(gs_logTaskHandle);
                        }
                        else if((log.logCode == eLowO2ConcentrationLogId)||(log.logCode == eAbnormalO2ConcentrationErrorLogId))
                        {
                            pressLog.year = clock.year;
                            pressLog.month = clock.month;
                            pressLog.day = clock.mday;
                            pressLog.hour = clock.hour;
                            pressLog.minute = clock.min;
                            pressLog.second = clock.sec;
                            pressLog.logCode = eTankPressureLog;
                            setting_GetTankPressData((uint8_t*)&pressLog.PressData[0]);
                            log.CRC = logTask_CreatePressureLogCRC(&pressLog);
                            logMgr_RecordTankPressureLog(&pressLog);

                            setting_GetVacuumPressData(&pressLog.PressData[0]);
                            pressLog.logCode = eVacuumPressureLog;
                            log.CRC = logTask_CreatePressureLogCRC(&pressLog);
                            logMgr_RecordVacuumPressureLog(&pressLog);

                            setting_GetSupplyPressData((uint8_t*)&pressLog.PressData[0]);
                            pressLog.logCode = eSupplyPressureLog;
                            log.CRC = logTask_CreatePressureLogCRC(&pressLog);
                            logMgr_RecordSupplyPressureLog(&pressLog);
                        }
                    }
                    else
                    {
                        DEBUG_LOG("\nThere is a request to record first log \n");
                    }
                    break;
                case eRequestDeleteLog:
                    DEBUG_LOG("delete all Log\n");
                    logMgr_DeleteAllLog();
                    break;

                case eRequestDeletePressureLog:
                    DEBUG_LOG("delete pressure Log\n");
                    logMgr_DeleteAllPressureLog();
                    break;

                default:
                    DEBUG_LOG("\nUnexpected Log Queue ID \n");
                    break;
            }
        }
        deviceQueue.msgData.logMgs.numberOfLog = logMgr_GetNumberOfLog();
        if(!devTask_SendToDeviceQueue(deviceQueue,100))
        {
            DEBUG_TASK("Failed to send Item to deviceQueue from LogTask_Loop");
        }

#ifdef CHECK_REMAINING_STACK_SIZE
        uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
        if(cnt>=1000/LOG_TASK_DELAY_TIME)
        {
            DEBUGOUT("Log Task %d\n",uxHighWaterMark);
            cnt = 0;
        }
        else
        {
            cnt++;
        }
#endif

        vTaskDelay(LOG_TASK_DELAY_TICK);

    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logTask_CreateTask					              				    */
/*                                                                                      */
/* Details:        Create FREERTOS task name Log task									*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/* Local variables : None				                          						*/
/*                                                                                      */
/****************************************************************************************/
void logTask_CreateTask()
{
    xTaskCreate((TaskFunction_t)LogTask_Loop, "Log task", LOG_TASK_SIZE, (void *)1, TASK_PRIO_LOG_TASK, ( TaskHandle_t * ) &gs_logTaskHandle);
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logTask_SendToLogQueueReceive   	              				    */
/*                                                                                      */
/* Details:        Send data to Log Queue Receive										*/
/*                                                                                      */
/* Arguments:	E_logQueueType LogQueueType 									 		*/
/*              TickType_t xTicksToWait                                                 */
/*                                                                                      */
/* ReturnValue : true if it sent successfully                                           */
/*  			 false if it fails to sent                                              */
/*                                                                                      */
/****************************************************************************************/
bool logTask_SendToLogQueueReceive(E_logQueueReceive LogQueueData,TickType_t xTicksToWait)
{
    bool status = false;
    E_logQueueReceive logQueueDataSend = LogQueueData;
    //send Data to Device Queue
    if(!xQueueSend(logQueueReceive,&logQueueDataSend, xTicksToWait)){
        //return false if it fails to sent
        status = false;
    }else{
        //return true if it sent successfully
        status = true;
    }

    return status;
}

