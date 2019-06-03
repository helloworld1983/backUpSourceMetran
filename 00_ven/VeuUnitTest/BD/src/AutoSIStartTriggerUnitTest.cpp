#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "AutoSIStartTrigger.h"
#include "DebugTrace.h"

#include "MgrBdSettingMockHeader.h"
#include "PhaseMgrMockHeader.h"

using namespace ::testing;

AutoSIStartTrigger* AutoSIStartTrigger::S_Instance = NULL;
AutoSIStartTrigger::AutoSIStartTrigger()
: TimeTrigger(eAutoSIOnTimeTrigger,(CHAR*)"AutoSI")
{
    currentTime = 0;
    currentSIRate = 1*SCALE;
}
AutoSIStartTrigger* AutoSIStartTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new AutoSIStartTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()
void AutoSIStartTrigger::EnableTrigger(void)
{
    if(currentTime == 0)
        return;
    //    STATUS status;

    // It is possible for this method to be called when the
    // trigger is already enabled (e.g. when changing modes Ac -> Cpap)
    // If it is already enabled, leave the timer alone since the timer should
    // run across mode change boundaries.
    if (eFalse == IsEnabled)
    {

        // Restart the timer for the Apnea Interval time period, which is
        // stored in the TimePeriod attribute.   Since this timer is a one-shot
        // timer, 0 is passed as the reschedule time.
        //        status = Reset(GetTimePeriod(), 0, TRUE);
        Reset();

        // Invoke base class enable method which updates the IsEnabled and
        // TriggerDidFire flags
//        Trigger::EnableTrigger();
    }
}   // end EnableTrigger()
E_Bool AutoSIStartTrigger::CheckCondition(void)
{
    E_Bool rtnValue = TriggerDidFire;
    if(TriggerDidFire)
    {
        TriggerDidFire = eFalse;
        ReEnableTrigger();
    }

    return (rtnValue);

}
void AutoSIStartTrigger::ReEnableTrigger(void)
{
    // Only re-enable it if it is enabled.
    if (eTrue == IsEnabled)
    {
        //Disable timer
        Stop();
        IsEnabled = eFalse;
        EnableTrigger();

    }
}   // end ReEnableTrigger()
void AutoSIStartTrigger::PhaseInSettingChange(LONG value)
{
    static const UNSIGNED BD_CYCLE_TIME_IN_TICKS = 2;

    LONG settingSIRate = BDMgrBdSettingGetCurrentValue(eSIRate);

    if(value == 0)
    {
        DisableTrigger();
        currentTime = value;

    }
    else if((currentTime != value)||(currentSIRate != settingSIRate))
    {
        //        DisableTrigger();
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
        EnableTrigger();
        currentTime = value;
        currentSIRate = settingSIRate;


        //calculate SI Time Off
        //        LONG SIrate = MgrBdSetting::S_GetInstance()->GetCurrentValue(eSIRate);
        if(settingSIRate != 0)
        {
            LONG timeoff = (3600*SCALE/settingSIRate)*SCALE - value;
            AutoSIStopTriggerPhaseInSettingChange(timeoff);

        }
    }
}

class AutoSIStartTriggerUnitMock : public BDTestFixture
{
public:
    AutoSIStartTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~AutoSIStartTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyAutoSIStartTrigger = new AutoSIStartTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyAutoSIStartTrigger;
    }
    AutoSIStartTrigger *MyAutoSIStartTrigger;
};

//test for AutoSIStartTrigger* AutoSIStartTrigger::S_GetInstance(void)
TEST_F(AutoSIStartTriggerUnitMock, S_GetInstance)
{
    MyAutoSIStartTrigger->S_GetInstance();

    EXPECT_NE(MyAutoSIStartTrigger->S_Instance, (void*)0);
}

//test for AutoSIStartTrigger::AutoSIStartTrigger()
TEST_F(AutoSIStartTriggerUnitMock, AutoSIStartTrigger)
{
    EXPECT_EQ(MyAutoSIStartTrigger->currentTime, 0);
}

//test for E_Bool AutoSIStartTrigger::CheckCondition(void)
TEST_F(AutoSIStartTriggerUnitMock, CheckCondition)
{
    MyAutoSIStartTrigger->TriggerDidFire = eTrue;

    EXPECT_EQ(MyAutoSIStartTrigger->CheckCondition(), eTrue);
}

//test for void AutoSIStartTrigger::EnableTrigger(void)
TEST_F(AutoSIStartTriggerUnitMock, EnableTrigger)
{
    //all calling funciton is tested, not need to test
}

//test for void AutoSIStartTrigger::PhaseInSettingChange(LONG value)
TEST_F(AutoSIStartTriggerUnitMock, PhaseInSettingChange)
{
    //all calling function is tested, not need to test
}

#endif //end define BD_TRIGGER_UNIT_TEST
