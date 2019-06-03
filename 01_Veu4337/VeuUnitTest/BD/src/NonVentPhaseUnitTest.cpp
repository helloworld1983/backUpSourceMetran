#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "NonVentPhase.h"
#include "DebugTrace.h"

#include "BreathMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"

using namespace ::testing;

UNSIGNED NonVentPhase::S_StartTime = 0;
NonVentPhase* NonVentPhase::S_Instance = NULL;

UNSIGNED NonVentPhase::S_CalcTimeSincePhaseStart (void)
{

    // Return the time elapsed since the start of the exhalation phase.
//    return (CalcElapsedTime (S_StartTime));

}   // end S_CalcTimeSincePhaseStart()
NonVentPhase* NonVentPhase::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new NonVentPhase ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()
NonVentPhase::NonVentPhase (void) :
        Phase ()
{

}   // end NonVentPhase()
void NonVentPhase::Start (void)
{
    // Update Patient Data
    MgrBreathDataNonVentStart ();

    // Update Patient Data
    MgrBreathDataNonVentStart();

}   // end Start()
void NonVentPhase::End (void)
{

}   // end End()
void NonVentPhase::ProcessBdCycle (void)
{

}   // end ProcessBdCycle()
E_Bool NonVentPhase::S_IsInUnrestrictedPhase (UNSIGNED minExhTime)
{
    E_Bool rtnValue = eFalse;

    // If the breath phase is exhalation and the time elapsed since the start
    // of exhalation is greater than or equal to the minimum exhalation time,
    // return TRUE because the ventilator is in the unrestricted phase.
    if((phaseMgrPtrGetCurrentPhaseId () == eAlarmPhase)
            && (NonVentPhaseS_CalcTimeSincePhaseStart () >= minExhTime))
    {
        rtnValue = eTrue;
    }

    return (rtnValue);

}   // end S_IsInUnrestrictedPhase()

class NonVentPhaseUnitMock : public BDTestFixture
{
public:
    NonVentPhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~NonVentPhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MyNonVentPhase = new NonVentPhase();
    }
    virtual void TearDown(void)
    {
        delete MyNonVentPhase;
    }
    NonVentPhase *MyNonVentPhase;
};

//test for NonVentPhase* NonVentPhase::S_GetInstance (void)
TEST_F(NonVentPhaseUnitMock, S_GetInstance)
{
    MyNonVentPhase->S_GetInstance();

    EXPECT_NE(MyNonVentPhase->S_Instance, (void*)0);
}

//test for UNSIGNED NonVentPhase::S_CalcTimeSincePhaseStart (void)
TEST_F(NonVentPhaseUnitMock, S_CalcTimeSincePhaseStart)
{
    //calling function is tested, not need to test
}

//test for NonVentPhase::NonVentPhase (void) :
TEST_F(NonVentPhaseUnitMock, NonVentPhase)
{
    //no source, not need to test
}

//test for void NonVentPhase::Start (void)
TEST_F(NonVentPhaseUnitMock, Start)
{
    EXPECT_CALL(*_BreathMgr, MgrBreathDataNonVentStart()).Times(2);

    MyNonVentPhase->Start();
}

//test for void NonVentPhase::End (void)
TEST_F(NonVentPhaseUnitMock, End)
{
   //no source, not need to test
}

//test for void NonVentPhase::ProcessBdCycle (void)
TEST_F(NonVentPhaseUnitMock, ProcessBdCycle)
{
   //no source, not need to test
}

//test for E_Bool NonVentPhase::S_IsInUnrestrictedPhase (UNSIGNED minExhTime)
TEST_F(NonVentPhaseUnitMock, S_IsInUnrestrictedPhase_0)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1).WillOnce(Return(eAlarmPhase));

    EXPECT_CALL(*_PhaseMgrPtr, NonVentPhaseS_CalcTimeSincePhaseStart()).Times(1).WillOnce(Return(1));

    EXPECT_EQ(MyNonVentPhase->S_IsInUnrestrictedPhase(0), eTrue);
}

TEST_F(NonVentPhaseUnitMock, S_IsInUnrestrictedPhase_1)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1).WillOnce(Return(eMandInhVABreathPhase));

    EXPECT_EQ(MyNonVentPhase->S_IsInUnrestrictedPhase(0), eFalse);
}

#endif //end define BD_PHASE_UNIT_TEST
