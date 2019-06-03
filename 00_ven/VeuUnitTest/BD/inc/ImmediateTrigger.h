/******************************************************************************/
//$COMMON.H$
//   File Name:  ImmediateTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The ImmediateTrigger is derived from the base Trigger class.
//      It is used to cause an immediate transition out of the StartUp Mode.
/******************************************************************************/
#ifndef IMMEDIATETRIGGER_H
#define IMMEDIATETRIGGER_H

#include <Trigger.h>

class ImmediateTrigger : public Trigger
{
public:
    //Constructor
    ImmediateTrigger (void);

    //Check condition and process action
    virtual E_Bool CheckCondition (void);

    //Process action base on trigger ID
    virtual inline void ProcessTrigger (Mode* mode);

    //Copy constructor
    ImmediateTrigger (const ImmediateTrigger&);

};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessImmediateTrigger() operation of the 
//      active mode.
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
inline void ImmediateTrigger::ProcessTrigger (Mode* mode)
{
    // Invoke the method to handle the Immediate Trigger in the currently
    // active mode.
    mode->ProcessTriggerID(eImmediateTrigger);

}   // end ProcessTrigger()

#endif  // IMMEDIATETRIGGER_H
