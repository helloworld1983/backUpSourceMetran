/********************************************************************/
/*                                                                  */
/* File Name     : ModeHandleUnitTest.cpp                           */
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

class ModeHandleUnitTest : public ::testing::Test
{
};

typedef enum
{
    eSynchronizedMode=0,
    eContinuosMode
} E_ModeSetting;

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

typedef struct {
    E_ID_Audio ID;
    uint8_t volume;
}AUDIO_T;

enum
{
    eFlowRateZeroOneZero,
    eFlowRateZeroTwoFive,
    eFlowRateZeroFiveZero,
    eFlowRateZeroSevenFive,
    eFlowRateOneZeroZero,
    eFlowRateOneTwoFive,
    eFlowRateOneFiveZero,
    eFlowRateOneSevenFive,
    eFlowRateTwoZeroZero,
    eFlowRateTwoTowFive,
    eFlowRateTwoFiveZero,
    eFlowRateTwoSevenFive,
    eFlowRateThreeZeroZero,
    eFlowRateZeroZeroZero,
    eFlowRateLast
};

E_ID_Audio mode_ChangeMode(E_ModeSetting Mode, uint8_t currentFlowRate)
{
    E_ID_Audio eventAudio;
    if(Mode == eSynchronizedMode)
    {
        eventAudio = (E_ID_Audio)(eZeroOneZeroContAudio);

    }
    else
    {
        eventAudio = (E_ID_Audio)(eZeroTwoFiveSyncAudio - 1);
        if(currentFlowRate == eFlowRateTwoFiveZero)
        {
            eventAudio = eTwoFiveZeroSyncAudio;
        }
        if(currentFlowRate == eFlowRateThreeZeroZero)
        {
            eventAudio = eThreeZeroZeroSyncAudio;
        }

    }
    return eventAudio;
}

struct ChangeMode
{
        E_ModeSetting Mode;
        uint8_t currentFlowRate;
        E_ID_Audio eventAudio;
};
struct ChangeMode ChangeModeTestCase[] =
{
        {eSynchronizedMode, 0, (E_ID_Audio)eZeroOneZeroContAudio},
        {eContinuosMode, 0, (E_ID_Audio)(eZeroTwoFiveSyncAudio - 1)},
        {eContinuosMode, eFlowRateTwoFiveZero, eTwoFiveZeroSyncAudio},
        {eContinuosMode, eFlowRateThreeZeroZero, eThreeZeroZeroSyncAudio}
};
class TestChangeMode : public ::testing::TestWithParam<ChangeMode>
{
};
TEST_P(TestChangeMode, ChangeModeTestCase)
{
    E_ModeSetting Mode = GetParam().Mode;
    uint8_t currentFlowRate = GetParam().currentFlowRate;
    E_ID_Audio eventAudio = GetParam().eventAudio;

    EXPECT_EQ(eventAudio, mode_ChangeMode(Mode, currentFlowRate));
}
INSTANTIATE_TEST_CASE_P(TestChangeMode,
                        TestChangeMode,
                        testing::ValuesIn(ChangeModeTestCase)
);
