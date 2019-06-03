#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "TimeTrigger.h"
#include "DebugTrace.h"

const UNSIGNED BD_CYCLE_TIME_IN_TICKS = 1;

using namespace ::testing;

TimeTrigger::TimeTrigger (E_TriggerId id, CHAR* timerName) :
        Trigger (id) , Timer("TimeTrigger", 0, pdFALSE)
{

}   // end TimeTrigger()

void TimeTrigger::EnableTrigger (void)
{
    // Reset the timer for the TimePeriod.  Using 0 as the second argument
    // indicates that the timer only goes off 1 time (it's a 1 shot timer).
    // Passing TRUE as the last argument indicates that the timer should be
    // enabled now.   Perform error handling if the reset is not successful.
    Reset();
    // Invoke the base class method to perform common processing.
//    Trigger::EnableTrigger ();

}   // end EnableTrigger()

void TimeTrigger::EnableTrigger (UNSIGNED time)
{
    // Since the BD Task only processes time triggers once a cycle, modify
    // the time so that it is processed in the closest interval by subtracting
    // half of the cycle time.   For eg., if the cycle time is 20 ticks and the
    // time is 104 ticks, the time is set to 94 ticks, which means that
    // it would be processed at time interval 100 ticks.  If left at 104, it
    // would not be processed until 120 ticks.
    if (time >= BD_CYCLE_TIME_IN_TICKS)
    {
        time-= BD_CYCLE_TIME_IN_TICKS >> 1;
    }

    // Reset the timer. Using 0 as the second argument indicates that the timer
    // only goes off 1 time (it's a 1 shot timer).  Passing TRUE as the last
    // argument indicates that the timer should be enabled now.
    // Perform error handling if the reset is not successful.
    Reset();
    // Invoke the base class method to perform common processing.
//    Trigger::EnableTrigger ();

}  // end EnableTrigger()

void TimeTrigger::DisableTrigger (void)
{
    // Disable the Timer.  If an error status is returned, perform error
    // handling.
    Stop();
    // Invoke Base Class Disable Method
//    Trigger::DisableTrigger ();

}   // end DisableTrigger()
void TimeTrigger::PhaseInSettingChange (LONG value)
{
    UNSIGNED timePeriod;

    // All times are in seconds * 100; need to convert to TICKS
    timePeriod = (value * 10) / MS_PER_TICK;

    // Update the time period.
    SetTimePeriod (timePeriod);

}   // end PhaseInSettingChange()
void TimeTrigger::ExpirationRoutine (void)
{
    // The timer expired so the trigger should fire.  Set the TriggerDidFire
    // attribute to TRUE.  On the next BD cycle, the BD Task will process
    // this trigger.
    TriggerDidFire = eTrue;

}   // end ExpirationRoutine()
void TimeTrigger::SetTimePeriod (UNSIGNED time)
{

    // Since the BD Task only processes time triggers once a cycle, modify
    // the time so that it is processed in the closest interval by subtracting
    // half of the cycle time.   For eg., if the cycle time is 20 ticks and the
    // time is 104 ticks, the time is set to 94 ticks, which means that
    // it would be processed at time interval 100 ticks.  If left at 104, it
    // would not be processed until 120 ticks.
    if (time >= BD_CYCLE_TIME_IN_TICKS)
    {
        time-= BD_CYCLE_TIME_IN_TICKS >> 1;
    }

    // Update both the TimePeriod and the BackupTimePeriod.  BackupTimePeriod
    // is a one's complement of the TimePeriod and is used to protect against
    // data corruption.
    TimePeriod = time;
    BackupTimePeriod = ~time;

}   // end SetTimePeriod()
UNSIGNED TimeTrigger::GetTimePeriod (void)
{
    const UNSIGNED ALL_FF = ~0;

    // Verify that the values have not been corrupted.
    ASSERTION((TimePeriod ^ BackupTimePeriod) == ALL_FF);

    return (TimePeriod);

}   // GetTimePeriod()

class TimeTriggerUnitMock : public BDTestFixture
{
public:
    TimeTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~TimeTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyTimeTrigger = new TimeTrigger(eAutoSIOnTimeTrigger, (char*)"test_timer");
    }
    virtual void TearDown(void)
    {
        delete MyTimeTrigger;
    }
    TimeTrigger *MyTimeTrigger;
};

//test for TimeTrigger::TimeTrigger (E_TriggerId id, CHAR* timerName)
TEST_F(TimeTriggerUnitMock, TimeTrigger)
{
    //no sorce, not need to test
}

//test for void TimeTrigger::EnableTrigger (void)
TEST_F(TimeTriggerUnitMock, EnableTrigger)
{
    //all calling function is tested, not need to test
}

//test for void TimeTrigger::EnableTrigger (UNSIGNED time)
TEST_F(TimeTriggerUnitMock, EnableTrigger_time)
{
    //all calling function is tested, not need to test
}

//test for void TimeTrigger::DisableTrigger (void)
TEST_F(TimeTriggerUnitMock, DisableTrigger)
{
    //all calling function is tested, not need to test
}

//test for void TimeTrigger::PhaseInSettingChange (LONG value)
TEST_F(TimeTriggerUnitMock, PhaseInSettingChange)
{
    //all calling function is tested, not need to test
}

//test for void TimeTrigger::ExpirationRoutine (void)
TEST_F(TimeTriggerUnitMock, ExpirationRoutine)
{
    MyTimeTrigger->ExpirationRoutine();

    EXPECT_EQ(MyTimeTrigger->TriggerDidFire, eTrue);
}

//test for void TimeTrigger::SetTimePeriod (UNSIGNED time)
TEST_F(TimeTriggerUnitMock, SetTimePeriod)
{
    //all calling function is tested, not need to test
}

//test for UNSIGNED TimeTrigger::GetTimePeriod (void)
TEST_F(TimeTriggerUnitMock, GetTimePeriod)
{
    MyTimeTrigger->TimePeriod = 10;

    EXPECT_EQ(MyTimeTrigger->GetTimePeriod(), 10);
}

#endif //end define BD_TRIGGER_UNIT_TEST
