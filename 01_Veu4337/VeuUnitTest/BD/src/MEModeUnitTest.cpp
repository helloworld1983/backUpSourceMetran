#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "MEMode.h"
#include "DebugTrace.h"

#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"

using namespace ::testing;
// Initialize statics
MEMode* MEMode::S_Instance = NULL;

MEMode* MEMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new MEMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()
MEMode::MEMode(void):Mode()
{

}   // end MEMode()
void MEMode::Enter(E_VentilationMode fromModeId)
{
    //Initialize pointer to TriggerMgr
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    //Enable the mode change trigger.
    triggerMgrPtrEnableTrigger(eModeChangeTrigger);

    //Process DiagnosticPhase
    PhaseMgrProcessNewPhase(eDiagnosticPhase);

    //Suspend auto-zero scheduling.
//    PInhAzTestPtr->SuspendTest();
//    PExhAzTestPtr->SuspendTest();
}   // end Enter()
void MEMode::Exit(void)
{
    //Resume auto-zero scheduling.
//    PInhAzTestPtr->ResumeTest();
//    PExhAzTestPtr->ResumeTest();

}   // end Exit()
void MEMode::UpdateAlarmStatus(void)
{
    // This operation should never be called.
    ASSERTION(eFalse);

}  // end UpdateAlarmStatus()
void MEMode::ProcessTriggerID (E_TriggerId id)
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
void MEMode::ProcessModeChangeTrigger(void)
{
    ModeMgrGoToOperatorSetMode();
}

class MEModeUnitMock : public BDTestFixture
{
public:
    MEModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~MEModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyMEMode = new MEMode();
    }
    virtual void TearDown(void)
    {
        delete MyMEMode;
    }
    MEMode *MyMEMode;
};

//test for MEMode* MEMode::S_GetInstance(void)
TEST_F(MEModeUnitMock, S_GetInstance)
{
    MyMEMode->S_GetInstance();

    EXPECT_NE((void*)0, MyMEMode->S_Instance);
}

//test for MEMode::MEMode(void):Mode()
TEST_F(MEModeUnitMock, Mode)
{
    //no source, not neet to test
}

//test for void MEMode::Enter(E_VentilationMode fromModeId)
TEST_F(MEModeUnitMock, Enter)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eModeChangeTrigger)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eDiagnosticPhase)).Times(1);

    MyMEMode->Enter(eFirstCommonMode);
}

//test for void MEMode::Exit(void)
TEST_F(MEModeUnitMock, Exit)
{
    //no source code, not need to test
}

//test for void MEMode::UpdateAlarmStatus(void)
TEST_F(MEModeUnitMock, UpdateAlarmStatus)
{
    //this function is never called, not need to test
}

//test for void MEMode::ProcessTriggerID (E_TriggerId id)
TEST_F(MEModeUnitMock, ProcessTriggerID)
{
    //all calling function is tested, not need to test
}

//test for void MEMode::ProcessModeChangeTrigger(void)
TEST_F(MEModeUnitMock, ProcessModeChangeTrigger)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToOperatorSetMode()).Times(1);

    MyMEMode->ProcessModeChangeTrigger();
}
















#endif //end define BD_MODE_UNIT_TEST
