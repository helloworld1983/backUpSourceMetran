#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "ApneaVAMode.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "AlarmMockHeader.h"
#include "StepMotorMockHeader.h"
#include "ModeMgrMockHeader.h"
#include "BreathMgrMockHeader.h"

ApneaVAMode* ApneaVAMode::S_Instance = NULL;
ApneaVAMode* ApneaVAMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new ApneaVAMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()
void ApneaVAMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eProxyDisconnectTrigger:
            ProcessProxyDisconnectTrigger();
            break;

        case eProxyConnectTrigger:
            ProcessProxyConnectTrigger();
            break;

        case ePtInhTrigger:
            ProcessPtInhTrigger();
            break;

        case eMandPressInhTimeTrigger:
            ProcessMandPressInhTimeTrigger();
            break;

        case eAlarmResetTrigger:
            ProcessAlarmResetTrigger();
            break;

        case eApneaAlarmTimeTrigger:
            ProcessApneaAlarmTimeTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }

}
ApneaVAMode::ApneaVAMode()
{
    fromMode = eStandbyMode;
    IsPtInhTrigger = false;
}
void ApneaVAMode::Enter(E_VentilationMode fromModeId)
{
    switch(fromModeId)
    {
    case eHighPressMode:
    case eHardwareErrorMode:
        ModeMgrGoToOperatorSetMode();
        break;
    default:

        fromMode = fromModeId;
        IsPtInhTrigger = false;
        Window = eMandWindow;

//        E_State TempState;
//        TempState = eOn;
//        DeviceInterface::S_GetInstance()->RequestDevice(eInpedanceLineSetStateeOn, &TempState);
//        TempState = eOff;
//        DeviceInterface::S_GetInstance()->RequestDevice(eInpedanceLineSetDesiredStateeOff, &TempState);
//        TempState = eClose;
//        DeviceInterface::S_GetInstance()->RequestDevice(eInpedanceLineSetDesiredStateeOff, &TempState);


//        TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
        triggerMgrPtrDisableAllTriggers();

        triggerMgrPtrEnableIMVTrigger();
        triggerMgrPtrDisableTrigger(eApneaAlarmTimeTrigger);

        UNSIGNED breathPeriod = triggerMgrPtrGetTimePeriod(eBpBackupTimeTrigger);
        triggerMgrPtrEnableTriggerWith(eBpBackupTimeTrigger,breathPeriod);

        BDRequestDevice(eCMVServoControllerEnable);

        ProxyStatus Status = ProxyFlowSensorgetLastStatus();

        if((Status ==eCalibrated)||(Status==eResetAndRecovered))
        {
            triggerMgrPtrEnableTrigger(eProxyDisconnectTrigger);
            prevProxyStatus = eCalibrated;
        }
        else
        {
            prevProxyStatus = eNotCalibrated;
            triggerMgrPtrEnableTrigger(eProxyConnectTrigger);
        }
        BDAlarmTaskSetIsActive(eApnea, eActive);
//        ProcessInhPhase();
        break;
    }
}
void ApneaVAMode::Exit()
{
    BDAlarmTaskSetIsActive(eApnea, eNotActive);

    MgrBreathDataInitializeBreathDataPtr();
    triggerMgrPtrDisableTrigger(eBpBackupTimeTrigger);

    MgrControllerCMVServoControllerDisable();

}
void ApneaVAMode::ProcessPtInhTrigger(void)
{
    // Initialize pointer to the PhaseMgr object
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // A patient trigger should only fire during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Start a new inhalation.  The type of inhalation (mand or
    // spont) depends upon the window that is currently
    // open.  If the mandatory window is open, deliver a mand
    // breath and close the mandatory window since this breath
    // will meet the mandatory breath criteria for this breath
    // period
    if (eMandWindow == Window)
    {
        // Start a mandatory inhalation
        phaseMgrPtrProcessNewPhase(eMandInhVABreathPhase);

        // Close the mandatory window; open the spontaneous window.
        Window = eSpontWindow;
    }

    else if (eSpontWindow == Window)
    {
        // If the spontaneous window is open, deliver a spontaneous
        // inhalation.  The spontaneous window remains open until the
        // breath period timer elapses.
        phaseMgrPtrProcessNewPhase(eMandInhVABreathPhase);
    }
    else
    {
        // Window contains illegal value
        ASSERTION((Window == eMandWindow) || (Window == eSpontWindow));
    }
    IsPtInhTrigger = true;

}   // end ProcessPtInhTrigger()
void ApneaVAMode::ProcessMandPressInhTimeTrigger()
{
    if(!IsPtInhTrigger)
    {
        // Get a pointer to the PhaseMgr object
//        PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

        phaseMgrPtrProcessNewPhase(eExhPhase);
    }
    else
    {
        ModeMgrGoToOperatorSetMode();
    }
}
void ApneaVAMode::ProcessAlarmResetTrigger()
{
//    MgrBreathData::S_GetInstance()->SetFirstBreath();

    ModeMgrGoToOperatorSetMode();
}
void ApneaVAMode::ProcessApneaAlarmTimeTrigger()
{
    //Do nothing
}

