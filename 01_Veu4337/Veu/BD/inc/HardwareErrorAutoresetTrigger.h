/******************************************************************************/
//$COMMON.H$
//   File Name:  HardwareErrorAutoresetTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the HardwareErrorAutoresetTrigger
/******************************************************************************/
#ifndef HARDWAREERRORAUTORESETTRIGGER_H
#define HARDWAREERRORAUTORESETTRIGGER_H

#include "Trigger.h"

class HardwareErrorAutoresetTrigger : public Trigger
{
public:
    //pointer to the instance of this class
    static HardwareErrorAutoresetTrigger* S_GetInstance(void);

    //Check condition and process action
    virtual E_Bool CheckCondition(void);

    //Enable trigger
    virtual void EnableTrigger(void);

    //Process action base on trigger ID
    virtual inline void ProcessTrigger(Mode* mode);

protected:

private:
    //constructor
    HardwareErrorAutoresetTrigger(void);

    //Copy constructor
    HardwareErrorAutoresetTrigger(const HardwareErrorAutoresetTrigger&);



    //$COMMON.ATTRIBUTE$
    //  Name: TimeCounter
    //  Description:  System Time that the condition was met; used to
    //      insure the criteria are met for a minimum amount of time
    //  Units: TICKS
    //  Range: 0 - maximum UNSIGNED value
    UNSIGNED TimeCounter;

    //$COMMON.ATTRIBUTE$
    //  Name: IsArmed
    //  Description:  Indicates if the trigger has fired but
    //  not honored yet.
    //  Units: None
    //  Range: eTrue, eFalse
    E_Bool IsArmed;

    //$COMMON.ATTRIBUTE$
    //  Name: two_second
    //  Description:
    //  Units: None
    //  Range:
    int two_second;

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static HardwareErrorAutoresetTrigger* S_Instance;
};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing:
//      This operation invokes the HardwareErrorAutoresetTrigger() operation of
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
inline void HardwareErrorAutoresetTrigger::ProcessTrigger(Mode* mode)
{
    // Invoke the operation to handle the High MAP_2 trigger in the current mode.
    mode->ProcessTriggerID(eHardwareErrorAutoresetTrigger);

}   // end ProcessTrigger()

#endif // HARDWAREERRORAUTORESETTRIGGER_H
