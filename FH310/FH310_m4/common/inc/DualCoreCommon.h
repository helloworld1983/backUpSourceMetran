#ifndef DUALCORECOMMON_H_
#define DUALCORECOMMON_H_

/********************************************************************/
/*                                                                  */
/* File Name    : DualCoreCommon.h                      			*/
/*                                                                  */
/* General      : This file contains some defines used by			*/
/*                 	 other project on this workspace		        */
/*                                                                  */
/* Product Name  : FH310_m4                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/* #0001       2016/9/14      Quyen Ngo(MV)  	    new file        */
/*																	*/
/* #0002	   2017/09/08	  Quyen Ngo				modify			*/
/*	- Remove E_CompressorSeq										*/
/* #0003	   2017/09/08	  Viet Le				modify			*/
/*	- Edit eGotoBatteryAlarmMode->ePowerLostDetected				*/
/* #0004	   2017/09/12     Quyen Ngo				modify			*/
/*	- Remove eGotoBatteryStopmode 									*/
/*	-Change IPC_SEND_REQUEST_VALVE_OPERATION to correct naming rule */
/* #0005	   2017/09/19	  Viet Le				modify			*/
/*	- Edit eNegativePressureId->eResverd1AlarmId (E_AlarmId)		*/
/* #0006	   2017/10/07		Quyen Ngo	modify					*/
/*	 - Add new status eFcErrProcessed to E_FlowCtrlNoAlarmStatus for*/
/*	   avoid the problem that the log No response does not record when*/
/*	  FC return some err code and does not return data				*/
/* #0007	   2017/10/11		Viet Le  	modify					*/
/*	 - Remove definition TIMER3_IRQ_PRIO not use					*/
/* #0008	   2017/11/02	  Quyen Ngo				modify			*/
/*	 - Add product tank, canual, temperature AD value in Psa monitor*/
/*	 packet (Bug 2012)												*/
/********************************************************************/

#include <lpc_types.h>

/* Priority of various IRQs used in dual-core examples */
/* lower the number higher the priority */
#define IRQ_PRIO_IPC              7
#define I2S0_INT_PRIO			  3
#define WDT_INT_PRIO			  0
#define RTC_INT_PRIO			  7
#define TOUCH_INT_PRIO			  5
#define POWER_INT_PRIO			  1 //just use when upgrade boot loader and M0 must not init when using this IRQ
#define UART1_INT_PRIO			  1
#define UART2_INT_PRIO			  1
#define IRDA_IRQ_PRIORITY 		  5
#define USB0_IRQ_PRIO			  0
#define USB1_IRQ_PRIO             0

/*
 * Offset of M0 image from the starting of M4 image
 * Usually the size allocated for M0 image in scatter
 * file/ Linker Definition file / Memory configuration
 * in the IDE.
 * NOTE: Make sure when this macro is overridden, change
 * the link address of M0 in such way that the CODE section
 * starts at TARGET_XXXX address + M0_IMAGE_OFFSET
 */
#ifndef M0_IMAGE_OFFSET
#define M0_IMAGE_OFFSET      0x40000
#endif

/*
 * Absolute addresses used by both cores. Make sure these memory regions are not
 * used in DATA/CODE regions of the generated image (check the map file to verify)
 * Currently the shared memory buffers are located at the end of the SRAM area
 * 0x1008000. Last 64 bytes of the 40K area are used for M4 queue header and
 * the 64 bytes above that is used by M0 queue. This value need not be changed
 * for normal circumstances.
 */
#ifndef SHARED_MEM_M0
#define SHARED_MEM_M0          0x10089F80
#endif
#ifndef SHARED_MEM_M4
#define SHARED_MEM_M4          0x10089FC0
#endif

/* Size RAM DISK image used by FAT Filesystem */
#ifndef RAMDISK_SIZE
#define RAMDISK_SIZE           0x2000
#endif

/* Base address of various flashes */
#define SPIFI_BASE_ADDR      0x14000000
#define XFLASH_BASE_ADDR     0x1C000000
#define IFLASH_BANKA_ADDR    0x1A000000
#define IFLASH_BANKB_ADDR    0x1B000000

