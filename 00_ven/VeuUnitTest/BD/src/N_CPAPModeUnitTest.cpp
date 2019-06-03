#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "N_CPAPMode.h"
#include "DebugTrace.h"

#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "StepMotorMockHeader.h"

using namespace ::testing;

N_CPAPMode* N_CPAPMode::S_Instance = NULL;
const int INIT_UPDATE_STAT_TIME = 0;
N_CPAPMode* N_CPAPMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new N_CPAPMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()

N_CPAPMode::N_CPAPMode():NormBreathingMode()
{
}
void N_CPAPMode::Enter(E_VentilationMode fromModeId)
{
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    //enable trigger
    triggerMgrPtrEnableTrigger(eModeChangeTrigger);
    triggerMgrPtrEnableTrigger(eAlarmResetOpReqTrigger);
    triggerMgrPtrEnableTrigger(eTotalGasLossTrigger);
    triggerMgrPtrEnableTriggerWith(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);
    triggerMgrPtrEnableTrigger(eHardwareErrorAlarmTriger);

    PhaseMgrProcessNewPhase(eNCPAPPhase);

    NCPAPControllerResetIMVStaticVariables();
    AirFlowControllerEnable();
    O2FlowControllerEnable();
    NCPAPControllerEnable();

    //Close safe valve
    SafetyValveSetDesiredState(eClose);
}
void N_CPAPMode::Exit()
{
    AirFlowControllerDisable();
    O2FlowControllerDisable();
    NCPAPControllerDisable();
    SafetyValveSetDesiredState(eOpen); //eOn
}

class N_CPAPModeUnitMock : public BDTestFixture
{
public:
    N_CPAPModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~N_CPAPModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyN_CPAPMode = new N_CPAPMode();
    }
    virtual void TearDown(void)
    {
        delete MyN_CPAPMode;
    }
    N_CPAPMode *MyN_CPAPMode;
};

//test for N_CPAPMode* N_CPAPMode::S_GetInstance(void)
TEST_F(N_CPAPModeUnitMock, S_GetInstance)
{
    MyN_CPAPMode->S_GetInstance();

    EXPECT_NE(MyN_CPAPMode->S_Instance, (void*)0);
}

//test for N_CPAPMode::N_CPAPMode():NormBreathingMode()
TEST_F(N_CPAPModeUnitMock, N_CPAPMode)
{
    //no source code, not need to test
}

//test for void N_CPAPMode::Enter(E_VentilationMode fromModeId)
TEST_F(N_CPAPModeUnitMock, Enter)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eModeChangeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eAlarmResetOpReqTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eTotalGasLossTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eHardwareErrorAlarmTriger)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eNCPAPPhase));

    EXPECT_CALL(*_StepMotor, NCPAPControllerResetIMVStaticVariables()).Times(1);
    EXPECT_CALL(*_StepMotor, AirFlowControllerEnable()).Times(1);
    EXPECT_CALL(*_StepMotor, O2FlowControllerEnable()).Times(1);
    EXPECT_CALL(*_StepMotor, NCPAPControllerEnable()).Times(1);

    EXPECT_CALL(*_StepMotor, SafetyValveSetDesiredState(eClose)).Times(1);

    MyN_CPAPMode->Enter(eHardwareErrorMode);
}

//test for void N_CPAPMode::Exit()
TEST_F(N_CPAPModeUnitMock, Exit)
{
    EXPECT_CALL(*_StepMotor, AirFlowControllerDisable()).Times(1);
    EXPECT_CALL(*_StepMotor, O2FlowControllerDisable()).Times(1);
    EXPECT_CALL(*_StepMotor, NCPAPControllerDisable()).Times(1);

    EXPECT_CALL(*_StepMotor, SafetyValveSetDesiredState(eOpen)).Times(1);

    MyN_CPAPMode->Exit();
}
#endif //end define BD_MODE_UNIT_TEST
