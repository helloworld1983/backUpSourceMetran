#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "NoGasMode.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "AlarmMockHeader.h"
#include "TriggerMgrMockHeader.h"

NoGasMode* NoGasMode::S_Instance = NULL;
const int INIT_UPDATE_STAT_TIME = 0;

NoGasMode* NoGasMode::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new NoGasMode ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

void NoGasMode::Enter (E_VentilationMode fromModeId)
{
    // Invoke the base class Enter() operation.  This must be done before
    // enabling any triggers.
//    NonVenMode::Enter(fromModeId);

    // Initialize pointer to the TriggerMgr
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();

    // Enable the reset gas loss trigger.
    triggerMgrPtrEnableTrigger (eResetGasLossTrigger);

    // Enable the gas loss trigger.
    triggerMgrPtrEnableTrigger (eTotalGasLossTrigger);

    // If there is a low O2 supply pressure condition, set the total gas loss
    // alarm to the active state.
    if(O2SettingO2SourceFault () == eTrue)
    {
        BDAlarmTaskSetIsActive (eGasSuppliesLost, eActive);
    }

    // If the blower has never been stopped, it indicates that this mode is
    // being entered due to the loss of the air source for the first time.
    if(BlowerSpinDownState == eSpinDownNever)
    {
        // Enable the blower spindown time trigger.  Give the blower 30 seconds
        // to spin down.
        triggerMgrPtrEnableTriggerWith (eBlowerSpinDownTimeTrigger, (30 * TICKS_PER_SEC));
        BlowerSpinDownState = eSpinDownActive;
    }

}   // end Enter()

void NoGasMode::Exit (void)
{
    // Initialize pointer to the TriggerMgr
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();

    // Disable the gas loss triggers.
    triggerMgrPtrDisableTrigger (eResetGasLossTrigger);
    triggerMgrPtrDisableTrigger (eBlowerSpinDownTimeTrigger);
    triggerMgrPtrEnableTrigger (eTotalGasLossTrigger);

    // Update the alarm status to indicate that the total gas loss alarm is
    // not active.
    BDAlarmTaskSetIsActive (eGasSuppliesLost, eNotActive);


    // Invoke the base class Exit() operation.
//    NonVenMode::Exit ();

}   // end Exit()

NoGasMode::NoGasMode (void) :
        NonVenMode (eFalse), BlowerSpinDownState (eSpinDownNever)
{

}   // end NoGasMode()

void NoGasMode::ProcessResetGasLossTrigger (void)
{
//    ModeMgr* modeMgrPtr;

    // If the blower is not spinning down in response to an air source fault,
    // this mode can be exited and ventilation can continue using all O2.
    if(BlowerSpinDownState != eSpinDownActive)
    {
//        modeMgrPtr = ModeMgr::S_GetInstance ();
        modeMgrPtrProcessAutoReset ();
        ASSERTION((PhaseMgr::S_GetInstance())->GetCurrentPhaseId() == eAlarmPhase);
        modeMgrPtrReturnToLastMode ();
    }
    else
    {
        // The O2 Supply pressure came back so remove the Total Gas Loss message.
        // The temporary SVO condition to allow the blower time to spin down is
        // still active.
        BDAlarmTaskSetIsActive (eGasSuppliesLost, eNotActive);
    }

}   // end ProcessResetGasLossTrigger()

void NoGasMode::ProcessBlowerSpinDownTimeTrigger (void)
{
//    ModeMgr* modeMgrPtr;

    // The blower has had time to spin down; update the state.
    BlowerSpinDownState = eSpinDownComplete;

    // If there is not a low O2 supply, then the ventilator can exit this mode
    // and resume ventilation using all O2.
    if(O2SettingO2SourceFault () == eFalse)
    {
//        modeMgrPtr = ModeMgr::S_GetInstance ();
        modeMgrPtrProcessAutoReset ();
        ASSERTION((PhaseMgr::S_GetInstance())->GetCurrentPhaseId() == eAlarmPhase);
        modeMgrPtrReturnToLastMode ();
    }

}   // end ProcessBlowerSpinDownTimeTrigger()

void NoGasMode::UpdateAlarmStatus (void)
{
    E_ReturnStatus status;
//    TriggerMgr* triggerMgrPtr;

    // Make sure that there is an O2 source fault.  It is possible for the
    // ventilator to be in the SVO state temporarily while waiting for the
    // blower to spin down.
    if(O2SettingO2SourceFault () == eTrue)
    {
        status = (E_ReturnStatus)BDAlarmTaskSetIsActive (eGasSuppliesLost, eActive);


        // If the status update was unsuccessful, reschedule the next update
        // for the short interval
        if(status != eSuccess)
        {
//            triggerMgrPtr = TriggerMgr::S_GetInstance ();
            triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger);
            triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);
        }
    }

}  // end UpdateAlarmStatus()
void NoGasMode::ProcessTotalGasLossTrigger (void)
{
    // The O2 Supply Pressure must have dropped if this trigger fires while the
    // mode is already active, so put up the total gas loss message.
    BDAlarmTaskSetIsActive (eGasSuppliesLost, eActive);

}   // end ProcessTotalGasLossTrigger()
void NoGasMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eTotalGasLossTrigger:
            ProcessTotalGasLossTrigger();
            break;

        case eResetGasLossTrigger:
            ProcessResetGasLossTrigger();
            break;

        case eBlowerSpinDownTimeTrigger:
            ProcessBlowerSpinDownTimeTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

