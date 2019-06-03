#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "AlarmResetOpReqTrigger.h"
#include "DebugTrace.h"

#include "PhaseMgrMockHeader.h"
#include "AlarmMockHeader.h"
#include "ModeMgrMockHeader.h"

using namespace ::testing;

// define statics
AlarmResetOpReqTrigger* AlarmResetOpReqTrigger::S_Instance = NULL;

AlarmResetOpReqTrigger* AlarmResetOpReqTrigger::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new AlarmResetOpReqTrigger ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()
void AlarmResetOpReqTrigger::EnableTrigger (void)
{
    IsArmed = eFalse;
//    Trigger::EnableTrigger ();

}   // end EnableTrigger()
E_Bool AlarmResetOpReqTrigger::GetIsArmed(void)
{
    return IsArmed;
}
E_Bool AlarmResetOpReqTrigger::CheckCondition (void)
{
    // If an alarm reset request has been received (indicated by IsArmed) and
    // if the ventilator is in the exhalation phase, set TriggerDidFire to True.
    if((eTrue == IsArmed)
            && (eExhPhase == BDPhaseMgrGetCurrentPhaseId ()))
    {
        TriggerDidFire = eTrue;
    }

    return (TriggerDidFire);

}   // end CheckCondition()

void AlarmResetOpReqTrigger::ProcessRequest (E_OpRequestCommand command)
{

    // Check if the request is to start a manual inhalation
    if(eStart == command)
    {
        // Set IsArmed to True to indicate that the operator has requested that
        // alarms be reset.
        if (IsIgnored == eTrue)
        {
            IsIgnored = eFalse;
        }
        else
        {
            BDAlarmTaskSetResetFlag(eTrue);

        }

        IsArmed = eTrue;

        // The ModeMgr may need to updates its mode list based upon the
        // manual reset.
        ModeMgrProcessManualReset ();

        // Provide audible feedback to the operator.  Alarm Reset requests are
        // always accepted.
        // AcceptRequest();
    }
    else
    {
        // An alarm reset can only be requested to start.
        ASSERTION(eStart == command);
    }

}   // end ProcessRequest()
AlarmResetOpReqTrigger::AlarmResetOpReqTrigger (void) :
        Trigger (eAlarmResetOpReqTrigger), OpRequest (), IsArmed (eFalse)
{

}   // end AlarmResetOpReqTrigger()
void AlarmResetOpReqTrigger::SetIsIgnoreAlarmReset(E_Bool state)
{
    IsIgnored = state;
}

class AlarmResetOpReqTriggerUnitMock : public BDTestFixture
{
public:
    AlarmResetOpReqTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~AlarmResetOpReqTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyAlarmResetOpReqTrigger = new AlarmResetOpReqTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyAlarmResetOpReqTrigger;
    }
    AlarmResetOpReqTrigger *MyAlarmResetOpReqTrigger;
};

//test for  AlarmResetOpReqTrigger* AlarmResetOpReqTrigger::S_GetInstance (void)
TEST_F(AlarmResetOpReqTriggerUnitMock, S_GetInstance)
{
    MyAlarmResetOpReqTrigger->S_GetInstance();

    EXPECT_NE(MyAlarmResetOpReqTrigger->S_Instance, (void*)0);
}

//test for void AlarmResetOpReqTrigger::EnableTrigger (void)
TEST_F(AlarmResetOpReqTriggerUnitMock, EnableTrigger)
{
    MyAlarmResetOpReqTrigger->EnableTrigger();

    EXPECT_EQ(MyAlarmResetOpReqTrigger->IsArmed, eFalse);
}

//test for E_Bool AlarmResetOpReqTrigger::GetIsArmed(void)
TEST_F(AlarmResetOpReqTriggerUnitMock, GetIsArmed)
{
    MyAlarmResetOpReqTrigger->IsArmed = eTrue;

    EXPECT_EQ(MyAlarmResetOpReqTrigger->GetIsArmed(), eTrue);
}

//test for E_Bool AlarmResetOpReqTrigger::CheckCondition (void)
TEST_F(AlarmResetOpReqTriggerUnitMock, CheckCondition)
{
    MyAlarmResetOpReqTrigger->IsArmed = eTrue;
    EXPECT_CALL(*_PhaseMgrPtr, BDPhaseMgrGetCurrentPhaseId()).Times(1).WillOnce(Return(eExhPhase));

    EXPECT_EQ(MyAlarmResetOpReqTrigger->CheckCondition(), eTrue);
}

//test for void AlarmResetOpReqTrigger::ProcessRequest (E_OpRequestCommand command)
TEST_F(AlarmResetOpReqTriggerUnitMock, ProcessRequest_0)
{
    MyAlarmResetOpReqTrigger->IsIgnored = eTrue;
    EXPECT_CALL(*_ModeMgr, ModeMgrProcessManualReset ()).Times(1);

    MyAlarmResetOpReqTrigger->ProcessRequest(eStart);

    EXPECT_EQ(MyAlarmResetOpReqTrigger->IsArmed, eTrue);
    EXPECT_EQ(MyAlarmResetOpReqTrigger->IsIgnored, eFalse);
}
TEST_F(AlarmResetOpReqTriggerUnitMock, ProcessRequest_1)
{
    MyAlarmResetOpReqTrigger->IsIgnored = eFalse;
    EXPECT_CALL(*_ModeMgr, ModeMgrProcessManualReset ()).Times(1);
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetResetFlag(eTrue)).Times(1);

    MyAlarmResetOpReqTrigger->ProcessRequest(eStart);

    EXPECT_EQ(MyAlarmResetOpReqTrigger->IsArmed, eTrue);
}

//test for AlarmResetOpReqTrigger::AlarmResetOpReqTrigger (void) :
TEST_F(AlarmResetOpReqTriggerUnitMock, AlarmResetOpReqTrigger)
{
    //no source, not need to test
}

//test for  void AlarmResetOpReqTrigger::SetIsIgnoreAlarmReset(E_Bool state)
TEST_F(AlarmResetOpReqTriggerUnitMock, SetIsIgnoreAlarmReset)
{
    MyAlarmResetOpReqTrigger->SetIsIgnoreAlarmReset(eTrue);

    EXPECT_EQ(MyAlarmResetOpReqTrigger->IsIgnored, eTrue);
}
#endif //end define BD_TRIGGER_UNIT_TEST
