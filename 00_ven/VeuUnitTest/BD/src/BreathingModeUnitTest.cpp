#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "DebugTrace.h"

#include "BreathingMode.h"

#include "PhaseMgrMockHeader.h"
#include "ModeMgrMockHeader.h"

using namespace ::testing;

BreathingMode::BreathingMode (E_Bool doesManualReset) :
        Mode (doesManualReset)
{

}   // end BreathingMode()

void BreathingMode::ProcessVcvInhTimeTrigger (void)
{
    // Get a handle to the PhaseMgr
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // The trigger should only fire in the mandatory inhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eMandInhPhase);

    // Assume that the ventilator will transition to plateau
    // from a VCV inhalation.  It is possible that the ventilator
    // will not go to plateau but instead will go to exhalation.
    // The phase manager takes care of that.
    phaseMgrPtrProcessNewPhase (ePlateauPhase);

}   // end ProcessVcvInhTimeTrigger()

void BreathingMode::ProcessMandPressInhTimeTrigger (void)
{
    // Get a pointer to the PhaseMgr object
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // If the breath phase is a mandatory inhalation, transition to exhalation.
    // Otherwise, perform error handling.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eMandInhPhase);
    phaseMgrPtrProcessNewPhase (eExhPhase);

}   // end ProcessMandPressInhTimeTrigger()

void BreathingMode::ProcessHipAlarmTrigger (void)
{
    // Get a pointer to the PhaseMgr
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Get the current breath phase to be used to check that triggers only
    // fire when they are supposed to fire
    E_PhaseId breathPhase = phaseMgrPtrGetCurrentPhaseId ();

    // Make sure the ventilator is in inhalation or plateau
    if((eMandInhPhase == breathPhase) || (eSpontInhPhase == breathPhase)
            || (ePlateauPhase == breathPhase))
    {
        phaseMgrPtrProcessNewPhase (eExhPhase);
    }
    else
    {
        // HIP Trigger in wrong phase
        ASSERTION(eFalse);
    }

}   // end ProcessHipAlarmTrigger()

void BreathingMode::ProcessPlateauTimeTrigger (void)
{
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Plateau Time Trigger is only valid in the Plateau Phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == ePlateauPhase);

    // Transition to the exhalation phase.
    phaseMgrPtrProcessNewPhase (eExhPhase);

}   // end ProcessPlateauTimeTrigger()

void BreathingMode::ProcessManualInhOpReqTrigger (void)
{
}   // end ProcessManualInhOpReqTrigger()

void BreathingMode::ProcessTotalGasLossTrigger (void)
{

    // Transition to the No Gas Mode.
    ModeMgrGoToAlarmMode (eNoGasMode);

}   // end ProcessTotalGasLossTrigger()

void BreathingMode::ProcessOcclusionAlarmTrigger (void)
{

    // Transition to the occlusion mode.
    ModeMgrGoToAlarmMode (eOcclusionMode);

}   // end ProcessOcclusionAlarmTrigger()

void BreathingMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eVcvInhTimeTrigger:
            ProcessVcvInhTimeTrigger();
            break;

        case eMandPressInhTimeTrigger:
            ProcessMandPressInhTimeTrigger();
            break;

        case ePlateauTimeTrigger:
            ProcessPlateauTimeTrigger();
            break;

        case eManualInhOpReqTrigger:
            ProcessManualInhOpReqTrigger();
            break;

        case eHipAlarmTrigger:
            ProcessHipAlarmTrigger();
            break;

        case eTotalGasLossTrigger:
            ProcessTotalGasLossTrigger();
            break;

        case eOcclusionAlarmTrigger:
            ProcessOcclusionAlarmTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

void BreathingMode::ProcessDisconnectionAlarmTrigger(void)
{
    // Transition to Disconnect Mode
    ModeMgrGoToAlarmMode(eDisconnectionMode);

}   // end ProcessDisconnectionAlarmTrigger()

