/******************************************************************************/
//$COMMON.CPP$
//   File Name:  Mode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: This is the base Mode class.  All Mode classes are derived
//      from this class.  This class contains operations for handling the
//      triggers when they fire.   All operations in this class that handle 
//      trigger invoke error handling.  If a trigger is valid in a mode, 
//      that mode class redefines the operation to handle the trigger
//      appropriately.
//
//   Interfaces:
//      Operations in this class interface with the the error handler since
//      the trigger operations in this base class should not be called.
//
//   Restrictions:
//      This class is an abstract base class.  It contains pure virtual 
//      methods and cannot be instantiated.
/******************************************************************************/
#include <Mode.h>
#include "MgrStemMotor.h"
#include "DebugTrace.h"

// Define statics
E_TriggerId Mode::S_LastTriggerId = eNoTrigger;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Mode()
//
//    Processing: 
//      This operation is the constructor.  It is an empty constructor.
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
Mode::Mode (E_Bool doesManualReset) :
                        DoesManualReset (doesManualReset)
{

}   // end Mode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Mode()
//
//    Processing:  Copy constructor for Mode class
//
//
//    Input Parameters:
//      const Mode&
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
inline Mode::Mode (const Mode&)
{

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_SetLastTriggerId()
//
//    Processing:
//      This operation sets the static attribute S_LastTriggerId to the passed
//      trigger ID.
//
//    Input Parameters:
//      id - Trigger ID
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
inline void Mode::S_SetLastTriggerId (E_TriggerId id)
{
    S_LastTriggerId = id;

}   // end S_SetLastTriggerId()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_IsInhPtTriggered()
//
//    Processing: 
//      This static operation uses the S_LastTriggerId attribute to determine if
//      an inhalation was patient triggered.  This operation is invoked at
//      the start of a mandatory inhalation to determine if the breath is
//      an assist breath or not.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      TRUE if the inhalation was patient triggered; FALSE otherwise.
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
E_Bool Mode::S_IsInhPtTriggered (void)
{
    E_Bool rtnValue = eFalse;

    // If the inhalation was triggered by the patient, set the return value to
    // TRUE.
    if((ePtInhPressTrigger == S_LastTriggerId)
            || (ePtInhFlowTrigger == S_LastTriggerId))
    {
        rtnValue = eTrue;
    }

    return (rtnValue);

}   // end S_IsInhPtTriggered()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CreateSafeState()
//
//    Processing: Create a safe state
//
//    Input Parameters:
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
void Mode::CreateSafeState()
{
//    MgrStemMotor::O2Valve->SetStepPosition(0);
//    MgrStemMotor::AirValve->SetStepPosition(0);
//    MgrStemMotor::ExhValve->SetStepPosition(0);
//    MgrStemMotor::HfoValve->SetStepPosition(0);
}   //end CreateSafeState()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing:
//      This operation simply invokes the error handler since all modes that
//      use this trigger define their own processing.  If this operation is
//      called, it means that the trigger fired in a mode where the trigger
//      was not expected to be enabled or not expected to fire.
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
//      This operation is not expected to be called.
//
//    Requirements:
//
/******************************************************************************/
void Mode::ProcessTriggerID (E_TriggerId id)
{
    // This method should not be invoked; derived classes that enable this
    // trigger and expect it to fire should provide the process method!
    ASSERTION(eFalse);
}   // end ProcessTrigger()
