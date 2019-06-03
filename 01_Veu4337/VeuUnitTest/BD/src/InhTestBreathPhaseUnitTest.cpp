#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "InhTestBreathPhase.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "ModeMgrMockHeader.h"

InhTestBreathPhase::InhTestBreathPhase
(
    E_BdSettingId targetPressId,
    E_BdSettingId offsetPressId
    )
   // : PressInhPhase(targetPressId, offsetPressId)
{

}
void InhTestBreathPhase::Start(void)
{
    E_BreathPhaseIndicator breathType;

    breathType = (ModeS_IsInhPtTriggered() == eTrue) ? eAssisted : eMandatory;

    // Invoke base class method to perform processing common to all
    // pressure based inhalation phases.
//    PressInhPhase::Start();

    TargetPressure = (LONG)((double)TargetPressure*0.7);
    LONG lowPIPAlarmSetting = mgrBdSettingPtrGetCurrentValue(eLipLimit);
    if(TargetPressure < lowPIPAlarmSetting)
        TargetPressure = lowPIPAlarmSetting;
    printf("\n\n %ld TargetPressure = ", TargetPressure);
    MgrControllerCMVServoControllerSetInhalationTarget(TargetPressure/10);

    // Get a handle to the Trigger Manager
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Enable the inhalation time trigger
    triggerMgrPtrEnableTrigger(eMandPressInhTimeTrigger);
    // Update patient data;
    MgrBreathDataInhalationStart(breathType);
    InhVABreathPhaseSetPrevPIP(TargetPressure);
    InhVABreathPhaseClearPIDParameter();

}
void InhTestBreathPhase::End(void)
{
//    TriggerMgr* triggerMgrPtr;  // pointer to the TriggerMgr

//    triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtrDisableTrigger(eMandPressInhTimeTrigger);

    triggerMgrPtrDisableTrigger(eHip2AlarmTrigger);

    triggerMgrPtrDisableTrigger(eHipAlarmTrigger);
}

class InhTestBreathPhaseUnitMock : public BDTestFixture
{
public:
    InhTestBreathPhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~InhTestBreathPhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MyInhTestBreathPhase = new InhTestBreathPhase();
    }
    virtual void TearDown(void)
    {
        delete MyInhTestBreathPhase;
    }
    InhTestBreathPhase *MyInhTestBreathPhase;
};

//test for InhTestBreathPhase::InhTestBreathPhase
TEST_F(InhTestBreathPhaseUnitMock, InhTestBreathPhase)
{
    //no source, not need to test
}

//test for void InhTestBreathPhase::Start(void)
TEST_F(InhTestBreathPhaseUnitMock, Start_0)
{
    EXPECT_CALL(*_ModeMgr, ModeS_IsInhPtTriggered()).Times(1).WillOnce(Return(eTrue));

    MyInhTestBreathPhase->TargetPressure = 10;

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eLipLimit)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerSetInhalationTarget(10/10)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eMandPressInhTimeTrigger)).Times(1);
    EXPECT_CALL(*_BreathMgr, MgrBreathDataInhalationStart(eAssisted)).Times(1);
    EXPECT_CALL(*_BreathMgr, InhVABreathPhaseSetPrevPIP(10)).Times(1);
    EXPECT_CALL(*_BreathMgr, InhVABreathPhaseClearPIDParameter()).Times(1);

    MyInhTestBreathPhase->Start();

}
TEST_F(InhTestBreathPhaseUnitMock, Start_1)
{
    EXPECT_CALL(*_ModeMgr, ModeS_IsInhPtTriggered()).Times(1).WillOnce(Return(eTrue));

    MyInhTestBreathPhase->TargetPressure = 100;

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eLipLimit)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerSetInhalationTarget(69/10)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eMandPressInhTimeTrigger)).Times(1);
    EXPECT_CALL(*_BreathMgr, MgrBreathDataInhalationStart(eAssisted)).Times(1);
    EXPECT_CALL(*_BreathMgr, InhVABreathPhaseSetPrevPIP(69)).Times(1);
    EXPECT_CALL(*_BreathMgr, InhVABreathPhaseClearPIDParameter()).Times(1);

    MyInhTestBreathPhase->Start();

}
TEST_F(InhTestBreathPhaseUnitMock, Start_2)
{
    EXPECT_CALL(*_ModeMgr, ModeS_IsInhPtTriggered()).Times(1).WillOnce(Return(eFalse));

    MyInhTestBreathPhase->TargetPressure = 100;

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eLipLimit)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerSetInhalationTarget(69/10)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eMandPressInhTimeTrigger)).Times(1);
    EXPECT_CALL(*_BreathMgr, MgrBreathDataInhalationStart(eMandatory)).Times(1);
    EXPECT_CALL(*_BreathMgr, InhVABreathPhaseSetPrevPIP(69)).Times(1);
    EXPECT_CALL(*_BreathMgr, InhVABreathPhaseClearPIDParameter()).Times(1);

    MyInhTestBreathPhase->Start();
}

//test for void InhTestBreathPhase::End(void)
TEST_F(InhTestBreathPhaseUnitMock, End)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eMandPressInhTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eHip2AlarmTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eHipAlarmTrigger)).Times(1);

    MyInhTestBreathPhase->End();
}

#endif //end define BD_PHASE_UNIT_TEST