class ApneaVAModeUnitMock : public BDTestFixture
{
public:
    ApneaVAModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~ApneaVAModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyApneaVAMode = new ApneaVAMode();
    }
    virtual void TearDown(void)
    {
        delete MyApneaVAMode;
    }
    ApneaVAMode *MyApneaVAMode;
};

//test for ApneaVAMode* ApneaVAMode::S_GetInstance(void)
TEST_F(ApneaVAModeUnitMock, S_GetInstance)
{
    MyApneaVAMode->S_GetInstance();

    EXPECT_NE(MyApneaVAMode->S_Instance, (void*)0);
}

//test for void ApneaVAMode::ProcessTriggerID (E_TriggerId id)
TEST_F(ApneaVAModeUnitMock, ProcessTriggerID)
{
    //all calling funciton is tested, not need to test
}

//test for ApneaVAMode::ApneaVAMode()
TEST_F(ApneaVAModeUnitMock, ApneaVAMode)
{
    EXPECT_EQ(MyApneaVAMode->fromMode, eStandbyMode);
    EXPECT_EQ(MyApneaVAMode->IsPtInhTrigger, false);
}

//test for void ApneaVAMode::Enter(E_VentilationMode fromModeId)
TEST_F(ApneaVAModeUnitMock, Enter_0)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToOperatorSetMode()).Times(1);

    MyApneaVAMode->Enter(eHighPressMode);
}
TEST_F(ApneaVAModeUnitMock, Enter_1)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToOperatorSetMode()).Times(1);

    MyApneaVAMode->Enter(eHardwareErrorMode);
}
TEST_F(ApneaVAModeUnitMock, Enter_2)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableAllTriggers()).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableIMVTrigger()).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eApneaAlarmTimeTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod(eBpBackupTimeTrigger)).Times(1).WillOnce(Return(4));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eBpBackupTimeTrigger,4)).Times(1);

    EXPECT_CALL(*_StepMotor, BDRequestDevice(eCMVServoControllerEnable)).Times(1);

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eCalibrated));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eProxyDisconnectTrigger)).Times(1);

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eApnea, eActive)).Times(1);

    MyApneaVAMode->Enter(eAPRVMode);

    EXPECT_EQ(MyApneaVAMode->fromMode, eAPRVMode);
    EXPECT_EQ(MyApneaVAMode->IsPtInhTrigger, false);
    EXPECT_EQ(MyApneaVAMode->Window, eMandWindow);

    EXPECT_EQ(MyApneaVAMode->prevProxyStatus, eCalibrated);
}
TEST_F(ApneaVAModeUnitMock, Enter_3)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableAllTriggers()).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableIMVTrigger()).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eApneaAlarmTimeTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod(eBpBackupTimeTrigger)).Times(1).WillOnce(Return(4));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eBpBackupTimeTrigger,4)).Times(1);

    EXPECT_CALL(*_StepMotor, BDRequestDevice(eCMVServoControllerEnable)).Times(1);

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eResetAndRecovered));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eProxyDisconnectTrigger)).Times(1);

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eApnea, eActive)).Times(1);

    MyApneaVAMode->Enter(eAPRVMode);

    EXPECT_EQ(MyApneaVAMode->fromMode, eAPRVMode);
    EXPECT_EQ(MyApneaVAMode->IsPtInhTrigger, false);
    EXPECT_EQ(MyApneaVAMode->Window, eMandWindow);

    EXPECT_EQ(MyApneaVAMode->prevProxyStatus, eCalibrated);
}
TEST_F(ApneaVAModeUnitMock, Enter_4)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableAllTriggers()).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableIMVTrigger()).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eApneaAlarmTimeTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod(eBpBackupTimeTrigger)).Times(1).WillOnce(Return(4));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eBpBackupTimeTrigger,4)).Times(1);

    EXPECT_CALL(*_StepMotor, BDRequestDevice(eCMVServoControllerEnable)).Times(1);

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eSensorError));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eProxyConnectTrigger)).Times(1);

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eApnea, eActive)).Times(1);

    MyApneaVAMode->Enter(eAPRVMode);

    EXPECT_EQ(MyApneaVAMode->fromMode, eAPRVMode);
    EXPECT_EQ(MyApneaVAMode->IsPtInhTrigger, false);
    EXPECT_EQ(MyApneaVAMode->Window, eMandWindow);

    EXPECT_EQ(MyApneaVAMode->prevProxyStatus, eNotCalibrated);
}

