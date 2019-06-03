/******************************************************************************/
//$COMMON.CPP$
//   File Name:  AcMode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      The operations in this class define the Assist/Control mode
//      of ventilation used for the Pressure, Volume and Non-Invasive 
//      Ventilation Types.  A/C is a purely mandatory mode of ventilation. 
//
//   Interfaces:
//      Operations in this class interface with the TriggerMgr to enable,
//      re-enable and disable triggers via EnableTrigger(), ReEnableTrigger()
//      and DisableTrigger().
//
//      Operations in this class also call PhaseMgr::GetCurrentPhaseId() to
//      determine the current breath phase and PhaseMgr::ProcessNewPhase() to
//      change the breath phase.  
//
//      AcMode::Enter() calls InhPhase::S_CalcTimeSincePhaseStart() to determine
//      the time elasped since the start of the last inhalation.
//
//      AcMode::ReportModeChangeInducedReset() interfaces with the 
//      InhTooLongAlarmStat and Hip2AlarmStat to reset the alarm statuses when 
//      entering the A/C mode.
//
//   Restrictions:
//      There is only 1 object instantiated from this class.
/******************************************************************************/
#include <InhPhase.h>
#include <PhaseMgr.h>
#include "AcMode.h"
#include "TriggerMgr.h"
#include "DebugTrace.h"
#include "AlarmTask.h"