void BreathingMode::ProcessHardwareErrorAlarmTrigger(void)
{
    ModeMgrGoToAlarmMode(eHardwareErrorMode);
}

class BreathModeUnitMock : public BDTestFixture
{
public:
    BreathModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~BreathModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyBreathingMode = new BreathingMode(eTrue);
    }
    virtual void TearDown(void)
    {
        delete MyBreathingMode;
    }
    BreathingMode *MyBreathingMode;
};

//test for BreathingMode::BreathingMode (E_Bool doesManualReset)
TEST_F(BreathModeUnitMock, BreathingMode)
{
    //no source to test
}

//test for void BreathingMode::ProcessVcvInhTimeTrigger (void)
TEST_F(BreathModeUnitMock, ProcessVcvInhTimeTrigger)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (ePlateauPhase)).Times(1);

    MyBreathingMode->ProcessVcvInhTimeTrigger();
}

//test for void BreathingMode::ProcessMandPressInhTimeTrigger (void)
TEST_F(BreathModeUnitMock, ProcessMandPressInhTimeTrigger)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eExhPhase)).Times(1);

    MyBreathingMode->ProcessMandPressInhTimeTrigger();
}

//test for void BreathingMode::ProcessHipAlarmTrigger (void)
TEST_F(BreathModeUnitMock, ProcessHipAlarmTrigger_0)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId ()).Times(1).WillOnce(Return(eMandInhPhase));

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eExhPhase)).Times(1);

    MyBreathingMode->ProcessHipAlarmTrigger();
}
TEST_F(BreathModeUnitMock, ProcessHipAlarmTrigger_1)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId ()).Times(1).WillOnce(Return(eSpontInhPhase));

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eExhPhase)).Times(1);

    MyBreathingMode->ProcessHipAlarmTrigger();
}
TEST_F(BreathModeUnitMock, ProcessHipAlarmTrigger_2)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId ()).Times(1).WillOnce(Return(ePlateauPhase));

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eExhPhase)).Times(1);

    MyBreathingMode->ProcessHipAlarmTrigger();
}

//test for void BreathingMode::ProcessPlateauTimeTrigger (void)
TEST_F(BreathModeUnitMock, ProcessPlateauTimeTrigger)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eExhPhase)).Times(1);

    MyBreathingMode->ProcessPlateauTimeTrigger();
}

//test for void BreathingMode::ProcessManualInhOpReqTrigger (void)
TEST_F(BreathModeUnitMock, ProcessManualInhOpReqTrigger)
{
    //no source code to test
}

//test for void BreathingMode::ProcessTotalGasLossTrigger (void)
TEST_F(BreathModeUnitMock, ProcessTotalGasLossTrigger)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToAlarmMode (eNoGasMode)).Times(1);

    MyBreathingMode->ProcessTotalGasLossTrigger();
}

//test for void BreathingMode::ProcessOcclusionAlarmTrigger (void)
TEST_F(BreathModeUnitMock, ProcessOcclusionAlarmTrigger)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToAlarmMode (eOcclusionMode)).Times(1);

    MyBreathingMode->ProcessOcclusionAlarmTrigger();
}

//test for void BreathingMode::ProcessTriggerID (E_TriggerId id)
TEST_F(BreathModeUnitMock, ProcessTriggerID)
{
    //call all function tested in class > no test
}

//test for void BreathingMode::ProcessDisconnectionAlarmTrigger(void)
TEST_F(BreathModeUnitMock, ProcessDisconnectionAlarmTrigger)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToAlarmMode(eDisconnectionMode)).Times(1);

    MyBreathingMode->ProcessDisconnectionAlarmTrigger();
}

//test for void BreathingMode::ProcessHardwareErrorAlarmTrigger(void)
TEST_F(BreathModeUnitMock, ProcessHardwareErrorAlarmTrigger)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToAlarmMode(eHardwareErrorMode)).Times(1);

    MyBreathingMode->ProcessHardwareErrorAlarmTrigger();
}

#endif //end define BD_MODE_UNIT_TEST
