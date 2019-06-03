#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "SupportedInhPhase.h"
#include "DebugTrace.h"

using namespace ::testing;
#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "ModeMgrMockHeader.h"
#include "StepMotorMockHeader.h"

SupportedInhPhase::SupportedInhPhase (E_BdSettingId targetPressId,
                                      E_BdSettingId baselinePressId) :
        PressInhPhase (targetPressId, baselinePressId)
{

}   // end SupportedInhPhase()
void SupportedInhPhase::Start (void)
{
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();// pointer to the TriggerMgr object
    E_BreathPhaseIndicator inhType;  // Spontaneous or Supported
    LONG desiredPress;

    // Invoke base class method to perform processing common to all pressure
    // based inhalation phases.
//    PressInhPhase::Start();

    // Enable the Inh Pressure Controller!  Passing target and
    // initial pressure sets up the gain factors.
    LONG PS = mgrBdSettingPtrGetCurrentValue(eSupportPress);

    //for new VA
#ifdef VA_WITH_PS //for new VA
    E_PhaseId id = (PhaseMgr::S_GetInstance())->GetCurrentPhaseId();
    if(id == ePsvInhVABreathPhase){
        PS = InhVABreathPhaseCompPIP()-BaseLine;
    }
    else{
        PS = MgrBdSettingGetCurrentValue(eSupportPress);
    }
#endif
    //for new VA

    if (PS > 0)
    {
        inhType = eSupported;
        desiredPress = PS + BaseLine;
        triggerMgrPtrEnableTrigger(ePtExhFlowSuppTrigger);

    }
    else
    {
        inhType = eSpontaneous;
        desiredPress = BaseLine;
        triggerMgrPtrEnableTrigger(ePtExhFlowSpontTrigger);
    }

    MgrControllerCMVServoControllerSetInhalationTarget(desiredPress/10);
    MgrControllerVCVServoControllerSetInhalationTarget(desiredPress/10);
    MgrControllerVCVServoControllerSetSepecificPhase();

    E_VentilationMode currentMode = BDModeMgrGetCurrentModeId();
    if(currentMode == eVaMixedMode)
    {
        InhVABreathPhaseSetPrevInhType();
    }

    // Enable the flow, pressure and time triggers than can terminate a
    // supported inhalation.


    // Update patient data
    MgrBreathDataInhalationStart(inhType);

    TriggerMgrEnableTrigger(eItLongAlarmTimeTrigger);
}   // end Start()
void SupportedInhPhase::End (void)
{
//    TriggerMgr* triggerMgrPtr;  // pointer to the TriggerMgr

    // Get handle to the TriggerMgr object
//    triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable triggers
    triggerMgrPtrDisableTrigger(ePtExhFlowSuppTrigger);
    triggerMgrPtrDisableTrigger(ePtExhFlowSpontTrigger);
    triggerMgrPtrDisableTrigger(eItLongAlarmTimeTrigger);
    triggerMgrPtrDisableTrigger(eHipAlarmTrigger);

}   // end End()

class SupportedInhPhaseUnitMock : public BDTestFixture
{
public:
    SupportedInhPhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~SupportedInhPhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MySupportedInhPhase = new SupportedInhPhase(eNumBdSettings, eNumBdSettings);
    }
    virtual void TearDown(void)
    {
        delete MySupportedInhPhase;
    }
    SupportedInhPhase *MySupportedInhPhase;
};

//test for SupportedInhPhase::SupportedInhPhase
TEST_F(SupportedInhPhaseUnitMock, SupportedInhPhase)
{
    //no source, not need to test
}

//test for void SupportedInhPhase::Start (void)
TEST_F(SupportedInhPhaseUnitMock, Start_0)
{
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eSupportPress)).Times(1).WillOnce(Return(9));
    MySupportedInhPhase->BaseLine = 1;

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(ePtExhFlowSuppTrigger)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerSetInhalationTarget(10/10)).Times(1);
    EXPECT_CALL(*_StepMotor, MgrControllerVCVServoControllerSetInhalationTarget(10/10)).Times(1);
    EXPECT_CALL(*_StepMotor, MgrControllerVCVServoControllerSetSepecificPhase()).Times(1);

    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eVaMixedMode));
    EXPECT_CALL(*_BreathMgr, InhVABreathPhaseSetPrevInhType()).Times(1);

    EXPECT_CALL(*_BreathMgr, MgrBreathDataInhalationStart(eSupported)).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(eItLongAlarmTimeTrigger)).Times(1);

    MySupportedInhPhase->Start();
}
TEST_F(SupportedInhPhaseUnitMock, Start_1)
{
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eSupportPress)).Times(1).WillOnce(Return(0));
    MySupportedInhPhase->BaseLine = 10;

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(ePtExhFlowSpontTrigger)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerSetInhalationTarget(10/10)).Times(1);
    EXPECT_CALL(*_StepMotor, MgrControllerVCVServoControllerSetInhalationTarget(10/10)).Times(1);
    EXPECT_CALL(*_StepMotor, MgrControllerVCVServoControllerSetSepecificPhase()).Times(1);

    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eVaMixedMode));
    EXPECT_CALL(*_BreathMgr, InhVABreathPhaseSetPrevInhType()).Times(1);

    EXPECT_CALL(*_BreathMgr, MgrBreathDataInhalationStart(eSpontaneous)).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(eItLongAlarmTimeTrigger)).Times(1);

    MySupportedInhPhase->Start();
}

//test for void SupportedInhPhase::End (void)
TEST_F(SupportedInhPhaseUnitMock, End)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtExhFlowSuppTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtExhFlowSpontTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eItLongAlarmTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eHipAlarmTrigger)).Times(1);

    MySupportedInhPhase->End();
}

#endif //end define BD_PHASE_UNIT_TEST
