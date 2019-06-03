#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "ApneaAlarmTimeTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "AlarmMockHeader.h"

const int BD_CYCLE_TIME_IN_TICKS = 1;

// Define static attributes.
ApneaAlarmTimeTrigger* ApneaAlarmTimeTrigger::S_Instance = NULL;
ApneaAlarmTimeTrigger* ApneaAlarmTimeTrigger::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new ApneaAlarmTimeTrigger ();
        ASSERTION(S_Instance != NULL);

    }

    return (S_Instance);

}   // end S_GetInstance()
E_Bool ApneaAlarmTimeTrigger::CheckCondition (void)
{
    // If the timer has already expired, the TriggerDidFire attribute has been
    // set to TRUE.
    if(eTrue == TriggerDidFire)
    {
        // Update alarm status
        BDAlarmTaskSetIsActive (eApnea, eActive);
    }

    return (TriggerDidFire);

}   // end CheckCondition()
void ApneaAlarmTimeTrigger::ReEnableTrigger (void)
{
    // Only re-enable it if it is enabled.
    if(eTrue == IsEnabled)
    {
        // Disable the apnea interval timer
        Stop();

        // Set the IsEnabled flag to false so that EnableTrigger re-enables
        // the trigger.
        IsEnabled = eFalse;

        // Re-enable the trigger.  This operation will also restart the timer.
        EnableTrigger ();

    }
}   // end ReEnableTrigger()
void ApneaAlarmTimeTrigger::EnableTrigger (void)
{
    // It is possible for this method to be called when the
    // trigger is already enabled (e.g. when changing modes Ac -> Cpap)
    // If it is already enabled, leave the timer alone since the timer should
    // run across mode change boundaries.
    if(eFalse == IsEnabled)
    {

        // Restart the timer for the Apnea Interval time period, which is
        // stored in the TimePeriod attribute.   Since this timer is a one-shot
        // timer, 0 is passed as the reschedule time.
        Reset();

        // Perform error handling if return status is not successful.

        // Invoke base class enable method which updates the IsEnabled and
        // TriggerDidFire flags
//        Trigger::EnableTrigger ();
    }

}   // end EnableTrigger()
void ApneaAlarmTimeTrigger::PhaseInSettingChange (LONG value)
{
    // To avoid a race condition if the apnea interval is exactly equal to
    // the breath period, the apnea interval is offset by two times the BD cycle
    const UNSIGNED APNEA_INT_OFFSET_IN_TICKS = (BD_CYCLE_TIME_IN_TICKS << 1);

    UNSIGNED timePeriod;    // new time period in TICKS

    // The Apnea interval is in seconds * SCALE.  Need to convert to TICKS.
    // Also, offset the value.
    timePeriod = ((value * TICKS_PER_SEC) / SCALE) + APNEA_INT_OFFSET_IN_TICKS;

    // Update the time period; it will be used when the Apnea Timer is enabled
    // at the start of the next inhalation.  Apnea Interval changes are not
    // phased in right away.
    SetPeriod(timePeriod);

}   // end PhaseInSettingChange()
ApneaAlarmTimeTrigger::ApneaAlarmTimeTrigger (void) :
        TimeTrigger (eApneaAlarmTimeTrigger, (CHAR*)"AVINTTMR")
{

}   // end ApneaAlarmTimeTrigger()

class ApneaAlarmTimeTriggerUnitMock : public BDTestFixture
{
public:
    ApneaAlarmTimeTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~ApneaAlarmTimeTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyApneaAlarmTimeTrigger = new ApneaAlarmTimeTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyApneaAlarmTimeTrigger;
    }
    ApneaAlarmTimeTrigger *MyApneaAlarmTimeTrigger;
};

//test for ApneaAlarmTimeTrigger* ApneaAlarmTimeTrigger::S_GetInstance (void)
TEST_F(ApneaAlarmTimeTriggerUnitMock, S_GetInstance)
{
    MyApneaAlarmTimeTrigger->S_GetInstance();

    EXPECT_NE(MyApneaAlarmTimeTrigger->S_Instance, (void*)0);
}

//test for E_Bool ApneaAlarmTimeTrigger::CheckCondition (void)
TEST_F(ApneaAlarmTimeTriggerUnitMock, CheckCondition)
{
    MyApneaAlarmTimeTrigger->TriggerDidFire = eTrue;
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eApnea, eActive)).Times(1);

    EXPECT_EQ(MyApneaAlarmTimeTrigger->CheckCondition(), eTrue);
}

//test for void ApneaAlarmTimeTrigger::ReEnableTrigger (void)
TEST_F(ApneaAlarmTimeTriggerUnitMock, ReEnableTrigger)
{
    MyApneaAlarmTimeTrigger->IsEnabled = eTrue;

    MyApneaAlarmTimeTrigger->ReEnableTrigger();

    EXPECT_EQ(MyApneaAlarmTimeTrigger->IsEnabled, eFalse);
}

//test for void ApneaAlarmTimeTrigger::EnableTrigger (void)
TEST_F(ApneaAlarmTimeTriggerUnitMock, EnableTrigger)
{
    //all calling function is tested, not need to test
}

//test for void ApneaAlarmTimeTrigger::PhaseInSettingChange (LONG value)
TEST_F(ApneaAlarmTimeTriggerUnitMock, PhaseInSettingChange)
{
    //all calling function is tested, not need to test
}

//test for ApneaAlarmTimeTrigger::ApneaAlarmTimeTrigger (void) :
TEST_F(ApneaAlarmTimeTriggerUnitMock, ApneaAlarmTimeTrigger)
{
    //no srouce, not need to test
}

#endif //end define BD_TRIGGER_UNIT_TEST
