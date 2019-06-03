/********************************************************************/
/*                                                                  */
/* File Name     : AlarmMgsHandlerUnitTest.cpp                      */
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

// constants, macro definition
#define LIMIT_COUNT_250MS   1
#define LIMIT_COUNT_500MS   2
#define LIMIT_COUNT_750MS   3
#define LIMIT_COUNT_1250MS  5
#define LIMIT_COUNT_1500MS  6
#define LIMIT_COUNT_2250MS  9
#define LIMIT_COUNT_2500MS  10
#define LIMIT_COUNT_2750MS  11
#define LIMIT_COUNT_3250MS  13
#define LIMIT_COUNT_3500MS  14
#define LIMIT_COUNT_4250MS  17
#define LIMIT_COUNT_10S     40

#define LIMIT_COUNT_400MS   1
#define LIMIT_COUNT_800MS   2
#define LIMIT_COUNT_1200MS  3
#define LIMIT_COUNT_2400MS  6
#define LIMIT_COUNT_4S      10
#define LIMIT_COUNT_25S     62

#define ALARM_LED_TIMER_5S_ID           1
#define ALARM_LED_TIMER_2000MS_ID       2
#define ALARM_AUDIO_TIMER_400MS_ID      3
#define ALARM_AUDIO_TIMER_250MS_ID      4
#define NUMBER_ALARM_ACTIVE    10

typedef enum{
    eNoneAlarm,
    eLowPriority,
    eMediumPriority,
    eHighPriority,
    Reset,
    ePriorityDontCare,
}E_PriorityOfAlarm;

typedef enum {
    eNoneAudio,
    eStartUpAudio,
    eTerminateAudio,
    eZeroOneZeroContAudio,
    eZeroTwoFiveContAudio,
    eZeroFiveZeroContAudio,
    eZeroSevenFiveContAudio,
    eOneZeroZeroContAudio,
    eZeroTwoFiveSyncAudio,
    eZeroFiveZeroSycAudio,
    eZeroSevenFiveSyncAudio,
    eOneZeroZeroSyncAudio,
    eOneTwoFiveSyncAudio,
    eOneFiveZeroSycAudio,
    eOneSevenFiveSyncAudio,
    eTwoZeroZeroSyncAudio,
    eTwoFiveZeroSyncAudio,
    eThreeZeroZeroSyncAudio,
    eBuzzer100msAudio,
    eBuzzer200msAudio,
    eAlertContactAudio,
    eAlertCannulaAudio,
    eAlertChargetAudio,
    eAlertInspectAudio,
    eAlertRestartAudio,
    eAlertSyncOnlyAudio,
    eAlertBattertyAudio,
    eMuteAudio,
    ePowerACAudio,
    ePowerBatteryAudio,
    ePowerLostAudio,
    eContModeAudio,
    eSyncModeAudio,
    eMaintenanceModeAudio,
    eUpdateModeAudio,
    eVolumeLevel01,
    eVolumeLevel02,
    eVolumeLevel03,
    eVolumeLevel04,
    eVolumeLevel05,
    eVolumeLevel06,
    eVolumeLevel07,
    eVolumeLevel08,
    eVolumeLevel09,
    eVolumeLevel10,
    eContAudio,
    eSyncAudio,
    eZeroOneZeroAudio,
}E_ID_Audio;

typedef enum{
    eAlertCannula,
    eAlertCharge,
    eAlertInspect,
    eAlertDontCare,
}E_VoiceGuidace;

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
    eInactive = 0,
    eActive
} E_AlarmStatus;

typedef struct {
        E_AlarmId ID;
        E_AlarmStatus currentStatus;
}ALARM_STAT_T;

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

enum {
    eEvent,
    eAlarmClear,
    eAlarmContact,
    eAlarmBreath,
    eAlarmCharge,
    eAlarmInspect,
    eAlarmPowerLost
};

E_PriorityOfAlarm gs_highestPriority = eNoneAlarm;
E_PriorityOfAlarm gs_highestPriorityEndSound = eNoneAlarm;
E_VoiceGuidace gs_voiceGuidance;

uint8_t timeTickHighAlarm = 0;
uint8_t timeTickMedAlarm = 0;
uint8_t timeTickLowAlarm = 0;
bool is_displayErrorCodeAgain = true;
bool gs_isMuteButtonTOGGLE2S = false;
bool gs_isPowerButtonTOGGLE2S = false;
bool gs_isNoAlarm = true;
bool gs_isVolumeButtonOncePress = false;
bool gs_isPowerLostOccur = false;
bool gs_checkPrecedenceVoice = false ;



