/******************************************************************************/
//$COMMON.CPP$
//   File Name:  ApneaAutoResetTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Apnea Auto-Reset Trigger.
//      The patient can cause the ventilator to auto-reset out of Apnea 
//      Ventilation mode by triggering 2 PIMs before a VIM is delivered.
//      The operations and attributes of this class maintain a state machine
//      to keep track of the Apnea Auto-reset processing.
//
//   Interfaces:
//      The CheckCondition() operation interfaces with the ExhPhase object
//      via S_IsInUnrestrictedPhase() to determine if the unrestricted phase
//      of exhalation is active.   
//      
//      The UpdateState() operation interfaces also invokes the base class
//      ReportAlarmReset() to update the Apnea status.    
//
//      Otherwise, operations in this class only invoke methods defined in 
//      base Trigger class.
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
#include <ExhPhase.h>
#include <ModeMgr.h>
#include "ApneaAutoResetTrigger.h"
#include "ApneaAlarmTimeTrigger.h"
#include "AlarmTask.h"
#include "DebugTrace.h"

// Define statics
ApneaAutoResetTrigger* ApneaAutoResetTrigger::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the ApneaAutoResetTrigger object, it invokes
//      ApneaAutoResetTrigger::S_GetInstance(), which returns a pointer
//      to the ApneaAutoResetTrigger object.
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
//      ApneaAutoResetTrigger* - pointer to the instance of this class
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
ApneaAutoResetTrigger* ApneaAutoResetTrigger::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new ApneaAutoResetTrigger ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end ApneaAutoResetTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This operation is called whenever Apnea Ventilation mode is entered
//      to enable the Apnea Auto-reset trigger.
//      
//      This operation sets the State to eWaiting1 to indicate that the 
//      apnea auto-reset trigger is waiting for the first PIM.  The base
//      class Trigger::EnableTrigger() is then called to perform processing
//      common to all triggers.
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
void ApneaAutoResetTrigger::EnableTrigger (void)
{
    // Set the State to eWaiting1, waiting for the 1st PIM
    State = eWaiting1;
    Trigger::EnableTrigger ();

}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation is invoked every BD cycle while this trigger is enabled.
//      If the State attribute is eArmed, indicating that the auto-reset
//      criteria have been detected, and the ventilator is in the unrestricted
//      phase of exhalation, the TriggerDidFire attribute is  set to True.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the trigger fired or not
//
//    Pre-Conditions:
//      This operation assumes that the TriggerMgr only invokes it while
//      the trigger is enabled.
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
E_Bool ApneaAutoResetTrigger::CheckCondition (void)
{
    // If the trigger is armed, meaning that the auto-reset criteria have 
    // been detected, and the ventilator is in the unrestricted phase of 
    // exhalation, set the TriggerDidFire attribute to True.
    if((eArmed == State) && (eTrue == ExhPhase::S_IsInUnrestrictedPhase ()))
    {
        TriggerDidFire = eTrue;
    }

    // If not updated above, this value is still False.  Return value to caller.
    return (TriggerDidFire);

}   // end CheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdateState()
//
//    Processing: 
//      This operation is called each time a PIM or VIM is triggered in the
//      AV Mode.
//
//    Input Parameters:
//      id - Trigger ID for the time trigger or patient inhalation trigger in AV
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
//      If the State contains an unexpected value or if this operation is
//      called with any trigger ID other than the AV BP Time Trigger ID or 
//      one of the Patient Inhalation Trigger IDs, error handling is performed.
//
//    Requirements:  
//
/******************************************************************************/
void ApneaAutoResetTrigger::UpdateState (E_TriggerId id)
{
    // If a VIM was trigger, go back to the Waiting 1 State
    if(eAvBpTimeTrigger == id)
    {
        State = eWaiting1;

    }
    // Check if the inhalation was patient triggered.   
    else if((ePtInhPressTrigger == id) || (ePtInhFlowTrigger == id))
    {
        // If the trigger is in the Waiting1 state, update the state to 
        // Waiting2, waiting for the 2nd PIM    
        if(eWaiting1 == State)
        {
            State = eWaiting2;
        }

        // If the trigger is already in the Waiting2 state, the reset criteria
        // have been met.
        else if(eWaiting2 == State)
        {
            // Set the state to armed.  Even though the auto-reset criteria
            // have been met, the ventilator does not transition out of AV
            // until the patient initiated inhalation is delivered and a
            // minimum exhalation time has elapsed.
            State = eArmed;

            // Update the Apnea Alarm status.  Even though the ventilator
            // does not officially transition out of AV until the inhalation
            // is complete, the alarm indications are updated once the 
            // reset is detected.
            AlarmTask::S_GetInstance ()->SetIsActive (eApnea, eNotActive);

            // Tell ModeMgr about the auto reset
            ModeMgr::S_GetInstance ()->ProcessAutoReset ();

            // Restart apnea interval timing since the inhalation is starting
            // now.
            (ApneaAlarmTimeTrigger::S_GetInstance ())->EnableTrigger ();
        }
        else
        {
            // Invalid state
            ASSERTION(eFalse);
        }
    }
    else
    {
        // Invalid trigger ID
        ASSERTION(eFalse);
    }

}   // end UpdateState()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ApneaAutoResetTrigger()
//
//    Processing: 
//      This operation is the constructor for the Apnea Auto Reset Trigger.
//      This operation invokes the base class Trigger constructor with the
//      Trigger ID and initializes the State attribute to eIdle.
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
ApneaAutoResetTrigger::ApneaAutoResetTrigger (void) :
        Trigger (eApneaAutoResetTrigger), State (eIdle)
{

}   // end ApneaAutoResetTrigger()

