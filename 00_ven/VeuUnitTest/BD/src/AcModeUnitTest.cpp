#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "DebugTrace.h"
#include "AcMode.h"

#include "PhaseMgrMockHeader.h"
#include "ModeMgrMockHeader.h"
#include "AlarmMockHeader.h"
#include "TriggerMgrMockHeader.h"

using namespace ::testing;

AcMode* AcMode::S_Instance = NULL;
const UNSIGNED INIT_UPDATE_STAT_TIME = 350 / MS_PER_TICK; // 350ms in ticks

AcMode* AcMode::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new AcMode ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

void AcMode::Enter (E_VentilationMode fromModeId, UNSIGNED Time)
{
    UNSIGNED timeSinceInhStart = Time;     // time elaspsed since start of last inh
    UNSIGNED breathPeriod;          // current breath period

    // Initialize pointers to the TriggerMgr object and the PhaseMgr object
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Enable triggers common to all breathing modes such as the Apnea Time
    // trigger and the Mode change trigger.
//    UpdateCommonTriggers ();

    // Enable the exhalation hold request trigger.
    triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger);

    // Processing depends on the mode being exited.
    switch (fromModeId)
    {
        case eStandbyMode:
        case eOcclusionMode:
        case eNoGasMode:

            // When entering AC from Start-Up, go right into
            // inhalation.   All inhalations in A/C are mandatory.
            phaseMgrPtrProcessNewPhase (eMandInhPhase);

            // Enable the Breath Period time trigger.  Since no parameters
            // are being passed, the trigger will enable the breath period
            // timer based on the current respiratory rate setting
            triggerMgrPtrEnableTrigger (eBpTimeTrigger);

            break;

        case eSpontaneousMode:
        case eMixedMode:
        case eApneaMode:

            // The mode change should only occur in exhalation.
            ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

            // Determine the time elasped since the start of the last
            // inhalation phase
//            timeSinceInhStart = InhPhase::S_CalcTimeSincePhaseStart ();

            // The value associated with the Breath Period Time Trigger is
            // the breath period.  Retrieve it.
            breathPeriod = triggerMgrPtrGetTimePeriod (eBpTimeTrigger);

            // When entering A/C from any other mode, figure out time
            // elapsed since the start of the last inhalation.
            if(timeSinceInhStart >= breathPeriod)
            {
                // If the time elapsed exceeds the breath period, transition
                // to inhalation right away.
                phaseMgrPtrProcessNewPhase (eMandInhPhase);

                // Enable the Breath Period time trigger.  Since no parameters
                // are being passed, the trigger will enable the breath period
                // timer based on the current respiratory rate setting
                triggerMgrPtrEnableTrigger (eBpTimeTrigger);

            }
            else
            {
                // If the time elapsed since the start of the last
                // inhalation is less than the breath period, enable the
                // trigger for the difference between the breath period and
                // the time elapsed since the start of the last inhlation.
                triggerMgrPtrEnableTriggerWith (eBpTimeTrigger, 0);//(breathPeriod - timeSinceInhStart));
            }

            break;

        default:
            // Illegal mode
            ASSERTION(eFalse);
            break;
    }

    // Notify Alarms of mode change induced auto-resets.  Any alarm
    // that applies only to a spontaneous breath must be auto-reset
    // on entry into A/C.
    ReportModeChangeInducedReset ();

}   // end Enter()

AcMode::AcMode (void) :
        NormBreathingMode ()
{

}   // end AcMode()

void AcMode::ProcessBpTimeTrigger (void)
{
    // Initialize pointers to the PhaseMgr and TriggerMgr objects
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Check if the phase is exhalation; the Breath Period Timer should only
    // expire in exhalation.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Trigger a new inhalation.  All inhalations in A/C are mandatory.
    phaseMgrPtrProcessNewPhase (eMandInhPhase);

}   // end ProcessBpTimeTrigger()

void AcMode::ProcessPtInhTrigger (void)
{

    // Initialize pointers to the PhaseMgr and TriggerMgr objects
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();

    // Patient inhalation triggers should only fire during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Re-enable the breath period time trigger.  This call will
    // cause the breath period timer to be restarted.
    TriggerMgrReEnableTrigger (eBpTimeTrigger);

    // Start a mandatory inhalation.
    phaseMgrPtrProcessNewPhase (eMandInhPhase);

}   // end ProcessPtInhTrigger()

void AcMode::ProcessExhHoldTrigger (void)
{
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // This trigger should only occur in the exhalation hold phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhHoldPhase);

    // Transition to the mandatory inhalation phase since a mandatory
    // inhalation was being held off to perform the hold.
    triggerMgrPtrEnableTrigger (eBpTimeTrigger);
    phaseMgrPtrProcessNewPhase (eMandInhPhase);

}   // end ProcessExhHoldTrigger()

void AcMode::UpdateAlarmStatus (void)
{
    E_ReturnStatus status;
//    TriggerMgr* triggerMgrPtr;

    // ITime too long cannot be active in Assist Control.
    status = (E_ReturnStatus)BDAlarmTaskSetIsActive (eITimeTooLong, eNotActive);

    // If the status update was unsuccessful, reschedule the next update
    // for the short interval
    if(status != eSuccess)
    {
//        triggerMgrPtr = TriggerMgr::S_GetInstance ();
        triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger);
        triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);
    }

    // Invoke the base class operation to update common alarms.
