/********************************************************************/
/*                                                                  */
/* File Name     : DevCtrlUniTest    .cpp                           */
/*                                                                  */
/* General       : This module contain function for Unit Test       */
/*                                                                  */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/* Company       : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*                                                                  */
/*                                                                  */
/********************************************************************/
#include "gtest.h"
using ::testing::TestWithParam;

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
        uint16_t CRC;
} POR_LOG_T;

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

    //  eMessageIDSetO2AlarmDetectionTime,
    eMessageIDClosePort = 53
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

enum {
    eEvent,
    eAlarmClear,
    eAlarmContact,
    eAlarmBreath,
    eAlarmCharge,
    eAlarmInspect,
    eAlarmPowerLost
};

uint8_t devCtrl_ProcessUSBQueue(uint8_t requestId)
{
    uint8_t returnRequestId;
    switch(requestId)
    {
        case eMessageIDGetLog:
            returnRequestId = eMessageIDGetLog;
            break;
        case eMessageIDSetTriggerSensitivity:
            returnRequestId = eMessageIDSetTriggerSensitivity;
            break;
        case eMessageIDSetVolume:
            returnRequestId = eMessageIDSetVolume;
            break;
        case eMessageIDSetOperationTimeFromPreviousMaintenanceContinuousReset:
            returnRequestId = eMessageIDSetOperationTimeFromPreviousMaintenanceContinuousReset;
            break;
        case eMessageIDSetOperationTimeFromPreviousMaintenanceSynchronizeReset:
            returnRequestId = eMessageIDSetOperationTimeFromPreviousMaintenanceSynchronizeReset;
            break;
        case eMessageIDClearHistory:
            returnRequestId = eMessageIDClearHistory;
            break;
        case eMessageIDClearPressureLog:
            returnRequestId = eMessageIDClearPressureLog;
            break;
        case eMessageIDSetDateAndTime:
            returnRequestId = eMessageIDSetDateAndTime;
            break;
        case eMessageIDGetSystemInformation:
            returnRequestId = eMessageIDGetSystemInformation;
            break;
        case eMessageIDGetPressureLog:
            returnRequestId = eMessageIDGetPressureLog;
            break;
        case eMessageIDTestAudioVolume:
            returnRequestId = eMessageIDTestAudioVolume;
            break;
        case eMessageIDSetAllLEDOn:
            returnRequestId = eMessageIDSetAllLEDOn;
            break;
        case eMessageIDSetAllLEDOff:
            returnRequestId = eMessageIDSetAllLEDOff;
            break;
        case eMessageIDSetTotalOperationTimeContinuousReset:
            returnRequestId = eMessageIDSetTotalOperationTimeContinuousReset;
            break;
        case eMessageIDSetTotalOperationTimeSynchronizeReset:
            returnRequestId = eMessageIDSetTotalOperationTimeSynchronizeReset;
            break;
        case eMessageIDSetTriggerSensitivityOffset:
            returnRequestId = eMessageIDSetTriggerSensitivityOffset;
            break;
        case eMessageIDSetTriggerSensitivityOffsetFanLow:
            returnRequestId = eMessageIDSetTriggerSensitivityOffsetFanLow;
            break;
        case eMessageIDSetTriggerSensitivityOffsetFanMid:
            returnRequestId = eMessageIDSetTriggerSensitivityOffsetFanMid;
            break;
        case eMessageIDSetTriggerSensitivityOffsetFanHigh:
            returnRequestId = eMessageIDSetTriggerSensitivityOffsetFanHigh;
            break;
        case eMessageIDSetFlowrateOffset:
            returnRequestId = eMessageIDSetFlowrateOffset;
            break;
        case eMessageIDSetO2Offset:
            returnRequestId = eMessageIDSetO2Offset;
            break;
        case eMessageIDSetDischargeVolumeOffset:
            returnRequestId = eMessageIDSetDischargeVolumeOffset;
            break;
        case eMessageIDSetPVSAPress:
            returnRequestId = eMessageIDSetPVSAPress;
            break;
        case eMessageIDSetSol1OnOffsetTime:
            returnRequestId = eMessageIDSetSol1OnOffsetTime;
            break;
        case eMessageIDSetSol3OnOffsetTime:
            returnRequestId = eMessageIDSetSol3OnOffsetTime;
            break;
        case eMessageIDSetSol2OpenTimeA:
            returnRequestId = eMessageIDSetSol2OpenTimeA;
            break;
        case eMessageIDSetSol2OpenTimeB:
            returnRequestId = eMessageIDSetSol2OpenTimeB;
            break;
        case eMessageIDSetSol1DelayOpenTime:
            returnRequestId = eMessageIDSetSol1DelayOpenTime;
            break;
        case eMessageIDSetSol3DelayOpenTime:
            returnRequestId = eMessageIDSetSol3DelayOpenTime;
            break;
        case eMessageIDSetCoolingFan:
            returnRequestId = eMessageIDSetCoolingFan;
            break;
        case eMessageIDSetFCOutput:
            returnRequestId = eMessageIDSetFCOutput;
            break;
        case eMessageIDSetCompressor:
            returnRequestId = eMessageIDSetCompressor;
            break;
        case eMessageIDSetMaxPSATime:
            returnRequestId = eMessageIDSetMaxPSATime;
            break;
        case eMessageIDClosePort:
            returnRequestId = eMessageIDClosePort;
            break;
        case eMessageIDSetTestSynchronizeMode:
            returnRequestId = eMessageIDSetTestSynchronizeMode;

            break;
        case eMessageIDIncreaseFlowrate:
            returnRequestId = eMessageIDIncreaseFlowrate;
            break;
        case eMessageIDDecreaseFlowrate:
            returnRequestId = eMessageIDDecreaseFlowrate;
            break;
        case eMessageIDChangeMode:
            returnRequestId = eMessageIDChangeMode;
            break;
        case eMessageIDTestAlarmValue:
            returnRequestId = eMessageIDTestAlarmValue;
            break;
        case eMessageIDUpgrade:
            returnRequestId = eMessageIDUpgrade;
            break;
        default:
            break;
    }
    return returnRequestId;
}

