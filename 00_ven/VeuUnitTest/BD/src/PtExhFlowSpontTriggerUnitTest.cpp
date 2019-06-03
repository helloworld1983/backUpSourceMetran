#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "PtExhFlowSpontTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "StepMotorMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"

static const SHORT SPON_PEAKFLOW_TERM    = 10; // 1/10%
static const SHORT SPON_FLOWRATE_TERM    = 5*SCALE; // 5ml/s,scaled

PtExhFlowSpontTrigger::PtExhFlowSpontTrigger(void):Trigger(ePtExhFlowSpontTrigger)
{
    PrevFilteredDelFlow = 0;
}   // end PtExhFlowSpontTrigger()
E_Bool PtExhFlowSpontTrigger::CheckCondition(void)
{
    const UNSIGNED INIT_FLOW_CRITERIA_PHASE = 200/MS_PER_TICK;  // 200 ms in TICKS

    LONG filteredDelFlow;

    // Get ta pointer to the MgrBreathData object
//    MgrBreathData* mgrBreathDataPtr = MgrBreathData::S_GetInstance();

    // Filter the flow value using the equation:
    // y(n) = 0.5 * x(n) + 0.5 * y(n-1)
    filteredDelFlow = (PrevFilteredDelFlow >> 1) +
                      (mgrBreathDataPtrGetDeliveredFlow() >> 1);

    // Update the Previous value to prepare for the next cycle
    PrevFilteredDelFlow = filteredDelFlow;

    // Check if the time elapsed since the start of inhalation is greater
    // than or equal to the initial phase.  During the initial part of
    // inhalation, the criteria that cause this trigger to fire must be
    // met for a minimum amount of time.  But after the initial
    // time period, the trigger criteria is met right away, so set
    // the TriggerDidFire attribute.
    if (InhPhaseS_CalcTimeSincePhaseStart() >= INIT_FLOW_CRITERIA_PHASE)
    {
        // Compare the delivered flow for the current cycle to the exhalation
        // sensitivity (percentage) of the peak delivered flow.
        // The divide by 100 is needed since ExhSens is a percentage.
        if ((filteredDelFlow < (mgrBreathDataPtrGetPeakDeliveredFlow()/ SPON_PEAKFLOW_TERM))
            ||(filteredDelFlow < SPON_FLOWRATE_TERM))
        {
            TriggerDidFire = eTrue;
        }
    }

    return(TriggerDidFire);

}   // end CheckCondition()
void PtExhFlowSpontTrigger::EnableTrigger(void)
{
    // Initialize the previous filtered flow value
    PrevFilteredDelFlow = 0;

    // Invoke Base Class Enable Method
//    Trigger::EnableTrigger();


}   // end EnableTrigger()

class PtExhFlowSpontTriggerUnitMock : public BDTestFixture
{
public:
    PtExhFlowSpontTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~PtExhFlowSpontTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyPtExhFlowSpontTrigger = new PtExhFlowSpontTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyPtExhFlowSpontTrigger;
    }
    PtExhFlowSpontTrigger *MyPtExhFlowSpontTrigger;
};

//test for PtExhFlowSpontTrigger::PtExhFlowSpontTrigger(void):Trigger(ePtExhFlowSpontTrigger)
TEST_F(PtExhFlowSpontTriggerUnitMock, PtExhFlowSpontTrigger)
{
    EXPECT_EQ(MyPtExhFlowSpontTrigger->PrevFilteredDelFlow, 0);
}

//test for E_Bool PtExhFlowSpontTrigger::CheckCondition(void)
TEST_F(PtExhFlowSpontTriggerUnitMock, CheckCondition)
{
    MyPtExhFlowSpontTrigger->PrevFilteredDelFlow = 4;
    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetDeliveredFlow()).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_PhaseMgrPtr, InhPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(200/MS_PER_TICK));

    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetPeakDeliveredFlow()).Times(1).WillOnce(Return(100*SPON_PEAKFLOW_TERM));

    EXPECT_EQ(MyPtExhFlowSpontTrigger->CheckCondition(), eTrue);
}

//test for void PtExhFlowSpontTrigger::EnableTrigger(void)
TEST_F(PtExhFlowSpontTriggerUnitMock, EnableTrigger)
{
    MyPtExhFlowSpontTrigger->EnableTrigger();

    EXPECT_EQ(MyPtExhFlowSpontTrigger->PrevFilteredDelFlow, 0);
}

#endif //end define BD_TRIGGER_UNIT_TEST
