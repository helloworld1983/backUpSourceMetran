#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "DisconnectionMode.h"
#include "DebugTrace.h"

#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "AlarmMockHeader.h"
#include "TriggerMgrMockHeader.h"

#include "StepMotorMockHeader.h"

using namespace ::testing;
DisconnectionMode* DisconnectionMode::S_Instance = NULL;
const UNSIGNED INIT_UPDATE_STAT_TIME = 0;

DisconnectionMode* DisconnectionMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new DisconnectionMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()
DisconnectionMode::DisconnectionMode(void):AlarmBreathingMode(eTrue)
{

}   // end DisconnectionMode()
void DisconnectionMode::Enter(E_VentilationMode fromModeId)
{
    // Initialize pointer to the TriggerMgr
    //    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    //    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // Put the ventilator into the eExhPhase phase.
    phaseMgrPtrProcessNewPhase(eExhPhase);

    // Disable some triggers.  These call must be made after forcing the vent
    // into the exhalaion phase above because depending on the phase
    // transitioned from, there may be some triggers left enabled.
    triggerMgrPtrDisableTrigger(eManualInhOpReqTrigger);
    triggerMgrPtrDisableTrigger(ePtInhPressTrigger);
    triggerMgrPtrDisableTrigger(ePtInhFlowTrigger);

    // Enable the Update Status trigger which causes the update of the
    // status of the alarms associated with the active Disconnected mode.
    triggerMgrPtrEnableTriggerWith(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);

    // Enable the Reconected trigger.
    triggerMgrPtrEnableTrigger(eReconnectionTrigger);

    // Enable the reset trigger
    triggerMgrPtrEnableTrigger(eAlarmResetOpReqTrigger);

    // Update the alarm status to indicate that the total gas loss alarm is
    // not active.

    if(BDAlarmTaskGetIsActive(eGasSuppliesLost) == eActive)
    {
        BDAlarmTaskSetIsActive(eGasSuppliesLost, eNotActive);
    }

    // Update the Disconnected Circuit alarm status.
    BDAlarmTaskSetIsActive(eDisconnectedCircuit, eActive);

    MgrControllerCMVServoControllerEnable();

    // Suspend auto-zero scheduling.
    //    PInhAzTestPtr->SuspendTest();
    //    PExhAzTestPtr->SuspendTest();


}   // end Enter()
void DisconnectionMode::Exit(void)
{

    // Initialize pointer to the TriggerMgr
    //    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable triggers.
    triggerMgrPtrDisableTrigger(eReconnectionTrigger);
    triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger);

    // Update the Disconnected Circuit alarm status
    BDAlarmTaskSetIsActive(eDisconnectedCircuit, eNotActive);

    MgrControllerCMVServoControllerDisable();


    // Resume auto-zero scheduling.
    //    PInhAzTestPtr->ResumeTest();
    //    PExhAzTestPtr->ResumeTest();

}   // end Exit()
void DisconnectionMode::ProcessReconnectionTrigger(void)
{
    //    ModeMgr* modeMgrPtr = ModeMgr::S_GetInstance();
    modeMgrPtProcessAutoReset();
    ASSERTION((PhaseMgr::S_GetInstance())->GetCurrentPhaseId() == eExhPhase);
    modeMgrPtrReturnToLastMode();

}   // end ProcessReconectedTrigger()
void DisconnectionMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eReconnectionTrigger:
            ProcessReconnectionTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}
void DisconnectionMode::UpdateAlarmStatus(void)
{
    E_ReturnStatus status;
    //    TriggerMgr* triggerMgrPtr;

    // Update the alarm status to indicate that the disconnected circuit alarm
    // is active.
    status = (E_ReturnStatus)BDAlarmTaskSetIsActive(eDisconnectedCircuit, eActive);

    // If the status update was unsuccessful, reschedule the next update
    // for the short interval
    if (status != eSuccess)
    {
        //        triggerMgrPtr = TriggerMgr::S_GetInstance();
        triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger);
        triggerMgrPtrEnableTriggerWith(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);
    }

}   // end UpdateAlarmStatus()

class DisconnectionModeUnitMock : public BDTestFixture
{
public:
    DisconnectionModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~DisconnectionModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyDisconnectionModeUnitMock = new DisconnectionMode();
    }
    virtual void TearDown(void)
    {
        delete MyDisconnectionModeUnitMock;
    }
    DisconnectionMode *MyDisconnectionModeUnitMock;
};

//test for DisconnectionMode* DisconnectionMode::S_GetInstance(void)
TEST_F(DisconnectionModeUnitMock, S_GetInstance)
{
    MyDisconnectionModeUnitMock->S_GetInstance();

    EXPECT_NE(MyDisconnectionModeUnitMock->S_Instance, (void*)0);
}

//test for DisconnectionMode::DisconnectionMode(void):AlarmBreathingMode(eTrue)
TEST_F(DisconnectionModeUnitMock, DisconnectionMode)
{
    //no source code, not need to test
}

//test for void DisconnectionMode::Enter(E_VentilationMode fromModeId)
TEST_F(DisconnectionModeUnitMock, Enter)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eExhPhase)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eManualInhOpReqTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtInhPressTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtInhFlowTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eReconnectionTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eAlarmResetOpReqTrigger)).Times(1);

    EXPECT_CALL(*_Alarm, BDAlarmTaskGetIsActive(eGasSuppliesLost)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eGasSuppliesLost, eNotActive)).Times(1);

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eDisconnectedCircuit, eActive)).Times(1);
    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerEnable()).Times(1);

    MyDisconnectionModeUnitMock->Enter(eFirstCommonMode);
}

//test for void DisconnectionMode::Exit(void)
TEST_F(DisconnectionModeUnitMock, Exit)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eReconnectionTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger)).Times(1);

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eDisconnectedCircuit, eNotActive)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerDisable()).Times(1);

    MyDisconnectionModeUnitMock->Exit();
}

//test for void DisconnectionMode::ProcessReconnectionTrigger(void)
TEST_F(DisconnectionModeUnitMock, ProcessReconnectionTrigger)
{
    EXPECT_CALL(*_ModeMgr, modeMgrPtProcessAutoReset()).Times(1);
    EXPECT_CALL(*_ModeMgr, modeMgrPtrReturnToLastMode()).Times(1);

    MyDisconnectionModeUnitMock->ProcessReconnectionTrigger();
}

//test for void DisconnectionMode::ProcessTriggerID (E_TriggerId id)
TEST_F(DisconnectionModeUnitMock, ProcessTriggerID)
{
    //all calling function is tested, not need to test
}

//test for void DisconnectionMode::UpdateAlarmStatus(void)
TEST_F(DisconnectionModeUnitMock, UpdateAlarmStatus)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eDisconnectedCircuit, eActive)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME)).Times(1);

    MyDisconnectionModeUnitMock->UpdateAlarmStatus();
}

#endif //end define BD_MODE_UNIT_TEST
