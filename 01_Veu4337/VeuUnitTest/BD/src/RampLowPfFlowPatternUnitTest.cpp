#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "RampLowPfFlowPattern.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "DebugTrace.h"

// define statics
RampLowPfFlowPattern* RampLowPfFlowPattern::S_Instance = NULL;

RampLowPfFlowPattern* RampLowPfFlowPattern::S_GetInstance (void)
{
    // If the object has not already been instantiated, instantiated it
    if(NULL == S_Instance)
    {
        S_Instance = new RampLowPfFlowPattern ();
        ASSERTION(S_Instance != NULL);
    }

    // Return a pointer to the object
    return (S_Instance);

}   // end S_GetInstance()
RampLowPfFlowPattern::RampLowPfFlowPattern (void) :
        FlowPattern ()
{

}   // end RampLowPfFlowPattern()
UNSIGNED RampLowPfFlowPattern::CalcTargetInhTime (LONG tidalVol, LONG peakFlow)
{
    LONG inhTime;              // calculated inhalation time in msec

    // For set Peak Flows less than 10 Lpm, inhalation time is calculated as
    //
    //      4* V(t) ml|    sec       | 1000 ms
    //      ----------|--------------|---------
    //                |   3*Qpf mL   |  sec

    ASSERTION(peakFlow > 0);
    inhTime = (4000 * tidalVol) / (3 * peakFlow);

    // Convert the Inhalation Time to TICKS and store the value in the
    // TargetInhTime attribute
    TargetInhTime = (UNSIGNED) inhTime / MS_PER_TICK;

    // Return TargetInhTime in TICKS to the caller
    return (TargetInhTime);

}   // end CalcTargetInhTime()
void RampLowPfFlowPattern::CalcCompensatedPeakFlow (LONG complCompVol)
{
    ASSERTION(TargetInhTime > 0);

    // Calculate the compliance compensated peak flow and store this
    // value in the CompensatedPeakFlow attribute.  CompensatedPeakFlow is
    // stored in ml/sec * SCALE, so the result must be converted

    CompensatedPeakFlow = (4 * complCompVol * TICKS_PER_SEC)
            / (3 * TargetInhTime);

}   // end CalcCompensatedPeakFlow()
LONG RampLowPfFlowPattern::GetDesiredFlows (int iTime)
{
    LONG adjustedTime;
    LONG totalFlow;     // in ml/sec * SCALE

    // Put a 60ms shelf on the ramp to allow time to reach the peak
    adjustedTime = iTime - (60 / MS_PER_TICK);

    if(adjustedTime < 0)
        totalFlow = CompensatedPeakFlow;
    else
    {
        totalFlow = (((0 - CompensatedPeakFlow) * (LONG) adjustedTime)
                / (2 * (LONG) TargetInhTime)) + CompensatedPeakFlow;

        if(CompensatedPeakFlow < RAMP_PEAK_FLOW_THRESHOLD)
        {
            totalFlow = (totalFlow * RAMP_TARGET_MULT) >> RAMP_TARGET_SHIFT;
        }
        else
        {
            totalFlow = (totalFlow * RAMP_TARGET_MULT_HI) >> RAMP_TARGET_SHIFT;
        }
    }

    return totalFlow;
    // CalculateAirO2Flows will determine the desired BTPS compensated air
    // and oxygen flows.
//    (O2Setting::S_GetInstance ())->CalculateAirO2Flows (totalFlow,
//                                                        desiredAirPtr,
//                                                        desiredO2Ptr);

}   // end GetDesiredFlows()

class RampLowPfFlowPatternUnitMock : public BDTestFixture
{
public:
    RampLowPfFlowPatternUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~RampLowPfFlowPatternUnitMock(void){}
    virtual void SetUp(void)
    {
        MyRampLowPfFlowPattern = new RampLowPfFlowPattern();
    }
    virtual void TearDown(void)
    {
        delete MyRampLowPfFlowPattern;
    }
    RampLowPfFlowPattern *MyRampLowPfFlowPattern;
};

//test for RampLowPfFlowPattern* RampLowPfFlowPattern::S_GetInstance (void)
TEST_F(RampLowPfFlowPatternUnitMock, S_GetInstance)
{
    MyRampLowPfFlowPattern->S_GetInstance();

    EXPECT_NE(MyRampLowPfFlowPattern->S_Instance, (void*)0);
}

//test for RampLowPfFlowPattern::RampLowPfFlowPattern (void)
TEST_F(RampLowPfFlowPatternUnitMock, RampLowPfFlowPattern)
{
    //no source, not need to test
}

//test for UNSIGNED RampLowPfFlowPattern::CalcTargetInhTime (LONG tidalVol, LONG peakFlow)
TEST_F(RampLowPfFlowPatternUnitMock, CalcTargetInhTime)
{
    EXPECT_EQ(MyRampLowPfFlowPattern->CalcTargetInhTime(10, 10), 1333);
}

//test for void RampLowPfFlowPattern::CalcCompensatedPeakFlow (LONG complCompVol)
TEST_F(RampLowPfFlowPatternUnitMock, CalcCompensatedPeakFlow)
{
    MyRampLowPfFlowPattern->TargetInhTime = 10;

    MyRampLowPfFlowPattern->CalcCompensatedPeakFlow(100);

    EXPECT_EQ(MyRampLowPfFlowPattern->CompensatedPeakFlow, 13333);
}

//test for LONG RampLowPfFlowPattern::GetDesiredFlows (int iTime)
TEST_F(RampLowPfFlowPatternUnitMock, GetDesiredFlows_0)
{
    MyRampLowPfFlowPattern->CompensatedPeakFlow = 10;

    EXPECT_EQ(MyRampLowPfFlowPattern->GetDesiredFlows(0), 10);
}
TEST_F(RampLowPfFlowPatternUnitMock, GetDesiredFlows_1)
{
    MyRampLowPfFlowPattern->CompensatedPeakFlow = RAMP_PEAK_FLOW_THRESHOLD - 100;
    MyRampLowPfFlowPattern->TargetInhTime = 1;

    EXPECT_EQ(MyRampLowPfFlowPattern->GetDesiredFlows(100), -1994488);
}
TEST_F(RampLowPfFlowPatternUnitMock, GetDesiredFlows_2)
{
    MyRampLowPfFlowPattern->CompensatedPeakFlow = RAMP_PEAK_FLOW_THRESHOLD + 100;
    MyRampLowPfFlowPattern->TargetInhTime = 1;

    EXPECT_EQ(MyRampLowPfFlowPattern->GetDesiredFlows(100), -1961335);
}

#endif //end define BD_PHASE_UNIT_TEST
