/******************************************************************************/
//$COMMON.CPP$
//   File Name:  OcclusionMode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Occlusion Mode.  The Occlusion
//      Mode is entered in response to the detection of an occluded circuit.  
//      It is exited when the reset for the occlusion condition occurs.
//
//   Interfaces:
//      The ProcessOcclAutoResetTrigger() operation in this class calls 
//      ModeMgr::ReturnToLastMode() to transition to the operator set mode or 
//      the mode that was active when the Occlusion mode was entered.
//
//      The Enter() and Exit() operations interface with the TriggerMgr to 
//      enable/disable triggers.  
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
#include <ModeMgr.h>
#include <OcclMode.h>
#include <PhaseMgr.h>
#include "TriggerMgr.h"
#include "DebugTrace.h"

#include "AlarmTask.h"

// Initialize statics
OcclMode* OcclMode::S_Instance = NULL;
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the OcclMode object, it invokes OcclMode::S_GetInstance(),
//      which returns a pointer to the OcclMode object.
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
//      OcclMode* - pointer to the object instantiated from this class
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
OcclMode* OcclMode::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new OcclMode ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: OcclMode()
//
//    Processing: 
//      This operation is the constructor for the Occlusion Mode.  It invokes 
//      the base class NonVentMode constructor.
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
OcclMode::OcclMode (void) :
        NonVenMode (eFalse)
{

}   // end OcclMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter()
//
//    Processing: 
//      This operation is called whenever an occlusion trigger fires.
//      This operation calls the base class NonVentMode::Enter() operation,
//      enables the occlusion reset trigger and updates the occlusion alarm
//      status.
//
//    Input Parameters:
//      fromModeId - ID of the Mode that is being exited 
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
void OcclMode::Enter (E_VentilationMode fromModeId)
{
    // Invoke the base class Enter() operation.  This must be done before
    // enabling any triggers.
    NonVenMode::Enter (fromModeId);

    // Initialize pointer to the TriggerMgr
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();

    // Enable the reset occlusion trigger.
//    triggerMgrPtr->EnableTrigger (eOcclAutoResetTrigger);

    // Update the Occlusion alarm status.
    AlarmTask::S_GetInstance ()->SetIsActive (eOcclusion, eActive);

}   // end Enter()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Exit()
//
//    Processing: 
//      This operation is invoked when the occlusion condition is reset and
//      the ventilator is to exit the occlusion mode.  This operation disables
//      the occlusion reset trigger, invokes the base class NonVentMode::Exit() 
//      operation, and updates the occlusion alarm status.
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
void OcclMode::Exit (void)
{
    // Initialize pointer to the TriggerMgr
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();

    //Disable eOcclAutoResetTrigger
//    triggerMgrPtr->DisableTrigger (eOcclAutoResetTrigger);

    // Update the Occlusion alarm status
    AlarmTask::S_GetInstance ()->SetIsActive (eOcclusion, eNotActive);

    // The base class method takes care of closing the safety valve.
    NonVenMode::Exit ();

}   // end Exit()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessOcllAutoResetTrigger()
//
//    Processing: 
//      This operation handles the processing of the Occlusion Auto-Reset 
//      Trigger for the Occlusion Mode.  This operation checks if the current
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
void OcclMode::ProcessOcclAutoResetTrigger (void)
{
    // The Occlusion Reset Trigger should only fire during non-ventilation
    // phase since the ventilator was forced into this phase when occlusion was
    // detected.  
    ASSERTION((PhaseMgr::S_GetInstance())->GetCurrentPhaseId() == eAlarmPhase);

    (ModeMgr::S_GetInstance ())->ReturnToLastMode ();

}   // end ProcessOcclAutoResetTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdateAlarmStatus()
//
//    Processing: 
//      This operation is invoked each time the UpdateStatus TimeTrigger fires.
//      It updates the occlusion alarm status to indicate that occlusion is
//      still active.
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
void OcclMode::UpdateAlarmStatus (void)
{
    E_ReturnStatus status = eEmpty;
    TriggerMgr* triggerMgrPtr;

    AlarmTask::S_GetInstance ()->SetIsActive (eOcclusion, eActive);

    // If the status update was unsuccessful, reschedule the next update 
    // for the short interval
    if(status != eSuccess)
    {
        triggerMgrPtr = TriggerMgr::S_GetInstance ();
        triggerMgrPtr->DisableTrigger (eUpdateStatusTimeTrigger);
        triggerMgrPtr->EnableTrigger (eUpdateStatusTimeTrigger,
                                      INIT_UPDATE_STAT_TIME);
    }

}   // end UpdateAlarmStatus()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of OcclMode Mode
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
void OcclMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
//        case eOcclAutoResetTrigger:
//            ProcessOcclAutoResetTrigger();
//            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}
