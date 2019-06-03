#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "VA_AC_Mode.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"

#include "StepMotorMockHeader.h"

#define NUM_TEST_BREATH 3

VA_AC_Mode* VA_AC_Mode::S_Instance = NULL;
BreathType VA_AC_Mode::mBreathType = eTestBreath;
USHORT VA_AC_Mode::mTestBreathCount = 0;

VA_AC_Mode* VA_AC_Mode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new VA_AC_Mode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}

VA_AC_Mode::VA_AC_Mode(void) : MixedMode()
{
    prevProxyStatus = eNotCalibrated;
}

void VA_AC_Mode::Enter(E_VentilationMode fromModeId)
{
//    MixedMode::Enter(fromModeId);
    PhaseMgrProcessNewVentType(ePressureVentilation);

    if(fromModeId != eApneaVAMode)
    {
        MgrControllerCMVServoControllerResetIMVStaticVariables();
        InhVABreathPhaseClearPIDParameter();
        mTestBreathCount = 0;
        mBreathType = eTestBreath;
    }

    MgrControllerCMVServoControllerEnable();

    PhaseMgrProcessNewPhase(eExhPhase);


    ProxyStatus Status = ProxyFlowSensorgetLastStatus();
    if((Status == eCalibrated)||(Status == eResetAndRecovered))
    {
        triggerMgrPtrEnableTrigger(eProxyDisconnectTrigger);

        prevProxyStatus = eCalibrated;
    }
    else
    {
        triggerMgrPtrEnableTrigger(eProxyConnectTrigger);

        prevProxyStatus = eNotCalibrated;

    }
}

void VA_AC_Mode::Exit(void)
{
//    QMetaObject::invokeMethod(MainHandlerDelegate::S_GetInstance(), "EmitAlarmModeSignal",
//                             Q_ARG(int, (int)ePcvACMode),Q_ARG(bool,false));

    triggerMgrPtrDisableTrigger(eProxyConnectTrigger);
    triggerMgrPtrDisableTrigger(eProxyDisconnectTrigger);
    SafetyValveSetDesiredState(eOpen);
    MgrControllerCMVServoControllerDisable();
}

void VA_AC_Mode::StartTestBreath(void)
{
    mTestBreathCount = 0;
    mBreathType = eTestBreath;
}

void VA_AC_Mode::ProcessBpTimeTrigger(void)
{
    E_PhaseId phaseId;          // Current Phase ID
//    PhaseMgr* phaseMgrPtr;      // Pointer to the PhaseMgr object
    switch (Window)
    {
    case eMandWindow:
        // Initialize pointer to the PhaseMgr object
//        phaseMgrPtr = PhaseMgr::S_GetInstance();

        // Make sure the ventilator is in the exhalation phase.
        phaseId = phaseMgrPtrGetCurrentPhaseId();
        ASSERTION(phaseId == eExhPhase);

        ProcessInhPhase();
        break;

    case eSpontWindow:
        // If the breath period fires while the spontaneous window is open,
        // the ventilator just re-opens the mandatory window.
        Window = eMandWindow;
        break;

    default:
        // Trigger fired in wrong window
        ASSERTION((Window == eSpontWindow) || (Window == eMandWindow));
        break;
    }

}   // end ProcessBpTimeTrigger()

