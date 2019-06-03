#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "HighPressMode.h"
#include "DebugTrace.h"

#include "PhaseMgrMockHeader.h"
#include "AlarmMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "StepMotorMockHeader.h"
#include "PdTaskMockHeader.h"

#include "PdInterface.h"

using namespace ::testing;

HighPressMode* HighPressMode::S_Instance = NULL;
const int INIT_UPDATE_STAT_TIME = 0;

HighPressMode* HighPressMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new HighPressMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()
HighPressMode::HighPressMode(void):AlarmBreathingMode(eTrue)
{
    causeOfAlarm = eNoEventID;
    FromModeId = eStandbyMode;
}   // end HighPressMode()
void HighPressMode::Enter(E_VentilationMode fromModeId)
{
    FromModeId = fromModeId;

    //    SafetyValve->SetDesiredState(eOpen);

//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    triggerMgrPtrDisableAllTriggers();
    triggerMgrPtrEnableTrigger(eModeChangeTrigger);
    triggerMgrPtrEnableTrigger(eAlarmResetOpReqTrigger);
    triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger);
    triggerMgrPtrEnableTriggerWith(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);

    MgrControllerCMVServoControllerDisable();
    MgrControllerVCVServoControllerDisable();
    //    HfoController->Disable();

    //  Suspend auto-zero scheduling.
    //    PInhAzTestPtr->SuspendTest();
    //    PExhAzTestPtr->SuspendTest();
    PdTaskS_SetEventFlag(NONBREATHING_MODE_EVENT_FLAG);

//    AlarmBreathingMode::Enter(fromModeId);
    PhaseMgrProcessNewPhase(eAlarmPhase);
}  // end Enter()
void HighPressMode::Exit(void)
{
    causeOfAlarm = eNoEventID;
    // Initialize pointer to the TriggerMgr
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable triggers.
    triggerMgrPtrDisableTrigger(eHighMAP_2AutoResetTrigger);
    triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger);

    // Resume auto-zero scheduling.
    //    PInhAzTestPtr->ResumeTest();
    //    PExhAzTestPtr->ResumeTest();

    AlarmBreathingMode::Exit();
}   // end Exit()
void HighPressMode::UpdateAlarmStatus(void)
{
//    TriggerMgr* triggerMgrPtr;
    if(causeOfAlarm != eNoEventID)
    {
        BDAlarmTaskSetIsActive(causeOfAlarm, eActive);
        {
//            triggerMgrPtr = TriggerMgr::S_GetInstance();
            triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger);
            triggerMgrPtrEnableTriggerWith(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);
        }
    }
}   // end UpdateAlarmStatus()

class MyHighPressModeUnitMock : public BDTestFixture
{
public:
    MyHighPressModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~MyHighPressModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyHighPressMode = new HighPressMode();
    }
    virtual void TearDown(void)
    {
        delete MyHighPressMode;
    }
    HighPressMode *MyHighPressMode;
};

//test for HighPressMode* HighPressMode::S_GetInstance(void)
TEST_F(MyHighPressModeUnitMock, S_GetInstance)
{
    MyHighPressMode->S_GetInstance();

    EXPECT_NE((void*)0, MyHighPressMode->S_Instance);
}

//test for HighPressMode::HighPressMode(void):AlarmBreathingMode(eTrue)
TEST_F(MyHighPressModeUnitMock, HighPressMode)
{
    EXPECT_EQ(eNoEventID, MyHighPressMode->causeOfAlarm);
    EXPECT_EQ(eStandbyMode, MyHighPressMode->FromModeId);
}

//test for void HighPressMode::Enter(E_VentilationMode fromModeId)
TEST_F(MyHighPressModeUnitMock, Enter)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableAllTriggers()).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eModeChangeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eAlarmResetOpReqTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerDisable()).Times(1);
    EXPECT_CALL(*_StepMotor, MgrControllerVCVServoControllerDisable()).Times(1);

    EXPECT_CALL(*_PdTask, PdTaskS_SetEventFlag(NONBREATHING_MODE_EVENT_FLAG)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eAlarmPhase)).Times(1);

    MyHighPressMode->Enter(eHardwareErrorMode);

    EXPECT_EQ(eHardwareErrorMode, MyHighPressMode->FromModeId);
}

//test for void HighPressMode::Exit(void)
TEST_F(MyHighPressModeUnitMock, Exit)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eHighMAP_2AutoResetTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger)).Times(1);

    MyHighPressMode->Exit();

    EXPECT_EQ(eNoEventID, MyHighPressMode->causeOfAlarm);
}

//test for void HighPressMode::UpdateAlarmStatus(void)
TEST_F(MyHighPressModeUnitMock, UpdateAlarmStatus)
{
    MyHighPressMode->causeOfAlarm = eO2FlowSensorRes;
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eO2FlowSensorRes, eActive)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME)).Times(1);

    MyHighPressMode->UpdateAlarmStatus();

}

#endif //end define BD_MODE_UNIT_TEST
