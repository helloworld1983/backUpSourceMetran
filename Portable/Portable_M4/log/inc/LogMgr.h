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

#ifndef SYSTEM_INC_LOGMGR_H_
#define SYSTEM_INC_LOGMGR_H_

#include <DualCoreCommon.h>
#include <TaskCommon.h>
#include "board.h"
#include "Setting.h"

#define FISRT_ADDRESS_FOR_MACHINE_LOG  				0x15000000
#define FIRST_ADDRESS_OF_LAST_BLOCK_FOR_MACHINE_LOG 0x15010000
#define LAST_ADDRESS_FOR_MACHINE_LOG   				0x15020000
#define FISRT_BLOCK_FOR_MACHINE_LOG 				256
#define LAST_BLOCK_FOR_MACHINE_LOG 					257
#define NUMBER_OF_BLOCK_FOR_MACHINE_LOG 			2

#define TANK_PRESSURE_ADDRESS_FOR_PRESSURE_LOG  	0x15020000
#define TANK_PRESSURE_BLOCK_FOR_PRESSURE_LOG        258

#define VACUUM_PRESSURE_ADDRESS_FOR_PRESSURE_LOG  	0x15030000
#define VACUUM_PRESSURE_BLOCK_FOR_PRESSURE_LOG      259
#define NUMBER_OF_BLOCK_FOR_PRESSURE_LOG            1

#define SUPPLY_PRESSURE_ADDRESS_FOR_PRESSURE_LOG  	0x15040000
#define SUPPLY_PRESSURE_BLOCK_FOR_PRESSURE_LOG      260

#define BYTES_ONE_TIME_COPY 						128
#define LOG_LENGTH                                  32
#define LOG_PRESS_LENGTH                            512

#define BLOCK_SIZE                                  0x10000 //65 536
#define LOGS_1_BLOCK                                BLOCK_SIZE/LOG_LENGTH

#define MACHINE_LOG_HEADER                          0xAB    //171

