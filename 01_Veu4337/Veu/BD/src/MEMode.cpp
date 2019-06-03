/******************************************************************************/
//$COMMON.CPP$
//   File Name:  StartupMode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Start-Up Mode.  The Start-Up
//      Mode is entered at power up and is exited once the BD Task enters
//      its main loop after it has received the settings from the GUI.
//
//   Interfaces:
//      The ProcessImmediateTrigger() operation in this class calls 
//      ModeMgr::GoToOperatorSetMode() to transition to the operator set mode
//      when the immediate trigger fires.  The Enter() and Exit() operations
//      interface with the TriggerMgr to enable/disable the Immediate Trigger
//      via the EnableTrigger() operation.  And, the Exit() operation interfaces
//      with SafetyValve to close the safety valve when leaving the Start-Up
//      Mode.  Exit() also interfaces with the flow controllers to enable 
//      them and the Exhalation Controller to close the exhalation valve.
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
#include "TriggerMgr.h"
#include "MEMode.h"
#include "ModeMgr.h"
#include "PhaseMgr.h"
#include "AzTest.h"

#include "DebugTrace.h"

// Initialize statics
MEMode* MEMode::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the MEMode object, it calls MEMode::S_GetInstance(),
//      which returns a pointer to the MEMode object.
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
//      MEMode* - pointer to the object instantiated from this class
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
MEMode* MEMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new MEMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MEMode()
//
//    Processing: 
//      This operation is the constructor for the Start-Up Mode.  It invokes the
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
MEMode::MEMode(void):Mode()
{

}   // end MEMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter()
//
//    Processing: 
//      This operation is called from the BD Task entry point during 
//      initialization.  This operation calls TriggerMgr::EnableTrigger()
//      to enable the Immediate Trigger, which will fire once the BD Task
//      enters its main loop.
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
//      It is assumed that the ventilator is in a safe state by the time
//      this mode is entered.  In the safe state, the exhalation and safety
//      valves are open, the delivery valves are closed and the controllers
//      are disabled.
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MEMode::Enter(E_VentilationMode fromModeId)
{
    //Initialize pointer to TriggerMgr
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    //Enable the mode change trigger.
    triggerMgrPtr->EnableTrigger(eModeChangeTrigger);

    //Process DiagnosticPhase
    PhaseMgr::S_GetInstance()->ProcessNewPhase(eDiagnosticPhase);

    //Suspend auto-zero scheduling.
//    PInhAzTestPtr->SuspendTest();
//    PExhAzTestPtr->SuspendTest();
}   // end Enter()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Exit()
//
//    Processing: 
//      This operation is called from ModeMgr::GoToOperatorSetMode() when
//      the Immediate Trigger Fires.  This operation invokes
//      SafetyValve::SetStateDirect() to close the safety valve.
//      This operation also calls the Enable() operations
//      of the Air and Oxygen Flow Controllers to enable the controllers.
//      Finally, TriggerMgr::DisableTrigger() is invoked to disable the
//      Immediate Trigger.
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
//      SafetyValve, AirFlowController and O2FlowController objects have
//      already been created.
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void MEMode::Exit(void)
{
    //Resume auto-zero scheduling.
//    PInhAzTestPtr->ResumeTest();
//    PExhAzTestPtr->ResumeTest();

}   // end Exit()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdateAlarmStatus()
//
//    Processing: 
//      This operation simply performs error handling.  It should never be
//      invoked but is defined here since it is declared as a pure virtual
//      operation in the base class.
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
//      This operation should not be invoked.
//
//    Requirements:
//
/******************************************************************************/
void MEMode::UpdateAlarmStatus(void)
{
    // This operation should never be called.
    ASSERTION(eFalse);

}  // end UpdateAlarmStatus()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of ME Mode
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
void MEMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eModeChangeTrigger:
            ProcessModeChangeTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessModeChangeTrigger()
//
//    Processing:
//      This operation simply performs error handling.  It should never be
//      invoked but is defined here since it is declared as a pure virtual
//      operation in the base class.
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
//      This operation should not be invoked.
//
//    Requirements:
//
/******************************************************************************/
void MEMode::ProcessModeChangeTrigger(void)
{
    (ModeMgr::S_GetInstance())->GoToOperatorSetMode();
}
