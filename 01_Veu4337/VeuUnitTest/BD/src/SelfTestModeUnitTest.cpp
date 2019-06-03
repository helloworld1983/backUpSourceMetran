#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "SelfTestMode.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "AlarmMockHeader.h"
#include "TriggerMgrMockHeader.h"

SelfTestMode* SelfTestMode::S_Instance = NULL;
SelfTestMode* SelfTestMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new SelfTestMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()
SelfTestMode::SelfTestMode(void):Mode()
{

    CurrentTestId = eNoBdEventId;

}  // end SelfTestMode()
void SelfTestMode::Enter(E_VentilationMode fromModeId)
{
    // Initialize pointer to the PhaseMgr
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();
    // Put the ventilator into the SelfTest phase.
    phaseMgrPtrProcessNewPhase(eSelfTestPhase);

    // Initialize pointer to the TriggerMgr
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    // Disable all triggers.  This call must be made after forcing the vent
    // into the oxygen calibration phase above because depending on the phase
    // transitioned from, there may be some triggers left enabled.
    triggerMgrPtrDisableAllTriggers();

//    InhPressureZero->SetDesiredState(eOff);
//    ExhPressureZero->SetDesiredState(eOff);
//
//    SafetyValve->SetDesiredState(eOpen);

    // Suspend auto-zero scheduling.
//    PInhAzTestPtr->SuspendTest();
//    PExhAzTestPtr->SuspendTest();

}   // end Enter()
void SelfTestMode::Exit(void)
{
    // Initialize pointer to the TriggerMgr
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable triggers.
    triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger);
    triggerMgrPtrDisableTrigger(eAlarmResetOpReqTrigger);

    CurrentTestId = eNoBdEventId;
    // Resume auto-zero scheduling
//    PInhAzTestPtr->ResumeTest();
//    PExhAzTestPtr->ResumeTest();

}   // end Exit()
void SelfTestMode::UpdateAlarmStatus(void)
{
    // This operation should never be called.
    ASSERTION(eFalse);

}  // end UpdateAlarmStatus()
void SelfTestMode::ProcessModeChangeTrigger(void)
{
    ModeMgrGoToOperatorSetMode();
}
void SelfTestMode::ProcessTriggerID (E_TriggerId id)
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

class SelfTestModeUnitMock : public BDTestFixture
{
public:
    SelfTestModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~SelfTestModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MySelfTestMode = new SelfTestMode();
    }
    virtual void TearDown(void)
    {
        delete MySelfTestMode;
    }
    SelfTestMode *MySelfTestMode;
};

//test for SelfTestMode* SelfTestMode::S_GetInstance(void)
TEST_F(SelfTestModeUnitMock, S_GetInstance)
{
    MySelfTestMode->S_GetInstance();

    EXPECT_NE((void*)0, MySelfTestMode->S_Instance);
}

//test for SelfTestMode::SelfTestMode(void):Mode()
TEST_F(SelfTestModeUnitMock, Mode)
{
    EXPECT_EQ(MySelfTestMode->CurrentTestId, eNoBdEventId);
}

//test for void SelfTestMode::Enter(E_VentilationMode fromModeId)
TEST_F(SelfTestModeUnitMock, Enter)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eSelfTestPhase)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableAllTriggers()).Times(1);

    MySelfTestMode->Enter(eHardwareErrorMode);

}

//test for void SelfTestMode::Exit(void)
TEST_F(SelfTestModeUnitMock, Exit)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eAlarmResetOpReqTrigger)).Times(1);

    MySelfTestMode->Exit();

    EXPECT_EQ(MySelfTestMode->CurrentTestId, eNoBdEventId);
}

//test for void SelfTestMode::UpdateAlarmStatus(void)
TEST_F(SelfTestModeUnitMock, UpdateAlarmStatus)
{
    //never call this function, not need to test
}

//test for void SelfTestMode::ProcessModeChangeTrigger(void)
TEST_F(SelfTestModeUnitMock, ProcessModeChangeTrigger)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToOperatorSetMode()).Times(1);

    MySelfTestMode->ProcessModeChangeTrigger();
}

//test for void SelfTestMode::ProcessTriggerID (E_TriggerId id)
TEST_F(SelfTestModeUnitMock, ProcessTriggerID)
{
    //all calling is tested, not need to test
}
#endif //end define BD_MODE_UNIT_TEST
