#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "ApneaAutoResetTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "AlarmMockHeader.h"
#include "TriggerMgrMockHeader.h"

ApneaAutoResetTrigger* ApneaAutoResetTrigger::S_Instance = NULL;

ApneaAutoResetTrigger* ApneaAutoResetTrigger::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new ApneaAutoResetTrigger ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end ApneaAutoResetTrigger()
void ApneaAutoResetTrigger::EnableTrigger (void)
{
    // Set the State to eWaiting1, waiting for the 1st PIM
    State = eWaiting1;
//    Trigger::EnableTrigger ();

}   // end EnableTrigger()
E_Bool ApneaAutoResetTrigger::CheckCondition (void)
{
    // If the trigger is armed, meaning that the auto-reset criteria have
    // been detected, and the ventilator is in the unrestricted phase of
    // exhalation, set the TriggerDidFire attribute to True.
    if((eArmed == State) && (eTrue == ExhPhaseS_IsInUnrestrictedPhase ()))
    {
        TriggerDidFire = eTrue;
    }

    // If not updated above, this value is still False.  Return value to caller.
    return (TriggerDidFire);

}   // end CheckCondition()
void ApneaAutoResetTrigger::UpdateState (E_TriggerId id)
{
    // If a VIM was trigger, go back to the Waiting 1 State
    if(eAvBpTimeTrigger == id)
    {
        State = eWaiting1;

    }
    // Check if the inhalation was patient triggered.
    else if((ePtInhPressTrigger == id) || (ePtInhFlowTrigger == id))
    {
        // If the trigger is in the Waiting1 state, update the state to
        // Waiting2, waiting for the 2nd PIM
        if(eWaiting1 == State)
        {
            State = eWaiting2;
        }

        // If the trigger is already in the Waiting2 state, the reset criteria
        // have been met.
        else if(eWaiting2 == State)
        {
            // Set the state to armed.  Even though the auto-reset criteria
            // have been met, the ventilator does not transition out of AV
            // until the patient initiated inhalation is delivered and a
            // minimum exhalation time has elapsed.
            State = eArmed;

            // Update the Apnea Alarm status.  Even though the ventilator
            // does not officially transition out of AV until the inhalation
            // is complete, the alarm indications are updated once the
            // reset is detected.
            BDAlarmTaskSetIsActive (eApnea, eNotActive);

            // Tell ModeMgr about the auto reset
            modeMgrPtrProcessAutoReset ();

            // Restart apnea interval timing since the inhalation is starting
            // now.
            ApneaAlarmTimeTriggerEnableTrigger ();
        }
        else
        {
            // Invalid state
            ASSERTION(eFalse);
        }
    }
    else
    {
        // Invalid trigger ID
        ASSERTION(eFalse);
    }

}   // end UpdateState()

ApneaAutoResetTrigger::ApneaAutoResetTrigger (void) :
        Trigger (eApneaAutoResetTrigger), State (eIdle)
{

}   // end ApneaAutoResetTrigger()

class ApneaAutoResetTriggerUnitMock : public BDTestFixture
{
public:
    ApneaAutoResetTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~ApneaAutoResetTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyApneaAutoResetTrigger = new ApneaAutoResetTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyApneaAutoResetTrigger;
    }
    ApneaAutoResetTrigger *MyApneaAutoResetTrigger;
};

//test for ApneaAutoResetTrigger* ApneaAutoResetTrigger::S_GetInstance (void)
TEST_F(ApneaAutoResetTriggerUnitMock, S_GetInstance)
{
    MyApneaAutoResetTrigger->S_GetInstance();

    EXPECT_NE(MyApneaAutoResetTrigger->S_Instance, (void*)0);
}

//test for void ApneaAutoResetTrigger::EnableTrigger (void)
TEST_F(ApneaAutoResetTriggerUnitMock, EnableTrigger)
{
    MyApneaAutoResetTrigger->EnableTrigger();

    EXPECT_EQ(MyApneaAutoResetTrigger->State, eWaiting1);
}

//test for E_Bool ApneaAutoResetTrigger::CheckCondition (void)
TEST_F(ApneaAutoResetTriggerUnitMock, CheckCondition)
{
    MyApneaAutoResetTrigger->State = eArmed;
    EXPECT_CALL(*_PhaseMgrPtr, ExhPhaseS_IsInUnrestrictedPhase()).Times(1).WillOnce(Return(eTrue));

    EXPECT_EQ(MyApneaAutoResetTrigger->CheckCondition(), eTrue);
}

//test for void ApneaAutoResetTrigger::UpdateState (E_TriggerId id)
TEST_F(ApneaAutoResetTriggerUnitMock, UpdateState_0)
{
    MyApneaAutoResetTrigger->UpdateState(eAvBpTimeTrigger);

    EXPECT_EQ(MyApneaAutoResetTrigger->State, eWaiting1);
}
TEST_F(ApneaAutoResetTriggerUnitMock, UpdateState_1)
{
    MyApneaAutoResetTrigger->State = eWaiting1;

    MyApneaAutoResetTrigger->UpdateState(ePtInhPressTrigger);

    EXPECT_EQ(MyApneaAutoResetTrigger->State, eWaiting2);
}
TEST_F(ApneaAutoResetTriggerUnitMock, UpdateState_2)
{
    MyApneaAutoResetTrigger->State = eWaiting2;

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eApnea, eNotActive)).Times(1);
    EXPECT_CALL(*_ModeMgr, modeMgrPtrProcessAutoReset ()).Times(1);
    EXPECT_CALL(*_TriggerMgr, ApneaAlarmTimeTriggerEnableTrigger ()).Times(1);

    MyApneaAutoResetTrigger->UpdateState(ePtInhPressTrigger);

    EXPECT_EQ(MyApneaAutoResetTrigger->State, eArmed);
}
TEST_F(ApneaAutoResetTriggerUnitMock, UpdateState_3)
{
    MyApneaAutoResetTrigger->State = eWaiting1;

    MyApneaAutoResetTrigger->UpdateState(ePtInhFlowTrigger);

    EXPECT_EQ(MyApneaAutoResetTrigger->State, eWaiting2);
}
TEST_F(ApneaAutoResetTriggerUnitMock, UpdateState_4)
{
    MyApneaAutoResetTrigger->State = eWaiting2;

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eApnea, eNotActive)).Times(1);
    EXPECT_CALL(*_ModeMgr, modeMgrPtrProcessAutoReset ()).Times(1);
    EXPECT_CALL(*_TriggerMgr, ApneaAlarmTimeTriggerEnableTrigger ()).Times(1);

    MyApneaAutoResetTrigger->UpdateState(ePtInhFlowTrigger);

    EXPECT_EQ(MyApneaAutoResetTrigger->State, eArmed);
}

//test for ApneaAutoResetTrigger::ApneaAutoResetTrigger (void) :
TEST_F(ApneaAutoResetTriggerUnitMock, ApneaAutoResetTrigger)
{
    //no source, not need to test
}

#endif //end define BD_TRIGGER_UNIT_TEST
