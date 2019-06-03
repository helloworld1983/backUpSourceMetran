#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "PlateauPhase.h"
#include "DebugTrace.h"

using namespace ::testing;
#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "StepMotorMockHeader.h"

// Define statics
PlateauPhase* PlateauPhase::S_Instance = NULL;

PlateauPhase* PlateauPhase::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new PlateauPhase ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

PlateauPhase::PlateauPhase (void) :
        Phase ()
{

}   // end PlateauPhase()
void PlateauPhase::Start (void)
{
    E_BreathPhaseIndicator breathType;
    breathType = ePlateau;
    // Enable the plateau trigger
    TriggerMgrEnableTrigger(ePlateauTimeTrigger);

    // Notify the Breath Data Manager of the start of plateau.  MgrBreathData
    // is then responsible for notifying the GUI of the plateau phase.
    MgrBreathDataPlateauStart(breathType);

    MgrControllerVCVServoControllerSetSepecificPhase();

}   // end Start()
void PlateauPhase::End (void)
{
    // Disable the plateau trigger
    TriggerMgrDisableTrigger (ePlateauTimeTrigger);

}   // end End()
void PlateauPhase::ProcessBdCycle (void)
{

}   // end ProcessBdCycle()

class PlateauPhaseUnitMock : public BDTestFixture
{
public:
    PlateauPhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~PlateauPhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MyPlateauPhase = new PlateauPhase();
    }
    virtual void TearDown(void)
    {
        delete MyPlateauPhase;
    }
    PlateauPhase *MyPlateauPhase;
};

//test for PlateauPhase* PlateauPhase::S_GetInstance (void)
TEST_F(PlateauPhaseUnitMock, S_GetInstance)
{
    MyPlateauPhase->S_GetInstance();

    EXPECT_NE(MyPlateauPhase->S_Instance, (void*)0);

}

//test for PlateauPhase::PlateauPhase (void) :
TEST_F(PlateauPhaseUnitMock, PlateauPhase)
{
    //no source, not need to test
}

//test for void PlateauPhase::Start (void)
TEST_F(PlateauPhaseUnitMock, Start)
{
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(ePlateauTimeTrigger)).Times(1);

    EXPECT_CALL(*_BreathMgr, MgrBreathDataPlateauStart(ePlateau)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerVCVServoControllerSetSepecificPhase()).Times(1);

    MyPlateauPhase->Start();
}

//test for void PlateauPhase::End (void)
TEST_F(PlateauPhaseUnitMock, End)
{
    EXPECT_CALL(*_TriggerMgr, TriggerMgrDisableTrigger (ePlateauTimeTrigger)).Times(1);

    MyPlateauPhase->End();
}

//test for void PlateauPhase::ProcessBdCycle (void)
TEST_F(PlateauPhaseUnitMock, ProcessBdCycle)
{
    //no source, not need to test
}

#endif //end define BD_PHASE_UNIT_TEST
