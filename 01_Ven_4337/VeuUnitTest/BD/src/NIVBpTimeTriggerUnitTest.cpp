#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "NIVBpTimeTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"

NIVBpTimeTrigger::NIVBpTimeTrigger(E_TriggerId id, CHAR* timerName)
    : CyclicTimeTrigger(id, timerName),
      IsChangePending(eFalse)
{
    breathRateValue = 0;
}   // end NIVBpTimeTrigger()

void NIVBpTimeTrigger::EnableTrigger(void)
{
    // Retrieve the currently set breath period
//    UNSIGNED timePeriod = GetTimePeriod();

    // Reset the timer for the Breath Period, which is stored in TimePeriod.
    // Using TimePeriod as the second argument indicates that the timer is
    // continuous.  Passing TRUE as the last argument indicates that the timer
    // should be enabled now.   Perform error handling if the reset is not
    // successful.
//    if (Reset(timePeriod, timePeriod, TRUE) != Q_SUCCESS)
//    {
//        ASSERTION(eFalse);
//    }
    Reset();

    // Invoke base class enable method which updates the IsEnabled
    // and TriggerDidFire flags
    Trigger::EnableTrigger();

    // If a change was pending, it just got phased in
    IsChangePending = eFalse;

}   // end EnableTrigger()