enum {
	eEvent,
	eAlarmClear,
	eAlarmContact,
	eAlarmBreath,
	eAlarmCharge,
	eAlarmInspect,
	eAlarmPowerLost
};
typedef enum {
	eFirsLogId=0,
	eOperationStartLogId =1,
	eOperationStopLogId=2,
	eOperationRestartLogId=4,
	ePowerLost=6,
	ePowerLostClear=7,
	eAbnormalTemperatureLogId,
	eLowO2ConcentrationLogId=11,
	eAbnormalO2ConcentrationErrorLogId=12,
	eBreathErrorLogId=13,
	eFlowControllerO2SensorErrorLogId=14,
	eFlowControllerErrorLogId=38,
	ePsaParameterSetLowNormalO2ConcentrationLogId=139,
	ePsaParameterSetLowLowO2ConcentrationLogId=140,
	ePsaParameterSetNormalNormalO2ConcentrationLogId=141,
	ePsaParameterSetNormalLowO2ConcentrationLogId=142,
	ePsaParameterSetHighNormalO2ConcentrationLogId=143,
	ePsaParameterSetHighLowO2ConcentrationLogId=144,
	eSpeakerErrorLogId=160,
	eResetWatchdogLogId=166,
	eUseDefaultParameterLogId=169,
	eSPIFlashMemoryErrorLogId=171,
	eFlowControllerPressureSensorErrorLogId=173,
	eFlowControllerFlowSensorErrorLogId=174,
	eFlowControllerTemperatureSensorErrorLogId=175,
	eFlowControllerCommunicationCOMErrorLogId=176,
	eFlowControllerCommunicationCommandCheckSumErrorLogId=177,
	eFlowControllerCommunicationCommandCodeErrorLogId=178,
	eFlowControllerCommunicationCommandParameterErrorLogId=179,
	eFlowControllerCommunicationResponseCheckSumErrorLogId=180,
	eFlowControllerCommunicationResponseCodeErrorLogId=181,
	eFlowControllerCommunicationResponseDataLengthErrorLogId=183,
	eFlowControllerCommunicationNoResponsemErrorLogId=184,
	eCompressorDriverDCOverVoltageErrorLogId=185,
	eCompressorDriverDCLowerVoltageErrorLogId=186,
	eCompressorDriverSoftwareOverCurrentErrorLogId=187,
	eCompressorDriverHardwareOverCurrentErrorLogId=188,
	eCompressorDriverBoardTemperatureErrorLogId=189,
	eCompressorDriverMotorTemperatureErrorLogId=190,
	eCompressorDriverRotationSpeedErrorLogId=191,
	eCompressorDriverStepOutErrorLogId=192,
	eCompressorDriverCommunicationCommunicationErrorErrorLogId=193,
	eCompressorDriverCommunicationCommandCheckSumErrorLogId=194,
	eCompressorDriverCommunicationCommandCodeErrorLogId=195,
	eCompressorDriverCommunicationCommandParameterErrorLogId=196,
	eCompressorDriverCommunicationResponseCheckSumErrorLogId=197,
	eCompressorDriverCommunicationResponseCodeErrorLogId=198,
	eCompressorDriverCommunicationResponseDataLengthErrorLogId=200,
	eCompressorDriverCommunicationNoResponseErrorLogId=201,
	eACOperationLogId=1000,
	eDCOperationLogId=1001,
	eBatteryOperationLogId=1002,
	eChangeModeLogId=1003,
	eMuteLogId=1004,
	eChangeSoundVolumeLogId=1005,
	eChangeTriggerLevelLogId=1006,
	eChangeContinuousFlowrateLogId=1007,
	eChangeSynchronizeFlowrateLogId=1008,
	eUpdateModeSetLogId=1009,
	eResetPowerSwitching=1010,
	eBootloaderUpdateModeSetId=1011,
	eOxygenTankPressureErrorLogId=1101,
	eSupplyPressureSensorErrorLogId=1102,
	eVacuumPressureSensorErrorLogId=1103,
	eTankPressureSensorErrorLogId=1104,
	eDifferentialPressureSensorCommunicationErrorLogId=1105,
	eBatteryRemaininingCapacityLowerLogId=1106,
	eBatteryTemperatureErrorLogId=1107,
	eBatteryChargeErrorLogId=1108,
	eLEDDriverCommunicationErrorLogId=1109,
	eAudioCommunicationErrorLogId=1110,
	eUpdateFileErrorLogId=1111,
	eUpdateFileCopyErrorSPIFLogId=1112,
	eUpdateFileCopyErrorROMLogId=1113,
	eTachypneaError=1114,
	eCompressorSupplyPressureErrorLogId=1115,
	eCompressorVacuumPressureErrorLogId=1116,
	eBatteryTypeErrorLogId=1117,
	eNoneLog = 0xff
} LogId;
typedef struct
{
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint16_t logCode;
	uint8_t EventOrAlarm;
	uint8_t alarmType;
	SettingValue oldValue;
	SettingValue newValue;
	uint16_t CRC;
} POR_LOG_T;
typedef struct
{
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint16_t logCode;
	int8_t PressData[300];
	uint16_t CRC;
} POR_PRESS_LOG_T;

typedef enum
{
	eGetLogError,
	eContinue,
	eEnd
} GetLogResult;
typedef enum
{
	eRequestWriteLog,
	eRequestDeleteLog,
	eRequestDeletePressureLog,
	eRequestSendLogForPCApp,
	eRequestSendPressureLogForPCApp,
}E_logQueueType;

typedef struct
{
	E_logQueueType id;
	uint8_t data[20];
}E_logQueueReceive;

enum{
	eTankPressureLog=1,
	eVacuumPressureLog=2,
	eSupplyPressureLog=3
};

