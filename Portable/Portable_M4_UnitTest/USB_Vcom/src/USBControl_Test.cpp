/*
 * USBControl_Test.cpp
 *
 *  Created on: Feb 8, 2018
 *      Author: WINDOWS
 */


#include "stdafx.h"

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"
#include "string.h"



typedef enum{
	eErrorBreath = 1,
	eErrorAbnormalOxygenConcentration,
	eErrorLowOxygenConcentration,
	eErrorOxygenTankPressure,
	eErrorBatteryTemperature,
	eErrorBatteryCharge,
	eErrorBatteryRemainingCapacityLower,
	eErrorPowerLost,
	eErrorCompressorSupplyPressure,
	eErrorCompressorVacuumPressure,
	eErrorFlowcontrol = 11,
	eErrorSupplyPressureSensor,
	eErrorVacuumPressureSensor,
	eErrorTankPressureSensor,
	eErrorDifferentialPressureSensorCommunication,
	eErrorO2Sensor,
	eErrorFlowControllerCommunicationNoResponse,
	eErrorFlowControllerPressureSensor,
	eErrorFlowControllerFlowSensor,
	eErrorFlowControllerTemperatureSensor,
	eErrorFlowControllerCommunicationCheckSum,
	eErrorFlowControllerCommunication ,//22
	eErrorFlowControllerCommunicationCommand,
	eErrorFlowControllerCommunicationParameter,
	eFlowControllerResponseCheckSumError,//25
	eFlowControllerResponseCodeError,//26
	eFlowControllerResponseDataLenghtError,//27
	eErrorCompressorCommunicationNoResponse,
	eErrorCompressorDCOverVoltage,
	eErrorCompressorDCLowerVoltage,
	eErrorCompressorSoftwareOverCurrent,
	eErrorCompressorHardwareOverCurrent,
	eErrorCompressorBoardTemperature,
	eErrorCompressorMotorTemperature,
	eErrorCompressorRotationSpeed ,
	eErrorCompressorStepOut,
	eErrorCompressorCommunication,
	eErrorCompressorCommunicationCheckSum,
	eErrorCompressorCommunicationCommand,
	eErrorCompressorCommunicationParameter,
	eErrorCompressorResponseCheckSumError,//41
	eErrorCompressorResponseCodeError,//42
	eErrorCompressorResponseDataLenghtError,//43
	eErrorLEDDriverCommunication,
	eErrorAudioCommunication,
	eErrorSpeaker,
	eErrorSPIFlashMemory,
	eErrorBatteryType,	//48
	eErrorTachypnea,

}E_ID_ERRORCODE;

enum {
	eFromUsbTask,
	eFromLogTask,
	eFromAudioTask,
	eFromBatteryTask,
	eFromAlarmTask,
};

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

typedef struct
{
	uint8_t requestId;
	int16_t requestData;
	int16_t requestTestAlarmType;
	int16_t requestTestAlarmData;
}USB_MSG_TO_DEV;


typedef struct
{
	uint16_t numberOfLog;
}LOG_MSG_TO_DEV;


typedef struct
{
	uint16_t deviceId;
}ALARM_MSG_TO_DEV;


typedef struct
{
	bool isSpeakerConnected;
}AUDIO_MSG_TO_DEV;

typedef union
{
	USB_MSG_TO_DEV 		usbMsg;
	LOG_MSG_TO_DEV 		logMgs;
	ALARM_MSG_TO_DEV	alarmMsg;
	AUDIO_MSG_TO_DEV 	audioMsg;
}DEV_MSG_DATA;

typedef struct{
	uint8_t id;
	DEV_MSG_DATA msgData;
}DEV_EVENT_STRUCT;


enum
{
	eMessageIDGetLog,
	eMessageIDGetLogACK,
	eMessageIDSetTriggerSensitivity,
	eMessageIDSetVolume,
	eMessageIDSetOperationTimeFromPreviousMaintenanceContinuousReset,
	eMessageIDSetOperationTimeFromPreviousMaintenanceSynchronizeReset,
	eMessageIDClearHistory,
	eMessageIDClearPressureLog,
	eMessageIDSetDateAndTime,
	eMessageIDGetSystemInformation,
	eMessageIDGetSystemInformationACK,
	eMessageIDGetPressureLog,
	eMessageIDGetPressureLogACK,
	eMessageIDTestAudioVolume,
	eMessageIDSetAllLEDOn,
	eMessageIDSetAllLEDOff,
	eMessageIDSetTotalOperationTimeContinuousReset,
	eMessageIDSetTotalOperationTimeSynchronizeReset,
	eMessageIDSetTriggerSensitivityOffset,
	eMessageIDSetFlowrateOffset,
	eMessageIDSetO2Offset,
	eMessageIDSetDischargeVolumeOffset,
	eMessageIDSetPVSAPress=23,
	eMessageIDSetSol1OnOffsetTime,
	eMessageIDSetSol3OnOffsetTime,
	eMessageIDSetSol2OpenTimeA=28,
	eMessageIDSetSol2OpenTimeB,
	eMessageIDSetSol1DelayOpenTime,
	eMessageIDSetSol3DelayOpenTime,
	eMessageIDSetCoolingFan,
	eMessageIDSetCompressor,
	eMessageIDSetFCOutput,
	eMessageIDSetMaxPSATime,
	eMessageIDSendOperationTimeACK,
	eMessageIDSetTestSynchronizeMode,
	eMessageIDIncreaseFlowrate,
	eMessageIDDecreaseFlowrate,
	eMessageIDChangeMode,
	eMessageIDTestAlarmValue,
	eMessageIDUpgrade,
	eMessageIDSetTriggerSensitivityOffsetFanLow,
	eMessageIDSetTriggerSensitivityOffsetFanMid,
	eMessageIDSetTriggerSensitivityOffsetFanHigh,

//	eMessageIDSetO2AlarmDetectionTime,
	eMessageIDClosePort = 53
};

