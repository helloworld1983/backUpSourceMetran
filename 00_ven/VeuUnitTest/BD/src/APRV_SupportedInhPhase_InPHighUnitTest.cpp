#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "APRV_SupportedInhPhase_InPHigh.h"

#include "DebugTrace.h"

using namespace ::testing;

#include "AlarmMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"

UNSIGNED APRV_SupportedInhPhase_InPHigh::S_InhStartTime = 0;
UNSIGNED APRV_SupportedInhPhase_InPHigh::S_TimeElapsed = 0;
UNSIGNED APRV_SupportedInhPhase_InPHigh::S_Time = 0;

APRV_SupportedInhPhase_InPHigh::APRV_SupportedInhPhase_InPHigh
(
        E_BdSettingId targetPressId,
        E_BdSettingId offsetPressId
)
{

}   // end APRV_SupportedInhPhase_InPHigh()

void APRV_SupportedInhPhase_InPHigh::Start(void)
{

    //    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();// pointer to the TriggerMgr object
    E_BreathPhaseIndicator inhType;  // Spontaneous or Supported
    LONG desiredPress = 0;

    // Invoke base class method to perform processing common to all pressure
    // based inhalation phases.
    //    PressInhPhase::Start();
    //    S_InhStartTime = SystemClock::Retrieve();
    S_TimeElapsed = 0;

    // Enable the Inh Pressure Controller!  Passing target and
    // initial pressure sets up the gain factors.

    if (mgrBdSettingPtrGetCurrentValue(eSupportPress) > 0)
    {
        desiredPress = mgrBdSettingPtrGetCurrentValue(eAPRVHighInhPress)
                        + mgrBdSettingPtrGetCurrentValue(eSupportPress);

        triggerMgrPtrEnableTrigger(ePtExhFlowSuppTrigger);
    }
    else
    {
        desiredPress = mgrBdSettingPtrGetCurrentValue(eAPRVHighInhPress);

        triggerMgrPtrEnableTrigger(ePtExhFlowSpontTrigger);

    }

    inhType = eSupported;

    MgrControllerCMVServoControllerSetInhalationTarget(desiredPress/10);

    // Enable the flow, pressure and time triggers than can terminate a
    // supported inhalation.

    // Update patient data
    MgrBreathDataAPRV_InhPhaseStart(inhType);

    TriggerMgrEnableTrigger(eItLongAlarmTimeTrigger);

    TriggerMgrEnableTrigger(eApneaAlarmTimeTrigger);


    triggerMgrPtrEnableTrigger(eHip2AlarmTrigger);

    BDAlarmTaskSetIsActive(eApnea, eNotActive);


}   // end Start()

void APRV_SupportedInhPhase_InPHigh::ProcessBdCycle(void)
{

}   // end ProcessBdCycle()
void APRV_SupportedInhPhase_InPHigh::End(void)
{
    //    TriggerMgr* triggerMgrPtr;  // pointer to the TriggerMgr

    // Get handle to the TriggerMgr object
    //    triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable triggers
    triggerMgrPtrDisableTrigger(ePtExhFlowSuppTrigger);

    triggerMgrPtrDisableTrigger(ePtExhFlowSpontTrigger);

    triggerMgrPtrDisableTrigger(eItLongAlarmTimeTrigger);

    triggerMgrPtrDisableTrigger(eHipAlarmTrigger);

    triggerMgrPtrDisableTrigger(eHip2AlarmTrigger);

    //    S_TimeElapsed = APRV_SupportedInhPhase_InPHigh::S_CalcTimeSincePhaseStart();
    S_Time += S_TimeElapsed;

}   // end End()

UNSIGNED APRV_SupportedInhPhase_InPHigh::S_CalcTimeSincePhaseStart(void)
{
    // Return the time elapsed since the start of the exhalation phase.

    //    return(CalcElapsedTime(S_InhStartTime));


}   // end S_CalcTimeSincePhaseStart()
UNSIGNED APRV_SupportedInhPhase_InPHigh::GetTimeDuration (void)
{
    return S_Time;
}
void APRV_SupportedInhPhase_InPHigh::ResetTimeDuration (void)
{
    S_Time = 0;
}
UNSIGNED APRV_SupportedInhPhase_InPHigh::GetTimeElapsed (void)
{
    return S_TimeElapsed;
}

