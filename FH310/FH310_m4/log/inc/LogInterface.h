#ifndef INC_LOGINTERFACE_H_
#define INC_LOGINTERFACE_H_
/********************************************************************/
/*                                                                  */
/* File Name    : LogInterface.h                              		*/
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
/* #0001       2016/09/05      Linh Nguyen	  	    modify          */
/*  Improve bluetooth transfer mode									*/ 
/*	- add new event to event list									*/
/*	- Add NUM_OF_LOG_t struct										*/
/*  - Add new member to  FH310_LOG_T struct                         */
/*                                                                  */
/* #0002       2016/09/12      Linh Nguyen	  	    modify          */
/*  Add new define, change save log function protocol				*/
/* #0003       2016/09/19     Linh Nguyen	  	    modify          */
/*  Remove unuse define ID, modify some id and define				*/
/*  Add receive log function for another task						*/
/* #0004	   2017/09/28	   Quyen Ngo			modify			*/
/*			-Chang the name to eAbnormalMcuClockId					*/
/********************************************************************/
#include "board.h"
#include "portmacro.h"

#define QUEUE_WAITING 500/portTICK_PERIOD_MS
#define RECEIVE_LOG_WAITITNG 500/portTICK_PERIOD_MS
#define MAX_LOG_CBX_RQ 4
#define MAX_LOG_GUI_RQ 40
#define MAX_LOG_USB_RQ 40
#define MAX_LOG_ATP_RQ 40

#define FIRST_LOG_INDEX 1

typedef enum {
	eFirsLogId=0,
	eOperationStartLogId = 1,
	eOperationStopLogId,
	eChangingFlowRateLogId,
	eOperationRestartLogId,
	eAbnormalpoweroutageininoperationLogId =6,
	eAbnormalpressedoperationswduringunplugLogId,
	eAbnormalTemperatureLogId,
	eAbnormalHighPressureLogId,
	eAbnormalLowPressureLogId,
	eAbnormalLoweringO2ConcentrationLogId,
	eAbnormalO2ConcentrationLogId,
	eAbnormalCannulaLogId,
	eAbnormalO2ConcentrationSensorLogId,
	eAbnormalMcuClockId=23,
	eAbnormalCoolingFanLogId,
	eAbnormalFlowRateLogId =38,
	eAbnormalAnypalCommunicationLogId =53,
	eAbnormalAnypalBatteryLogId =55,
	eAbnormalAnypalRtcLogId,
	eOverCurrentIsDetectedLogId,
	eUsbCommunicationsuccessLogId =108,
	eUsbCommunicationFailureLogId,
	ePsaParameterSetLowNormalO2ConcentrationLogId =139,
	ePsaParameterSetLowLoweringO2ConcentrationLogId,
	ePsaParameterSetNormalNormalO2ConcentrationLogId,
	ePsaParameterSetNormalLoweringO2ConcentrationLogId,
	ePsaParameterSetHighNormalO2ConcentrationLogId,
	ePsaParameterSetHighLoweringO2ConcentrationLogId,
	eAbnormalCompressorCommunicationLogId=156,
	eAbnormalFilterCleaningModuleLogId =159,
	eAbnormalSpeakerLogId,
	eResetWatchdogLogId = 166,
	eAbnormalRisingCompressorOutletTemperatureLogId,
	eAbnormalCompressorOutletTemperatureLogId,
	eUseDefaultParameterLogId,
	eAbnormalNegativePressureLogId,
	eAbnormalNonvolatileMemoryLogId,
	eAbnormalExternalFlowPathPressureLogId,
	eAbnormalFlowControllerPressureSensorLogId,
	eAbnormalUltrasonicControllerPressureSensorLogId,
	eAbnormalTemperatureControllerPressureSensorLogId,
	eAbnormalFlowControllerCommunicationLogId,
	eFlowControllerCommandChecksumErrorLogId,
	eFlowControllerCommandCodeErrorLogId,
	eFlowControllerCommandParameterErrorLogId,
	eFlowControllerResponseChecksumErrorLogId,
	eFlowControllerResponseCodeErrorLogId,
	eFlowControllerResponseParameterErrorLogId,
	eFlowControllerResponseLengthErrorLogId,
	eFlowControllerNoResponseLogId,
	eAbnormalCompressorDCOverVoltageLogId,
	eAbnormalCompressorSoftwareOverCurrentLogId = 187,
	eAbnormalCompressorHardwareOverCurrentLogId,
	eAbnormalCompressorBoardTemperatureLogId,
	eCompressorRpmErrorLogId = 191,
	eOxygenCylinderConnectionDetectedLogId = 202,
	eOxygenCylinderNoConnectionDetectedLogId,
	eFlowControllerCommandTimeOutAbnormalId,
	eFlowControllerResetNotificationId,
	eCompressorCommandTimeOutAbnormalId,
	eCompressorCommandResetNotificationId,
	eAnypalLogId,
	eAnypalInsertLogId,
	eAnypalPutOutLogId,
	eChangeHourLogId,
	eChangeDayLogId,
	eNoneLog = 0xff
} LogId;