struct ProcessUSBQueue
{
        uint8_t requestId;
        uint8_t returnRequestId;
};

struct ProcessUSBQueue ProcessUSBQueueTestCase[] =
{
        {eMessageIDGetLog, eMessageIDGetLog},
        {eMessageIDSetTriggerSensitivity, eMessageIDSetTriggerSensitivity},
        {eMessageIDSetVolume,eMessageIDSetVolume},
        {eMessageIDSetOperationTimeFromPreviousMaintenanceContinuousReset,eMessageIDSetOperationTimeFromPreviousMaintenanceContinuousReset},
        {eMessageIDSetOperationTimeFromPreviousMaintenanceSynchronizeReset,eMessageIDSetOperationTimeFromPreviousMaintenanceSynchronizeReset},
        {eMessageIDClearHistory,eMessageIDClearHistory},
        {eMessageIDClearPressureLog,eMessageIDClearPressureLog},
        {eMessageIDSetDateAndTime,eMessageIDSetDateAndTime},
        {eMessageIDGetSystemInformation, eMessageIDGetSystemInformation},
        {eMessageIDGetPressureLog, eMessageIDGetPressureLog},
        {eMessageIDTestAudioVolume, eMessageIDTestAudioVolume},
        {eMessageIDSetAllLEDOn, eMessageIDSetAllLEDOn},
        {eMessageIDSetAllLEDOff, eMessageIDSetAllLEDOff},
        {eMessageIDSetTotalOperationTimeContinuousReset, eMessageIDSetTotalOperationTimeContinuousReset},
        {eMessageIDSetTotalOperationTimeSynchronizeReset, eMessageIDSetTotalOperationTimeSynchronizeReset},
        {eMessageIDSetTriggerSensitivityOffset, eMessageIDSetTriggerSensitivityOffset},
        {eMessageIDSetTriggerSensitivityOffsetFanLow, eMessageIDSetTriggerSensitivityOffsetFanLow},
        {eMessageIDSetTriggerSensitivityOffsetFanMid, eMessageIDSetTriggerSensitivityOffsetFanMid},
        {eMessageIDSetTriggerSensitivityOffsetFanHigh, eMessageIDSetTriggerSensitivityOffsetFanHigh},
        {eMessageIDSetFlowrateOffset, eMessageIDSetFlowrateOffset},
        {eMessageIDSetO2Offset, eMessageIDSetO2Offset},
        {eMessageIDSetDischargeVolumeOffset, eMessageIDSetDischargeVolumeOffset},
        {eMessageIDSetPVSAPress, eMessageIDSetPVSAPress},
        {eMessageIDSetSol1OnOffsetTime, eMessageIDSetSol1OnOffsetTime},
        {eMessageIDSetSol3OnOffsetTime, eMessageIDSetSol3OnOffsetTime},
        {eMessageIDSetSol2OpenTimeA, eMessageIDSetSol2OpenTimeA},
        {eMessageIDSetSol2OpenTimeB, eMessageIDSetSol2OpenTimeB},
        {eMessageIDSetSol1DelayOpenTime, eMessageIDSetSol1DelayOpenTime},
        {eMessageIDSetSol3DelayOpenTime, eMessageIDSetSol3DelayOpenTime},
        {eMessageIDSetCoolingFan, eMessageIDSetCoolingFan},
        {eMessageIDSetFCOutput, eMessageIDSetFCOutput},
        {eMessageIDSetCompressor, eMessageIDSetCompressor},
        {eMessageIDSetMaxPSATime, eMessageIDSetMaxPSATime},
        {eMessageIDClosePort, eMessageIDClosePort},
        {eMessageIDSetTestSynchronizeMode, eMessageIDSetTestSynchronizeMode},
        {eMessageIDIncreaseFlowrate, eMessageIDIncreaseFlowrate},
        {eMessageIDDecreaseFlowrate, eMessageIDDecreaseFlowrate},
        {eMessageIDChangeMode, eMessageIDChangeMode},
        {eMessageIDTestAlarmValue, eMessageIDTestAlarmValue},
        {eMessageIDUpgrade, eMessageIDUpgrade}
};