/****************************************************************************************/
/*                                                                                      */
/* Function name:   logMgr_InitLogMemory                                                */
/*                                                                                      */
/* Details      :   init memory for Portable log                                        */
/*                                                                                      */
/* ReturnValue : uint32_t                                                               */
/*                                                                                      */
/* Using Global Data : (I) gs_CurrentAddress                                            */
/*                                                                                      */
/****************************************************************************************/
void logMgr_InitLogMemory(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   logMgr_RecordLog                                                    */
/*                                                                                      */
/* Details      :   Record a log                                                        */
/*                                                                                      */
/* Arguments    :   (I)  POR_LOG_T  log                                                 */
/*                  (O)  NONE                                                           */
/*                                                                                      */
/****************************************************************************************/
void logMgr_RecordLog(POR_LOG_T log);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   logMgr_GetNumberOfLog                                               */
/*                                                                                      */
/* Details      :   Get number of existed log in the SPIF                               */
/*                                                                                      */
/* Arguments    :   (I) NONE                                                            */
/*                  (O) NONE                                                            */
/* ReturnValue  : uint32_t                                                              */
/* Using Global Data : (I) gs_CurrentAddress                                            */
/*                                                                                      */
/****************************************************************************************/
uint32_t logMgr_GetNumberOfLog(void );

/****************************************************************************************/
/*                                                                                      */
/* Function name:   logMgr_SendAllLog                                                   */
/*                                                                                      */
/* Details      :   Send all of Portable log to PC Application                          */
/*                                                                                      */
/* ReturnValue : NONE                                                                   */
/*                                                                                      */
/* Using Global Data : (I) gs_CurrentAddress                                            */
/*                                                                                      */
/****************************************************************************************/
void logMgr_SendAllLog();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   logMgr_SendPressureLog                                              */
/*                                                                                      */
/* Details      :   Get and Send all of Pressure log to PC Application                  */
/*                                                                                      */
/* ReturnValue : NONE                                                                   */
/*                                                                                      */
/* Using Global Data : (I) NONE                                                         */
/*                                                                                      */
/****************************************************************************************/
void logMgr_SendTankPressureLog();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   logMgr_DeleteAllLog                                                 */
/*                                                                                      */
/* Details      :   Delete all log in SPIF                                              */
/*                                                                                      */
/* ReturnValue : NONE                                                                   */
/*                                                                                      */
/* Using Global Data : (I) NONE                                                         */
/*                                                                                      */
/****************************************************************************************/
void logMgr_DeleteAllLog(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   logMgr_RecordTankPressureLog                                        */
/*                                                                                      */
/* Details      :   Record pressure log                                                 */
/*                                                                                      */
/* ReturnValue : NONE                                                                   */
/*                                                                                      */
/* Using Global Data : (I) POR_PRESS_LOG_T* pressLog                                    */
/*                                                                                      */
/****************************************************************************************/
void logMgr_RecordTankPressureLog(POR_PRESS_LOG_T*);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   logMgr_RecordPressureLog                                            */
/*                                                                                      */
/* Details      :   Record pressure log                                                 */
/*                                                                                      */
/* ReturnValue : NONE                                                                   */
/*                                                                                      */
/* Using Global Data : (I) POR_PRESS_LOG_T* pressLog                                    */
/*                                                                                      */
/****************************************************************************************/
void logMgr_RecordVacuumPressureLog(POR_PRESS_LOG_T*);

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
void logMgr_RecordSupplyPressureLog(POR_PRESS_LOG_T*);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   logMgr_DeleteAllPressureLog                                         */
/*                                                                                      */
/* Details      :   Delete all pressure log in SPIF                                     */
/*                                                                                      */
/* ReturnValue : NONE                                                                   */
/*                                                                                      */
/* Using Global Data : (I) NONE                                                         */
/*                                                                                      */
/****************************************************************************************/
void logMgr_DeleteAllPressureLog(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   logMgr_SendPressureLog                                              */
/*                                                                                      */
/* Details      :   Get and Send all of Pressure log to PC Application                  */
/*                                                                                      */
/* ReturnValue : NONE                                                                   */
/*                                                                                      */
/* Using Global Data : (I) NONE                                                         */
/*                                                                                      */
/****************************************************************************************/
void logMgr_SendVacuumPressureLog();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   logMgr_SendSupplyPressureLog                                        */
/*                                                                                      */
/* Details      :   Record supply   log                                                 */
/*                                                                                      */
/* ReturnValue : NONE                                                                   */
/*                                                                                      */
/* Using Global Data : (I) POR_PRESS_LOG_T* pressLog                                    */
/*                                                                                      */
/****************************************************************************************/
void logMgr_SendSupplyPressureLog();

#endif /* SYSTEM_INC_LOGMGR_H_ */
