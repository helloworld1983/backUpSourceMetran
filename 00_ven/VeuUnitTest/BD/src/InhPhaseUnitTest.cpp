#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "InhPhase.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "TriggerMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"
#include "PhaseMgrMockHeader.h"

// define statics
UNSIGNED InhPhase::S_InhStartTime = 0;
InhPhase::InhPhase (void) :
        Phase ()
{

}   // end InhPhase()

UNSIGNED InhPhase::S_CalcTimeSincePhaseStart (void)
{
    // Return the time elapsed since the start of the inhalation phase.
//    return (CalcElapsedTime (S_InhStartTime));
    return 100; //for test

}   // end S_CalcTimeSincePhaseStart()
void InhPhase::Start (void)
{
    // Initialize a pointer to the TriggerMgr
    TriggerMgrEnableInhaleTrigger();
//    S_InhStartTime = SystemClock::Retrieve();

}   // end Start()

E_Bool InhPhase::S_IsInUnrestrictedPhase(UNSIGNED minInhTime)
{

    E_Bool rtnValue = eFalse;
    E_PhaseId phase = phaseMgrPtrGetCurrentPhaseId();

    // If the breath phase is exhalation and the time elapsed since the start
    // of exhalation is greater than or equal to the minimum exhalation time,
    // return TRUE because the ventilator is in the unrestricted phase.
    if (((phase == eMandInhPhase)||(phase == eSpontInhPhase))
            &&(S_CalcTimeSincePhaseStart() >= minInhTime))
    {
        rtnValue = eTrue;
    }

    return(rtnValue);

}   // end S_IsInUnrestrictedPhase()

class InhPhaseUnitMock : public BDTestFixture
{
public:
    InhPhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~InhPhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MyInhPhase = new InhPhase();
    }
    virtual void TearDown(void)
    {
        delete MyInhPhase;
    }
    InhPhase *MyInhPhase;
};

//test for InhPhase::InhPhase (void) :
TEST_F(InhPhaseUnitMock, InhPhase)
{
    //no source, not need to test
}

//test for UNSIGNED InhPhase::S_CalcTimeSincePhaseStart (void)
TEST_F(InhPhaseUnitMock, S_CalcTimeSincePhaseStart)
{
    //call calling function is tested, not need to test
}

//test for void InhPhase::Start (void)
TEST_F(InhPhaseUnitMock, Start)
{
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableInhaleTrigger()).Times(1);

    MyInhPhase->Start();
}

//test for E_Bool InhPhase::S_IsInUnrestrictedPhase(UNSIGNED minInhTime)
TEST_F(InhPhaseUnitMock, S_IsInUnrestrictedPhase_0)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1).WillOnce(Return(eMandInhVABreathPhase));

    EXPECT_EQ(MyInhPhase->S_IsInUnrestrictedPhase(), eFalse);
}
TEST_F(InhPhaseUnitMock, S_IsInUnrestrictedPhase_1)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1).WillOnce(Return(eMandInhPhase));

    EXPECT_EQ(MyInhPhase->S_IsInUnrestrictedPhase(50), eTrue);
}
TEST_F(InhPhaseUnitMock, S_IsInUnrestrictedPhase_2)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1).WillOnce(Return(eSpontInhPhase));

    EXPECT_EQ(MyInhPhase->S_IsInUnrestrictedPhase(50), eTrue);
}

#endif //end define BD_PHASE_UNIT_TEST