enum
{
    eProvidePressTestAlarmID,
    eVacuumPressTestAlarmID,
    eTankPressTestAlarmID,
    eBatteryStatusTestAlarmID,
    eBatteryCapacityTestAlarmID,
    eO2ConcentrationTestAlarmID,
    eFlowControllerStatusTestAlarmID,
    eFlowControllerCommunicationTestAlarmID,
    eFlowControllerResponseStatusTestAlarmID,
    eCompressorStatusTestAlarmID,
    eCompressorCommunicationStatusTestAlarmID,
    eCompressorResponseStatusTestAlarmID,
	eDifferentialPressureTestAlarmID,
	eWatchDogTimerTestAlarmID,
	eCompressorBoardTemperatureTestAlarmId,
	eCompressorTemperatureTestAlarmId,
	eSetDefaultParameterTestAlarmId,
	eBatteryChargerErrorTestAlarmId,
    eFlowRateTestAlarmId,
    eEndTestAlarmID
};



enum
{
	ePortableSWVersion=2,
	eTotalOperationTimeContinuous=11,
	eTotalOperationTimeSynchronize=16,
	eOperationTimeFromPreviousMaintenanceContinuous=21,
	eOperationTimeFromPreviousMaintenanceSynchronize=26,
	eFlowControllerVersion=31,
	eCompressorVersion=38,
	eTriggerSetting=45,
	eVolumeSetting=46,
	eDate=47,
	eTime=55,
	eTriggerSensitivityOffset=61,
	eFlowrateOffset=65,
	eO2Offset=69,
	eDischargeVolumeOffset=73,
	ePortableBootloaderVersion =77,
	eSol1DelayOpenTimeValue = 86,
	eSol3DelayOpenTimeValue = 90,
	eTriggerSensitivityOffsetFanLow= 94,
	eTriggerSensitivityOffsetFanMid= 98,
	eTriggerSensitivityOffsetFanHigh= 102,
};

#define USER_PRO_ADDR_BOOT		0x1A000000

#define MESSAGE_ID_PCAPP 166
#define NUMBER_OF_RESEND 10

#define MONITOR_PACKET_SIZE 59
#define MONITOR_PACKET_TAG_SIZE 2
#define MONITOR_PACKET_TAG "A:"
#define MONITOR_DATA_SIZE 4

#define SYSTEM_INFO_PACKET_SIZE 107//95
#define SYSTEM_INFO_PACKET_TAG_SIZE 2
#define SYSTEM_INFO_PACKET_TAG "B:"

#define SYSTEM_INFO_PACKET_OPERATION_TIME_SIZE 27
#define SYSTEM_INFO_PACKET_OPERATION_TIME_TAG_SIZE 6
#define SYSTEM_INFO_PACKET_OPERATION_TIME_TAG "B:time"
#define SYSTEM_INFO_PACKET_OPERATION_TIME_DATA_SIZE 5


#define LOG_PACKET_SIZE 30
#define LOG_PACKET_TAG_SIZE 2
#define LOG_PACKET_TAG "C:"

#define LOG_NUMBER_PACKET_SIZE 11
#define LOG_NUMBER_PACKET_TAG_SIZE 6
#define LOG_NUMBER_PACKET_TAG "C:numb"

#define PRESSURE_VACUUM_LOG_PACKET_SIZE 24
#define PRESSURE_VACUUM_LOG_PACKET_TAG_SIZE 6
#define PRESSURE_VACUUM_LOG_PACKET_TAG "F:name"

#define PRESSURE_TANK_LOG_PACKET_SIZE 24
#define PRESSURE_TANK_LOG_PACKET_TAG_SIZE 6
#define PRESSURE_TANK_LOG_PACKET_TAG "D:name"

#define PRESSURE_TANK_PACKET_SIZE 50
#define PRESSURE_TANK_PACKET_TAG_SIZE 2
#define PRESSURE_TANK_PACKET_TAG "D:"

