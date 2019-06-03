#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "PtExhFlowTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"
#include "PhaseMgrMockHeader.h"

PtExhFlowTrigger::PtExhFlowTrigger (void) :
        Trigger (ePtExhFlowTrigger)
{

}   // end PtExhFlowTrigger()

E_Bool PtExhFlowTrigger::CheckCondition (void)
{
    LONG filteredDelFlow;

    // Get ta pointer to the MgrBreathData object
//    MgrBreathData* mgrBreathDataPtr = MgrBreathData::S_GetInstance ();

    // Filter the flow value using the equation:
    // y(n) = 0.5 * x(n) + 0.5 * y(n-1)
    filteredDelFlow = (PrevFilteredDelFlow >> 1)
            + (mgrBreathDataPtrGetDeliveredFlow () >> 1);

    // Update the Previous value to prepare for the next cycle
    PrevFilteredDelFlow = filteredDelFlow;

    // Compare the delivered flow for the current cycle to the exhalation
    // sensitivity (percentage) of the peak delivered flow.
    // The divide by 100 is needed since ExhSens is a percentage.
    if(filteredDelFlow
            <= ((ExhSens * mgrBreathDataPtrGetPeakDeliveredFlow ()) / 100))
    {

        // Check if the time elapsed since the start of inhalation is greater
        // than or equal to the initial phase.  During the initial part of
        // inhalation, the criteria that cause this trigger to fire must be
        // met for a minimum amount of time.  But after the initial
        // time period, the trigger criteria is met right away, so set
        // the TriggerDidFire attribute.
        if(InhPhaseS_CalcTimeSincePhaseStart () >= INIT_FLOW_CRITERIA_PHASE)
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
                if(BDCalcElapsedTime (TimeConditionFirstMet) >= MIN_DURATION_FLOW_COND_MET)

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
        // Failed the flow condition check.
        IsConditionMet = eFalse;
    }

    return (TriggerDidFire);

}   // end CheckCondition()
void PtExhFlowTrigger::EnableTrigger (void)
{

    // Initialize IsConditionMet.
    IsConditionMet = eFalse;

    // Get the exhalation sensitivity; remove scaling and store the value in %
    ExhSens = BDMgrBdSettingGetCurrentValue (eExhSens)/ SCALE;

    // Initialize the previous filtered flow value
    PrevFilteredDelFlow = 0;

    // Invoke Base Class Enable Method
//    Trigger::EnableTrigger ();

}   // end EnableTrigger()
void PtExhFlowTrigger::EnableTrigger (UNSIGNED sensitivity)
{

    // Initialize IsConditionMet.
    IsConditionMet = eFalse;

    // Use the passed in sensitivity
    ExhSens = (LONG) sensitivity;

    // Invoke Base Class Enable Method
//    Trigger::EnableTrigger ();

}   // end EnableTrigger()

class PtExhFlowTriggerUnitMock : public BDTestFixture
{
public:
    PtExhFlowTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~PtExhFlowTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyPtExhFlowTrigger = new PtExhFlowTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyPtExhFlowTrigger;
    }
    PtExhFlowTrigger *MyPtExhFlowTrigger;
};

//test for PtExhFlowTrigger::PtExhFlowTrigger (void) :
TEST_F(PtExhFlowTriggerUnitMock, PtExhFlowTrigger)
{
    //no source, not need to test
}

//test for E_Bool PtExhFlowTrigger::CheckCondition (void)
TEST_F(PtExhFlowTriggerUnitMock, CheckCondition_0)
{
    MyPtExhFlowTrigger->TriggerDidFire = eFalse;
    MyPtExhFlowTrigger->PrevFilteredDelFlow = 2;
    MyPtExhFlowTrigger->ExhSens = 1;

    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetDeliveredFlow()).Times(1).WillOnce(Return(2));
    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetPeakDeliveredFlow()).Times(1).WillOnce(Return(1000));

    EXPECT_CALL(*_PhaseMgrPtr, InhPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(300));

    MyPtExhFlowTrigger->CheckCondition();

    EXPECT_EQ(MyPtExhFlowTrigger->TriggerDidFire, eTrue);
}
TEST_F(PtExhFlowTriggerUnitMock, CheckCondition_1)
{
    MyPtExhFlowTrigger->TriggerDidFire = eFalse;
    MyPtExhFlowTrigger->PrevFilteredDelFlow = 2;
    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetDeliveredFlow()).Times(1).WillOnce(Return(2));
    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetPeakDeliveredFlow()).Times(1).WillOnce(Return(1000));

//    EXPECT_CALL(*_PhaseMgrPtr, InhPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(1));

    MyPtExhFlowTrigger->IsConditionMet = eTrue;
    MyPtExhFlowTrigger->TimeConditionFirstMet = 1;
//    EXPECT_CALL(*_BreathMgr, BDCalcElapsedTime (1)).Times(1).WillOnce(Return(200));

    EXPECT_EQ(MyPtExhFlowTrigger->CheckCondition(), eFalse) ;
}
TEST_F(PtExhFlowTriggerUnitMock, CheckCondition_2)
{
    MyPtExhFlowTrigger->TriggerDidFire = eFalse;
    MyPtExhFlowTrigger->PrevFilteredDelFlow = 2;
    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetDeliveredFlow()).Times(1).WillOnce(Return(2));
    EXPECT_CALL(*_BreathMgr, mgrBreathDataPtrGetPeakDeliveredFlow()).Times(1).WillOnce(Return(1000));

//    EXPECT_CALL(*_PhaseMgrPtr, InhPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(1));

    MyPtExhFlowTrigger->IsConditionMet = eFalse;
    MyPtExhFlowTrigger->TimeConditionFirstMet = 1;

    EXPECT_EQ(MyPtExhFlowTrigger->CheckCondition(), eFalse);
    EXPECT_EQ(MyPtExhFlowTrigger->IsConditionMet, eFalse);
}

//test for void PtExhFlowTrigger::EnableTrigger (void)
TEST_F(PtExhFlowTriggerUnitMock, EnableTrigger)
{
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue (eExhSens)).Times(1);

    MyPtExhFlowTrigger->EnableTrigger();

    EXPECT_EQ(MyPtExhFlowTrigger->IsConditionMet, eFalse);
    EXPECT_EQ(MyPtExhFlowTrigger->PrevFilteredDelFlow, 0);
}

//test for void PtExhFlowTrigger::EnableTrigger (UNSIGNED sensitivity)
TEST_F(PtExhFlowTriggerUnitMock, EnableTrigger_with)
{
    MyPtExhFlowTrigger->EnableTrigger(1);

    EXPECT_EQ(MyPtExhFlowTrigger->IsConditionMet, eFalse);
}

#endif //end define BD_TRIGGER_UNIT_TEST
