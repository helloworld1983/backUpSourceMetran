/********************************************************************/
/*                                                                  */
/* File Name     : EEPRom.cpp                                       */
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
    eOperationMode,
    eUpdateMode,
    ePowerLostMode,
    eResetWatchdogTimer,
    eVeryLowBatteryMode
}E_OperationMode;

E_OperationMode EEPROM_SetMode(E_OperationMode mode)
{
    switch(mode)
    {
    case eOperationMode:
        mode =  eOperationMode;
        break;
    case eUpdateMode:
        mode = eUpdateMode;
        break;
    case ePowerLostMode:
        mode = ePowerLostMode;
        break;
    default:
        break;
    }
    return mode;
};

class EEPRomUnitTest: public ::testing::Test
{
};
TEST_F(EEPRomUnitTest, EEPROM_SetModeeOperationMode)
{
    EXPECT_EQ(eOperationMode, EEPROM_SetMode(eOperationMode));
}
TEST_F(EEPRomUnitTest, EEPROM_SetModeeeUpdateMode)
{
    EXPECT_EQ(eUpdateMode, EEPROM_SetMode(eUpdateMode));
}
TEST_F(EEPRomUnitTest, EEPROM_SetModeeePowerLostMode)
{
    EXPECT_EQ(ePowerLostMode, EEPROM_SetMode(ePowerLostMode));
}
