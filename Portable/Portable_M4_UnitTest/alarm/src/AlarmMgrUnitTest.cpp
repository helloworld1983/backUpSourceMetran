/*
 * AlarmMgrUnitTest.cpp
 *
 *  Created on: Feb 2, 2018
 *      Author: WINDOWS
 */
#include "stdafx.h"
#include "AlarmMgr.c"



class AlarmMgrUnitTest : public ::testing::Test
{
public:
	AlarmMgrUnitTest()
    {
		alarmMgr_InitAlarm();
    }
};



TEST(AlarmMgr, alarmMgr_CheckFCAbnormalOxygenConcentration)
{
	uint16_t O2Concentration = 3000; //30%

    for (int i = 1; i < 1000/50*30; i++)
    	EXPECT_EQ(0, alarmMgr_CheckFCAbnormalOxygenConcentration(O2Concentration));

    O2Concentration = 8000; //80%
    EXPECT_EQ(0, alarmMgr_CheckFCAbnormalOxygenConcentration(O2Concentration));

    O2Concentration = 3000; //30%
    for (int i = 1; i < 1000/50*60; i++)
    	EXPECT_EQ(0, alarmMgr_CheckFCAbnormalOxygenConcentration(O2Concentration));

    EXPECT_EQ(1, alarmMgr_CheckFCAbnormalOxygenConcentration(O2Concentration));


    for (int i = 1; i < 1000/50*30; i++)
    	EXPECT_EQ(0, alarmMgr_CheckFCAbnormalOxygenConcentration(O2Concentration));

    for (int i = 1; i < 1000/50*60; i++)
    	EXPECT_EQ(0, alarmMgr_CheckFCAbnormalOxygenConcentration(O2Concentration));

    EXPECT_EQ(1, alarmMgr_CheckFCAbnormalOxygenConcentration(O2Concentration));
}




TEST(AlarmMgr, alarmMgr_CheckBreathErrorAlarmStatus)
{

    for (int i = 0; i < 1000/50*30; i++)
    	EXPECT_EQ(eInactive,alarmMgr_CheckBreathErrorAlarmStatus(25,true,10 ));
    EXPECT_EQ(eActive,alarmMgr_CheckBreathErrorAlarmStatus(25,true,10 ));

    alarmMgrUnitTest_ReSetUpForBreathErrorTest();
    for (int i = 0; i < 1000/50*30; i++)
    	EXPECT_EQ(eInactive,alarmMgr_CheckBreathErrorAlarmStatus(10,true,41 ));
    EXPECT_EQ(eActive,alarmMgr_CheckBreathErrorAlarmStatus(10,true,41 ));

    alarmMgrUnitTest_ReSetUpForBreathErrorTest();
    for (int i = 0; i < 1000/50*30; i++)
    	EXPECT_EQ(eInactive,alarmMgr_CheckBreathErrorAlarmStatus(25,true,41 ));
    EXPECT_EQ(eInactive,alarmMgr_CheckBreathErrorAlarmStatus(25,true,41 ));

    for (int i = 0; i < 1000/50*30; i++)
    	EXPECT_EQ(eInactive,alarmMgr_CheckBreathErrorAlarmStatus(25,true,101 ));
    EXPECT_EQ(eActive,alarmMgr_CheckBreathErrorAlarmStatus(25,true,101 ));

    alarmMgrUnitTest_ReSetUpForBreathErrorTest();
    for (int i = 0; i < 1000/50*30; i++)
    	EXPECT_EQ(eInactive,alarmMgr_CheckBreathErrorAlarmStatus(50,true,150 ));
    EXPECT_EQ(eInactive,alarmMgr_CheckBreathErrorAlarmStatus(50,true,150 ));

    for (int i = 0; i < 1000/50*30; i++)
    	EXPECT_EQ(eInactive,alarmMgr_CheckBreathErrorAlarmStatus(50,true,201 ));
    EXPECT_EQ(eActive,alarmMgr_CheckBreathErrorAlarmStatus(50,true,201 ));

    alarmMgrUnitTest_ReSetUpForBreathErrorTest();
    for (int i = 0; i < 1000/50*30; i++)
    	EXPECT_EQ(eInactive,alarmMgr_CheckBreathErrorAlarmStatus(75,true,150 ));
    EXPECT_EQ(eInactive,alarmMgr_CheckBreathErrorAlarmStatus(75,true,150 ));

    for (int i = 0; i < 1000/50*30; i++)
    	EXPECT_EQ(eInactive,alarmMgr_CheckBreathErrorAlarmStatus(75,true,201 ));
    EXPECT_EQ(eActive,alarmMgr_CheckBreathErrorAlarmStatus(75,true,201 ));
}