class ProcessUSBQueueTest: public ::testing::TestWithParam<ProcessUSBQueue>
{
};
TEST_P(ProcessUSBQueueTest, ProcessUSBQueueTestCaseList)
{
    uint8_t requestId = GetParam().requestId;
    uint8_t returnRequestId = GetParam().returnRequestId;
    EXPECT_EQ(returnRequestId, devCtrl_ProcessUSBQueue(requestId));
};

INSTANTIATE_TEST_CASE_P(ProcessUSBQueueTestCaseList,
                        ProcessUSBQueueTest,
                        testing::ValuesIn(ProcessUSBQueueTestCase)
);

uint8_t devCtrl_Handle(uint8_t gs_alarmTestType)
{

    if(gs_alarmTestType!=eEndTestAlarmID)
    {
        switch(gs_alarmTestType)
        {
            case eCompressorBoardTemperatureTestAlarmId:
                return eCompressorBoardTemperatureTestAlarmId;
                break;
            case eCompressorTemperatureTestAlarmId:
                return eCompressorTemperatureTestAlarmId;
                break;
            case eSetDefaultParameterTestAlarmId:
                return eSetDefaultParameterTestAlarmId;
                break;
            default:
                gs_alarmTestType = eEndTestAlarmID;
                break;
        }
    }
    return gs_alarmTestType;
}
struct devCtrlHandle
{
        uint8_t gs_alarmTestType;
        uint8_t expect_alarmTestType;
};
struct devCtrlHandle devCtrl_HandleTestCase[] =
{
        {eEndTestAlarmID, eEndTestAlarmID},
        {eCompressorBoardTemperatureTestAlarmId, eCompressorBoardTemperatureTestAlarmId},
        {eCompressorTemperatureTestAlarmId, eCompressorTemperatureTestAlarmId},
        {eSetDefaultParameterTestAlarmId, eSetDefaultParameterTestAlarmId},
        {2, eEndTestAlarmID},

};
class devCtrl_HandleTest: public ::testing::TestWithParam<devCtrlHandle>
{
};
TEST_P(devCtrl_HandleTest, devCtrl_HandleTestCaseList)
{
    uint8_t gs_alarmTestType = GetParam().gs_alarmTestType;
    uint8_t expect_alarmTestType = GetParam().expect_alarmTestType;
    EXPECT_EQ(expect_alarmTestType, devCtrl_Handle(gs_alarmTestType));
}
INSTANTIATE_TEST_CASE_P(devCtrl_HandleTestCaseList,
                        devCtrl_HandleTest,
                        testing::ValuesIn(devCtrl_HandleTestCase)
);

