#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "PressTrigExhPhase.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "TriggerMgrMockHeader.h"
#include "StepMotorMockHeader.h"

PressTrigExhPhase::PressTrigExhPhase (void) :
        ExhPhase ()
{

}   // end PressTrigExhPhase()
void PressTrigExhPhase::Start (void)
{
    // Close the delivery valves
    O2FlowControllerSetDesired(0);
    AirFlowControllerSetDesired(0);

    // Invoke the base class method to perform processing common to all
    // exhalation phases.
//    ExhPhase::Start ();

    // Enable the patient pressure trigger.  Not passing a pressure value
    // causes the trigger to use the operator setting for pressure
    // sensitivity
    TriggerMgrEnableTrigger (ePtInhPressTrigger);

}   // end Start()

void PressTrigExhPhase::End(void)
{

    // Invoke the base class method to perform processing common to all
    // exhalation phases.
//    ExhPhase::End();


    // Get a pointer to the TriggerMgr object
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtrDisableTrigger(ePtInhFlowTrigger);
    triggerMgrPtrDisableTrigger(ePtInhPressTrigger);

}

class PressTrigExhPhaseUnitMock : public BDTestFixture
{
public:
    PressTrigExhPhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~PressTrigExhPhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MyPressTrigExhPhase = new PressTrigExhPhase();
    }
    virtual void TearDown(void)
    {
        delete MyPressTrigExhPhase;
    }
    PressTrigExhPhase *MyPressTrigExhPhase;
};

//test for PressTrigExhPhase::PressTrigExhPhase (void) :
TEST_F(PressTrigExhPhaseUnitMock, PressTrigExhPhase)
{
    //no source, not need to test
}

//test for void PressTrigExhPhase::Start (void)
TEST_F(PressTrigExhPhaseUnitMock, Start)
{
    EXPECT_CALL(*_StepMotor, O2FlowControllerSetDesired(0)).Times(1);
    EXPECT_CALL(*_StepMotor, AirFlowControllerSetDesired(0)).Times(1);

    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger (ePtInhPressTrigger)).Times(1);

    MyPressTrigExhPhase->Start();
}

//test for void PressTrigExhPhase::End(void)
TEST_F(PressTrigExhPhaseUnitMock, End)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtInhFlowTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtInhPressTrigger)).Times(1);

    MyPressTrigExhPhase->End();
}

#endif //end define BD_PHASE_UNIT_TEST
