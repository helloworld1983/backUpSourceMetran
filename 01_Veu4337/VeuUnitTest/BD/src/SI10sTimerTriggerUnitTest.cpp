#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "SI10sTimerTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "AlarmMockHeader.h"
#include "ModeMgrMockHeader.h"

const UNSIGNED SI_TIME_IN_TICKS = 30000/MS_PER_TICK; // 30 sec

SI10sTimerTrigger::SI10sTimerTrigger(void)
    : TimeTrigger(eSI10sTimeTrigger,(CHAR*) "SITENSEC")
{
//    SIMsg.Value = 0;

    one_hundred_ms = 0;
    currentSITime = 0;
    // Store the Time Period in System Clock Ticks.
    SetTimePeriod(SI_TIME_IN_TICKS);

}   // end SI10sTimerTrigger()
void SI10sTimerTrigger::EnableTrigger(void)
{
    one_hundred_ms = 0;
    currentSITime = 0;
    this->Start();
    TimeTrigger::EnableTrigger();
}   // end EnableTrigger()
E_Bool SI10sTimerTrigger::CheckCondition(void)
{
    UpdateSITime();
    return(TriggerDidFire);
}   // end CheckCondition()
void SI10sTimerTrigger::UpdateSITime()
{
    one_hundred_ms = one_hundred_ms + 2;
    if(one_hundred_ms >= 100)
    {
        one_hundred_ms = 0;
        currentSITime = currentSITime + 10;
//        SIMsg.Value = currentSITime;
//        MainHandlerDelegate::S_GetInstance()->postCustomEvent<PatientDataGUIEvent>(eManInspHoldTimePtDataID, SIMsg);
    }
}
void SI10sTimerTrigger::DisableTrigger(void)
{
    one_hundred_ms = 0;
    currentSITime = 0;
//    Disable();
    this->Stop();
    Trigger::DisableTrigger();

}   // end DisableTrigger()
void SI10sTimerTrigger::ProcessTrigger(Mode* mode)
{
    modeProcessTriggerID(eSI10sTimeTrigger);
}

class SI10sTimerTriggerUnitMock : public BDTestFixture
{
public:
    SI10sTimerTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~SI10sTimerTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MySI10sTimerTrigger = new SI10sTimerTrigger();
    }
    virtual void TearDown(void)
    {
        delete MySI10sTimerTrigger;
    }
    SI10sTimerTrigger *MySI10sTimerTrigger;
};

//test for SI10sTimerTrigger::SI10sTimerTrigger(void)
TEST_F(SI10sTimerTriggerUnitMock, SI10sTimerTrigger)
{
    EXPECT_EQ(MySI10sTimerTrigger->one_hundred_ms, 0);
    EXPECT_EQ(MySI10sTimerTrigger->currentSITime, 0);
}

//test for void SI10sTimerTrigger::EnableTrigger(void)
TEST_F(SI10sTimerTriggerUnitMock, EnableTrigger)
{
    MySI10sTimerTrigger->EnableTrigger();

    EXPECT_EQ(MySI10sTimerTrigger->one_hundred_ms, 0);
    EXPECT_EQ(MySI10sTimerTrigger->currentSITime, 0);
}

//test for E_Bool SI10sTimerTrigger::CheckCondition(void)
TEST_F(SI10sTimerTriggerUnitMock, CheckCondition)
{
    MySI10sTimerTrigger->TriggerDidFire = eTrue;

    EXPECT_EQ(MySI10sTimerTrigger->CheckCondition(), eTrue);
}

//test for void SI10sTimerTrigger::UpdateSITime()
TEST_F(SI10sTimerTriggerUnitMock, UpdateSITime_0)
{
    MySI10sTimerTrigger->one_hundred_ms = 0;

    MySI10sTimerTrigger->UpdateSITime();

    EXPECT_EQ(MySI10sTimerTrigger->one_hundred_ms, 2);
}
TEST_F(SI10sTimerTriggerUnitMock, UpdateSITime_1)
{
    MySI10sTimerTrigger->one_hundred_ms = 0;
    MySI10sTimerTrigger->currentSITime = 0;

    for(int i = 0; i < 50; ++i)
    {
        MySI10sTimerTrigger->UpdateSITime();
    }

    EXPECT_EQ(MySI10sTimerTrigger->one_hundred_ms, 0);
    EXPECT_EQ(MySI10sTimerTrigger->currentSITime, 10);
}

//test for void SI10sTimerTrigger::DisableTrigger(void)
TEST_F(SI10sTimerTriggerUnitMock, DisableTrigger)
{
    MySI10sTimerTrigger->EnableTrigger();

    EXPECT_EQ(MySI10sTimerTrigger->one_hundred_ms, 0);
    EXPECT_EQ(MySI10sTimerTrigger->currentSITime, 0);
}

//test for void SI10sTimerTrigger::ProcessTrigger(Mode* mode)
TEST_F(SI10sTimerTriggerUnitMock, ProcessTrigger)
{
    EXPECT_CALL(*_ModeMgr, modeProcessTriggerID(eSI10sTimeTrigger)).Times(1);

    MySI10sTimerTrigger->ProcessTrigger((Mode*)0);
}

#endif //end define BD_TRIGGER_UNIT_TEST