TEST(AlarmMgr, alarmMgr_CheckAbnormalOxygenConcentrationAlarmStatus)
{
	alarmMgrUnitTest_ReSetUpForBreathErrorTest();
    for (int i = 0; i < 1000/50*60*20; i++)
    	EXPECT_EQ(eInactive,alarmMgr_CheckAbnormalOxygenConcentrationAlarmStatus(FLOWRATE_ZERO_ONE_LPM,8000 ));
    EXPECT_EQ(eActive,alarmMgr_CheckAbnormalOxygenConcentrationAlarmStatus(FLOWRATE_ZERO_ONE_LPM,8000 ));
    EXPECT_EQ(eInactive,alarmMgr_CheckAbnormalOxygenConcentrationAlarmStatus(FLOWRATE_ZERO_ONE_LPM,8200 ));

    for (int i = 0; i < 1000/50*60*20; i++)
    	EXPECT_EQ(eInactive,alarmMgr_CheckAbnormalOxygenConcentrationAlarmStatus(FlOWRATE_ZERO_TWO_FIVE_LPM,8000 ));
    EXPECT_EQ(eActive,alarmMgr_CheckAbnormalOxygenConcentrationAlarmStatus(FlOWRATE_ZERO_TWO_FIVE_LPM,8000 ));
    EXPECT_EQ(eInactive,alarmMgr_CheckAbnormalOxygenConcentrationAlarmStatus(FLOWRATE_ZERO_ONE_LPM,8200 ));

    for (int i = 0; i < 1000/50*60*3; i++)
    	EXPECT_EQ(eInactive,alarmMgr_CheckAbnormalOxygenConcentrationAlarmStatus(50,8000 ));
    EXPECT_EQ(eActive,alarmMgr_CheckAbnormalOxygenConcentrationAlarmStatus(50,8000 ));
    EXPECT_EQ(eInactive,alarmMgr_CheckAbnormalOxygenConcentrationAlarmStatus(50,8200 ));

    for (int i = 0; i < 1000/50*60*3; i++)
    	EXPECT_EQ(eInactive,alarmMgr_CheckAbnormalOxygenConcentrationAlarmStatus(50,8000 ));
    EXPECT_EQ(eActive,alarmMgr_CheckAbnormalOxygenConcentrationAlarmStatus(50,8000 ));
    EXPECT_EQ(eInactive,alarmMgr_CheckAbnormalOxygenConcentrationAlarmStatus(50,8200 ));
}





TEST(AlarmMgr, alarmMgr_CheckLowOxygenConcentrationAlarmStatus)
{
    for (int i = 0; i < 1000/50*60*20; i++)
    	EXPECT_EQ(eInactive,alarmMgr_CheckLowOxygenConcentrationAlarmStatus(FLOWRATE_ZERO_ONE_LPM,8300 ));
    EXPECT_EQ(eActive,alarmMgr_CheckLowOxygenConcentrationAlarmStatus(FLOWRATE_ZERO_ONE_LPM,8300 ));
    EXPECT_EQ(eInactive,alarmMgr_CheckLowOxygenConcentrationAlarmStatus(FLOWRATE_ZERO_ONE_LPM,8600 ));

    for (int i = 0; i < 1000/50*60*20; i++)
    	EXPECT_EQ(eInactive,alarmMgr_CheckLowOxygenConcentrationAlarmStatus(FlOWRATE_ZERO_TWO_FIVE_LPM,8300 ));
    EXPECT_EQ(eActive,alarmMgr_CheckLowOxygenConcentrationAlarmStatus(FlOWRATE_ZERO_TWO_FIVE_LPM,8300 ));
    EXPECT_EQ(eInactive,alarmMgr_CheckLowOxygenConcentrationAlarmStatus(FlOWRATE_ZERO_TWO_FIVE_LPM,8600 ));

    for (int i = 0; i < 1000/50*60*3; i++)
    	EXPECT_EQ(eInactive,alarmMgr_CheckLowOxygenConcentrationAlarmStatus(50,8300 ));
    EXPECT_EQ(eActive,alarmMgr_CheckLowOxygenConcentrationAlarmStatus(50,8300 ));
    EXPECT_EQ(eInactive,alarmMgr_CheckLowOxygenConcentrationAlarmStatus(50,8600 ));

    for (int i = 0; i < 1000/50*60*3; i++)
    	EXPECT_EQ(eInactive,alarmMgr_CheckLowOxygenConcentrationAlarmStatus(50,8300 ));
    EXPECT_EQ(eActive,alarmMgr_CheckLowOxygenConcentrationAlarmStatus(50,8300 ));
    EXPECT_EQ(eInactive,alarmMgr_CheckLowOxygenConcentrationAlarmStatus(50,8600 ));

    for (int i = 0; i < 1000/50*60*1; i++)
     	EXPECT_EQ(eInactive,alarmMgr_CheckLowOxygenConcentrationAlarmStatus(50,8300 ));
     EXPECT_EQ(eInactive,alarmMgr_CheckLowOxygenConcentrationAlarmStatus(50,8600 ));

    for (int i = 0; i < 1000/50*60*3; i++)
    	EXPECT_EQ(eInactive,alarmMgr_CheckLowOxygenConcentrationAlarmStatus(50,8300 ));
     EXPECT_EQ(eActive,alarmMgr_CheckLowOxygenConcentrationAlarmStatus(50,8300 ));

     EXPECT_EQ(eInactive,alarmMgr_CheckLowOxygenConcentrationAlarmStatus(50,8600 ));
}




