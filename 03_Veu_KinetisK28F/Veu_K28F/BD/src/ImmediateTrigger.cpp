/******************************************************************************/
//$COMMON.CPP$
//   File Name:  ImmediateTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Immediate Trigger.
//      The immediate trigger is used to transition out of the StartUp
//      Mode once the first ventilator settings are received by the BD Task
//      From the GUI Task.
//
//   Interfaces:
//      None
//
//   Restrictions:
//      None
/******************************************************************************/
#include "ImmediateTrigger.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ImmediateTrigger()
//
//    Processing: 
//      This operation is the constructor for the Immediate Trigger.  It
//      simply calls the base class Trigger constructor with the Immediate
//      Trigger ID.
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
ImmediateTrigger::ImmediateTrigger (void) :
        Trigger (eImmediateTrigger)
{

}   // end ImmediateTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation is called when the Immediate Trigger is enabled to
//      determine if the immediate trigger has fired.  The immediate trigger
//      has fired if it is enabled.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      Boolean indication of whether or not the trigger fired
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
E_Bool ImmediateTrigger::CheckCondition (void)
{
    // The immediate trigger should fire immediately if it is enabled; so,
    // just return the value of the IsEnabled attribute.
    return (IsEnabled);

}   // end CheckCondition()

