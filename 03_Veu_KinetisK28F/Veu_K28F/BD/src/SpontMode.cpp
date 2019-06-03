/******************************************************************************/
//$COMMON.CPP$
//   File Name:  SpontMode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      The operations in this class define the Spontaneous Mode of
//      ventilation.  The SpontMode is used to implement the CPAP mode in
//      the Pressure and the Volume Ventilation Types.  It is also used
//      to implement the NIPPV spontaneous mode.
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
//      SpontMode::ReportModeChangeInducedReset() interfaces with the 
//      Hip2AlarmStat to reset the alarm statuses when entering a Spontaneous 
//      mode.
//
//   Restrictions:
//      There is only 1 object instantiated from this class.
/******************************************************************************/
//#include <ModeMgr.h>
#include <PhaseMgr.h>
#include "SpontMode.h"
#include "TriggerMgr.h"
#include "Hip2AlarmTrigger.h"
#include "AlarmTask.h"
#include "ModeMgr.h"
#include "MgrDiscreteOutput.h"
#include "Btps.h"

#include "DebugTrace.h"

// Define statics
SpontMode* SpontMode::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the SpontMode object, it calls SpontMode::S_GetInstance(),
//      which returns a pointer to the SpontMode object.
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
//      SpontMode* - pointer to the object instantiated from this class
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
SpontMode* SpontMode::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new SpontMode ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SpontMode()
//
//    Processing: 
//      This operation is the constructor for the SpontMode.  It invokes
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
SpontMode::SpontMode (void) :
        NormBreathingMode ()
{

}   // end SpontMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter()
//
//    Processing: 
//      This operation is invoked when a Spontaneous mode is entered from
//      any other mode in the system.  
//
//      When this operation is called TriggerMgr::EnableTrigger() is called to
//      enable the Mode Change Trigger and the Apnea Alarm Time Trigger.  
//
//      If the Spontaneous Mode is being entered from the Start-Up Mode or
//      Non-Ventilation Mode, PhaseMgr::ProcessNewPhase() is called to 
//      put the ventilator into the exhalation phase.
//
//      If the Spontaneous is being entered from any other valid mode, 
//      PhaseMgr::GetCurrentPhaseId() is called to determine the current
//      breath phase.  If the breath phase is exhalation,
//      ReportModeChangeInducedReset() is called to reset any alarms
//      that are not applicable to the Spontaneous mode.
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
//      is performed.  If the Spontaneous Mode is entered from any mode except 
//      StartUp, the breath phase is expected to be exhalation; if not,
//      error handling is performed.
//
//    Requirements:  
//      None
//
/******************************************************************************/
void SpontMode::Enter (E_VentilationMode fromModeId)
{
    // Set state to on, and desired to off to make sure that this writen to the port, due to state and desired difference
    MgrDiscreteOutput::InpedanceLine->SetState(eOn);
    MgrDiscreteOutput::InpedanceLine->SetDesiredState(eOff);

    MgrDiscreteOutput::SafetyValve->SetDesiredState(eClose);
    // Initialize pointer to the PhaseMgr object


    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtr->DisableAllTriggers();
    // Enable triggers common to all breathing modes such as the Apnea Time
    // trigger and the Mode change trigger.

    triggerMgrPtr->EnableIMVTrigger();

    Btps::S_ComputeBtpsFactors();

//    MainHandlerDelegate::S_GetInstance()->postCustomEvent(eAmpAlarmAutoSetStopEvent);
}   // end Enter()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessPtInhTrigger()
//
//    Processing: 
//      This operation handles the processing of the Patient Inhalation Trigger
//      for the Spontaneous Mode.
//
//      This operation retrieves the current phase from the PhaseMgr object.
//      If the current phase is exhalation, PhaseMgr::ProcessNewPhase() is 
//      called to transition the ventilator to a spontaneous inhalation.  
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
void SpontMode::ProcessPtInhTrigger (void)
{

    // Initialize pointer to the PhaseMgr object
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Patient trigger should only fire during the exhalation phase.  
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Start a spontaneous inhalation.
    phaseMgrPtr->ProcessNewPhase (eSpontInhPhase);

}   // end ProcessPtInhTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ReportModeChangeInducedReset()
//
//    Processing: 
//      This operation is invoked from the Enter() operation.  It forces
//      alarms that cannot exist in the Spontaneous mode into auto-reset.
//  
//      Alarms that are auto-reset are alarms that pertain only to mandatory
//      inhalations like the Low Inhalation Pressure, the Low Exhaled
//      Mandatory Tidal Volume and the HIP-2 alarms.  There are no mandatory
//      breaths in the Spontaneous mode, so leaving these alarms active when 
//      transitioning from a Mixed Mode or Assist/Control would leave 
//      non-resettable nuisance alarms in the Spont mode.  These alarms are 
//      forced into the auto-reset state on entry into the Spont mode.  The
//      HIP-1 alarm is also reset here.  Although a HIP-1 can occur on a 
//      spontaneous breath, the slate is cleared on entry into the Spont Mode.
//
//      Most of the resets are performed by Patient Data since Patient Data
//      also detects those alarms.  Only the HIP alarms are reset here since BD 
//      is responsible for detecting the HIP alarms.
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
void SpontMode::ReportModeChangeInducedReset (void)
{

}   // end ReportModeChangeInducedReset()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of Spon MOde
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
void SpontMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
//        case ePtInhTrigger:
//            ProcessPtInhTrigger();
//            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Exit(void)
//
//    Processing: process doing something when exit from this mode
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
void SpontMode::Exit(void)
{
    MgrDiscreteOutput::SafetyValve->SetDesiredState(eOpen);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessModeChangeTrigger(void)
//
//    Processing: Process doing something when mode change
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
void SpontMode::ProcessModeChangeTrigger(void)
{
    (ModeMgr::S_GetInstance())->GoToOperatorSetMode();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBpTimeTrigger(void)
//
//    Processing: Process doing something when Bq Time trigger is on
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
void SpontMode::ProcessBpTimeTrigger(void)
{
    PhaseMgr* phaseMgrPtr;      // Pointer to the PhaseMgr object

    // Initialize pointer to the PhaseMgr object
    phaseMgrPtr = PhaseMgr::S_GetInstance();
    phaseMgrPtr->ProcessNewPhase(eMandInhPhase);
}
