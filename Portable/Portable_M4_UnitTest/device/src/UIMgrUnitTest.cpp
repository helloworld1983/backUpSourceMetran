/********************************************************************/
/*                                                                  */
/* File Name     : UINgrUnitTest.cpp                                */
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

#define VERY_LOW_BATTERY_LOW_PERCENTAGE 0
#define VERY_LOW_BATTERY_HIGH_PERCENTAGE 20
#define LOW_BATTERY_PERCENTAGE 50
#define MEDIUM_BATTERY_PERCENTAGE 75
#define HIGH_BATTERY_PERCENTAGE 90
#define FULL_BATTERY_PERCENTAGE 100

typedef enum
{
    eCarSource,
    eACSource,
    eBatterySource
} E_ChargeSource;

class UIMgrUnitTest : public ::testing::Test
{
};

uint8_t UIMgr_UpdateBattery(bool gs_isBatteryCharging, uint16_t gs_currentBatteryPercent)
{
    if(gs_isBatteryCharging == false)
    {
        if(gs_currentBatteryPercent == VERY_LOW_BATTERY_LOW_PERCENTAGE)
        {
            return 1;
        }
        else
            if((gs_currentBatteryPercent <= VERY_LOW_BATTERY_HIGH_PERCENTAGE)&&(gs_currentBatteryPercent>VERY_LOW_BATTERY_LOW_PERCENTAGE))
            {
                return 2;
            }
            else
                if((gs_currentBatteryPercent > VERY_LOW_BATTERY_HIGH_PERCENTAGE) && (gs_currentBatteryPercent <= LOW_BATTERY_PERCENTAGE))
                {
                    return 3;
                }
                else
                    if((gs_currentBatteryPercent > LOW_BATTERY_PERCENTAGE) && (gs_currentBatteryPercent <= MEDIUM_BATTERY_PERCENTAGE))
                    {
                        return 4;
                    }
                    else
                        if((gs_currentBatteryPercent > MEDIUM_BATTERY_PERCENTAGE) && (gs_currentBatteryPercent <= FULL_BATTERY_PERCENTAGE))
                        {
                            return 5;
                        }

    }
    else
    {
        if(gs_currentBatteryPercent == VERY_LOW_BATTERY_LOW_PERCENTAGE)
        {
            return 6;
        }
        else
            if((gs_currentBatteryPercent <= VERY_LOW_BATTERY_HIGH_PERCENTAGE)&&(gs_currentBatteryPercent > VERY_LOW_BATTERY_LOW_PERCENTAGE))
            {
                return 7;
            }
            else
                if((gs_currentBatteryPercent > VERY_LOW_BATTERY_HIGH_PERCENTAGE) && (gs_currentBatteryPercent <= LOW_BATTERY_PERCENTAGE))
                {
                    return 8;
                }
                else
                    if((gs_currentBatteryPercent > LOW_BATTERY_PERCENTAGE) && (gs_currentBatteryPercent <= HIGH_BATTERY_PERCENTAGE))
                    {
                        return 9;
                    }
                    else
                        if((gs_currentBatteryPercent > HIGH_BATTERY_PERCENTAGE) && (gs_currentBatteryPercent <= FULL_BATTERY_PERCENTAGE))
                        {
                            return 10;
                        }

    }
    return 11;
}

struct UpdateBattery
{
        bool gs_isBatteryCharging;
        uint16_t gs_currentBatteryPercent;

        uint8_t result;
};
struct UpdateBattery UpdateBatteryTestCase[] =
{
        {false, VERY_LOW_BATTERY_LOW_PERCENTAGE, 1},

        {false, VERY_LOW_BATTERY_LOW_PERCENTAGE + 1, 2},
        {false, VERY_LOW_BATTERY_HIGH_PERCENTAGE, 2},

        {false, VERY_LOW_BATTERY_HIGH_PERCENTAGE + 1, 3},
        {false, LOW_BATTERY_PERCENTAGE - 1, 3},
        {false, LOW_BATTERY_PERCENTAGE, 3},

        {false, LOW_BATTERY_PERCENTAGE + 1, 4},
        {false, LOW_BATTERY_PERCENTAGE + 5, 4},
        {false, MEDIUM_BATTERY_PERCENTAGE, 4},

        {false, MEDIUM_BATTERY_PERCENTAGE + 1, 5},
        {false, MEDIUM_BATTERY_PERCENTAGE + 5, 5},
        {false, FULL_BATTERY_PERCENTAGE, 5},

        {true, VERY_LOW_BATTERY_LOW_PERCENTAGE, 6},

        {true, VERY_LOW_BATTERY_LOW_PERCENTAGE + 1, 7},
        {true, VERY_LOW_BATTERY_HIGH_PERCENTAGE, 7},

        {true, VERY_LOW_BATTERY_HIGH_PERCENTAGE + 1, 8},
        {true, LOW_BATTERY_PERCENTAGE - 1, 8},
        {true, LOW_BATTERY_PERCENTAGE, 8},

        {true, LOW_BATTERY_PERCENTAGE + 1, 9},
        {true, LOW_BATTERY_PERCENTAGE + 5, 9},
        {true, MEDIUM_BATTERY_PERCENTAGE, 9},

        {true, HIGH_BATTERY_PERCENTAGE + 1, 10},
        {true, HIGH_BATTERY_PERCENTAGE + 5, 10},
        {true, FULL_BATTERY_PERCENTAGE, 10}

};
class UpdateBatteryTest: public::testing::TestWithParam<UpdateBattery>
{
};
TEST_P(UpdateBatteryTest, UpdateBatteryTestCase)
{
    EXPECT_EQ(GetParam().result, UIMgr_UpdateBattery(GetParam().gs_isBatteryCharging, GetParam().gs_currentBatteryPercent));
}
INSTANTIATE_TEST_CASE_P(UpdateBatteryTest,
                        UpdateBatteryTest,
                        testing::ValuesIn(UpdateBatteryTestCase)
);

bool UIMgr_UpdateACDetect(E_ChargeSource GetChargeSource)
{
    bool gs_isACExist;
    if(GetChargeSource != eBatterySource)
    {
        gs_isACExist = true;
    }
    else
    {
        gs_isACExist = false;
    }
    return gs_isACExist;
}


TEST_F(UIMgrUnitTest, UIMgrUnitTestCase)
{
    EXPECT_EQ(false, UIMgr_UpdateACDetect(eBatterySource));
    EXPECT_EQ(true, UIMgr_UpdateACDetect(eCarSource));
    EXPECT_EQ(true, UIMgr_UpdateACDetect(eACSource));
}


typedef enum
{
    VAL1,
    VAL2,
    VAL3,
    VAL
} E_ValveNum;

typedef enum {
    VAL_ON,
    VAL_OFF
} E_ValStatus;

E_ValveNum valve_getStatus(E_ValveNum valNum)
{
    E_ValveNum value = VAL;
    switch (valNum) {
    case VAL1:
        value = VAL1;
        break;
    case VAL2:
        value = VAL2;
        break;
    case VAL3:
        value = VAL3;
        break;
    default:

        break;
    }
    return value;
}
class ValveCtrlUnitTest : public::testing::Test
{
};

TEST_F(ValveCtrlUnitTest, ValveCtrlUnitTestCase)
{
    EXPECT_EQ(VAL1, valve_getStatus(VAL1));
    EXPECT_EQ(VAL2, valve_getStatus(VAL2));
    EXPECT_EQ(VAL3, valve_getStatus(VAL3));
    EXPECT_EQ(VAL, valve_getStatus(VAL));
}

