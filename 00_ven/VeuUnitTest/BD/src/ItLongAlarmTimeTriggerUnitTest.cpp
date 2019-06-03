#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "ItLongAlarmTimeTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "MgrBdSettingMockHeader.h"
#include "AlarmMockHeader.h"
#include "TriggerMgrMockHeader.h"

const UNSIGNED IT_LONG_TIME_PERIOD_IN_TICKS = 3500 / MS_PER_TICK;
const int BD_CYCLE_TIME_IN_TICKS = 0;

ItLongAlarmTimeTrigger::ItLongAlarmTimeTrigger (void) :
        TimeTrigger (eItLongAlarmTimeTrigger,(CHAR*) "ITLNGTMR")
{

    // Store the Time Period in System Clock Ticks.
    SetTimePeriod (IT_LONG_TIME_PERIOD_IN_TICKS);

}   // end ItLongAlarmTimeTrigger()
void ItLongAlarmTimeTrigger::EnableTrigger (void)
{
    STATUS status;              // status of RTOS calls
    E_VentilationMode mode;     // current mode
    UNSIGNED breathPeriod;      // BP based on current Resp Rate setting

    mode = (E_VentilationMode)BDMgrBdSettingGetCurrentValue(eMode);
    breathPeriod = triggerMgrPtrGetTimePeriod(eBpTimeTrigger);

    if ((eMixedMode == mode) &&
            (breathPeriod < IT_LONG_TIME_PERIOD_IN_TICKS))
    {
        // Reset the timer for one BD cycle less than the Breath Period.  Using
        // 1 BD cycle less avoids a potential race condition where both triggers
        // fire at about the same time but the BP trigger gets processed first.
        // Using 0 as the second argument indicates that the timer only goes off
        // 1 time (it's a 1 shot timer).  Passing TRUE as the last argument
        // indicates that the timer should be enabled now.
        // Perform error handling if the reset is not successful.
        Reset(breathPeriod - BD_CYCLE_TIME_IN_TICKS);
        ASSERTION(status == NU_SUCCESS);

        // Invoke the base class method to perform common processing.
        Trigger::EnableTrigger();
    }
    else
    {

        // Use the default time period.
        TimeTrigger::EnableTrigger();
    }

}   // end EnableTrigger()
void ItLongAlarmTimeTrigger::PhaseInSettingChange (LONG)
{

    // This operation should never be called; just assert.
    ASSERTION(eFalse);

}   // end PhaseInSettingChange()
E_Bool ItLongAlarmTimeTrigger::CheckCondition (void)
{

    // If the TriggerDidFire attribute is TRUE, it indicates that the timer
    // expired, in which case the alarm status is updated to active.
    if(eTrue == TriggerDidFire)
    {
        BDAlarmTaskSetIsActive(eITimeTooLong, eActive);
    }

    return (TriggerDidFire);

}   // end CheckCondition()
void ItLongAlarmTimeTrigger::DisableTrigger (void)
{
    // Disable the Timer; this invokes the RTOS Timer::Disable() method.
    Stop();

    if(eFalse == TriggerDidFire)
    {
        // Update Alarm Status
        BDAlarmTaskSetIsActive(eITimeTooLong, eNotActive);
    }

    // Invoke base class disable method
    Trigger::DisableTrigger ();

}   // end DisableTrigger()

class ItLongAlarmTimeTriggerUnitMock : public BDTestFixture
{
public:
    ItLongAlarmTimeTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~ItLongAlarmTimeTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyItLongAlarmTimeTrigger = new ItLongAlarmTimeTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyItLongAlarmTimeTrigger;
    }
    ItLongAlarmTimeTrigger *MyItLongAlarmTimeTrigger;
};

//test for ItLongAlarmTimeTrigger::ItLongAlarmTimeTrigger (void)
TEST_F(ItLongAlarmTimeTriggerUnitMock, ItLongAlarmTimeTrigger)
{
    //all calling function is tested, not need to test
}

//test for void ItLongAlarmTimeTrigger::EnableTrigger (void)
TEST_F(ItLongAlarmTimeTriggerUnitMock, EnableTrigger)
{
    //all calling function is tested, not need to test
}

//test for void ItLongAlarmTimeTrigger::PhaseInSettingChange (LONG)
TEST_F(ItLongAlarmTimeTriggerUnitMock, PhaseInSettingChange)
{
    //all calling function is tested, not need to test
}

//test for E_Bool ItLongAlarmTimeTrigger::CheckCondition (void)
TEST_F(ItLongAlarmTimeTriggerUnitMock, CheckCondition)
{
    MyItLongAlarmTimeTrigger->TriggerDidFire = eTrue;

    EXPECT_CALL(*_Alarm,  BDAlarmTaskSetIsActive(eITimeTooLong, eActive)).Times(1);

    MyItLongAlarmTimeTrigger->CheckCondition();
}

//test for void ItLongAlarmTimeTrigger::DisableTrigger (void)
TEST_F(ItLongAlarmTimeTriggerUnitMock, DisableTrigger)
{
    MyItLongAlarmTimeTrigger->TriggerDidFire = eFalse;

    EXPECT_CALL(*_Alarm,  BDAlarmTaskSetIsActive(eITimeTooLong, eNotActive)).Times(1);

    MyItLongAlarmTimeTrigger->DisableTrigger();
}

#endif //end define BD_TRIGGER_UNIT_TEST
