/******************************************************************************/
//$COMMON.H$
//   File Name:  ApneaAutoResetTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The Apnea Auto Reset Trigger class is used to trigger a reset
//      out of Apnea Ventilation when the auto-reset criteria are met.
//      This class is derived from the base Trigger class.
/******************************************************************************/
#ifndef APNEAAUTORESETTRIGGER_H
#define APNEAAUTORESETTRIGGER_H
#include <Trigger.h>
#include "BdInterface.h"

// The Auto-Reset state indicates the state of the apnea auto-reset
// eIdle = Trigger is not enabled and never has been enabled; this state is 
//      only used when the trigger is first created
// eWaiting1 = Trigger is enabled and waiting for the first PIM
// eWaiting2 = Trigger is enabled and waiting for the 2nd consecutive PIM
// eArmed = auto-reset criteria have been met and waiting for the unrestricted
//   phase of exhalation to transition out of Apnea Ventilation Mode.
enum E_AutoResetState
{
    eIdle, eWaiting1, eWaiting2, eArmed
};

class ApneaAutoResetTrigger : public Trigger
{
public:

    //Get auto reset state
    inline E_AutoResetState GetState (void)
    {
        return State;
    }

    //pointer to the instance of this class
    static ApneaAutoResetTrigger* S_GetInstance (void);

    //process action base on trigger id
    virtual inline void ProcessTrigger (Mode* mode);

    //Enable trigger
    virtual void EnableTrigger (void);

    //Check condition and process action
    virtual E_Bool CheckCondition (void);

    //Update state
    void UpdateState (E_TriggerId id);

protected:

private:
    //$COMMON.ATTRIBUTE$
    //  Name: State
    //  Description:  State of the Apnea Auto-reset Trigger
    //  Units: None
    //  Range: E_AutoResetState
    E_AutoResetState State;

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static ApneaAutoResetTrigger* S_Instance;

    //Copy constructor
    ApneaAutoResetTrigger (const ApneaAutoResetTrigger&);
    //Constructor
    ApneaAutoResetTrigger (void);

};

#endif  // APNEAAUTORESETTRIGGER_H

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessApneaAutoResetTrigger() operation of 
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
inline void ApneaAutoResetTrigger::ProcessTrigger (Mode* mode)
{
    // Invoke the method to handle the Apnea Auto-Reset trigger in the currently
    // active mode.
    mode->ProcessTriggerID(eApneaAutoResetTrigger);

}   // end ProcessTrigger()
