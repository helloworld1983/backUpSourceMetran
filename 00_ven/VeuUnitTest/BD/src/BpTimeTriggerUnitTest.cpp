#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "BpTimeTrigger.h"
#include "DebugTrace.h"

#include "PhaseMgrMockHeader.h"

using namespace ::testing;

BpTimeTrigger::BpTimeTrigger (E_TriggerId id, CHAR* timerName) :
        CyclicTimeTrigger (id, timerName), IsChangePending (eFalse)
{

}   // end BpTimeTrigger()
void BpTimeTrigger::EnableTrigger (void)
{
    // Retrieve the currently set breath period
//    UNSIGNED timePeriod = GetTimePeriod ();

    // Reset the timer for the Breath Period, which is stored in TimePeriod.
    // Using TimePeriod as the second argument indicates that the timer is
    // continuous.  Passing TRUE as the last argument indicates that the timer
    // should be enabled now.   Perform error handling if the reset is not
    // successful.
//    if (Reset(timePeriod, timePeriod, TRUE) != NU_SUCCESS)
//    {
//        ASSERTION(eFalse);
//    }
    Reset();
    // Invoke base class enable method which updates the IsEnabled
    // and TriggerDidFire flags
//    Trigger::EnableTrigger ();

    // If a change was pending, it just got phased in
    IsChangePending = eFalse;

}   // end EnableTrigger()
void BpTimeTrigger::EnableTrigger (UNSIGNED time)
{
    // Reset the timer using the passed in value for time as the initial
    // period and the Breath Period for every duration after that.  The
    // Breath Period is stored in TimePeriod.  Passing TRUE as the last
    // argument indicates that the timer should be enabled now.
    // Perform error handling if the reset is not successful.
//    if (Reset(time, GetTimePeriod(), TRUE) != NU_SUCCESS)
//    {
//        ASSERTION(eFalse);
//    }
    Reset();

    // Invoke base class enable method which updates the IsEnabled
    // and TriggerDidFire flags
    Trigger::EnableTrigger ();

    // If a change was pending, it just got phased in
    IsChangePending = eFalse;

}   // end EnableTrigger()
void BpTimeTrigger::ReEnableTrigger (void)
{
    // Check IsEnabled here; only want to re-enable if currently enabled
    if(eTrue == IsEnabled)
    {
        // Disable the timer
        Stop();

        // Re-enable the trigger
        EnableTrigger ();
    }

}   // end ReEnableTrigger()
E_Bool BpTimeTrigger::CheckCondition (void)
{
//    static E_Bool isDelayed = eFalse;
    E_Bool rtnValue = TriggerDidFire;

    // If the trigger fired, reset it.
    if(eTrue == rtnValue)
    {
//        if (PExhAzTestPtr->GetAzTestState() != eAzCompleted)
//        {
//            // If an auto-zero is in progress, delay the BP trigger.  This case
//            // is only possible for high rates and inverse I:E ratios.
//            rtnValue = eFalse;
//
//            // Set a flag indicating that the BP timer was delayed.
//            isDelayed = eTrue;
//        }
//
//        else
//        {
//            TriggerDidFire = eFalse;
//
//            // If a change is pending, re-enable the trigger so that the new
//            // breath period is phased in.  If the previous trigger was delayed,
//            // re-enable the trigger so that BP timer is synched up with the
//            // start of the inhalation.
//            if ((eTrue == IsChangePending) || (eTrue == isDelayed))
//            {
//                ReEnableTrigger();
//                isDelayed = eFalse;
//            }
//        }

    }
    else
    {
        // If the trigger hasn't already fired and a respiratory rate change is
        // pending, phase in the change.  The trigger may fire as a result of
        // the new rate being phased in.
        if(eTrue == IsChangePending)
        {
            PhaseIn ();
        }
    }

    return (rtnValue);

}   // end CheckCondition()
void BpTimeTrigger::PhaseInSettingChange (LONG newValue)
{
    UNSIGNED breathPeriod;      // new breath period calculated from new RR

    // Always update the Breath period; will only phase in the change if the
    // trigger is currently active.  The new RR passed to this method is
    // scaled.
    breathPeriod = (TICKS_PER_MIN * SCALE) / newValue;

    // Store the breath period in the TimePeriod attribute
    SetTimePeriod (breathPeriod);

    // Only need to worry about phasing in the time change if the trigger is
    // enabled and a change is not already pending
    if((eTrue == IsEnabled) && (eFalse == IsChangePending))
    {
        PhaseIn ();
    }

}   // end PhaseInSetting()
void BpTimeTrigger::PhaseIn (void)
{
    UNSIGNED bpElapsedTime;     // time elapsed since the start of the last inh

    // See if the ventilator is in the unrestricted phase of exhalation
    // By not passing any parameters, the default minimum exhalation time
    // is used.   The ventilator waits until the minimum exhalation time has
    // elapsed since the start of exhalation before phasing in a rate change.
    if(ExhPhaseS_IsInUnrestrictedPhase () == eTrue)
    {

        // Disable the Timer
        Stop();

        // Determine the amount of the total breath period time that
        // has already elapsed.
        bpElapsedTime = InhPhaseS_CalcTimeSincePhaseStart ();

        // If the new breath period has already elapsed, the trigger
        // fires right away; otherwise, the timer is re-enabled
        // accounting for the new breath period
        if(3 <= bpElapsedTime)
        {
            // Since this is a continuous timer, enable it right away
            EnableTrigger ();

            // Set the TriggerDidFire flag since the trigger has already
            // fired.   This must be done after the call to EnableTrigger()
            // since EnableTrigger() resets the flag.
            TriggerDidFire = eTrue;

        }

        else
        {
            // Enable the trigger for the amount of time remaining
            EnableTrigger (GetTimePeriod () - bpElapsedTime);
        }

        // If a change was pending, it just got phased in; reset the flag.
        IsChangePending = eFalse;

    }   // in unrestricted phase

    else
    {

        // Phase it in later; can't do it now
        IsChangePending = eTrue;
    }

}   // end PhaseIn()
E_Bool BpTimeTrigger::PreCheckCondition (void)
{

    E_Bool rtnValue = eFalse;

//    if ((eTrue == IsEnabled) &&
//        (eTrue == TriggerDidFire) &&
//        (PExhAzTestPtr->GetAzTestState() == eAzCompleted))

//    {
//        rtnValue = eTrue;
//    }
//    else
//    {
//        rtnValue = eFalse;
//    }

    return (rtnValue);

}   // end PreCheckCondition()

