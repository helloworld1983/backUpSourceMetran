#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "NonVenMode.h"
#include "DebugTrace.h"

#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "StepMotorMockHeader.h"

const int INIT_UPDATE_STAT_TIME = 0;

using namespace ::testing;

NonVenMode::NonVenMode (E_Bool doesManualReset) :
        Mode (doesManualReset)
{

}   // end NonVentMode()
void NonVenMode::Enter (E_VentilationMode)
{
    // Open the safety valve.
    SafetyValveSetStateDirect(eOpen);

    // Initialize pointer to the TriggerMgr
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Put the ventilator into the alarm phase.
    phaseMgrPtrProcessNewPhase (eAlarmPhase);

    // Disable all triggers.  This call must be made after forcing the vent
    // into the non-ventilation phase above because depending on the phase
    // transitioned from, there may be some triggers left enabled.
    triggerMgrPtrDisableAllTriggers ();

    // Enable the Update Status trigger which causes the update of the
    // status of the alarms associated with the active Alarm mode.
    triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);

    // Suspend auto-zero scheduling.
//    InhAzSchedulerPtr->Suspend();
//    ExhAzSchedulerPtr->Suspend();

}   // end Enter()
void NonVenMode::Exit (void)
{
    // Close the safety valve.
    SafetyValveSetStateDirect(eClose);

    // Initialize pointer to the TriggerMgr
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();

    // Disable triggers.
    triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger);

    // Resume auto-zero scheduling.
//    InhAzSchedulerPtr->Resume();
//    ExhAzSchedulerPtr->Resume();

}   // end Exit()

class NonVenModeUnitMock : public BDTestFixture
{
public:
    NonVenModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~NonVenModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyNonVenMode = new NonVenMode();
    }
    virtual void TearDown(void)
    {
        delete MyNonVenMode;
    }
    NonVenMode *MyNonVenMode;
};

//test for NonVenMode::NonVenMode (E_Bool doesManualReset)
TEST_F(NonVenModeUnitMock, NonVenMode)
{
    //no source code, not need to test
}

//test for void NonVenMode::Enter (E_VentilationMode)
TEST_F(NonVenModeUnitMock, Enter)
{
    EXPECT_CALL(*_StepMotor, SafetyValveSetStateDirect(eOpen)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eAlarmPhase)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableAllTriggers ()).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME)).Times(1);

    MyNonVenMode->Enter(eHardwareErrorMode);
}

//test for void NonVenMode::Exit (void)
TEST_F(NonVenModeUnitMock, Exit)
{
    EXPECT_CALL(*_StepMotor, SafetyValveSetStateDirect(eClose)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger)).Times(1);

    MyNonVenMode->Exit();
}
#endif //end define BD_MODE_UNIT_TEST

