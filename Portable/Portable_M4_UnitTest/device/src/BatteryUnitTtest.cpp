/********************************************************************/
/*                                                                  */
/* File Name     : BatteryUnitTtest.cpp                             */
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

#define MSB_CRC8    (0x07)      // 1000 0111 x8 + x2 + x1 + x0

typedef enum
{
    eCarChargeType,
    eACChargeType,
    eNoBatteryChargeType,
    eChargeTypeReset,

} E_ChargeType;

typedef enum
{
    eCarSource,
    eACSource,
    eBatterySource,
    eChargesourceDefault
} E_ChargeSource;

enum
{
    eGetBatteryRemainingPercentage,
    eGetBatteryStatus,
    eGetBatteryRemainingCapacity,
    eGetBatteryDefault,
};
uint8_t battery_GetCRC8( const void *buff, size_t size )
{
    uint8_t *p = (uint8_t *)buff;
    uint8_t crc8;

    for ( crc8 = 0x00 ; size != 0 ; size-- ){
        crc8 ^= *p++;

        for ( int i = 0 ; i < 8 ; i++ ){
            if ( crc8 & 0x80 ){
                crc8 <<= 1; crc8 ^= MSB_CRC8;
            }
            else{
                crc8 <<= 1;
            }
        }
    }
    return crc8;
}

class BatteryUnitTtest : public ::testing::Test
{
};

TEST_F(BatteryUnitTtest, battery_GetCRC8)
{
    uint8_t TestCase1[1] = {0xAA};
    EXPECT_EQ(0x5F, battery_GetCRC8(TestCase1, 1));

    uint8_t TestCase2[2] = {0x11, 0x22};
    EXPECT_EQ(0xAC, battery_GetCRC8(TestCase2, 2));

    uint8_t Size = 0xFF;
    uint8_t TestCase3[Size];
    uint8_t TestCase4[Size];
    for(int i = 0; i < Size; ++i)
    {
        TestCase3[i] = i;
        TestCase4[i] = 0xff;
    }
    EXPECT_EQ(0x21, battery_GetCRC8(TestCase3, Size));
    EXPECT_EQ(0xF3, battery_GetCRC8(TestCase4, Size));
}

E_ChargeType battery_ChangeInputCurrentLimit(E_ChargeType chargeType)
{
    E_ChargeType ChargeType = eChargeTypeReset;
    switch(chargeType)
    {
    case eCarChargeType:
        ChargeType = eCarChargeType;
        break;
    case eACChargeType:
        ChargeType = eACChargeType;
        break;
    case eNoBatteryChargeType:
        ChargeType = eNoBatteryChargeType;
        break;
    default:
        break;
    }
    return ChargeType;
}

TEST_F(BatteryUnitTtest, battery_ChangeInputCurrentLimit)
{
    EXPECT_EQ(eChargeTypeReset, battery_ChangeInputCurrentLimit(eChargeTypeReset));
    EXPECT_EQ(eCarChargeType, battery_ChangeInputCurrentLimit(eCarChargeType));
    EXPECT_EQ(eACChargeType, battery_ChangeInputCurrentLimit(eACChargeType));
    EXPECT_EQ(eNoBatteryChargeType, battery_ChangeInputCurrentLimit(eNoBatteryChargeType));
}

E_ChargeSource battery_UpdateChargingType(E_ChargeSource chargeSource)
{
    if(chargeSource == eCarSource)
    {
        return eCarSource;
    }
    else if(chargeSource == eACSource)
    {
        return eACSource;
    }
    else
    {
        return eChargesourceDefault;
        //Doing nothing here if this is battery case
    }
}

TEST_F(BatteryUnitTtest, battery_UpdateChargingType)
{
    EXPECT_EQ(eCarSource, battery_UpdateChargingType(eCarSource));
    EXPECT_EQ(eACSource, battery_UpdateChargingType(eACSource));
    EXPECT_EQ(eChargesourceDefault, battery_UpdateChargingType(eChargesourceDefault));
}

const char gs_expectedResponseHMAC[20] =
{
        1,2,3,4,
        5,6,7,8,
        9,10,11,12,
        13,14,15,20,
        16,17,18,19

};

bool battery_CompareHMAC(const char* input,const char* expectedHMAC,uint8_t len)
{
    for(int i=0;i<len;i++)
    {
        if(input[i]!=expectedHMAC[i])
            return false;
    }
    return true;
}

const char TestCaseA[20] =
{
        1,2,3,4,
        5,6,7,8,
        9,10,11,12,
        13,14,15,20,
        16,17,18,19
};

const char TestCaseB[20] =
{
        1,2,3,4,
        5,6,7,8,
        9,10,11,12,
        13,14,20,20,
        16,17,18,19
};

