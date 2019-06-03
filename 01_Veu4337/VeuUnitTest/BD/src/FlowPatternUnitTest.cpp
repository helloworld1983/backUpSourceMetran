#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "FlowPattern.h"
#include "DebugTrace.h"

using namespace ::testing;

FlowPattern::FlowPattern (void)
{
    TargetInhTime = 0;
    CompensatedPeakFlow = 0;
}   // end FlowPattern()

void FlowPattern::SetCompensatedPeakFlow (LONG setPeakFlow)
{

    // Set the CompensatedPeakFlow attribute to the passed argument
    CompensatedPeakFlow = setPeakFlow;

}   // end SetCompensatedPeakFlow()

LONG FlowPattern::GetCompensatedPeakFlow()
{

    // Set the CompensatedPeakFlow attribute to the passed argument
    return(CompensatedPeakFlow);

}   // end GetCompensatedPeakFlow()

class FlowPatternUnitMock : public BDTestFixture
{
public:
    FlowPatternUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~FlowPatternUnitMock(void){}
    virtual void SetUp(void)
    {
        MyFlowPattern = new FlowPattern();
    }
    virtual void TearDown(void)
    {
        delete MyFlowPattern;
    }
    FlowPattern *MyFlowPattern;
};


//test for FlowPattern
TEST_F(FlowPatternUnitMock, FlowPattern)
{
    EXPECT_EQ(MyFlowPattern->TargetInhTime, 0);
    EXPECT_EQ(MyFlowPattern->CompensatedPeakFlow, 0);
}

//test for void FlowPattern::SetCompensatedPeakFlow (LONG setPeakFlow)
TEST_F(FlowPatternUnitMock, SetCompensatedPeakFlow)
{
    MyFlowPattern->SetCompensatedPeakFlow(10);

    EXPECT_EQ(MyFlowPattern->CompensatedPeakFlow, 10);
}

//test for LONG FlowPattern::GetCompensatedPeakFlow()
TEST_F(FlowPatternUnitMock, GetCompensatedPeakFlow)
{
    MyFlowPattern->CompensatedPeakFlow = 10;

    EXPECT_EQ(MyFlowPattern->GetCompensatedPeakFlow(), 10);
}
#endif //end define BD_PHASE_UNIT_TEST
