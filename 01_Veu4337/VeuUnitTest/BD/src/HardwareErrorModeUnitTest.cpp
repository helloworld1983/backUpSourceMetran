#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "HardwareErrorMode.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "AlarmMockHeader.h"
#include "TriggerMgrMockHeader.h"
const int INIT_UPDATE_STAT_TIME = 0;

HardwareErrorMode* HardwareErrorMode::S_Instance = NULL;

HardwareErrorMode* HardwareErrorMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new HardwareErrorMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()

HardwareErrorMode::HardwareErrorMode(void):NonVenMode(eTrue)
{
    causeOfAlarm = eNoEventID;
}   // end HardwareErrorMode()

void HardwareErrorMode::Enter(E_VentilationMode fromModeId)
{
    SafetyValveSetDesiredState(eOpen);

//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    triggerMgrPtrDisableAllTriggers();
    triggerMgrPtrDisableTrigger(eHardwareErrorAlarmTriger);
    triggerMgrPtrEnableTrigger(eModeChangeTrigger);
    triggerMgrPtrEnableTrigger(eHardwareErrorAutoresetTrigger);
    triggerMgrPtrEnableTrigger(eAlarmResetOpReqTrigger);
    triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger);
    triggerMgrPtrEnableTriggerWith(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);

//    BdTaskSend(eSuspendAutoSetRequest);

    //  Suspend auto-zero scheduling.
//    PInhAzTestPtr->SuspendTest();
//    PExhAzTestPtr->SuspendTest();
    MgrControllerCMVServoControllerDisable();

//    HfoController->Disable();
    HfoServoDisable();


    PhaseMgrProcessNewPhase(eAlarmPhase);
}  // end Enter()

void HardwareErrorMode::Exit(void)
{
    // Update the High Press alarm status
    if(causeOfAlarm != eNoEventID)
    {
        BDAlarmTaskSetIsActive(causeOfAlarm, eNotActive);
    }
    causeOfAlarm = eNoEventID;
    // Initialize pointer to the TriggerMgr
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable triggers.
    triggerMgrPtrDisableTrigger(eHardwareErrorAutoresetTrigger);
    triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger);
    triggerMgrPtrEnableTrigger(eHardwareErrorAlarmTriger);

    // Resume auto-zero scheduling.
//    PInhAzTestPtr->ResumeTest();
//    PExhAzTestPtr->ResumeTest();

}   // end Exit()

void HardwareErrorMode::UpdateAlarmStatus(void)
{

    E_ReturnStatus status = eError;
//    TriggerMgr* triggerMgrPtr;
    if(causeOfAlarm != eNoEventID)
    {
        status = (E_ReturnStatus)BDAlarmTaskSetIsActive(causeOfAlarm, eActive);


        // If the status update was unsuccessful, reschedule the next update
        // for the short interval
        if (status != eSuccess)
        {
//            triggerMgrPtr = TriggerMgr::S_GetInstance();
            triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger);
            triggerMgrPtrEnableTriggerWith(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);
        }
    }

}   // end UpdateAlarmStatus()

void HardwareErrorMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eHardwareErrorAutoresetTrigger:
            ProcessHardwareErrorAutoresetTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

void HardwareErrorMode::ProcessHardwareErrorAutoresetTrigger(void)
{
    ModeMgrGoToOperatorSetMode();
}

class HardwareErrorModeUnitMock : public BDTestFixture
{
public:
    HardwareErrorModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~HardwareErrorModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyHardwareErrorMode = new HardwareErrorMode();
    }
    virtual void TearDown(void)
    {
        delete MyHardwareErrorMode;
    }
    HardwareErrorMode *MyHardwareErrorMode;
};

//test for HardwareErrorMode* HardwareErrorMode::S_GetInstance(void)
TEST_F(HardwareErrorModeUnitMock, S_GetInstance)
{
    MyHardwareErrorMode->S_GetInstance();

    EXPECT_NE(MyHardwareErrorMode->S_Instance, (void*)0);
}

//test for HardwareErrorMode::HardwareErrorMode(void):NonVenMode(eTrue)
TEST_F(HardwareErrorModeUnitMock, HardwareErrorMode)
{
    EXPECT_EQ(MyHardwareErrorMode->causeOfAlarm, eNoEventID);
}

//test for void HardwareErrorMode::Enter(E_VentilationMode fromModeId)
TEST_F(HardwareErrorModeUnitMock, Enter)
{
    EXPECT_CALL(*_StepMotor, SafetyValveSetDesiredState(eOpen)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableAllTriggers()).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eHardwareErrorAlarmTriger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eModeChangeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eHardwareErrorAutoresetTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eAlarmResetOpReqTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerDisable()).Times(1);
    EXPECT_CALL(*_StepMotor, HfoServoDisable()).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eAlarmPhase)).Times(1);

    MyHardwareErrorMode->Enter(eHardwareErrorMode);
}

//test for void HardwareErrorMode::Exit(void)
TEST_F(HardwareErrorModeUnitMock, Exit)
{
    MyHardwareErrorMode->causeOfAlarm = eCheckGuiTask;

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eCheckGuiTask, eNotActive)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eHardwareErrorAutoresetTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eHardwareErrorAlarmTriger)).Times(1);

    MyHardwareErrorMode->Exit();

    EXPECT_EQ(MyHardwareErrorMode->causeOfAlarm, eNoEventID);
}

//test for void HardwareErrorMode::UpdateAlarmStatus(void)
TEST_F(HardwareErrorModeUnitMock, UpdateAlarmStatus)
{
    MyHardwareErrorMode->causeOfAlarm = eCheckGuiTask;

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eCheckGuiTask, eActive)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME)).Times(1);

    MyHardwareErrorMode->UpdateAlarmStatus();
}

//test for void HardwareErrorMode::ProcessTriggerID (E_TriggerId id)
TEST_F(HardwareErrorModeUnitMock, ProcessTriggerID)
{
    //all calliing function is tested, not need to test
}

//test for void HardwareErrorMode::ProcessHardwareErrorAutoresetTrigger(void)
TEST_F(HardwareErrorModeUnitMock, ProcessHardwareErrorAutoresetTrigger)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToOperatorSetMode()).Times(1);

    MyHardwareErrorMode->ProcessHardwareErrorAutoresetTrigger();
}
#endif //end define BD_MODE_UNIT_TEST
