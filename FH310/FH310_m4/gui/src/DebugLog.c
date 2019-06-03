/********************************************************************/
/*                                                                  */
/* File Name    : DebugLog.c                                		*/
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
/* #0001       2017/09/01		Viet Le				modify			*/
/*    major redesign this module for easy 							*/
/*    portability and extendibility 								*/
/* #0002       2017/10/18		Viet Le				modify			*/
/*	when debug log is cleared, all variables related debug 			*/
/* log should be initialize at same time on one function (bug 1979)	*/
/*  		   2017/10/28      Linh Nguyen(MV)  	    modify	    */
/* #0003 Change get RTC (bug #2002)									*/
/* #0004       2017/11/21		Viet Le				modify			*/
/*  -Increment s_lowPressureCounter after from finish PSA sequence	*/
/*	 if product tank pressure < 50 kPa(bug 2036)					*/
/* #0005       2017/11/22		Viet Le			modify				*/
/*	-add Increase 	s_logList[i].timerCounter++ when timer counter	*/
/*	reset (Bug 2039)												*/
/* #0006       2017/11/22		Viet Le			modify				*/
/*	-Change Check the O2 condition after 20min from start operation */
/*	(Bug 2038)														*/
/* #0007       2017/11/22		Viet Le			modify				*/
/*	-Change the condition to record Debug log for low pressure case	*/
/*	Bug 2036)														*/
/* #0008       2017/11/23		Viet Le			modify				*/
/*	-Edit process time indicate to write (Bug 2039)					*/
/********************************************************************/

#include "integer.h"
#include "EEPROM.h"
#include "RealTimeClock.h"
#include "DebugLog.h"
#include "setting.h"
#include "GuiTask.h"
#include "GuiHandler.h"
#include "ModeMgr.h"
#include "ExternalEeprom.h"
#include "GuiInterface.h"

DEBUG_LOG_T s_logList[eMaxDebugLog];
static uint16_t s_numberOfLog = 0;
static uint16_t s_extEEPROMBlockNumber = 0;
static uint16_t s_extEEPROMPageNumber = 0;
static uint16_t s_extEEPROMOffsetNumber = 0;
static uint16_t s_lowPressureCounter = 0;
static uint16_t s_fanErrCounter = 0;
static uint16_t s_o2ErrCounter = 0;
static uint16_t s_flowrateErrCounter = 0;

static int16_t s_readLogBlock = 0;
static int16_t s_readLogPage = 0;
static int16_t s_readLogOffset = 0;

//these values have been scale by 100
static const int16_t s_flowRateSetting[11]={25, 50, 75, 100, 125, 150, 175, 200, 250, 300, 300};
static const int16_t s_underFlowRateValue[11] = {5, 30, 55, 80, 105, 130, 155, 180, 225, 270, 270};
static const int16_t s_upperFlowRateValue[11] = {45, 75, 95, 120, 145, 170, 195, 220, 275, 330, 330};

/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_CheckDectecingFRDebugLog						                    */
/*                                                                                      */
/* Details  -Check a condition of detecting flow rate									*/
/* 			 A condition of detecting flow rate for 20 seconds.							*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint16_t flowRateSetting : - flow rate setting						*/
/*				   -range : 25-300														*/
/*                 -unit : L/M                                                          */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : - true: condition is mapped										*/
/*						- false: if flowrate setting in range (measured FR  < under FR	*/
/* 								  || measured FR  > upper FR)							*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_flowRateSetting : array contains all flow rate setting     */
/*                     (O) s_underFlowRateValue : array contains all under FR value 	*/
/*                     (O) s_upperFlowRateValue : array contains all upper FR value 	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
bool debugLog_CheckDectecingFRDebugLog(uint16_t flowRateSetting)
{
	uint8_t flowRateIndex;
	for(flowRateIndex = 0; flowRateIndex < NUMBER_SETTING_FLOWRATE; flowRateIndex++)
	{
		if(flowRateSetting == s_flowRateSetting[flowRateIndex])
		{
			break;
		}
	}

	if ((guiTask_GetRealMonitorData().flowMeasure < s_underFlowRateValue[flowRateIndex])||
			guiTask_GetRealMonitorData().flowMeasure > s_upperFlowRateValue[flowRateIndex])
	{
		return true;
	}
	return false;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_CheckWriteStartLowO2						                    	*/
