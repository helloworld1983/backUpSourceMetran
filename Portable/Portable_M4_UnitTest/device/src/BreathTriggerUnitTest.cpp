/********************************************************************/
/*                                                                  */
/* File Name     : BreathTriggerUnitTest.cpp                        */
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
    OFF,
    OPERATE_5V,
    OPERATE_9V,
    OPERATE_12V,
    DRAFT
} E_FanVoltage;

typedef enum {
    eTriggerSensitivityLevel1=1,
    eTriggerSensitivityLevel2,
    eTriggerSensitivityLevel3,
    eTriggerDefault,
}E_ID_TriggerSensitivity;

enum
{
    eTriggerSensitivityOffsetID,
    eFlowrateOffsetID,
    eO2OffsetID,
    eDischargeVolumeOffsetID,
    eTriggerSensitivityOffsetFanLowID,
    eTriggerSensitivityOffsetFanMidID,
    eTriggerSensitivityOffsetFanHighID,
    eOffsetCRCID,
    eLastOffsetID,
    eDefaulr_breathTrigger
};

uint32_t breathTrigger_UpdateTriggerSensitivityLevel(E_FanVoltage fanVoltage, E_ID_TriggerSensitivity level)
{
    uint32_t gs_currentTriggerValue = eDefaulr_breathTrigger;
    switch (fanVoltage)
    {
        case OFF:
            if(level == eTriggerSensitivityLevel1)
            {
                gs_currentTriggerValue = eTriggerSensitivityOffsetID;
            }
            else
            {
                if(level==eTriggerSensitivityLevel2)
                {
                    gs_currentTriggerValue = eTriggerSensitivityOffsetID;
                }
                else
                {
                    if(level==eTriggerSensitivityLevel3)
                    {
                        gs_currentTriggerValue = eTriggerSensitivityOffsetID;
                    }
                    else
                    {
                        gs_currentTriggerValue = eTriggerSensitivityOffsetID;
                    }
                }
            }
            break;
        case OPERATE_5V:
            if(level == eTriggerSensitivityLevel1)
            {
                gs_currentTriggerValue = eTriggerSensitivityOffsetID + eTriggerSensitivityOffsetFanLowID;
            }
            else
            {
                if(level==eTriggerSensitivityLevel2)
                {
                    gs_currentTriggerValue = eTriggerSensitivityOffsetID + eTriggerSensitivityOffsetFanLowID;
                }
                else
                {
                    if(level==eTriggerSensitivityLevel3)
                    {
                        gs_currentTriggerValue = eTriggerSensitivityOffsetID + eTriggerSensitivityOffsetFanLowID;
                    }
                    else
                    {
                        gs_currentTriggerValue = eTriggerSensitivityOffsetID + eTriggerSensitivityOffsetFanLowID;
                    }
                }
            }
            break;
        case OPERATE_9V:
            if(level == eTriggerSensitivityLevel1)
            {
                gs_currentTriggerValue = eTriggerSensitivityOffsetID +  eTriggerSensitivityOffsetFanMidID;
            }
            else
            {
                if(level==eTriggerSensitivityLevel2)
                {
                    gs_currentTriggerValue = eTriggerSensitivityOffsetID +  eTriggerSensitivityOffsetFanMidID;
                }
                else
                {
                    if(level==eTriggerSensitivityLevel3)
                    {
                        gs_currentTriggerValue = eTriggerSensitivityOffsetID +  eTriggerSensitivityOffsetFanMidID;
                    }
                    else
                    {
                        gs_currentTriggerValue = eTriggerSensitivityOffsetID +  eTriggerSensitivityOffsetFanMidID;
                    }
                }
            }
            break;
        case OPERATE_12V:
            if(level == eTriggerSensitivityLevel1)
            {
                gs_currentTriggerValue = eTriggerSensitivityOffsetID +  eTriggerSensitivityOffsetFanHighID;
            }
            else
            {
                if(level==eTriggerSensitivityLevel2)
                {
                    gs_currentTriggerValue = eTriggerSensitivityOffsetID +  eTriggerSensitivityOffsetFanHighID;
                }
                else
                {
                    if(level==eTriggerSensitivityLevel3)
                    {
                        gs_currentTriggerValue = eTriggerSensitivityOffsetID +  eTriggerSensitivityOffsetFanHighID;
                    }
                    else
                    {
                        gs_currentTriggerValue = eTriggerSensitivityOffsetID +  eTriggerSensitivityOffsetFanHighID;
                    }
                }
            }
            break;
        default:
            break;
    }
    return gs_currentTriggerValue;
}

struct UpdateTriggerSensitivityLevel
{
        E_FanVoltage fanVoltage;
        E_ID_TriggerSensitivity level;
        uint32_t gs_currentTriggerValue;
};
struct UpdateTriggerSensitivityLevel UpdateTriggerSensitivityLevelTestCase [] =
{
        {OFF, eTriggerSensitivityLevel1, eTriggerSensitivityOffsetID},
        {OFF, eTriggerSensitivityLevel2, eTriggerSensitivityOffsetID},
        {OFF, eTriggerSensitivityLevel3, eTriggerSensitivityOffsetID},
        {OFF, eTriggerDefault, eTriggerSensitivityOffsetID},