/*
 * Select the default device to which the image
 * be loaded, based on the board. It could be overridden
 * by the user by defining one of the TARGET_XXXXX in the
 * compiler defines.
 */
#define TARGET_IFLASH

/* Selecting base address based on Target */
#ifdef TARGET_SPIFI
#define M0_IMAGE_ADDR  (SPIFI_BASE_ADDR + M0_IMAGE_OFFSET)
#elif defined(TARGET_XFLASH)
#define M0_IMAGE_ADDR  (XFLASH_BASE_ADDR + M0_IMAGE_OFFSET)
#elif defined(TARGET_IFLASH)
#define M0_IMAGE_ADDR  IFLASH_BANKB_ADDR
#else
#error "Unknown load target!"
#endif
/**
 * \def IPCEX_QUEUE_SZ
 * Size of the IPC Queue used by this example implimentation, the current
 * implementation uses same queue size for both M0 and M4, application
 * developer can override it.
 */
#define IPCEX_QUEUE_SZ         32

/**
 * \def IPCEX_MAX_IDS
 * Maximum number of message IDs that this IPC examples uses
 */
#define IPCEX_MAX_IDS         20

#define IPCEX_ID_GUI		   1  /* GUI monoitor data event ID */
#define IPCEX_ID_DEVICE		   2  /* Device handle setting ID */
#define IPCEX_ID_IRDA		   3  /* GUI monoitor data event ID */

#define TURN_MOTOR_OFF 		0
#define TURN_LED_ON          1
#define VAL_CLOSE		0
#define VAL_OPEN		1

//Common define for GPIO port
#define GPIO_PORT(x)					(uint8_t)x

//Commom define for GPIO pin
#define GPIO_BIT(x)                     (uint8_t)x

typedef enum {
	VAL_ON,
	VAL_OFF,
	ERR
} E_ValStatus;

typedef enum {
	LOW_TEMP_NORM_02,
	NORM_TEMP_NORM_02,
	HIGH_TEMP_NORM_02,
	LOW_TEMP_LOW_02,
	NORM_TEMP_LOW_02,
	HIGH_TEMP_LOW_02
} E_PsaSelectName;

typedef enum
{
	DEFAULT_SELECT,
	EEPROM_SELECT
}E_PsaPortSelect;

typedef enum
{
	O2_CYLIN_UNKNOW_STATUS,
	O2_CYLIN_ATTACH,
	O2_CYLIN_NOTATTACH,
	O2_CYLIN_REMOVE
} E_O2CylinStatus;

typedef enum{
	eLatchValveOpen,
	eLatchValveClose,
	eLatchValveOpenClose
}E_LatchValveRequest;

