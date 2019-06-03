#ifndef INC_DEBUGLOG_H_
#define INC_DEBUGLOG_H_

/********************************************************************/
/*                                                                  */
/* File Name    : DebugLog.h                                		*/
/*                                                                  */
/* General      : Write and read debug log of FH310		            */
/* 				                            						*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/07/06       Viet Le		  	    new file        */
/* #0001       														*/
/*                                                                  */
/* #0002	   2017/07/06       Quyen Ngo	  	    modify		    */
/* 		-Change wait time TIMER_FINISH_START_PSA from 20s to 33s 	*/
/*		(Bug 2027)													*/
/********************************************************************/

#include "DualCoreCommon.h"

//Define event debug log
typedef enum{
	eHighTempDebugLog,
	eHighPressDebugLog,
	eLowPressDebugLog,
	eCoolingFanDebugLog,
	eO2ConcentraionDebugLog,
	eFRDebugLog,
	eCompOutletDebugLog,
	eStartLowO2DebugLog,
	eMaxDebugLog
}E_DEBUG_LOG_EVENT_ID;

#define SIZE_BUFFER_NUMBER_DEBUG_LOG				5
#define SIZE_BUFFER_DATE_DEBUG_LOG					3

#define EXT_EEPROM_CTRL_PARA_BLOCK					(1)
#define EXT_EEPROM_CTR_PARA_PAGE					(255)
#define EXT_EEPROM_CTR_PARA_OFFSET					(0)
#define EXT_EEPROM_CTR_PARA_ITEM					(4)
#define POSITION_NUMBER_LOG							0
#define POSITION_PAGE_NUMBER_EXT					1
#define POSITION_PAGE_OFFSET_EXT					2
#define POSITION_BLOCK_EXT							3


#define VALUE_INIT_EEPROM							0xFFFF
#define EXT_EEPROM_OFFSET_PER_PAGE					256
#define SCALE_DEBUG_LOG								100.0
#define FAN_ERROR_DEBUG_LOG							0b01
#define FILTER_ERROR_DEBUG_LOG						0b10
#define FAN_AND_FILTER_ERROR_DEBUG_LOG				0b11

#define UNIT_TEMP_CONDITION_DEBUG_LOG				65
#define PT_TANK_HIGH_CONDITION_DEBUG_LOG			135
#define PT_TANK_LOW_CONDITION_DEBUG_LOG				50
#define TIMER_LOW_CONDITION_DEBUG_LOG       		35
#define TIMER_FAN_ERR_CONDITION_DEBUG_LOG   		15
#define TIMER_O2_CONDITION_DEBUG_LOG   				1200

#define O2_CONDITION_DEBUG_LOG						85
#define FR_CONDITION_DEBUG_LOG						20
#define COMP_TEMP_CONDITION_DEBUG_LOG				73

#define TIMER_FINISH_START_PSA						33
#define TIMER_CHECK_LOW_PT_AFTER_FINISH_START_PSA	15
#define NUMBER_SETTING_FLOWRATE						10
#define DIV_100_IN_DEBUG_LOG						100
#define LOG_PERIOD_SECOND							1
#define LOG_PERIOD_MINUTES							60

#define EXT_EEPROM_BLOCK_0							(0)
#define EXT_EEPROM_BLOCK_1							(1)
#define EXT_EEPROM_LOG_SIZE							(32)
#define EXT_EEPROM_PAGES_PER_BLOCK_0				(256)
#define EXT_EEPROM_PAGES_PER_BLOCK_1				(255)
#define MAX_LOG_EXT									((EXT_EEPROM_OFFSET_PER_PAGE/EXT_EEPROM_LOG_SIZE)\
													*(EXT_EEPROM_PAGES_PER_BLOCK_0+EXT_EEPROM_PAGES_PER_BLOCK_1))
//Define packet data debug log
typedef struct{
	uint8_t year;//year
	uint8_t month;//month
	uint8_t day;//day
	uint8_t hour;//hour
	uint8_t min;//minutes
	uint8_t sec;//seconds

	int32_t productTank;//Product tank pressure
	int32_t unitTemp;//Unit internal temperature
	int32_t compOutletTemp;//Compressor outlet temperature

	int16_t measurementFR;//Measured flow rate
	int16_t o2Concentration;//O2 concentration
	uint16_t other;//Others
}DEBUG_LOG_STRUCT;

//Define information debug log
typedef struct{
	E_DEBUG_LOG_EVENT_ID id;
	bool active;
	uint8_t timerCounter;
	uint8_t period;
	bool readyTolog;
}DEBUG_LOG_T;

//Read number log from EEPROM
void debugLog_ReadCtrlParameters(void);

//Write debug log to EEPROM
void debugLog_Write(PSA_MONITOR_PACKET_t data);

//Save number log to EEPROM
void debugLog_SaveCtrlParameters(void);

//Get number log
uint16_t debugLog_GetNumberOfLog(void);

//Get page number external
uint16_t debugLog_GetPageNumber(void);

//Get block external
uint16_t debugLog_GetBlockNumber(void);

//Clear debug log
void debugLog_ClearLog(void);

//Save debug log by condition
void debugLog_SaveDebugLog(PSA_MONITOR_PACKET_t data);

//Check condition save debug log
void debugLog_CheckConditionDebugLog(PSA_MONITOR_PACKET_t data);

//Start timer check debug log
void debugLog_StartTimerCheckDebugLog(void);

//Get page offset
uint16_t debugLog_GetOffsetNumber(void);

//Reset all condition
void debugLog_ResetAllCondition(void);

//Write log to csv file
void debugLog_WriteLogToFile(int logNumber, int8_t* logArray);

//Reset log index
void debugLog_ResetLogIndex(void);

//Get next log
void debugLog_GetNextLog(DEBUG_LOG_STRUCT* logPrt);

#endif /* INC_DEBUGLOG_H_ */

/* end of file */