//test for void ApneaVAMode::Exit()
TEST_F(ApneaVAModeUnitMock, Exit)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eApnea, eNotActive)).Times(1);

    EXPECT_CALL(*_BreathMgr, MgrBreathDataInitializeBreathDataPtr()).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eBpBackupTimeTrigger)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerDisable()).Times(1);

    MyApneaVAMode->Exit();
}

//test for void ApneaVAMode::ProcessPtInhTrigger(void)
TEST_F(ApneaVAModeUnitMock, ProcessPtInhTrigger_0)
{
    MyApneaVAMode->Window = eMandWindow;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eMandInhVABreathPhase)).Times(1);

    MyApneaVAMode->ProcessPtInhTrigger();

    EXPECT_EQ(MyApneaVAMode->IsPtInhTrigger, true);
    EXPECT_EQ(MyApneaVAMode->Window, eSpontWindow);
}
TEST_F(ApneaVAModeUnitMock, ProcessPtInhTrigger_1)
{
    MyApneaVAMode->Window = eSpontWindow;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eMandInhVABreathPhase)).Times(1);

    MyApneaVAMode->ProcessPtInhTrigger();

    EXPECT_EQ(MyApneaVAMode->IsPtInhTrigger, true);
}

//test for void ApneaVAMode::ProcessMandPressInhTimeTrigger()
TEST_F(ApneaVAModeUnitMock, ProcessMandPressInhTimeTrigger_0)
{
    MyApneaVAMode->IsPtInhTrigger = false;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eExhPhase)).Times(1);

    MyApneaVAMode->ProcessMandPressInhTimeTrigger();
}
TEST_F(ApneaVAModeUnitMock, ProcessMandPressInhTimeTrigger_1)
{
    MyApneaVAMode->IsPtInhTrigger = true;

    EXPECT_CALL(*_ModeMgr, ModeMgrGoToOperatorSetMode()).Times(1);

    MyApneaVAMode->ProcessMandPressInhTimeTrigger();
}

//test for void ApneaVAMode::ProcessAlarmResetTrigger()
TEST_F(ApneaVAModeUnitMock, ProcessAlarmResetTrigger)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToOperatorSetMode()).Times(1);

    MyApneaVAMode->ProcessAlarmResetTrigger();
}

//test for void ApneaVAMode::ProcessApneaAlarmTimeTrigger()
TEST_F(ApneaVAModeUnitMock, ProcessApneaAlarmTimeTrigger)
{
    //no source, not need to test
}

#endif //end define BD_MODE_UNIT_TEST
