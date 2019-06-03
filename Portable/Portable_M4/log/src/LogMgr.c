/********************************************************************/
/*                                                                  */
/* File Name    : LogMgr.c                                    		*/
/*                                                                  */
/* General      : Initialization Log function						*/
/*				  Record and send Log function						*/
/* 				  Delete Log function                               */
/*                 	 						                        */
/*                                                                  */
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
#include <EEPRom.h>
#include <LogMgr.h>
#include "spifilib_dev.h"
#include "SPIFMgr.h"
#include "string.h"
#include "USBTask.h"

#define DEBUG_LOGMGR_EN
#ifdef DEBUG_LOGMGR_EN
#define DEBUG_LOGMGR(...) printf(__VA_ARGS__)
#else
#define DEBUG_LOGMGR(...)
#endif
static uint32_t gs_CurrentAddress;  //save the current address
static uint32_t gs_NumLogSave;      //save the number of all current log

extern SPIFI_HANDLE_T *pSpifi;
#define MIN_YEAR 2000
#define MIN_MONTH 1
#define MAX_MONTH 12
#define MIN_DAY 1
#define MAX_DAY 31
#define MIN_HOUR 0
#define MAX_HOUR 23
#define MIN_MINUTE 0
#define MAX_MINUTE 59
#define MIN_SECOND 0
#define MAX_SECOND 59
#define MAX_NUMBER_OF_SAVED_LOG 1000    //the max number of log sending to PC at one times
#define NUMBER_OF_LOG_SENT_PER_QUEUE 10
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_CheckTimePressLogValid		                                */
/*                                                                                      */
/* Details		: 	Check whether the time in pressure log is eligible. if not, set to	*/
/*               default value                                                          */
/* Arguments : (I)  POR_PRESS_LOG_T  	log	       		                               	*/
/*             (O)  POR_PRESS_LOG_T  	log                                      		*/
/*                                                                                      */
/****************************************************************************************/
static void logMgr_CheckTimePressLogValid(POR_PRESS_LOG_T* log)
{
    if(log->year<MIN_YEAR)
    {
        log->year=MIN_YEAR;
    }
    if(log->month<MIN_MONTH || log->month>MAX_MONTH)
    {
        log->month=MIN_MONTH;
    }
    if(log->day<MIN_DAY || log->day>MAX_DAY)
    {
        log->day=MIN_DAY;
    }
    if(log->hour>MAX_HOUR)
    {
        log->day=MIN_HOUR;
    }
    if(log->minute>MAX_MINUTE)
    {
        log->day=MIN_MINUTE;
    }
    if(log->second>MAX_SECOND)
    {
        log->day=MIN_SECOND;
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_CheckTimeLogValid			                                */
/*                                                                                      */
/* Details		: 	Check whether the time in log is eligible. if not, set to default	*/
/*               	value	                                                            */
/* Arguments : (I)  POR_LOG_T  	log	       				                               	*/
/*             (O)  POR_LOG_T  	log			                                      		*/
/*                                                                                      */
/****************************************************************************************/
static void logMgr_CheckTimeLogValid(POR_LOG_T* log)
{
    if(log->year<MIN_YEAR)
    {
        log->year=MIN_YEAR;
    }
    if(log->month<MIN_MONTH || log->month>MAX_MONTH)
    {
        log->month=MIN_MONTH;
    }
    if(log->day<MIN_DAY || log->day>MAX_DAY)
    {
        log->day=MIN_DAY;
    }
    if(log->hour>MAX_HOUR)
    {
        log->day=MIN_HOUR;
    }
    if(log->minute>MAX_MINUTE)
    {
        log->day=MIN_MINUTE;
    }
    if(log->second>MAX_SECOND)
    {
        log->day=MIN_SECOND;
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_ConvertPressureLogToBinaryArray                              */
/*                                                                                      */
/* Details		: 	convert the Porable Pressure log to binary array			     	*/
/*                                                                                      */
/* Arguments : (I)  POR_PRESS_LOG_T  	log	       		                               	*/
/*             (I)  LogType 		logType                                      		*/
/*             (O)  uint8_t* 		logArray                                      		*/
/*                                                                                      */
/****************************************************************************************/
static void logMgr_ConvertPressureLogToBinaryArray(POR_PRESS_LOG_T* log, uint8_t* logArray)
{
    memset(logArray,'\0',LOG_LENGTH);
    logArray[0] = 0xAC;
    logMgr_CheckTimePressLogValid(log);
    logArray[1] = log->year;
    logArray[2] = log->year >> 8;
    logArray[3] = log->month;
    logArray[4] = log->day;
    logArray[5] = log->hour;
    logArray[6] = log->minute;
    logArray[7] = log->second;
    logArray[8] = log->logCode;
    logArray[9] = log->logCode>>8;
    for(int i = 0 ; i<300;i++)
    {
        logArray[10+i]=(uint8_t)log->PressData[i];
    }
    logArray[310] = log->CRC;
    logArray[311] = log->CRC >> 8;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_ConvertLogToBinaryArray                                      */
/*                                                                                      */
/* Details		: 	convert the Porable log to binary array						     	*/
/*                                                                                      */
/* Arguments : (I)  POR_LOG_T  	    log	       		                  		          	*/
/*             (I)  LogType 		logType                                      		*/
/*             (O)  uint8_t* 		logArray                                      		*/
/*                                                                                      */
/****************************************************************************************/
static void logMgr_ConvertLogToBinaryArray(POR_LOG_T log, uint8_t* logArray)
{
    memset(logArray,'\0',LOG_LENGTH);
    logArray[0] = 0xAB;
    logMgr_CheckTimeLogValid(&log);
    logArray[1] = log.year;
    logArray[2] = log.year >> 8;
    logArray[3] = log.month;
    logArray[4] = log.day;
    logArray[5] = log.hour;
    logArray[6] = log.minute;
    logArray[7] = log.second;
    logArray[8] = log.logCode;
    logArray[9] = log.logCode >> 8;
    logArray[10] = log.EventOrAlarm;
    logArray[11] = log.alarmType;
    logArray[12] = log.oldValue.mode;
    logArray[13] = log.oldValue.flowRate;
    logArray[14] = log.oldValue.triggerSensitivity;
    logArray[15] = log.newValue.mode;
    logArray[16] = log.newValue.flowRate;
    logArray[17] = log.newValue.triggerSensitivity;
    logArray[18] = log.CRC;
    logArray[19] = log.CRC >> 8;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_DeleteOneBlockLog                                   		    */
/*                                                                                      */
/* Details		: 	Delete one block of  Log in the SPIF						     	*/
/*                                                                                      */
/* Arguments : (I)  NONE				       		                  		          	*/
/*             (O)  uint32_t - current address			                                */
/*                                                                                      */
/****************************************************************************************/
static uint32_t logMgr_DeleteOneBlockLog()
{
    const uint8_t STEP_OF_NEXT_BLOCK = 1;
    const uint8_t NUMBER_OF_DELETE_BLOCK = 1;

    //get the next block = current block + STEP_OF_NEXT_BLOCK(1)
    uint32_t nextBlock = spifiGetBlockFromAddr(pSpifi, gs_CurrentAddress - LOG_LENGTH) + STEP_OF_NEXT_BLOCK;


    if (nextBlock > LAST_BLOCK_FOR_MACHINE_LOG )
    {
        //reset the next block when it is out of range
        nextBlock = FISRT_BLOCK_FOR_MACHINE_LOG;
    }//another case, do nothing

    DEBUG_LOGMGR("Delete block %d\n",nextBlock);
    spifMgr_spifiErase(pSpifi, nextBlock,NUMBER_OF_DELETE_BLOCK);

    return spifiGetAddrFromBlock(pSpifi, nextBlock);
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_WriteLogToSpiFlash		                                  	*/
/*                                                                                      */
/* Details		: 	Write a log to SPI Flash									     	*/
/*                                                                                      */
/* Arguments 	: 	(I)  POR_LOG_T	 	log                                 			*/
/*                                                                                      */
/* Using Global Data : (I) gs_CurrentAddress			                              	*/
/*                                                                                      */
/****************************************************************************************/
static void logMgr_WriteLogToSpiFlash(POR_LOG_T log)
{

    uint8_t logArray[LOG_LENGTH];
    //convert Log to Binary array sending to SPI flash
    logMgr_ConvertLogToBinaryArray(log, logArray);
    spifMgr_spifiProgram(pSpifi,gs_CurrentAddress,(uint32_t*)logArray, LOG_LENGTH);

    DEBUG_LOGMGR("WriteLogToSpiFlash_at Address = 0x%x Log Save = %d\n",gs_CurrentAddress,gs_NumLogSave);
    //update data gs_CurrentAddress and gs_NumLogSave for the next times
    gs_CurrentAddress += LOG_LENGTH;
    ++gs_NumLogSave;


    //check if block is full
    if(gs_CurrentAddress % BLOCK_SIZE == 0)
    {
        //current block is full, delete and write to the next block
        gs_CurrentAddress = logMgr_DeleteOneBlockLog();

        if(gs_NumLogSave == NUMBER_OF_BLOCK_FOR_MACHINE_LOG * LOGS_1_BLOCK)
        {
            //2 block is full, delete the first block, update gs_NumLogSave
            gs_NumLogSave -= LOGS_1_BLOCK;
        }
        else
        {
            //do nothing
        }
    }
    else
    {
        //block is not full, do nothing
    }

}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_GetNumberOfLog			                                  	*/
/*                                                                                      */
/* Details		: 	Get number of require log in the SPIF						     	*/
/*                                                                                      */
/* Arguments 	: 	(I) NONE				                                 			*/
/*                  (O) NONE                                                            */
/* ReturnValue 	: uint32_t                                                         		*/
/* Using Global Data : (I) gs_CurrentAddress			                              	*/
/*                                                                                      */
/****************************************************************************************/
uint32_t logMgr_GetNumberOfLog(void)
{
    uint32_t tempNumLogSave = 0;

    if(gs_NumLogSave >= MAX_NUMBER_OF_SAVED_LOG)
    {
        tempNumLogSave = MAX_NUMBER_OF_SAVED_LOG;
    }
    else
    {
        tempNumLogSave = gs_NumLogSave;
    }

    return tempNumLogSave;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_InitLogMemory	                                        	*/
/*                                                                                      */
/* Details		: 	init memory for Portable log										*/
/*                                                                                      */
/* ReturnValue : uint32_t                                                         		*/
/*                                                                                      */
/* Using Global Data : (I) gs_CurrentAddress			                              	*/
/*                                                                                      */
/****************************************************************************************/
void logMgr_InitLogMemory(void)
{
    const uint8_t NO_LOG = 0;
    uint32_t checkByte = 0;
    gs_CurrentAddress = FISRT_ADDRESS_FOR_MACHINE_LOG;
    gs_NumLogSave = 0;
    //check available log in each block
    for(int i = 0; i < NUMBER_OF_BLOCK_FOR_MACHINE_LOG; ++i)
    {
        //get the first address of the first block
        const uint32_t firstAdd = spifiGetAddrFromBlock(pSpifi, FISRT_BLOCK_FOR_MACHINE_LOG + i);
        checkByte = 0;
        //read header byte of the last log in this block
        spifMgr_spifiRead(pSpifi, firstAdd + BLOCK_SIZE - LOG_LENGTH, &checkByte, 1);

        if(MACHINE_LOG_HEADER != checkByte)
        {
            //this block hasn't full block
            uint32_t currentAdd = firstAdd;
            //search all block to find all log base on header byte of each log
            for(int k = 0; k < LOGS_1_BLOCK; ++k)
            {
                checkByte = 0;
                spifMgr_spifiRead(pSpifi, currentAdd, &checkByte, 1);
                //check header byte of log
                if(MACHINE_LOG_HEADER == checkByte)
                {
                    //this is a log, increase current address, update gs_CurrentAddress, gs_NumLogSave
                    currentAdd += LOG_LENGTH;
                    gs_CurrentAddress = currentAdd;
                    ++gs_NumLogSave;
                }
                else
                {
                    //check chechbyte of previous log, expect the first and the last address of block
                    if((currentAdd != FISRT_ADDRESS_FOR_MACHINE_LOG) && (currentAdd != LAST_ADDRESS_FOR_MACHINE_LOG))
                    {
                        checkByte = 0;
                        spifMgr_spifiRead(pSpifi, currentAdd - LOG_LENGTH, &checkByte, 1);
                        if(MACHINE_LOG_HEADER == checkByte)
                        {
                            gs_CurrentAddress = currentAdd;
                        }//another case, do nothing
                    }//another case, do nothing
                    //stop searching
                    break;
                }
            }
        }
        else
        {
            //this block has full block
            gs_NumLogSave += LOGS_1_BLOCK;
        }
    }

    //delete all page when it has no log
    if((NO_LOG == gs_NumLogSave) || (NUMBER_OF_BLOCK_FOR_MACHINE_LOG*LOGS_1_BLOCK == gs_NumLogSave))
    {
        spifMgr_spifiErase(pSpifi, FISRT_BLOCK_FOR_MACHINE_LOG, NUMBER_OF_BLOCK_FOR_MACHINE_LOG);
        gs_NumLogSave = 0;
        gs_CurrentAddress = FISRT_ADDRESS_FOR_MACHINE_LOG;
    }
    DEBUG_LOGMGR("Init-gs_NumLogSave = %d (0x%x byte)\n",gs_NumLogSave,gs_NumLogSave*LOG_LENGTH);
    DEBUG_LOGMGR("Init_gs_CurrentAddress = 0x%x\n",gs_CurrentAddress);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_RecordLog		                                        	*/
/*                                                                                      */
/* Details		: 	Record a log 												     	*/
/*                                                                                      */
/* Arguments 	: 	(I)  POR_LOG_T 	log			                                  		*/
/*                  (O)  NONE           	                                            */
/*                                                                                      */
/****************************************************************************************/
void logMgr_RecordLog(POR_LOG_T log)
{
//	for(int i = 0; i<100; i++)
//	{
		logMgr_WriteLogToSpiFlash(log);
//	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_GetNLog			                                        	*/
/*                                                                                      */
/* Details		: 	get 10 Portable logs from spi flash									*/
/*                                                                                      */
/* Arguments 	: 	(O)  POR_LOG_T* 	stLog                                  			*/
/*					(I)	 uint32_t 		address											*/
/* 					(I)  uint32_t		numberOfLog     								*/
/* 																						*/
/* ReturnValue :	GetLogResult														*/
/*                                                                                      */
/* Using Global Data : (I) gs_CurrentAddress			                              	*/
/*                                                                                      */
/****************************************************************************************/
void static logMgr_GetNLog(POR_LOG_T* stLog, uint32_t address, uint32_t numberOfLog)
{
    uint8_t logArray[numberOfLog*LOG_LENGTH];
    uint16_t *p_Year;
    uint16_t *p_logCode;

    //LOG module always send NUMBER_OF_LOG_SENT_PER_QUEUE(10) to PC app
    //if data sending is lower than NUMBER_OF_LOG_SENT_PER_QUEUE(10)
    //PC app ignores it base on logCode=eFirsLogId(0) or it fails to get data
    for (int i =0; i< NUMBER_OF_LOG_SENT_PER_QUEUE; i++)
    {
        stLog[i].logCode=eFirsLogId;
    }
    //read numberOfLog*LOG_LENGTH from SPIF
    spifMgr_spifiRead(pSpifi, (address - (numberOfLog*LOG_LENGTH)), (uint32_t*)logArray, numberOfLog*LOG_LENGTH);
    //store data above to POR_LOG_T* stLog
    for(int8_t i=(numberOfLog-1); i >= 0; i--)
    {
        p_Year = (uint16_t*)&logArray[(LOG_LENGTH*i)+1];
        stLog->year = *p_Year;
        stLog->month = logArray[(LOG_LENGTH*i)+3];
        stLog->day = logArray[(LOG_LENGTH*i)+4];
        stLog->hour = logArray[(LOG_LENGTH*i)+5];
        stLog->minute = logArray[(LOG_LENGTH*i)+6];
        stLog->second = logArray[(LOG_LENGTH*i)+7];
        p_logCode = (uint16_t*)&logArray[(LOG_LENGTH*i)+8];
        stLog->logCode = *p_logCode;
        stLog->EventOrAlarm = logArray[(LOG_LENGTH*i)+10];
        stLog->alarmType = logArray[(LOG_LENGTH*i)+11];
        stLog->oldValue.mode = logArray[(LOG_LENGTH*i)+12];
        stLog->oldValue.flowRate = logArray[(LOG_LENGTH*i)+13];
        stLog->oldValue.triggerSensitivity = logArray[(LOG_LENGTH*i)+14];
        stLog->newValue.mode = logArray[(LOG_LENGTH*i)+15];
        stLog->newValue.flowRate = logArray[(LOG_LENGTH*i)+16];
        stLog->newValue.triggerSensitivity = logArray[(LOG_LENGTH*i)+17];
        stLog->CRC = (uint16_t)logArray[(LOG_LENGTH*i)+18];
        stLog++;
    }
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_Send10LogToPCApp				 								*/
/*                                                                                      */
/* Details:         send 10 Portale logs to PC Application			 					*/
/*                                                                                      */
/* Arguments : (I)  POR_LOG_T *log			        								*/
/*                                                                                      */
/****************************************************************************************/
static bool logMgr_Send10LogToPCApp(const POR_LOG_T *log)
{
    bool error = false;
    if(!USBTask_SendToLogQueueForPCApp( log, 500))
    {
        error = true;
        DEBUG_LOGMGR("Failed to send Item to logQueueSendForPCApp from logMgr_Send10LogToPCApp");
    }//another case, do nothing

    return error;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_SendAllLog	                                        		*/
/*                                                                                      */
/* Details		: 	Send all of Portable log to PC Application							*/
/*                                                                                      */
/* ReturnValue : NONE	                                                         		*/
/*                                                                                      */
/* Using Global Data : (I) gs_CurrentAddress			                              	*/
/*                                                                                      */
/****************************************************************************************/
void logMgr_SendAllLog()
{
    POR_LOG_T logGet[10];
    uint32_t address = gs_CurrentAddress;
    uint32_t numberOfLog = gs_NumLogSave;

    //PC just requires MAX_NUMBER_OF_SAVED_LOG(1000) for each times
    if(numberOfLog >= MAX_NUMBER_OF_SAVED_LOG)
    {
        numberOfLog = MAX_NUMBER_OF_SAVED_LOG;
    }//another case, do nothing

    DEBUG_LOGMGR("get %d Log From address = 0x%x \n",numberOfLog, address);

    //send until number of log is 0
    while(numberOfLog > 0)
    {
        uint32_t numberOfLogOneTime = 0;

        //log send to Queue in one times is NUMBER_OF_LOG_SENT_PER_QUEUE(10) or lower
        if(numberOfLog >= NUMBER_OF_LOG_SENT_PER_QUEUE)
        {
            numberOfLogOneTime = NUMBER_OF_LOG_SENT_PER_QUEUE;
            numberOfLog -= NUMBER_OF_LOG_SENT_PER_QUEUE;
        }
        else
        {
            numberOfLogOneTime = numberOfLog;
            numberOfLog = 0;
        }

        if((address - FISRT_ADDRESS_FOR_MACHINE_LOG)/LOG_LENGTH <  NUMBER_OF_LOG_SENT_PER_QUEUE)
        {
        	//Initialize log Code
            for (int i = 0; i< NUMBER_OF_LOG_SENT_PER_QUEUE; i++)
            {
                logGet[i].logCode = eFirsLogId;
            }
            //if this block has not enough NUMBER_OF_LOG_SENT_PER_QUEUE(10), read each log from this block to previous block
            for(uint8_t i = 0; i < numberOfLogOneTime; ++i)
            {
                POR_LOG_T TemplogGet[10];
                logMgr_GetNLog(TemplogGet,address,1);
                //copy from buffer to data
                logGet[i].CRC = TemplogGet[0].CRC;
                logGet[i].EventOrAlarm = TemplogGet[0].EventOrAlarm;
                logGet[i].alarmType = TemplogGet[0].alarmType;
                logGet[i].day = TemplogGet[0].day;
                logGet[i].hour = TemplogGet[0].hour;
                logGet[i].logCode = TemplogGet[0].logCode;
                logGet[i].minute = TemplogGet[0].minute;
                logGet[i].month = TemplogGet[0].month;
                logGet[i].second = TemplogGet[0].second;
                logGet[i].year = TemplogGet[0].year;
                logGet[i].newValue.flowRate = TemplogGet[0].newValue.flowRate;
                logGet[i].newValue.mode = TemplogGet[0].newValue.mode;
                logGet[i].newValue.triggerSensitivity = TemplogGet[0].newValue.triggerSensitivity;

                logGet[i].oldValue.flowRate = TemplogGet[0].oldValue.flowRate;
                logGet[i].oldValue.mode = TemplogGet[0].oldValue.mode;
                logGet[i].oldValue.triggerSensitivity = TemplogGet[0].oldValue.triggerSensitivity;

                address -= LOG_LENGTH;
                if(address <= FISRT_ADDRESS_FOR_MACHINE_LOG)
                {
                    address = LAST_ADDRESS_FOR_MACHINE_LOG;
                }//another case, do nothing
            }
        }
        else
        {
            //if this block has more than NUMBER_OF_LOG_SENT_PER_QUEUE(10) log read normally and update address
            logMgr_GetNLog(logGet,address,numberOfLogOneTime);
            address -= LOG_LENGTH * NUMBER_OF_LOG_SENT_PER_QUEUE;
        }
        DEBUG_LOGMGR("Get from 0x%x %d Log, Remain Log = %d\n",address,numberOfLogOneTime,numberOfLog);

        //send to USB queue
        bool error = logMgr_Send10LogToPCApp(logGet);
        if(error == true)
            break;
        else
            vTaskDelay(DELAY_TICK_60MS);
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_DeleteAllLog				                               		*/
/*                                                                                      */
/* Details		: 	Delete all log in SPIF												*/
/*                                                                                      */
/* ReturnValue : NONE	                                                         		*/
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                                                                                      */
/****************************************************************************************/
void logMgr_DeleteAllLog(void)
{
    spifMgr_spifiErase(pSpifi, FISRT_BLOCK_FOR_MACHINE_LOG, NUMBER_OF_BLOCK_FOR_MACHINE_LOG);
    logMgr_InitLogMemory();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_DeleteAllPressureLog		                               		*/
/*                                                                                      */
/* Details		: 	Delete all pressure log in SPIF										*/
/*                                                                                      */
/* ReturnValue : NONE	                                                         		*/
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                                                                                      */
/****************************************************************************************/
void logMgr_DeleteAllPressureLog(void)
{
    spifMgr_spifiErase(pSpifi, TANK_PRESSURE_BLOCK_FOR_PRESSURE_LOG, NUMBER_OF_BLOCK_FOR_PRESSURE_LOG);
    spifMgr_spifiErase(pSpifi, VACUUM_PRESSURE_BLOCK_FOR_PRESSURE_LOG, NUMBER_OF_BLOCK_FOR_PRESSURE_LOG);
    spifMgr_spifiErase(pSpifi, SUPPLY_PRESSURE_BLOCK_FOR_PRESSURE_LOG, NUMBER_OF_BLOCK_FOR_PRESSURE_LOG);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_SendPressureLogToPCApp	                               		*/
/*                                                                                      */
/* Details		: 	Send all of Pressure log to PC Application							*/
/*                                                                                      */
/* ReturnValue : NONE	                                                         		*/
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                                                                                      */
/****************************************************************************************/
static void logMgr_SendPressureLogToPCApp(const POR_PRESS_LOG_T *log)
{
    if(!USBTask_SendToLogQueueSendPressureForPCApp(log, 100))
    {
        DEBUG_TASK("Failed to send Item to logQueueSendPressureForPCApp from logMgr_SendPressureLogToPCApp");
    }//another case, do nothing
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_SendPressureLog			                               		*/
/*                                                                                      */
/* Details		: 	Get and Send all of Pressure log to PC Application					*/
/*                                                                                      */
/* ReturnValue : NONE	                                                         		*/
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                                                                                      */
/****************************************************************************************/
void logMgr_SendTankPressureLog()
{
    uint8_t logArray[LOG_PRESS_LENGTH] = {'\0'};
    POR_PRESS_LOG_T logPressGet;
    spifMgr_spifiRead(pSpifi, TANK_PRESSURE_ADDRESS_FOR_PRESSURE_LOG, (uint32_t*)logArray, LOG_PRESS_LENGTH);
    if(logArray[0]==0xAC)
    {
        memcpy(&logPressGet,&logArray[1],sizeof(POR_PRESS_LOG_T));
        memcpy(&logPressGet.PressData[0],&logArray[10],300);
    }
    else
    {
        memset(&logPressGet,'\0',sizeof(POR_PRESS_LOG_T));
    }
    logPressGet.logCode = eTankPressureLog;
    logMgr_SendPressureLogToPCApp(&logPressGet);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_SendPressureLog			                               		*/
/*                                                                                      */
/* Details		: 	Get and Send all of Pressure log to PC Application					*/
/*                                                                                      */
/* ReturnValue : NONE	                                                         		*/
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                                                                                      */
/****************************************************************************************/
void logMgr_SendVacuumPressureLog()
{
    uint8_t logArray[LOG_PRESS_LENGTH] = {'\0'};
    POR_PRESS_LOG_T logPressGet;
    spifMgr_spifiRead(pSpifi, VACUUM_PRESSURE_ADDRESS_FOR_PRESSURE_LOG, (uint32_t*)logArray, LOG_PRESS_LENGTH);
    if(logArray[0]==0xAC)
    {
        memcpy(&logPressGet,&logArray[1],sizeof(POR_PRESS_LOG_T));
        memcpy(&logPressGet.PressData[0],&logArray[10],300);
    }
    else
    {
        memset(&logPressGet,'\0',sizeof(POR_PRESS_LOG_T));
    }
    logPressGet.logCode = eVacuumPressureLog;
    logMgr_SendPressureLogToPCApp(&logPressGet);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_RecordTankPressureLog		                               		*/
/*                                                                                      */
/* Details		: 	Record pressure log													*/
/*                                                                                      */
/* ReturnValue : NONE	                                                         		*/
/*                                                                                      */
/* Using Global Data : (I) POR_PRESS_LOG_T* pressLog	                              	*/
/*                                                                                      */
/****************************************************************************************/
void logMgr_RecordTankPressureLog(POR_PRESS_LOG_T* const pressLog)
{

    uint8_t logArray[LOG_PRESS_LENGTH];
    logMgr_ConvertPressureLogToBinaryArray(pressLog, logArray);
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT, 1, 7,true);
    spifMgr_spifiErase(pSpifi, TANK_PRESSURE_BLOCK_FOR_PRESSURE_LOG, NUMBER_OF_BLOCK_FOR_PRESSURE_LOG);
    spifMgr_spifiProgram(pSpifi,TANK_PRESSURE_ADDRESS_FOR_PRESSURE_LOG,(uint32_t*)logArray, LOG_PRESS_LENGTH);
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT, 1, 7,false);
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:   logMgr_RecordSupplyPressureLog                                      */
/*                                                                                      */
/* Details      :   Record supply log                                                   */
/*                                                                                      */
/* ReturnValue : NONE                                                                   */
/*                                                                                      */
/* Using Global Data : (I) POR_PRESS_LOG_T* pressLog                                    */
/*                                                                                      */
/****************************************************************************************/
void logMgr_RecordSupplyPressureLog(POR_PRESS_LOG_T* const pressLog)
{
    uint8_t logArray[LOG_PRESS_LENGTH];
    logMgr_ConvertPressureLogToBinaryArray(pressLog, logArray);
    spifMgr_spifiErase(pSpifi, SUPPLY_PRESSURE_BLOCK_FOR_PRESSURE_LOG, NUMBER_OF_BLOCK_FOR_PRESSURE_LOG);
    spifMgr_spifiProgram(pSpifi,SUPPLY_PRESSURE_ADDRESS_FOR_PRESSURE_LOG,(uint32_t*)logArray, LOG_PRESS_LENGTH);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_RecordPressureLog		                               		*/
/*                                                                                      */
/* Details		: 	Record pressure log													*/
/*                                                                                      */
/* ReturnValue : NONE	                                                         		*/
/*                                                                                      */
/* Using Global Data : (I) POR_PRESS_LOG_T* pressLog	                              	*/
/*                                                                                      */
/****************************************************************************************/
void logMgr_RecordVacuumPressureLog(POR_PRESS_LOG_T* const pressLog)
{
    uint8_t logArray[LOG_PRESS_LENGTH];
    logMgr_ConvertPressureLogToBinaryArray(pressLog, logArray);
    spifMgr_spifiErase(pSpifi, VACUUM_PRESSURE_BLOCK_FOR_PRESSURE_LOG, NUMBER_OF_BLOCK_FOR_PRESSURE_LOG);
    spifMgr_spifiProgram(pSpifi,VACUUM_PRESSURE_ADDRESS_FOR_PRESSURE_LOG,(uint32_t*)logArray, LOG_PRESS_LENGTH);
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:   logMgr_SendSupplyPressureLog                                        */
/*                                                                                      */
/* Details      :   Get and Send all of supply log to PC Application                    */
/*                                                                                      */
/* ReturnValue : NONE                                                                   */
/*                                                                                      */
/* Using Global Data : (I) NONE                                                         */
/*                                                                                      */
/****************************************************************************************/
void logMgr_SendSupplyPressureLog()
{
    uint8_t logArray[LOG_PRESS_LENGTH] = {'\0'};
    POR_PRESS_LOG_T logPressGet;
    spifMgr_spifiRead(pSpifi, SUPPLY_PRESSURE_ADDRESS_FOR_PRESSURE_LOG, (uint32_t*)logArray, LOG_PRESS_LENGTH);
    if(logArray[0]==0xAC)
    {
        memcpy(&logPressGet,&logArray[1],sizeof(POR_PRESS_LOG_T));
        memcpy(&logPressGet.PressData[0],&logArray[10],300);
    }
    else
    {
        memset(&logPressGet,'\0',sizeof(POR_PRESS_LOG_T));
    }
    logPressGet.logCode = eSupplyPressureLog;
    logMgr_SendPressureLogToPCApp(&logPressGet);
}