class APRV_SupportedInhPhase_InPHighUnitMock : public BDTestFixture
{
public:
    APRV_SupportedInhPhase_InPHighUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~APRV_SupportedInhPhase_InPHighUnitMock(void){}
    virtual void SetUp(void)
    {
        MyAPRV_SupportedInhPhase_InPHigh = new APRV_SupportedInhPhase_InPHigh();
    }
    virtual void TearDown(void)
    {
        delete MyAPRV_SupportedInhPhase_InPHigh;
    }
    APRV_SupportedInhPhase_InPHigh *MyAPRV_SupportedInhPhase_InPHigh;
};

//test for APRV_SupportedInhPhase_InPHigh::APRV_SupportedInhPhase_InPHigh
TEST_F(APRV_SupportedInhPhase_InPHighUnitMock, APRV_SupportedInhPhase_InPHigh)
{
    //no source, not need to test
}

//test for void APRV_SupportedInhPhase_InPHigh::Start(void)
TEST_F(APRV_SupportedInhPhase_InPHighUnitMock, Start_0)
{
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eSupportPress)).Times(2)
                    .WillOnce(Return(2))
                    .WillOnce(Return(5));

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eAPRVHighInhPress)).Times(1).WillOnce(Return(5));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(ePtExhFlowSuppTrigger)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerSetInhalationTarget(10/10));


    EXPECT_CALL(*_BreathMgr, MgrBreathDataAPRV_InhPhaseStart(eSupported)).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(eItLongAlarmTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(eApneaAlarmTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eHip2AlarmTrigger)).Times(1);

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eApnea, eNotActive)).Times(1);

    MyAPRV_SupportedInhPhase_InPHigh->Start();
}

TEST_F(APRV_SupportedInhPhase_InPHighUnitMock, Start_1)
{
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eSupportPress)).Times(1).WillOnce(Return(0));

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eAPRVHighInhPress)).Times(1).WillOnce(Return(20));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(ePtExhFlowSpontTrigger)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerSetInhalationTarget(20/10));

    EXPECT_CALL(*_BreathMgr, MgrBreathDataAPRV_InhPhaseStart(eSupported)).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(eItLongAlarmTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(eApneaAlarmTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eHip2AlarmTrigger)).Times(1);

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eApnea, eNotActive)).Times(1);

    MyAPRV_SupportedInhPhase_InPHigh->Start();
}

//test for void APRV_SupportedInhPhase_InPHigh::ProcessBdCycle(void)
TEST_F(APRV_SupportedInhPhase_InPHighUnitMock, ProcessBdCycle)
{
    //no cource, not need to test
}

//test for void APRV_SupportedInhPhase_InPHigh::End(void)
TEST_F(APRV_SupportedInhPhase_InPHighUnitMock, End)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtExhFlowSuppTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtExhFlowSpontTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eItLongAlarmTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eHipAlarmTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eHip2AlarmTrigger)).Times(1);

    MyAPRV_SupportedInhPhase_InPHigh->End();

}

//test for UNSIGNED APRV_SupportedInhPhase_InPHigh::S_CalcTimeSincePhaseStart(void)
TEST_F(APRV_SupportedInhPhase_InPHighUnitMock, S_CalcTimeSincePhaseStart)
{
    //all calling function is tested, not need to test
}

//test for UNSIGNED APRV_SupportedInhPhase_InPHigh::GetTimeDuration (void
TEST_F(APRV_SupportedInhPhase_InPHighUnitMock, GetTimeDuration)
{
    MyAPRV_SupportedInhPhase_InPHigh->S_Time = 10;

    EXPECT_EQ(MyAPRV_SupportedInhPhase_InPHigh->GetTimeDuration(), 10);
}

//test for void APRV_SupportedInhPhase_InPHigh::ResetTimeDuration (void)
TEST_F(APRV_SupportedInhPhase_InPHighUnitMock, ResetTimeDuration)
{
    MyAPRV_SupportedInhPhase_InPHigh->S_Time = 10;

    MyAPRV_SupportedInhPhase_InPHigh->ResetTimeDuration();

    EXPECT_EQ(MyAPRV_SupportedInhPhase_InPHigh->S_Time, 0);
}

//test for UNSIGNED APRV_SupportedInhPhase_InPHigh::GetTimeElapsed (void)
TEST_F(APRV_SupportedInhPhase_InPHighUnitMock, GetTimeElapsed)
{
    MyAPRV_SupportedInhPhase_InPHigh->S_TimeElapsed = 12;

    EXPECT_EQ(MyAPRV_SupportedInhPhase_InPHigh->S_TimeElapsed, 12);
}
#endif //end define BD_PHASE_UNIT_TEST
