#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "DisconnectionAlarmTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "BreathMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "StepMotorMockHeader.h"

DisconnectionAlarmTrigger* DisconnectionAlarmTrigger::S_Instance = NULL;
DisconnectionAlarmTrigger* DisconnectionAlarmTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new DisconnectionAlarmTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end DisconnectionAlarmTrigger()
DisconnectionAlarmTrigger::DisconnectionAlarmTrigger(void)
: Trigger(eDisconnectionAlarmTrigger)
{
    InactiveTimer = 0;
    LowInhPressLimit = 0;
}   // end DisconnectionAlarmTrigger()
E_Bool DisconnectionAlarmTrigger::CheckCondition(void)
{

    if(InactiveTimer > 0)
    {
        InactiveTimer--;
        return (eFalse);
    }

    if(InhPhaseS_IsInUnrestrictedPhase(100) == eTrue)
    {
        //Check Inh Volume > 100*SCALE ml
        if(BDMgrBreathDataGetPdBreathDataPtrGetInhVolume() > 10000)
        {
            if((MgrBreathDataGetMonitoredPressure() < LowInhPressLimit)&&
                    (PressureSensorGetLastReading() < 2000)) //20 cmH2O
            {
                TriggerDidFire = eTrue;
            }
        }
    }

    return (TriggerDidFire);
}   // end CheckCondition()
void DisconnectionAlarmTrigger::PhaseInSettingChange(LONG value)
{
    InactiveTimer = 50;
    LowInhPressLimit = value/3;

    if(LowInhPressLimit < 100)
    {
        LowInhPressLimit = 100;
    }

    HipAlarmTriggerSetInactiveTimer(100);

}// end PhaseInSettingChange()

class DisconnectionAlarmTriggerUnitMock : public BDTestFixture
{
public:
    DisconnectionAlarmTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~DisconnectionAlarmTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyDisconnectionAlarmTrigger = new DisconnectionAlarmTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyDisconnectionAlarmTrigger;
    }
    DisconnectionAlarmTrigger *MyDisconnectionAlarmTrigger;
};

//test for DisconnectionAlarmTrigger* DisconnectionAlarmTrigger::S_GetInstance(void)
TEST_F(DisconnectionAlarmTriggerUnitMock, S_GetInstance)
{
    MyDisconnectionAlarmTrigger->S_GetInstance();

    EXPECT_NE(MyDisconnectionAlarmTrigger->S_Instance, (void*)0);
}

//test for DisconnectionAlarmTrigger::DisconnectionAlarmTrigger(void)
TEST_F(DisconnectionAlarmTriggerUnitMock, DisconnectionAlarmTrigger)
{
    EXPECT_EQ(MyDisconnectionAlarmTrigger->InactiveTimer, 0);
    EXPECT_EQ(MyDisconnectionAlarmTrigger->LowInhPressLimit, 0);
}

//test for E_Bool DisconnectionAlarmTrigger::CheckCondition(void)
TEST_F(DisconnectionAlarmTriggerUnitMock, DisconnectionAlarmTrigger_0)
{
    MyDisconnectionAlarmTrigger->InactiveTimer = 5;

    EXPECT_EQ(MyDisconnectionAlarmTrigger->CheckCondition(), eFalse);
    EXPECT_EQ(MyDisconnectionAlarmTrigger->InactiveTimer, 4);
}
TEST_F(DisconnectionAlarmTriggerUnitMock, DisconnectionAlarmTrigger_1)
{
    MyDisconnectionAlarmTrigger->InactiveTimer = 0;
    MyDisconnectionAlarmTrigger->LowInhPressLimit = 100;

    EXPECT_CALL(*_PhaseMgrPtr, InhPhaseS_IsInUnrestrictedPhase(100)).Times(1).WillOnce(Return(eTrue));
    EXPECT_CALL(*_BreathMgr, BDMgrBreathDataGetPdBreathDataPtrGetInhVolume()).Times(1).WillOnce(Return(20000));

    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(10));
    EXPECT_CALL(*_StepMotor, PressureSensorGetLastReading()).Times(1).WillOnce(Return( 1000));

    EXPECT_EQ(MyDisconnectionAlarmTrigger->CheckCondition(), eTrue);
}

//test for void DisconnectionAlarmTrigger::PhaseInSettingChange(LONG value)
TEST_F(DisconnectionAlarmTriggerUnitMock, PhaseInSettingChange_0)
{
    EXPECT_CALL(*_TriggerMgr, HipAlarmTriggerSetInactiveTimer(100)).Times(1);

    MyDisconnectionAlarmTrigger->PhaseInSettingChange(3);

    EXPECT_EQ(MyDisconnectionAlarmTrigger->LowInhPressLimit, 100);
}
TEST_F(DisconnectionAlarmTriggerUnitMock, PhaseInSettingChange_1)
{
    EXPECT_CALL(*_TriggerMgr, HipAlarmTriggerSetInactiveTimer(100)).Times(1);

    MyDisconnectionAlarmTrigger->PhaseInSettingChange(600);

    EXPECT_EQ(MyDisconnectionAlarmTrigger->LowInhPressLimit, 200);
}

#endif //end define BD_TRIGGER_UNIT_TEST
