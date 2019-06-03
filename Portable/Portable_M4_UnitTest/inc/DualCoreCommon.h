/*
 * DualCoreCommon.h
 *
 *  Created on: Dec 26, 2016
 *      Author: ThaoHa
 */

#ifndef INC_DUALCORECOMMON_H_
#define INC_DUALCORECOMMON_H_

#include <AudioMgr.h>
#include <lpc_types.h>
#include <PanelKey.h>
#include "TaskCommon.h"


/* Priority of various IRQs used in dual-core examples */
/* lower the number higher the priority */
#define IRQ_PRIO_IPC              7
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

typedef enum{
	eAlertCannula,
	eAlertCharge,
	eAlertInspect,
}E_VoiceGuidace;

typedef enum{
	eNoneAlarm,
	eLowPriority,
	eMediumPriority,
	eHighPriority,
}E_PriorityOfAlarm;

typedef enum {
	VAL_ON,
	VAL_OFF
} E_ValStatus;

typedef enum
{
	eInactive = 0,
	eActive
} E_AlarmStatus;



typedef enum
{
	eNoneAlarmId,
	eFirsAlarmId = 1,
	eAlarmBreathErr = eFirsAlarmId,
	eAlarmAbnormalOxygenConcentration,
	eAlarmLowOxygenConcentration,
	eAlarmOxygenTankPressure,
	eAlarmBatteryTemperature,
	eAlarmBatteryCharge,
	eAlarmBatteryRemainingCapacityLower,
	eAlarmPowerLostErr,
	eAlarmCompressorSupplyPressure,
	eAlarmCompressorSuctionPressure,
	eAlarmFlowcontrol,
	eAlarmSupplyPressureSensor,
	eAlarmVacuumPressureSensor,
	eAlarmTankPressureSensor,
	eAlarmDifferentialPressureSensorCommunication,
	eAlarmO2Sensor,
	eAlarmFlowControllerCommunicationNoResponse,
	eAlarmFlowControllerPressureSensor,
	eAlarmFlowControllerFlowSensor,
	eAlarmFlowControllerTemperatureSensor,
	eAlarmFlowControllerCommunication,
	eAlarmFlowControllerCommunicationCheckSum,
	eAlarmFlowControllerCommunicationCommand,
	eAlarmFlowControllerCommunicationParameter,
	eAlarmFlowControllerResponseCheckSumError,
	eAlarmFlowControllerResponseCodeError,
	eAlarmFlowControllerResponseDataLengthError,
	eAlarmCompressorCommunicationNoResponse,
	eAlarmCompressorDCOverVoltage,
	eAlarmCompressorDCLowerVoltage,
	eAlarmCompressorSoftwareOverCurrent,
	eAlarmCompressorHardwareOverCurrent,
	eAlarmCompressorBoardTemperature,
	eAlarmCompressorMotorTemperature,
	eAlarmCompressorRotationSpeed,
	eAlarmCompressorStepOut,
	eAlarmCompressorCommunication,
	eAlarmCompressorCommunicationCheckSum,
	eAlarmCompressorCommunicationCommand,
	eAlarmCompressorCommunicationParameter,
	eAlarmCompressorResponseCheckSumError,
	eAlarmCompressorResponseCodeError,
	eAlarmCompressorResponseDataLenghtError,
	eAlarmvLEDDriverCommunication,
	eAlarmAudioCommunication,
	eAlarmSpeaker,
	eAlarmSPIFlashMemory,
	eAlarmBatteryType,
	eAlarmTachypnea,
	eALarmVeryLowBattery,
	eLastAlarmId,
	NumberOfAlarm = eLastAlarmId
} E_AlarmId;

typedef enum
{
	eSynchronizedMode=0,
	eContinuosMode
} E_ModeSetting;
enum
{
	ePSATableCompressorRPM,
	ePSATableFCOutput,
	ePSATablePVSAPressure,
	ePSATableSol2OpenTime,
	ePSATableSol1AndSol3OffsetTime,
	ePSATableMaxPVSATime,
	ePSATableLastItem
};

enum
{
    eNoneCommand,
    eFlowControllerResetCommand,
    eCompressorResetCommand,
};
typedef enum{
	ePSALowTemperature,
	ePSANormalTemperature,
	ePSAHighTemperature
}E_PSATemperature;
typedef enum
{
	eCompressorNoErr = 0,
	eCommunicationError,
	eDCOverVoltageError,
	eDCLowVoltageError,
	eSoftwareError,
	eHardwareError,
	eBoardTemperatureError,
	eMotorTemperatureError,
	eRPMError,
	eMotorStepOutError,
	eCompChecksumErr,
	eCompCommandErr,
	eCompParamErr,
	eCompNoResponseErr,
	eCompResponseLengthErr,
	eCompResponseCodeErr,
	eCompResponseChecksumErr,
	eCompResponseParamErr,
	eCompressorReset,
}E_CompressorStatus;