class BpTimeTriggerUnitMock : public BDTestFixture
{
public:
    BpTimeTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~BpTimeTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyBpTimeTrigger = new BpTimeTrigger(eFirstIMVTrigger, (CHAR*)"test");
    }
    virtual void TearDown(void)
    {
        delete MyBpTimeTrigger;
    }
    BpTimeTrigger *MyBpTimeTrigger;
};

//test for BpTimeTrigger::BpTimeTrigger (E_TriggerId id, CHAR* timerName)
TEST_F(BpTimeTriggerUnitMock, BpTimeTrigger)
{
    //no source, not need to test
}

//test for void BpTimeTrigger::EnableTrigger (void)
TEST_F(BpTimeTriggerUnitMock, EnableTrigger)
{
    MyBpTimeTrigger->EnableTrigger();
    MyBpTimeTrigger->IsChangePending = eFalse;
}

//test for void BpTimeTrigger::EnableTrigger (UNSIGNED time)
TEST_F(BpTimeTriggerUnitMock, EnableTrigger_With)
{
    MyBpTimeTrigger->EnableTrigger(1);
    MyBpTimeTrigger->IsChangePending = eFalse;
}

//test for void BpTimeTrigger::ReEnableTrigger (void)
TEST_F(BpTimeTriggerUnitMock, ReEnableTrigger)
{
    //all calling function is tested, not need to test
}

//test for E_Bool BpTimeTrigger::CheckCondition (void)
TEST_F(BpTimeTriggerUnitMock, CheckCondition)
{
    //all calling function is tested, not need to test
}

//test for void BpTimeTrigger::PhaseInSettingChange (LONG newValue)
TEST_F(BpTimeTriggerUnitMock, PhaseInSettingChange)
{
    //all calling function is tested, not need to test
}

//test for E_Bool BpTimeTrigger::PreCheckCondition (void)
TEST_F(BpTimeTriggerUnitMock, PreCheckCondition)
{
    EXPECT_EQ(MyBpTimeTrigger->PreCheckCondition(), eFalse);
}

//test for void BpTimeTrigger::PhaseIn (void)
TEST_F(BpTimeTriggerUnitMock, PhaseIn_0)
{
    EXPECT_CALL(*_PhaseMgrPtr, ExhPhaseS_IsInUnrestrictedPhase()).Times(1).WillOnce(Return(eFalse));

    MyBpTimeTrigger->PhaseIn();

    EXPECT_EQ(MyBpTimeTrigger->IsChangePending, eTrue);
}
TEST_F(BpTimeTriggerUnitMock, PhaseIn_1)
{
    EXPECT_CALL(*_PhaseMgrPtr, ExhPhaseS_IsInUnrestrictedPhase()).Times(1).WillOnce(Return(eTrue));
    EXPECT_CALL(*_PhaseMgrPtr, InhPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(5));

    MyBpTimeTrigger->PhaseIn();

    EXPECT_EQ(MyBpTimeTrigger->IsChangePending, eFalse);
    EXPECT_EQ(MyBpTimeTrigger->TriggerDidFire, eTrue);
}

#endif //end define BD_TRIGGER_UNIT_TEST