#define PRESSURE_SUPPLY_PACKET_SIZE 50
#define PRESSURE_SUPPLY_PACKET_TAG_SIZE 2
#define PRESSURE_SUPPLY_PACKET_TAG "G:"

#define PRESSURE_SUPPLY_LOG_PACKET_SIZE 24
#define PRESSURE_SUPPLY_LOG_PACKET_TAG_SIZE 6
#define PRESSURE_SUPPLY_LOG_PACKET_TAG "G:name"

#define PRESSURE_VACUUM_PACKET_SIZE 65
#define PRESSURE_VACUUM_PACKET_TAG_SIZE 2
#define PRESSURE_VACUUM_PACKET_TAG "F:"

#define ALARM_PACKET_SIZE 5
#define ALARM_PACKET_TAG_SIZE 2
#define ALARM_PACKET_TAG "E:"

#define NEGATIVE_VALUE '-'
#define POSITIVE_VALUE '0'

#define DEFAULT_VALUE_OF_ACK_NUMBER 11
#define ACK_PACKET_SIZE 10

#define SYSTEM_BYTE 0
#define MESSAGE_BYTE 1
#define ACK_BYTE 2
#define TRIGGER_SENSITIVITY_BYTE 2
#define VOLUME_BYTE 2
#define FIRST_OPERATION_TIME_BYTE 2
#define SECOND_OPERATION_TIME_BYTE 3
#define THIRD_OPERATION_TIME_BYTE 4
#define TRIGGER_SENSITIVITY_OFFSET_BYTE 3
#define TRIGGER_SENSITIVITY_OFFSET_SIGN_BYTE 2
#define FLOWRATE_OFFSET_BYTE 3
#define FLOWRATE_OFFSET_SIGN_BYTE 2
#define O2_OFFSET_BYTE 3
#define O2_OFFSET_SIGN_BYTE 2
#define DISCHARGE_VOLUME_OFFSET_BYTE 3
#define DISCHARGE_VOLUME_SIGN_BYTE 2
#define AC_CURRENT_LIMIT_BYTE 3
#define AC_CURRENT_LIMIT_SIGN_BYTE 2
#define O2_ALARM_DETECTION_TIME_OFFSET_BYTE 2
#define PVSA_PRESSURE_BYTE 3
#define SOL1_ON_OFFSET_TIME_BYTE 3
#define SOL3_ON_OFFSET_TIME_BYTE 3
#define SOL2_OPEN_TIME_A_BYTE 3
#define SOL2_OPEN_TIME_B_BYTE 3
#define COOLING_FAN_BYTE 3
#define COMPRESSOR_L_BYTE 3
#define COMPRESSOR_H_BYTE 2
#define FC_OUTPUT_BYTE 3
#define MAX_PSA_TIME_BYTE 3
#define YEAR_H_BYTE 4
#define YEAR_L_BYTE 5
#define MONTH_BYTE 3
#define DAY_BYTE 2
#define HOUR_BYTE 6
#define MINUTE_BYTE 7
#define SECOND_BYTE 8
#define BPM_BYTE 2
#define ALARM_TEST_TYPE_BYTE 2
#define ALARM_TEST_VALUE_H_BYTE 3
#define ALARM_TEST_VALUE_L_BYTE 4
#define PRESSURE_SCALE 10
#define SYSTEM_INFO_PACKET_TOTAL_OPERATION_TIME_SIZE 5
#define SYSTEM_INFO_PACKET_FLOW_CONTROLLER_VERSION_SIZE 7
#define SYSTEM_INFO_PACKET_COMPRESSOR_VERSION_SIZE 7
#define SYSTEM_INFO_PACKET_TRIGGER_SETTING_SIZE 1
#define SYSTEM_INFO_PACKET_VOLUME_SETTING_SIZE 1
#define SYSTEM_INFO_PACKET_DATE_SIZE 8
#define SYSTEM_INFO_PACKET_TIME_SIZE 6
#define SYSTEM_INFO_PACKET_PORTABLE_VERSION_SIZE 9
#define SYSTEM_INFO_PACKET_OFFSET_SIZE 4
#define SYSTEM_INFO_PACKET_BOOTLOADER_VERSION_SIZE 9
#define SYSTEM_INFO_PACKET_SOL_DELAY_OPEN_TIME_SIZE 4
#define SYSTEM_INFO_PACKET_OFFSET_SIZE 4

