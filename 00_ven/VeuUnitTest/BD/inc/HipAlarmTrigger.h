/******************************************************************************/
//$COMMON.H$
//   File Name:  HipAlarmTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the declaration for the HIP Alarm Trigger
//      class.  The HIP Alarm Trigger is derived from the Trigger class.
/******************************************************************************/
#ifndef HIPALARMTRIGGER_H
#define HIPALARMTRIGGER_H

#include "Trigger.h"

class HipAlarmTrigger : public Trigger
{
public:
    //Constructor
    HipAlarmTrigger (E_TriggerId triggerId, SHORT alarmIx);

    //pointer to the instance of this class
    static HipAlarmTrigger* S_GetInstance(void);

    //retrieves the current pressure measured in the exhalation
    virtual E_Bool CheckCondition (void);

    //Disable trigger
    virtual void DisableTrigger (void);

    //Process action base on trigger id
    virtual inline void ProcessTrigger (Mode* mode);

    //set inactive timer
    void SetInactiveTimer(USHORT value);


    //$COMMON.ATTRIBUTE$
    //    Name: AlarmIx
    //    Description: Alarm index.  Used to access the AlarmStat object
    //      associated with this alarm.
    //    Units:  None
    //    Range:  n/a
    SHORT AlarmIx;

    //Constructor
    HipAlarmTrigger(void);
    //Copy constructor
    HipAlarmTrigger(const HipAlarmTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static HipAlarmTrigger* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: InactiveTimer
    //  Description:  Keep track the time of being inactive.
    //  Units: None
    //  Range: n/a
    USHORT InactiveTimer;

    //$COMMON.ATTRIBUTE$
    //  Name: hip1Count
    //  Description:  Hip 1 counter
    //  Units: None
    //  Range: n/a
    LONG hip1Count;

};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessHipAlarmTrigger() operation of the 
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
inline void HipAlarmTrigger::ProcessTrigger (Mode* mode)
{
    // Invoke the operation to handle the HIP trigger in the current mode.
    mode->ProcessTriggerID(eHipAlarmTrigger);

}   // end ProcessTrigger()

#endif // HIPALARMTRIGGER_H
