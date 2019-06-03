#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "AutoSIOffTimeTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

AutoSIOffTimeTrigger* AutoSIOffTimeTrigger::S_Instance = NULL;

AutoSIOffTimeTrigger* AutoSIOffTimeTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new AutoSIOffTimeTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()
AutoSIOffTimeTrigger::AutoSIOffTimeTrigger()
    : TimeTrigger(eAutoSIOffTimeTrigger,(CHAR*)"AutoSI")
{
    currentTime = 0;
}
E_Bool AutoSIOffTimeTrigger::CheckCondition(void)
{
    E_Bool rtnValue = TriggerDidFire;
    if(TriggerDidFire)
    {
        TriggerDidFire = eFalse;
        DisableTrigger();
    }
    return (rtnValue);
}
void AutoSIOffTimeTrigger::EnableTrigger(void)
{
    const UNSIGNED BD_CYCLE_TIME_IN_TICKS = 2;

    const UNSIGNED OFFSET_IN_TICKS =
            (BD_CYCLE_TIME_IN_TICKS << 1);

    UNSIGNED timePeriod;

//    STATUS status;

    if(currentTime == 0)
    {
        return;
    }
    else
    {
        timePeriod = ((currentTime * TICKS_PER_SEC) / SCALE) + OFFSET_IN_TICKS;

        SetTimePeriod(timePeriod);
//        status = Reset(GetTimePeriod(), 0, TRUE);
        Reset();

        Trigger::EnableTrigger();
    }
}   // end EnableTrigger()
void AutoSIOffTimeTrigger::PhaseInSettingChange(LONG value)
{
    const UNSIGNED BD_CYCLE_TIME_IN_TICKS = 2;

    if(currentTime == value)
        return;
    if(value == 0)
    {
        DisableTrigger();
        currentTime = value;
    }
    else if(currentTime != value)
    {
        DisableTrigger();
        // To avoid a race condition if the apnea interval is exactly equal to
        // the breath period, the apnea interval is offset by two times the BD cycle
        const UNSIGNED OFFSET_IN_TICKS =
                (BD_CYCLE_TIME_IN_TICKS << 1);

        UNSIGNED timePeriod;    // new time period in TICKS

        // The Apnea interval is in seconds * SCALE.  Need to convert to TICKS.
        // Also, offset the value.
        timePeriod = ((value * TICKS_PER_SEC) / SCALE) + OFFSET_IN_TICKS;

        // Update the time period; it will be used when the Apnea Timer is enabled
        // at the start of the next inhalation.  Apnea Interval changes are not
        // phased in right away.
        SetTimePeriod(timePeriod);
        currentTime = value;
        EnableTrigger();
    }
}

class AutoSIOffTimeTriggerUnitMock : public BDTestFixture
{
public:
    AutoSIOffTimeTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~AutoSIOffTimeTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyAutoSIOffTimeTrigger = new AutoSIOffTimeTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyAutoSIOffTimeTrigger;
    }
    AutoSIOffTimeTrigger *MyAutoSIOffTimeTrigger;
};

//test for AutoSIOffTimeTrigger* AutoSIOffTimeTrigger::S_GetInstance(void)
TEST_F(AutoSIOffTimeTriggerUnitMock, S_GetInstance)
{
    MyAutoSIOffTimeTrigger->S_GetInstance();

    EXPECT_NE(MyAutoSIOffTimeTrigger->S_Instance, (void*)0);
}

//test for AutoSIOffTimeTrigger::AutoSIOffTimeTrigger()
TEST_F(AutoSIOffTimeTriggerUnitMock, AutoSIOffTimeTrigger)
{
    EXPECT_EQ(MyAutoSIOffTimeTrigger->currentTime, 0);
}

//test for E_Bool AutoSIOffTimeTrigger::CheckCondition(void)
TEST_F(AutoSIOffTimeTriggerUnitMock, CheckCondition)
{
    MyAutoSIOffTimeTrigger->TriggerDidFire = eTrue;

    EXPECT_EQ(MyAutoSIOffTimeTrigger->CheckCondition(), eTrue);
}

//test for void AutoSIOffTimeTrigger::EnableTrigger(void)
TEST_F(AutoSIOffTimeTriggerUnitMock, EnableTrigger)
{
    //all calling function is tested, not need to test
}

//test for void AutoSIOffTimeTrigger::PhaseInSettingChange(LONG value)
TEST_F(AutoSIOffTimeTriggerUnitMock, PhaseInSettingChange)
{
    MyAutoSIOffTimeTrigger->PhaseInSettingChange(10);

    EXPECT_EQ(MyAutoSIOffTimeTrigger->currentTime, 10);
}

#endif //end define BD_TRIGGER_UNIT_TEST