        {OPERATE_5V, eTriggerSensitivityLevel1, eTriggerSensitivityOffsetID + eTriggerSensitivityOffsetFanLowID},
        {OPERATE_5V, eTriggerSensitivityLevel2, eTriggerSensitivityOffsetID + eTriggerSensitivityOffsetFanLowID},
        {OPERATE_5V, eTriggerSensitivityLevel3, eTriggerSensitivityOffsetID + eTriggerSensitivityOffsetFanLowID},
        {OPERATE_5V, eTriggerDefault, eTriggerSensitivityOffsetID + eTriggerSensitivityOffsetFanLowID},

        {OPERATE_9V, eTriggerSensitivityLevel1, eTriggerSensitivityOffsetID + eTriggerSensitivityOffsetFanMidID},
        {OPERATE_9V, eTriggerSensitivityLevel2, eTriggerSensitivityOffsetID + eTriggerSensitivityOffsetFanMidID},
        {OPERATE_9V, eTriggerSensitivityLevel3, eTriggerSensitivityOffsetID + eTriggerSensitivityOffsetFanMidID},
        {OPERATE_9V, eTriggerDefault, eTriggerSensitivityOffsetID + eTriggerSensitivityOffsetFanMidID},

        {OPERATE_12V, eTriggerSensitivityLevel1, eTriggerSensitivityOffsetID + eTriggerSensitivityOffsetFanHighID},
        {OPERATE_12V, eTriggerSensitivityLevel2, eTriggerSensitivityOffsetID + eTriggerSensitivityOffsetFanHighID},
        {OPERATE_12V, eTriggerSensitivityLevel3, eTriggerSensitivityOffsetID + eTriggerSensitivityOffsetFanHighID},
        {OPERATE_12V, eTriggerDefault, eTriggerSensitivityOffsetID + eTriggerSensitivityOffsetFanHighID},

        {DRAFT, eTriggerSensitivityLevel1, eDefaulr_breathTrigger},
        {DRAFT, eTriggerSensitivityLevel2, eDefaulr_breathTrigger},
        {DRAFT, eTriggerSensitivityLevel3, eDefaulr_breathTrigger},
        {DRAFT, eTriggerDefault, eDefaulr_breathTrigger}

};
class UpdateTriggerSensitivityLevelTest: public ::testing::TestWithParam<UpdateTriggerSensitivityLevel>
{
};
TEST_P(UpdateTriggerSensitivityLevelTest, UpdateTriggerSensitivityLevelTestCaseList)
{
    E_FanVoltage fanVoltage = GetParam().fanVoltage;
    E_ID_TriggerSensitivity level = GetParam().level;
    uint32_t gs_currentTriggerValue = GetParam().gs_currentTriggerValue;

    EXPECT_EQ(gs_currentTriggerValue, breathTrigger_UpdateTriggerSensitivityLevel(fanVoltage, level));
}

INSTANTIATE_TEST_CASE_P(UpdateTriggerSensitivityLevelTestCase,
                        UpdateTriggerSensitivityLevelTest,
                        testing::ValuesIn(UpdateTriggerSensitivityLevelTestCase)
);

bool breathTrigger_SetTestSynchronizeMode(uint8_t gs_testSynchronizeModeBPM)
{
    bool gs_isResetDetectBreathMode;
    if(gs_testSynchronizeModeBPM == 0)
    {
        gs_isResetDetectBreathMode = true;
    }
    else
    {
        gs_isResetDetectBreathMode = false;
    }

    return gs_isResetDetectBreathMode;
}
class BreathTriggerUnitTest : public ::testing::Test
{
};

TEST_F(BreathTriggerUnitTest, breathTrigger_SetTestSynchronizeMode)
{
    EXPECT_EQ(true, breathTrigger_SetTestSynchronizeMode(0));
    EXPECT_EQ(false, breathTrigger_SetTestSynchronizeMode(1));
    EXPECT_EQ(false, breathTrigger_SetTestSynchronizeMode(21));
    EXPECT_EQ(false, breathTrigger_SetTestSynchronizeMode(0xFF));
}

void breathTrigger_ResetDetectBreathMode(bool &gs_isResetDetectBreathMode)
{
    gs_isResetDetectBreathMode = true;
}

TEST_F(BreathTriggerUnitTest, breathTrigger_ResetDetectBreathMode)
{
    bool result = false;
    EXPECT_EQ(false, result);
    breathTrigger_ResetDetectBreathMode(result);
    EXPECT_EQ(true, result);
}

void breathTrigger_ResetTestSynchronizeMode(bool &gs_isResetTestSynchronizeMode)
{
    gs_isResetTestSynchronizeMode = true;
}

TEST_F(BreathTriggerUnitTest, breathTrigger_ResetTestSynchronizeMode)
{
    bool result = false;
    EXPECT_EQ(false, result);
    breathTrigger_ResetTestSynchronizeMode(result);
    EXPECT_EQ(true, result);
}
