/********************************************************************/
/*                                                                  */
/* File Name    : LogMgr.h                                    		*/
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
#include <LogMgr.h>
#include "spifilib_dev.h"
#include "SPIFMgr.h"
#include "string.h"
#include "RealTimeClock.h"
#define DEBUG_LOGMGR_EN
#ifdef DEBUG_LOGMGR_EN
#define DEBUG_LOGMGR(...) printf(__VA_ARGS__)
#else
#define DEBUG_LOGMGR(...)
#endif
static uint32_t gs_CurrentAddress;
static uint32_t gs_NumLogSave;
extern SPIFI_HANDLE_T *pSpifi;

#define LOG_LENGTH                                  32
#define BLOCK_SIZE                                  0x10000 //65 536
#define LOGS_1_BLOCK                                BLOCK_SIZE/LOG_LENGTH
#define MACHINE_LOG_HEADER                          0xAB    //171
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_ConvertLogToBinaryArray                                      */
/*                                                                                      */
/* Details		: 	convert the Portable log to binary array						     	*/
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
	if(log.year < 2000)
	{
		log.year = 2000;
	}
	if(log.month < 1)
	{
		log.month = 1;
	}
	if(log.day < 1)
	{
		log.day = 1;
	}
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
/* Function name: 	logMgr_Delete2048Logs	                                		    */
/*                                                                                      */
/* Details		: 	Delete 2048 Log in the SPIF									     	*/
/*                                                                                      */
/* Arguments : (I)  NONE				       		                  		          	*/
/*             (O)  uint32_t				                                      		*/
/*                                                                                      */
/****************************************************************************************/
static uint32_t logMgr_Delete2048Logs()
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
        gs_CurrentAddress = logMgr_Delete2048Logs();

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
/* Details		: 	Get number of existed log in the SPIF						     	*/
/*                                                                                      */
/* Arguments 	: 	(I) NONE				                                 			*/
/*                  (O) NONE                                                            */
/* ReturnValue 	: uint32_t                                                         		*/
/****************************************************************************************/
uint32_t logMgr_GetNumberOfLog(void)
{

    const uint32_t MAX_NUMBER_OF_SAVED_LOG = 1000;

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
                    //check check byte of previous log, expect the first and the last address of block
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
/* Function name: 	logMgr_CreateCRC						              				*/
/*                                                                                      */
/* Details:        Create CRC for Portable Log											*/
/*                                                                                      */
/* Arguments:	(I) POR_LOG_T* log														*/
/*                                                                                      */
/* ReturnValue : unsigned char	                                                        */
/*                                                                                     	*/
/****************************************************************************************/
static unsigned char logMgr_CreateCRC(POR_LOG_T* log)
{
	unsigned char data[LOG_LENGTH]={'\0'};
	memcpy(data,log,sizeof(POR_LOG_T));
	unsigned char sum = 0;
	for(int i = 0; i < (sizeof(POR_LOG_T)-2); i++)
	{
		sum ^= data[i];
	}
	return sum;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_RecordLog		                                        	*/
/*                                                                                      */
/* Details		: 	Record a log 												     	*/
/*                                                                                      */
/* Arguments 	: 	(I)  POR_LOG_T 	log			                                  		*/
/*                  (O)  NONE           	                                            */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void logMgr_RecordLog(POR_LOG_T log)
{
	Real_Time_Clock clock;
	realtimeclock_gettime(&clock);
	log.year = clock.year;
	log.month = clock.month;
	log.day = clock.mday;
	log.hour = clock.hour;
	log.minute = clock.min;
	log.second = clock.sec;
	log.CRC = logMgr_CreateCRC(&log);
	logMgr_WriteLogToSpiFlash(log);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	logMgr_GetCurrentSetting                                        	*/
/*                                                                                      */
/* Details		: 	Get current Mode, Flow rate and trigger sensitivity value from 		*/
/* 					setting		 												     	*/
/*                                                                                      */
/* Arguments 	: 	(I)  SettingValue* setting	                                  		*/
/*                  (O)  NONE           	                                            */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void logMgr_GetCurrentSetting(SettingValue* setting)
{
	setting->mode = 0;
	setting->flowRate = 0;
	setting->triggerSensitivity = 0;
}
