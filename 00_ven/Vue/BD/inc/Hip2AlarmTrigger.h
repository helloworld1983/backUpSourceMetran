/******************************************************************************/
//$COMMON.H$
//   File Name:  Hip2AlarmTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the declaration for the HIP2 Alarm Trigger
//      class.  The HIP2 Alarm Trigger is derived from the HIP Alarm Trigger
//      class and implements a HIP alarm that does not truncate the breath.
/******************************************************************************/
#ifndef HIP2ALARMTRIGGER_H
#define HIP2ALARMTRIGGER_H
#include "Trigger.h"

class Hip2AlarmTrigger : public Trigger
{
public:
    //pointer to the instance of this class
    static Hip2AlarmTrigger* S_GetInstance(void);

    //Disable trigger
    void DisableTrigger(void);

    virtual void ProcessTrigger(Mode* mode);

    //retrieves the current pressure measured in the exhalation
    virtual E_Bool CheckCondition (void);

    //Get high pressure alarm
    LONG GetHighPressAlarm(void);

    //set inactive timer value
    void SetInactiveTimer(USHORT value);

protected:

    //$COMMON.ATTRIBUTE$
    //    Name: HighPressAlarm
    //    Description:
    //    Units:  LONG
    //    Range:
    LONG HighPressAlarm;

    //$COMMON.ATTRIBUTE$
    //  Name: InactiveTimer
    //  Description:  Keep track the time of being inactive.
    //  Units: None
    //  Range: n/a
    USHORT InactiveTimer;

private:
    //Constructor
    Hip2AlarmTrigger (void);
    Hip2AlarmTrigger (const Hip2AlarmTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static Hip2AlarmTrigger* S_Instance;

};

#endif // HIP2ALARMTRIGGER_H
