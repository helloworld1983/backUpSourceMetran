#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "AvMode.h"
#include "DebugTrace.h"
#include "CommonTypes.h"

using namespace ::testing;

#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "AlarmMockHeader.h"
#include "TriggerMgrMockHeader.h"

const int INIT_UPDATE_STAT_TIME = 0;

AvMode* AvMode::S_Instance = NULL;
AvMode* AvMode::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new AvMode ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

void AvMode::Enter (E_VentilationMode fromModeId)
{
    E_PhaseId phaseId;      // current phase ID

    // Initialize pointers to the TriggerMgr object and the PhaseMgr object
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Enable the reset triggers.
    triggerMgrPtrEnableTrigger (eAlarmResetOpReqTrigger);
    triggerMgrPtrEnableTrigger (eApneaAutoResetTrigger);

    // Enable the total gas loss trigger
    triggerMgrPtrEnableTrigger (eTotalGasLossTrigger);

    // Enable the alarm status update trigger
    triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);

    // Processing depends on the mode being exited.
    switch (fromModeId)
    {
        case eSpontaneousMode:
        case eMixedMode:
        case eNoGasMode:
        case eOcclusionMode:

            phaseId = phaseMgrPtrGetCurrentPhaseId ();

            // The mode change should only occur in exhalation.
            ASSERTION((eExhPhase == phaseId) || (eExhHoldPhase == phaseId) ||
                       (eAlarmPhase == phaseId));

            // Deliver a mandatory inhalation
            phaseMgrPtrProcessNewPhase (eMandInhPhase);

            // Enable the breath period trigger.
            triggerMgrPtrEnableTrigger (eAvBpTimeTrigger);
            break;

        case eStandbyMode:
        case eApneaMode:
            // Cannot enter AV from these modes.
            ASSERTION( (fromModeId != eStartUpMode) &&
                       (fromModeId != eApneaMode));
            break;

        default:
            // Illegal Mode
            ASSERTION(eFalse);
            break;

    }   // end switch()

    // Notify Alarms of mode change induced auto-resets.  Any alarm
    // that applies only to a spontaneous breath must be auto-reset
    // on entry into AV.
    ReportModeChangeInducedReset ();

    // Notify AlarmStat objects that AV is active
    BDAlarmTaskSetIsActive (eApnea, eActive);

}   // end Enter()

void AvMode::Exit (void)
{

    // Get a handle to the TriggerMgr object
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();

    // Disable AV Mode specific triggers.
    triggerMgrPtrDisableTrigger (eAvBpTimeTrigger);
    triggerMgrPtrDisableTrigger (eAlarmResetOpReqTrigger);
    triggerMgrPtrDisableTrigger (eApneaAutoResetTrigger);
    triggerMgrPtrDisableTrigger (eTotalGasLossTrigger);
    triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger);

}   // end Exit()

AvMode::AvMode (void) :
        AlarmBreathingMode (eTrue)
{

}   // end AvMode()

void AvMode::ProcessPtInhTrigger (void)
{
    // Initialize pointers to the PhaseMgr and TriggerMgr objects
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();

    // Patient inhalation triggers should only fire during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Re-enable the breath period time trigger.  This call will
    // cause the breath period timer to be restarted.
    TriggerMgrReEnableTrigger (eAvBpTimeTrigger);

    // Start a mandatory inhalation.
    phaseMgrPtrProcessNewPhase (eMandInhPhase);

    // Notify the ApneaAutoResetTrigger of the PIM.  It does not matter
    // if the trigger ID is flow or pressure as long as it is patient
    // initiated.
    ApneaAutoResetTriggerUpdateState (ePtInhPressTrigger);

}   // end ProcessPtInhTrigger()

void AvMode::ProcessAvBpTimeTrigger (void)
{
    // Initialize pointers to the PhaseMgr.
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Check if the phase is exhalation; the Apnea Breath Period Timer should
    // only expire in exhalation.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Trigger a new inhalation.  All inhalations in AV are mandatory.
    phaseMgrPtrProcessNewPhase (eMandInhPhase);

    // Notify the ApneaAutoResetTrigger of the VIM
    ApneaAutoResetTriggerUpdateState (eAvBpTimeTrigger);

}   // end ProcessAvBpTimeTrigger()

void AvMode::ProcessApneaAutoResetTrigger (void)
{
    // The Apnea Reset Trigger should only fire during exhalation.
    ASSERTION((PhaseMgr::S_GetInstance())->GetCurrentPhaseId() == eExhPhase);

    ModeMgrReturnToLastMode ();

}   // end ProcessApneaAutoResetTrigger()

void AvMode::UpdateAlarmStatus (void)
{
    SHORT status = 0;
//    ApneaAutoResetTrigger* apneaArTriggerPtr;
//    AlarmResetOpReqTrigger* alarmResetTriggerPtr;
//    TriggerMgr* triggerMgrPtr;

//    apneaArTriggerPtr = ApneaAutoResetTrigger::S_GetInstance ();
//    alarmResetTriggerPtr = AlarmResetOpReqTrigger::S_GetInstance ();

    // As long as a reset is not pending, notify Alarms that Apnea is active.
    if((apneaArTriggerPtrGetState () != eArmed)
            && (alarmResetTriggerPtrGetIsArmed () == eFalse))
    {
        // Notify AlarmStat objects that AV is active
        status += (SHORT) BDAlarmTaskSetIsActive (eApnea, eActive);
    }

    // If the ventilator is in Apnea Ventilation, the occlusion alarm is
    // not active, the Total Gas Loss alarm is not active and the IT Too Long
    // alarm is not active.
    status += (SHORT) BDAlarmTaskSetIsActive (eOcclusion, eNotActive);
    status += (SHORT) BDAlarmTaskSetIsActive (eGasSuppliesLost, eNotActive);
    status += (SHORT) BDAlarmTaskSetIsActive (eITimeTooLong, eNotActive);

    // If any of the status updates was unsuccessful, reschedule the next update
    // for the short interval
    if(status != (SHORT) eSuccess)
    {
//        triggerMgrPtr = TriggerMgr::S_GetInstance ();
        triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger);
        triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);
    }

}   // end UpdateAlarmStatus()

void AvMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case ePtInhTrigger:
            ProcessPtInhTrigger();
            break;

        case eAvBpTimeTrigger:
            ProcessAvBpTimeTrigger();
            break;

        case eApneaAutoResetTrigger:
            ProcessApneaAutoResetTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

void AvMode::ReportModeChangeInducedReset (void)
{
    // Inhalation Too Long cannot occur in AV.  Reset it on entry.
    BDAlarmTaskSetIsActive (eITimeTooLong, eNotActive);

}   // end ReportModeChangeInducedReset()

class AvModeUnitMock : public BDTestFixture
{
public:
    AvModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~AvModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyAvMode = new AvMode();
    }
    virtual void TearDown(void)
    {
        delete MyAvMode;
    }
    AvMode *MyAvMode;
};

//test for AvMode* AvMode::S_GetInstance (void)
TEST_F(AvModeUnitMock, S_GetInstance)
{
    MyAvMode->S_GetInstance();

    EXPECT_NE(MyAvMode->S_Instance, (void*)0);
}

//test for void AvMode::Enter (E_VentilationMode fromModeId)
TEST_F(AvModeUnitMock, Enter_0)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eAlarmResetOpReqTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eApneaAutoResetTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eTotalGasLossTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1);
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eAvBpTimeTrigger)).Times(1);

    MyAvMode->Enter(eSpontaneousMode);
}
TEST_F(AvModeUnitMock, Enter_1)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eAlarmResetOpReqTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eApneaAutoResetTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eTotalGasLossTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1);
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eAvBpTimeTrigger)).Times(1);

    MyAvMode->Enter(eMixedMode);
}
TEST_F(AvModeUnitMock, Enter_2)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eAlarmResetOpReqTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eApneaAutoResetTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eTotalGasLossTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1);
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eAvBpTimeTrigger)).Times(1);

    MyAvMode->Enter(eNoGasMode);
}
TEST_F(AvModeUnitMock, Enter_3)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eAlarmResetOpReqTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eApneaAutoResetTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eTotalGasLossTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1);
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eAvBpTimeTrigger)).Times(1);

    MyAvMode->Enter(eOcclusionMode);
}

//test for void AvMode::Exit (void)
TEST_F(AvModeUnitMock, Exit)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eAvBpTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eAlarmResetOpReqTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eApneaAutoResetTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eTotalGasLossTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger)).Times(1);

    MyAvMode->Exit();
}

//test for AvMode::AvMode (void) :
TEST_F(AvModeUnitMock, AvMode)
{
    //no source, not need to test
}

//test for void AvMode::ProcessPtInhTrigger (void)
TEST_F(AvModeUnitMock, ProcessPtInhTrigger)
{
    EXPECT_CALL(*_TriggerMgr, TriggerMgrReEnableTrigger (eAvBpTimeTrigger)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);

    EXPECT_CALL(*_TriggerMgr, ApneaAutoResetTriggerUpdateState (ePtInhPressTrigger)).Times(1);

    MyAvMode->ProcessPtInhTrigger();
}

//test for void AvMode::ProcessAvBpTimeTrigger (void)
TEST_F(AvModeUnitMock, ProcessAvBpTimeTrigger)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);

    EXPECT_CALL(*_TriggerMgr, ApneaAutoResetTriggerUpdateState (eAvBpTimeTrigger)).Times(1);

    MyAvMode->ProcessAvBpTimeTrigger();
}

//test for void AvMode::ProcessApneaAutoResetTrigger (void)
TEST_F(AvModeUnitMock, ProcessApneaAutoResetTrigger)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrReturnToLastMode ()).Times(1);

    MyAvMode->ProcessApneaAutoResetTrigger();
}

//test for void AvMode::UpdateAlarmStatus (void)
TEST_F(AvModeUnitMock, UpdateAlarmStatus)
{
    EXPECT_CALL(*_Alarm, apneaArTriggerPtrGetState()).Times(1).WillOnce(Return(eWaiting1));
    EXPECT_CALL(*_Alarm, alarmResetTriggerPtrGetIsArmed()).Times(1).WillOnce(Return(eFalse));

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eApnea, eActive)).Times(1);

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eOcclusion, eNotActive)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eGasSuppliesLost, eNotActive)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eITimeTooLong, eNotActive)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME)).Times(1);

    MyAvMode->UpdateAlarmStatus();

}

//test for void AvMode::ProcessTriggerID (E_TriggerId id)
TEST_F(AvModeUnitMock, ProcessTriggerID)
{
    //all calling function is tested, not need to test
}

//test for void AvMode::ReportModeChangeInducedReset (void)
TEST_F(AvModeUnitMock, ReportModeChangeInducedReset)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eITimeTooLong, eNotActive)).Times(1);

    MyAvMode->ReportModeChangeInducedReset();
}
#endif //end define BD_MODE_UNIT_TEST
