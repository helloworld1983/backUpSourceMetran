/********************************************************************/
/*                                                                  */
/* File Name     : FanUnitTest.cpp                                  */
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

#define HIGH_TEMPERATURE 60
#define LOW_TEMPERATURE 45

typedef enum
{
    OFF,
    OPERATE_5V,
    OPERATE_9V,
    OPERATE_12V
} E_FanVoltage;

E_FanVoltage fan_SetVoltage(E_FanVoltage voltage)
{
    E_FanVoltage gs_fanVoltage = OFF;
    switch(voltage)
    {
    case OFF:
        gs_fanVoltage = OFF;
        break;
    case OPERATE_5V:
        gs_fanVoltage = OPERATE_5V;
        break;
    case OPERATE_9V:
        gs_fanVoltage = OPERATE_9V;
        break;
    case OPERATE_12V:
        gs_fanVoltage = OPERATE_12V;
        break;
    default:
        break;
    }
    return gs_fanVoltage;
}
class FanUnitTest : public ::testing::Test
{
};
TEST_F(FanUnitTest, fan_SetVoltageOFF)
{
    EXPECT_EQ(OFF, fan_SetVoltage(OFF));
}
TEST_F(FanUnitTest, fan_SetVoltageOPERATE_5V)
{
    EXPECT_EQ(OPERATE_5V, fan_SetVoltage(OPERATE_5V));
}
TEST_F(FanUnitTest, fan_SetVoltageOPERATE_12V)
{
    EXPECT_EQ(OPERATE_12V, fan_SetVoltage(OPERATE_12V));
}
TEST_F(FanUnitTest, fan_SetVoltageOPERATE_9V)
{
    EXPECT_EQ(OPERATE_9V, fan_SetVoltage(OPERATE_9V));
}

uint8_t fan_UpdateFanVoltage(bool gs_isFanShutDown, bool psa_isThreeSecondFinish,
                          bool psa_isWarmUpFinish, uint16_t avrTemperature)
{
    if(gs_isFanShutDown == false)
    {
        if(psa_isThreeSecondFinish == false)
        {
            return 1;
        }
        else
        {
            if(psa_isWarmUpFinish == false)
            {
                if(avrTemperature<=HIGH_TEMPERATURE)
                {
                    return 2;
                }
                else
                {
                    return 3;
                }
            }
            else
            {
                if(avrTemperature<=LOW_TEMPERATURE)
                {
                    return 4;
                }
                else
                {
                    if((avrTemperature>LOW_TEMPERATURE)&&(avrTemperature<=HIGH_TEMPERATURE))
                    {
                        return 5;
                    }
                    else
                    {
                        return 6;
                    }
                }
            }
        }
    }
    return 7;
}
struct UpdateFanVoltage
{
        bool gs_isFanShutDown;
        bool psa_isThreeSecondFinish;
        bool psa_isWarmUpFinish;
        uint16_t avrTemperature;
        uint8_t result;
};
struct UpdateFanVoltage UpdateFanVoltageTestCase[] =
{
        {true, true, true, 1, 7},
        {true, true, false, HIGH_TEMPERATURE, 7},
        {true, false, true, LOW_TEMPERATURE, 7},
        {true, false, false, 7, 7},

        {false, false, false, HIGH_TEMPERATURE, 1},
        {false, false, true, LOW_TEMPERATURE, 1},

        {false, true, false, HIGH_TEMPERATURE, 2},
        {false, true, false, HIGH_TEMPERATURE - 1, 2},

        {false, true, false, HIGH_TEMPERATURE + 1, 3},

        {false, true, true, LOW_TEMPERATURE - 1, 4},
        {false, true, true, LOW_TEMPERATURE, 4},

        {false, true, true, 50, 5},
        {false, true, true, HIGH_TEMPERATURE + 1, 6}
};

class UpdateFanVoltageTest: public ::testing::TestWithParam<UpdateFanVoltage>
{
};

TEST_P(UpdateFanVoltageTest, UpdateFanVoltageTestCase)
{
    bool gs_isFanShutDown = GetParam().gs_isFanShutDown;
    bool psa_isThreeSecondFinish = GetParam().psa_isThreeSecondFinish;
    bool psa_isWarmUpFinish = GetParam().psa_isWarmUpFinish;
    uint16_t avrTemperature = GetParam().avrTemperature;
    uint8_t result = GetParam().result;
    EXPECT_EQ(result, fan_UpdateFanVoltage(gs_isFanShutDown, psa_isThreeSecondFinish, psa_isWarmUpFinish, avrTemperature));
}
INSTANTIATE_TEST_CASE_P(UpdateFanVoltageTestCase,
                        UpdateFanVoltageTest,
                        testing::ValuesIn(UpdateFanVoltageTestCase)
);