TEST(AlarmMgr, alarmMgr_OxygenTankPressureErrorAlarmStatus)
{
    for (int i = 0; i < 1000/50*20; i++)
    	EXPECT_EQ(eInactive, alarmMgr_OxygenTankPressureErrorAlarmStatus(20));
    EXPECT_EQ(eInactive, alarmMgr_OxygenTankPressureErrorAlarmStatus(49));
    for (int i = 0; i < 1000/50*10 - 1; i++)
    	EXPECT_EQ(eInactive, alarmMgr_OxygenTankPressureErrorAlarmStatus(20));
    EXPECT_EQ(eInactive, alarmMgr_OxygenTankPressureErrorAlarmStatus(20));


    for (int i = 0; i < 1000/50*20; i++)
    	EXPECT_EQ(eInactive, alarmMgr_OxygenTankPressureErrorAlarmStatus(20));
    EXPECT_EQ(eInactive, alarmMgr_OxygenTankPressureErrorAlarmStatus(48));
    for (int i = 0; i < 1000/50*10 - 1; i++)
    	EXPECT_EQ(eInactive, alarmMgr_OxygenTankPressureErrorAlarmStatus(20));
    EXPECT_EQ(eActive, alarmMgr_OxygenTankPressureErrorAlarmStatus(20));
}





TEST(AlarmMgr, alarmMgr_CheckBatteryTemperatureErrorAlarmStatus)
{
    for (int i = 1; i <= 1000/50*12; i++)
    	EXPECT_EQ(eInactive, alarmMgr_CheckBatteryTemperatureErrorAlarmStatus(false, BATTERY_OVER_TEMP_ALARM_ERR_BIT));

    for (int i = 1; i < 1000/50*12; i++)
    	EXPECT_EQ(eInactive, alarmMgr_CheckBatteryTemperatureErrorAlarmStatus(true, BATTERY_OVER_TEMP_ALARM_ERR_BIT));
    EXPECT_EQ(eActive, alarmMgr_CheckBatteryTemperatureErrorAlarmStatus(true, BATTERY_OVER_TEMP_ALARM_ERR_BIT));
}






TEST(AlarmMgr, alarmMgr_CheckBatteryChargeErrorAlarmStatus)
{
    for (int i = 0; i < 1000/50*60*30; i++)
    	EXPECT_EQ(eInactive, alarmMgr_CheckBatteryChargeErrorAlarmStatus(true, true, false, false, !BATTERY_FULLY_CHARGED_BIT));
	EXPECT_EQ(eInactive, alarmMgr_CheckBatteryChargeErrorAlarmStatus(true, true, false, false, BATTERY_FULLY_CHARGED_BIT));

    for (int i = 0; i < 1000/50*60*30; i++)
    	EXPECT_EQ(eInactive, alarmMgr_CheckBatteryChargeErrorAlarmStatus(false, true, false, false, !BATTERY_FULLY_CHARGED_BIT));
	EXPECT_EQ(eInactive, alarmMgr_CheckBatteryChargeErrorAlarmStatus(false, true, false, false, BATTERY_FULLY_CHARGED_BIT));

    for (int i = 0; i < 1000/50*60*30; i++)
    	EXPECT_EQ(eInactive, alarmMgr_CheckBatteryChargeErrorAlarmStatus(true, true, false, false, !BATTERY_FULLY_CHARGED_BIT));
	EXPECT_EQ(eActive, alarmMgr_CheckBatteryChargeErrorAlarmStatus(true, true, false, false, !BATTERY_FULLY_CHARGED_BIT));

	alarmMgr_ReSetupForCheckBatteryChargeErrorAlarmStatus();
    for (int i = 0; i < 1000/50*10; i++)
    	EXPECT_EQ(eInactive, alarmMgr_CheckBatteryChargeErrorAlarmStatus(true, true, true, false, BATTERY_FULLY_CHARGED_BIT));
	EXPECT_EQ(eActive, alarmMgr_CheckBatteryChargeErrorAlarmStatus(true, true, true, false, BATTERY_FULLY_CHARGED_BIT));
}





