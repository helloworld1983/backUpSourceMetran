#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "APRV_SupportedInhPhase_InPLow.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "AlarmMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"

UNSIGNED APRV_SupportedInhPhase_InPLow::S_InhStartTime = 0;
UNSIGNED APRV_SupportedInhPhase_InPLow::S_TimeElapsed = 0;
UNSIGNED APRV_SupportedInhPhase_InPLow::S_Time = 0;

APRV_SupportedInhPhase_InPLow::APRV_SupportedInhPhase_InPLow
(
    E_BdSettingId targetPressId,
    E_BdSettingId offsetPressId
    )
   // : PressInhPhase(targetPressId, offsetPressId)
{

}   // end APRV_SupportedInhPhase_InPLow()
void APRV_SupportedInhPhase_InPLow::Start(void)
{
    E_BreathPhaseIndicator inhType;  // Spontaneous or Supported
    LONG desiredPress;

    // Invoke base class method to perform processing common to all pressure
    // based inhalation phases.
//    PressInhPhase::Start();
//    S_InhStartTime = SystemClock::Retrieve();
    S_TimeElapsed = 0;


    inhType = eSpontaneous;
    TriggerMgrEnableTrigger(ePtExhFlowSuppTrigger);


    desiredPress = mgrBdSettingPtrGetCurrentValue(eAPRVLowInhPress);
    MgrControllerCMVServoControllerSetInhalationTarget(desiredPress/10);

    TriggerMgrEnableTrigger(eItLongAlarmTimeTrigger);
    TriggerMgrEnableTrigger(eApneaAlarmTimeTrigger);
    BDAlarmTaskSetIsActive(eApnea, eNotActive);

    MgrBreathDataAPRV_InhPhaseStart(inhType);
}   // end Start()
void APRV_SupportedInhPhase_InPLow::ProcessBdCycle(void)
{

}   // end ProcessBdCycle()
void APRV_SupportedInhPhase_InPLow::End(void)
{
//    TriggerMgr* triggerMgrPtr;  // pointer to the TriggerMgr

    // Get handle to the TriggerMgr object
//    triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable triggers
    triggerMgrPtrDisableTrigger(ePtExhFlowSuppTrigger);
    triggerMgrPtrDisableTrigger(ePtExhFlowSpontTrigger);
    triggerMgrPtrDisableTrigger(eItLongAlarmTimeTrigger);
    triggerMgrPtrDisableTrigger(eHipAlarmTrigger);
    S_TimeElapsed = APRV_SupportedInhPhase_InPLow::S_CalcTimeSincePhaseStart();
    S_Time += S_TimeElapsed;
}   // end End()
UNSIGNED APRV_SupportedInhPhase_InPLow::S_CalcTimeSincePhaseStart(void)
{
    // Return the time elapsed since the start of the exhalation phase.

//    return(CalcElapsedTime(S_InhStartTime));


}   // end S_CalcTimeSincePhaseStart()
UNSIGNED APRV_SupportedInhPhase_InPLow::GetTimeDuration (void)
{
    return S_Time;
}
void APRV_SupportedInhPhase_InPLow::ResetTimeDuration (void)
{
    S_Time = 0;
}
UNSIGNED APRV_SupportedInhPhase_InPLow::GetTimeElapsed (void)
{
    return S_TimeElapsed;
}

class APRV_SupportedInhPhase_InPLowUnitMock : public BDTestFixture
{
public:
    APRV_SupportedInhPhase_InPLowUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~APRV_SupportedInhPhase_InPLowUnitMock(void){}
    virtual void SetUp(void)
    {
        MyAPRV_SupportedInhPhase_InPLow = new APRV_SupportedInhPhase_InPLow();
    }
    virtual void TearDown(void)
    {
        delete MyAPRV_SupportedInhPhase_InPLow;
    }
    APRV_SupportedInhPhase_InPLow *MyAPRV_SupportedInhPhase_InPLow;
};

//test for APRV_SupportedInhPhase_InPHigh::APRV_SupportedInhPhase_InPHigh
TEST_F(APRV_SupportedInhPhase_InPLowUnitMock, APRV_SupportedInhPhase_InPHigh)
{
    //no source, not need to test
}

//test for void APRV_SupportedInhPhase_InPHigh::Start(void)
TEST_F(APRV_SupportedInhPhase_InPLowUnitMock, Start_0)
{
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(ePtExhFlowSuppTrigger)).Times(1);

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eAPRVLowInhPress)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerSetInhalationTarget(10/10)).Times(1);

    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(eItLongAlarmTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(eApneaAlarmTimeTrigger)).Times(1);

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eApnea, eNotActive));

    EXPECT_CALL(*_BreathMgr, MgrBreathDataAPRV_InhPhaseStart(eSpontaneous)).Times(1);

    MyAPRV_SupportedInhPhase_InPLow->Start();
}

//test for UNSIGNED APRV_SupportedInhPhase_InPHigh::S_CalcTimeSincePhaseStart(void)
TEST_F(APRV_SupportedInhPhase_InPLowUnitMock, S_CalcTimeSincePhaseStart)
{
    //all calling function is tested, not need to test
}

//test for UNSIGNED APRV_SupportedInhPhase_InPHigh::GetTimeDuration (void
TEST_F(APRV_SupportedInhPhase_InPLowUnitMock, GetTimeDuration)
{
    MyAPRV_SupportedInhPhase_InPLow->S_Time = 10;

    EXPECT_EQ(MyAPRV_SupportedInhPhase_InPLow->GetTimeDuration(), 10);
}

//test for void APRV_SupportedInhPhase_InPHigh::ResetTimeDuration (void)
TEST_F(APRV_SupportedInhPhase_InPLowUnitMock, ResetTimeDuration)
{
    MyAPRV_SupportedInhPhase_InPLow->S_Time = 10;

    MyAPRV_SupportedInhPhase_InPLow->ResetTimeDuration();

    EXPECT_EQ(MyAPRV_SupportedInhPhase_InPLow->S_Time, 0);
}

//test for UNSIGNED APRV_SupportedInhPhase_InPHigh::GetTimeElapsed (void)
TEST_F(APRV_SupportedInhPhase_InPLowUnitMock, GetTimeElapsed)
{
    MyAPRV_SupportedInhPhase_InPLow->S_TimeElapsed = 12;

    EXPECT_EQ(MyAPRV_SupportedInhPhase_InPLow->S_TimeElapsed, 12);
}

#endif //end define BD_PHASE_UNIT_TEST
