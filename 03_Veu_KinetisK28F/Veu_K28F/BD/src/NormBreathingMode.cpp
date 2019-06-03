/******************************************************************************/
//$COMMON.CPP$
//   File Name:  NormBreathingMode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of methods to process triggers common
//      to all normal breathing modes.  The NormBreathingMode class is derived 
//      from the BreathingMode class and is used to define methods common to
//      all normal breathing mode.  A normal breathing mode is a non-alarm
//      mode where breathing occurs such as Assist Control or CPAP.
//
//   Interfaces:
//      Operations in this class interface with the PhaseMgr object to 
//      determine the current phase by calling GetCurrentPhaseId().  Operations
//      also update the phase by calling PhaseMgr::ProcessNewPhase().
//
//      ProcessApneaAlarmTimeTrigger() and ProcessModeChangeTrigger() 
//      inteface with the ModeMgr via the operations GoToAlarmMode() and 
//      GoToOperatorSetMode() to cause mode changes in response to triggers.
//      The ProcessApneaAlarmTimeTrigger() operation also interfaces with the
//      TriggerMgr to disable the Apnea Alarm Trigger after the trigger fires.
//
//      The UpdateAlarmStatus() operation interfaces with the 
//      OcclusionAlarmStat and ApneaAlarmStat objects via the SetIsActive()
//      operations.
//
//   Restrictions:
//      This class is an abstract base class.  It contains pure virtual 
//      methods and cannot be instantiated.
/******************************************************************************/
#include <ModeMgr.h>
#include <PhaseMgr.h>
#include "NormBreathingMode.h"
#include "TriggerMgr.h"
#include "Hip2AlarmTrigger.h"
#include "AlarmTask.h"
#include "MgrBreathData.h"

