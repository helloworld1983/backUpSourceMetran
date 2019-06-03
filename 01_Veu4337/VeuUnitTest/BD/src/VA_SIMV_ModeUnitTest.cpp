#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "VA_SIMV_Mode.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "TriggerMgrMockHeader.h"
#include "StepMotorMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "BreathMgrMockHeader.h"

VA_SIMV_Mode* VA_SIMV_Mode::S_Instance = NULL;

VA_SIMV_Mode* VA_SIMV_Mode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new VA_SIMV_Mode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}
E_PhaseId VA_SIMV_Mode::GetInhPhaseType(void)
{
    return InhPhaseType;
}
VA_SIMV_Mode::VA_SIMV_Mode(void) : VA_AC_Mode()
{
    InhPhaseType = eMandInhVABreathPhase;
}
void VA_SIMV_Mode::Enter(E_VentilationMode fromModeId)
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
}
void VA_SIMV_Mode::ProcessPtInhTrigger()
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
        PhaseMgrProcessNewPhase(eMandInhVABreathPhase);

        // Close the mandatory window; open the spontaneous window.
        Window = eSpontWindow;

        InhPhaseType = eMandInhVABreathPhase;
    }

    else if (eSpontWindow == Window)
    {
#ifdef VA_WITH_PS //for new VA
        phaseMgrPtr->ProcessNewPhase(ePsvInhVABreathPhase);

        InhPhaseType = ePsvInhVABreathPhase;
#else
        PhaseMgrProcessNewPhase(eSpontInhPhase);

        InhPhaseType = eSpontInhPhase;
#endif
    }
    else
    {
        // Window contains illegal value
        ASSERTION((Window == eMandWindow) || (Window == eSpontWindow));
    }
}
void VA_SIMV_Mode::ProcessTriggerID (E_TriggerId id)
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

class VA_SIMV_ModeUnitMock : public BDTestFixture
{
public:
    VA_SIMV_ModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~VA_SIMV_ModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyVA_SIMV_Mode = new VA_SIMV_Mode();
    }
    virtual void TearDown(void)
    {
        delete MyVA_SIMV_Mode;
    }
    VA_SIMV_Mode *MyVA_SIMV_Mode;
};

//test for VA_SIMV_Mode* VA_SIMV_Mode::S_GetInstance(void)
TEST_F(VA_SIMV_ModeUnitMock, S_GetInstance)
{
    MyVA_SIMV_Mode->S_GetInstance();

    EXPECT_NE(MyVA_SIMV_Mode->S_Instance, (void*)0);
}

//test for E_PhaseId VA_SIMV_Mode::GetInhPhaseType(void)
TEST_F(VA_SIMV_ModeUnitMock, GetInhPhaseType)
{
    MyVA_SIMV_Mode->InhPhaseType = eMandInhVABreathPhase;

    EXPECT_EQ(MyVA_SIMV_Mode->GetInhPhaseType(), eMandInhVABreathPhase);
}

//test for void VA_SIMV_Mode::Enter(E_VentilationMode fromModeId)
TEST_F(VA_SIMV_ModeUnitMock, Enter_0)
{
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewVentType(ePressureVentilation)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerResetIMVStaticVariables()).Times(1);

    EXPECT_CALL(*_BreathMgr, InhVABreathPhaseClearPIDParameter()).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerEnable()).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eExhPhase)).Times(1);

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eCalibrated));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eProxyDisconnectTrigger)).Times(1);

    MyVA_SIMV_Mode->Enter(eHardwareErrorMode);

    EXPECT_EQ(MyVA_SIMV_Mode->mTestBreathCount, 0);
    EXPECT_EQ(MyVA_SIMV_Mode->mBreathType, eTestBreath);

    EXPECT_EQ(MyVA_SIMV_Mode->prevProxyStatus, eCalibrated);

}
TEST_F(VA_SIMV_ModeUnitMock, Enter_1)
{
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewVentType(ePressureVentilation)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerResetIMVStaticVariables()).Times(1);

    EXPECT_CALL(*_BreathMgr, InhVABreathPhaseClearPIDParameter()).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerEnable()).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eExhPhase)).Times(1);

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eResetAndRecovered));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eProxyDisconnectTrigger)).Times(1);

    MyVA_SIMV_Mode->Enter(eHardwareErrorMode);

    EXPECT_EQ(MyVA_SIMV_Mode->mTestBreathCount, 0);
    EXPECT_EQ(MyVA_SIMV_Mode->mBreathType, eTestBreath);

    EXPECT_EQ(MyVA_SIMV_Mode->prevProxyStatus, eCalibrated);

}
TEST_F(VA_SIMV_ModeUnitMock, Enter_2)
{
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewVentType(ePressureVentilation)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerResetIMVStaticVariables()).Times(1);

    EXPECT_CALL(*_BreathMgr, InhVABreathPhaseClearPIDParameter()).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerEnable()).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eExhPhase)).Times(1);

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eSensorError));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eProxyConnectTrigger)).Times(1);

    MyVA_SIMV_Mode->Enter(eHardwareErrorMode);

    EXPECT_EQ(MyVA_SIMV_Mode->mTestBreathCount, 0);
    EXPECT_EQ(MyVA_SIMV_Mode->mBreathType, eTestBreath);

    EXPECT_EQ(MyVA_SIMV_Mode->prevProxyStatus, eNotCalibrated);

}

//test for void VA_SIMV_Mode::ProcessPtInhTrigger()
TEST_F(VA_SIMV_ModeUnitMock, ProcessPtInhTrigger_0)
{
    MyVA_SIMV_Mode->Window = eMandWindow;

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eMandInhVABreathPhase)).Times(1);

    MyVA_SIMV_Mode->ProcessPtInhTrigger();

    EXPECT_EQ(MyVA_SIMV_Mode->Window, eSpontWindow);
    EXPECT_EQ(MyVA_SIMV_Mode->InhPhaseType, eMandInhVABreathPhase);
}
TEST_F(VA_SIMV_ModeUnitMock, ProcessPtInhTrigger_1)
{
    MyVA_SIMV_Mode->Window = eSpontWindow;

    MyVA_SIMV_Mode->ProcessPtInhTrigger();
}

//test for void VA_SIMV_Mode::ProcessTriggerID (E_TriggerId id)
TEST_F(VA_SIMV_ModeUnitMock, ProcessTriggerID)
{
    //no source, not need to test
}

#endif //end define BD_MODE_UNIT_TEST