E_ID_Audio alarmMsgHandler_PlayHighPriorityAlarmAudio(uint8_t timeTickHighAlarm, bool gs_checkPrecedenceVoice)
{
    E_ID_Audio eventAudio;
    if((timeTickHighAlarm == 1)
            ||(timeTickHighAlarm == LIMIT_COUNT_250MS)
            ||(timeTickHighAlarm == LIMIT_COUNT_500MS)
            ||(timeTickHighAlarm == LIMIT_COUNT_750MS)
            ||(timeTickHighAlarm == LIMIT_COUNT_1250MS)
            ||(timeTickHighAlarm == LIMIT_COUNT_1500MS)
            ||(timeTickHighAlarm == LIMIT_COUNT_2250MS)
            ||(timeTickHighAlarm == LIMIT_COUNT_2500MS)
            ||(timeTickHighAlarm == LIMIT_COUNT_2750MS)
            ||(timeTickHighAlarm == LIMIT_COUNT_3250MS)
            ||(timeTickHighAlarm == LIMIT_COUNT_3500MS)
    )
    {
        eventAudio = eBuzzer200msAudio;
    }
    else if (timeTickHighAlarm == LIMIT_COUNT_4250MS )
    {
        if (gs_checkPrecedenceVoice)
        {
            eventAudio = eAlertContactAudio;
        }
        else
        {
            eventAudio = eAlertBattertyAudio;
        }

    }

    return eventAudio;
}

struct PlayHighPriorityAlarmAudio
{
        E_ID_Audio eventAudio;
        uint8_t timeTickHighAlarm;
        bool gs_checkPrecedenceVoice;
};

struct PlayHighPriorityAlarmAudio MyPlayHighPriorityAlarmAudioTestCase[] ={
        {eBuzzer200msAudio,1, true},
        {eBuzzer200msAudio,LIMIT_COUNT_250MS, true },
        {eBuzzer200msAudio,LIMIT_COUNT_500MS, true },
        {eBuzzer200msAudio,LIMIT_COUNT_750MS, true },
        {eBuzzer200msAudio,LIMIT_COUNT_1250MS, true },
        {eBuzzer200msAudio,LIMIT_COUNT_1500MS, true },
        {eBuzzer200msAudio,LIMIT_COUNT_2250MS, true },
        {eBuzzer200msAudio,LIMIT_COUNT_2500MS, true },
        {eBuzzer200msAudio,LIMIT_COUNT_2750MS, true },
        {eBuzzer200msAudio,LIMIT_COUNT_3250MS, true },
        {eBuzzer200msAudio,LIMIT_COUNT_3500MS, true },
        {eAlertContactAudio,LIMIT_COUNT_4250MS, true },
        {eBuzzer200msAudio,1, false},
        {eBuzzer200msAudio,LIMIT_COUNT_250MS, false },
        {eBuzzer200msAudio,LIMIT_COUNT_500MS, false },
        {eBuzzer200msAudio,LIMIT_COUNT_750MS, false },
        {eBuzzer200msAudio,LIMIT_COUNT_1250MS, false },
        {eBuzzer200msAudio,LIMIT_COUNT_1500MS, false },
        {eBuzzer200msAudio,LIMIT_COUNT_2250MS, false },
        {eBuzzer200msAudio,LIMIT_COUNT_2500MS, false },
        {eBuzzer200msAudio,LIMIT_COUNT_2750MS, false },
        {eBuzzer200msAudio,LIMIT_COUNT_3250MS, false },
        {eBuzzer200msAudio,LIMIT_COUNT_3500MS, false },
        {eAlertBattertyAudio,LIMIT_COUNT_4250MS, false },

};

class TestPlayHighPriorityAlarmAudio: public ::testing::TestWithParam<PlayHighPriorityAlarmAudio>
{
};

TEST_P(TestPlayHighPriorityAlarmAudio, timeTickHighAlarm_gs_checkPrecedenceVoice)
{
    E_ID_Audio eventAudio = GetParam().eventAudio;
    uint8_t timeTickHighAlarm = GetParam().timeTickHighAlarm;
    bool gs_checkPrecedenceVoice = GetParam().gs_checkPrecedenceVoice;
    EXPECT_EQ(eventAudio,alarmMsgHandler_PlayHighPriorityAlarmAudio(timeTickHighAlarm,gs_checkPrecedenceVoice));
}

INSTANTIATE_TEST_CASE_P(TestPlayHighPriorityAlarmAudio,
                        TestPlayHighPriorityAlarmAudio,
                        testing::ValuesIn(MyPlayHighPriorityAlarmAudioTestCase)
);

E_ID_Audio alarmMsgHandler_PlayLowPriorityAlarmAudio(uint8_t timeTickLowAlarm, E_PriorityOfAlarm gs_highestPriority)
{
    E_ID_Audio eventAudio = eNoneAudio;
    if (timeTickLowAlarm == LIMIT_COUNT_400MS)
    {
        eventAudio = eBuzzer200msAudio;
    }
    else if(timeTickLowAlarm == LIMIT_COUNT_800MS)
    {
        eventAudio = eBuzzer200msAudio;

    }
    else if(timeTickLowAlarm == LIMIT_COUNT_1200MS)
    {
        eventAudio = eBuzzer200msAudio;

        if(gs_highestPriority == eNoneAlarm)
        {
            gs_highestPriorityEndSound = eNoneAlarm;
        }
    }
    return eventAudio;
}

