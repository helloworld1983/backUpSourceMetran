#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "NIV_Mode.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "StepMotorMockHeader.h"

NIV_Mode* NIV_Mode::S_Instance = NULL;

NIV_Mode* NIV_Mode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new NIV_Mode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}
NIV_Mode::NIV_Mode(void): MixedMode()
{
}
void NIV_Mode::Enter(E_VentilationMode fromModeId)
{
//    MixedMode::Enter(fromModeId);

    PhaseMgrProcessNewVentType(ePressureVentilation);

    MgrControllerCMVServoControllerEnable();

    PhaseMgrProcessNewTriggerType(ePressureTrigger);

    PhaseMgrProcessNewPhase(eExhPhase);

    triggerMgrPtrDisableTrigger(eBpTimeTrigger);

    UNSIGNED breathPeriod =  triggerMgrPtrGetTimePeriod(eNIVBpTimeTrigger);

    triggerMgrPtrEnableTriggerWith(eNIVBpTimeTrigger,breathPeriod);

    triggerMgrPtrDisableTrigger(ePtInhFlowTrigger);
}
void NIV_Mode::ProcessManualInhOpReqTrigger()
{
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // Operator initiated inhalation triggers should only fire
    // during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    TriggerMgrReEnableTrigger(eNIVBpTimeTrigger);

    // Start a mandatory inhalation.
    phaseMgrPtrProcessNewPhase(eMandInhPhase);

    Window = eMandWindow;
}
void NIV_Mode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eManualInhOpReqTrigger:
            ProcessManualInhOpReqTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

class NIV_ModeUnitMock : public BDTestFixture
{
public:
    NIV_ModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~NIV_ModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyNIV_Mode = new NIV_Mode();
    }
    virtual void TearDown(void)
    {
        delete MyNIV_Mode;
    }
    NIV_Mode *MyNIV_Mode;
};

//test for NIV_Mode* NIV_Mode::S_GetInstance(void)
TEST_F(NIV_ModeUnitMock, S_GetInstance)
{
    MyNIV_Mode->S_GetInstance();

    EXPECT_NE(MyNIV_Mode->S_Instance, (void*)0);
}

//test for NIV_Mode::NIV_Mode(void): MixedMode()
TEST_F(NIV_ModeUnitMock, NIV_Mode)
{
    //no source, not need to test
}

//test for void NIV_Mode::Enter(E_VentilationMode fromModeId)
TEST_F(NIV_ModeUnitMock, Enter)
{
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewVentType(ePressureVentilation)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerEnable()).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewTriggerType(ePressureTrigger)).Times(1);
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eExhPhase)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eBpTimeTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod(eNIVBpTimeTrigger)).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eNIVBpTimeTrigger,2)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtInhFlowTrigger)).Times(1);


    MyNIV_Mode->Enter(eHardwareErrorMode);
}

//test for void NIV_Mode::ProcessManualInhOpReqTrigger()
TEST_F(NIV_ModeUnitMock, ProcessManualInhOpReqTrigger)
{
    EXPECT_CALL(*_TriggerMgr, TriggerMgrReEnableTrigger(eNIVBpTimeTrigger)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eMandInhPhase)).Times(1);

    MyNIV_Mode->ProcessManualInhOpReqTrigger();

    EXPECT_EQ(MyNIV_Mode->Window, eMandWindow);
}

//test for void NIV_Mode::ProcessTriggerID (E_TriggerId id)
TEST_F(NIV_ModeUnitMock, ProcessTriggerID)
{
    //all calling function is tested, not need to test
}

#endif //end define BD_MODE_UNIT_TEST