#define LOG_PACKET_NO_TAG_START_BYTE 0
#define LOG_PACKET_YEAR_START_BYTE 2
#define LOG_PACKET_MONTH_START_BYTE 6
#define LOG_PACKET_DAY_START_BYTE 8
#define LOG_PACKET_HOUR_START_BYTE 10
#define LOG_PACKET_MINUTE_START_BYTE 12
#define LOG_PACKET_SECOND_START_BYTE 14
#define LOG_PACKET_LOG_CODE_START_BYTE 16
#define LOG_PACKET_EVENT_OR_ALARM_START_BYTE 20
#define LOG_PACKET_OLD_MODE_START_BYTE 21
#define LOG_PACKET_OLD_FLOWRATE_START_BYTE 22
#define LOG_PACKET_OLD_TRIGGER_SENSITIVITY_START_BYTE 23
#define LOG_PACKET_NEW_MODE_START_BYTE 24
#define LOG_PACKET_NEW_FLOWRATE_START_BYTE 25
#define LOG_PACKET_NEW_TRIGGER_SENSITIVITY_START_BYTE 26

#define PRESSURE_LOG_PACKET_YEAR_START_BYTE 0
#define PRESSURE_LOG_PACKET_MONTH_START_BYTE 4
#define PRESSURE_LOG_PACKET_DAY_START_BYTE 6
#define PRESSURE_LOG_PACKET_HOUR_START_BYTE 8
#define PRESSURE_LOG_PACKET_MINUTE_START_BYTE 10
#define PRESSURE_LOG_PACKET_SECOND_START_BYTE 12
#define PRESSURE_LOG_PACKET_LOG_CODE_START_BYTE 14
#define NUMBER_OF_PRESSURE_PER_SEND 15
#define NUMBER_OF_SEND 300/NUMBER_OF_PRESSURE_PER_SEND
#define NUMBER_OF_BYTE_PER_PRESS 3
#define PRESSURE_LOG_PACKET_NO_TAG_SIZE 2
#define NUMBER_OF_BYTE_PER_VACUUM_PRESS 4

#define CONNECTED_CONSTANT_1 0
#define CONNECTED_CONSTANT_2 257

#define SEND_MONITOR_DATA_TO_PC_APP_LOOP_CNT 60/USB_TASK_DELAY_TIME

#define MAX_WAITING_CNT 2
#define NUMBER_H_LEFT_SHIFT_BIT 16
#define NUMBER_L_LEFT_SHIFT_BIT 8
#define CONVERT_CONSTANT 256
#define CHANGE_TO_NEGATIVE_CONST -1
#define TIME_OFFSET_CONST 100


#define START_MONTH_BIT 4
#define START_MDAY_BIT 6
#define START_MINUTE_BIT 2
#define START_SEC_BIT 4

#define VACUUM_PRESSURE_SIGN_BYTE 0
#define SEND_PRESSURE_LOG_TIME_OUT 9000/USB_TASK_DELAY_TIME
#define MAXIMUM_NUMBER_OF_RECEIVE_BYTE 10
#define ONE_SECOND_WAIT_FOR_RTC_SET_TIME	1000/USB_TASK_DELAY_TIME






enum
{
	eTankPress,
	eVacuumPress,
	eFlowrate,
	eO2Concentration,
	eFCTemp,
	eFCPress,
	eMotorSpeed,
	eMotorBoardTemp,
	eMotorTemp,
	eCannulaPress,
	eProvidePress,
	eBatteryRemainCapacity,
	eIsInhale,
	eTriggerPressure
};
enum
{
	eExhale,
	eInhale
};


class TestUSBcontrol : public ::testing::Test
{
public:
	TestUSBcontrol()
    {

    }
};

static void mockVcom_bread(uint8_t *pBuf, uint32_t buf_len)
{
	*(pBuf+0) = MESSAGE_ID_PCAPP;
	*(pBuf+1) = 1;
	*(pBuf+2) = 10;
}

static bool USB_sendData(uint8_t ACKID,uint16_t number)
{
	uint8_t cnt = 0;
	uint8_t numberOfSend = 0;
	bool error = false;
	uint8_t receivedACK[ACK_PACKET_SIZE];
//	vcom_write(buff,size);
	while(numberOfSend < NUMBER_OF_RESEND)
	{
//		vTaskDelay(DELAY_TICK_5MS);
		memset(receivedACK, DEFAULT_VALUE_OF_ACK_NUMBER, ACK_PACKET_SIZE);
		mockVcom_bread(&receivedACK[0], ACK_PACKET_SIZE);
		if((receivedACK[SYSTEM_BYTE] == MESSAGE_ID_PCAPP)&&(receivedACK[MESSAGE_BYTE] == ACKID)&&(receivedACK[ACK_BYTE] == number))
		{
			return error;
		}
		if(cnt >= MAX_WAITING_CNT)
		{
//			vcom_write(buff,size);
			cnt=0;
		}
		cnt++;
		numberOfSend++;
	}
	error = true;
	return error;
}


TEST(TestUSBcontrol, USB_sendData)
{
    	EXPECT_EQ(false, USB_sendData(1, 10));
    	EXPECT_EQ(true, USB_sendData(1, 1));
}



