TEST(AlarmMgr, alarmMgr_CheckBatteryRemainingCapacityLowerAlarmStatus)
{
    for (int i = 1; i < 1000/50*2; i++)
    	EXPECT_EQ(eInactive, alarmMgr_CheckBatteryRemainingCapacityLowerAlarmStatus(20, true, false, eBatterySource));
    EXPECT_EQ(eActive, alarmMgr_CheckBatteryRemainingCapacityLowerAlarmStatus(20, true, false, eBatterySource));

    EXPECT_EQ(eInactive, alarmMgr_CheckBatteryRemainingCapacityLowerAlarmStatus(20, true, true, eBatterySource));
    EXPECT_EQ(eInactive, alarmMgr_CheckBatteryRemainingCapacityLowerAlarmStatus(20, true, true, eBatterySource));

    for (int i = 1; i < 1000/50*2; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckBatteryRemainingCapacityLowerAlarmStatus(19, true, false, eBatterySource));
    EXPECT_EQ(eActive, alarmMgr_CheckBatteryRemainingCapacityLowerAlarmStatus(19, true, false, eBatterySource));

    EXPECT_EQ(eInactive, alarmMgr_CheckBatteryRemainingCapacityLowerAlarmStatus(21, true, true, eBatterySource));
    EXPECT_EQ(eInactive, alarmMgr_CheckBatteryRemainingCapacityLowerAlarmStatus(21, true, true, eBatterySource));

    for (int i = 1; i < 1000/50*2; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckBatteryRemainingCapacityLowerAlarmStatus(10, true, false, eBatterySource));
    EXPECT_EQ(eActive, alarmMgr_CheckBatteryRemainingCapacityLowerAlarmStatus(10, true, false, eBatterySource));
    EXPECT_EQ(eInactive, alarmMgr_CheckBatteryRemainingCapacityLowerAlarmStatus(10, false, false, eBatterySource));

    for (int i = 1; i < 1000/50*10; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckBatteryRemainingCapacityVelyLowAlarmStatus(7, true, false, eBatterySource));
    EXPECT_EQ(eActive, alarmMgr_CheckBatteryRemainingCapacityVelyLowAlarmStatus(7, true, false, eBatterySource));
}




