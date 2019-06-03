/******************************************************************************/
//$COMMON.H$
//   File Name:  ManualInhOpReqTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the Manual Inhalation
//      Operator Request Trigger.  ManualInhOpReqTrigger is derived from
//      the Trigger and the OpRequest base classes.
/******************************************************************************/
#ifndef MANUALINHOPREQTRIGGER_H
#define MANUALINHOPREQTRIGGER_H

#include <Trigger.h>
#include "OpRequest.h"

class ManualInhOpReqTrigger : public Trigger, public OpRequest
{
public:
    //pointer to the instance of this class
    static ManualInhOpReqTrigger* S_GetInstance (void);

    //invokes the ProcessManualInhOpReqTrigger() operation of active mode
    virtual inline void ProcessTrigger (Mode* mode);

    //TriggerDidFire attribute is set to TRUE if they pass condition follow commnad
    virtual void ProcessRequest (E_OpRequestCommand command);

    //Constructor
    ManualInhOpReqTrigger (void);
    //Copy constructor
    ManualInhOpReqTrigger (const ManualInhOpReqTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static ManualInhOpReqTrigger* S_Instance;
};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessManualInhOpReqTrigger() operation of 
//      the active mode.
//
//    Input Parameters:
//      mode - pointer to the currently active Mode object
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
inline void ManualInhOpReqTrigger::ProcessTrigger (Mode* mode)
{
    // Invoke the operation to handle the Manual Inhalation trigger in the
    // current mode.
    mode->ProcessTriggerID(eManualInhOpReqTrigger);

}   // end ProcessTrigger()

#endif  // MANUALINHOPREQTRIGGER_H
