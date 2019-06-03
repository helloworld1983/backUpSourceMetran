#ifndef INC_LOGMGR_H_
#define INC_LOGMGR_H_
/********************************************************************/
/*                                                                  */
/* File Name    : LogMgr.h                                    		*/
/*                                                                  */
/* General       : manager of FH310 log							    */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/19      Linh Nguyen		  	new file        */
/* #0001       2016/09/05      Linh Nguyen	  	    modify          */
/*  Improve bluetooth transfer mode: 								*/
/*	- define LOG_HEADER												*/
/*  - Add new member to  FH310_LOG_T struct                         */
/* #0002       2017/09/22      Linh Nguyen	  	    modify          */
/*  Modify and remove difine macro	 								*/
/********************************************************************/

/* include user headers */
#include "board.h"
#include "TaskCommon.h"
#include "LogInterface.h"

/* constants, definitions */

//FH310 log
#define FISRT_ADDRESS_FOR_MACHINE_LOG  				0x15fe0000
#define LAST_ADDRESS_FOR_MACHINE_LOG   				0x16000000
#define FISRT_BLOCK_FOR_MACHINE_LOG 				510
#define LAST_BLOCK_FOR_MACHINE_LOG 					511
#define NUMBER_OF_BLOCK_FOR_MACHINE_LOG 			2
#define MACHINE_MAX_LOG								1000

#define FISRT_ADDRESS_FOR_SERVER_LOG  				0x15fb0000
#define LAST_ADDRESS_FOR_SERVER_LOG  				0x15fe0000
#define FISRT_BLOCK_FOR_SERVER_LOG 					507
#define LAST_BLOCK_FOR_SERVER_LOG 					509
#define NUMBER_OF_BLOCK_FOR_SERVER_LOG 				3
#define CBX_USAGE_MAX_LOG							8000

#define FISRT_ADDRESS_FOR_ANYPAL_LOG   				0x15f50000
#define LAST_ADDRESS_FOR_ANYPAL_LOG  				0x15fb0000
#define FISRT_BLOCK_FOR_ANYPAL_LOG 					501
#define LAST_BLOCK_FOR_ANYPAL_LOG 					506
#define NUMBER_OF_BLOCK_FOR_ANYPAL_LOG 				6
#define ANYPAL_MAX_LOG								20000

#define FISRT_ADDRESS_FOR_USAGE_LOG  				0x15f20000
#define LAST_ADDRESS_FOR_USAGE_LOG   				0x15f50000
#define FISRT_BLOCK_FOR_USAGE_LOG 					498
#define LAST_BLOCK_FOR_USAGE_LOG 					500
#define NUMBER_OF_BLOCK_FOR_USAGE_LOG 				3
#define ANYPAL_USAGE_MAX_LOG						8000


#define LOG_LENGTH 									16
#define BLOCK_SIZE 									0x10000
#define LOGS_1_BLOCK								BLOCK_SIZE/LOG_LENGTH


#define LOG_HEADER 0xAC

typedef struct
{
	uint32_t currentAdd; 	//current address
	uint32_t numLogSaved;	// number of log saved
	uint32_t firstAdd; 		//fist address
	uint32_t lastAdd;		//last address
	uint32_t firstBlock;	//first block
	uint32_t lastBlock;		//last block
	uint32_t numBlock;		//number of block
	uint32_t maxLog;		//maximum log
}LOG_INFO_T;

/* prototype declarations */
//write FH310 log to spi flash
void logMgr_RecordLog(FH310_LOG_T log, LogType logType);

//init memory for FH310 log
void logMgr_InitLogMemory(void);

void logMgr_RequsetLogEvent(LOG_REQUEST_t getLog);

void logMgr_ClearLog(LogType logType);

void logMgr_WriteLogToSpiFlash(FH310_LOG_T log, LogType logType);

uint32_t logMgr_GetNumberOfLog(LogType logType);

#endif /* INC_LOGMGR_H_ */