TEST(AlarmMgr, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus)
{
	alarmMgr_SetupCheckCompressorSupplyPressureErrorAlarmStatus(eContinuosMode);
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(FLOWRATE_ONE_LPM, 62, ePSAHighTemperature));
    EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(FLOWRATE_ONE_LPM, 63, ePSAHighTemperature));

    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(FLOWRATE_ONE_LPM, 62, ePSAHighTemperature));
    EXPECT_EQ(eActive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(FLOWRATE_ONE_LPM, 62, ePSAHighTemperature));


	alarmMgr_SetupCheckCompressorSupplyPressureErrorAlarmStatus(eContinuosMode);
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(FLOWRATE_ONE_LPM, 71, ePSANormalTemperature));
    EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(FLOWRATE_ONE_LPM, 72, ePSANormalTemperature));

    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(FLOWRATE_ONE_LPM, 71, ePSANormalTemperature));
    EXPECT_EQ(eActive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(FLOWRATE_ONE_LPM, 71, ePSANormalTemperature));

	alarmMgr_SetupCheckCompressorSupplyPressureErrorAlarmStatus(eContinuosMode);
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(FLOWRATE_ONE_LPM, 71, ePSALowTemperature));
    EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(FLOWRATE_ONE_LPM, 72, ePSALowTemperature));

    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(FLOWRATE_ONE_LPM, 71, ePSALowTemperature));
    EXPECT_EQ(eActive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(FLOWRATE_ONE_LPM, 71, ePSALowTemperature));

    // continuous mode, 0.75Lpm, Low Temperature
	alarmMgr_SetupCheckCompressorSupplyPressureErrorAlarmStatus(eContinuosMode);
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(75, 71, ePSALowTemperature));
    EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(75, 72, ePSALowTemperature));

    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(75, 71, ePSALowTemperature));
    EXPECT_EQ(eActive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(75, 71, ePSALowTemperature));

    // continuous mode, 0.75Lpm, High Temperature
	alarmMgr_SetupCheckCompressorSupplyPressureErrorAlarmStatus(eContinuosMode);
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(75, 71, ePSAHighTemperature));
    EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(75, 72, ePSAHighTemperature));
    		//max pressure less than 72Kpa
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(75, 71, ePSAHighTemperature));
    EXPECT_EQ(eActive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(75, 71, ePSAHighTemperature));


    // Synchronized mode, 0.25Lpm, High Temperature
	alarmMgr_SetupCheckCompressorSupplyPressureErrorAlarmStatus(eSynchronizedMode);
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(25, 44, ePSAHighTemperature));
    EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(25, 45, ePSAHighTemperature));
    	//max pressure less than 45Kpa
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(25, 44, ePSAHighTemperature));
    EXPECT_EQ(eActive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(25, 44, ePSAHighTemperature));

    // Synchronized mode, 0.50Lpm, High Temperature
	alarmMgr_SetupCheckCompressorSupplyPressureErrorAlarmStatus(eSynchronizedMode);
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(50, 49, ePSAHighTemperature));
    EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(50, 50, ePSAHighTemperature));
    	//max pressure less than 50Kpa
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(50, 49, ePSAHighTemperature));
    EXPECT_EQ(eActive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(50, 49, ePSAHighTemperature));

    // Synchronized mode, 0.75Lpm, High Temperature
	alarmMgr_SetupCheckCompressorSupplyPressureErrorAlarmStatus(eSynchronizedMode);
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(75, 53, ePSAHighTemperature));
    EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(75, 54, ePSAHighTemperature));
    	//max pressure less than 54Kpa
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(75, 53, ePSAHighTemperature));
    EXPECT_EQ(eActive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(75, 53, ePSAHighTemperature));

    // Synchronized mode, 0.75Lpm, High Temperature
	alarmMgr_SetupCheckCompressorSupplyPressureErrorAlarmStatus(eSynchronizedMode);
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(75, 53, ePSAHighTemperature));
    EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(75, 54, ePSAHighTemperature));
    	//max pressure less than 54Kpa
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(75, 53, ePSAHighTemperature));
    EXPECT_EQ(eActive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(75, 53, ePSAHighTemperature));

    // Synchronized mode, 1.00Lpm, High Temperature
	alarmMgr_SetupCheckCompressorSupplyPressureErrorAlarmStatus(eSynchronizedMode);
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(100, 62, ePSAHighTemperature));
    EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(100, 63, ePSAHighTemperature));
    	//max pressure less than 63Kpa
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(100, 62, ePSAHighTemperature));
    EXPECT_EQ(eActive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(100, 62, ePSAHighTemperature));

    // Synchronized mode, 1.25Lpm, High Temperature
	alarmMgr_SetupCheckCompressorSupplyPressureErrorAlarmStatus(eSynchronizedMode);
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(125, 62, ePSAHighTemperature));
    EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(125, 63, ePSAHighTemperature));
    	//max pressure less than 63Kpa
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(125, 62, ePSAHighTemperature));
    EXPECT_EQ(eActive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(125, 62, ePSAHighTemperature));

    // Synchronized mode, 1.50Lpm, High Temperature
	alarmMgr_SetupCheckCompressorSupplyPressureErrorAlarmStatus(eSynchronizedMode);
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(150, 62, ePSAHighTemperature));
    EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(150, 63, ePSAHighTemperature));
    	//max pressure less than 63Kpa
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(150, 62, ePSAHighTemperature));
    EXPECT_EQ(eActive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(150, 62, ePSAHighTemperature));

    // Synchronized mode, 1.75Lpm, High Temperature
	alarmMgr_SetupCheckCompressorSupplyPressureErrorAlarmStatus(eSynchronizedMode);
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(175, 67, ePSAHighTemperature));
    EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(175, 68, ePSAHighTemperature));
    	//max pressure less than 68Kpa
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(175, 67, ePSAHighTemperature));
    EXPECT_EQ(eActive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(175, 67, ePSAHighTemperature));

    // Synchronized mode, 2.00Lpm, High Temperature
	alarmMgr_SetupCheckCompressorSupplyPressureErrorAlarmStatus(eSynchronizedMode);
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(200, 67, ePSAHighTemperature));
    EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(200, 68, ePSAHighTemperature));
    	//max pressure less than 68Kpa
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(200, 67, ePSAHighTemperature));
    EXPECT_EQ(eActive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(200, 67, ePSAHighTemperature));

    // Synchronized mode, 2.50Lpm, High Temperature
	alarmMgr_SetupCheckCompressorSupplyPressureErrorAlarmStatus(eSynchronizedMode);
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(250, 71, ePSAHighTemperature));
    EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(250, 72, ePSAHighTemperature));
    	//max pressure less than 72Kpa
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(250, 71, ePSAHighTemperature));
    EXPECT_EQ(eActive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(250, 71, ePSAHighTemperature));

    // Synchronized mode, 3.00Lpm, High Temperature
	alarmMgr_SetupCheckCompressorSupplyPressureErrorAlarmStatus(eSynchronizedMode);
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(300, 71, ePSAHighTemperature));
    EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(300, 72, ePSAHighTemperature));
    	//max pressure less than 72Kpa
    for (int i = 1; i <= 1000/50*30; i++)
     	EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(300, 71, ePSAHighTemperature));
    EXPECT_EQ(eActive, alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(300, 71, ePSAHighTemperature));
}


TEST(AlarmMgr, alarmMgr_CheckCompressorSuctionPressureErrorAlarmStatus)
{
	 for (int i = 1; i <= 1000/50*30; i++)
		 EXPECT_EQ(eInactive,alarmMgr_CheckCompressorSuctionPressureErrorAlarmStatus(-58));
	 EXPECT_EQ(eInactive,alarmMgr_CheckCompressorSuctionPressureErrorAlarmStatus(-59));
	 for (int i = 1; i <= 1000/50*30; i++)
		 EXPECT_EQ(eInactive,alarmMgr_CheckCompressorSuctionPressureErrorAlarmStatus(-58));
	 EXPECT_EQ(eActive,alarmMgr_CheckCompressorSuctionPressureErrorAlarmStatus(-58));
}