struct PlayLowPriorityAlarmAudio
{
        uint8_t timeTickLowAlarm;
        E_PriorityOfAlarm gs_highestPriority;
        E_ID_Audio eventAudio;
};

struct PlayLowPriorityAlarmAudio PlayLowPriorityAlarmAudioTestCaseList[] = {
        {0, eNoneAlarm, eNoneAudio},
        {LIMIT_COUNT_400MS, eNoneAlarm, eBuzzer200msAudio},
        {LIMIT_COUNT_800MS, eNoneAlarm, eBuzzer200msAudio},
        {LIMIT_COUNT_1200MS, eNoneAlarm, eBuzzer200msAudio},
        {0, eLowPriority,eNoneAudio},
        {LIMIT_COUNT_400MS, eLowPriority, eBuzzer200msAudio},
        {LIMIT_COUNT_800MS, eLowPriority, eBuzzer200msAudio},
        {LIMIT_COUNT_1200MS, eLowPriority, eBuzzer200msAudio},
        {0, eMediumPriority,eNoneAudio},
        {LIMIT_COUNT_400MS, eMediumPriority, eBuzzer200msAudio},
        {LIMIT_COUNT_800MS, eMediumPriority, eBuzzer200msAudio},
        {LIMIT_COUNT_1200MS, eMediumPriority, eBuzzer200msAudio},
        {0, eHighPriority,eNoneAudio},
        {LIMIT_COUNT_400MS, eHighPriority, eBuzzer200msAudio},
        {LIMIT_COUNT_800MS, eHighPriority, eBuzzer200msAudio},
        {LIMIT_COUNT_1200MS, eHighPriority, eBuzzer200msAudio},
};

class TestPlayLowPriorityAlarmAudio: public ::testing::TestWithParam<PlayLowPriorityAlarmAudio>
{
};


TEST_F(TestPlayLowPriorityAlarmAudio, timeTickLowAlarmIsLIMIT_COUNT_1200MS_gs_highestPriorityIseNoneAlarm)
{
    gs_highestPriorityEndSound = Reset;
    alarmMsgHandler_PlayLowPriorityAlarmAudio(LIMIT_COUNT_1200MS, eNoneAlarm);
    EXPECT_EQ(eNoneAlarm, gs_highestPriorityEndSound);

    gs_highestPriorityEndSound = Reset;
    alarmMsgHandler_PlayLowPriorityAlarmAudio(0, eNoneAlarm);
    EXPECT_EQ(Reset, gs_highestPriorityEndSound);
}

TEST_P(TestPlayLowPriorityAlarmAudio, PlayLowPriorityAlarmAudioTestCaseList)
{
    E_ID_Audio eventAudio = GetParam().eventAudio;
    uint8_t timeTickLowAlarm = GetParam().timeTickLowAlarm;
    E_PriorityOfAlarm gs_highestPriority = GetParam().gs_highestPriority;

    EXPECT_EQ(eventAudio, alarmMsgHandler_PlayLowPriorityAlarmAudio(timeTickLowAlarm, gs_highestPriority));
}

INSTANTIATE_TEST_CASE_P(TestPlayLowPriorityAlarmAudio,
                        TestPlayLowPriorityAlarmAudio,
                        testing::ValuesIn(PlayLowPriorityAlarmAudioTestCaseList)
);

E_ID_Audio alarmMsgHandler_PlayMediumPriorityAlarmAudio(uint8_t timeTickMedAlarm, E_VoiceGuidace gs_voiceGuidance)
{
    E_ID_Audio eventAudio = eNoneAudio;
    if ((timeTickMedAlarm == LIMIT_COUNT_400MS)
            ||(timeTickMedAlarm == LIMIT_COUNT_800MS)
            ||(timeTickMedAlarm == LIMIT_COUNT_1200MS))
    {
        eventAudio =  eBuzzer200msAudio;

    }
    else if(timeTickMedAlarm == LIMIT_COUNT_2400MS)
    {
        if (gs_voiceGuidance == eAlertCannula)
        {
            eventAudio = eAlertCannulaAudio;

            if(gs_highestPriority == eNoneAlarm)
            {
                gs_highestPriorityEndSound = eNoneAlarm;
            }
        }
        else if (gs_voiceGuidance == eAlertCharge)
        {
            eventAudio = eAlertChargetAudio;

            if(gs_highestPriority == eNoneAlarm)
            {
                gs_highestPriorityEndSound = eNoneAlarm;
            }
        }
        else
        {
            eventAudio = eAlertInspectAudio;

            if(gs_highestPriority == eNoneAlarm)
            {
                gs_highestPriorityEndSound = eNoneAlarm;
            }
        }
    }

    return eventAudio;
}

struct PlayMediumPriorityAlarmAudio
{
        uint8_t timeTickMedAlarm;
        E_VoiceGuidace gs_voiceGuidance;
        E_ID_Audio eventAudio;
};

struct PlayMediumPriorityAlarmAudio PlayMediumPriorityAlarmAudioTestCaseList[] =
{
        {LIMIT_COUNT_400MS, eAlertDontCare,  eBuzzer200msAudio},
        {LIMIT_COUNT_800MS, eAlertDontCare,  eBuzzer200msAudio},
        {LIMIT_COUNT_1200MS, eAlertDontCare,  eBuzzer200msAudio},

