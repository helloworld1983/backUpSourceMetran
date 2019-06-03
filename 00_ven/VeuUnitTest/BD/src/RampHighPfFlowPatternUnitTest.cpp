#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"

#include "DebugTrace.h"
#include "RampHighPfFlowPattern.h"

using namespace ::testing;

// define statics
RampHighPfFlowPattern* RampHighPfFlowPattern::S_Instance = NULL;

// 5 Lpm converted to ml/sec
static const LONG HALF_PF_THRESHOLD_IN_SCALED_MLS = (5 * 1000 * SCALE) / 60;

RampHighPfFlowPattern* RampHighPfFlowPattern::S_GetInstance (void)
{
    // If the object has not already been instantiated, instantiated it
    if(NULL == S_Instance)
    {
        S_Instance = new RampHighPfFlowPattern ();
        ASSERTION(S_Instance != NULL);
    }

    // Return a pointer to the object
    return (S_Instance);

}   // end S_GetInstance()
RampHighPfFlowPattern::RampHighPfFlowPattern (void) :
        FlowPattern ()
{

}   // end RampHighPfFlowPattern()

UNSIGNED RampHighPfFlowPattern::CalcTargetInhTime (LONG tidalVol, LONG peakFlow)
{
    LONG inhTime;              // calculated inhalation time in msec

    // For set Peak Flows greater than or equal to 10 Lpm,
    // inhalation time is calculated as
    //
    //      2* V(t) ml|        sec       | 1000 ms
    //      ----------|------------------|---------
    //                | (Qpf + 83.33) mL |  sec
    //
    // Note that both tidalVol and peakFlow are scaled by 100, but these
    // scale factors cancel each other out in the division.
    //
    ASSERTION(peakFlow > 0);
    inhTime = (2000 * tidalVol) / (peakFlow + HALF_PF_THRESHOLD_IN_SCALED_MLS);

    // Convert the Inhalation Time to TICKS and store the value in the
    // TargetInhTime attribute
    TargetInhTime = (UNSIGNED) inhTime / MS_PER_TICK;

    // Return TargetInhTime in TICKS to the caller
    return (TargetInhTime);

}   // end CalcTargetInhTime()
void RampHighPfFlowPattern::CalcCompensatedPeakFlow (LONG complCompVol)
{
    ASSERTION(TargetInhTime > 0);

    // Calculate the compliance compensated peak flow and store this
    // value in the CompensatedPeakFlow attribute.  CompensatedPeakFlow is
    // stored in ml/sec * SCALE, so the result must be converted
    CompensatedPeakFlow = ((2 * complCompVol * TICKS_PER_SEC) / TargetInhTime)
            - HALF_PF_THRESHOLD_IN_SCALED_MLS;

}   // end CalcCompensatedPeakFlow()
LONG RampHighPfFlowPattern::GetDesiredFlows (int iTime)
{
    LONG adjustedTime;
    LONG totalFlow;     // in ml/sec * SCALE

    // Put a 60ms shelf on the ramp to allow time to reach the peak
    adjustedTime = iTime - (60 / MS_PER_TICK);

    if(adjustedTime < 0)
        totalFlow = CompensatedPeakFlow;
    else
    {

        totalFlow = (((HALF_PF_THRESHOLD_IN_SCALED_MLS - CompensatedPeakFlow)
                * (LONG) iTime) / (LONG) TargetInhTime) + CompensatedPeakFlow;

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

class RampHighPfFlowPatternUnitMock : public BDTestFixture
{
public:
    RampHighPfFlowPatternUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~RampHighPfFlowPatternUnitMock(void){}
    virtual void SetUp(void)
    {
        MyRampHighPfFlowPattern = new RampHighPfFlowPattern();
    }
    virtual void TearDown(void)
    {
        delete MyRampHighPfFlowPattern;
    }
    RampHighPfFlowPattern *MyRampHighPfFlowPattern;
};

//test for RampHighPfFlowPattern* RampHighPfFlowPattern::S_GetInstance (void)
TEST_F(RampHighPfFlowPatternUnitMock, S_GetInstance)
{
    MyRampHighPfFlowPattern->S_GetInstance();

    EXPECT_NE(MyRampHighPfFlowPattern->S_Instance, (void*)0);
}

//test for RampHighPfFlowPattern::RampHighPfFlowPattern (void) :
TEST_F(RampHighPfFlowPatternUnitMock, RampHighPfFlowPattern)
{
    //no source, not need to test
}

//test for UNSIGNED RampHighPfFlowPattern::CalcTargetInhTime (LONG tidalVol, LONG peakFlow)
TEST_F(RampHighPfFlowPatternUnitMock, CalcTargetInhTime)
{
    EXPECT_EQ(MyRampHighPfFlowPattern->CalcTargetInhTime(10 , 10), 2);
}

//test for void RampHighPfFlowPattern::CalcCompensatedPeakFlow (LONG complCompVol)
TEST_F(RampHighPfFlowPatternUnitMock, CalcCompensatedPeakFlow)
{
    MyRampHighPfFlowPattern->CalcTargetInhTime(10 , 10);
    MyRampHighPfFlowPattern->CalcCompensatedPeakFlow(100);

    EXPECT_EQ(MyRampHighPfFlowPattern->CompensatedPeakFlow, 91667);
}

//test for LONG RampHighPfFlowPattern::GetDesiredFlows (int iTime)
TEST_F(RampHighPfFlowPatternUnitMock, GetDesiredFlows_0)
{
    MyRampHighPfFlowPattern->CompensatedPeakFlow = 10;

    EXPECT_EQ(MyRampHighPfFlowPattern->GetDesiredFlows(0), 10);
}
TEST_F(RampHighPfFlowPatternUnitMock, GetDesiredFlows_1)
{
    MyRampHighPfFlowPattern->CompensatedPeakFlow = RAMP_PEAK_FLOW_THRESHOLD - 100;

    EXPECT_EQ(MyRampHighPfFlowPattern->GetDesiredFlows(0), 99900);
}
TEST_F(RampHighPfFlowPatternUnitMock, GetDesiredFlows_2)
{
    MyRampHighPfFlowPattern->CompensatedPeakFlow = RAMP_PEAK_FLOW_THRESHOLD + 100;

    EXPECT_EQ(MyRampHighPfFlowPattern->GetDesiredFlows(0), 100100);
}

#endif //end define BD_PHASE_UNIT_TEST
