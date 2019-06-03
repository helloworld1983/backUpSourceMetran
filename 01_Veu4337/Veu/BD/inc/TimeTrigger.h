/******************************************************************************/
//$COMMON.H$
//   File Name:  TimeTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: This file contains the class declaration for the TimeTrigger 
//      class.  The TimeTrigger class is derived from the Nucleus Plus 
//      RTOS class Timer and the BD Application class Trigger.
/******************************************************************************/
#ifndef TIMETRIGGER_H
#define TIMETRIGGER_H

#include <Trigger.h>
#include "Timer.h"

class TimeTrigger : public Trigger , public Timer
{
public:
    //Constructor
    TimeTrigger (E_TriggerId id, CHAR* timerName);

    //Enable trigger
    virtual void EnableTrigger (void);
    virtual void EnableTrigger (UNSIGNED time);

    //Disable trigger
    virtual void DisableTrigger (void);

    //invoked when a setting tied to the Time Trigger change
    virtual void PhaseInSettingChange (LONG value);

    //call function when timer is expired
    virtual void ExpirationRoutine (void);

    //Process action base on trigger ID
    virtual inline void ProcessTrigger (Mode* mode);

    virtual UNSIGNED GetTimePeriod (void);

protected:
    //Set time period
    virtual void SetTimePeriod (UNSIGNED time);

    //$COMMON.ATTRIBUTE$
    //    Name: TimePeriod
    //    Description:
    //      This attribute contains the time period in system
    //      clock TICKS used to determine the initial and/or restart
    //      time values for the Timer.
    //
    //    Units:  clock TICKS
    //    Range:  0 - max UNSIGNED
    UNSIGNED TimePeriod;

    //$COMMON.ATTRIBUTE$
    //    Name: BackupTimePeriod
    //    Description:
    //      This attribute contains a backup copy of the time period.  Its
    //      value is a one's complement of the TimePeriod and is used to
    //      help insure the TimePeriod does not get corrupted.
    //
    //    Units:  none
    //    Range:  0 - max UNSIGNED
    UNSIGNED BackupTimePeriod;

private:
    //Constructor
    TimeTrigger (void);
    //Copy constructor
    TimeTrigger (const TimeTrigger&);

};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessTimeTrigger() operation of the active
//      mode.
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
inline void TimeTrigger::ProcessTrigger (Mode* mode)
{
    // Invoke the operation to process the time trigger in the currently 
    // active mode.
    mode->ProcessTriggerID(TriggerId);

}   // end ProcessTrigger()

#endif  // TIMETRIGGER_H