        {LIMIT_COUNT_2400MS, eAlertCannula,  eAlertCannulaAudio},
        {LIMIT_COUNT_2400MS, eAlertCharge,  eAlertChargetAudio},
        {LIMIT_COUNT_2400MS, eAlertInspect,  eAlertInspectAudio},

        {0, eAlertDontCare,  eNoneAudio}

};

class PlayMediumPriorityAlarmAudioTest: public ::testing::TestWithParam<PlayMediumPriorityAlarmAudio>
{
};

TEST_P(PlayMediumPriorityAlarmAudioTest, PlayMediumPriorityAlarmAudioTestCaseList)
{
    uint8_t timeTickMedAlarm = GetParam().timeTickMedAlarm;
    E_VoiceGuidace gs_voiceGuidance = GetParam().gs_voiceGuidance;
    E_ID_Audio eventAudio = GetParam().eventAudio;

    EXPECT_EQ(eventAudio, alarmMsgHandler_PlayMediumPriorityAlarmAudio(timeTickMedAlarm, gs_voiceGuidance));
}

INSTANTIATE_TEST_CASE_P(PlayMediumPriorityAlarmAudioTest,
                        PlayMediumPriorityAlarmAudioTest,
                        testing::ValuesIn(PlayMediumPriorityAlarmAudioTestCaseList)
);

TEST_F(PlayMediumPriorityAlarmAudioTest, gs_highestPriorityIseNoneAlarm)
{
    gs_highestPriority = eNoneAlarm;
    gs_highestPriorityEndSound = Reset;

    alarmMsgHandler_PlayMediumPriorityAlarmAudio(0, eAlertCannula);
    EXPECT_NE(eNoneAlarm, gs_highestPriorityEndSound);

    alarmMsgHandler_PlayMediumPriorityAlarmAudio(0, eAlertCharge);
    EXPECT_NE(eNoneAlarm, gs_highestPriorityEndSound);

    alarmMsgHandler_PlayMediumPriorityAlarmAudio(0, eAlertInspect);
    EXPECT_NE(eNoneAlarm, gs_highestPriorityEndSound);

    alarmMsgHandler_PlayMediumPriorityAlarmAudio(LIMIT_COUNT_2400MS, eAlertCannula);
    EXPECT_EQ(eNoneAlarm, gs_highestPriorityEndSound);

    alarmMsgHandler_PlayMediumPriorityAlarmAudio(LIMIT_COUNT_2400MS, eAlertCharge);
    EXPECT_EQ(eNoneAlarm, gs_highestPriorityEndSound);

    alarmMsgHandler_PlayMediumPriorityAlarmAudio(LIMIT_COUNT_2400MS, eAlertInspect);
    EXPECT_EQ(eNoneAlarm, gs_highestPriorityEndSound);

    gs_highestPriority = Reset;
    gs_highestPriorityEndSound = Reset;
    alarmMsgHandler_PlayMediumPriorityAlarmAudio(LIMIT_COUNT_2400MS, eAlertCannula);
    EXPECT_EQ(Reset, gs_highestPriorityEndSound);

    alarmMsgHandler_PlayMediumPriorityAlarmAudio(LIMIT_COUNT_2400MS, eAlertCharge);
    EXPECT_EQ(Reset, gs_highestPriorityEndSound);

    alarmMsgHandler_PlayMediumPriorityAlarmAudio(LIMIT_COUNT_2400MS, eAlertInspect);
    EXPECT_EQ(Reset, gs_highestPriorityEndSound);
}

E_PriorityOfAlarm alarmMsgHandler_PlayAlarmAudio(bool gs_isMuteButtonTOGGLE2S, bool gs_isPowerButtonTOGGLE2S,
                                                 bool  is_displayErrorCodeAgain, E_PriorityOfAlarm gs_highestPriorityEndSound)
{
    E_PriorityOfAlarm temp = Reset;
    if (gs_isMuteButtonTOGGLE2S == true)
    {
        temp = eNoneAlarm;

    }
    if ((gs_isPowerButtonTOGGLE2S == false)&&(is_displayErrorCodeAgain == true))
    {
        switch (gs_highestPriorityEndSound)
        {
            case eLowPriority:
                temp = eLowPriority;
                break;
            case eMediumPriority:
                temp = eMediumPriority;
                break;
            case eHighPriority:
                temp = eHighPriority;
                break;
            default:
                temp = Reset;
                break;
        }
    }
    return temp;
}

struct PlayAlarmAudio
{
        bool gs_isMuteButtonTOGGLE2S;
        bool gs_isPowerButtonTOGGLE2S;
        bool  is_displayErrorCodeAgain;
        E_PriorityOfAlarm gs_highestPriorityEndSound;
        E_PriorityOfAlarm temp;
};

