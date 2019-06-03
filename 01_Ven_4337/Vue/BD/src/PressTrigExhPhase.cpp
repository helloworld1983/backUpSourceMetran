/******************************************************************************/
//$COMMON.CPP$
//   File Name:  PressTrigExhPhase.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file defines the Exhalation Phase with Pressure Triggering class.
//      The Pressure Triggering Exhalation Phase is active during exhalation
//      when the operator setting for Trigger Type is pressure.  
//
//      This class is derived from the ExhPhase base class.
//
//   Interfaces:
//      The Start() operation in this class interfaces with the TriggerMgr
//      to enable triggers specific to a pressure triggering exhalation phase.
//      Operations in this class also invoke methods in the base class ExhPhase
//      to perform processing common to all exhalation phases.
//
//   Restrictions:
//      None
/******************************************************************************/
#include "PressTrigExhPhase.h"
#include "TriggerMgr.h"
#include "MgrFlowController.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PressTrigExhPhase()
//
//    Processing: 
//      This operation is the constructor for the PressTrigExhPhase.  It invokes 
//      the base class ExhPhase constructor.
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
PressTrigExhPhase::PressTrigExhPhase (void) :
        ExhPhase ()
{

}   // end PressTrigExhPhase()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing: 
//      This operation is invoked at the start of every exhalation phase when
//      pressure triggering is active.  This operation invokes ExhPhase::Start()
//      to perform processing common to all exhalation phases and enables the 
//      Patient Inhalation Pressure Trigger.
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
void PressTrigExhPhase::Start (void)
{
    // Close the delivery valves
    MgrFlowController::O2FlowController->SetDesired(0);
    MgrFlowController::AirFlowController->SetDesired(0);

    // Invoke the base class method to perform processing common to all 
    // exhalation phases.
    ExhPhase::Start ();

    // Enable the patient pressure trigger.  Not passing a pressure value
    // causes the trigger to use the operator setting for pressure
    // sensitivity
    (TriggerMgr::S_GetInstance ())->EnableTrigger (ePtInhPressTrigger);

}   // end Start()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing:
//      This operation is invoked at the start of every exhalation phase when
//      pressure triggering is active.  This operation invokes ExhPhase::End()
//      to perform processing common to all exhalation phases and enables the
//      Patient Inhalation Pressure Trigger.
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

void PressTrigExhPhase::End(void)
{

    // Invoke the base class method to perform processing common to all
    // exhalation phases.
    ExhPhase::End();


    // Get a pointer to the TriggerMgr object
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtr->DisableTrigger(ePtInhFlowTrigger);
    triggerMgrPtr->DisableTrigger(ePtInhPressTrigger);

}
