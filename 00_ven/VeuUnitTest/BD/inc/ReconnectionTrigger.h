/******************************************************************************/
//$COMMON.H$
//   File Name:  ReconnectionTrigger.h
//
//   Class: 
//      This file contains the declaration for the Reconected Trigger
//      class.  The Reconected Trigger is derived from Trigger
//      class 
/******************************************************************************/
#ifndef RECONNECTIONTRIGGER_H
#define RECONNECTIONTRIGGER_H

#include "Trigger.h"

class ReconnectionTrigger : public Trigger
{
public:
    //pointer to the instance of this class
    static ReconnectionTrigger* S_GetInstance(void);

    //Check condition and process action
    virtual E_Bool CheckCondition(void);

    //Enable trigger
    virtual void EnableTrigger(void);

    //Process action base on trigger ID
    virtual inline void ProcessTrigger(Mode* mode);

    //Constructor
    ReconnectionTrigger(void);
    //Copy constructor
    ReconnectionTrigger(const ReconnectionTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static ReconnectionTrigger* S_Instance;
};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessReconectedTrigger() operation of 
//      the HFO mode.
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
inline void ReconnectionTrigger::ProcessTrigger(Mode* mode)
{
    // Invoke the operation to handle the Reconnection trigger in the current mode.
    mode->ProcessTriggerID(eReconnectionTrigger);

}   // end ProcessTrigger()

#endif // RECONNECTIONTRIGGER_H