struct PlayAlarmAudio PlayAlarmAudioTestCase[] =
{
        {false, true, true, ePriorityDontCare, Reset},
        {false, true, false,ePriorityDontCare, Reset},
        {false, false, true,ePriorityDontCare, Reset},
        {false, false, false,ePriorityDontCare, Reset},

        {true, true, true,ePriorityDontCare, eNoneAlarm},
        {true, true, false,ePriorityDontCare, eNoneAlarm},
        {true, false, false,ePriorityDontCare, eNoneAlarm},

        {false, false, true,eLowPriority, eLowPriority},
        {false, false, true,eMediumPriority, eMediumPriority},
        {false, false, true,eHighPriority, eHighPriority},
        {false, false, true,ePriorityDontCare, Reset}
};

class PlayAlarmAudioTest : public ::testing::TestWithParam<PlayAlarmAudio>
{
};

TEST_P(PlayAlarmAudioTest, PlayAlarmAudioTestTestCaseList)
{
    bool gs_isMuteButtonTOGGLE2S = GetParam().gs_isMuteButtonTOGGLE2S;
    bool gs_isPowerButtonTOGGLE2S = GetParam().gs_isPowerButtonTOGGLE2S;
    bool  is_displayErrorCodeAgain = GetParam().is_displayErrorCodeAgain;
    E_PriorityOfAlarm gs_highestPriorityEndSound = GetParam().gs_highestPriorityEndSound;
    E_PriorityOfAlarm temp = GetParam().temp;

    EXPECT_EQ(temp, alarmMsgHandler_PlayAlarmAudio(gs_isMuteButtonTOGGLE2S, gs_isPowerButtonTOGGLE2S,
                                                   is_displayErrorCodeAgain, gs_highestPriorityEndSound));
}

INSTANTIATE_TEST_CASE_P(PlayAlarmAudioTestTestCaseList,
                        PlayAlarmAudioTest,
                        testing::ValuesIn(PlayAlarmAudioTestCase)
);

void alarmMsgHandler_SendAlarmLog(ALARM_STAT_T alarm_Receive, uint16_t &logCode, uint8_t &EventOrAlarm)
{
    switch (alarm_Receive.ID)
    {
        case eAlarmBreathErr:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eBreathErrorLogId;
                EventOrAlarm = eAlarmBreath;
            }
            break;
        case eAlarmAbnormalOxygenConcentration:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eAbnormalO2ConcentrationErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmLowOxygenConcentration:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eLowO2ConcentrationLogId;
                EventOrAlarm = eAlarmInspect;
            }
            break;
        case eAlarmOxygenTankPressure:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eOxygenTankPressureErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmBatteryTemperature:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eBatteryTemperatureErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmBatteryCharge:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eBatteryChargeErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmBatteryRemainingCapacityLower:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eBatteryRemaininingCapacityLowerLogId;
                EventOrAlarm = eAlarmCharge;
            }
            break;
        case eAlarmPowerLostErr:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = ePowerLost;
                EventOrAlarm = eAlarmPowerLost;
            }
            else
            {
                logCode = ePowerLostClear;
                EventOrAlarm = eAlarmClear;
            }
            break;
        case eAlarmCompressorSupplyPressure:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorSupplyPressureErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmCompressorSuctionPressure:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorVacuumPressureErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmFlowcontrol:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eFlowControllerErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmSupplyPressureSensor:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eSupplyPressureSensorErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmVacuumPressureSensor:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eVacuumPressureSensorErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmTankPressureSensor:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eTankPressureSensorErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmDifferentialPressureSensorCommunication:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eDifferentialPressureSensorCommunicationErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmO2Sensor:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eFlowControllerO2SensorErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmFlowControllerCommunicationNoResponse:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eFlowControllerCommunicationNoResponsemErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmFlowControllerPressureSensor:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eFlowControllerPressureSensorErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmFlowControllerFlowSensor:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eFlowControllerFlowSensorErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmFlowControllerTemperatureSensor:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eFlowControllerTemperatureSensorErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmFlowControllerCommunication:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eFlowControllerCommunicationCOMErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmFlowControllerCommunicationCheckSum:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eFlowControllerCommunicationCommandCheckSumErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmFlowControllerCommunicationCommand:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eFlowControllerCommunicationCommandCodeErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmFlowControllerCommunicationParameter:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eFlowControllerCommunicationCommandParameterErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmFlowControllerResponseCheckSumError:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eFlowControllerCommunicationResponseCheckSumErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmFlowControllerResponseCodeError:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eFlowControllerCommunicationResponseCodeErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmFlowControllerResponseDataLengthError:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eFlowControllerCommunicationResponseDataLengthErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmCompressorCommunicationNoResponse:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorDriverCommunicationNoResponseErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmCompressorDCOverVoltage:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorDriverDCOverVoltageErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmCompressorDCLowerVoltage:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorDriverDCLowerVoltageErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmCompressorSoftwareOverCurrent:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorDriverSoftwareOverCurrentErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmCompressorHardwareOverCurrent:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorDriverHardwareOverCurrentErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmCompressorBoardTemperature:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorDriverBoardTemperatureErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmCompressorMotorTemperature:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorDriverMotorTemperatureErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmCompressorRotationSpeed:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorDriverRotationSpeedErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmCompressorStepOut:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorDriverStepOutErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmCompressorCommunication:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorDriverCommunicationCommunicationErrorErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmCompressorCommunicationCheckSum:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorDriverCommunicationCommandCheckSumErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmCompressorCommunicationCommand:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorDriverCommunicationCommandCodeErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmCompressorCommunicationParameter:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorDriverCommunicationCommandParameterErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmCompressorResponseCheckSumError:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorDriverCommunicationResponseCheckSumErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmCompressorResponseCodeError:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorDriverCommunicationResponseCodeErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmCompressorResponseDataLenghtError:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eCompressorDriverCommunicationResponseDataLengthErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmSpeaker:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eSpeakerErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmBatteryType:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eBatteryTypeErrorLogId;
                EventOrAlarm = eAlarmContact;
            }
            break;
        case eAlarmTachypnea:
            if(alarm_Receive.currentStatus == eActive)
            {
                logCode = eTachypneaError;
                EventOrAlarm = eAlarmBreath;
            }
            break;
        default:
            break;
    }
}

