/******************************************************************************/
//$COMMON.CPP$
//   File Name:  AvMode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      The operations in this class define the Apnea Ventilation mode.
//      Apnea Ventilation is a mode entered in response to detection of the
//      apnea alarm condition.  AV is a purely mandatory mode of ventilation.
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
//      The operations that process triggers for inhalations (e.g.
//      ProcessPtInhTrigger()) interface with the ApneaAutoResetTrigger object
//      via the operation UpdateState() to notify that operation each time
//      a PIM or VIM is triggered.
//
//      The UpdateAlarmStatus() operation interfaces with the ApneaAlarmStat
//      and OcclAlarmStat objects to update the alarm statuses.
//
//   Restrictions:
//      There is only 1 object instantiated from this class.
/******************************************************************************/
#include <ModeMgr.h>
#include <PhaseMgr.h>
#include "AvMode.h"
#include "TriggerMgr.h"
#include "ApneaAutoResetTrigger.h"
#include "AlarmResetOpReqTrigger.h"
#include "AlarmTask.h"
#include "DebugTrace.h"

// Define statics
AvMode* AvMode::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the AvMode object, it calls AvMode::S_GetInstance(),
//      which returns a pointer to the AvMode object.
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
//      AvMode* - pointer to the object instantiated from this class
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter()
//
//    Processing: 
//      This operation is invoked when Apnea Ventilation is entered from
//      any other mode in the system.  
//
//      When this operation is called TriggerMgr::EnableTrigger() is called to
//      enable the the Apnea Alarm Time Trigger, the Apnea Auto-Reset Trigger,
//      the Alarm Reset Trigger and the AV Breath Period Time Trigger.  
//
//      If the AV mode is being entered from one of the normal breathing modes
//      or from the Non-Ventilation mode, PhaseMgr::GetCurrentPhaseId() to 
//      determine the current breath phase.  If the ventilator is in the 
//      exhalation phase, PhaseMgr::ProcessNewPhase() is called to start a 
//      Mandatory Inhalation.
//
//      If the AV mode is entered from any other mode, error handling is 
//      performed since apnea cannot be entered from the other modes.
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
//      If the fromModeId is not a legal mode or if fromModeId is a mode that
//      Apnea cannot be entered from, error handling is performed.  If the 
//      AV Mode is entered from a normal breathing mode or from the 
//      Non-Vent Mode, the breath phase is expected to be exhalation; 
//      if not, error handling is performed.
//
//    Requirements:  
//
/******************************************************************************/
void AvMode::Enter (E_VentilationMode fromModeId)
{
    E_PhaseId phaseId;      // current phase ID

    // Initialize pointers to the TriggerMgr object and the PhaseMgr object
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Enable the reset triggers.
    triggerMgrPtr->EnableTrigger (eAlarmResetOpReqTrigger);
    triggerMgrPtr->EnableTrigger (eApneaAutoResetTrigger);

    // Enable the total gas loss trigger
    triggerMgrPtr->EnableTrigger (eTotalGasLossTrigger);

    // Enable the alarm status update trigger    
    triggerMgrPtr->EnableTrigger (eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);

    // Processing depends on the mode being exited.
    switch (fromModeId)
    {
        case eSpontaneousMode:
        case eMixedMode:
        case eNoGasMode:
        case eOcclusionMode:

            phaseId = phaseMgrPtr->GetCurrentPhaseId ();

            // The mode change should only occur in exhalation.
            ASSERTION((eExhPhase == phaseId) || (eExhHoldPhase == phaseId) ||
                       (eAlarmPhase == phaseId));

            // Deliver a mandatory inhalation
            phaseMgrPtr->ProcessNewPhase (eMandInhPhase);

            // Enable the breath period trigger.
            triggerMgrPtr->EnableTrigger (eAvBpTimeTrigger);
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
    AlarmTask::S_GetInstance ()->SetIsActive (eApnea, eActive);

}   // end Enter()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Exit()
//
//    Processing: 
//      This operation is invoked when the Apnea Ventilation mode is exited.
//      This operation invokes TriggerMgr::DisableTrigger() to disable the
//      the Apnea Alarm Time Trigger, the Apnea Auto-Reset Trigger,
//      the Alarm Reset Trigger and the AV Breath Period Time Trigger.  
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
void AvMode::Exit (void)
{

    // Get a handle to the TriggerMgr object
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();

    // Disable AV Mode specific triggers.
    triggerMgrPtr->DisableTrigger (eAvBpTimeTrigger);
    triggerMgrPtr->DisableTrigger (eAlarmResetOpReqTrigger);
    triggerMgrPtr->DisableTrigger (eApneaAutoResetTrigger);
    triggerMgrPtr->DisableTrigger (eTotalGasLossTrigger);
    triggerMgrPtr->DisableTrigger (eUpdateStatusTimeTrigger);

}   // end Exit()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AvMode()
//
//    Processing: 
//      This operation is the constructor for the AvMode.  It invokes
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
AvMode::AvMode (void) :
        AlarmBreathingMode (eTrue)
{

}   // end AvMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessPtInhTrigger()
//
//    Processing: 
//      This operation handles the processing of the Patient Inhalation Trigger
//      for the Apnea Ventilation Mode.
//
//      This operation retrieves the current phase from the PhaseMgr object.
//      If the current phase is exhalation, PhaseMgr::ProcessNewPhase() is 
//      called to transition the ventilator to a mandatory inhalation.  
//      TriggerMgr::ReEnableTrigger() is also called to restart the  
//      Apnea Breath Period Timer and ApneaAutoResetTrigger::UpdateState()
//      is called to notify that object of the start of a PIM.
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
void AvMode::ProcessPtInhTrigger (void)
{
    // Initialize pointers to the PhaseMgr and TriggerMgr objects
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();

    // Patient inhalation triggers should only fire during the exhalation phase.  
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Re-enable the breath period time trigger.  This call will
    // cause the breath period timer to be restarted.
    triggerMgrPtr->ReEnableTrigger (eAvBpTimeTrigger);

    // Start a mandatory inhalation.
    phaseMgrPtr->ProcessNewPhase (eMandInhPhase);

    // Notify the ApneaAutoResetTrigger of the PIM.  It does not matter
    // if the trigger ID is flow or pressure as long as it is patient
    // initiated.
    (ApneaAutoResetTrigger::S_GetInstance ())->UpdateState (ePtInhPressTrigger);

}   // end ProcessPtInhTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessAvBpTimeTrigger()
//
//    Processing: 
//      This operation handles the processing of the Apnea Breath Period Time 
//      Trigger for the Apnea Ventilation Mode.
//
//      This operation retrieves the current phase from the PhaseMgr object.
//      If the current phase is exhalation, PhaseMgr::ProcessNewPhase() is 
//      called to transition the ventilator to a mandatory inhalation.  
//      TriggerMgr::SetTriggerDidFire() is also called to set the 
//      TriggerDidFire attribute of the Apnea Breath Period Time Trigger
//      and ApneaAutoResetTrigger::UpdateState() is called to notify that 
//      object of the start of a VIM.
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
void AvMode::ProcessAvBpTimeTrigger (void)
{
    // Initialize pointers to the PhaseMgr.
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Check if the phase is exhalation; the Apnea Breath Period Timer should 
    // only expire in exhalation.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Trigger a new inhalation.  All inhalations in AV are mandatory.
    phaseMgrPtr->ProcessNewPhase (eMandInhPhase);

    // Notify the ApneaAutoResetTrigger of the VIM
    (ApneaAutoResetTrigger::S_GetInstance ())->UpdateState (eAvBpTimeTrigger);

}   // end ProcessAvBpTimeTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessApneaAutoResetTrigger()
//
//    Processing: 
//      This operation handles the processing of the Apnea Auto-Reset Trigger
//      for the Apnea Ventilation Mode.  This operation checks if the current
//      breath phase is exhalation.  If so, ModeMgr::GoToLastMode() is called
//      to return the ventilator to the previous mode or the operator set mode.
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
void AvMode::ProcessApneaAutoResetTrigger (void)
{
    // The Apnea Reset Trigger should only fire during exhalation.  
    ASSERTION((PhaseMgr::S_GetInstance())->GetCurrentPhaseId() == eExhPhase);

    (ModeMgr::S_GetInstance ())->ReturnToLastMode ();

}   // end ProcessApneaAutoResetTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdateAlarmStatus()
//
//    Processing: 
//      This operation is invoked each time the update status time trigger
//      fires to update the mode related alarm statuses.  As part of 
//      the built in redundancy, the Apnea Alarm Status is updated to indicate 
//      that it is active. Occlusion, Total Gas Loss and IT Too Long alarms
//      are updated to indicate that they are not active.
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
void AvMode::UpdateAlarmStatus (void)
{
    SHORT status = 0;
    ApneaAutoResetTrigger* apneaArTriggerPtr;
    AlarmResetOpReqTrigger* alarmResetTriggerPtr;
    TriggerMgr* triggerMgrPtr;

    apneaArTriggerPtr = ApneaAutoResetTrigger::S_GetInstance ();
    alarmResetTriggerPtr = AlarmResetOpReqTrigger::S_GetInstance ();

    // As long as a reset is not pending, notify Alarms that Apnea is active.
    if((apneaArTriggerPtr->GetState () != eArmed)
            && (alarmResetTriggerPtr->GetIsArmed () == eFalse))
    {
        // Notify AlarmStat objects that AV is active
        status += (SHORT) AlarmTask::S_GetInstance ()->SetIsActive (eApnea, eActive);
    }

    // If the ventilator is in Apnea Ventilation, the occlusion alarm is
    // not active, the Total Gas Loss alarm is not active and the IT Too Long
    // alarm is not active.
    status += (SHORT) AlarmTask::S_GetInstance ()->SetIsActive (eOcclusion, eNotActive);
    status += (SHORT) AlarmTask::S_GetInstance ()->SetIsActive (eGasSuppliesLost, eNotActive);
    status += (SHORT) AlarmTask::S_GetInstance ()->SetIsActive (eITimeTooLong, eNotActive);

    // If any of the status updates was unsuccessful, reschedule the next update 
    // for the short interval
    if(status != (SHORT) eSuccess)
    {
        triggerMgrPtr = TriggerMgr::S_GetInstance ();
        triggerMgrPtr->DisableTrigger (eUpdateStatusTimeTrigger);
        triggerMgrPtr->EnableTrigger (eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);
    }

}   // end UpdateAlarmStatus()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of Av Mode
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
void AvMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
//        case ePtInhTrigger:
//            ProcessPtInhTrigger();
//            break;

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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ReportModeChangeInducedReset()
//
//    Processing: 
//      This operation is invoked from the Enter() operation.  It forces
//      alarms that cannot exist in the mandatory mode into an auto-reset state.
//  
//      Alarms that are auto-reset are alarms that pertain only to spontaneous
//      inhalations like the Inhalation Too Long Alarm.  There are no 
//      spontaneous breaths in AV so leaving these alarms active when 
//      transitioning from a Mixed Mode or a Spontaneous Mode would leave 
//      non-resettable nuisance alarms in the AV mode.  These alarms are forced 
//      into the auto-reset state on entry into the AV mode.  Other alarms
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
void AvMode::ReportModeChangeInducedReset (void)
{
    // Inhalation Too Long cannot occur in AV.  Reset it on entry.
    AlarmTask::S_GetInstance ()->SetIsActive (eITimeTooLong, eNotActive);

}   // end ReportModeChangeInducedReset()