TEST(AlarmMgr, alarmMgr_CheckFlowControlErrorAlarmStatus)
{
	alarmMgr_SetupForCheckFlowControlErrorAlarmStatus();
	for (int i = 1; i <= 1000/50*40; i++)
		 EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(10, 20));

	for (int i = 1; i <= 1000/50*40; i++)
		 EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(10,31));
	EXPECT_EQ(eActive,alarmMgr_CheckFlowControlErrorAlarmStatus(10,31));

	EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(10,30));
	// case 0.25Lpm
	for (int i = 1; i <= 1000/50*40; i++)
		 EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(25,46));
	EXPECT_EQ(eActive,alarmMgr_CheckFlowControlErrorAlarmStatus(25,46));

	EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(25,45));

	for (int i = 1; i <= 1000/50*40; i++)
		 EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(25,4));
	EXPECT_EQ(eActive,alarmMgr_CheckFlowControlErrorAlarmStatus(25,4));

	EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(25,5));

	// case 0.50Lpm
	for (int i = 1; i <= 1000/50*40; i++)
		 EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(50,71));
	EXPECT_EQ(eActive,alarmMgr_CheckFlowControlErrorAlarmStatus(50,71));

	EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(50,70));

	for (int i = 1; i <= 1000/50*40; i++)
		 EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(50,29));
	EXPECT_EQ(eActive,alarmMgr_CheckFlowControlErrorAlarmStatus(50,29));

	EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(50,30));

	// case 0.75Lpm
	for (int i = 1; i <= 1000/50*40; i++)
		 EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(75,96));
	EXPECT_EQ(eActive,alarmMgr_CheckFlowControlErrorAlarmStatus(75,96));

	EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(75,85));

	for (int i = 1; i <= 1000/50*40; i++)
		 EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(75,54));
	EXPECT_EQ(eActive,alarmMgr_CheckFlowControlErrorAlarmStatus(75,54));

	EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(75,55));

	// case 1.00Lpm
	for (int i = 1; i <= 1000/50*40; i++)
		 EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(100,121));
	EXPECT_EQ(eActive,alarmMgr_CheckFlowControlErrorAlarmStatus(100,121));

	EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(100,120));

	for (int i = 1; i <= 1000/50*40; i++)
		 EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(100,79));
	EXPECT_EQ(eActive,alarmMgr_CheckFlowControlErrorAlarmStatus(100,79));

	EXPECT_EQ(eInactive,alarmMgr_CheckFlowControlErrorAlarmStatus(100,80));

}


TEST(AlarmMgr, alarmMgr_CheckSupplyPressureSensorErrorAlarmStatus)
{
	EXPECT_EQ(eActive,alarmMgr_CheckSupplyPressureSensorErrorAlarmStatus(201));
}


TEST(AlarmMgr, alarmMgr_CheckVacuumPressureSensorErrorAlarmStatus)
{
	EXPECT_EQ(eActive,alarmMgr_CheckVacuumPressureSensorErrorAlarmStatus(-101));
}

TEST(AlarmMgr, alarmMgr_CheckTankPressureSensorErrorAlarmStatus)
{
	EXPECT_EQ(eActive,alarmMgr_CheckTankPressureSensorErrorAlarmStatus(101));
}


TEST(AlarmMgr, alarmMgr_CheckDifferentialPressureSensorCommunicationErrorAlarmStatus)
{
	 for (int i = 1; i <= 1000/50*60; i++)
		 EXPECT_EQ(eInactive,alarmMgr_CheckDifferentialPressureSensorCommunicationErrorAlarmStatus(ERROR_DIFFPRESS_CONST));
	 EXPECT_EQ(eActive,alarmMgr_CheckDifferentialPressureSensorCommunicationErrorAlarmStatus(ERROR_DIFFPRESS_CONST));
}

TEST(AlarmMgr, alarmMgr_CheckOxygenSensorErrorAlarmStatus)
{
	 for (int i = 1; i < 1000/50*30; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckOxygenSensorErrorAlarmStatus(900));
	 EXPECT_EQ(eActive, alarmMgr_CheckOxygenSensorErrorAlarmStatus(900));
}


