/******************************************************************************/
//$COMMON.H$
//   File Name:  HighMAP_2AutoResetTrigger.h
//
//   Class: 
//      This file contains the declaration for the HighMAP_2 Auto Reset Trigger
//      class.  The HighMAP_2 Auto Reset Trigger is derived from Trigger
//      class 
//
//   Revision History:
/******************************************************************************/
#ifndef HIGHMAP_2AUTORESETTRIGGER_H
#define HIGHMAP_2AUTORESETTRIGGER_H

#include "Trigger.h"

class HighMAP_2AutoResetTrigger : public Trigger
{
public:
    //pointer to the instance of this class
    static HighMAP_2AutoResetTrigger* S_GetInstance(void);

    //check condition and process action
    virtual E_Bool CheckCondition(void);

    //enable trigger
    virtual void EnableTrigger(void);

    //process trigger
    virtual inline void ProcessTrigger(Mode* mode);

protected:

private:
    //Constructor
    HighMAP_2AutoResetTrigger(void);

    //Copy constructor
    HighMAP_2AutoResetTrigger(const HighMAP_2AutoResetTrigger&);


    //$COMMON.ATTRIBUTE$
    //  Name: TimeCounter
    //  Description:  System Time that the condition was met; used to
    //      insure the criteria are met for a minimum amount of time
    //  Units: TICKS
    //  Range: 0 - maximum UNSIGNED value
    UNSIGNED TimeCounter;

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static HighMAP_2AutoResetTrigger* S_Instance;
};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessHighMAP_2AutoResetTrigger() operation of 
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
inline void HighMAP_2AutoResetTrigger::ProcessTrigger(Mode* mode)
{
    // Invoke the operation to handle the High MAP_2 trigger in the current mode.
    mode->ProcessTriggerID(eHighMAP_2AutoResetTrigger);
}   // end ProcessTrigger()


#endif // HIGHMAP_2AUTORESETTRIGGER_H

