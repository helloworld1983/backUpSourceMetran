/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: APRV_SupportedInhPhase_InPLow()
//
//    Purpose:
//     The APRV_SupportedInhPhase_InPHigh is the phase that occurs only in APRV mode
//
//
//       _____________________________________                          ______________________
//      |                                    |          _______        |                      |
//   ___|                                    |_________|       |_______|                      |
//
//
//                                        APRV_SupportedInhPhase_InPLow
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
#include "APRV_SupportedInhPhase_InPLow.h"
#include "MgrBreathData.h"
#include "TriggerMgr.h"
#include "MgrBdSetting.h"
#include "AlarmTask.h"
#include "MgrController.h"

UNSIGNED APRV_SupportedInhPhase_InPLow::S_InhStartTime = 0;
UNSIGNED APRV_SupportedInhPhase_InPLow::S_TimeElapsed = 0;
UNSIGNED APRV_SupportedInhPhase_InPLow::S_Time = 0;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: APRV_SupportedInhPhase_InPLow()
//
//    Processing:
//      This operation is the constructor for the APRV_SupportedInhPhase_InPLow class.
//      This operation invokes the constructor of its base class
//      PressInhPhase.
//
//    Input Parameters:
//     None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
APRV_SupportedInhPhase_InPLow::APRV_SupportedInhPhase_InPLow
(
    E_BdSettingId targetPressId,
    E_BdSettingId offsetPressId
    )
    : PressInhPhase(targetPressId, offsetPressId)
{

}   // end APRV_SupportedInhPhase_InPLow()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing:
//      This operation is invoked at the start of just spontaneous
//      inhalation phase.
//      .This operation interfaces with the TriggerMgr object
//      to enable the flow, pressure and time triggers that can end a
//      supported inhalation.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void APRV_SupportedInhPhase_InPLow::Start(void)
{
    E_BreathPhaseIndicator inhType;  // Spontaneous or Supported
    LONG desiredPress;

    // Invoke base class method to perform processing common to all pressure
    // based inhalation phases.
    PressInhPhase::Start();
//    S_InhStartTime = SystemClock::Retrieve();
    S_TimeElapsed = 0;


    inhType = eSpontaneous;
    TriggerMgr::S_GetInstance()->EnableTrigger(ePtExhFlowSuppTrigger);


    desiredPress = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVLowInhPress);
    MgrController::CMVServoController->SetInhalationTarget(desiredPress/10,RiseTime);

    TriggerMgr::S_GetInstance()->EnableTrigger(eItLongAlarmTimeTrigger);
    TriggerMgr::S_GetInstance()->EnableTrigger(eApneaAlarmTimeTrigger);
    AlarmTask::S_GetInstance()->SetIsActive(eApnea, eNotActive);

    (MgrBreathData::S_GetInstance())->APRV_InhPhaseStart(inhType);
}   // end Start()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBdCycle()
//
//    Processing:
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void APRV_SupportedInhPhase_InPLow::ProcessBdCycle(void)
{

}   // end ProcessBdCycle()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing:
//      This operation is invoked at the end of a spont inhalation phase.
//      This operation interfaces with the TriggerMgr object to disable the flow, pressure
//      and time triggers that were enabled at the start of the inhalation
//      phase.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//
//    Requirements:
//
/******************************************************************************/
void APRV_SupportedInhPhase_InPLow::End(void)
{
    TriggerMgr* triggerMgrPtr;  // pointer to the TriggerMgr

    // Get handle to the TriggerMgr object
    triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable triggers
    triggerMgrPtr->DisableTrigger(ePtExhFlowSuppTrigger);
    triggerMgrPtr->DisableTrigger(ePtExhFlowSpontTrigger);
    triggerMgrPtr->DisableTrigger(eItLongAlarmTimeTrigger);
    triggerMgrPtr->DisableTrigger(eHipAlarmTrigger);
    S_TimeElapsed = APRV_SupportedInhPhase_InPLow::S_CalcTimeSincePhaseStart();
    S_Time += S_TimeElapsed;
}   // end End()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_CalcTimeSincePhaseStart()
//
//    Processing:
//      Return the time elapsed since the start of the exhalation phase.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      Return the time elapsed since the start of the exhalation phase
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//
//    Requirements:
//
/******************************************************************************/
UNSIGNED APRV_SupportedInhPhase_InPLow::S_CalcTimeSincePhaseStart(void)
{
    // Return the time elapsed since the start of the exhalation phase.

    return(CalcElapsedTime(S_InhStartTime));


}   // end S_CalcTimeSincePhaseStart()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_CalcTimeSincePhaseStart()
//
//    Processing:
//      Return the time duration
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      Return the time duration
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//
//    Requirements:
//
/******************************************************************************/
UNSIGNED APRV_SupportedInhPhase_InPLow::GetTimeDuration (void)
{
    return S_Time;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ResetTimeDuration()
//
//    Processing:
//      Reset time duration
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//
//    Requirements:
//
/******************************************************************************/
void APRV_SupportedInhPhase_InPLow::ResetTimeDuration (void)
{
    S_Time = 0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetTimeElapsed()
//
//    Processing:
//      return time duration
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      return time duration
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//
//    Requirements:
//
/******************************************************************************/
UNSIGNED APRV_SupportedInhPhase_InPLow::GetTimeElapsed (void)
{
    return S_TimeElapsed;
}