TEST(AlarmMgr, alarmMgr_CheckFlowControllerCommunicationNoResponseErrorAlarmStatus)
{
	 for (int i = 1; i < 1000/50*60; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckFlowControllerCommunicationNoResponseErrorAlarmStatus(FLOWCONTROLLER_COMMUNICATION_NO_RESPONSE_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckFlowControllerCommunicationNoResponseErrorAlarmStatus(FLOWCONTROLLER_COMMUNICATION_NO_RESPONSE_ERR_BIT));
}


TEST(AlarmMgr, alarmMgr_CheckFlowControllerPressureSensorErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckFlowControllerPressureSensorErrorAlarmStatus(FLOWCONTROLLER_PRESSURE_SENSOR_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckFlowControllerPressureSensorErrorAlarmStatus(FLOWCONTROLLER_PRESSURE_SENSOR_ERR_BIT));
}


TEST(AlarmMgr, alarmMgr_CheckFlowControllerFlowSensorErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckFlowControllerFlowSensorErrorAlarmStatus(FLOWCONTROLLER_FLOW_SENSOR_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckFlowControllerFlowSensorErrorAlarmStatus(FLOWCONTROLLER_FLOW_SENSOR_ERR_BIT));
}


TEST(AlarmMgr, alarmMgr_CheckFlowControllerTemperatureSensorErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckFlowControllerTemperatureSensorErrorAlarmStatus(FLOWCONTROLLER_TEMPERATURE_SENSOR_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckFlowControllerTemperatureSensorErrorAlarmStatus(FLOWCONTROLLER_TEMPERATURE_SENSOR_ERR_BIT));
}



TEST(AlarmMgr, alarmMgr_CheckFlowControllerCommunicationErrorAlarmStatus)
{
	 EXPECT_EQ(eActive, alarmMgr_CheckFlowControllerCommunicationErrorAlarmStatus(FLOWCONTROLLER_CMD_TIMEOUT_ERR_BIT));
}


TEST(AlarmMgr, alarmMgr_CheckFlowControllerCommunicationChecksumErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckFlowControllerCommunicationChecksumErrorAlarmStatus(FLOWCONTROLLER_CHECKSUM_COMMUNICATION_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckFlowControllerCommunicationChecksumErrorAlarmStatus(FLOWCONTROLLER_CHECKSUM_COMMUNICATION_ERR_BIT));
}


TEST(AlarmMgr, alarmMgr_CheckFlowControllerCommunicationCommandErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckFlowControllerCommunicationCommandErrorAlarmStatus(FLOWCONTROLLER_COMMAND_COMMUNICATION_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckFlowControllerCommunicationCommandErrorAlarmStatus(FLOWCONTROLLER_COMMAND_COMMUNICATION_ERR_BIT));
}

TEST(AlarmMgr, alarmMgr_CheckFlowControllerCommunicationParameterErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckFlowControllerCommunicationParameterErrorAlarmStatus(FLOWCONTROLLER_PARAMETER_COMMUNICATION_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckFlowControllerCommunicationParameterErrorAlarmStatus(FLOWCONTROLLER_PARAMETER_COMMUNICATION_ERR_BIT));
}


TEST(AlarmMgr, alarmMgr_CheckFlowControllerResponseCheckSumErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckFlowControllerResponseCheckSumErrorAlarmStatus(eResponseCheckSumErr));
	 EXPECT_EQ(eActive, alarmMgr_CheckFlowControllerResponseCheckSumErrorAlarmStatus(eResponseCheckSumErr));
}

TEST(AlarmMgr, alarmMgr_CheckFlowControllerResponseCodeErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckFlowControllerResponseCodeErrorAlarmStatus(eResponseCodeErr));
	 EXPECT_EQ(eActive, alarmMgr_CheckFlowControllerResponseCodeErrorAlarmStatus(eResponseCodeErr));
}

TEST(AlarmMgr, alarmMgr_CheckFlowControllerResponseDataLengthErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckFlowControllerResponseDataLengthErrorAlarmStatus(eResponseLengthErr));
	 EXPECT_EQ(eActive, alarmMgr_CheckFlowControllerResponseDataLengthErrorAlarmStatus(eResponseLengthErr));
}

