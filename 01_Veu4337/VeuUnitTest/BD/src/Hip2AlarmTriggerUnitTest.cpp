#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "Hip2AlarmTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"
#include "AlarmMockHeader.h"
#include "ModeMgrMockHeader.h"

// Define statics
Hip2AlarmTrigger* Hip2AlarmTrigger::S_Instance = NULL;
Hip2AlarmTrigger::Hip2AlarmTrigger()
    : Trigger(eHip2AlarmTrigger)
{
    HighPressAlarm  = 20 * SCALE;
    InactiveTimer = 0;

}   // end Hip2AlarmTrigger()
Hip2AlarmTrigger* Hip2AlarmTrigger::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new Hip2AlarmTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end Hip2AlarmTrigger()
void Hip2AlarmTrigger::DisableTrigger(void)
{
    // Must check IsEnabled because HIP2 alarm is always disabled at the
    // start of exhalation (end of inhalation), but it is not always enabled.
    if ((IsEnabled == eTrue) && (TriggerDidFire == eFalse))
    {
        // If alarm was active, it no longer is
        BDAlarmTaskSetIsActive(eExtremeHip, eNotActive);
    }

    // Invoke Base Class Disable Method
//    Trigger::DisableTrigger();

}   // end DisableTrigger()
E_Bool Hip2AlarmTrigger::CheckCondition (void)
{
    static const USHORT ABOVE15_PRESS_TIME = 15;
    static LONG hip2Count = 0;

//    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();
    LONG ExtremeHighInsp_Offset =  mgrBdSettingPtrGetCurrentValue(eExtremeHighInsp_Autoset);

    if (InactiveTimer > 0)
    {
        InactiveTimer--;
        return (eFalse);
    }

    HighPressAlarm = mgrBdSettingPtrGetCurrentValue(eHipLimit);

    LONG InhLastPressSensor = 10;//test
    LONG ExhLastPressSensor = 10;//test

    BDRequestDevice(eInhPressSensorGetLastReading);
    BDRequestDevice(eExhPressSensorGetLastReading);



    if ((InhLastPressSensor > HighPressAlarm + ExtremeHighInsp_Offset)||
        (ExhLastPressSensor
                > HighPressAlarm + ExtremeHighInsp_Offset))
    {
        hip2Count++;
        if(hip2Count >= ABOVE15_PRESS_TIME)
        {
            hip2Count = 0;
            // If the pressure condition is met, set the TriggerDidFire attribute
            // and update the alarm status.

            HighPressModeSetCauseOfAlarm(eExtremeHip);
            BDAlarmTaskSetIsActive(eExtremeHip, eActive);
            TriggerDidFire = eTrue;
        }
    }
    else
    {
        hip2Count = 0;
        TriggerDidFire = eFalse;

    }
    return (TriggerDidFire);

}   // end CheckCondition()
void Hip2AlarmTrigger::ProcessTrigger(Mode* mode)
{
    // Invoke the operation to handle the HIP trigger in the current mode.
    modeProcessTriggerID(eHip2AlarmTrigger);
}   // end ProcessTrigger()
LONG Hip2AlarmTrigger::GetHighPressAlarm (void)
{
    return HighPressAlarm;
}
void Hip2AlarmTrigger::SetInactiveTimer (USHORT value)
{
    InactiveTimer = value;
}

class Hip2AlarmTriggerUnitMock : public BDTestFixture
{
public:
    Hip2AlarmTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~Hip2AlarmTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyHip2AlarmTrigger = new Hip2AlarmTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyHip2AlarmTrigger;
    }
    Hip2AlarmTrigger *MyHip2AlarmTrigger;
};

//test for Hip2AlarmTrigger::Hip2AlarmTrigger()
TEST_F(Hip2AlarmTriggerUnitMock, Hip2AlarmTrigger)
{
    EXPECT_EQ(MyHip2AlarmTrigger->HighPressAlarm, 20 * SCALE);
    EXPECT_EQ(MyHip2AlarmTrigger->InactiveTimer, 0);
}

//test for Hip2AlarmTrigger* Hip2AlarmTrigger::S_GetInstance (void)
TEST_F(Hip2AlarmTriggerUnitMock, S_GetInstance)
{
    MyHip2AlarmTrigger->S_GetInstance();

    EXPECT_NE(MyHip2AlarmTrigger->S_Instance, (void*)0);
}

//test for void Hip2AlarmTrigger::DisableTrigger(void)
TEST_F(Hip2AlarmTriggerUnitMock, DisableTrigger)
{
    MyHip2AlarmTrigger->IsEnabled = eTrue;
    MyHip2AlarmTrigger->TriggerDidFire = eFalse;

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eExtremeHip, eNotActive)).Times(1);

    MyHip2AlarmTrigger->DisableTrigger();
}

//test for E_Bool Hip2AlarmTrigger::CheckCondition (void)
TEST_F(Hip2AlarmTriggerUnitMock, CheckCondition_0)
{
    MyHip2AlarmTrigger->InactiveTimer = 5;

    EXPECT_EQ(MyHip2AlarmTrigger->CheckCondition(), eFalse);
    EXPECT_EQ(MyHip2AlarmTrigger->InactiveTimer, 4);
}
TEST_F(Hip2AlarmTriggerUnitMock, CheckCondition_1)
{
    MyHip2AlarmTrigger->InactiveTimer = 0;
    const LONG ABOVE15_PRESS_TIME = 15;

    for(int i = 0; i < ABOVE15_PRESS_TIME; ++i)
    {
        EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eExtremeHighInsp_Autoset)).Times(1).WillOnce(Return(1));
        EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eHipLimit)).Times(1).WillOnce(Return(1));

        EXPECT_CALL(*_StepMotor, BDRequestDevice(eInhPressSensorGetLastReading)).Times(1);
        EXPECT_CALL(*_StepMotor, BDRequestDevice(eExhPressSensorGetLastReading)).Times(1);

        MyHip2AlarmTrigger->CheckCondition();
    }
    EXPECT_EQ(MyHip2AlarmTrigger->TriggerDidFire, eTrue);
}

//test for void Hip2AlarmTrigger::ProcessTrigger(Mode* mode)
TEST_F(Hip2AlarmTriggerUnitMock, ProcessTrigger)
{
    EXPECT_CALL(*_ModeMgr, modeProcessTriggerID(eHip2AlarmTrigger));

    MyHip2AlarmTrigger->ProcessTrigger((Mode*)0);
}

//test for LONG Hip2AlarmTrigger::GetHighPressAlarm (void)
TEST_F(Hip2AlarmTriggerUnitMock, GetHighPressAlarm)
{
    MyHip2AlarmTrigger->HighPressAlarm = 3;

    EXPECT_EQ(MyHip2AlarmTrigger->GetHighPressAlarm(), 3);
}

//test for void Hip2AlarmTrigger::SetInactiveTimer (USHORT value)
TEST_F(Hip2AlarmTriggerUnitMock, SetInactiveTimer)
{
    MyHip2AlarmTrigger->SetInactiveTimer(10);

    EXPECT_EQ(MyHip2AlarmTrigger->InactiveTimer, 10);
}

#endif //end define BD_TRIGGER_UNIT_TEST