// Define statics
AcMode* AcMode::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the AcMode object, it calls AcMode::S_GetInstance(),
//      which returns a pointer to the AcMode object.
//
//      If the object has not already been created, this method instantiates
//      it and returns a pointer to the object.  If the object has already been
//      instantiated, a pointer to the object is returned.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      AcMode* - pointer to the object instantiated from this class
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter()
//
//    Processing: 
//      This operation is invoked when the Assist/Control mode is entered from
//      any other mode in the system.  
//
//      When this operation is called TriggerMgr::EnableTrigger() is called to
//      enable the Mode Change Trigger and the Apnea Alarm Time Trigger.  The
//      remainder of the processing depends upon the mode being exited in order
//      to enter A/C.
//
//      If A/C is being entered from the Start-Up Mode, 
//      PhaseMgr::ProcessNewPhase() is called to start a Mandatory Inhalation
//      and TriggerMgr::EnableTrigger() is called to start the Breath Period
//      Time Trigger for the operator set breath period.
//
//      If A/C is being entered from any other valid mode, 
//      InhPhase::S_CalcTimeSincePhaseStart() is invoked to determine the time
//      elapsed since the start of the last inhalation and 
//      TriggerMgr::GetTimePeriod() is called to retrieve the set breath 
//      period from the BpTimeTrigger Object.  If the time elapsed since the
//      start of the last inhalation is greater than the breath period,
//      PhaseMgr::ProcessNewPhase() is called to start a Mandatory Inhalation
//      and TriggerMgr::EnableTrigger() is called to start the Breath Period
//      Time Trigger for the operator set breath period.
//
//      Otherwise, EnableTrigger() is called to enable the Breath Period Time
//      Trigger for the time difference between the breath period and the time
//      since the start of the last inhalation.  
//
//      Finally, ReportModeChangeInducedReset() is called to reset any alarms
//      that are not applicable to the A/C mode.
//
//    Input Parameters: 
//      fromModeId - ID of the Mode that is being exited so that A/C can be
//          entered
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Miscellaneous:
//      If the fromModeId is not one of the expected modes, error handling
//      is performed.  If the A/C Mode is entered from any mode except Start-Up,
//      the breath phase is expected to be exhalation; if not, error handling 
//      is performed.
//
//    Requirements:  
//
/******************************************************************************/
void AcMode::Enter (E_VentilationMode fromModeId)
{
    UNSIGNED timeSinceInhStart;     // time elaspsed since start of last inh
    UNSIGNED breathPeriod;          // current breath period

    // Initialize pointers to the TriggerMgr object and the PhaseMgr object
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Enable triggers common to all breathing modes such as the Apnea Time
    // trigger and the Mode change trigger.
    UpdateCommonTriggers ();

    // Enable the exhalation hold request trigger.
//    triggerMgrPtr->EnableTrigger (eExhHoldOpReqTrigger);

    // Processing depends on the mode being exited.
    switch (fromModeId)
    {
        case eStandbyMode:
        case eOcclusionMode:
        case eNoGasMode:

            // When entering AC from Start-Up, go right into 
            // inhalation.   All inhalations in A/C are mandatory.
            phaseMgrPtr->ProcessNewPhase (eMandInhPhase);

            // Enable the Breath Period time trigger.  Since no parameters
            // are being passed, the trigger will enable the breath period
            // timer based on the current respiratory rate setting
            triggerMgrPtr->EnableTrigger (eBpTimeTrigger);

            break;

        case eSpontaneousMode:
        case eMixedMode:
        case eApneaMode:

            // The mode change should only occur in exhalation.
            ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

            // Determine the time elasped since the start of the last 
            // inhalation phase
            timeSinceInhStart = InhPhase::S_CalcTimeSincePhaseStart ();

            // The value associated with the Breath Period Time Trigger is
            // the breath period.  Retrieve it.
            breathPeriod = triggerMgrPtr->GetTimePeriod (eBpTimeTrigger);

            // When entering A/C from any other mode, figure out time 
            // elapsed since the start of the last inhalation. 
            if(timeSinceInhStart >= breathPeriod)
            {
                // If the time elapsed exceeds the breath period, transition 
                // to inhalation right away.  
                phaseMgrPtr->ProcessNewPhase (eMandInhPhase);

                // Enable the Breath Period time trigger.  Since no parameters
                // are being passed, the trigger will enable the breath period
                // timer based on the current respiratory rate setting
                triggerMgrPtr->EnableTrigger (eBpTimeTrigger);

            }
            else
            {
                // If the time elapsed since the start of the last 
                // inhalation is less than the breath period, enable the 
                // trigger for the difference between the breath period and
                // the time elapsed since the start of the last inhlation.
                triggerMgrPtr->EnableTrigger (eBpTimeTrigger, (breathPeriod - timeSinceInhStart));
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AcMode()
//
//    Processing: 
//      This operation is the constructor for the AcMode.  It invokes
//      the base class constructor.
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
//    Miscellaneous:
//      None
//
//    Requirements:  
//      None
//
/******************************************************************************/
AcMode::AcMode (void) :
        NormBreathingMode ()
{

}   // end AcMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBpTimeTrigger()
//
//    Processing: 
//      This operation handles the processing of the Breath Period Time Trigger
//      for the A/C Mode.
//
//      This operation retrieves the current phase from the PhaseMgr object.
//      If the current phase is exhalation, PhaseMgr::ProcessNewPhase() is 
//      called to transition the ventilator to a mandatory inhalation.  
//      TriggerMgr::SetTriggerDidFire() is also called to set the 
//      TriggerDidFire attribute of the Breath Period Time Trigger.
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
//      If the breath phase is not exhalation, error handling is performed.
//
//    Requirements:  
//
/******************************************************************************/
void AcMode::ProcessBpTimeTrigger (void)
{
    // Initialize pointers to the PhaseMgr and TriggerMgr objects
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Check if the phase is exhalation; the Breath Period Timer should only 
    // expire in exhalation.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Trigger a new inhalation.  All inhalations in A/C are mandatory.
    phaseMgrPtr->ProcessNewPhase (eMandInhPhase);

}   // end ProcessBpTimeTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessPtInhTrigger()
//
//    Processing: 
//      This operation handles the processing of the Patient Inhalation Trigger
//      for the A/C Mode.
//
//      This operation retrieves the current phase from the PhaseMgr object.
//      If the current phase is exhalation, PhaseMgr::ProcessNewPhase() is 
//      called to transition the ventilator to a mandatory inhalation.  
//      TriggerMgr::ReEnableTrigger() is also called to restart the  
//      Breath Period Timer.
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
//      If the breath phase is not exhalation, error handling is performed.
//
//    Requirements:  
//
/******************************************************************************/
void AcMode::ProcessPtInhTrigger (void)
{

    // Initialize pointers to the PhaseMgr and TriggerMgr objects
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();

    // Patient inhalation triggers should only fire during the exhalation phase.  
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Re-enable the breath period time trigger.  This call will
    // cause the breath period timer to be restarted.
    triggerMgrPtr->ReEnableTrigger (eBpTimeTrigger);

    // Start a mandatory inhalation.
    phaseMgrPtr->ProcessNewPhase (eMandInhPhase);

}   // end ProcessPtInhTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessExhHoldTrigger()
//
//    Processing: 
//      This operation handles the processing of the Exhalation Hold Trigger 
//      for the A/C Mode.  The Exhalation Hold Trigger fires when
//      either the BD receives a request to stop the hold
//      (operator released the button) or a timeout occurred.
//  
//      This operation retrieves the current phase ID from the PhaseMgr.  If
//      the breath phase is exhalation hold, this operation invokes the 
//      ProcessNewPhase() operation of the PhaseMgr to transition
//      the ventilator to the Mandatory Inhalation Phase.  This operation
//      also enables the Breath Period Time Trigger since it was disabled when
//      The Exhalation Hold phase started.
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
//      If the phase is not exhalation hold, error handling is performed
//      since this trigger should only fire in that phase.
//
//    Requirements:  
//
/******************************************************************************/
void AcMode::ProcessExhHoldTrigger (void)
{
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // This trigger should only occur in the exhalation hold phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhHoldPhase);

    // Transition to the mandatory inhalation phase since a mandatory 
    // inhalation was being held off to perform the hold.   
    (TriggerMgr::S_GetInstance ())->EnableTrigger (eBpTimeTrigger);
    phaseMgrPtr->ProcessNewPhase (eMandInhPhase);

}   // end ProcessExhHoldTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdateAlarmStatus()
//
//    Processing: 
//      This operation is invoked each time the update status time trigger
//      fires to update the mode related alarm statuses.  As part of 
//      the built in redundancy, the IT Too Long alarm is updated to indicate  
//      it is not active. Then NormBreathingMode::UpdateAlarmStatus() is called
//      to update alarm statuses common to all normal breathing modes.
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
void AcMode::UpdateAlarmStatus (void)
{
    E_ReturnStatus status;
    TriggerMgr* triggerMgrPtr;

    // ITime too long cannot be active in Assist Control.
    status = AlarmTask::S_GetInstance ()->SetIsActive (eITimeTooLong,
                                                       eNotActive);

    // If the status update was unsuccessful, reschedule the next update 
    // for the short interval
    if(status != eSuccess)
    {
        triggerMgrPtr = TriggerMgr::S_GetInstance ();
        triggerMgrPtr->DisableTrigger (eUpdateStatusTimeTrigger);
        triggerMgrPtr->EnableTrigger (eUpdateStatusTimeTrigger,
                                      INIT_UPDATE_STAT_TIME);
    }

    // Invoke the base class operation to update common alarms.
    NormBreathingMode::UpdateAlarmStatus ();

}   // end UpdateAlarmStatus()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of AC Mode
//
//    Input Parameters:
//      E_TriggerId id
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
void AcMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eBpTimeTrigger:
            ProcessBpTimeTrigger();
            break;

//        case ePtInhTrigger:
//            ProcessPtInhTrigger();
//            break;
//
//        case eExhHoldTrigger:
//            ProcessExhHoldTrigger();
//            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ReportModeChangeInducedReset()
//
//    Processing: 
//      This operation is invoked from the Enter() operation.  It forces
//      alarms that cannot exist in the A/C mode into an auto-reset state.
//  
//      Alarms that are auto-reset are alarms that pertain only to spontaneous
//      inhalations like the Inhalation Too Long Alarm.  There are no 
//      spontaneous breaths in A/C so leaving these alarms active when 
//      transitioning from a Mixed Mode or a Spontaneous Mode would leave 
//      non-resettable nuisance alarms in the A/C mode.  These alarms are forced 
//      into the auto-reset state on entry into the A/C mode.  Other alarms
//      such as the Low Exhaled Spontaneous Tidal Volume alarm are handled
//      by Patient Data since these alarms are also detected by Patient Data.
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
void AcMode::ReportModeChangeInducedReset (void)
{
    // Inhalation Too Long cannot occur in A/C.  Reset it on entry.
    AlarmTask::S_GetInstance ()->SetIsActive (eITimeTooLong, eNotActive);

}   // end ReportModeChangeInducedReset()