struct SendAlarmLog
{
        ALARM_STAT_T alarm_Receive;
        uint16_t logCode;
        uint8_t EventOrAlarm;
};

struct SendAlarmLog SendAlarmLogTestCase[] =
{
        {eAlarmBreathErr, eActive, eBreathErrorLogId, eAlarmBreath},
        {eAlarmAbnormalOxygenConcentration, eActive, eAbnormalO2ConcentrationErrorLogId, eAlarmContact},
        {eAlarmLowOxygenConcentration, eActive, eLowO2ConcentrationLogId, eAlarmInspect},
        {eAlarmOxygenTankPressure, eActive, eOxygenTankPressureErrorLogId, eAlarmContact},
        {eAlarmBatteryTemperature, eActive, eBatteryTemperatureErrorLogId, eAlarmContact},
        {eAlarmBatteryCharge, eActive, eBatteryChargeErrorLogId, eAlarmContact},
        {eAlarmBatteryRemainingCapacityLower, eActive, eBatteryRemaininingCapacityLowerLogId, eAlarmCharge},
        {eAlarmPowerLostErr, eActive, ePowerLost, eAlarmPowerLost},
        {eAlarmPowerLostErr, eInactive, ePowerLostClear, eAlarmClear},
        {eAlarmCompressorSupplyPressure, eActive, eCompressorSupplyPressureErrorLogId, eAlarmContact},
        {eAlarmCompressorSuctionPressure, eActive, eCompressorVacuumPressureErrorLogId, eAlarmContact},
        {eAlarmFlowcontrol, eActive, eFlowControllerErrorLogId, eAlarmContact},
        {eAlarmSupplyPressureSensor, eActive, eSupplyPressureSensorErrorLogId, eAlarmContact},
        {eAlarmVacuumPressureSensor, eActive, eVacuumPressureSensorErrorLogId, eAlarmContact},
        {eAlarmTankPressureSensor, eActive, eTankPressureSensorErrorLogId, eAlarmContact},
        {eAlarmDifferentialPressureSensorCommunication, eActive, eDifferentialPressureSensorCommunicationErrorLogId, eAlarmContact},
        {eAlarmO2Sensor, eActive, eFlowControllerO2SensorErrorLogId, eAlarmContact},
        {eAlarmFlowControllerCommunicationNoResponse, eActive, eFlowControllerCommunicationNoResponsemErrorLogId, eAlarmContact},
        {eAlarmFlowControllerPressureSensor, eActive, eFlowControllerPressureSensorErrorLogId, eAlarmContact},
        {eAlarmFlowControllerFlowSensor, eActive, eFlowControllerFlowSensorErrorLogId, eAlarmContact},
        {eAlarmFlowControllerTemperatureSensor, eActive, eFlowControllerTemperatureSensorErrorLogId, eAlarmContact},
        {eAlarmFlowControllerCommunication, eActive, eFlowControllerCommunicationCOMErrorLogId, eAlarmContact},
        {eAlarmFlowControllerCommunicationCheckSum, eActive, eFlowControllerCommunicationCommandCheckSumErrorLogId, eAlarmContact},
        {eAlarmFlowControllerCommunicationCommand, eActive, eFlowControllerCommunicationCommandCodeErrorLogId, eAlarmContact},
        {eAlarmFlowControllerCommunicationParameter, eActive, eFlowControllerCommunicationCommandParameterErrorLogId, eAlarmContact},
        {eAlarmFlowControllerResponseCheckSumError, eActive, eFlowControllerCommunicationResponseCheckSumErrorLogId, eAlarmContact},
        {eAlarmFlowControllerResponseCodeError, eActive, eFlowControllerCommunicationResponseCodeErrorLogId, eAlarmContact},
        {eAlarmFlowControllerResponseDataLengthError, eActive, eFlowControllerCommunicationResponseDataLengthErrorLogId, eAlarmContact},
        {eAlarmCompressorCommunicationNoResponse, eActive, eCompressorDriverCommunicationNoResponseErrorLogId, eAlarmContact},
        {eAlarmCompressorDCOverVoltage, eActive, eCompressorDriverDCOverVoltageErrorLogId, eAlarmContact},
        {eAlarmCompressorDCLowerVoltage, eActive, eCompressorDriverDCLowerVoltageErrorLogId, eAlarmContact},
        {eAlarmCompressorSoftwareOverCurrent, eActive, eCompressorDriverSoftwareOverCurrentErrorLogId, eAlarmContact},
        {eAlarmCompressorHardwareOverCurrent, eActive, eCompressorDriverHardwareOverCurrentErrorLogId, eAlarmContact},
        {eAlarmCompressorBoardTemperature, eActive, eCompressorDriverBoardTemperatureErrorLogId, eAlarmContact},
        {eAlarmCompressorMotorTemperature, eActive, eCompressorDriverMotorTemperatureErrorLogId, eAlarmContact},
        {eAlarmCompressorRotationSpeed, eActive, eCompressorDriverRotationSpeedErrorLogId, eAlarmContact},
        {eAlarmCompressorStepOut, eActive, eCompressorDriverStepOutErrorLogId, eAlarmContact},
        {eAlarmCompressorCommunication, eActive, eCompressorDriverCommunicationCommunicationErrorErrorLogId, eAlarmContact},
        {eAlarmCompressorCommunicationCheckSum, eActive, eCompressorDriverCommunicationCommandCheckSumErrorLogId, eAlarmContact},
        {eAlarmCompressorCommunicationCommand, eActive, eCompressorDriverCommunicationCommandCodeErrorLogId, eAlarmContact},
        {eAlarmCompressorCommunicationParameter, eActive, eCompressorDriverCommunicationCommandParameterErrorLogId, eAlarmContact},
        {eAlarmCompressorResponseCheckSumError, eActive, eCompressorDriverCommunicationResponseCheckSumErrorLogId, eAlarmContact},
        {eAlarmCompressorResponseCodeError, eActive, eCompressorDriverCommunicationResponseCodeErrorLogId, eAlarmContact},
        {eAlarmCompressorResponseDataLenghtError, eActive, eCompressorDriverCommunicationResponseDataLengthErrorLogId, eAlarmContact},
        {eAlarmSpeaker, eActive, eSpeakerErrorLogId, eAlarmContact},
        {eAlarmBatteryType, eActive, eBatteryTypeErrorLogId, eAlarmContact},
        {eAlarmTachypnea, eActive, eTachypneaError, eAlarmBreath},

