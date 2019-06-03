/******************************************************************************/
//$COMMON.CPP$
//   File Name:  AlarmResetOpReqTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Alarm Reset 
//      Operator Request Trigger.  This class multiply inherits from the
//      Trigger base class and the OpRequest base class and provides the 
//      functionality to manually reset alarm conditions such as Apnea
//      in response to an operator request.  
//
//   Interfaces:
//      The CheckCondition() operation interfaces with the PhaseMgr to determine
//      the current breath phase.  Otherwise, operations in this class only
//      invoke methods defined in the classes from which it is derived.
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
#include <ModeMgr.h>
#include <PhaseMgr.h>
#include "AlarmTask.h"
#include "AlarmResetOpReqTrigger.h"

#include "DebugTrace.h"

// define statics
AlarmResetOpReqTrigger* AlarmResetOpReqTrigger::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the AlarmResetOpReqTrigger object, it invokes
//      AlarmResetOpReqTrigger::S_GetInstance(), which returns a pointer to
//      the AlarmResetOpReqTrigger object.
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
//      AlarmResetOpReqTrigger* - pointer to the instance of this class
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
AlarmResetOpReqTrigger* AlarmResetOpReqTrigger::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new AlarmResetOpReqTrigger ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This operation is called whenever a mode is entered where the alarm
//      reset is active.  The key itself is always active, but it only
//      affects ventilation in modes that can be manually reset.  This operation
//      sets the IsArmed attribute to FALSE, indicating that no request has
//      been made and invokes the base class Trigger::EnableTrigger() 
//      operation.
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
void AlarmResetOpReqTrigger::EnableTrigger (void)
{
    IsArmed = eFalse;
    Trigger::EnableTrigger ();

}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing:  Get Armed value
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
E_Bool AlarmResetOpReqTrigger::GetIsArmed(void)
{
    return IsArmed;
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation is invoked every BD cycle while this trigger is enabled.
//
//      This operation checks the IsArmed attribute to determine if the operator
//      has made an Alarm Reset Request.  If the operator has made a request
//      and the ventilator is in the unrestricted phase of exhalation, 
//      TriggerDidFire is set to True.  
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
E_Bool AlarmResetOpReqTrigger::CheckCondition (void)
{
    // If an alarm reset request has been received (indicated by IsArmed) and
    // if the ventilator is in the exhalation phase, set TriggerDidFire to True. 
    if((eTrue == IsArmed)
            && (eExhPhase == (PhaseMgr::S_GetInstance ())->GetCurrentPhaseId ()))
    {
        TriggerDidFire = eTrue;
    }

    return (TriggerDidFire);

}   // end CheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessRequest()
//
//    Processing: 
//      This operation executes when the operator has pressed the alarm reset
//      key.  The IsArmed attribute is set to True to indicate that the operator
//      has requested the manual reset and AcceptRequest() is called to provide
//      operator feedback for the key press.
//
//    Input Parameters:
//      command -- Start or Stop command; should always be start.
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
//      If the command is anything but Start, error handling is invoked.
//
//    Requirements:  
//
/******************************************************************************/
void AlarmResetOpReqTrigger::ProcessRequest (E_OpRequestCommand command)
{

    // Check if the request is to start a manual inhalation
    if(eStart == command)
    {
        // Set IsArmed to True to indicate that the operator has requested that
        // alarms be reset.  
        if (IsIgnored == eTrue)
        {
            IsIgnored = eFalse;
        }
        else
        {
            AlarmTask::S_GetInstance()->SetResetFlag(eTrue);

        }

        IsArmed = eTrue;

        // The ModeMgr may need to updates its mode list based upon the 
        // manual reset.
        ModeMgr::S_GetInstance ()->ProcessManualReset ();

        // Provide audible feedback to the operator.  Alarm Reset requests are
        // always accepted.
        // AcceptRequest();
    }
    else
    {
        // An alarm reset can only be requested to start.
        ASSERTION(eStart == command);
    }

}   // end ProcessRequest()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AlarmResetOpReqTrigger()
//
//    Processing: 
//      This operation is the constructor for the Alarm Reset Operator Request
//      Trigger.  This operation invokes the base class Trigger constructor
//      with the TriggerId and the base class OpRequest() constructors.  And,
//      it initializes the IsArmed attribute to FALSE since the trigger cannot
//      be armed at start-up.
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
AlarmResetOpReqTrigger::AlarmResetOpReqTrigger (void) :
        Trigger (eAlarmResetOpReqTrigger), OpRequest (), IsArmed (eFalse)
{

}   // end AlarmResetOpReqTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing:  Set value for IsIgnore value- ignore alarm or not
//
//    Input Parameters:
//      E_Bool state - state of IsIgnore
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
void AlarmResetOpReqTrigger::SetIsIgnoreAlarmReset(E_Bool state)
{
    IsIgnored = state;
}
