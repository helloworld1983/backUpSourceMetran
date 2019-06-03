/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: APRV_SupportedInhPhase_InPHigh()
//
//    Purpose:
//     The APRV_SupportedInhPhase_InPHigh is the phase that occurs only in APRV mode
//
//                       ______              _______
//       _______________|      |____________|       |__________           ___________________________
//      |                                                      |         |                          |
//   ___|                                                      |_________|                          |_______
//             APRV_SupportedInhPhase_InPHigh
//
//                                APRV_SupportedInhPhase_InPHigh
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
#include "APRV_SupportedInhPhase_InPHigh.h"
#include "MgrBreathData.h"
#include "TriggerMgr.h"
#include "MgrBdSetting.h"
#include "AlarmTask.h"
#include "MgrController.h"

UNSIGNED APRV_SupportedInhPhase_InPHigh::S_InhStartTime = 0;
UNSIGNED APRV_SupportedInhPhase_InPHigh::S_TimeElapsed = 0;
UNSIGNED APRV_SupportedInhPhase_InPHigh::S_Time = 0;
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: APRV_SupportedInhPhase_InPHigh()
//
//    Processing:
//      This operation is the constructor for the APRV_SupportedInhPhase_InPHigh class.
//      This operation invokes the constructor of its base class
//      PressInhPhase.
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
APRV_SupportedInhPhase_InPHigh::APRV_SupportedInhPhase_InPHigh
(
    E_BdSettingId targetPressId,
    E_BdSettingId offsetPressId
    )
    : PressInhPhase(targetPressId, offsetPressId)
{

}   // end APRV_SupportedInhPhase_InPHigh()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing:
//      This operation is invoked at the start of any supported or spontaneous
//      inhalation phase.  This operation interfaces with the TriggerMgr object
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
void APRV_SupportedInhPhase_InPHigh::Start(void)
{

    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();// pointer to the TriggerMgr object
    E_BreathPhaseIndicator inhType;  // Spontaneous or Supported
    LONG desiredPress = 0;

    // Invoke base class method to perform processing common to all pressure
    // based inhalation phases.
    PressInhPhase::Start();
//    S_InhStartTime = SystemClock::Retrieve();
    S_TimeElapsed = 0;

    // Enable the Inh Pressure Controller!  Passing target and
    // initial pressure sets up the gain factors.

    if (MgrBdSetting::S_GetInstance()->GetCurrentValue(eSupportPress)>0)
    {
        desiredPress = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVHighInhPress)
                + MgrBdSetting::S_GetInstance()->GetCurrentValue(eSupportPress);

        triggerMgrPtr->EnableTrigger(ePtExhFlowSuppTrigger);
    }
    else
    {
        desiredPress = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVHighInhPress);

        triggerMgrPtr->EnableTrigger(ePtExhFlowSpontTrigger);

    }

    inhType = eSupported;

    MgrController::CMVServoController->SetInhalationTarget(desiredPress/10,RiseTime);

    // Enable the flow, pressure and time triggers than can terminate a
    // supported inhalation.

    // Update patient data
    (MgrBreathData::S_GetInstance())->APRV_InhPhaseStart(inhType);

    TriggerMgr::S_GetInstance()->EnableTrigger(eItLongAlarmTimeTrigger);

    TriggerMgr::S_GetInstance()->EnableTrigger(eApneaAlarmTimeTrigger);


    triggerMgrPtr->EnableTrigger(eHip2AlarmTrigger);

    AlarmTask::S_GetInstance()->SetIsActive(eApnea, eNotActive);


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
void APRV_SupportedInhPhase_InPHigh::ProcessBdCycle(void)
{

}   // end ProcessBdCycle()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing:
//      This operation is invoked at the end of a supported inhalation phase.
//      It first calls the PressInhPhase::End() operation to perform processing
//      common to all pressure based inhalation phases.  This operation then
//      interfaces with the TriggerMgr object to disable the flow, pressure
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
void APRV_SupportedInhPhase_InPHigh::End(void)
{
    TriggerMgr* triggerMgrPtr;  // pointer to the TriggerMgr

    // Get handle to the TriggerMgr object
    triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable triggers
    triggerMgrPtr->DisableTrigger(ePtExhFlowSuppTrigger);

    triggerMgrPtr->DisableTrigger(ePtExhFlowSpontTrigger);

    triggerMgrPtr->DisableTrigger(eItLongAlarmTimeTrigger);

    triggerMgrPtr->DisableTrigger(eHipAlarmTrigger);

    triggerMgrPtr->DisableTrigger(eHip2AlarmTrigger);

    S_TimeElapsed = APRV_SupportedInhPhase_InPHigh::S_CalcTimeSincePhaseStart();
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
UNSIGNED APRV_SupportedInhPhase_InPHigh::S_CalcTimeSincePhaseStart(void)
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
UNSIGNED APRV_SupportedInhPhase_InPHigh::GetTimeDuration (void)
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
void APRV_SupportedInhPhase_InPHigh::ResetTimeDuration (void)
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
UNSIGNED APRV_SupportedInhPhase_InPHigh::GetTimeElapsed (void)
{
    return S_TimeElapsed;
}