        {eAlarmBreathErr, eInactive, 0, 0},
        {eAlarmAbnormalOxygenConcentration, eInactive, 0, },
        {eAlarmLowOxygenConcentration, eInactive, 0, 0},
        {eAlarmOxygenTankPressure, eInactive, 0, 0},
        {eAlarmBatteryTemperature, eInactive, 0, 0},
        {eAlarmBatteryCharge, eInactive, 0, 0},
        {eAlarmBatteryRemainingCapacityLower, eInactive, 0, 0},
        {eAlarmCompressorSupplyPressure, eInactive, 0, 0},
        {eAlarmCompressorSuctionPressure, eInactive, 0, 0},
        {eAlarmFlowcontrol, eInactive, 0, 0},
        {eAlarmSupplyPressureSensor, eInactive, 0, 0},
        {eAlarmVacuumPressureSensor, eInactive, 0, 0},
        {eAlarmTankPressureSensor, eInactive, 0, 0},
        {eAlarmDifferentialPressureSensorCommunication, eInactive, 0, 0},
        {eAlarmO2Sensor, eInactive, 0, 0},
        {eAlarmFlowControllerCommunicationNoResponse, eInactive, 0, 0},
        {eAlarmFlowControllerPressureSensor, eInactive, 0, 0},
        {eAlarmFlowControllerFlowSensor, eInactive, 0, 0},
        {eAlarmFlowControllerTemperatureSensor, eInactive, 0, 0},
        {eAlarmFlowControllerCommunication, eInactive, 0, 0},
        {eAlarmFlowControllerCommunicationCheckSum, eInactive, 0, 0},
        {eAlarmFlowControllerCommunicationCommand, eInactive, 0, 0},
        {eAlarmFlowControllerCommunicationParameter, eInactive, 0, 0},
        {eAlarmFlowControllerResponseCheckSumError, eInactive, 0, 0},
        {eAlarmFlowControllerResponseCodeError, eInactive, 0, 0},
        {eAlarmFlowControllerResponseDataLengthError, eInactive, 0, 0},
        {eAlarmCompressorCommunicationNoResponse, eInactive, 0, 0},
        {eAlarmCompressorDCOverVoltage, eInactive, 0, 0},
        {eAlarmCompressorDCLowerVoltage, eInactive, 0, 0},
        {eAlarmCompressorSoftwareOverCurrent, eInactive, 0, 0},
        {eAlarmCompressorHardwareOverCurrent, eInactive, 0, 0},
        {eAlarmCompressorBoardTemperature, eInactive, 0, 0},
        {eAlarmCompressorMotorTemperature, eInactive, 0, 0},
        {eAlarmCompressorRotationSpeed, eInactive, 0, 0},
        {eAlarmCompressorStepOut, eInactive, 0, 0},
        {eAlarmCompressorCommunication, eInactive, 0, 0},
        {eAlarmCompressorCommunicationCheckSum, eInactive, 0, 0},
        {eAlarmCompressorCommunicationCommand, eInactive, 0, 0},
        {eAlarmCompressorCommunicationParameter, eInactive, 0, 0},
        {eAlarmCompressorResponseCheckSumError, eInactive, 0, 0},
        {eAlarmCompressorResponseCodeError, eInactive, 0, 0},
        {eAlarmCompressorResponseDataLenghtError, eInactive, 0, 0},
        {eAlarmSpeaker, eInactive, 0, 0},
        {eAlarmBatteryType, eInactive, 0, 0},
        {eAlarmTachypnea, eInactive, 0, 0}
};