#include "DebugTrace.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: NormBreathingMode()
//
//    Processing: 
//      This operation is the constructor.  It invokes the BreathingMode 
//      constructor.
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
NormBreathingMode::NormBreathingMode (void) :
BreathingMode ()
{

}   // end NormBreathingMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Exit()
//
//    Processing: 
//      This operation is invoked when the a normal breathing mode is exited.
//      This operation invokes TriggerMgr::DisableTrigger() to disable several
//      triggers including the Breath Period Time Trigger and the Mode Change 
//      Trigger.
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
void NormBreathingMode::Exit (void)
{
    // Get a handle to the TriggerMgr object
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable the normal mode specific triggers
    triggerMgrPtr->DisableTrigger(eBpTimeTrigger);
    triggerMgrPtr->DisableTrigger(eModeChangeTrigger);
    triggerMgrPtr->DisableTrigger(eHip2AlarmTrigger);
    triggerMgrPtr->DisableTrigger(eTotalGasLossTrigger);
    triggerMgrPtr->DisableTrigger(eUpdateStatusTimeTrigger);
    triggerMgrPtr->DisableTrigger(eDisconnectionAlarmTrigger);

}   // end Exit()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdateCommonTriggers()
//
//    Processing: 
//      This operation is invoked from the Enter() operation of Normal 
//      Breathing Mode objects to enable and disable triggers that are common
//      to all normal breathing modes.
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
void NormBreathingMode::UpdateCommonTriggers (void)
{

    // Initialize pointer to the TriggerMgr
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Enable triggers

    triggerMgrPtr->EnableTrigger(eModeChangeTrigger);
    triggerMgrPtr->EnableTrigger(eHip2AlarmTrigger);
    triggerMgrPtr->EnableTrigger(eHipAlarmTrigger);
    triggerMgrPtr->EnableTrigger(eTotalGasLossTrigger);
    triggerMgrPtr->EnableTrigger(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);
    triggerMgrPtr->EnableTrigger(eApneaAlarmTimeTrigger);
    triggerMgrPtr->EnableTrigger(eHardwareErrorAlarmTriger);
    triggerMgrPtr->EnableTrigger(eOcclusionTrigger);

}   // end UpdateCommonTriggers()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdateAlarmStatus()
//
//    Processing: 
//      This operation is invoked each time the update status time trigger
//      fires to update the mode related alarm statuses.  As part of 
//      the built in redundancy, the Apnea, Occlusion and SVO alarm statuses
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
void NormBreathingMode::UpdateAlarmStatus (void)
{
    SHORT status = 0;
    TriggerMgr* triggerMgrPtr;

    // Notify AlarmStat objects that Occlusion, SVO and Apnea are not active
    status += (SHORT) AlarmTask::S_GetInstance()->SetIsActive(eApnea, eNotActive);
    status += (SHORT) AlarmTask::S_GetInstance()->SetIsActive(eOcclusion, eNotActive);
    status += (SHORT) AlarmTask::S_GetInstance()->SetIsActive(eGasSuppliesLost, eNotActive);

    // If any of the status updates was unsuccessful, reschedule the next update 
    // for the short interval
    if(status != (SHORT) eSuccess)
    {
        triggerMgrPtr = TriggerMgr::S_GetInstance ();
        triggerMgrPtr->DisableTrigger (eUpdateStatusTimeTrigger);
        triggerMgrPtr->EnableTrigger (eUpdateStatusTimeTrigger,
                                      INIT_UPDATE_STAT_TIME);
    }

}   // UpdateAlarmStatus()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessModeChangeTrigger()
//
//    Processing: 
//      This operation handles the processing of the Mode Change Trigger
//      for all Normal Breathing Modes.
//  
//      This operation retrieves the current phase ID from the PhaseMgr.  If
//      the breath phase is the exhalation phase, this operation invokes
//      ModeMgr::GoToOperatorSetMode() to transition to the new mode.
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
//      If the phase is not exhalation, error handling is performed
//      since this trigger should only fire in that phase.
//
//    Requirements:  
//
/******************************************************************************/
void NormBreathingMode::ProcessModeChangeTrigger (void)
{
    // Transition to the new mode
    (ModeMgr::S_GetInstance())->GoToOperatorSetMode();

}   // end ProcessModeChangeTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessItLongAlarmTimeTrigger()
//
//    Processing: 
//      This operation handles the processing of the Inhalation Time Too Long
//      Alarm Time Trigger for all Normal Breathing Modes.
//
//      This operation calls the private operation EndSpontInhalation() to 
//      terminate the inhalation.
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
void NormBreathingMode::ProcessItLongAlarmTimeTrigger (void)
{
    if(eNotActive == AlarmTask::S_GetInstance()->GetIsActive(eITimeTooLong))
    {
        AlarmTask::S_GetInstance()->SetIsActive(eITimeTooLong, eActive);
    }
    EndSpontInhalation();

}   // end ProcessItLongAlarmTimeTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessPtExhTrigger()
//
//    Processing: 
//      This operation handles the processing of the Patient Exhalation Flow
//      Trigger and the Patient Exhalation Pressure Trigger for all Normal 
//      Breathing Modees.
//
//      This operation calls the private operation EndSpontInhalation() to 
//      terminate the inhalation.
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
void NormBreathingMode::ProcessPtExhTrigger (void)
{
    AlarmTask::S_GetInstance()->SetIsActive(eITimeTooLong, eNotActive);
    EndSpontInhalation ();

}   // ProcessPtExhTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EndSpontInhalation()
//
//    Processing: 
//      This operation retrieves the current phase from the PhaseMgr object.
//      If the current phase is a spontaneous inhalation phase, 
//      PhaseMgr::ProcessNewPhase() is called to transition the ventilator
//      to the exhalation phase.
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
//      If the breath phase is not a spontaneous inhalation phase, error 
//      handling is performed.
//
//    Requirements:  
//
/******************************************************************************/
void NormBreathingMode::EndSpontInhalation (void)
{
    // Initialize pointer to the PhaseMgr
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Verify that the active phase is a spontaneous inhalation phase
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eSpontInhPhase);

    // Transition to exhalation.
    phaseMgrPtr->ProcessNewPhase (eExhPhase);

}   // end EndSpontInhalation()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of Normal Breathing Mode
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
void NormBreathingMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eApneaAlarmTimeTrigger:
            ProcessApneaAlarmTimeTrigger();
            break;

        case eModeChangeTrigger:
            ProcessModeChangeTrigger();
            break;

        case eItLongAlarmTimeTrigger:
            ProcessItLongAlarmTimeTrigger();
            break;

