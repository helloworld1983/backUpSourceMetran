/******************************************************************************/
//$COMMON.CPP$
//   File Name:  ManualInhOpReqTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Manual Inhalation
//      Operator Request Trigger.  This class multiply inherits from the
//      Trigger base class and the OpRequest base class and provides the 
//      functionality to trigger a manual inhalation in response to an operator
//      request.  
//
//   Interfaces:
//      MgrOpRequest::ReceiveRequest() invokes the ProcessRequest() operation
//      of this class whenever the BD Task receives a Manual Inhalation request
//      event on the BD Queue.  ProcessRequest() interfaces with the static
//      method ExhPhase::S_CalcTimeSincePhaseStart() to determine if a 
//      manual inhalation can be honored.  Manual inhalations are only honored
//      after a minimal exhalation time has elapsed.  ProcessRequest() also
//      interfaces with the ExhHoldOpReqTrigger object to ensure that an
//      exhalation hold phase is not pending; manual inhalations cannot be
//      used to trigger an exhalation hold.
//
//      The OpRequest base class operations AcceptRequest() or RejectRequest() 
//      are invoked to provide audible feedback to the user with regard to the 
//      manual inhalaiton request.
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
#include <ExhPhase.h>
#include "ManualInhOpReqTrigger.h"

#include "DebugTrace.h"

// define and initialize statics
ManualInhOpReqTrigger* ManualInhOpReqTrigger::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the ManualInhOpReqTrigger object, it invokes
//      ManualInhOpReqTrigger::S_GetInstance(), which returns a pointer to the
//      ManualInhOpReqTrigger object.
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
//      ManualInhOpReqTrigger* - pointer to the instance of this class
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
ManualInhOpReqTrigger* ManualInhOpReqTrigger::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new ManualInhOpReqTrigger ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ManualInhOpReqTrigger()
//
//    Processing: 
//      This operation is the constructor for objects of this class.
//      This operation calls the base class Trigger constructor with the
//      TriggerId and calls the base class OpRequest constructor.
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
ManualInhOpReqTrigger::ManualInhOpReqTrigger (void) :
                        Trigger (eManualInhOpReqTrigger), OpRequest ()
{

}   // end ManualInhOpReqTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessRequest()
//
//    Processing: 
//      This method is invoked by MgrOpRequest::ReceiveRequest() whenever the
//      Manual Inhalation is requested.  This operation checks if the 
//      Manual Inhalation trigger is enabled (it is only enabled during
//      exhalation), if a minimum amount of time in exhalation has passed and
//      no exhalation hold is pending.  If all of the conditions are met,
//      the TriggerDidFire attribute is set to TRUE so that the
//      BD Task transitions to a mandatory inhalation.  
//
//      If the Trigger is either not enabled or if the ventilator has not
//      been in exhalation for a minimum amount of time or an exhalation hold
//      is pending, the request is rejected.
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
void ManualInhOpReqTrigger::ProcessRequest (E_OpRequestCommand command)
{
    // Check if the request is to start a manual inhalation
    if (eStart == command)
    {
        // Check if the trigger is enabled and the ventilator is in the
        // unrestricted phase of exhalation
        if ((eTrue == IsEnabled) &&
                (ExhPhase::S_IsInUnrestrictedPhase() == eTrue))
        {

            // Set the TriggerDidFire attribute so that the BD Task will start
            // a mandatory inhalation.
            TriggerDidFire = eTrue;

            // Provide feedback to the operator
            AcceptRequest();
        }

        else
        {
            // If the request was received during inhalation or the restricted
            // phase of exhalation, it must be rejected.  Provide feedback
            // to the operator
            RejectRequest();
        }
    }
    else
    {
        // A manual inhalation can only be requested to start.
        ASSERTION(eStart == command);
    }

}   // end ProcessRequest()