void VA_AC_Mode::ProcessInhPhase(void)
{
//    PhaseMgr* phaseMgrPtr;      // Pointer to the PhaseMgr object

//     Initialize pointer to the PhaseMgr object
//    phaseMgrPtr = PhaseMgr::S_GetInstance();

    ProxyStatus Status = ProxyFlowSensorgetLastStatus();
    if((Status ==eCalibrated)||(Status==eResetAndRecovered))
    {
        if(prevProxyStatus == eNotCalibrated)
        {
            double Compliance = 0;//MainHandlerDelegateGetNumModel(eCompliancePtDataID)->GetValue();
            if(Compliance < 0.1*SCALE)
            {
                mTestBreathCount = 1;
                phaseMgrPtrProcessNewPhase(eMandInhTestBreathPhase);
                InhVABreathPhaseClearPIDParameter();
            }
            else
            {
                prevProxyStatus = eCalibrated;
            }

        }
        else
        {
            if(NUM_TEST_BREATH > mTestBreathCount)
            {
                mTestBreathCount++;
                phaseMgrPtrProcessNewPhase(eMandInhTestBreathPhase);
            }
            else
            {
                phaseMgrPtrProcessNewPhase(eMandInhVABreathPhase);
            }
        }

    }
    else
    {
        prevProxyStatus = eNotCalibrated;

        phaseMgrPtrProcessNewPhase(eMandInhPhase);
        mTestBreathCount = 0;
        mBreathType = eTestBreath;
        InhVABreathPhaseClearPIDParameter();
    }
}
void VA_AC_Mode::ProcessPtInhTrigger(void)
{
    // Initialize pointer to the PhaseMgr object
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // A patient trigger should only fire during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Start a mandatory inhalation
    phaseMgrPtrProcessNewPhase(eMandInhVABreathPhase);

    // Close the mandatory window; open the spontaneous window.
    Window = eMandWindow;

    triggerMgrPtrDisableTrigger(eBpTimeTrigger);
    UNSIGNED breathPeriod = triggerMgrPtrGetTimePeriod(eBpTimeTrigger);

    triggerMgrPtrEnableTriggerWith(eBpTimeTrigger,breathPeriod);


}   // end ProcessPtInhTrigger()

void VA_AC_Mode::ProcessProxyDisconnectTrigger()
{
    prevProxyStatus = eNotCalibrated;

//    UpdatePIPSetting();

    E_VentilationMode currentMode = BDModeMgrGetCurrentModeId();
    if(currentMode == eVaACMode)
    {
//        QMetaObject::invokeMethod(MainHandlerDelegate::S_GetInstance(), "EmitAlarmModeSignal",
//                                 Q_ARG(int, (int)ePcvACMode),Q_ARG(bool,true));
    }
    else if(currentMode == eVaMixedMode)
    {
//        QMetaObject::invokeMethod(MainHandlerDelegate::S_GetInstance(), "EmitAlarmModeSignal",
//                                 Q_ARG(int, (int)ePcvMixedMode),Q_ARG(bool,true));
    }

}

void VA_AC_Mode::ProcessProxyConnectTrigger()
{
//    QMetaObject::invokeMethod(MainHandlerDelegate::S_GetInstance(), "EmitAlarmModeSignal",
//                             Q_ARG(int, (int)ePcvACMode),Q_ARG(bool,false));
}

void VA_AC_Mode::ProcessApneaAlarmTimeTrigger(void)
{
//    QMetaObject::invokeMethod(MainHandlerDelegate::S_GetInstance(), "EmitAlarmModeSignal",
//                             Q_ARG(int, (int)ePcvACMode),Q_ARG(bool,false));
    ModeMgrGoToAlarmMode(eApneaVAMode);
}

void VA_AC_Mode::UpdatePIPSetting()
{
    LONG pipTarget = InhVABreathPhaseGetPIPTarget();

    LONG tmp = pipTarget % 100;

    if(tmp > 50)
    {
        pipTarget = pipTarget + (100 - tmp);//  round up
    }
    else if(tmp < 50)
    {
        pipTarget = pipTarget - tmp;//  round down
    }

//    MainHandlerDelegateGetNumSetting(ePCVPressureSettingID)->SetValue(pipTarget);
}

void VA_AC_Mode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eBpTimeTrigger:
            ProcessBpTimeTrigger();
            break;

        case ePtInhTrigger:
            ProcessPtInhTrigger();
            break;

        case eProxyDisconnectTrigger:
            ProcessProxyDisconnectTrigger();
            break;

        case eProxyConnectTrigger:
            ProcessProxyConnectTrigger();
            break;

        case eApneaAlarmTimeTrigger:
            ProcessApneaAlarmTimeTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

class VA_AC_ModeUnitMock : public BDTestFixture
{
public:
    VA_AC_ModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~VA_AC_ModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyVA_AC_Mode = new VA_AC_Mode();
    }
    virtual void TearDown(void)
    {
        delete MyVA_AC_Mode;
    }
    VA_AC_Mode *MyVA_AC_Mode;
};

//test forVA_AC_Mode* VA_AC_Mode::S_GetInstance(void)
TEST_F(VA_AC_ModeUnitMock, S_GetInstance)
{
    MyVA_AC_Mode->S_GetInstance();

    EXPECT_NE(MyVA_AC_Mode->S_Instance, (void*)0);
}

