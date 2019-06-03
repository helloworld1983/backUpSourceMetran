#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "ExtremeHigh_PHigh_Trigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "AlarmMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"

ExtremeHigh_PHigh_Trigger* ExtremeHigh_PHigh_Trigger::S_Instance = NULL;

ExtremeHigh_PHigh_Trigger* ExtremeHigh_PHigh_Trigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new ExtremeHigh_PHigh_Trigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}
ExtremeHigh_PHigh_Trigger::ExtremeHigh_PHigh_Trigger(void)
: Trigger(eExtremeHigh_PHigh_Trigger)
{
    HighPressAlarm = 0;
    InactiveTimer = 0;

    ExtremeHighPEEP_Offset = 0;
}
void ExtremeHigh_PHigh_Trigger::EnableTrigger(void)
{
    ExtremeHighPEEP_Offset =  BDMgrBdSettingGetCurrentValue(eExtremeHighPEEP_Autoset);
    Trigger::EnableTrigger();
}
E_Bool ExtremeHigh_PHigh_Trigger::CheckCondition(void)
{

    LONG PressureLimit = BDMgrBdSettingGetCurrentValue(eAPRVHighPHighLimit) + ExtremeHighPEEP_Offset;

    LONG InhPressSensor = 3;
    LONG ExhPressSensor = 3;

    BDRequestDevice(eInhPressSensorGetLastReading);
    BDRequestDevice(eExhPressSensorGetLastReading);


    if ((InhPressSensor > PressureLimit)||
            (ExhPressSensor > PressureLimit))

    {

        if(eNotActive == BDAlarmTaskGetIsActive(eExtremeHigh_PHigh))
        {
            BDAlarmTaskSetIsActive(eExtremeHigh_PHigh, eActive);
        }

        TriggerDidFire = eTrue;
    }
    else
    {
        if(eActive == BDAlarmTaskGetIsActive(eExtremeHigh_PHigh))
        {
            BDAlarmTaskSetIsActive(eExtremeHigh_PHigh, eNotActive);
        }
    }
    return (TriggerDidFire);

}   // end CheckCondition()

void ExtremeHigh_PHigh_Trigger::SetInactiveTimer(USHORT value)
{
    InactiveTimer = value;
}

class ExtremeHigh_PHigh_TriggerUnitMock : public BDTestFixture
{
public:
    ExtremeHigh_PHigh_TriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~ExtremeHigh_PHigh_TriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyExtremeHigh_PHigh_Trigger = new ExtremeHigh_PHigh_Trigger();
    }
    virtual void TearDown(void)
    {
        delete MyExtremeHigh_PHigh_Trigger;
    }
    ExtremeHigh_PHigh_Trigger *MyExtremeHigh_PHigh_Trigger;
};

//test for ExtremeHigh_PHigh_Trigger* ExtremeHigh_PHigh_Trigger::S_GetInstance(void)
TEST_F(ExtremeHigh_PHigh_TriggerUnitMock, S_GetInstance)
{
    MyExtremeHigh_PHigh_Trigger->S_GetInstance();

    EXPECT_NE(MyExtremeHigh_PHigh_Trigger->S_Instance, (void*)0);
}

//test for ExtremeHigh_PHigh_Trigger::ExtremeHigh_PHigh_Trigger(void)
TEST_F(ExtremeHigh_PHigh_TriggerUnitMock, ExtremeHigh_PHigh_Trigger)
{
    EXPECT_EQ(MyExtremeHigh_PHigh_Trigger->HighPressAlarm, 0);
    EXPECT_EQ(MyExtremeHigh_PHigh_Trigger->InactiveTimer, 0);
    EXPECT_EQ(MyExtremeHigh_PHigh_Trigger->ExtremeHighPEEP_Offset, 0);
}

//test for void ExtremeHigh_PHigh_Trigger::EnableTrigger(void)
TEST_F(ExtremeHigh_PHigh_TriggerUnitMock, EnableTrigger)
{
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eExtremeHighPEEP_Autoset)).Times(1);

    MyExtremeHigh_PHigh_Trigger->EnableTrigger();
}

//test for E_Bool ExtremeHigh_PHigh_Trigger::CheckCondition(void)
TEST_F(ExtremeHigh_PHigh_TriggerUnitMock, CheckCondition_0)
{
    MyExtremeHigh_PHigh_Trigger->ExtremeHighPEEP_Offset = 1;

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eAPRVHighPHighLimit)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_Alarm, BDAlarmTaskGetIsActive(eExtremeHigh_PHigh)).Times(1).WillOnce(Return(eNotActive));


    EXPECT_EQ(MyExtremeHigh_PHigh_Trigger->CheckCondition(), eTrue);
}
TEST_F(ExtremeHigh_PHigh_TriggerUnitMock, CheckCondition_1)
{
    MyExtremeHigh_PHigh_Trigger->ExtremeHighPEEP_Offset = 1;
    MyExtremeHigh_PHigh_Trigger->TriggerDidFire = eTrue;

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eAPRVHighPHighLimit)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_Alarm, BDAlarmTaskGetIsActive(eExtremeHigh_PHigh)).Times(1).WillOnce(Return(eActive));


    EXPECT_EQ(MyExtremeHigh_PHigh_Trigger->CheckCondition(), eTrue);
}

//test for void ExtremeHigh_PHigh_Trigger::SetInactiveTimer(USHORT value)
TEST_F(ExtremeHigh_PHigh_TriggerUnitMock, SetInactiveTimer)
{
    MyExtremeHigh_PHigh_Trigger->SetInactiveTimer(98);

    EXPECT_EQ(MyExtremeHigh_PHigh_Trigger->InactiveTimer, 98);
}

#endif //end define BD_TRIGGER_UNIT_TEST