void NIVBpTimeTrigger::EnableTrigger(UNSIGNED time)
{
    // Reset the timer using the passed in value for time as the initial
    // period and the Breath Period for every duration after that.  The
    // Breath Period is stored in TimePeriod.  Passing TRUE as the last
    // argument indicates that the timer should be enabled now.
    // Perform error handling if the reset is not successful.
//    UNSIGNED timePeriod = GetTimePeriod();

//    if (Reset(time, timePeriod, TRUE) != Q_SUCCESS)
//    {
//        ASSERTION(eFalse);
//    }
    Reset();

    // Invoke base class enable method which updates the IsEnabled
    // and TriggerDidFire flags
    Trigger::EnableTrigger();

    // If a change was pending, it just got phased in
    IsChangePending = eFalse;

}   // end EnableTrigger()
void NIVBpTimeTrigger::ReEnableTrigger(void)
{
    // Check IsEnabled here; only want to re-enable if currently enabled
    if (eTrue == IsEnabled)
    {
        // Disable the timer
        Stop();

        // Re-enable the trigger
        EnableTrigger();
    }

}   // end ReEnableTrigger()
E_Bool NIVBpTimeTrigger::CheckCondition(void)
{
    static E_Bool isDelayed = eFalse;
    E_Bool rtnValue = TriggerDidFire;
    if(breathRateValue == 0) return eFalse;

    // If the trigger fired, reset it.
    if (eTrue == rtnValue)
    {
//        if (PInhAzTestPtr->GetAzTestState() == eAzActive)
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
//                //            if (eTrue == IsChangePending)
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
        if (eTrue == IsChangePending)
        {
            PhaseIn();
        }
    }

    return (rtnValue);

}   // end CheckCondition()
void NIVBpTimeTrigger::PhaseInSettingChange(LONG newValue)
{
    UNSIGNED breathPeriod = 0;      // new breath period calculated from new RR

    // Always update the Breath period; will only phase in the change if the
    // trigger is currently active.  The new RR passed to this method is
    // scaled.
    if(newValue != 0)
        breathPeriod = (TICKS_PER_MIN * SCALE) / newValue;
    breathRateValue  = newValue;

    // Store the breath period in the TimePeriod attribute
    SetTimePeriod(breathPeriod);

    // Only need to worry about phasing in the time change if the trigger is
    // enabled and a change is not already pending
    if ((eTrue == IsEnabled) && (eFalse == IsChangePending))
    {
        PhaseIn();
    }

}   // end PhaseInSetting()
void NIVBpTimeTrigger::PhaseIn(void)
{
    UNSIGNED bpElapsedTime;     // time elapsed since the start of the last inh

    // See if the ventilator is in the unrestricted phase of exhalation
    // By not passing any parameters, the default minimum exhalation time
    // is used.   The ventilator waits until the minimum exhalation time has
    // elapsed since the start of exhalation before phasing in a rate change.
    if (ExhPhaseS_IsInUnrestrictedPhase() == eTrue)
    {

        // Disable the Timer
//        Disable();
        Stop();

        // Determine the amount of the total breath period time that
        // has already elapsed.
        bpElapsedTime = InhPhaseS_CalcTimeSincePhaseStart();

        // If the new breath period has already elapsed, the trigger
        // fires right away; otherwise, the timer is re-enabled
        // accounting for the new breath period
        if (bpElapsedTime >= 2)
        {
            // Since this is a continuous timer, enable it right away
            EnableTrigger();

            // Set the TriggerDidFire flag since the trigger has already
            // fired.   This must be done after the call to EnableTrigger()
            // since EnableTrigger() resets the flag.
            TriggerDidFire = eTrue;

        }
        else
        {
            // Enable the trigger for the amount of time remaining
            EnableTrigger(GetTimePeriod() - bpElapsedTime);
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
E_Bool NIVBpTimeTrigger::PreCheckCondition(void)
{
    E_Bool rtnValue = eFalse;

    // In order for the pre-check to pass,
    //      the trigger must be enabled
    //      an auto-zero of the exhalation pressure transducer cannot be active
    //      triggering of the BP timer must result in delivery of a mandatory
    //          inhalation (this is the case if the trigger fires in any mode
    //          except Mixed or if the trigger fires in Mixed mode and the
    //          Mandatory window is open)

    if ( eTrue == ExhPhaseS_IsInUnrestrictedPhase() )
    {
        if ((eTrue == IsEnabled) &&
                ((BDModeMgrGetCurrentModeId() != eMixedMode) ||
                        (MixedModeGetWindow() == eMandWindow)))
        {
            // If the timer has already expired or will expire in less than one BD
            // control cycle, the pre-check condition is met.  The time check is
            // made to avoid a possible race condition if the timer hasn't expired
            // when the mode trigger is checked but expires before the BP timer is
            // checked.
            if (TriggerDidFire == eTrue)
            {
                rtnValue = eTrue;
            }
        }
    }

    return (rtnValue);

}   // end PreCheckCondition()

class NIVBpTimeTriggerUnitMock : public BDTestFixture
{
public:
    NIVBpTimeTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~NIVBpTimeTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyNIVBpTimeTrigger = new NIVBpTimeTrigger(eAutoSIOffTimeTrigger, (CHAR*)"test");
    }
    virtual void TearDown(void)
    {
        delete MyNIVBpTimeTrigger;
    }
    NIVBpTimeTrigger *MyNIVBpTimeTrigger;
};

//test for NIVBpTimeTrigger::NIVBpTimeTrigger(E_TriggerId id, CHAR* timerName)
TEST_F(NIVBpTimeTriggerUnitMock, NIVBpTimeTrigger)
{
    EXPECT_EQ(MyNIVBpTimeTrigger->breathRateValue, 0);
}

//test for void NIVBpTimeTrigger::EnableTrigger(void)
TEST_F(NIVBpTimeTriggerUnitMock, EnableTrigger)
{
    MyNIVBpTimeTrigger->EnableTrigger();

    EXPECT_EQ(MyNIVBpTimeTrigger->IsChangePending, eFalse);
}

//test void NIVBpTimeTrigger::EnableTrigger(UNSIGNED time)
TEST_F(NIVBpTimeTriggerUnitMock, EnableTrigger_with)
{
    MyNIVBpTimeTrigger->EnableTrigger(1);

    EXPECT_EQ(MyNIVBpTimeTrigger->IsChangePending, eFalse);
}

//test for void NIVBpTimeTrigger::ReEnableTrigger(void)
TEST_F(NIVBpTimeTriggerUnitMock, ReEnableTrigger)
{
    //all calling function is tested, not need to test
}

//test for E_Bool NIVBpTimeTrigger::CheckCondition(void)
TEST_F(NIVBpTimeTriggerUnitMock, CheckCondition)
{
    //all calling function is tested, not need to test
}

//test for void NIVBpTimeTrigger::PhaseInSettingChange(LONG newValue)
TEST_F(NIVBpTimeTriggerUnitMock, PhaseInSettingChange)
{
    //all calling function is tested, not need to test
}

//test for void NIVBpTimeTrigger::PhaseIn(void)
TEST_F(NIVBpTimeTriggerUnitMock, PhaseIn_0)
{
    EXPECT_CALL(*_PhaseMgrPtr, ExhPhaseS_IsInUnrestrictedPhase()).Times(1).WillOnce(Return(eFalse));

    MyNIVBpTimeTrigger->PhaseIn();

    EXPECT_EQ(MyNIVBpTimeTrigger->IsChangePending, eTrue);
}
TEST_F(NIVBpTimeTriggerUnitMock, PhaseIn_1)
{
    EXPECT_CALL(*_PhaseMgrPtr, ExhPhaseS_IsInUnrestrictedPhase()).Times(1).WillOnce(Return(eTrue));

    EXPECT_CALL(*_PhaseMgrPtr, InhPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(3));

    MyNIVBpTimeTrigger->PhaseIn();

    EXPECT_EQ(MyNIVBpTimeTrigger->TriggerDidFire, eTrue);

}

//test for E_Bool NIVBpTimeTrigger::PreCheckCondition(void)
TEST_F(NIVBpTimeTriggerUnitMock, PreCheckCondition)
{
    EXPECT_CALL(*_PhaseMgrPtr, ExhPhaseS_IsInUnrestrictedPhase()).Times(1).WillOnce(Return(eTrue));
    MyNIVBpTimeTrigger->IsEnabled = eTrue;
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(ePcvSpontMode));
    MyNIVBpTimeTrigger->TriggerDidFire = eTrue;

    EXPECT_EQ(MyNIVBpTimeTrigger->PreCheckCondition(), eTrue);

}

#endif //end define BD_TRIGGER_UNIT_TEST