//        case ePtExhTrigger:
//            ProcessPtExhTrigger();
//            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessHip1AlarmTrigger()
//
//    Processing:
//      This operation handles the processing for the High Inhalation Pressure-1
//      and High Inhalation Pressure-2 Alarm Triggers for all Breathing Modes.
//
//      A High Inhalation Pressure Trigger fires when the measured pressure
//      exceeds the operator set HIP limit during the inhalation or plateau
//      phase of a breath.
//
//      This operation retrieves the current phase ID from the PhaseMgr.  If
//      the breath phase is inhalation or plateau, this operation
//      invokes the ProcessNewPhase() operation of the PhaseMgr to transition
//      the ventilator to the Exhalation Phase.
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
//      If the phase is not inhalation or plateau, error handling is
//      performed since this trigger should only fire in those phases.
//
//    Requirements:
//
/******************************************************************************/
void NormBreathingMode::ProcessHipAlarmTrigger(void)
{
    // Initialize pointer to the PhaseMgr
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // Verify that the active phase is a inhalation phase
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() != eExhPhase);

    // Transition to exhalation.
    phaseMgrPtr->ProcessNewPhase(eExhPhase);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessHip2AlarmTrigger()
//
//    Processing:
//      This operation handles the processing for the High Inhalation Pressure-1
//      and High Inhalation Pressure-2 Alarm Triggers for all Breathing Modes.
//
//      A High Inhalation Pressure Trigger fires when the measured pressure
//      exceeds the operator set HIP limit during the inhalation or plateau
//      phase of a breath.
//
//      This operation retrieves the current phase ID from the PhaseMgr.  If
//      the breath phase is inhalation or plateau, this operation
//      invokes the ProcessNewPhase() operation of the PhaseMgr to transition
//      the ventilator to the Exhalation Phase.
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
//      If the phase is not inhalation or plateau, error handling is
//      performed since this trigger should only fire in those phases.
//
//    Requirements:
//
/******************************************************************************/
void NormBreathingMode::ProcessExtremePressAlarmTrigger(void)
{
    (ModeMgr::S_GetInstance())->GoToAlarmMode(eHighPressMode);
}   // end ProcessHip2AlarmTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessOcclusionAlarmTrigger()
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
//
//    Requirements:
//
/******************************************************************************/
void NormBreathingMode::ProcessOcclusionAlarmTrigger(void)
{
    (ModeMgr::S_GetInstance())->GoToAlarmMode(eHighPressMode);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessApneaAlarmTimeTrigger()
//
//    Processing:
//      This operation handles the processing of the Apnea Alarm Time Trigger
//      for all Normal Breathing Modes.
//
//      This operation retrieves the current phase ID from the PhaseMgr.  If
//      the breath phase is the exhalation phase, this operation invokes
//      TriggerMgr::DisableTrigger to disable the ApneaAlarmTimeTrigger and
//      ModeMgr::GoToAlarmMode() to transition to the Apnea Ventilation Mode.
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
//      If the phase is not exhalation, error handling is performed
//      since this trigger should only fire in that phase.
//
//    Requirements:
//
/******************************************************************************/

void NormBreathingMode::ProcessApneaAlarmTimeTrigger(void)
{
    (ModeMgr::S_GetInstance())->GoToAlarmMode(eApneaBackupMode);
}   // end ProcessApneaAlarmTimeTrigger()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessAlarmResetTrigger()
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
//
//    Requirements:
//
/******************************************************************************/
void NormBreathingMode::ProcessAlarmResetTrigger()
{
    ASSERTION(eFalse);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessManualInhOpReqTrigger()
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
//
//    Requirements:
//
/******************************************************************************/
void NormBreathingMode::ProcessManualInhOpReqTrigger()
{
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // Operator initiated inhalation triggers should only fire
    // during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    TriggerMgr::S_GetInstance()->ReEnableTrigger(eBpTimeTrigger);

    // Start a mandatory inhalation.
    phaseMgrPtr->ProcessNewPhase(eMandInhPhase);

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessProxyDisconnectTrigger()
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void NormBreathingMode::ProcessProxyDisconnectTrigger()
{
    TriggerMgr::S_GetInstance()->EnableTrigger(eProxyConnectTrigger);
    TriggerMgr::S_GetInstance()->DisableTrigger(eProxyDisconnectTrigger);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessProxyConnectTrigger()
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void NormBreathingMode::ProcessProxyConnectTrigger()
{
    TriggerMgr::S_GetInstance()->EnableTrigger(eProxyDisconnectTrigger);
    TriggerMgr::S_GetInstance()->DisableTrigger(eProxyConnectTrigger);
    MgrBreathData::S_GetInstance()->ClearCompensateBuffer();
}
