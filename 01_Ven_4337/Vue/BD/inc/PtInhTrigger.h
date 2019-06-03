/******************************************************************************/
//$COMMON.H$
//   File Name:  PtInhTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The alarm reset trigger class is used to trigger a reset
//      out of the Apnea Ventilation or Occlusion mode when the Alarm Reset 
//      key is pressed. This class is derived from the base Trigger and
//      OpRequest classes.
/******************************************************************************/
#ifndef PTINHTRIGGER_H
#define PTINHTRIGGER_H
#include "Trigger.h"
#include "BdInterface.h"

class PtInhTrigger : public Trigger
{
public:

    //Get armed
    E_Bool GetIsArmed(void);

    //pointer to the instance of this class
    static PtInhTrigger* S_GetInstance(void);

    //process action base on trigger ID
    virtual inline void ProcessTrigger(Mode* mode);

    //Receive event
    void ReceiveEvent(void);

    //enable trigger
    virtual void EnableTrigger(void);

    //check condition and process action
    virtual E_Bool CheckCondition(void);

    //Check previous condition
    virtual E_Bool PreCheckCondition(void);

protected:

private:
    //$COMMON.ATTRIBUTE$
    //  Name: IsArmed
    //  Description:
    //      Indicates if the Alarm Reset Request has been received
    //  but not honored yet.
    //  Units: None
    //  Range: eTrue, eFalse
    E_Bool IsArmed;

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static PtInhTrigger* S_Instance;

    E_BdEventId OpReqEvent;

    PtInhTrigger(const PtInhTrigger&);
    PtInhTrigger(void);

};

#endif  // PTINHOPREQTRIGGER_H

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessAlarmResetTrigger() operation of the 
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
inline void PtInhTrigger::ProcessTrigger(Mode* mode)
{
    // Invoke the method to handle the Patient Inhalation Breath in the currently
    // active mode.
//    mode->ProcessTriggerID(ePtInhTrigger);

}   // end ProcessTrigger()
