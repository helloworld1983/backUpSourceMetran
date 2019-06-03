/******************************************************************************/
//$COMMON.CPP$
//   File Name:  AlarmBreathingMode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of methods to process triggers common
//      to all alarm breathing modes.  The AlarmBreathingMode class is derived 
//      from the BreathingMode class and is used to define methods common to
//      all alarm breathing modes.  An alarm breathing mode is an alarm
//      mode where breathing occurs such as Apnea Ventilation.
//
//   Interfaces:
//      Operations in this class interface with the PhaseMgr object to 
//      determine the current phase by calling GetCurrentPhaseId().  Operations
//      also update the phase by calling PhaseMgr::ProcessNewPhase().
//
//      The ProcessAlarmResetTrigger() also interfaces with the ModeMgr object
//      to transition the ventilator from the alarm mode to the operator
//      set mode via the operation GoToOperatorSetMode().
//
//   Restrictions:
//      This class is an abstract base class.  It contains pure virtual 
//      methods and cannot be instantiated.
/******************************************************************************/
#include <ModeMgr.h>
#include <PhaseMgr.h>
#include "AlarmBreathingMode.h"

#include "DebugTrace.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AlarmBreathingMode()
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
AlarmBreathingMode::AlarmBreathingMode (E_Bool doesManualReset) :
            BreathingMode (doesManualReset)
{

}   // end AlarmBreathingMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of Alarm Breathing Mode
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
void AlarmBreathingMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
//        case eAlarmResetTrigger:
//            ProcessAlarmResetTrigger();
//            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessAlarmResetTrigger()
//
//    Processing: 
//      This operation handles the processing of the Alarm Reset Trigger
//      for all Alarm Breathing Modes.
//  
//      This operation retrieves the current phase ID from the PhaseMgr.  If
//      the breath phase is the exhalation phase, this operation invokes
//      ModeMgr::GoToOperatorSetMode() to transition to the operator set mode.
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
void AlarmBreathingMode::ProcessAlarmResetTrigger (void)
{
    // Pointer to the Mode Manager
    ModeMgr* modeMgrPtr;

    // The Alarm Reset Trigger should only fire during exhalation.  The 
    // operator can press the Alarm Reset key at any time, but the trigger 
    // will only fire once the unrestricted phase of exhalation is active.
    ASSERTION((PhaseMgr::S_GetInstance())->GetCurrentPhaseId() == eExhPhase);

    // Go to previous mode or the operator set mode if no non-resettable
    // breathing alarms are active.
    modeMgrPtr = ModeMgr::S_GetInstance ();
    modeMgrPtr->ReturnToLastMode ();

}   // ProcessAlarmResetTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter()
//
//    Processing:   Process doing something when another mode enters this mode
//
//    Input Parameters:
//      E_VentilationMode fromModeId
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
void AlarmBreathingMode::Enter (E_VentilationMode fromModeId)
{
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter()
//
//    Processing:   Process doing something when exit from this mode
//
//    Input Parameters:
//      E_VentilationMode fromModeId
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
void AlarmBreathingMode::Exit (void)
{
}