//    NormBreathingMode::UpdateAlarmStatus ();

}   // end UpdateAlarmStatus()

void AcMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eBpTimeTrigger:
            ProcessBpTimeTrigger();
            break;

        case ePtInhTrigger:
            ProcessPtInhTrigger();
            break;

        case eExhHoldTrigger:
            ProcessExhHoldTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

void AcMode::ReportModeChangeInducedReset (void)
{
    // Inhalation Too Long cannot occur in A/C.  Reset it on entry.
    BDAlarmTaskSetIsActive (eITimeTooLong, eNotActive);

}   // end ReportModeChangeInducedReset()

class AcModeUnitMock : public BDTestFixture
{
public:
    AcModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~AcModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyAcMode = new AcMode();
    }
    virtual void TearDown(void)
    {
        delete MyAcMode;
    }
    AcMode *MyAcMode;
};

//test for AcMode* AcMode::S_GetInstance (void)
TEST_F(AcModeUnitMock, S_GetInstance)
{
    MyAcMode->S_GetInstance();

    EXPECT_NE((void*)0, MyAcMode->S_Instance);
}

//test for void AcMode::Enter (E_VentilationMode fromModeId)
TEST_F(AcModeUnitMock, Enter_0)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eBpTimeTrigger)).Times(1);

    MyAcMode->Enter(eStandbyMode);
}
TEST_F(AcModeUnitMock, Enter_1)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eBpTimeTrigger)).Times(1);

    MyAcMode->Enter(eOcclusionMode);
}
TEST_F(AcModeUnitMock, Enter_2)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eBpTimeTrigger)).Times(1);

    MyAcMode->Enter(eNoGasMode);
}
TEST_F(AcModeUnitMock, Enter_3)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod (eBpTimeTrigger)).Times(1).WillOnce(Return(0));

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eBpTimeTrigger)).Times(1);


    MyAcMode->Enter(eSpontaneousMode);
}
TEST_F(AcModeUnitMock, Enter_4)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod (eBpTimeTrigger)).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eBpTimeTrigger, 0)).Times(1);


    MyAcMode->Enter(eSpontaneousMode);
}
TEST_F(AcModeUnitMock, Enter_5)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod (eBpTimeTrigger)).Times(1).WillOnce(Return(0));

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eBpTimeTrigger)).Times(1);


    MyAcMode->Enter(eMixedMode);
}
TEST_F(AcModeUnitMock, Enter_6)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod (eBpTimeTrigger)).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eBpTimeTrigger, 0)).Times(1);


    MyAcMode->Enter(eMixedMode);
}
TEST_F(AcModeUnitMock, Enter_7)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod (eBpTimeTrigger)).Times(1).WillOnce(Return(0));

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eBpTimeTrigger)).Times(1);


    MyAcMode->Enter(eApneaMode);
}
TEST_F(AcModeUnitMock, Enter_8)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eExhHoldOpReqTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod (eBpTimeTrigger)).Times(1).WillOnce(Return(2));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eBpTimeTrigger, 0)).Times(1);


    MyAcMode->Enter(eApneaMode);
}

//test for AcMode::AcMode (void) :
TEST_F(AcModeUnitMock, AcMode)
{
    //no source code
}

//test for void AcMode::ProcessBpTimeTrigger (void)
TEST_F(AcModeUnitMock, ProcessBpTimeTrigger)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);

    MyAcMode->ProcessBpTimeTrigger();
}

//test for void AcMode::ProcessPtInhTrigger (void)
TEST_F(AcModeUnitMock, ProcessPtInhTrigger)
{
    EXPECT_CALL(*_TriggerMgr, TriggerMgrReEnableTrigger (eBpTimeTrigger)).Times(1);
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);

    MyAcMode->ProcessPtInhTrigger();
}

//test for void AcMode::ProcessExhHoldTrigger (void)
TEST_F(AcModeUnitMock, ProcessExhHoldTrigger)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eBpTimeTrigger)).Times(1);
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eMandInhPhase)).Times(1);

    MyAcMode->ProcessExhHoldTrigger();
}

//test for void AcMode::UpdateAlarmStatus (void)
TEST_F(AcModeUnitMock, UpdateAlarmStatus)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eITimeTooLong, eNotActive)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger,INIT_UPDATE_STAT_TIME) ).Times(1);

    MyAcMode->UpdateAlarmStatus();
}

//test for void AcMode::ProcessTriggerID (E_TriggerId id)
TEST_F(AcModeUnitMock, ProcessTriggerID)
{
    //all function is tested, not need to test
}

//test for void AcMode::ReportModeChangeInducedReset (void)
TEST_F(AcModeUnitMock, ReportModeChangeInducedReset)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eITimeTooLong, eNotActive)).Times(1).WillOnce(Return(1));

    MyAcMode->ReportModeChangeInducedReset();
}
#endif  //end define BD_MODE_UNIT_TEST