static uint8_t USB_MessageHandle(uint8_t* buffer)
{

	uint32_t firstByte;
	uint32_t SecondByte;
	uint32_t ThirdByte;
	uint32_t operationTime;
	E_logQueueReceive logQueue;
	for(int i = 0; i < 20; i++)
		logQueue.data[i] = 0;
	DEV_EVENT_STRUCT deviceQueue;
	deviceQueue.id = eFromUsbTask;
	if(buffer[SYSTEM_BYTE]==MESSAGE_ID_PCAPP)
	{
		switch(buffer[MESSAGE_BYTE])
		{
		case eMessageIDGetLog:

			break;

		case eMessageIDSetTriggerSensitivity:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetTriggerSensitivity;
			deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_BYTE];

			break;

		case eMessageIDSetVolume:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetVolume;
			deviceQueue.msgData.usbMsg.requestData = buffer[VOLUME_BYTE];


			break;

		case eMessageIDSetOperationTimeFromPreviousMaintenanceContinuousReset:
			firstByte = buffer[FIRST_OPERATION_TIME_BYTE];
			SecondByte = buffer[SECOND_OPERATION_TIME_BYTE];
			ThirdByte = buffer[THIRD_OPERATION_TIME_BYTE];
			operationTime = (firstByte<<NUMBER_H_LEFT_SHIFT_BIT)|(SecondByte<<NUMBER_L_LEFT_SHIFT_BIT)|(ThirdByte);
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetOperationTimeFromPreviousMaintenanceContinuousReset;
			deviceQueue.msgData.usbMsg.requestData = operationTime;


			break;

		case eMessageIDSetOperationTimeFromPreviousMaintenanceSynchronizeReset:
			firstByte = buffer[FIRST_OPERATION_TIME_BYTE];
			SecondByte = buffer[SECOND_OPERATION_TIME_BYTE];
			ThirdByte = buffer[THIRD_OPERATION_TIME_BYTE];
			operationTime = (firstByte<<NUMBER_H_LEFT_SHIFT_BIT)|(SecondByte<<NUMBER_L_LEFT_SHIFT_BIT)|(ThirdByte);
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetOperationTimeFromPreviousMaintenanceSynchronizeReset;
			deviceQueue.msgData.usbMsg.requestData = operationTime;

			break;

		case eMessageIDClearHistory:
			logQueue.id = eRequestDeleteLog;


			break;

		case eMessageIDClearPressureLog:
			logQueue.id = eRequestDeletePressureLog;


			break;

		case eMessageIDSetDateAndTime:

			break;

		case eMessageIDGetSystemInformation:

			break;
		case eMessageIDGetPressureLog:

			logQueue.id = eRequestSendPressureLogForPCApp;


			break;

		case eMessageIDTestAudioVolume:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDTestAudioVolume;
			deviceQueue.msgData.usbMsg.requestData = buffer[VOLUME_BYTE];


			break;

		case eMessageIDSetAllLEDOn:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetAllLEDOn;
			deviceQueue.msgData.usbMsg.requestData = 0;


			break;

		case eMessageIDSetAllLEDOff:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetAllLEDOff;
			deviceQueue.msgData.usbMsg.requestData = 0;


			break;

		case eMessageIDSetTotalOperationTimeContinuousReset:
			firstByte = buffer[FIRST_OPERATION_TIME_BYTE];
			SecondByte = buffer[SECOND_OPERATION_TIME_BYTE];
			ThirdByte = buffer[THIRD_OPERATION_TIME_BYTE];
			operationTime = (firstByte<<NUMBER_H_LEFT_SHIFT_BIT)|(SecondByte<<NUMBER_L_LEFT_SHIFT_BIT)|(ThirdByte);
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetTotalOperationTimeContinuousReset;
			deviceQueue.msgData.usbMsg.requestData = operationTime;

			break;

		case eMessageIDSetTotalOperationTimeSynchronizeReset:
			firstByte = buffer[FIRST_OPERATION_TIME_BYTE];
			SecondByte = buffer[SECOND_OPERATION_TIME_BYTE];
			ThirdByte = buffer[THIRD_OPERATION_TIME_BYTE];
			operationTime = (firstByte<<NUMBER_H_LEFT_SHIFT_BIT)|(SecondByte<<NUMBER_L_LEFT_SHIFT_BIT)|(ThirdByte);
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetTotalOperationTimeSynchronizeReset;
			deviceQueue.msgData.usbMsg.requestData = operationTime;


			break;

		case eMessageIDSetTriggerSensitivityOffset:
			deviceQueue.msgData.usbMsg.requestId  = eMessageIDSetTriggerSensitivityOffset;
			if(buffer[TRIGGER_SENSITIVITY_OFFSET_SIGN_BYTE]==NEGATIVE_VALUE)
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE]*CHANGE_TO_NEGATIVE_CONST;
			}
			else
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE];
			}


			break;

		case eMessageIDSetTriggerSensitivityOffsetFanLow:
			deviceQueue.msgData.usbMsg.requestId  = eMessageIDSetTriggerSensitivityOffsetFanLow;
			if(buffer[TRIGGER_SENSITIVITY_OFFSET_SIGN_BYTE]==NEGATIVE_VALUE)
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE]*CHANGE_TO_NEGATIVE_CONST;
			}
			else
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE];
			}


			break;

		case eMessageIDSetTriggerSensitivityOffsetFanMid:
			deviceQueue.msgData.usbMsg.requestId  = eMessageIDSetTriggerSensitivityOffsetFanMid;
			if(buffer[TRIGGER_SENSITIVITY_OFFSET_SIGN_BYTE]==NEGATIVE_VALUE)
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE]*CHANGE_TO_NEGATIVE_CONST;
			}
			else
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE];
			}


			break;

		case eMessageIDSetTriggerSensitivityOffsetFanHigh:
			deviceQueue.msgData.usbMsg.requestId  = eMessageIDSetTriggerSensitivityOffsetFanHigh;
			if(buffer[TRIGGER_SENSITIVITY_OFFSET_SIGN_BYTE]==NEGATIVE_VALUE)
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE]*CHANGE_TO_NEGATIVE_CONST;
			}
			else
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[TRIGGER_SENSITIVITY_OFFSET_BYTE];
			}

			break;

		case eMessageIDSetFlowrateOffset:
			deviceQueue.msgData.usbMsg.requestId  = eMessageIDSetFlowrateOffset;
			if(buffer[FLOWRATE_OFFSET_SIGN_BYTE] == NEGATIVE_VALUE)
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[FLOWRATE_OFFSET_BYTE]*CHANGE_TO_NEGATIVE_CONST;
			}
			else
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[FLOWRATE_OFFSET_BYTE];
			}



			break;
		case eMessageIDSetO2Offset:
			deviceQueue.msgData.usbMsg.requestId  = eMessageIDSetO2Offset;
			if(buffer[O2_OFFSET_SIGN_BYTE]==NEGATIVE_VALUE)
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[O2_OFFSET_BYTE]*CHANGE_TO_NEGATIVE_CONST;
			}
			else
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[O2_OFFSET_BYTE];
			}


			break;
		case eMessageIDSetDischargeVolumeOffset:
			deviceQueue.msgData.usbMsg.requestId  = eMessageIDSetDischargeVolumeOffset;
			if(buffer[DISCHARGE_VOLUME_SIGN_BYTE]==NEGATIVE_VALUE)
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[DISCHARGE_VOLUME_OFFSET_BYTE]*CHANGE_TO_NEGATIVE_CONST;
			}
			else
			{
				deviceQueue.msgData.usbMsg.requestData = buffer[DISCHARGE_VOLUME_OFFSET_BYTE];
			}

			break;

		case eMessageIDSetPVSAPress:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetPVSAPress;
			deviceQueue.msgData.usbMsg.requestData = buffer[PVSA_PRESSURE_BYTE];


			break;
		case eMessageIDSetSol1OnOffsetTime:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetSol1OnOffsetTime;
			deviceQueue.msgData.usbMsg.requestData = buffer[SOL1_ON_OFFSET_TIME_BYTE]*TIME_OFFSET_CONST;


			break;
		case eMessageIDSetSol3OnOffsetTime:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetSol3OnOffsetTime;
			deviceQueue.msgData.usbMsg.requestData = buffer[SOL3_ON_OFFSET_TIME_BYTE]*TIME_OFFSET_CONST;


			break;
		case eMessageIDSetSol2OpenTimeA:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetSol2OpenTimeA;
			deviceQueue.msgData.usbMsg.requestData = buffer[SOL2_OPEN_TIME_A_BYTE]*TIME_OFFSET_CONST;


			break;
		case eMessageIDSetSol2OpenTimeB:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetSol2OpenTimeB;
			deviceQueue.msgData.usbMsg.requestData = buffer[SOL2_OPEN_TIME_B_BYTE]*TIME_OFFSET_CONST;


			break;
		case eMessageIDSetSol1DelayOpenTime:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetSol1DelayOpenTime;
			deviceQueue.msgData.usbMsg.requestData = buffer[3]*TIME_OFFSET_CONST;


			break;
		case eMessageIDSetSol3DelayOpenTime:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetSol3DelayOpenTime;
			deviceQueue.msgData.usbMsg.requestData = buffer[3]*TIME_OFFSET_CONST;


			break;
		case eMessageIDSetCoolingFan:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetCoolingFan;
			deviceQueue.msgData.usbMsg.requestData = buffer[COOLING_FAN_BYTE];


			break;
		case eMessageIDSetFCOutput:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetFCOutput;
			deviceQueue.msgData.usbMsg.requestData = buffer[FC_OUTPUT_BYTE];


			break;
		case eMessageIDSetCompressor:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetCompressor;
			deviceQueue.msgData.usbMsg.requestData = buffer[COMPRESSOR_H_BYTE]*CONVERT_CONSTANT+buffer[COMPRESSOR_L_BYTE];


			break;
		case eMessageIDSetMaxPSATime:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetMaxPSATime;
			deviceQueue.msgData.usbMsg.requestData = buffer[MAX_PSA_TIME_BYTE];


			break;
		case eMessageIDClosePort:

			break;
		case eMessageIDSetTestSynchronizeMode:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDSetTestSynchronizeMode;
			deviceQueue.msgData.usbMsg.requestData = buffer[BPM_BYTE];


			break;
		case eMessageIDIncreaseFlowrate:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDIncreaseFlowrate;
			deviceQueue.msgData.usbMsg.requestData = 0;


			break;
		case eMessageIDDecreaseFlowrate:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDDecreaseFlowrate;
			deviceQueue.msgData.usbMsg.requestData = 0;


			break;
		case eMessageIDChangeMode:

			deviceQueue.msgData.usbMsg.requestId = eMessageIDChangeMode;
			deviceQueue.msgData.usbMsg.requestData = 0;


			break;
		case eMessageIDTestAlarmValue:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDTestAlarmValue;
			deviceQueue.msgData.usbMsg.requestTestAlarmType =buffer[ALARM_TEST_TYPE_BYTE];
			deviceQueue.msgData.usbMsg.requestTestAlarmData =buffer[ALARM_TEST_VALUE_H_BYTE]*CONVERT_CONSTANT+buffer[ALARM_TEST_VALUE_L_BYTE];


			break;

		case eMessageIDUpgrade:
			deviceQueue.msgData.usbMsg.requestId = eMessageIDUpgrade;
			deviceQueue.msgData.usbMsg.requestData = 1;

			break;;
		default:
			break;
		}
	}
	return deviceQueue.msgData.usbMsg.requestId;
}


