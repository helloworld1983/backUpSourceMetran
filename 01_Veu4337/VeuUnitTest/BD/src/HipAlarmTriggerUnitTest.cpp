#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "HipAlarmTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "AlarmMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"

HipAlarmTrigger* HipAlarmTrigger::S_Instance = NULL;
HipAlarmTrigger* HipAlarmTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new HipAlarmTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end Hip2AlarmTrigger()
HipAlarmTrigger::HipAlarmTrigger (E_TriggerId triggerId, SHORT alarmIx) :
                Trigger (triggerId), AlarmIx (alarmIx)
{

}   // end HipAlarmTrigger()

HipAlarmTrigger::HipAlarmTrigger() : Trigger()
{

}
E_Bool HipAlarmTrigger::CheckCondition (void)
{
    const LONG DELAY_50S = 15;
    //    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();

    LONG inhPressLimit;

    if (InactiveTimer > 0)
    {
        InactiveTimer--;
        return (eFalse);
    }

    inhPressLimit = (LONG) mgrBdSettingPtrGetCurrentValue(eHipLimit);

    LONG InhLastPressSensor = 20; //test
    LONG ExhLastPressSensor = 20; //test

    BDRequestDevice(eInhPressSensorGetLastReading);
    BDRequestDevice(eExhPressSensorGetLastReading);


    if ((InhLastPressSensor > inhPressLimit)||
            (ExhLastPressSensor > inhPressLimit))
    {
        hip1Count++;
        if(hip1Count >= DELAY_50S) //delay 50ms
        {
            hip1Count = 0;
            // If the pressure condition is met, set the TriggerDidFire attribute
            // and update the alarm status.
            TriggerDidFire = eTrue;

            if(eHigh != BDAlarmTaskGetLevel(eHip))
            {
                BDAlarmTaskSetIsActive(eHip, eActive);
            }
        }
    }
    else
    {
        hip1Count = 0;
    }

    return (TriggerDidFire);

}   // end CheckCondition()
void HipAlarmTrigger::DisableTrigger (void)
{
    // Must check IsEnabled because HIP2 alarm is always disabled at the
    // start of exhalation (end of inhalation), but it is not always enabled.
    if((IsEnabled == eTrue) && (TriggerDidFire == eFalse))
    {
        // If alarm was active, it no longer is
        BDAlarmTaskSetIsActive(eHip, eNotActive);
    }

    // Invoke Base Class Disable Method
    //    Trigger::DisableTrigger ();

}   // end DisableTrigger()
void HipAlarmTrigger::SetInactiveTimer (USHORT value)
{
    InactiveTimer = value;
}

class HipAlarmTriggerUnitMock : public BDTestFixture
{
public:
    HipAlarmTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~HipAlarmTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyHipAlarmTrigger = new HipAlarmTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyHipAlarmTrigger;
    }
    HipAlarmTrigger *MyHipAlarmTrigger;
};

//test for HipAlarmTrigger* HipAlarmTrigger::S_GetInstance(void)
TEST_F(HipAlarmTriggerUnitMock, S_GetInstance)
{
    MyHipAlarmTrigger->S_GetInstance();

    EXPECT_NE(MyHipAlarmTrigger->S_Instance, (void*)0);
}

//test for HipAlarmTrigger::HipAlarmTrigger (E_TriggerId triggerId, SHORT alarmIx)
TEST_F(HipAlarmTriggerUnitMock, HipAlarmTrigger_WithPara)
{
    //no source, not need to test
}

//test for HipAlarmTrigger::HipAlarmTrigger() : Trigger()
TEST_F(HipAlarmTriggerUnitMock, HipAlarmTrigger)
{
    //no source, not need to test
}

//test for E_Bool HipAlarmTrigger::CheckCondition (void)
TEST_F(HipAlarmTriggerUnitMock, CheckCondition_0)
{
    MyHipAlarmTrigger->InactiveTimer = 5;

    EXPECT_EQ(MyHipAlarmTrigger->CheckCondition(), eFalse);
    EXPECT_EQ(MyHipAlarmTrigger->InactiveTimer, 4);
}
TEST_F(HipAlarmTriggerUnitMock, CheckCondition_1)
{
    MyHipAlarmTrigger->InactiveTimer = 0;
    const LONG DELAY_50S = 15;

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eHipLimit)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_StepMotor, BDRequestDevice(eInhPressSensorGetLastReading)).Times(1);
    EXPECT_CALL(*_StepMotor, BDRequestDevice(eExhPressSensorGetLastReading)).Times(1);

    MyHipAlarmTrigger->hip1Count = DELAY_50S + 1;

    EXPECT_CALL(*_Alarm, BDAlarmTaskGetLevel(eHip)).Times(1).WillOnce(Return(eLow));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eHip, eActive)).Times(1).WillOnce(Return(eLow));

    EXPECT_EQ(MyHipAlarmTrigger->CheckCondition(), eTrue);
}

//test for void HipAlarmTrigger::DisableTrigger (void)
TEST_F(HipAlarmTriggerUnitMock, DisableTrigger)
{
    MyHipAlarmTrigger->IsEnabled = eTrue;
    MyHipAlarmTrigger->TriggerDidFire = eFalse;

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eHip, eNotActive)).Times(1);

    MyHipAlarmTrigger->DisableTrigger();
}

//test for void HipAlarmTrigger::SetInactiveTimer (USHORT value)
TEST_F(HipAlarmTriggerUnitMock, SetInactiveTimer)
{
    MyHipAlarmTrigger->SetInactiveTimer(10);

    EXPECT_EQ(MyHipAlarmTrigger->InactiveTimer, 10);
}

#endif //end define BD_TRIGGER_UNIT_TEST
