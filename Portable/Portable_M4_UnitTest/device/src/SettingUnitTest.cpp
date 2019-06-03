/********************************************************************/
/*                                                                  */
/* File Name     : SettingUnitTest.cpp                              */
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

typedef enum
{
    eSynchronizedMode=0,
    eContinuosMode
} E_ModeSetting;

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

typedef enum {
    eAudioVolumeLevel1=1,
    eAudioVolumeLevel2,
    eAudioVolumeLevel3,
    eAudioVolumeLevel4,
    eAudioVolumeLevel5,
}E_ID_AudioVolume;

typedef enum {
    eTriggerSensitivityLevel1=1,
    eTriggerSensitivityLevel2,
    eTriggerSensitivityLevel3,
}E_ID_TriggerSensitivity;

uint8_t setting_GetCurrentSetting(uint8_t mode)
{
    if(mode == eContinuosMode)
    {
        return eContinuosMode;
    }
    else
    {
        return eSynchronizedMode;
    }
}

#define MAX_TRIGGER_SENSITIVITY_OFFSET 0.11
#define MIN_TRIGGER_SENSITIVITY_OFFSET -0.11
#define MAX_FLOWRATE_OFFSET 0.5
#define MIN_FLOWRATE_OFFSET -0.5
#define MAX_O2_OFFSET 5.0
#define MIN_O2_OFFSET -5.0
#define MAX_DISCHARGE_OFFSET 50.0
#define MIN_DISCHARGE_OFFSET -50.0
#define MAX_O2_ALARM_DETECTION_OFFSET 57
#define MIN_O2_ALARM_DETECTION_OFFSET 0

class SettingUnitTest : public ::testing::Test
{
};
TEST_F(SettingUnitTest, SettingUnitTestCaseWitheContinuosMode)
{
    EXPECT_EQ(eContinuosMode, setting_GetCurrentSetting(eContinuosMode));
}
TEST_F(SettingUnitTest, SettingUnitTestCaseWitheeSynchronizedMode)
{
    EXPECT_EQ(eSynchronizedMode, setting_GetCurrentSetting(eSynchronizedMode));
}

bool setting_CheckRange(uint8_t mode, uint16_t flowRate, bool CheckVersion, bool CheckHour, uint16_t AudioVolume,
                        uint16_t TriggerSensitivity, float OffSet)
{
    bool error = false;
    if((mode != eSynchronizedMode) && (mode != eContinuosMode))
    {
        error = true;
    }
    else if(flowRate > eFlowRateOneZeroZero)
    {
        error = true;
    }
    else if((flowRate >= eFlowRateLast)||(flowRate < eFlowRateOneTwoFive))
    {
        error = true;
    }
    else if(CheckVersion == true)
    {
        error = true;
    }
    else if(CheckHour == true)
    {
        error = true;
    }
    else if(CheckHour == true)
    {
        error = true;
    }
    else if(CheckHour == true)
    {
        error = true;
    }
    else if(CheckHour == true)
    {
        error = true;
    }
    else if((AudioVolume > eAudioVolumeLevel5)||(AudioVolume < eAudioVolumeLevel1))
    {
        error = true;
    }
    else if((TriggerSensitivity > eTriggerSensitivityLevel3)||(TriggerSensitivity < eTriggerSensitivityLevel1))
    {
        error = true;
    }
    else if((OffSet > MAX_TRIGGER_SENSITIVITY_OFFSET)||(OffSet < MIN_TRIGGER_SENSITIVITY_OFFSET))
    {
        error = true;
    }
    else if((OffSet > MAX_FLOWRATE_OFFSET)||(OffSet < MIN_FLOWRATE_OFFSET))
    {
        error = true;
    }
    else if((OffSet > MAX_O2_OFFSET)||(OffSet < MIN_O2_OFFSET))
    {
        error = true;
    }
    else if((OffSet > MAX_DISCHARGE_OFFSET)||(OffSet < MIN_DISCHARGE_OFFSET))
    {
        error = true;
    }

    return error;
}

struct CheckRange
{
        uint8_t mode;
        uint16_t flowRate;
        bool CheckVersion;
        bool CheckHour;
        uint16_t AudioVolume;
        uint16_t TriggerSensitivity;
        float OffSet;
        bool Error;
};
struct CheckRange CheckRangeTestCase[] =
{
        {2, 0, false, false, 0, 0, 0, true},
        {2, 5, false, false, 0, 0, 0, true},
        {2, 5, false, false, 0, 0, 0, true},
        {2, 10, false, false, 0, 0, 0, true},
        {2, 10, true, true, 0, 0, 0, true},
        {2, 10, true, false, 0, 0, 0, true},
        {2, 10, false, false, 0, 0, 0, true},
        {2, 10, false, true, 0, 0, 0, true},
        {2, 10, false, true, 0, 0, 0, true},
        {2, 10, false, true, 6, 0, 0, true},
        {2, 10, false, true, 0, 0, 0, true},
        {2, 10, false, true, 0, 4, 0, true},
        {2, 10, false, true, 0, 4, MAX_TRIGGER_SENSITIVITY_OFFSET, true},
        {2, 10, false, true, 0, 4, MIN_TRIGGER_SENSITIVITY_OFFSET, true},
        {2, 10, false, true, 0, 4, MAX_FLOWRATE_OFFSET, true},
        {2, 10, false, true, 0, 4, MIN_FLOWRATE_OFFSET, true},
        {2, 10, false, true, 0, 4, MAX_O2_OFFSET, true},
        {2, 10, false, true, 0, 4, MIN_O2_OFFSET, true},
        {2, 10, false, true, 0, 4, MAX_DISCHARGE_OFFSET, true},
        {2, 10, false, true, 0, 4, MIN_DISCHARGE_OFFSET, true}
};
class CheckRangeTest : public ::testing::TestWithParam<CheckRange>
{
};
TEST_P(CheckRangeTest, CheckRangeTestCase)
{
    EXPECT_EQ(GetParam().Error, setting_CheckRange(GetParam().mode, GetParam().flowRate, GetParam().CheckVersion,
                                                   GetParam().CheckHour, GetParam().AudioVolume, GetParam().TriggerSensitivity,
                                                   GetParam().OffSet));
}
INSTANTIATE_TEST_CASE_P(CheckRangeTest,
                        CheckRangeTest,
                        testing::ValuesIn(CheckRangeTestCase)
);