class NoGasModeUnitMock : public BDTestFixture
{
public:
    NoGasModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~NoGasModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyNoGasMode = new NoGasMode();
    }
    virtual void TearDown(void)
    {
        delete MyNoGasMode;
    }
    NoGasMode *MyNoGasMode;
};

//test for NoGasMode* NoGasMode::S_GetInstance (void)
TEST_F(NoGasModeUnitMock, S_GetInstance)
{
    MyNoGasMode->S_GetInstance();

    EXPECT_NE((void*)0, MyNoGasMode->S_Instance);
}

//test for void NoGasMode::Enter (E_VentilationMode fromModeId)
TEST_F(NoGasModeUnitMock, Enter)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eResetGasLossTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eTotalGasLossTrigger)).Times(1);

    EXPECT_CALL(*_Alarm, O2SettingO2SourceFault ()).Times(1).WillOnce(Return(eTrue));

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eGasSuppliesLost, eActive)).Times(1);

    MyNoGasMode->BlowerSpinDownState = eSpinDownNever;

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eBlowerSpinDownTimeTrigger, (30 * TICKS_PER_SEC))).Times(1);

    MyNoGasMode->Enter(eHardwareErrorMode);

    EXPECT_EQ(MyNoGasMode->BlowerSpinDownState, eSpinDownActive);
}

//test for void NoGasMode::Exit (void)
TEST_F(NoGasModeUnitMock, Exit)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eResetGasLossTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eBlowerSpinDownTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eTotalGasLossTrigger)).Times(1);

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eGasSuppliesLost, eNotActive)).Times(1);

    MyNoGasMode->Exit();
}

//test for NoGasMode::NoGasMode (void) :
TEST_F(NoGasModeUnitMock, NoGasMode)
{
    //no source, not need to test
}

//test for void NoGasMode::ProcessResetGasLossTrigger (void)
TEST_F(NoGasModeUnitMock, ProcessResetGasLossTrigger_0)
{
    MyNoGasMode->BlowerSpinDownState = eSpinDownActive;

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eGasSuppliesLost, eNotActive)).Times(1);

    MyNoGasMode->ProcessResetGasLossTrigger();
}
TEST_F(NoGasModeUnitMock, ProcessResetGasLossTrigger_1)
{
    MyNoGasMode->BlowerSpinDownState = eSpinDownComplete;

    EXPECT_CALL(*_ModeMgr, modeMgrPtrProcessAutoReset ()).Times(1);
    EXPECT_CALL(*_ModeMgr, modeMgrPtrReturnToLastMode ()).Times(1);

    MyNoGasMode->ProcessResetGasLossTrigger();
}

//test for void NoGasMode::ProcessBlowerSpinDownTimeTrigger (void)
TEST_F(NoGasModeUnitMock, ProcessBlowerSpinDownTimeTrigger)
{
    EXPECT_CALL(*_Alarm, O2SettingO2SourceFault()).Times(1).WillOnce(Return(eFalse));

    EXPECT_CALL(*_ModeMgr, modeMgrPtrProcessAutoReset ()).Times(1);
    EXPECT_CALL(*_ModeMgr, modeMgrPtrReturnToLastMode ()).Times(1);

    MyNoGasMode->ProcessBlowerSpinDownTimeTrigger();

    EXPECT_EQ(MyNoGasMode->BlowerSpinDownState, eSpinDownComplete);
}

//test for void NoGasMode::UpdateAlarmStatus (void)
TEST_F(NoGasModeUnitMock, UpdateAlarmStatus)
{
    EXPECT_CALL(*_Alarm, O2SettingO2SourceFault()).Times(1).WillOnce(Return(eTrue));

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eGasSuppliesLost, eActive)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME));

    MyNoGasMode->UpdateAlarmStatus();
}

//test for void NoGasMode::ProcessTotalGasLossTrigger (void)
TEST_F(NoGasModeUnitMock, ProcessTotalGasLossTrigger)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eGasSuppliesLost, eActive)).Times(1);

    MyNoGasMode->ProcessTotalGasLossTrigger();
}

//test for void NoGasMode::ProcessTriggerID (E_TriggerId id)
TEST_F(NoGasModeUnitMock, ProcessTriggerID)
{
    //all calling function is tested, not need to test
}
#endif //end define BD_MODE_UNIT_TEST