uint8_t devCtrl_UpdateAlarmMonitorStruct(uint8_t gs_alarmTestType)
{
    if(gs_alarmTestType!=eEndTestAlarmID)
    {
        switch(gs_alarmTestType)
        {
            case eProvidePressTestAlarmID:
                gs_alarmTestType = eProvidePressTestAlarmID;
                break;
            case eVacuumPressTestAlarmID:
                gs_alarmTestType = eProvidePressTestAlarmID;
                break;
            case eTankPressTestAlarmID:
                gs_alarmTestType = eProvidePressTestAlarmID;
                break;
            case eBatteryStatusTestAlarmID:
                gs_alarmTestType = eProvidePressTestAlarmID;
                break;
            case eBatteryCapacityTestAlarmID:
                gs_alarmTestType = eProvidePressTestAlarmID;
                break;
            case eO2ConcentrationTestAlarmID:
                gs_alarmTestType = eProvidePressTestAlarmID;
                break;
            case eFlowControllerStatusTestAlarmID:
                gs_alarmTestType = eProvidePressTestAlarmID;
                break;
            case eFlowControllerCommunicationTestAlarmID:
                gs_alarmTestType = eProvidePressTestAlarmID;
                break;
            case eFlowControllerResponseStatusTestAlarmID:
                gs_alarmTestType = eProvidePressTestAlarmID;
                break;
            case eCompressorStatusTestAlarmID:
                gs_alarmTestType = eProvidePressTestAlarmID;
                break;
            case eCompressorCommunicationStatusTestAlarmID:
                gs_alarmTestType = eProvidePressTestAlarmID;
                break;
            case eCompressorResponseStatusTestAlarmID:
                gs_alarmTestType = eProvidePressTestAlarmID;
                break;
            case eDifferentialPressureTestAlarmID:
                gs_alarmTestType = eProvidePressTestAlarmID;
                break;
            case eWatchDogTimerTestAlarmID:
                gs_alarmTestType = eProvidePressTestAlarmID;
                break;
            case eBatteryChargerErrorTestAlarmId:
                gs_alarmTestType = eProvidePressTestAlarmID;
                break;
            case eFlowRateTestAlarmId:
                gs_alarmTestType = eProvidePressTestAlarmID;
            default:
                break;
        }
    }
}
class TestUpdateAlarmMonitorStruct : public ::testing::TestWithParam<devCtrlHandle>
{
};
struct devCtrlHandle UpdateAlarmMonitorStructTestCase[] =
{
        {eProvidePressTestAlarmID, eProvidePressTestAlarmID},
        {eVacuumPressTestAlarmID, eProvidePressTestAlarmID},
        {eTankPressTestAlarmID, eProvidePressTestAlarmID},
        {eBatteryStatusTestAlarmID, eProvidePressTestAlarmID},
        {eBatteryCapacityTestAlarmID, eProvidePressTestAlarmID},
        {eO2ConcentrationTestAlarmID, eProvidePressTestAlarmID},
        {eFlowControllerStatusTestAlarmID, eProvidePressTestAlarmID},
        {eFlowControllerCommunicationTestAlarmID, eProvidePressTestAlarmID},
        {eFlowControllerResponseStatusTestAlarmID, eProvidePressTestAlarmID},
        {eCompressorStatusTestAlarmID, eProvidePressTestAlarmID},
        {eCompressorCommunicationStatusTestAlarmID, eProvidePressTestAlarmID},
        {eCompressorResponseStatusTestAlarmID, eProvidePressTestAlarmID},
        {eDifferentialPressureTestAlarmID, eProvidePressTestAlarmID},
        {eWatchDogTimerTestAlarmID, eProvidePressTestAlarmID},
        {eBatteryChargerErrorTestAlarmId, eProvidePressTestAlarmID},
        {eFlowRateTestAlarmId, eProvidePressTestAlarmID}
};
TEST_P(TestUpdateAlarmMonitorStruct, TestUpdateAlarmMonitorStruct)
{
    uint8_t gs_alarmTestType = GetParam().gs_alarmTestType;
    uint8_t expect_alarmTestType = GetParam().expect_alarmTestType;
    EXPECT_EQ(expect_alarmTestType, devCtrl_UpdateAlarmMonitorStruct(gs_alarmTestType));
}
INSTANTIATE_TEST_CASE_P(devCtrl_UpdateAlarmMonitorStruct,
                        TestUpdateAlarmMonitorStruct,
                        testing::ValuesIn(UpdateAlarmMonitorStructTestCase)
);
