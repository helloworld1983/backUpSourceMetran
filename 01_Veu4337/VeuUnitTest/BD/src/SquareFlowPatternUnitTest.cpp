#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "SquareFlowPattern.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "StepMotorMockHeader.h"

// VCV PropRefWeight constants; scaled by 2**7
static const LONG VCV_SQUARE_REF_WEIGHT = 122;   // .95 scaled by 2**7
static const LONG VCV_RAMP_REF_WEIGHT   = 125;   // .98 scaled by 2**7
static const LONG VCV_O2_REF_WEIGHT     = 128;   // 1.0 scaled by 2**7

// define statics
SquareFlowPattern* SquareFlowPattern::S_Instance = NULL;

SquareFlowPattern::SquareFlowPattern (void) :
        FlowPattern ()
{

}   // end SquareFlowPattern()
UNSIGNED SquareFlowPattern::CalcTargetInhTime (LONG tidalVol, LONG peakFlow)
{
    LONG inhTime;              // calculated inhalation time in msec

    ASSERTION(peakFlow > 0);

    // inhalation time is calculated as
    //
    //        V(t) ml |  sec    |  1000 ms
    //      ----------|---------|---------
    //                |  Qpf mL |  sec
    //
    // Note that both tidalVol and peakFlow are scaled, but these
    // scale factors cancel each other out in the division.
    //
    inhTime = (tidalVol*MS_PER_SEC) / (peakFlow); //in ms

    // Convert the Inhalation Time to TICKS and store the value in the
    // TargetInhTime attribute.
    TargetInhTime = (UNSIGNED) inhTime / MS_PER_TICK;

    // Return TargetInhTime in TICKS to the caller
    return(TargetInhTime);

}   // end CalcTargetInhTime()
void SquareFlowPattern::CalcCompensatedPeakFlow (LONG complCompVol)
{
    ASSERTION(TargetInhTime > 0);

    // Calculate the compliance compensated peak flow and store this
    // value in the CompensatedPeakFlow attribute.  CompensatedPeakFlow is
    // stored in ml/sec * SCALE, so the result must be converted.
    CompensatedPeakFlow = (complCompVol * TICKS_PER_SEC) / TargetInhTime;

}   // end CalcCompensatedPeakFlow()

void SquareFlowPattern::GetDesiredFlows (UNSIGNED, // not used by this flow pattern
        LONG* desiredAirPtr, LONG* desiredO2Ptr)
{

    // CalculateAirO2Flows will determine the desired BTPS compensated air
    // and oxygen flows.
//    (O2Setting::S_GetInstance ())->CalculateAirO2Flows (CompensatedPeakFlow,
//                                                        desiredAirPtr,
//                                                        desiredO2Ptr);

}   // end GetDesiredFlows()

SquareFlowPattern* SquareFlowPattern::S_GetInstance (void)
{

    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new SquareFlowPattern ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

class SquareFlowPatternUnitMock : public BDTestFixture
{
public:
    SquareFlowPatternUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~SquareFlowPatternUnitMock(void){}
    virtual void SetUp(void)
    {
        MySquareFlowPattern = new SquareFlowPattern();
    }
    virtual void TearDown(void)
    {
        delete MySquareFlowPattern;
    }
    SquareFlowPattern *MySquareFlowPattern;
};

//test for SquareFlowPattern::SquareFlowPattern (void) :
TEST_F(SquareFlowPatternUnitMock, SquareFlowPattern)
{
    //no source, not need to test
}

//test for UNSIGNED SquareFlowPattern::CalcTargetInhTime (LONG tidalVol, LONG peakFlow)
TEST_F(SquareFlowPatternUnitMock, CalcTargetInhTime)
{
    EXPECT_EQ(MySquareFlowPattern->CalcTargetInhTime(10, 10), 1000);
}

//test for void SquareFlowPattern::CalcCompensatedPeakFlow (LONG complCompVol)
TEST_F(SquareFlowPatternUnitMock, CalcCompensatedPeakFlow)
{
    MySquareFlowPattern->TargetInhTime = 1;

    MySquareFlowPattern->CalcCompensatedPeakFlow(10);

    EXPECT_EQ(MySquareFlowPattern->CompensatedPeakFlow, 10000);
}

//test for void SquareFlowPattern::GetDesiredFlows
TEST_F(SquareFlowPatternUnitMock, GetDesiredFlows)
{
    //all calling function is tested, not need to test
}

//test for SquareFlowPattern* SquareFlowPattern::S_GetInstance (void)
TEST_F(SquareFlowPatternUnitMock, S_GetInstance)
{
    MySquareFlowPattern->S_GetInstance();

    EXPECT_NE(MySquareFlowPattern->S_Instance, (void*)0);
}

#endif //end define BD_PHASE_UNIT_TEST