TEST(AlarmMgr, alarmMgr_CheckCompressorCommunicationNoResponseErrorAlarmStatus)
{
	 for (int i = 1; i < 1000/50*60; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckCompressorCommunicationNoResponseErrorAlarmStatus(COMP_COMMUNICATION_NO_RESPONSE_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckCompressorCommunicationNoResponseErrorAlarmStatus(COMP_COMMUNICATION_NO_RESPONSE_ERR_BIT));
}


TEST(AlarmMgr, alarmMgr_CheckCompressorDCOverVoltageErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckCompressorDCOverVoltageErrorAlarmStatus(DC_OVER_VOLT_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckCompressorDCOverVoltageErrorAlarmStatus(DC_OVER_VOLT_ERR_BIT));
}


TEST(AlarmMgr, alarmMgr_CheckCompressorDCLowerVoltageErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckCompressorDCLowerVoltageErrorAlarmStatus(DC_LOW_VOLT_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckCompressorDCLowerVoltageErrorAlarmStatus(DC_LOW_VOLT_ERR_BIT));
}


TEST(AlarmMgr, alarmMgr_CheckCompressorSoftwareOverCurrentErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckCompressorSoftwareOverCurrentErrorAlarmStatus(SOFT_OVER_CURRENT_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckCompressorSoftwareOverCurrentErrorAlarmStatus(SOFT_OVER_CURRENT_ERR_BIT));
}

TEST(AlarmMgr, alarmMgr_CheckCompressorHardwareOverCurrentErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckCompressorHardwareOverCurrentErrorAlarmStatus(HARD_OVER_CURRENT_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckCompressorHardwareOverCurrentErrorAlarmStatus(HARD_OVER_CURRENT_ERR_BIT));
}

TEST(AlarmMgr, alarmMgr_CheckCompressorBoardTemperatureErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckCompressorBoardTemperatureErrorAlarmStatus(BOARD_TEMP_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckCompressorBoardTemperatureErrorAlarmStatus(BOARD_TEMP_ERR_BIT));
}


TEST(AlarmMgr, alarmMgr_CheckCompressorMotorTemperatureErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckCompressorMotorTemperatureErrorAlarmStatus(MOTOR_TEMP_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckCompressorMotorTemperatureErrorAlarmStatus(MOTOR_TEMP_ERR_BIT));
}

TEST(AlarmMgr, alarmMgr_CheckCompressorRotationSpeedErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckCompressorRotationSpeedErrorAlarmStatus(RPM_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckCompressorRotationSpeedErrorAlarmStatus(RPM_ERR_BIT));
}


TEST(AlarmMgr, alarmMgr_CheckCompressorStepOutErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckCompressorStepOutErrorAlarmStatus(MOTOR_STEP_OUT_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckCompressorStepOutErrorAlarmStatus(MOTOR_STEP_OUT_ERR_BIT));
}

TEST(AlarmMgr, alarmMgr_CheckCompressorCommunicationErrorAlarmStatus)
{

	 EXPECT_EQ(eActive, alarmMgr_CheckCompressorCommunicationErrorAlarmStatus(COMP_CMD_TIME_OUT_BIT));
}

TEST(AlarmMgr, alarmMgr_CheckCompressorCommunicationChecksumErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckCompressorCommunicationChecksumErrorAlarmStatus(COMPRESSOR_CHECKSUM_COMMUNICATION_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckCompressorCommunicationChecksumErrorAlarmStatus(COMPRESSOR_CHECKSUM_COMMUNICATION_ERR_BIT));
}


TEST(AlarmMgr, alarmMgr_CheckCompressorCommunicationCommandErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckCompressorCommunicationCommandErrorAlarmStatus(COMPRESSOR_COMMAND_COMMUNICATION_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckCompressorCommunicationCommandErrorAlarmStatus(COMPRESSOR_COMMAND_COMMUNICATION_ERR_BIT));
}


TEST(AlarmMgr, alarmMgr_CheckCompressorCommunicationParameterErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckCompressorCommunicationParameterErrorAlarmStatus(COMPRESSOR_PARAMETER_COMMUNICATION_ERR_BIT));
	 EXPECT_EQ(eActive, alarmMgr_CheckCompressorCommunicationParameterErrorAlarmStatus(COMPRESSOR_PARAMETER_COMMUNICATION_ERR_BIT));
}


TEST(AlarmMgr, alarmMgr_CheckCompressorResponseCheckSumErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckCompressorResponseCheckSumErrorAlarmStatus(eCompResponseChecksumErr));
	 EXPECT_EQ(eActive, alarmMgr_CheckCompressorResponseCheckSumErrorAlarmStatus(eCompResponseChecksumErr));
}

TEST(AlarmMgr, alarmMgr_CheckCompressorResponseCodeErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckCompressorResponseCodeErrorAlarmStatus(eCompResponseCodeErr));
	 EXPECT_EQ(eActive, alarmMgr_CheckCompressorResponseCodeErrorAlarmStatus(eCompResponseCodeErr));
}

TEST(AlarmMgr, alarmMgr_CheckCompressorResponseDataLengthErrorAlarmStatus)
{
	 for (int i = 1; i < 3; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckCompressorResponseDataLengthErrorAlarmStatus(eCompResponseLengthErr));
	 EXPECT_EQ(eActive, alarmMgr_CheckCompressorResponseDataLengthErrorAlarmStatus(eCompResponseLengthErr));
}

TEST(AlarmMgr, alarmMgr_CheckAudioCommunicateErrorAlarmStatus)
{
	 EXPECT_EQ(eActive, alarmMgr_CheckAudioCommunicateErrorAlarmStatus(true));
}


TEST(AlarmMgr, alarmMgr_CheckSpeakerErrorAlarmStatus)
{
	 EXPECT_EQ(eActive, alarmMgr_CheckSpeakerErrorAlarmStatus(false));
}


TEST(AlarmMgr, alarmMgr_CheckBatteryTypeAlarmStatus)
{
	 for (int i = 1; i < 1000/50*10; i++)
		 EXPECT_EQ(eInactive, alarmMgr_CheckBatteryTypeAlarmStatus(true, true));
	 EXPECT_EQ(eActive, alarmMgr_CheckBatteryTypeAlarmStatus(true, true));

	 EXPECT_EQ(eInactive, alarmMgr_CheckBatteryTypeAlarmStatus(true, false));
}

TEST(AlarmMgr, alarmMgr_CheckTachypneaErrorAlarmStatus)
{
	alarmMgr_SetupForCheckTachypneaErrorAlarmStatus();
	for (int i = 1; i <= 1000/50*30; i++)
	{
		alarmMgr_CheckTachypneaErrorAlarmStatus(51, true);
		alarmMgr_CheckTachypneaErrorAlarmStatus(51, false);
	}
	EXPECT_EQ(eActive, alarmMgr_CheckTachypneaErrorAlarmStatus(51, true));
	EXPECT_EQ(eInactive, alarmMgr_CheckTachypneaErrorAlarmStatus(30, false));
}
