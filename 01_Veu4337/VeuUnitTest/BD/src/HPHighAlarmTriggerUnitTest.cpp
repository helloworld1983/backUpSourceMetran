#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "HPHighAlarmTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "AlarmMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"

#define DELAY_TIME_50MS 25

HPHighAlarmTrigger* HPHighAlarmTrigger::S_Instance = NULL;
HPHighAlarmTrigger::HPHighAlarmTrigger():Trigger(eHPHighAlarmTrigger)
{
    highAlarmDelay = 0;
    highAlarmClearDelay = 0;

    lowAlarmDelay = 0;
    lowAlarmClearDelay = 0;

    InactiveTimer =0;

    alarmFlag = false;
}   // end HPHighAlarmTrigger()
E_Bool HPHighAlarmTrigger::CheckCondition(void)
{
    static const USHORT ABOVE15_PRESS_TIME = 15;
//    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();

    LONG highPressLimit = (LONG) mgrBdSettingPtrGetCurrentValue(eAPRVHighPHighLimit);;
//    LONG lowPressLimit = (LONG) mgrBdSettingPtr->GetCurrentValue(eAPRVLowPHighLimit);;

    if (InactiveTimer > 0)
    {
        InactiveTimer--;
        return (eFalse);
    }

    //   Check High Press Alarm
    if ((PressureSensorGetLastReading() > highPressLimit)||
            (PressureSensorGetLastReading() > highPressLimit))
    {

        highAlarmDelay++;

        highAlarmClearDelay = 0;

        if(highAlarmDelay >= ABOVE15_PRESS_TIME) //
        {

            highAlarmDelay = 0;
            alarmFlag = true;
            // If the pressure condition is met, set the TriggerDidFire attribute
            // and update the alarm status.
            TriggerDidFire = eTrue;

            BDAlarmTaskSetIsActive(eAPRVHigh_PHigh, eActive);
            return TriggerDidFire;
        }
    }

    return (eFalse);
}   // end CheckCondition()
void HPHighAlarmTrigger::DisableTrigger(void)
{
    highAlarmDelay = 0;
    highAlarmClearDelay = 0;

    lowAlarmDelay = 0;
    lowAlarmClearDelay = 0;
    // Must check IsEnabled because HIP2 alarm is always disabled at the
    // start of exhalation (end of inhalation), but it is not always enabled.
    if ((IsEnabled == eTrue) && (TriggerDidFire == eFalse))
    {
    }
    TriggerDidFire = eFalse;

    // Invoke Base Class Disable Method
//    Trigger::DisableTrigger();

}   // end DisableTrigger()


class HPHighAlarmTriggerUnitMock : public BDTestFixture
{
public:
    HPHighAlarmTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~HPHighAlarmTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyHPHighAlarmTrigger = new HPHighAlarmTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyHPHighAlarmTrigger;
    }
    HPHighAlarmTrigger *MyHPHighAlarmTrigger;
};

//test for HPHighAlarmTrigger::HPHighAlarmTrigger():Trigger(eHPHighAlarmTrigger)
TEST_F(HPHighAlarmTriggerUnitMock, HPHighAlarmTrigger)
{
    EXPECT_EQ(MyHPHighAlarmTrigger->highAlarmDelay, 0);
    EXPECT_EQ(MyHPHighAlarmTrigger->highAlarmClearDelay, 0);
    EXPECT_EQ(MyHPHighAlarmTrigger->lowAlarmDelay, 0);
    EXPECT_EQ(MyHPHighAlarmTrigger->lowAlarmClearDelay, 0);
    EXPECT_EQ(MyHPHighAlarmTrigger->InactiveTimer, 0);
    EXPECT_EQ(MyHPHighAlarmTrigger->alarmFlag, false);
}

//test for  HPHighAlarmTrigger* HPHighAlarmTrigger::S_GetInstance(void)
TEST_F(HPHighAlarmTriggerUnitMock, S_GetInstance)
{
    MyHPHighAlarmTrigger->S_GetInstance();

    EXPECT_NE(MyHPHighAlarmTrigger->S_Instance, (void*)0);
}

//test for E_Bool HPHighAlarmTrigger::CheckCondition(void)
TEST_F(HPHighAlarmTriggerUnitMock, CheckCondition_0)
{
    MyHPHighAlarmTrigger->InactiveTimer = 10;

    EXPECT_EQ(MyHPHighAlarmTrigger->CheckCondition(), eFalse);

    EXPECT_EQ(MyHPHighAlarmTrigger->InactiveTimer, 9);
}
TEST_F(HPHighAlarmTriggerUnitMock, CheckCondition_1)
{
    MyHPHighAlarmTrigger->InactiveTimer = 0;
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eAPRVHighPHighLimit)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_StepMotor, PressureSensorGetLastReading()).Times(1).WillOnce(Return(2));
    MyHPHighAlarmTrigger->highAlarmDelay = 15;

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eAPRVHigh_PHigh, eActive)).Times(1);

    EXPECT_EQ(MyHPHighAlarmTrigger->CheckCondition(), eTrue);
    EXPECT_EQ(MyHPHighAlarmTrigger->highAlarmDelay, 0);
}

//test for void HPHighAlarmTrigger::DisableTrigger(void)
TEST_F(HPHighAlarmTriggerUnitMock, DisableTrigger)
{
    MyHPHighAlarmTrigger->DisableTrigger();

    EXPECT_EQ(MyHPHighAlarmTrigger->highAlarmDelay, 0);
    EXPECT_EQ(MyHPHighAlarmTrigger->highAlarmClearDelay, 0);
    EXPECT_EQ(MyHPHighAlarmTrigger->lowAlarmDelay, 0);
    EXPECT_EQ(MyHPHighAlarmTrigger->lowAlarmClearDelay, 0);
    EXPECT_EQ(MyHPHighAlarmTrigger->TriggerDidFire, eFalse);
}

#endif //end define BD_TRIGGER_UNIT_TEST
