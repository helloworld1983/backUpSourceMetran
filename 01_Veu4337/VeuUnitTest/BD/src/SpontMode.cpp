#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "SpontMode.h"
#include "DebugTrace.h"

using namespace ::testing;
#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "AlarmMockHeader.h"
#include "TriggerMgrMockHeader.h"

#include "StepMotorMockHeader.h"

// Define statics
SpontMode* SpontMode::S_Instance = NULL;
SpontMode* SpontMode::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new SpontMode ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

SpontMode::SpontMode (void) :
        NormBreathingMode ()
{

}   // end SpontMode()
void SpontMode::Enter (E_VentilationMode fromModeId)
{
    // Set state to on, and desired to off to make sure that this writen to the port, due to state and desired difference
    InpedanceLineSetState(eOn);
    InpedanceLineSetDesiredState(eOff);

    SafetyValveSetDesiredState(eClose);
    // Initialize pointer to the PhaseMgr object


//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtrDisableAllTriggers();
    // Enable triggers common to all breathing modes such as the Apnea Time
    // trigger and the Mode change trigger.

    triggerMgrPtrEnableIMVTrigger();

//    Btps::S_ComputeBtpsFactors();

//    MainHandlerDelegate::S_GetInstance()->postCustomEvent(eAmpAlarmAutoSetStopEvent);
}   // end Enter()

void SpontMode::ProcessPtInhTrigger (void)
{

    // Initialize pointer to the PhaseMgr object
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Patient trigger should only fire during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Start a spontaneous inhalation.
    phaseMgrPtrProcessNewPhase (eSpontInhPhase);

}   // end ProcessPtInhTrigger()
void SpontMode::ReportModeChangeInducedReset (void)
{

}   // end ReportModeChangeInducedReset()

void SpontMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case ePtInhTrigger:
            ProcessPtInhTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

void SpontMode::Exit(void)
{
    SafetyValveSetDesiredState(eOpen);
}
void SpontMode::ProcessModeChangeTrigger(void)
{
    ModeMgrGoToOperatorSetMode();
}

void SpontMode::ProcessBpTimeTrigger(void)
{
//    PhaseMgr* phaseMgrPtr;      // Pointer to the PhaseMgr object

    // Initialize pointer to the PhaseMgr object
//    phaseMgrPtr = PhaseMgr::S_GetInstance();
    phaseMgrPtrProcessNewPhase(eMandInhPhase);
}

class SpontModeUnitMock : public BDTestFixture
{
public:
    SpontModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~SpontModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MySpontMode = new SpontMode();
    }
    virtual void TearDown(void)
    {
        delete MySpontMode;
    }
    SpontMode *MySpontMode;
};

//test for SpontMode* SpontMode::S_GetInstance (void)
TEST_F(SpontModeUnitMock, S_GetInstance)
{
    MySpontMode->S_GetInstance();

    EXPECT_NE((void*)0, MySpontMode->S_Instance);
}

//test for SpontMode::SpontMode (void) :
TEST_F(SpontModeUnitMock, SpontMode)
{
    //no source, not need to test
}

//test for void SpontMode::Enter (E_VentilationMode fromModeId)
TEST_F(SpontModeUnitMock, Enter)
{
    EXPECT_CALL(*_StepMotor, InpedanceLineSetState(eOn)).Times(1);
    EXPECT_CALL(*_StepMotor, InpedanceLineSetDesiredState(eOff)).Times(1);

    EXPECT_CALL(*_StepMotor, SafetyValveSetDesiredState(eClose)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableAllTriggers()).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableIMVTrigger()).Times(1);

    MySpontMode->Enter(eHardwareErrorMode);
}

//test for void SpontMode::ProcessPtInhTrigger (void)
TEST_F(SpontModeUnitMock, ProcessPtInhTrigger)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eSpontInhPhase)).Times(1);

    MySpontMode->ProcessPtInhTrigger();
}

//test for void SpontMode::ReportModeChangeInducedReset (void)
TEST_F(SpontModeUnitMock, ReportModeChangeInducedReset)
{
    //no source code, not need to test
}

//test for void SpontMode::ProcessTriggerID (E_TriggerId id)
TEST_F(SpontModeUnitMock, ProcessTriggerID)
{
    //all calling function is tested, not need to test
}

//test for void SpontMode::Exit(void)
TEST_F(SpontModeUnitMock, Exit)
{
    EXPECT_CALL(*_StepMotor, SafetyValveSetDesiredState(eOpen)).Times(1);

    MySpontMode->Exit();
}

//test for void SpontMode::ProcessModeChangeTrigger(void)
TEST_F(SpontModeUnitMock, ProcessModeChangeTrigger)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToOperatorSetMode()).Times(1);

    MySpontMode->ProcessModeChangeTrigger();
}

//test for void SpontMode::ProcessBpTimeTrigger(void)
TEST_F(SpontModeUnitMock, ProcessBpTimeTrigger)
{
    EXPECT_CALL(*_PhaseMgrPtr,  phaseMgrPtrProcessNewPhase(eMandInhPhase)).Times(1);

    MySpontMode->ProcessBpTimeTrigger();
}
#endif //end define BD_MODE_UNIT_TEST
