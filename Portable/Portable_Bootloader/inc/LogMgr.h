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
#include "board.h"
#define FISRT_ADDRESS_FOR_MACHINE_LOG  				0x15000000
#define LAST_ADDRESS_FOR_MACHINE_LOG   				0x15020000
#define FISRT_BLOCK_FOR_MACHINE_LOG 				256
#define LAST_BLOCK_FOR_MACHINE_LOG 					257
#define NUMBER_OF_BLOCK_FOR_MACHINE_LOG 			2
#define FISRT_ADDRESS_FOR_PRESSURE_LOG  			0x15020000
#define FISRT_BLOCK_FOR_PRESSURE_LOG 				258
#define NUMBER_OF_BLOCK_FOR_PRESSURE_LOG 			1
#define FIRST_ADDRESS_OF_LAST_BLOCK_FOR_MACHINE_LOG 0x15010000
#define BYTES_ONE_TIME_COPY 						128
#define BYTES_OF_1_BLOCK 							65536
#define LOG_LENGTH 32
#define LOG_PRESS_LENGTH 512
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
	eNoneLog = 0xff
} LogId;
typedef struct
{
	uint8_t mode;
	uint8_t flowRate;
	uint8_t triggerSensitivity;
} SettingValue;
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
	uint8_t tankPressData[300];
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
	eLog,
	ePressureLog,
} LOG_TYPE;

void logMgr_InitLogMemory(void);

void logMgr_RecordLog(POR_LOG_T log);

uint32_t logMgr_GetNumberOfLog(void );

void logMgr_GetCurrentSetting(SettingValue* setting);

void logMgr_SendAllLog();

void logMgr_SendPressureLog();

void logMgr_DeleteAllLog(void);

void logMgr_RecordPressureLog(POR_PRESS_LOG_T*);

void logMgr_DeleteAllPressureLog(void);

#endif /* SYSTEM_INC_LOGMGR_H_ */
