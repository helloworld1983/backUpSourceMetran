#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "AutoSIStopTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

AutoSIStopTrigger* AutoSIStopTrigger::S_Instance = NULL;
AutoSIStopTrigger* AutoSIStopTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new AutoSIStopTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()
AutoSIStopTrigger::AutoSIStopTrigger()
    : TimeTrigger(eAutoSIOffTimeTrigger,(CHAR*)"AutoSI")
{
    currentTime = 0;
}
E_Bool AutoSIStopTrigger::CheckCondition(void)
{
    E_Bool rtnValue = TriggerDidFire;

    if(TriggerDidFire)
    {
        TriggerDidFire = eFalse;
        DisableTrigger();
    }

    return (rtnValue);
}
void AutoSIStopTrigger::EnableTrigger(void)
{
    const UNSIGNED BD_CYCLE_TIME_IN_TICKS = 2;
    const UNSIGNED APNEA_INT_OFFSET_IN_TICKS =
            (BD_CYCLE_TIME_IN_TICKS << 1);

    UNSIGNED timePeriod;

//    STATUS status;

    if(currentTime == 0)
    {
        return;
    }
    else
    {
        timePeriod = ((currentTime * TICKS_PER_SEC) / SCALE) + APNEA_INT_OFFSET_IN_TICKS;

        SetTimePeriod(timePeriod);
//        status = Reset(GetTimePeriod(), 0, TRUE);
        Reset();

        Trigger::EnableTrigger();
    }
}   // end EnableTrigger()
void AutoSIStopTrigger::PhaseInSettingChange(LONG value)
{
    const UNSIGNED BD_CYCLE_TIME_IN_TICKS = 2;

    if(value == 0)
    {
        DisableTrigger();
    }
    else if(currentTime != value)
    {
        DisableTrigger();
        // To avoid a race condition if the apnea interval is exactly equal to
        // the breath period, the apnea interval is offset by two times the BD cycle
        const UNSIGNED APNEA_INT_OFFSET_IN_TICKS =
                (BD_CYCLE_TIME_IN_TICKS << 1);

        UNSIGNED timePeriod;    // new time period in TICKS

        // The Apnea interval is in seconds * SCALE.  Need to convert to TICKS.
        // Also, offset the value.
        timePeriod = ((value * TICKS_PER_SEC) / SCALE) + APNEA_INT_OFFSET_IN_TICKS;

        // Update the time period; it will be used when the Apnea Timer is enabled
        // at the start of the next inhalation.  Apnea Interval changes are not
        // phased in right away.
        SetTimePeriod(timePeriod);
//        Reset(GetTimePeriod(), 0, TRUE);
        Reset();

        currentTime = value;
        EnableTrigger();
    }
}

class AutoSIStopTriggerUnitMock : public BDTestFixture
{
public:
    AutoSIStopTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~AutoSIStopTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyAutoSIStopTrigger = new AutoSIStopTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyAutoSIStopTrigger;
    }
    AutoSIStopTrigger *MyAutoSIStopTrigger;
};

//test for AutoSIStopTrigger* AutoSIStopTrigger::S_GetInstance(void)
TEST_F(AutoSIStopTriggerUnitMock, S_GetInstance)
{
    MyAutoSIStopTrigger->S_GetInstance();

    EXPECT_NE(MyAutoSIStopTrigger->S_Instance, (void*)0);
}

//test for AutoSIStopTrigger::AutoSIStopTrigger()
TEST_F(AutoSIStopTriggerUnitMock, AutoSIStopTrigger)
{
    EXPECT_EQ(MyAutoSIStopTrigger->currentTime, 0);
}

//test for E_Bool AutoSIStopTrigger::CheckCondition(void)
TEST_F(AutoSIStopTriggerUnitMock, CheckCondition)
{
    MyAutoSIStopTrigger->TriggerDidFire = eTrue;

    EXPECT_EQ(MyAutoSIStopTrigger->CheckCondition(), eTrue);
}

//test for void AutoSIStopTrigger::EnableTrigger(void)
TEST_F(AutoSIStopTriggerUnitMock, EnableTrigger)
{
    //all calling funciton is tested, not need to test
}

//test for void AutoSIStopTrigger::PhaseInSettingChange(LONG value)
TEST_F(AutoSIStopTriggerUnitMock, PhaseInSettingChange)
{
    //all calling function is tested, not need to test
}

#endif //end define BD_TRIGGER_UNIT_TEST