class SendAlarmLogTest : public ::testing::TestWithParam<SendAlarmLog>
{
};

TEST_P(SendAlarmLogTest, SendAlarmLogTestCaseList)
{
    ALARM_STAT_T alarm_Receive = GetParam().alarm_Receive;

    uint16_t logCodeInPut = 0;
    uint16_t logCode = GetParam().logCode;
    uint8_t EventOrAlarmInput = 0;
    uint8_t EventOrAlarm = GetParam().EventOrAlarm;

    alarmMsgHandler_SendAlarmLog(alarm_Receive, logCodeInPut, EventOrAlarmInput);
    EXPECT_EQ(logCode, logCodeInPut);
    EXPECT_EQ(EventOrAlarm, EventOrAlarmInput);
}

INSTANTIATE_TEST_CASE_P(SendAlarmLogTestTest,
                        SendAlarmLogTest,
                        testing::ValuesIn(SendAlarmLogTestCase)
);

uint32_t alarmMsgHandler_timerCallback( uint32_t timerId, E_PriorityOfAlarm gs_highestPriorityEndSound, E_PriorityOfAlarm  &OutPutPriorityOfAlarm)
{
    switch (timerId)
    {
        OutPutPriorityOfAlarm = Reset;

        case ALARM_LED_TIMER_5S_ID:
            break;
        case ALARM_LED_TIMER_2000MS_ID:
            break;
        case ALARM_AUDIO_TIMER_250MS_ID:
            if(gs_highestPriorityEndSound == eHighPriority)
            {
                OutPutPriorityOfAlarm = gs_highestPriorityEndSound;
            }
            break;
        case ALARM_AUDIO_TIMER_400MS_ID:

            if(gs_highestPriorityEndSound == eMediumPriority)
            {
                OutPutPriorityOfAlarm = gs_highestPriorityEndSound;
            }
            if(gs_highestPriorityEndSound == eLowPriority)
            {
                OutPutPriorityOfAlarm = gs_highestPriorityEndSound;
            }
            break;
        default:
            break;
    }

    return timerId;
}

struct timerCallback
{
        uint32_t timerId;
        E_PriorityOfAlarm gs_highestPriorityEndSound;
        E_PriorityOfAlarm  OutPutPriorityOfAlarm;
};
struct timerCallback TimerCallBackTestCase[] =
{
        {ALARM_LED_TIMER_5S_ID, ePriorityDontCare, Reset},
        {ALARM_LED_TIMER_2000MS_ID, ePriorityDontCare, Reset},
        {ALARM_AUDIO_TIMER_250MS_ID, ePriorityDontCare, Reset},
        {ALARM_AUDIO_TIMER_250MS_ID, eHighPriority, eHighPriority},
        {ALARM_AUDIO_TIMER_400MS_ID, eMediumPriority, eMediumPriority},
        {ALARM_AUDIO_TIMER_400MS_ID, eLowPriority, eLowPriority},
        {ALARM_AUDIO_TIMER_400MS_ID, ePriorityDontCare, Reset}

};

class timerCallbackTest : public ::testing::TestWithParam<timerCallback>
{
};

TEST_P(timerCallbackTest, TimerCallBackTestCase)
{
    uint32_t timerId = GetParam().timerId;
    E_PriorityOfAlarm gs_highestPriorityEndSound = GetParam().gs_highestPriorityEndSound;
    E_PriorityOfAlarm  OutPutPriorityOfAlarm = GetParam().OutPutPriorityOfAlarm;
    E_PriorityOfAlarm inPut = Reset;

    EXPECT_EQ(timerId, alarmMsgHandler_timerCallback(timerId, gs_highestPriorityEndSound, inPut));
    EXPECT_EQ(OutPutPriorityOfAlarm, inPut);
}

INSTANTIATE_TEST_CASE_P(timerCallbackTestCaseList,
                        timerCallbackTest,
                        testing::ValuesIn(TimerCallBackTestCase)
);






