typedef enum {
	eNoDeviceEventId,
	eFirstEventId,
	eFirstSettingChangeId = eFirstEventId,
	resverd1 = eFirstSettingChangeId,
	eFirstSingleSettingChangeId,
	resverd2 = eFirstSingleSettingChangeId,
	eHandshakePowerNormalId,
	eSendFirstMonitorInfoToGuiId,
	resverd3,
	eOperationswToggle,
	eOperationswToggleShort,
	eOperationswToggle2S,
	eEncoderChangeStep,
	eLatchValveCloseFinish,
	eRequestPsaParamAtStart,
	ePsaMonitorInfo,
	ePowerLostDetected,
	reserve29,
	eFLowVersion,
	eCompVersion,
	eFcRequestRecordLog,
	eCompRequestRecordLog,
	eMotorCompressorChangeId,
	eMotorRPMFunctionTestChangeId,
	eSol5TimeAChangeId,
	eSol5TimeBChangeId,
	eSolEndOpenChangeId,
	eOxygenFlowChangeId,
	ePSAPressureChangeId,
	eSolenoid1OnOffSetTimeChangeId,
	eSolenoid1OffOffSetTimeChangeId,
	eSolenoid2OnOffSetTimeChangeId,
	eSolenoid2OffOffSetTimeChangeId,
	eSolenoid3OnOffSetTimeChangeId,
	eSolenoid3OffOffSetTimeChangeId,
	eSolenoid4OnOffSetTimeChangeId,
	eSolenoid4OffOffSetTimeChangeId,
	eSolenoid5OnOffSetTimeAChangeId,
	eSolenoid5OnOffSetTimeBChangeId,
	eCoolingFanChangeId,
	eCoolingFanFunctiontestChangeId,
	eO2ConcentrationTestChangeId,
	eFlowRateTestChangeId,
	eWatchDogM0ResetChangeId,
	eBacklightChangeId,
	eMaxPSATimeChangeId,
	eDutyCycleChangeId,
	resverd4,
	resverd5,
	resverd6,
	resverd7,
	resverd8,
	resverd9,
	resverd10,
	resverd11,
	resverd12,
	eMaxTemperatureChangeId,
	eMinTemperatureChangeId,
	eLimitOxygenConcentratorChangeId,
	resverd13,
	resverd14,
	eTurnValve1ChangeId,
	eTurnValve2ChangeId,
	eTurnValve3ChangeId,
	eTurnValve4ChangeId,
	eTurnValve5ChangeId,
	eTurnMotorCoolingFANChangeId,
	eTurnMotorFilterCleaningChangeId,
	eTurnMotorCompressorChangeId,
	resverd15,
	eTurnAlarmRedLEDChangeId,
	eTurnAlarmYellowLEDChangeId,
	eTurnAlarmPowerLEDChangeId,
	resverd16,
	eDeviceBackupSystemChangeId,
	eTurnBackLightChangeId,
	resverd17,
	resverd18,
	resverd19,
	resverd20,
	eInitPacketPSAParaChangeId,
	ePacketPSAParaChangeId,
	eEnterStandbyModeChangeId,
	eEnterPatientModeChangeId,
	eEnterServiceModeChangeId,
	eEnterAlarmModeChangeId,
	eEnterAlarmExternalFlowPathModeChangeId,
	eEnterAlarmNotO2CylinderModeChangeId,
	resverd28,
	eOxyCylinderChangeId,
	ePlayLedHighAlarmChangeId,
	ePlayLedMediumAlarmChangeId,
	eStopLedAlarmChangeID,
	eFlowControllerTest1Id,
	eFlowControllerTest2Id,
	eUintCumulativeHourId,
	resverd21,
	eLastSingleSettingChangeId = resverd21,
	eLastSettingChangeId = eLastSingleSettingChangeId,
	eFirstRequestId,
	resverd22 = eFirstRequestId,
	resverd23,
	resverd24,
	resverd25,
	resverd26,
	resverd27,
	eAlarmMGSId,
	eLastRequestId = eAlarmMGSId
} E_MsgDataId;

//define alarm status
typedef enum
{
	eInactive = 0,
	eActive
} E_AlarmStatus;

//define all alarm ID
typedef enum
{
	eFirsAlarmId = 0,
	eMCUClockId = eFirsAlarmId,
	eNonvolatileMemoryId,
	ePowerOutageInInOperationId,
	ePressedOperationSWDuringUnplugId,
	eTemperatureId,
	eHighPressureId,
	eLowPressureId,
	eResverd1AlarmId,
	eCompressorDCOverVoltageId,
	eCompressorSoftwareOverCurrentId,
	eCompressorBoardTemperatureId,
	eCompressorRPMId,
	eCompressorHardwareOverCurrent,
	eFlowControllerCommunicationId,
	eFlowControllerPressureSensorId,
	eFlowControllerUltrasonicSensorId,
	eFlowControllerTemperatureSensorId,
	eFlowControllerCommandTimeOutId,
	eCoolingFanId,
	eO2ConcentrationId,
	eCompressorOutletTemperatureId,
	eFlowRateId,
	eRisingCompressorOutletTemperatureId,
	eO2ConcentrationSensorId,
	eCannulaMore27sId,
	eCannulaUnder27sId,
	eLoweringO2ConcentrationId,
	eExternalFlowPathpressureId,
	eFilterCleaningId,
	eSpeakerId,
	eOverCurrentId,
	eAnypalBatterId,
	eAnypalRTCId,
	eAnypalCommunicationId,
	eLastAlarmId,
	NumberOfAlarm = eLastAlarmId
} E_AlarmId;