struct TestCaseList_USB_MessageHandle
{
	uint8_t id;
	uint8_t buffer[5];
};

TestCaseList_USB_MessageHandle MyTestCaseList_USB_MessageHandle[29] =
{
	    {eMessageIDSetTriggerSensitivity, {166, eMessageIDSetTriggerSensitivity}},
	    {eMessageIDSetVolume,{166, eMessageIDSetVolume}},
		{eMessageIDSetOperationTimeFromPreviousMaintenanceContinuousReset, {166, eMessageIDSetOperationTimeFromPreviousMaintenanceContinuousReset}},
		{eMessageIDSetOperationTimeFromPreviousMaintenanceSynchronizeReset, {166, eMessageIDSetOperationTimeFromPreviousMaintenanceSynchronizeReset}},
		{eMessageIDTestAudioVolume, {166, eMessageIDTestAudioVolume}},
		{eMessageIDSetAllLEDOn, {166, eMessageIDSetAllLEDOn}},
		{eMessageIDSetAllLEDOff, {166, eMessageIDSetAllLEDOff}},
		{eMessageIDSetTotalOperationTimeContinuousReset, {166, eMessageIDSetTotalOperationTimeContinuousReset}},
		{eMessageIDSetTotalOperationTimeSynchronizeReset, {166, eMessageIDSetTotalOperationTimeSynchronizeReset}},
		{eMessageIDSetTriggerSensitivityOffset, {166, eMessageIDSetTriggerSensitivityOffset}},
		{eMessageIDSetTriggerSensitivityOffsetFanLow, {166, eMessageIDSetTriggerSensitivityOffsetFanLow}},
		{eMessageIDSetTriggerSensitivityOffsetFanMid, {166, eMessageIDSetTriggerSensitivityOffsetFanMid}},
		{eMessageIDSetTriggerSensitivityOffsetFanHigh, {166, eMessageIDSetTriggerSensitivityOffsetFanHigh}},
		{eMessageIDSetFlowrateOffset, {166, eMessageIDSetFlowrateOffset}},
		{eMessageIDSetO2Offset, {166, eMessageIDSetO2Offset}},
		{eMessageIDSetPVSAPress, {166, eMessageIDSetPVSAPress}},
		{eMessageIDSetSol1OnOffsetTime, {166, eMessageIDSetSol1OnOffsetTime}},
		{eMessageIDSetSol3OnOffsetTime, {166, eMessageIDSetSol3OnOffsetTime}},//18
		{eMessageIDSetSol2OpenTimeB, {166, eMessageIDSetSol2OpenTimeB}},
		{eMessageIDSetSol1DelayOpenTime, {166, eMessageIDSetSol1DelayOpenTime}},
		{eMessageIDSetSol3DelayOpenTime, {166, eMessageIDSetSol3DelayOpenTime}},
		{eMessageIDSetFCOutput, {166, eMessageIDSetFCOutput}},
		{eMessageIDSetCompressor, {166, eMessageIDSetCompressor}},
		{eMessageIDSetMaxPSATime, {166, eMessageIDSetMaxPSATime}},
		{eMessageIDSetTestSynchronizeMode, {166, eMessageIDSetTestSynchronizeMode}},
		{eMessageIDIncreaseFlowrate, {166, eMessageIDIncreaseFlowrate}},
		{eMessageIDChangeMode, {166, eMessageIDChangeMode}},
		{eMessageIDTestAlarmValue, {166, eMessageIDTestAlarmValue}},
		{eMessageIDUpgrade, {166, eMessageIDUpgrade}},

};


