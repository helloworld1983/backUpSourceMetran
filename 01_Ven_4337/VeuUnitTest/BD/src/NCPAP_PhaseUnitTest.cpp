#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "NCPAP_Phase.h"
#include "DebugTrace.h"

using namespace ::testing;
#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"

#define ONE_SECOND 1000
NCPAP_Phase* NCPAP_Phase::S_Instance = NULL;

NCPAP_Phase* NCPAP_Phase::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new NCPAP_Phase();
        ASSERTION(S_Instance != NULL);
    }
    return(S_Instance);

}   // end S_GetInstance()
NCPAP_Phase::NCPAP_Phase(): Phase()
{
    timer_1s = 0;
}
void NCPAP_Phase::Start()
{
    MgrBdSettingPhaseInNCPAPSettings();
    LONG FlowSetting = BDMgrBdSettingGetCurrentValue(eNasalCPAPFlow);
    NCPAPControllerSetTargetFlow(FlowSetting);

//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtrEnableTrigger(eExtremeHighPEEPTrigger);
    timer_1s = 0;
}
void NCPAP_Phase::End()
{
    timer_1s = 0;

//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    triggerMgrPtrDisableTrigger(eExtremeHighPEEPTrigger);
}
void NCPAP_Phase::ProcessBdCycle()
{
    MgrBdSettingPhaseInNCPAPSettings();
    LONG FlowSetting = BDMgrBdSettingGetCurrentValue(eNasalCPAPFlow);
    NCPAPControllerSetTargetFlow(FlowSetting);

    if(0 == timer_1s)
    {
        MgrBreathDataProcessNCPAP();
    }

    timer_1s = timer_1s + 2;
    //2ms
    if(timer_1s >= ONE_SECOND)
    {
        timer_1s = 0;
    }
}

class NCPAP_PhaseUnitMock : public BDTestFixture
{
public:
    NCPAP_PhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~NCPAP_PhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MyNCPAP_Phase = new NCPAP_Phase();
    }
    virtual void TearDown(void)
    {
        delete MyNCPAP_Phase;
    }
    NCPAP_Phase *MyNCPAP_Phase;
};

//test for NCPAP_Phase* NCPAP_Phase::S_GetInstance(void)
TEST_F(NCPAP_PhaseUnitMock, S_GetInstance)
{
    MyNCPAP_Phase->S_GetInstance();

    EXPECT_NE(MyNCPAP_Phase->S_Instance, (void*)0);
}

//test for NCPAP_Phase::NCPAP_Phase(): Phase()
TEST_F(NCPAP_PhaseUnitMock, NCPAP_Phase)
{
    EXPECT_EQ(MyNCPAP_Phase->timer_1s, 0);
}

//test for void NCPAP_Phase::Start()
TEST_F(NCPAP_PhaseUnitMock, Start)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingPhaseInNCPAPSettings()).Times(1);

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eNasalCPAPFlow)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_StepMotor, NCPAPControllerSetTargetFlow(10)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eExtremeHighPEEPTrigger)).Times(1);

    MyNCPAP_Phase->Start();

    EXPECT_EQ(MyNCPAP_Phase->timer_1s, 0);
}

//test for void NCPAP_Phase::End()
TEST_F(NCPAP_PhaseUnitMock, End)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eExtremeHighPEEPTrigger)).Times(1);

    MyNCPAP_Phase->End();

    EXPECT_EQ(MyNCPAP_Phase->timer_1s, 0);
}

//test for void NCPAP_Phase::ProcessBdCycle()
TEST_F(NCPAP_PhaseUnitMock, ProcessBdCycle_0)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingPhaseInNCPAPSettings()).Times(1);
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eNasalCPAPFlow)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_StepMotor, NCPAPControllerSetTargetFlow(10)).Times(1);

    MyNCPAP_Phase->timer_1s = 0;

    EXPECT_CALL(*_MgrBdSetting, MgrBreathDataProcessNCPAP()).Times(1);


    MyNCPAP_Phase->ProcessBdCycle();
    EXPECT_EQ(MyNCPAP_Phase->timer_1s, 2);
}
TEST_F(NCPAP_PhaseUnitMock, ProcessBdCycle_1)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingPhaseInNCPAPSettings()).Times(1);
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eNasalCPAPFlow)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_StepMotor, NCPAPControllerSetTargetFlow(10)).Times(1);

    MyNCPAP_Phase->timer_1s = ONE_SECOND;


    MyNCPAP_Phase->ProcessBdCycle();
    EXPECT_EQ(MyNCPAP_Phase->timer_1s, 0);
}

#endif //end define BD_PHASE_UNIT_TEST