typedef enum
{
	eWait,
	eSuccess,
	eTimeout
} GetLogResult;

typedef enum
{
	eFirstLogTypeID =0,
	eMachineLog = eFirstLogTypeID,
	eServerLog,
	eAnypalLog,
	eUsageLog, //Usage for anypal
	eNumberOfLogType
} LogType;


typedef enum
{
	eGuiGetMachineLogId=0,
	eCBXGetMachineLogId,
	eCBXGetServerLogId,
	eCBXGetAnypalLogId,
	eAnypalGetUsageLogId,
	eAnypalGetAnypalLogId,
	eUSBGetMachineLogId,
	eUSBGetServerLogId,
	eUSBGetAnypalLogId,
	eUSBGetUsageLogId,
	eDeleteMachineLogId,
	eDeleteServerLogId,
	eDeleteAnypalLogId,
	eDeleteUsageLogId,
	eDeleteAllLogid,
	eCopyLogToEXTRamid,
	eNotGetID
} LogRequestID;

typedef struct
{
	LogRequestID id; //get log id
	uint32_t beginLog; //begin log want to get
	uint32_t numberOfLog; //number of log want to get
}LOG_REQUEST_t;

typedef struct
{
	uint8_t header; //byte header
	LogId ID; //log id
	uint8_t flowRateSetting; //flow rate setting
	uint16_t YYYY; // year
	uint8_t MM; //mouth
	uint8_t DD; // day
	uint8_t hh; // hour
	uint8_t min;//minute
	uint8_t ss;//second
	uint8_t SpO2;//SpO2 %
	uint8_t pulse; //pulse/min
	uint16_t reserve; //reserve bytes
}FH310_LOG_T;

typedef struct
{
	uint16_t NoMachineLog;//number of machine log
	uint16_t NoCBXLog;//number of CBX log
	uint16_t NoATPLog;//number of Anypal log
}NUM_OF_LOG_t;


uint32_t logInterface_GetNumberOfLog(LogType logtype);

void logInterface_WriteMacineLog(uint8_t flowRateST, LogId ID);

void logInterface_SendLogRequestEvent(LOG_REQUEST_t event);

void logInterface_WriteSpO2Log(FH310_LOG_T log);

void logInterface_DeleteLog(LogRequestID id);

void logInterface_WriteLogToSpiFlash(FH310_LOG_T log, LogType logType);

GetLogResult logInterface_ReceiveLogForCBX(LOG_REQUEST_t request, FH310_LOG_T *stlog, uint8_t *num);

GetLogResult logInterface_ReceiveLogForGUI(LOG_REQUEST_t request, FH310_LOG_T *stlog, uint8_t *num);

GetLogResult logInterface_ReceiveLogForUSB(LOG_REQUEST_t request, FH310_LOG_T *stlog, uint8_t *num);

GetLogResult logInterface_ReceiveLogForAnypal(LOG_REQUEST_t request, FH310_LOG_T *stlog, uint8_t *num);

#endif /* INC_LOGINTERFACE_H_ */
