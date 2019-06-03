#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "AutoSIOnTimeTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;
#include "MgrBdSettingMockHeader.h"
#include "PhaseMgrMockHeader.h"

#define SECONDS_PER_MINUTE 3600
AutoSIOnTimeTrigger* AutoSIOnTimeTrigger::S_Instance = NULL;

AutoSIOnTimeTrigger* AutoSIOnTimeTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new AutoSIOnTimeTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()
AutoSIOnTimeTrigger::AutoSIOnTimeTrigger()
: TimeTrigger(eAutoSIOnTimeTrigger,(CHAR*)"AutoSI")
{
    currentTime = 0;
    currentSIRate = 1*SCALE;

    currentSITimeData = 0;
    one_hundred_ms = 0;
}
void AutoSIOnTimeTrigger::EnableTrigger(void)
{
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
        Trigger::EnableTrigger();

        currentSITimeData = 0;
        one_hundred_ms = 0;
    }
}   // end EnableTrigger()
E_Bool AutoSIOnTimeTrigger::CheckCondition(void)
{
    E_Bool rtnValue = TriggerDidFire;

    UpdateSITime();

    if(TriggerDidFire)
    {
        TriggerDidFire = eFalse;
    }

    return (rtnValue);

}
void AutoSIOnTimeTrigger::ReEnableTrigger(void)
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

void AutoSIOnTimeTrigger::PhaseInSettingChange(LONG value)
{
    const UNSIGNED BD_CYCLE_TIME_IN_TICKS = 2;

    LONG settingSIRate = BDMgrBdSettingGetCurrentValue(eSIRate);

    if(value == 0)
    {
        currentTime = value;
        AutoSIOffTimeTriggerPhaseInSettingChange(0);
    }
    else if((currentTime != value)||(currentSIRate != settingSIRate))
    {
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
        currentSIRate = settingSIRate;

        if(settingSIRate != 0)
        {
            LONG timeoff = (SECONDS_PER_MINUTE*SCALE/settingSIRate)*SCALE - value;
            AutoSIOffTimeTriggerPhaseInSettingChange(timeoff);

        }
    }
}
void AutoSIOnTimeTrigger::UpdateSITime()
{
    one_hundred_ms = one_hundred_ms + 2;
    if(one_hundred_ms >= 100)
    {
        one_hundred_ms = 0;
        currentSITimeData = currentSITimeData + 10;
        //        PatientDataGUIEvent SIMsg;
        //        SIMsg.Value = currentSITimeData;
        //        MainHandlerDelegate::S_GetInstance()->postCustomEvent<PatientDataGUIEvent>(eManInspHoldTimePtDataID, SIMsg);
    }
}

class AutoSIOnTimeTriggerUnitMock : public BDTestFixture
{
public:
    AutoSIOnTimeTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~AutoSIOnTimeTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyAutoSIOnTimeTrigger = new AutoSIOnTimeTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyAutoSIOnTimeTrigger;
    }
    AutoSIOnTimeTrigger *MyAutoSIOnTimeTrigger;
};

//test for AutoSIOnTimeTrigger* AutoSIOnTimeTrigger::S_GetInstance(void)
TEST_F(AutoSIOnTimeTriggerUnitMock, S_GetInstance)
{
    MyAutoSIOnTimeTrigger->S_GetInstance();

    EXPECT_NE(MyAutoSIOnTimeTrigger->S_Instance, (void*)0);
}

//test for AutoSIOnTimeTrigger::AutoSIOnTimeTrigger()
TEST_F(AutoSIOnTimeTriggerUnitMock, AutoSIOnTimeTrigger)
{
    EXPECT_EQ(MyAutoSIOnTimeTrigger->currentTime, 0);
    EXPECT_EQ(MyAutoSIOnTimeTrigger->currentSIRate, 1*SCALE);
    EXPECT_EQ(MyAutoSIOnTimeTrigger->currentSITimeData, 0);
    EXPECT_EQ(MyAutoSIOnTimeTrigger->one_hundred_ms, 0);
}

//test for void AutoSIOnTimeTrigger::EnableTrigger(void)
TEST_F(AutoSIOnTimeTriggerUnitMock, EnableTrigger)
{
    MyAutoSIOnTimeTrigger->IsEnabled = eFalse;

    MyAutoSIOnTimeTrigger->EnableTrigger();

    EXPECT_EQ(MyAutoSIOnTimeTrigger->currentSITimeData, 0);
    EXPECT_EQ(MyAutoSIOnTimeTrigger->one_hundred_ms, 0);
}

//test for E_Bool AutoSIOnTimeTrigger::CheckCondition(void)
TEST_F(AutoSIOnTimeTriggerUnitMock, CheckCondition)
{
    MyAutoSIOnTimeTrigger->TriggerDidFire = eTrue;

    EXPECT_EQ(MyAutoSIOnTimeTrigger->CheckCondition(), eTrue);
}

//test for void AutoSIOnTimeTrigger::ReEnableTrigger(void)
TEST_F(AutoSIOnTimeTriggerUnitMock, ReEnableTrigger)
{
    MyAutoSIOnTimeTrigger->IsEnabled = eTrue;

    MyAutoSIOnTimeTrigger->ReEnableTrigger();

    EXPECT_EQ(MyAutoSIOnTimeTrigger->IsEnabled, eTrue);
}

//test for void AutoSIOnTimeTrigger::PhaseInSettingChange(LONG value)
TEST_F(AutoSIOnTimeTriggerUnitMock, PhaseInSettingChange)
{
    MyAutoSIOnTimeTrigger->currentTime = 10;
    MyAutoSIOnTimeTrigger->currentSIRate = 40;

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eSIRate)).Times(1).WillOnce(Return(30));
    MyAutoSIOnTimeTrigger->PhaseInSettingChange(20);

    EXPECT_EQ(MyAutoSIOnTimeTrigger->currentTime, 20);
    EXPECT_EQ(MyAutoSIOnTimeTrigger->currentSIRate, 30);
}

//test for void AutoSIOnTimeTrigger::UpdateSITime()
TEST_F(AutoSIOnTimeTriggerUnitMock, UpdateSITime)
{
    //all calling function is tested, not need to test
}

#endif //end define BD_TRIGGER_UNIT_TEST