//test for VA_AC_Mode::VA_AC_Mode(void) : MixedMode()
TEST_F(VA_AC_ModeUnitMock, VA_AC_Mode)
{
    EXPECT_EQ(MyVA_AC_Mode->prevProxyStatus, eNotCalibrated);
}

//test for void VA_AC_Mode::Enter(E_VentilationMode fromModeId)
TEST_F(VA_AC_ModeUnitMock, Enter_0)
{
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewVentType(ePressureVentilation)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerResetIMVStaticVariables()).Times(1);
    EXPECT_CALL(*_BreathMgr, InhVABreathPhaseClearPIDParameter()).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerEnable()).Times(1);
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eExhPhase)).Times(1);

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eCalibrated));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eProxyDisconnectTrigger)).Times(1);

    MyVA_AC_Mode->Enter(eHardwareErrorMode);

    EXPECT_EQ(MyVA_AC_Mode->mTestBreathCount, 0);
    EXPECT_EQ(MyVA_AC_Mode->mBreathType, eTestBreath);
    EXPECT_EQ(MyVA_AC_Mode->prevProxyStatus, eCalibrated);
}
TEST_F(VA_AC_ModeUnitMock, Enter_1)
{
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewVentType(ePressureVentilation)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerResetIMVStaticVariables()).Times(1);
    EXPECT_CALL(*_BreathMgr, InhVABreathPhaseClearPIDParameter()).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerEnable()).Times(1);
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eExhPhase)).Times(1);

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eResetAndRecovered));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eProxyDisconnectTrigger)).Times(1);

    MyVA_AC_Mode->Enter(eHardwareErrorMode);

    EXPECT_EQ(MyVA_AC_Mode->mTestBreathCount, 0);
    EXPECT_EQ(MyVA_AC_Mode->mBreathType, eTestBreath);
    EXPECT_EQ(MyVA_AC_Mode->prevProxyStatus, eCalibrated);
}
TEST_F(VA_AC_ModeUnitMock, Enter_2)
{
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewVentType(ePressureVentilation)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerResetIMVStaticVariables()).Times(1);
    EXPECT_CALL(*_BreathMgr, InhVABreathPhaseClearPIDParameter()).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerEnable()).Times(1);
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eExhPhase)).Times(1);

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eSensorError));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eProxyConnectTrigger)).Times(1);

    MyVA_AC_Mode->Enter(eHardwareErrorMode);

    EXPECT_EQ(MyVA_AC_Mode->mTestBreathCount, 0);
    EXPECT_EQ(MyVA_AC_Mode->mBreathType, eTestBreath);
    EXPECT_EQ(MyVA_AC_Mode->prevProxyStatus, eNotCalibrated);
}

//test for void VA_AC_Mode::Exit(void)
TEST_F(VA_AC_ModeUnitMock, Exit)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eProxyConnectTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eProxyDisconnectTrigger)).Times(1);

    EXPECT_CALL(*_StepMotor, SafetyValveSetDesiredState(eOpen)).Times(1);
    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerDisable()).Times(1);

    MyVA_AC_Mode->Exit();
}

//test for void VA_AC_Mode::StartTestBreath(void)
TEST_F(VA_AC_ModeUnitMock, StartTestBreath)
{
    MyVA_AC_Mode->StartTestBreath();

    EXPECT_EQ(MyVA_AC_Mode->mTestBreathCount, 0);
    EXPECT_EQ(MyVA_AC_Mode->mBreathType, eTestBreath);
}

//test for void VA_AC_Mode::ProcessBpTimeTrigger(void)
TEST_F(VA_AC_ModeUnitMock, ProcessBpTimeTrigger_0)
{
    MyVA_AC_Mode->Window = eSpontWindow;

    MyVA_AC_Mode->ProcessBpTimeTrigger();

    EXPECT_EQ(MyVA_AC_Mode->Window, eMandWindow);
}
TEST_F(VA_AC_ModeUnitMock, ProcessBpTimeTrigger_1)
{
    MyVA_AC_Mode->Window = eMandWindow;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1);

    MyVA_AC_Mode->ProcessBpTimeTrigger();
}

