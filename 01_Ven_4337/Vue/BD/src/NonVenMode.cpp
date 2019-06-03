/******************************************************************************/
//$COMMON.CPP$
//   File Name:  NonVentMode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Non-Vent Mode.  The Non-Vent
//      Mode is a base class from which non-ventilation modes are derived. 
//      The non-vent modes include the NoGasMode entered in response to a total 
//      gas loss trigger and the OcclMode, entered in response to occlusion
//      detection.  The NonVentMode is exited when the alarm condition resets.
//
//   Interfaces:
//      The Enter() and Exit() operations interface with the TriggerMgr to 
//      enable/disable triggers.  Those operations also interface with
//      SafetyValve to open and close the safety valve.
//
//   Restrictions:
//      None
/******************************************************************************/
#include <NonVenMode.h>
#include <PhaseMgr.h>
#include "TriggerMgr.h"
#include "MgrDiscreteOutput.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: NonVentMode()
//
//    Processing: 
//      This operation is the constructor for the NonVent Mode.  It invokes the
//      base class Mode constructor.
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
NonVenMode::NonVenMode (E_Bool doesManualReset) :
        Mode (doesManualReset)
{

}   // end NonVentMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter()
//
//    Processing: 
//      This operation is called from the the derived AlarmMode mode
//      Enter() operations to perform processing common to all AlarmMode
//      modes.  
//
//    Input Parameters:
//      Not Used
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
void NonVenMode::Enter (E_VentilationMode)
{
    // Open the safety valve.
    MgrDiscreteOutput::SafetyValve->SetStateDirect(eOpen);

    // Initialize pointer to the TriggerMgr
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Put the ventilator into the alarm phase.
    phaseMgrPtr->ProcessNewPhase (eAlarmPhase);

    // Disable all triggers.  This call must be made after forcing the vent
    // into the non-ventilation phase above because depending on the phase
    // transitioned from, there may be some triggers left enabled.
    triggerMgrPtr->DisableAllTriggers ();

    // Enable the Update Status trigger which causes the update of the
    // status of the alarms associated with the active Alarm mode.
    triggerMgrPtr->EnableTrigger (eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);

    // Suspend auto-zero scheduling.    
//    InhAzSchedulerPtr->Suspend();
//    ExhAzSchedulerPtr->Suspend();

}   // end Enter()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Exit()
//
//    Processing: 
//      This operation is called when a trigger fires that causes a reset out
//      of the Non-Ventilation mode.
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
void NonVenMode::Exit (void)
{
    // Close the safety valve.
    MgrDiscreteOutput::SafetyValve->SetStateDirect(eClose);

    // Initialize pointer to the TriggerMgr
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();

    // Disable triggers.
    triggerMgrPtr->DisableTrigger (eUpdateStatusTimeTrigger);

    // Resume auto-zero scheduling.    
//    InhAzSchedulerPtr->Resume();
//    ExhAzSchedulerPtr->Resume();

}   // end Exit()

