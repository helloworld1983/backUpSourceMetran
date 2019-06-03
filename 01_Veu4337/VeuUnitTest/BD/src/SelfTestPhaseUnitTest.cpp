#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "SelfTestPhase.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "StepMotorMockHeader.h"
#include "BreathMgrMockHeader.h"

SelfTestPhase::SelfTestPhase(void) : Phase()
{

}   // end SelfTestPhase()

void SelfTestPhase::Start(void)
{

    // Update Patient Data
    MgrBreathDataNonVentStart();

    // Enable the air and O2 flow controllers
    O2FlowControllerReset();
    AirFlowControllerReset();
    AirFlowControllerSetDesired(0);
    O2FlowControllerSetDesired(0);
    AirFlowControllerEnable();
    O2FlowControllerEnable();

}   // end Start()
void SelfTestPhase::End(void)
{
}   // end End()
void SelfTestPhase::ProcessBdCycle(void)
{

}   // end ProcessBdCycle()

class SelfTestPhaseUnitMock : public BDTestFixture
{
public:
    SelfTestPhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~SelfTestPhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MySelfTestPhase = new SelfTestPhase();
    }
    virtual void TearDown(void)
    {
        delete MySelfTestPhase;
    }
    SelfTestPhase *MySelfTestPhase;
};

//test for SelfTestPhase::SelfTestPhase(void) : Phase()
TEST_F(SelfTestPhaseUnitMock, SelfTestPhase)
{
    //no source, not need to test
}

//test for void SelfTestPhase::Start(void)
TEST_F(SelfTestPhaseUnitMock, Start)
{
    EXPECT_CALL(*_BreathMgr, MgrBreathDataNonVentStart()).Times(1);

    EXPECT_CALL(*_StepMotor, O2FlowControllerReset()).Times(1);
    EXPECT_CALL(*_StepMotor, AirFlowControllerReset()).Times(1);

    EXPECT_CALL(*_StepMotor, AirFlowControllerSetDesired(0)).Times(1);
    EXPECT_CALL(*_StepMotor, O2FlowControllerSetDesired(0)).Times(1);

    EXPECT_CALL(*_StepMotor, AirFlowControllerEnable()).Times(1);
    EXPECT_CALL(*_StepMotor, O2FlowControllerEnable()).Times(1);

    MySelfTestPhase->Start();
}

//test for void SelfTestPhase::End(void)
TEST_F(SelfTestPhaseUnitMock, End)
{
    //no source, not need to test
}

//test for void SelfTestPhase::ProcessBdCycle(void)
TEST_F(SelfTestPhaseUnitMock, ProcessBdCycle)
{
    //no source, not need to test
}
#endif //end define BD_PHASE_UNIT_TEST