/*                                                                                      */
/* Details  -Check and write start low O2 log 											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) PSA_MONITOR_PACKET_t data : monitor data		 						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void debugLog_CheckWriteStartLowO2(PSA_MONITOR_PACKET_t data)
{
	static E_PsaSelectName currentPsaSelectName = NORM_TEMP_NORM_02;
	static bool isLowO2 = false;

	if(data.psaParamSelect != currentPsaSelectName)
	{
		switch(data.psaParamSelect)
		{
		case LOW_TEMP_NORM_02:
		case NORM_TEMP_NORM_02:
		case HIGH_TEMP_NORM_02:
			isLowO2 = false;
			break;
		case LOW_TEMP_LOW_02:
		case NORM_TEMP_LOW_02:
		case HIGH_TEMP_LOW_02:
			if(isLowO2 == false)
			{
				isLowO2 = true;
				debugLog_Write(data);
			}
			break;
		default:
			break;
		}
		currentPsaSelectName = data.psaParamSelect;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_CheckConditionDebugLog						                    */
/*                                                                                      */
/* Details  -Check all condtion and save debug log										*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) PSA_MONITOR_PACKET_t data : monitor data								*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_logList : log list											*/
/*                     (O) s_lowPressureCounter : counter check low pressure			*/
/*                     (O) s_fanErrCounter : counter check fan error					*/
/*                     (O) s_o2ErrCounter : counter check O2 concentration				*/
/*                     (O) s_flowrateErrCounter : counter check flow rate				*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void debugLog_CheckConditionDebugLog(PSA_MONITOR_PACKET_t data)
{
	static uint16_t flowrateSetting = 0;
	static bool isFinishStartPSA = false;
	static int counterPSA = 0;
	if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
	{
		if(isFinishStartPSA == false)
		{
			counterPSA++;
			if(counterPSA >= TIMER_FINISH_START_PSA)
			{
				isFinishStartPSA = true;
				counterPSA = 0;
			}
		}

		float productTankValue = data.productankPress /SCALE_DEBUG_LOG;
		float unitTempValue = data.temperature /SCALE_DEBUG_LOG;
		float compOutletTempValue = data.compTemperature /SCALE_DEBUG_LOG;
		float o2ConcentrationValue = data.o2Concentrator /SCALE_DEBUG_LOG;

		//Check condition temperature
		if(unitTempValue >= UNIT_TEMP_CONDITION_DEBUG_LOG)
		{
			s_logList[eHighTempDebugLog].active = true;
		}
		else
		{
			s_logList[eHighTempDebugLog].active = false;
		}
		//Check condition high pressure
		if(productTankValue >= PT_TANK_HIGH_CONDITION_DEBUG_LOG)
		{
			s_logList[eHighPressDebugLog].active = true;
		}
		else
		{
			s_logList[eHighPressDebugLog].active = false;
		}

		//Low Pressure
		if(isFinishStartPSA == true)
		{
			if(s_lowPressureCounter < TIMER_CHECK_LOW_PT_AFTER_FINISH_START_PSA)
			{
				s_lowPressureCounter++;
			}
			if(s_lowPressureCounter >= TIMER_CHECK_LOW_PT_AFTER_FINISH_START_PSA)
			{
				//Pass 15s from finish psa start, allow record debug log if pressure < PT_TANK_LOW_CONDITION_DEBUG_LOG
				if(productTankValue < PT_TANK_LOW_CONDITION_DEBUG_LOG)
				{
					s_logList[eLowPressDebugLog].active = true;
				}
				else
				{
					s_logList[eLowPressDebugLog].active = false;
				}
			}
		}

		//Check cooling fan
		if(data.coolingFanErrState == true)
		{
			s_fanErrCounter++;
			if(s_fanErrCounter >= TIMER_FAN_ERR_CONDITION_DEBUG_LOG)
			{
				s_logList[eCoolingFanDebugLog].active = true;
				s_fanErrCounter = 0;
			}
		}
		else
		{
			s_fanErrCounter = 0;
			s_logList[eCoolingFanDebugLog].active = false;
		}

		//Check O2 condition
		if(s_o2ErrCounter < TIMER_O2_CONDITION_DEBUG_LOG)
		{
			s_o2ErrCounter++;
		}
		if(s_o2ErrCounter >= TIMER_O2_CONDITION_DEBUG_LOG)
		{
			/*Pass 20 min from start operation, allow record debug log if
			o2ConcentrationValue <= O2_CONDITION_DEBUG_LOG*/
			if(o2ConcentrationValue <= O2_CONDITION_DEBUG_LOG)
			{
				s_logList[eO2ConcentraionDebugLog].active = true;
			}
			else
			{
				s_logList[eO2ConcentraionDebugLog].active = false;
			}
		}

		//Check detecting flow rate
		if(flowrateSetting != ((guiInterface_GetSettingValue(eFlowRateSettingId)*SCALE_DEBUG_LOG) / FLOWRATE_SCALE))
		{
			flowrateSetting = (guiInterface_GetSettingValue(eFlowRateSettingId)*SCALE_DEBUG_LOG) / FLOWRATE_SCALE;
			s_flowrateErrCounter = 0;
		}

		if(debugLog_CheckDectecingFRDebugLog(flowrateSetting) == true)
		{
			s_flowrateErrCounter++;
			if(s_flowrateErrCounter >= FR_CONDITION_DEBUG_LOG)
			{
				s_logList[eFRDebugLog].active = true;
				s_flowrateErrCounter = 0;
			}
		}
		else
		{
			s_flowrateErrCounter = 0;
			s_logList[eFRDebugLog].active = false;
		}

		//Check comp outlet
		if(compOutletTempValue >= COMP_TEMP_CONDITION_DEBUG_LOG)
		{
			s_logList[eCompOutletDebugLog].active = true;
		}
		else
		{
			s_logList[eCompOutletDebugLog].active = false;
		}

		//Write start low O2
		debugLog_CheckWriteStartLowO2(data);

		//process time indicate to write
		for(int i = 0; i < eMaxDebugLog; i++)
		{
			if(s_logList[i].active == true)		//a log condition is active
			{
				if(s_logList[i].timerCounter == 0)
				{
					//set flag ready to log
					s_logList[i].readyTolog = true;
				}

				//increase timer counter
				s_logList[i].timerCounter++;

				//Period 1 seconds
				if(s_logList[i].timerCounter >= s_logList[i].period)
				{
					s_logList[i].timerCounter = 0;
				}
			}
			else
			{
				//clear counter
				s_logList[i].timerCounter = 0;
			}
		}

		//Write log by condition
		debugLog_SaveDebugLog(data);
	}
	else
	{
		guihandler_StopTimerCheckDebugLog();
		debugLog_ResetAllCondition();
		s_lowPressureCounter = 0;
		s_fanErrCounter = 0;
		s_o2ErrCounter = 0;
		s_flowrateErrCounter = 0;
		isFinishStartPSA = false;
		counterPSA = 0;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_ConvertLogToArray						                    		*/
/*                                                                                      */
/* Details  -convert log to array														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) DEBUG_LOG_STRUCT* log : log data										*/
/*                 int8_t* logArray : log array                    					    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE															*/
/*                     (O) NONE															*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void debugLog_ConvertLogToArray(DEBUG_LOG_STRUCT* log, int8_t* logArray)
{
	int index = 0;
	memcpy((logArray + index), &log->year, sizeof(log->year));
	index += sizeof(log->year);
	memcpy((logArray + index), &log->month, sizeof(log->month));
	index += sizeof(log->month);
	memcpy((logArray + index), &log->day, sizeof(log->day));
	index += sizeof(log->day);
	memcpy((logArray + index), &log->hour, sizeof(log->hour));
	index += sizeof(log->hour);
	memcpy((logArray + index), &log->min, sizeof(log->min));
	index += sizeof(log->min);
	memcpy((logArray + index), &log->sec, sizeof(log->sec));
	index += sizeof(log->sec);

	memcpy((logArray + index), &log->productTank, sizeof(log->productTank));
	index += sizeof(log->productTank);
	memcpy((logArray + index), &log->unitTemp, sizeof(log->unitTemp));
	index += sizeof(log->unitTemp);
	memcpy((logArray + index), &log->compOutletTemp, sizeof(log->compOutletTemp));
	index += sizeof(log->compOutletTemp);

	memcpy((logArray + index), &log->measurementFR, sizeof(log->measurementFR));
	index += sizeof(log->measurementFR);
	memcpy((logArray + index), &log->o2Concentration, sizeof(log->o2Concentration));
	index += sizeof(log->o2Concentration);
	memcpy((logArray + index), &log->other, sizeof(log->other));
	index += sizeof(log->other);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_ConvertArrayToLog						                    		*/
/*                                                                                      */
/* Details  -convert array to log														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t* logArray : log array											*/
/*                 DEBUG_LOG_STRUCT* log : log data                    					*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE															*/
/*                     (O) NONE															*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void debugLog_ConvertArrayToLog(int8_t* logArray, DEBUG_LOG_STRUCT* log)
{
	int index = 0;
	memcpy(&log->year, logArray, sizeof(log->year));
	index += sizeof(log->year);
	memcpy(&log->month, (logArray + index), sizeof(log->month));
	index += sizeof(log->month);
	memcpy(&log->day, (logArray + index), sizeof(log->day));
	index += sizeof(log->day);
	memcpy(&log->hour, (logArray + index), sizeof(log->hour));
	index += sizeof(log->hour);
	memcpy(&log->min, (logArray + index), sizeof(log->min));
	index += sizeof(log->min);
	memcpy(&log->sec, (logArray + index), sizeof(log->sec));
	index += sizeof(log->sec);

	memcpy(&log->productTank, (logArray + index), sizeof(log->productTank));
	index += sizeof(log->productTank);

	memcpy(&log->unitTemp, (logArray + index), sizeof(log->unitTemp));
	index += sizeof(log->unitTemp);

	memcpy(&log->compOutletTemp,(logArray + index), sizeof(log->compOutletTemp));
	index += sizeof(log->compOutletTemp);

	memcpy(&log->measurementFR, (logArray + index), sizeof(log->measurementFR));
	index += sizeof(log->measurementFR);

	memcpy(&log->o2Concentration, (logArray + index), sizeof(log->o2Concentration));
	index += sizeof(log->o2Concentration);

	memcpy(&log->other, (logArray + index), sizeof(log->other));
	index += sizeof(log->other);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_ResetLogIndex						                    			*/
/*                                                                                      */
/* Details  -Reset log index																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t* logArray : log array											*/
/*                 DEBUG_LOG_STRUCT* log : log data                    					*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_readLogBlock : block index									*/
/*                     (O) s_readLogPage : page index									*/
/*                     (O) s_readLogOffset : offset index								*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void debugLog_ResetLogIndex(void)
{
	s_readLogBlock = debugLog_GetBlockNumber();
	s_readLogPage = debugLog_GetPageNumber();
	s_readLogOffset = debugLog_GetOffsetNumber();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_GetNextLog						                    			*/
/*                                                                                      */
/* Details  -Get next log																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) DEBUG_LOG_STRUCT* logPrt : log data									*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_readLogBlock : block index									*/
/*                     (O) s_readLogPage : page index									*/
/*                     (O) s_readLogOffset : offset index								*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void debugLog_GetNextLog(DEBUG_LOG_STRUCT* logPrt)
{
	//declare a log array
	int8_t logArray[EXT_EEPROM_LOG_SIZE] = {0};

	//adjust EEPROM address
	s_readLogOffset -= EXT_EEPROM_LOG_SIZE;
	if(s_readLogOffset < 0)
	{
		//set offset to the end of next page
		s_readLogOffset = EXT_EEPROM_OFFSET_PER_PAGE - EXT_EEPROM_LOG_SIZE;
		//move to next page
		s_readLogPage--;
		//check if reach to end of block
		if(s_readLogPage < 0)
		{
			//check for current block
			if(s_readLogBlock == EXT_EEPROM_BLOCK_0)	//if reach to begin of block 0
			{
				//move to block 1
				s_readLogBlock = EXT_EEPROM_BLOCK_1;
				//move to the end of block 1
				s_readLogPage = (EXT_EEPROM_PAGES_PER_BLOCK_1 - 1);
			}
			else if(s_readLogBlock == EXT_EEPROM_BLOCK_1)
			{
				//move to block 0
				s_readLogBlock = EXT_EEPROM_BLOCK_0;
				//move to the end of block 0
				s_readLogPage = (EXT_EEPROM_PAGES_PER_BLOCK_0 - 1);
			}
		}
	}

	//read data from external EEPROM
	extEeprom_Read(s_readLogBlock,(uint8_t*)&logArray[0],s_readLogPage,s_readLogOffset,EXT_EEPROM_LOG_SIZE);

	//convert array to Log
	debugLog_ConvertArrayToLog(&logArray[0], logPrt);
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_Write							                           	    	*/
/*                                                                                      */
/* Details  -Write debug log data to external EEPROM									*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) PSA_MONITOR_PACKET_t data : monitor data								*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_extEEPROMBlockNumber :current block of external EEPROM     */
/*                     (O) s_numberOfLog : number of log						        */
/*                     (O) s_extEEPROMOffsetNumber : current offset	number				*/
/*                     (O) s_extEEPROMPageNumber : current page number					*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void debugLog_Write(PSA_MONITOR_PACKET_t data)
{
	DEBUG_LOG_STRUCT log;
	//declare a log array
	int8_t logArray[EXT_EEPROM_LOG_SIZE] = {0};
	//put data to log array
	Real_Time_Clock lastTime = realtimeclock_GetLastTime();
	log.year = (lastTime.year%DIV_100_IN_DEBUG_LOG);
	log.month = (lastTime.month);
	log.day = (lastTime.mday);
	log.hour = (lastTime.hour);
	log.min = (lastTime.min);
	log.sec = (lastTime.sec);

	log.productTank = data.productankPress;
	log.unitTemp = data.temperature;
	log.compOutletTemp = data.compTemperature;
	log.measurementFR = data.flowMeasure;
	log.o2Concentration = data.o2Concentrator;

	if((data.coolingFanErrState == true) &&
			(data.filterPhotoErrStatus == true))
	{
		log.other = FAN_AND_FILTER_ERROR_DEBUG_LOG;
	}
	else
	{
		if(data.coolingFanErrState == true)
		{
			log.other = FAN_ERROR_DEBUG_LOG;
		}
		else
		{
			log.other = FILTER_ERROR_DEBUG_LOG;
		}
	}

	debugLog_ConvertLogToArray(&log, &logArray[0]);

	//write to EEPROM
	extEeprom_Write(s_extEEPROMBlockNumber,(uint8_t*)&logArray[0],s_extEEPROMPageNumber,s_extEEPROMOffsetNumber,EXT_EEPROM_LOG_SIZE);

	//move offset number to next
	s_extEEPROMOffsetNumber += EXT_EEPROM_LOG_SIZE;
	//check if current offset is reached to end of page, then move to next page
	if(s_extEEPROMOffsetNumber >= EXT_EEPROM_OFFSET_PER_PAGE)
	{
		//move to next page
		s_extEEPROMPageNumber++;
		if((s_extEEPROMBlockNumber == EXT_EEPROM_BLOCK_0)&&(s_extEEPROMPageNumber >= EXT_EEPROM_PAGES_PER_BLOCK_0))
		{
			//move to block 1
			s_extEEPROMBlockNumber = EXT_EEPROM_BLOCK_1;
			//reset number of page
			s_extEEPROMPageNumber = 0;
		}
		else if((s_extEEPROMBlockNumber == EXT_EEPROM_BLOCK_1)&&(s_extEEPROMPageNumber >= EXT_EEPROM_PAGES_PER_BLOCK_1))
		{
			//move to block 0
			s_extEEPROMBlockNumber = EXT_EEPROM_BLOCK_0;
			//reset number of page
			s_extEEPROMPageNumber = 0;
		}
		//reset offset number
		s_extEEPROMOffsetNumber = 0;
	}

	//increase number of LOG
	s_numberOfLog++;
	//check whether number of LOG reach to its limit
	if(s_numberOfLog > MAX_LOG_EXT)
	{
		s_numberOfLog = MAX_LOG_EXT;
	}

	//save EEPROM address
	debugLog_SaveCtrlParameters();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_ReadCtrlParameters							                    */
/*                                                                                      */
/* Details  -Read block number,number log, page number, page offset from external EEPROM*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE						 			    							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_extEEPROMBlockNumber :current block of external EEPROM     */
/*                     (O) s_numberOfLog : number of log						        */
/*                     (O) s_extEEPROMOffsetNumber : current offset	number				*/
/*                     (O) s_extEEPROMPageNumber : current page number					*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void debugLog_ReadCtrlParameters(void)
{
	uint16_t ctrlParameterBuffer[EXT_EEPROM_CTR_PARA_ITEM];
	extEeprom_Read(EXT_EEPROM_CTRL_PARA_BLOCK,(uint8_t*)ctrlParameterBuffer,EXT_EEPROM_CTR_PARA_PAGE,EXT_EEPROM_CTR_PARA_OFFSET,sizeof(ctrlParameterBuffer));

	if(ctrlParameterBuffer[POSITION_NUMBER_LOG] != VALUE_INIT_EEPROM)
	{
		s_numberOfLog = ctrlParameterBuffer[POSITION_NUMBER_LOG];
	}

	if(ctrlParameterBuffer[POSITION_PAGE_NUMBER_EXT] != VALUE_INIT_EEPROM)
	{
		s_extEEPROMPageNumber = ctrlParameterBuffer[POSITION_PAGE_NUMBER_EXT];
	}

	if(ctrlParameterBuffer[POSITION_PAGE_OFFSET_EXT] != VALUE_INIT_EEPROM)
	{
		s_extEEPROMOffsetNumber = ctrlParameterBuffer[POSITION_PAGE_OFFSET_EXT];
	}

	if(ctrlParameterBuffer[POSITION_BLOCK_EXT] != VALUE_INIT_EEPROM)
	{
		s_extEEPROMBlockNumber = ctrlParameterBuffer[POSITION_BLOCK_EXT];
	}

	debugLog_ResetAllCondition();

	DEBUGOUT("\n s_numberLog = %d", s_numberOfLog);
	DEBUGOUT("\n s_pageNumberExternal = %d", s_extEEPROMPageNumber);
	DEBUGOUT("\n s_pageOffsetExternal = %d", s_extEEPROMOffsetNumber);
	DEBUGOUT("\n s_blockExternal = %d", s_extEEPROMBlockNumber);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_SaveNumberLog							                        	*/
/*                                                                                      */
/* Details  -Save number log, page number, page offset									*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE													 			    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_extEEPROMBlockNumber :current block of external EEPROM		*/
/*                     (I) s_numberOfLog : number of log						        */
/*                     (I) s_extEEPROMOffsetNumber : current offset	number				*/
/*                     (I) s_extEEPROMPageNumber : current page number					*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void debugLog_SaveCtrlParameters(void)
{
	//declare a buffer to store control parameters
	uint16_t ctrParaBuffer[EXT_EEPROM_CTR_PARA_ITEM];
	//assign value to buffer
	ctrParaBuffer[POSITION_NUMBER_LOG] = s_numberOfLog;
	ctrParaBuffer[POSITION_PAGE_NUMBER_EXT] = s_extEEPROMPageNumber;
	ctrParaBuffer[POSITION_PAGE_OFFSET_EXT] = s_extEEPROMOffsetNumber;
	ctrParaBuffer[POSITION_BLOCK_EXT] = s_extEEPROMBlockNumber;

	//write control parameters to EEPROM
	extEeprom_Write(EXT_EEPROM_CTRL_PARA_BLOCK,(uint8_t*)ctrParaBuffer,EXT_EEPROM_CTR_PARA_PAGE,EXT_EEPROM_CTR_PARA_OFFSET,sizeof(ctrParaBuffer));
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_GetNumberLog							                        	*/
/*                                                                                      */
/* Details  -Get number of log															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE													 			    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint16_t : number of log		                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_numberOfLog : number of log							    */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
uint16_t debugLog_GetNumberOfLog(void)
{
	return s_numberOfLog;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_GetPageNumberExternal							                    */
/*                                                                                      */
/* Details  -Get page number external 													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE													 			    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint16_t : page number		                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_extEEPROMPageNumber : page number							*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
uint16_t debugLog_GetPageNumber(void)
{
	return s_extEEPROMPageNumber;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_GetBlockExternal							                    	*/
/*                                                                                      */
/* Details  -Get block external 														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE													 			    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint16_t : block external EEPROM	                                    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_extEEPROMBlockNumber : block external	EEPROM				*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
uint16_t debugLog_GetBlockNumber(void)
{
	return s_extEEPROMBlockNumber;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_ClearLog							                        		*/
/*                                                                                      */
/* Details  -Clear debug log															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE													 			    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_numberOfLog : number of log                                */
/*                     (O) s_extEEPROMPageNumber : page number						    */
/*                     (O) s_extEEPROMOffsetNumber : page offset						*/
/*                     (O) s_extEEPROMBlockNumber : block external eeprom				*/
/*                     (O) s_readLogBlock : block index									*/
/*                     (O) s_readLogPage : page index									*/
/*                     (O) s_readLogOffset : offset index								*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void debugLog_ClearLog(void)
{
	s_numberOfLog = 0;
	s_extEEPROMPageNumber = 0;
	s_extEEPROMOffsetNumber = 0;
	s_extEEPROMBlockNumber = 0;

	s_readLogBlock = 0;
	s_readLogPage = 0;
	s_readLogOffset = 0;

	//declare a buffer to store control parameters
	uint16_t ctrParaBuffer[EXT_EEPROM_CTR_PARA_ITEM];
	//assign value to buffer
	ctrParaBuffer[POSITION_NUMBER_LOG] = s_numberOfLog;
	ctrParaBuffer[POSITION_PAGE_NUMBER_EXT] = s_extEEPROMPageNumber;
	ctrParaBuffer[POSITION_PAGE_OFFSET_EXT] = s_extEEPROMOffsetNumber;
	ctrParaBuffer[POSITION_BLOCK_EXT] = s_extEEPROMBlockNumber;

	//write control parameters to EEPROM
	extEeprom_Write(EXT_EEPROM_CTRL_PARA_BLOCK,(uint8_t*)ctrParaBuffer,EXT_EEPROM_CTR_PARA_PAGE,EXT_EEPROM_CTR_PARA_OFFSET,sizeof(ctrParaBuffer));

	debugLog_ResetAllCondition();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_SaveDebugLog                        				    			*/
/*                                                                                      */
/* Details  -Save debug log FH310 by condition											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) PSA_MONITOR_PACKET_t data : monitor data								*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void			                                                    	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                                                                                      */
/*                     (O) NONE															*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void debugLog_SaveDebugLog(PSA_MONITOR_PACKET_t data)
{
	if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
	{
		bool okToWriteLog = true;
		for(int i = 0; i < eMaxDebugLog; i++)
		{
			if(s_logList[i].readyTolog == true)
			{
				if(okToWriteLog == true)
				{
					//log data
					debugLog_Write(data);
					//set flag do not write Log anymore
					okToWriteLog = false;
				}
				//clear flag
				s_logList[i].readyTolog = false;
			}
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_StartTimerCheckDebugLog                        				    */
/*                                                                                      */
/* Details  -Start timer check debug log												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void			                                                    	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void debugLog_StartTimerCheckDebugLog(void)
{
	if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
	{
		if(guihandler_GetActiveTimerCheckDebugLog() == false)
		{
			guihandler_StartTimerCheckDebugLog();
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_GetPageOffsetExternal                        				    	*/
/*                                                                                      */
/* Details  -Get page offset 															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint16_t : page offset		                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_extEEPROMOffsetNumber : offset number					    */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
uint16_t debugLog_GetOffsetNumber(void)
{
	return s_extEEPROMOffsetNumber;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-debugLog_ResetAllCondition                        				    		*/
/*                                                                                      */
/* Details  -Reset all condition 														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void						                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE															*/
/*                     (O) s_logList : log list                                         */
/*                     (O) s_lowPressureCounter : counter check low pressure			*/
/*                     (O) s_fanErrCounter : counter check fan error					*/
/*                     (O) s_o2ErrCounter : counter check O2 concentration				*/
/*                     (O) s_flowrateErrCounter : counter check flow rate				*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void debugLog_ResetAllCondition(void)
{
	s_lowPressureCounter = 0;
	s_fanErrCounter = 0;
	s_o2ErrCounter = 0;
	s_flowrateErrCounter = 0;

	s_logList[eHighTempDebugLog].id = eHighTempDebugLog;
	s_logList[eHighTempDebugLog].active = false;
	s_logList[eHighTempDebugLog].period = LOG_PERIOD_SECOND;
	s_logList[eHighTempDebugLog].timerCounter = 0;
	s_logList[eHighTempDebugLog].readyTolog = false;

	s_logList[eHighPressDebugLog].id = eHighPressDebugLog;
	s_logList[eHighPressDebugLog].active = false;
	s_logList[eHighPressDebugLog].period = LOG_PERIOD_SECOND;
	s_logList[eHighPressDebugLog].timerCounter = 0;
	s_logList[eHighPressDebugLog].readyTolog = false;

	s_logList[eLowPressDebugLog].id = eLowPressDebugLog;
	s_logList[eLowPressDebugLog].active = false;
	s_logList[eLowPressDebugLog].period = LOG_PERIOD_SECOND;
	s_logList[eLowPressDebugLog].timerCounter = 0;
	s_logList[eLowPressDebugLog].readyTolog = false;

	s_logList[eCoolingFanDebugLog].id = eCoolingFanDebugLog;
	s_logList[eCoolingFanDebugLog].active = false;
	s_logList[eCoolingFanDebugLog].period = LOG_PERIOD_SECOND;
	s_logList[eCoolingFanDebugLog].timerCounter = 0;
	s_logList[eCoolingFanDebugLog].readyTolog = false;

	s_logList[eO2ConcentraionDebugLog].id = eO2ConcentraionDebugLog;
	s_logList[eO2ConcentraionDebugLog].active = false;
	s_logList[eO2ConcentraionDebugLog].period = LOG_PERIOD_MINUTES;
	s_logList[eO2ConcentraionDebugLog].timerCounter = 0;
	s_logList[eO2ConcentraionDebugLog].readyTolog = false;

	s_logList[eFRDebugLog].id = eFRDebugLog;
	s_logList[eFRDebugLog].active = false;
	s_logList[eFRDebugLog].period = LOG_PERIOD_SECOND;
	s_logList[eFRDebugLog].timerCounter = 0;
	s_logList[eFRDebugLog].readyTolog = false;

	s_logList[eCompOutletDebugLog].id = eCompOutletDebugLog;
	s_logList[eCompOutletDebugLog].active = false;
	s_logList[eCompOutletDebugLog].period = LOG_PERIOD_MINUTES;
	s_logList[eCompOutletDebugLog].timerCounter = 0;
	s_logList[eCompOutletDebugLog].readyTolog = false;

	return;
}

/* end of files */


