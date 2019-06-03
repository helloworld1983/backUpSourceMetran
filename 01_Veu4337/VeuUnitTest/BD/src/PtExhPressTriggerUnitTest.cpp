#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "PtExhPressTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;
#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"

PtExhPressTrigger::PtExhPressTrigger (void) :
        Trigger (ePtExhPressTrigger)
{

}   // end PtExhPressTrigger()
E_Bool PtExhPressTrigger::CheckCondition (void)
{
    // Check if the monitored pressure is greater than or equal to the
    // PressureLimit.  The PressureLimit is initialized in EnableTrigger().
    if(MgrBreathDataGetMonitoredPressure () >= PressureLimit)
    {
        // Check if the time elapsed since the start of inhalation is greater
        // than or equal to the initial phase.  During the initial part of
        // inhalation, the criteria that cause this trigger to fire must be
        // met for a minimum amount of time.  But after the initial
        // time period, the trigger criteria is met right away, so set
        // the TriggerDidFire attribute.
        if(InhPhaseS_CalcTimeSincePhaseStart () >= INIT_PRESS_CRITERIA_PHASE)
        {
            TriggerDidFire = eTrue;
        }

        else
        {
            // The initial part of the inhalation is active.  Check if the
            // condition was met on the previous cycle.
            if(eTrue == IsConditionMet)
            {
                // Since the condition has been met before, see how long the
                // condition has been met.  If this value exceeds the minimum
                // time criteria, set the TriggerDidFire attribute.
                if(BDCalcElapsedTime (TimeConditionFirstMet) >= MIN_DURATION_PRESS_COND_MET)

                {
                    TriggerDidFire = eTrue;
                }

            }
            else
            {
                // The condition was not met on the previous cycle, so set
                // the IsConditionMet flag and store the time stamp of when
                // the condition was met.
                IsConditionMet = eTrue;
//                TimeConditionFirstMet = SystemClock::Retrieve();

            }

        }
    }
    else
    {
        // Failed the pressure condition check.
        IsConditionMet = eFalse;
    }

    return (TriggerDidFire);

}   // end CheckCondition()
void PtExhPressTrigger::EnableTrigger (void)
{

    // This enable method should not be used, must provide Pressure
    ASSERTION(eFalse);

}   // end EnableTrigger()
void PtExhPressTrigger::EnableTrigger (UNSIGNED pressure)
{

    // Initialize IsConditionMet.
    IsConditionMet = eFalse;

    // Use the passed in pressure
    PressureLimit = (SHORT) pressure;

    // Invoke Base Class Enable Method
//    Trigger::EnableTrigger ();

}   // end EnableTrigger()

class PtExhPressTriggerUnitMock : public BDTestFixture
{
public:
    PtExhPressTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~PtExhPressTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyPtExhPressTrigger = new PtExhPressTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyPtExhPressTrigger;
    }
    PtExhPressTrigger *MyPtExhPressTrigger;
};

//test for PtExhPressTrigger::PtExhPressTrigger (void) :
TEST_F(PtExhPressTriggerUnitMock, PtExhPressTrigger)
{
    //no source, not need to test
}

//test for E_Bool PtExhPressTrigger::CheckCondition (void)
TEST_F(PtExhPressTriggerUnitMock, CheckCondition_0)
{
    MyPtExhPressTrigger->PressureLimit = 3;
    MyPtExhPressTrigger->TriggerDidFire = eTrue;

    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(1));

    EXPECT_EQ(MyPtExhPressTrigger->CheckCondition(), eTrue);

}
TEST_F(PtExhPressTriggerUnitMock, CheckCondition_1)
{
    MyPtExhPressTrigger->PressureLimit = 3;
    MyPtExhPressTrigger->TriggerDidFire = eFalse;

    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(5));

    EXPECT_CALL(*_PhaseMgrPtr, InhPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(INIT_PRESS_CRITERIA_PHASE));

    EXPECT_EQ(MyPtExhPressTrigger->CheckCondition(), eTrue);
}
TEST_F(PtExhPressTriggerUnitMock, CheckCondition_2)
{
    MyPtExhPressTrigger->PressureLimit = 3;
    MyPtExhPressTrigger->TriggerDidFire = eFalse;

    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(5));

    EXPECT_CALL(*_PhaseMgrPtr, InhPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(INIT_PRESS_CRITERIA_PHASE - 2));

    MyPtExhPressTrigger->IsConditionMet = eTrue;
    MyPtExhPressTrigger->TimeConditionFirstMet = 1;

    EXPECT_CALL(*_BreathMgr, BDCalcElapsedTime (1)).Times(1).WillOnce(Return(MIN_DURATION_PRESS_COND_MET));

    EXPECT_EQ(MyPtExhPressTrigger->CheckCondition(), eTrue);
}

TEST_F(PtExhPressTriggerUnitMock, CheckCondition_3)
{
    MyPtExhPressTrigger->PressureLimit = 3;
    MyPtExhPressTrigger->TriggerDidFire = eFalse;

    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(5));

    EXPECT_CALL(*_PhaseMgrPtr, InhPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(INIT_PRESS_CRITERIA_PHASE - 2));

    MyPtExhPressTrigger->IsConditionMet = eFalse;
    MyPtExhPressTrigger->TimeConditionFirstMet = 1;

    EXPECT_EQ(MyPtExhPressTrigger->CheckCondition(), eFalse);
    EXPECT_EQ(MyPtExhPressTrigger->IsConditionMet, eTrue);
}

//test for void PtExhPressTrigger::EnableTrigger (void)
TEST_F(PtExhPressTriggerUnitMock, EnableTrigger)
{
    //no source, not need to test
}

//test for void PtExhPressTrigger::EnableTrigger (UNSIGNED pressure)
TEST_F(PtExhPressTriggerUnitMock, EnableTrigger_with)
{
    MyPtExhPressTrigger->EnableTrigger(2);

    EXPECT_EQ(MyPtExhPressTrigger->IsConditionMet, eFalse);
}

#endif //end define BD_TRIGGER_UNIT_TEST