class Test_USB_MessageHandle : public ::testing::TestWithParam<TestCaseList_USB_MessageHandle>
{
 protected:
  virtual void SetUp()
  {
  }

  virtual void TearDown()
  {
    // Code here will be called immediately after each test
    // (right before the destructor).
  }
};


TEST_P(Test_USB_MessageHandle, USB_MessageHandle)
{

	uint8_t buffer[5];
	uint8_t id = GetParam().id;
	for (int i = 0; i < 5; i++)
		buffer[i] = GetParam().buffer[i];
    EXPECT_EQ(id, USB_MessageHandle(buffer));
}

INSTANTIATE_TEST_CASE_P(USB_MessageHandle,Test_USB_MessageHandle, testing::ValuesIn(MyTestCaseList_USB_MessageHandle));



static void USB_sendSystemInformation()
{
	char buffer[10] = "00-01-007";
	uint8_t systemInfor[107] = "B:";
	memcpy((char*)&systemInfor[ePortableSWVersion], buffer, 10);
}

static uint32_t mock_vcom_write(uint8_t *pBuf, uint32_t len)
{
	return len;
}

uint32_t USB_sendErrorCode(E_ID_ERRORCODE code)
{
	uint8_t alarm[5] = "E:";
	char buff[3]={'\0'};
	sprintf (&buff[0], "%.2d", code);
	memcpy((char*)&alarm[2],buff,sizeof(buff));
	alarm[5 - 1]='\0';
	return mock_vcom_write(alarm,code);
}

