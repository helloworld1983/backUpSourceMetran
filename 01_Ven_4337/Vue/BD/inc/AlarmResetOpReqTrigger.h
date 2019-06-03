/******************************************************************************/
//$COMMON.H$
//   File Name:  AlarmResetOpReqTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The alarm reset trigger class is used to trigger a reset
//      out of the Apnea Ventilation or Occlusion mode when the Alarm Reset 
//      key is pressed. This class is derived from the base Trigger and
//      OpRequest classes.
/******************************************************************************/
#ifndef ALARMRESETOPREQTRIGGER_H
#define ALARMRESETOPREQTRIGGER_H
#include "Trigger.h"
#include "OpRequest.h"
#include "BdInterface.h"

class AlarmResetOpReqTrigger : public Trigger, public OpRequest
{
public:
    //Get Armed value
    E_Bool GetIsArmed (void);

    //pointer to the instance of this class
    static AlarmResetOpReqTrigger* S_GetInstance (void);

    //invokes the ProcessAlarmResetTrigger() operation of the
    virtual inline void ProcessTrigger (Mode* mode);

    //Enable trigger
    virtual void EnableTrigger (void);

    //indicates if the trigger fired or not
    virtual E_Bool CheckCondition (void);

    //executes when the operator has pressed the alarm reset key
    virtual void ProcessRequest (E_OpRequestCommand command);

    //Set ignore alrm or not
    void SetIsIgnoreAlarmReset(E_Bool state);

protected:

private:
    //$COMMON.ATTRIBUTE$
    //  Name: IsArmed
    //  Description:
    //      Indicates if the Alarm Reset Request has been received but
    //      not honored yet.
    //  Units: None
    //  Range: eTrue, eFalse
    E_Bool IsArmed;

    //$COMMON.ATTRIBUTE$
    //  Name: IsIgnored
    //  Description:
    //      Indicates if the Alarm Reset is ignored
    //  Units: None
    //  Range: eTrue, eFalse
    E_Bool IsIgnored;

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static AlarmResetOpReqTrigger* S_Instance;

    //Copy constructor
    AlarmResetOpReqTrigger (const AlarmResetOpReqTrigger&);
    //Constructor
    AlarmResetOpReqTrigger (void);

};

#endif  // ALARMRESETOPREQTRIGGER_H

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
inline void AlarmResetOpReqTrigger::ProcessTrigger (Mode* mode)
{
    // Invoke the method to handle the Alarm Reset Request in the currently
    // active mode.
    mode->ProcessTriggerID(eAlarmResetOpReqTrigger);

}   // end ProcessTrigger()