//Define mode
typedef enum
{
	eFirstMode = 0,
	eStopMode = eFirstMode,
	eStandyMode,
	ePatientMode,
	eServiceMode,
	eAlarmMode,
	eAlarmExternalFlowPathMode,
	eAlarmNoOxyCylinderMode,
	eBootMode,
	ePowerErrorMode,
	eBatteryMode,
	eEndOfBatteryActionMode,
	eLastMode = eEndOfBatteryActionMode,
	eNumberOfModes
}E_ModeSetting;

typedef enum
{
	eRTC_Error,
	eBatteryError,
	eAnypalCommunicationError,
	eAnypalPullIn,
	eAnypalPullout
}E_AnypalStatus;

typedef enum
{
	eFlowControllerNoErr,
	eFcErrProcessed,
	eChecksumErr,
	eCommandErr,
	eParamErr,
	eNoResponseErr,
	eResponseLengthErr,
	eResponseCodeErr,
	eResponseCheckSumErr,
	eResponseParamErr,
	eFlowReset,
}E_FlowControllerNoAlarmStatus;

typedef enum
{
	NONE,
	FLOW_VOLTDIP,
	COMP_VOLTDIP,
	END_FLOW_VOLT_DIP,
	END_COMP_VOLT_DIP
}E_VoltDipStatus;

typedef enum {
	PSA_STANDBY,
	PSA_START,
	PSA_BASIC,
	PSA_DECOMPRESS,
	PSA_END,
}E_PsaSequence;

typedef enum {
	PSA_STANDBY_STATE,
	PSA_START_STATE,
	PSA_PASS_3S_STATE,
	PSA_PASS_20S_STATE,
	PSA_PASS_23S_STATE,
	PSA_BASIC_STATE,
	PSA_RESTART_STATE,
	PSA_END_STATE
}E_PsaSate;

typedef enum {
	PSA_START_MODE,
	PSA_NORMAL_MODE,
	PSA_END_MODE
}E_PsaMode;

typedef enum {
	PSA_WAIT_END_A,
	PSA_WAIT_END_B
}E_PsaWaitEndSeqType;

/* definition of structures */

typedef struct {
	E_AlarmId ID; //id of alarm
	E_AlarmStatus previousStatus; //previous Status
	E_AlarmStatus currentStatus; //current Status
}ALARM_STAT_T;

typedef struct {
	uint8_t psaPressure; //maximum value of producTankPressure  for control PSA
	uint8_t solenoid1OnOffsetTime; //Offset time for turn on sololenoid valve1 in second scale 10
	uint8_t solenoid1OffOffsetTime; //Offset time for turn off sololenoid valve1 in second scale 10
	uint8_t solenoid2OnOffsetTime; //Offset time for turn on sololenoid valve2 in second scale 10
	uint8_t solenoid2OffOffsetTime; //Offset time for turn off sololenoid valve2 in second scale 10
	uint8_t solenoid3OnOffsetTime; //Offset time for turn on sololenoid valve3 in second scale 10
	uint8_t solenoid3OffOffsetTime; //Offset time for turn off sololenoid valve3 in second scale 10
	uint8_t solenoid4OnOffsetTime;  //Offset time for turn on sololenoid valve4 in second scale 10
	uint8_t solenoid4OffOffsetTime; //Offset time for turn off sololenoid valve4 in second scale 10
	uint8_t solenoid5OnOffsetTimeA; //Offset time A for turn on  solenoid valve 5 in second scale 10
	uint8_t solenoid5OnOffsetTimeB; //Offset time B for turn on  solenoid valve 5 in second scale 10
	uint8_t solenoid5OpenTimeA; //open time phase A of val5 in second scale 10
	uint8_t solenoid5OpenTimeB; //open time phase B of val5 in second scale 10
	uint8_t solenoidEndOpenTime; //open time for all valve at End operation phase A in second scale 10
	uint8_t coolingFan; //cooling fan speed (duty cycle)
	uint16_t compressor; // compressor speed RPM
	uint16_t FCoutput; // flow LPM value scale 100
	uint8_t MaxPsa; //Max PSA time in seconds for switch to new cycle
	uint16_t flowRateSetting; // flow rate setting value RPM scale 100
} PSA_PARAM_PACKET_t;

