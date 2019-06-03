/******************************************************************************/
//$COMMON.CPP$
//   File Name:  PlateauPhase.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      The file contains the implementation of the PlateauPhase class.  
//      The plateau phase follows a VCV inhalation when the operator setting
//      for plateau time is greater than 0.  During a plateau, the delivery
//      and exhalation valves remain closed.
//
//   Interfaces:
//      The Start() operation of this class calls MgrBreathData::PlateauStart()
//      to notify that object of the start of the plateau phase.
//      The Start() and End() operations also interface with TriggerMgr to
//      enable and disable the Plateau Time Trigger via the EnableTrigger()
//      and DisableTrigger() methods.
//
//   Restrictions:
//      There is only one instance of this class.
/******************************************************************************/
#include "PlateauPhase.h"
#include "TriggerMgr.h"
#include "MgrBreathData.h"
#include "MgrController.h"
#include "DebugTrace.h"

// Define statics
PlateauPhase* PlateauPhase::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants to
//      access the PlateauPhase object, it calls PlateauPhase::S_GetInstance(),
//      which returns a pointer to the PlateauPhase object.
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
//      PlateauPhase* - pointer to the object instantiated from this class
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
PlateauPhase* PlateauPhase::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new PlateauPhase ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PlateauPhase()
//
//    Processing: 
//      This operation is the constructor for the Plateau Phase.  It invokes
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
PlateauPhase::PlateauPhase (void) :
        Phase ()
{

}   // end PlateauPhase()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing: 
//      This operation is invoked at the start of the plateau phase.  It
//      enables the PlateauTimeTrigger and notifies the MgrBreathData object
//      of the start of plateau.
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
//
/******************************************************************************/
void PlateauPhase::Start (void)
{
    E_BreathPhaseIndicator breathType;
    breathType = ePlateau;
    // Enable the plateau trigger
    (TriggerMgr::S_GetInstance())->EnableTrigger(ePlateauTimeTrigger);

    // Notify the Breath Data Manager of the start of plateau.  MgrBreathData
    // is then responsible for notifying the GUI of the plateau phase.
    (MgrBreathData::S_GetInstance())->PlateauStart(breathType);

    MgrController::VCVServoController->SetSepecificPhase(eThePlateauPhase);

}   // end Start()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing: 
//      This operation is invoked at the end of the plateau phase.  This
//      operation interfaces with the Trigger Manager to disable the 
//      Plateau Time Trigger.
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
//
/******************************************************************************/
void PlateauPhase::End (void)
{
    // Disable the plateau trigger
    (TriggerMgr::S_GetInstance ())->DisableTrigger (ePlateauTimeTrigger);

}   // end End()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBdCycle()
//
//    Processing: 
//      This operation performs no processing.  It is needed to conform to
//      the structure of a Phase class, from which it is derived.
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
void PlateauPhase::ProcessBdCycle (void)
{

}   // end ProcessBdCycle()

