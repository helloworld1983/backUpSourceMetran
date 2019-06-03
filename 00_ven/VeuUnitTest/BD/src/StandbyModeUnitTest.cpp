#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "StandbyMode.h"
#include "DebugTrace.h"

#include "TriggerMgrMockHeader.h"

#include "StepMotorMockHeader.h"
#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "PdTaskMockHeader.h"

#include "PdInterface.h"

using namespace ::testing;

// Initialize statics
StandbyMode* StandbyMode::S_Instance = NULL;

StandbyMode* StandbyMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new StandbyMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()
StandbyMode::StandbyMode(void):Mode()
{

}   // end StandbyMode()

void StandbyMode::Enter(E_VentilationMode fromModeId)
{
    PurgeFlowSetStateDirect(eOff);
    OilPumpSetDesiredState(eOff); //close OilPump

    // Set state to on, and desired to off to make sure that this writen to the port, due to state and desired difference
    InpedanceLineSetState(eOn);
    InpedanceLineSetDesiredState(eOff);
//    HfoController->Disable();

    // Initialize pointer to the TriggerMgr
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    SafetyValveSetDesiredState(eOpen);

    triggerMgrPtrDisableAllTriggers();
    // Enable the mode change trigger.
    triggerMgrPtrEnableTrigger(eModeChangeTrigger);


    PhaseMgrProcessNewPhase(eDiagnosticPhase);
//
//    PInhAzTestPtr->SuspendTest();
//    PExhAzTestPtr->SuspendTest();
//
//    //blank all patient data when enter standbymode
    PdTaskS_SetEventFlag(NONBREATHING_MODE_EVENT_FLAG);
//    MainHandlerDelegate::S_GetInstance()->postCustomEvent(eAmpAlarmAutoSetStopEvent);

    HfoValveDisableMotorErrorCheck();
    O2ValveDisableMotorErrorCheck();
    AirValveDisableMotorErrorCheck();
    ExhValveDisableMotorErrorCheck();
//    WatchDogTimer::S_GetInstance()->DisableWatchDog();
}   // end Enter()
void StandbyMode::Exit(void)
{
    PdTaskS_SetEventFlag(EXIT_STANDBYMODE_FLAG);
    SafetyValveSetDesiredState(eOpen);
    PurgeFlowSetDesiredState(eOn);

//    PInhAzTestPtr->ResumeTest();
//    PExhAzTestPtr->ResumeTest();

    HfoValveEnableMotorErrorCheck();
    O2ValveEnableMotorErrorCheck();
    AirValveEnableMotorErrorCheck();
    ExhValveEnableMotorErrorCheck();

//    WatchDogTimer::S_GetInstance()->EnableWatchDog();

}   // end Exit()
void StandbyMode::UpdateAlarmStatus(void)
{
    // This operation should never be called.
    ASSERTION(eFalse);

}  // end UpdateAlarmStatus()
void StandbyMode::ProcessModeChangeTrigger(void)
{
    ModeMgrGoToOperatorSetMode();
}
void StandbyMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eModeChangeTrigger:
            ProcessModeChangeTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

class StandbyModeUnitMock : public BDTestFixture
{
public:
    StandbyModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~StandbyModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyStandbyMode = new StandbyMode();
    }
    virtual void TearDown(void)
    {
        delete MyStandbyMode;
    }
    StandbyMode *MyStandbyMode;
};

//test for StandbyMode* StandbyMode::S_GetInstance(void)
TEST_F(StandbyModeUnitMock, S_GetInstance)
{
    MyStandbyMode->S_GetInstance();

    EXPECT_NE((void*)0, MyStandbyMode->S_Instance);
}

//test for StandbyMode::StandbyMode(void):Mode()
TEST_F(StandbyModeUnitMock, StandbyMode)
{
    //no source, not need to test
}

//test for void StandbyMode::Enter(E_VentilationMode fromModeId)
TEST_F(StandbyModeUnitMock, Enter)
{
    EXPECT_CALL(*_StepMotor, PurgeFlowSetStateDirect(eOff)).Times(1);
    EXPECT_CALL(*_StepMotor, OilPumpSetDesiredState(eOff)).Times(1);

    EXPECT_CALL(*_StepMotor, InpedanceLineSetState(eOn)).Times(1);
    EXPECT_CALL(*_StepMotor, InpedanceLineSetDesiredState(eOff)).Times(1);

    EXPECT_CALL(*_StepMotor, SafetyValveSetDesiredState(eOpen)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableAllTriggers()).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eModeChangeTrigger)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eDiagnosticPhase)).Times(1);

    EXPECT_CALL(*_PdTask, PdTaskS_SetEventFlag(NONBREATHING_MODE_EVENT_FLAG)).Times(1);

    EXPECT_CALL(*_StepMotor, HfoValveDisableMotorErrorCheck()).Times(1);
    EXPECT_CALL(*_StepMotor, O2ValveDisableMotorErrorCheck()).Times(1);
    EXPECT_CALL(*_StepMotor, AirValveDisableMotorErrorCheck()).Times(1);
    EXPECT_CALL(*_StepMotor, ExhValveDisableMotorErrorCheck()).Times(1);

    MyStandbyMode->Enter(eFirstCommonMode);
}

//test for void StandbyMode::Exit(void)
TEST_F(StandbyModeUnitMock, Exit)
{
    EXPECT_CALL(*_PdTask, PdTaskS_SetEventFlag(EXIT_STANDBYMODE_FLAG)).Times(1);

    EXPECT_CALL(*_StepMotor, SafetyValveSetDesiredState(eOpen)).Times(1);
    EXPECT_CALL(*_StepMotor, PurgeFlowSetDesiredState(eOn)).Times(1);

    EXPECT_CALL(*_StepMotor, HfoValveEnableMotorErrorCheck()).Times(1);
    EXPECT_CALL(*_StepMotor, O2ValveEnableMotorErrorCheck()).Times(1);
    EXPECT_CALL(*_StepMotor, AirValveEnableMotorErrorCheck()).Times(1);
    EXPECT_CALL(*_StepMotor, ExhValveEnableMotorErrorCheck()).Times(1);

    MyStandbyMode->Exit();
}

//test for void StandbyMode::UpdateAlarmStatus(void)
TEST_F(StandbyModeUnitMock, UpdateAlarmStatus)
{
    //function is never called, not need to test
}

//test for void StandbyMode::ProcessModeChangeTrigger(void)
TEST_F(StandbyModeUnitMock, ProcessModeChangeTrigger)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToOperatorSetMode()).Times(1);

    MyStandbyMode->ProcessModeChangeTrigger();
}

//test for void StandbyMode::ProcessTriggerID (E_TriggerId id)
TEST_F(StandbyModeUnitMock, ProcessTriggerID)
{
    //all calling function in this function is tested, not need to test
}

#endif //end define BD_MODE_UNIT_TEST