TEST_F(BatteryUnitTtest, battery_CompareHMAC)
{
    EXPECT_EQ(true, battery_CompareHMAC(TestCaseA, gs_expectedResponseHMAC, 20));
    EXPECT_EQ(false, battery_CompareHMAC(TestCaseB, gs_expectedResponseHMAC, 20));
}

E_ChargeSource battery_ReadPowerSource(bool AC_DETECT_PORT_NUM, bool CAR_DETECT_PORT_NUM)
{
    E_ChargeSource gs_currentPowerSource = eChargesourceDefault;

    if((AC_DETECT_PORT_NUM == true)
            && (CAR_DETECT_PORT_NUM == true))
    {
        gs_currentPowerSource = eACSource;
    }
    else
    {
        if((AC_DETECT_PORT_NUM == true)
                && (CAR_DETECT_PORT_NUM == false))
        {
            gs_currentPowerSource = eCarSource;
        }
        else
        {
            gs_currentPowerSource = eBatterySource;
        }
    }
    return gs_currentPowerSource;
}

TEST_F(BatteryUnitTtest, battery_ReadPowerSource)
{
    EXPECT_EQ(eACSource, battery_ReadPowerSource(true, true));
    EXPECT_EQ(eCarSource, battery_ReadPowerSource(true, false));
    EXPECT_EQ(eBatterySource, battery_ReadPowerSource(false, true));
    EXPECT_EQ(eBatterySource, battery_ReadPowerSource(false, false));
}

uint8_t battery_Update(bool gs_isInvalidBatteryInserted, bool gs_isBatteryConnect,
                     uint8_t s_currentStage, bool &s_checkOneTimes )
{
    bool checkOneTimes = false;
    s_checkOneTimes = checkOneTimes;
    if(gs_isInvalidBatteryInserted == true)
    {
        s_currentStage = eGetBatteryDefault;
        return s_currentStage;
    }
    if (gs_isBatteryConnect == false)
    {

        if (checkOneTimes == false)
        {
            checkOneTimes = true;
            s_checkOneTimes = true;
        }
    }
    else
    {
        checkOneTimes = false;
        s_checkOneTimes = false;
        switch(s_currentStage)
        {
        case eGetBatteryRemainingPercentage:
            s_currentStage = eGetBatteryRemainingPercentage;
            break;
        case eGetBatteryStatus:
            s_currentStage = eGetBatteryStatus;
            break;
        case eGetBatteryRemainingCapacity:
            s_currentStage = eGetBatteryRemainingCapacity;
            break;

        default:
            s_currentStage = eGetBatteryRemainingPercentage;
            break;
        }
    }

    return s_currentStage;
}

struct battery_Update_TestCase
{
        bool gs_isInvalidBatteryInserted;
        bool gs_isBatteryConnect;
        uint8_t s_currentStage_Input;
        uint8_t s_currentStage_Output;
        bool s_checkOneTimes;
};
struct battery_Update_TestCase battery_UpdateTestCase[] =
{
        {true, true, eGetBatteryDefault,eGetBatteryDefault , false},
        {true, false, eGetBatteryDefault,eGetBatteryDefault , false},

        {false, false, eGetBatteryDefault,eGetBatteryDefault , true},

        {false, true, eGetBatteryRemainingPercentage,eGetBatteryRemainingPercentage , false},
        {false, true, eGetBatteryRemainingCapacity,eGetBatteryRemainingCapacity , false},
        {false, true, eGetBatteryStatus,eGetBatteryStatus , false},
        {false, true, eGetBatteryRemainingPercentage,eGetBatteryRemainingPercentage , false}
};
class battery_UpdateTest: public ::testing::TestWithParam<battery_Update_TestCase>
{
};

TEST_P(battery_UpdateTest, battery_UpdateTestTestCaseList)
{
    bool gs_isInvalidBatteryInserted = GetParam().gs_isInvalidBatteryInserted;
    bool gs_isBatteryConnect = GetParam().gs_isBatteryConnect;
    uint8_t s_currentStage = GetParam().s_currentStage_Output;
    bool s_checkOneTimes = GetParam().s_checkOneTimes;

    bool checkOneTimesInPut;

    EXPECT_EQ(s_currentStage, battery_Update(gs_isInvalidBatteryInserted, gs_isBatteryConnect,
                                             GetParam().s_currentStage_Input, checkOneTimesInPut));
    EXPECT_EQ(s_checkOneTimes, checkOneTimesInPut);
}
INSTANTIATE_TEST_CASE_P(battery_UpdateTestTestCase,
                        battery_UpdateTest,
                        testing::ValuesIn(battery_UpdateTestCase)
);