TEST(TestUSB_sendErrorCode, USB_sendData)
{
    	EXPECT_EQ(1, USB_sendErrorCode(eErrorBreath));
    	EXPECT_EQ(49,USB_sendErrorCode(eErrorTachypnea));
}



static uint32_t mock_OperationTime_GetTotalOperationTimeContinuous(void)
{
	float currentHour = 50000;
	return (uint32_t)currentHour;
}




static void USB_sendOperationTime()
{
	char buffer[9]={'\0'};
	uint8_t systemInfor[SYSTEM_INFO_PACKET_OPERATION_TIME_SIZE]=SYSTEM_INFO_PACKET_OPERATION_TIME_TAG;

	uint32_t totalOperationTimeContinuous = mock_OperationTime_GetTotalOperationTimeContinuous();
	sprintf (buffer,"%.5d",totalOperationTimeContinuous);
	memcpy((char*)&systemInfor[SYSTEM_INFO_PACKET_OPERATION_TIME_TAG_SIZE
							   + SYSTEM_INFO_PACKET_OPERATION_TIME_DATA_SIZE*0],buffer,SYSTEM_INFO_PACKET_TOTAL_OPERATION_TIME_SIZE);

	uint32_t totalOperationTimeSynchronize = mock_OperationTime_GetTotalOperationTimeContinuous();
	sprintf (buffer,"%.5d",totalOperationTimeSynchronize);
	memcpy((char*)&systemInfor[SYSTEM_INFO_PACKET_OPERATION_TIME_TAG_SIZE
							   + SYSTEM_INFO_PACKET_OPERATION_TIME_DATA_SIZE*1],buffer,SYSTEM_INFO_PACKET_TOTAL_OPERATION_TIME_SIZE);

	uint32_t operationTimeContinuous = mock_OperationTime_GetTotalOperationTimeContinuous();
	sprintf (buffer,"%.5d",operationTimeContinuous);
	memcpy((char*)&systemInfor[SYSTEM_INFO_PACKET_OPERATION_TIME_TAG_SIZE
							   + SYSTEM_INFO_PACKET_OPERATION_TIME_DATA_SIZE*2],buffer,SYSTEM_INFO_PACKET_TOTAL_OPERATION_TIME_SIZE);

	uint32_t operationTimeSynchronize = mock_OperationTime_GetTotalOperationTimeContinuous();
	sprintf (buffer,"%.5d",operationTimeSynchronize);
	memcpy((char*)&systemInfor[SYSTEM_INFO_PACKET_OPERATION_TIME_TAG_SIZE
							   + SYSTEM_INFO_PACKET_OPERATION_TIME_DATA_SIZE*3],buffer,SYSTEM_INFO_PACKET_TOTAL_OPERATION_TIME_SIZE);

	systemInfor[SYSTEM_INFO_PACKET_OPERATION_TIME_SIZE - 1]='\0';
//	USB_sendData(systemInfor,SYSTEM_INFO_PACKET_OPERATION_TIME_SIZE,eMessageIDSendOperationTimeACK,0);
}