typedef enum
{
	COMP_NORMAL_SEQ,
	COMP_RESET_SEQ,
	COMP_RESET_FOR_RESTART_SEQ,
	COMP_REBOOT_SEQ,
	COMP_REBOOT_FOR_RESTART_SEQ,
	COMP_RESTART_SEQ
}E_CompressorSequence;

typedef enum
{
	eFlowControllerNoErr,
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

typedef struct {
	E_AlarmId ID;
	E_AlarmStatus previousStatus;
	E_AlarmStatus currentStatus;
//	E_AlarmCommand command;
}ALARM_STAT_T;
typedef struct {
	E_ID_Audio ID;
	uint8_t volume;
}AUDIO_T;
typedef struct {
	E_ModeSetting operationMode;
	bool isAC100VplugIn; //1 plug in, 0 unplug in
	bool isBatteryConnected; //1 insert, 0 uninstall
	bool isBreathTrigger;// 1 Triger, 0  No trigger

	bool isAudioCommunicationError; //1 disconnection, 0 connection
	bool isDifferentialPressureSensorNoResponse;	// 1 not respond, 0 normal

	uint16_t batteryCapacity;	// remaining percent
	uint16_t batteryStatus; // 1 Error, 0 normal
	float providePressure;	//kpa
	float tankPressure; //kPa
	float vacuumPressure; //kpa

	uint16_t compressorStatus;
	uint8_t	compressorCommunicateStatus;
	E_CompressorStatus compressorResponseStatus;

	uint16_t O2Concentration; // %
	uint16_t flowRateSetting; //x100 L/min
	uint16_t flowRateSensor; // x100 L/min
	uint16_t flowPress;//x100 Kpa

	uint8_t flowControllerStatus;
	uint8_t flowControllerCommunicateStatus;
	E_FlowControllerNoAlarmStatus flowControllerResponseStatus;

	bool isSpeakerError;	//1 error, 0 normal
	E_CompressorSequence compressorSequence;
	uint8_t breathRate;
	float differentialPressure;
	bool isBatteryCharging;
	bool isInvalidBattery;
	bool isBatteryChargingError;
	E_PSATemperature PSATemperature;
//	double O2AlarmDetectionTimeOffset;
}ALARM_MONITOR_t;

typedef struct {
	float providePress;
	float tankPress;
	float vacuumPress;
	float cannualaPress;
	uint16_t o2Concentrator;
	uint16_t flowrate;
	uint16_t flowPress;
	uint16_t flowTemp;
	uint16_t motorSpeed;
	uint16_t motorTemp;
	uint16_t motorBoardTemp;
	uint16_t batteryRemainCap;
	uint8_t batteryPercentage;
	bool isInhale;
	float triggerPressure;
	uint16_t currentTriggerSensitivity;
	uint16_t currentAudioVolume;
	uint16_t currentMode;
	uint32_t totalOperationTimeContinuous;
	uint32_t totalOperationTimeSynchronize;
	uint32_t operationTimeContinuous;
	uint32_t operationTimeSynchronize;
	char compressorVersion[8];
	char flowControllerVersion[8];
	uint16_t numberOfLog;
	double triggerSensitivityOffset;
	double triggerSensitivityOffsetFanLow;
	double triggerSensitivityOffsetFanMid;
	double triggerSensitivityOffsetFanHigh;
	double flowrateOffset;
	double O2Offset;
	double dischargeVolumeOffset;
	double O2AlarmDetectionTimeOffset;
	double sol1DelayOpenTimeValue;
	double sol3DelayOpenTimeValue;

}PCAPP_MONITOR_t;

typedef enum
{
	eErrorCode,
	eOperationTimeUpdate
}E_ID_UsbQueue;

typedef struct {
	E_ID_UsbQueue ID;
	int16_t data;
}E_USBQUEUE;

enum {
	eFromUsbTask,
	eFromLogTask,
	eFromAudioTask,
	eFromBatteryTask,
	eFromAlarmTask,
};
typedef struct {
	uint8_t ID;
	int16_t data[7];
}E_DEVICEQUEUE;

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
//void MSleep(int32_t msecs);
//
///**
// * @brief	Initialize M0 and boot the core with image at \a m0_image_addr
// *
// * This function is avilable only for code that runs on M4 core, and defined in
// * m0_ImageLoader.c
// *
// * @param	m0_image_addr	: uint32_t converted pointer to M0 image address
// * @return	0 on success -1 on error
// */
//int M0Image_Boot(uint32_t m0_image_addr);
//
//#ifdef __cplusplus
//}
//#endif


#endif /* INC_DUALCORECOMMON_H_ */