//test for void VA_AC_Mode::ProcessInhPhase(void)
TEST_F(VA_AC_ModeUnitMock, ProcessInhPhase_0)
{
    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eSensorError));

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eMandInhPhase)).Times(1);

    EXPECT_CALL(*_BreathMgr, InhVABreathPhaseClearPIDParameter()).Times(1);

    MyVA_AC_Mode->ProcessInhPhase();

    EXPECT_EQ(MyVA_AC_Mode->prevProxyStatus, eNotCalibrated);
    EXPECT_EQ(MyVA_AC_Mode->mTestBreathCount, 0);
    EXPECT_EQ(MyVA_AC_Mode->mBreathType, eTestBreath);
}
TEST_F(VA_AC_ModeUnitMock, ProcessInhPhase_1)
{
    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eCalibrated));
    MyVA_AC_Mode->prevProxyStatus = eNotCalibrated;

    MyVA_AC_Mode->ProcessInhPhase();
}
TEST_F(VA_AC_ModeUnitMock, ProcessInhPhase_2)
{
    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eCalibrated));
    MyVA_AC_Mode->prevProxyStatus = eCalibrated;

    MyVA_AC_Mode->ProcessInhPhase();
}
TEST_F(VA_AC_ModeUnitMock, ProcessInhPhase_3)
{
    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eResetAndRecovered));
    MyVA_AC_Mode->prevProxyStatus = eNotCalibrated;

    MyVA_AC_Mode->ProcessInhPhase();
}
TEST_F(VA_AC_ModeUnitMock, ProcessInhPhase_4)
{
    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eResetAndRecovered));
    MyVA_AC_Mode->prevProxyStatus = eCalibrated;

    MyVA_AC_Mode->ProcessInhPhase();
}

//test for void VA_AC_Mode::ProcessPtInhTrigger(void)
TEST_F(VA_AC_ModeUnitMock, ProcessPtInhTrigger)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eMandInhVABreathPhase)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eBpTimeTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod(eBpTimeTrigger)).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eBpTimeTrigger,2)).Times(1);

    MyVA_AC_Mode->ProcessPtInhTrigger();

    EXPECT_EQ(MyVA_AC_Mode->Window, eMandWindow);
}

//test for void VA_AC_Mode::ProcessProxyDisconnectTrigger()
TEST_F(VA_AC_ModeUnitMock, ProcessProxyDisconnectTrigger_0)
{
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eVaACMode));

    MyVA_AC_Mode->ProcessProxyDisconnectTrigger();

    EXPECT_EQ(MyVA_AC_Mode->prevProxyStatus, eNotCalibrated);
}
TEST_F(VA_AC_ModeUnitMock, ProcessProxyDisconnectTrigger_1)
{
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eVaMixedMode));

    MyVA_AC_Mode->ProcessProxyDisconnectTrigger();

    EXPECT_EQ(MyVA_AC_Mode->prevProxyStatus, eNotCalibrated);
}

//test for void VA_AC_Mode::ProcessProxyConnectTrigger()
TEST_F(VA_AC_ModeUnitMock, ProcessProxyConnectTrigger)
{
    //no source, not need to test
}

//test for void VA_AC_Mode::ProcessApneaAlarmTimeTrigger(void)
TEST_F(VA_AC_ModeUnitMock, ProcessApneaAlarmTimeTrigger)
{
    EXPECT_CALL(*_ModeMgr,  ModeMgrGoToAlarmMode(eApneaVAMode)).Times(1);

    MyVA_AC_Mode->ProcessApneaAlarmTimeTrigger();
}

//test for void VA_AC_Mode::UpdatePIPSetting()
TEST_F(VA_AC_ModeUnitMock, UpdatePIPSetting_0)
{
    EXPECT_CALL(*_BreathMgr, InhVABreathPhaseGetPIPTarget()).Times(1).WillOnce(Return(250));

    MyVA_AC_Mode->UpdatePIPSetting();
}
TEST_F(VA_AC_ModeUnitMock, UpdatePIPSetting_1)
{
    EXPECT_CALL(*_BreathMgr, InhVABreathPhaseGetPIPTarget()).Times(1).WillOnce(Return(50));

    MyVA_AC_Mode->UpdatePIPSetting();
}

//test for void VA_AC_Mode::ProcessTriggerID (E_TriggerId id)
TEST_F(VA_AC_ModeUnitMock, ProcessTriggerID)
{
    //all calling function is test, not need to test
}

#endif //end define BD_MODE_UNIT_TEST