typedef struct {
	int16_t productankPress; //kpa value scale * 100
	uint16_t prouctankAdc;	//Adc value of Productant
	int16_t compPosPress; //kpa value scale * 100
	int16_t cannulaPress; //kpa value scale * 100
	uint16_t canulaAdc;	//Adc value of Canula
	int16_t compNegPress; //kpa value scale * 100
	int16_t extFlowPathPress; //kpa value scale * 100
	int16_t temperature; //degC value scale * 100
	uint16_t temperatureAdc; //Adc value of temperature
	int16_t compTemperature; //degC value scale * 100
	uint16_t o2Concentrator; //% value scale * 100
	uint16_t flowMeasure; //% value scale * 100
	uint16_t compessorRpm; //rpm value
	uint16_t fanDuty; // fan duty % value
	bool coolingFanErrState; //1 Err, 0 not err
	E_ValStatus val1Status; // on/off
	E_ValStatus val2Status; // on/off
	E_ValStatus val3Status; // on/off
	E_ValStatus val4Status; // on/off
	E_ValStatus val5Status; // on/off
	uint16_t psaCycleTime; // time value in ms
	E_PsaSelectName psaParamSelect; //psa current condition based on temperature and %O2
	E_PsaPortSelect psaPortSelect; //default or EEProm value
	int16_t illuminanceVoltVal; //voltage value scale * 100
	E_O2CylinStatus oxyCylinderStatus; //o2cylynder attaching status
	bool filterPhotoErrStatus; //true error, false not
	E_PsaSequence psaCurSeq; //curent sequence of PSA
} PSA_MONITOR_PACKET_t;

typedef struct
{
	PSA_MONITOR_PACKET_t PASMonitorData;
	E_AlarmId currentAlarm;
	E_ModeSetting currentMode;
	uint16_t flowRateSetting;
}MONITOR_DATA_t;

typedef struct {
	E_MsgDataId data_id; //id of ipc msg
	uint32_t data_value; //single data value
	PSA_PARAM_PACKET_t stPsaParamPacket; //psa parameter packet send from GUI to DEVICE
	PSA_MONITOR_PACKET_t stPsaMonitorPacket; // psa monitor packet send from DEVICE to GUI
	ALARM_STAT_T stAlarmStatPacket; //alarm status packet
	uint8_t version[8]; //compressor and flow controller version
} IPC_MSG_DATA_t;

typedef struct __ipcex_msg {
	uint16_t id; // id of receive task
	IPC_MSG_DATA_t ipcMsgData; //IPC Message data structure
} ipcex_msg_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	Millisecond sleep
 *
 * Calling this function will sleep for \a msec number of milli seconds
 * by a busy wait loop. This function uses the RITIMER in LPC43XX to calculate
 * the time lapsed.
 *
 * @param	msecs	: Number of milli seconds to sleep
 * @return	None
 */
void MSleep(int32_t msecs);

/**
 * @brief	Initialize M0 and boot the core with image at \a m0_image_addr
 *
 * This function is avilable only for code that runs on M4 core, and defined in
 * m0_ImageLoader.c
 *
 * @param	m0_image_addr	: uint32_t converted pointer to M0 image address
 * @return	0 on success -1 on error
 */
int M0Image_Boot(uint32_t m0_image_addr);

#ifdef __cplusplus
}
#endif

#endif /* DUALCORECOMMON_H_ */
