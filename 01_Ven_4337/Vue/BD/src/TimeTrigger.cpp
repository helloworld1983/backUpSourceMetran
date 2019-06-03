/******************************************************************************/
//$COMMON.CPP$
//   File Name:  TimeTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file defines a Time Trigger class, which is a class derived
//      from the Trigger base class and the RTOS provided Timer class.  A
//      TimeTrigger provides a means to implement a 1 shot timer associated
//      with a triggering condition. 
//
//   Interfaces:
//      This class is derived from the RTOS Timer class and the Trigger class.
//      Operations in this class invoke methods defined in the base classes.
//
//   Restrictions:
//      None
//
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      03    01/20/98  K.A. McGuigan       Morph
//      Description: 
//          SCR-147 :  In the EnableTrigger() operation where a time value is
//          passed and in the SetTimePeriod() operation where the default time
//          is set, the time value is modified by subtracting half of the BD
//          cycle time so that it is processed in the BD cycle closer to the
//          desired time.   
/******************************************************************************/
#include "TimeTrigger.h"
#include "Tasks.h"
#include "DebugTrace.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: TimeTrigger()
//
//    Processing: 
//      This operation is the constructor for objects of the TimeTrigger class.
//      This operation invokes the constructors for the Trigger and Timer
//      classes from which this class is derived.  The Timer constructor is
//      invoked with 0's for the initial and reschedule times and with FALSE
//      to indicate that the timer is not active.
//
//    Input Parameters:
//      id - trigger identifier
//      timerName - name of the timer
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
TimeTrigger::TimeTrigger (E_TriggerId id, CHAR* timerName) :
        Trigger (id) , Timer("TimeTrigger", 0, pdFALSE)
{

}   // end TimeTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This operation is invoked whenever the associated time trigger object
//      is enabled.  This method invokes the Timer::Reset() method of the 
//      base class Timer to start the associated timer as a 1 shot timer, 
//      using the time period contained in the TimePeriod attribute.  This
//      method then calls the base class EnableTrigger() method.
//
//    Input Parameters:
//      None
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
//      If anything but NU_SUCCESS is returned from the kernel service call
//      to Timer::Reset(), error handling is performed.
//
//    Requirements:  
//
/******************************************************************************/
void TimeTrigger::EnableTrigger (void)
{
    // Reset the timer for the TimePeriod.  Using 0 as the second argument
    // indicates that the timer only goes off 1 time (it's a 1 shot timer).
    // Passing TRUE as the last argument indicates that the timer should be
    // enabled now.   Perform error handling if the reset is not successful.
    Reset();
    // Invoke the base class method to perform common processing.
    Trigger::EnableTrigger ();

}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This operation is an overloaded version of the EnableTrigger() method.
//
//      This operation is invoked whenever the associated time trigger object
//      is enabled for a specified time other than that stored in the attribute
//      TimePeriod.  This method invokes the Timer::Reset() method of the 
//      base class Timer to start the associated timer as a 1 shot timer, 
//      using the time period passed in as an input parameter.  This
//      method then calls the base class EnableTrigger() method.
//
//    Input Parameters:
//      time - time duration of the timer in TICKS
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
//      If anything but NU_SUCCESS is returned from the kernel service call
//      to Timer::Reset(), error handling is performed.
//
//    Requirements:  
//
/******************************************************************************/
void TimeTrigger::EnableTrigger (UNSIGNED time)
{
    // Since the BD Task only processes time triggers once a cycle, modify
    // the time so that it is processed in the closest interval by subtracting
    // half of the cycle time.   For eg., if the cycle time is 20 ticks and the 
    // time is 104 ticks, the time is set to 94 ticks, which means that
    // it would be processed at time interval 100 ticks.  If left at 104, it 
    // would not be processed until 120 ticks.
    if (time >= BD_CYCLE_TIME_IN_TICKS)
    {
        time-= BD_CYCLE_TIME_IN_TICKS >> 1;
    }

    // Reset the timer. Using 0 as the second argument indicates that the timer 
    // only goes off 1 time (it's a 1 shot timer).  Passing TRUE as the last 
    // argument indicates that the timer should be enabled now.
    // Perform error handling if the reset is not successful.
    Reset();
    // Invoke the base class method to perform common processing.
    Trigger::EnableTrigger ();

}  // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DisableTrigger()
//
//    Processing: 
//      This operation is invoked whenever this time trigger is disabled.
//      This method invokes the Timer::Disable() method of the 
//      base class Timer to stop the timer and then calls the Trigger base
//      class DisableTrigger() operation.
//
//    Input Parameters:
//      None
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
//      If anything but NU_SUCCESS is returned from the kernel service call
//      to Timer::Disable(), error handling is performed.
//
//    Requirements:  
//
/******************************************************************************/
void TimeTrigger::DisableTrigger (void)
{
    // Disable the Timer.  If an error status is returned, perform error
    // handling.
    Stop();
    // Invoke Base Class Disable Method
    Trigger::DisableTrigger ();

}   // end DisableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PhaseInSettingChange()
//
//    Processing: 
//      This operation is invoked when a setting tied to the Time Trigger
//      changes.  This operation converts the input value from seconds to
//      ticks and stores it in the TimePeriod attribute.
//
//    Input Parameters:
//      value - new time value in seconds
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
void TimeTrigger::PhaseInSettingChange (LONG value)
{
    UNSIGNED timePeriod;

    // All times are in seconds * 100; need to convert to TICKS
    timePeriod = (value * 10) / MS_PER_TICK;

    // Update the time period.
    SetTimePeriod (timePeriod);

}   // end PhaseInSettingChange()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ExpirationRoutine()
//
//    Processing: 
//      This operation is the definition of the ExpirationRoutine() declared
//      as a pure virtual method in the RTOS Timer class.  This operation
//      is invoked by the RTOS whenever the associated timer expires.
//      This operation sets the TriggerDidFire attribute to TRUE so that
//      the BD Task can process the trigger associated with the timer the
//      next time it evaluates the enabled triggers.
//
//    Input Parameters:
//      None
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
//      Since this operation is invoked via the OS, it cannot make any 
//      function calls that could potentially cause suspension.
//
//    Requirements:  
//
/******************************************************************************/
void TimeTrigger::ExpirationRoutine (void)
{
    // The timer expired so the trigger should fire.  Set the TriggerDidFire
    // attribute to TRUE.  On the next BD cycle, the BD Task will process
    // this trigger.
    TriggerDidFire = eTrue;

}   // end ExpirationRoutine()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetTimePeriod()
//
//    Processing: 
//      This operation updates the TimePeriod and BackupTimePeriod attributes
//      based upon the input time.
//
//    Input Parameters:
//      time - new time to be used as the Time Period
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
void TimeTrigger::SetTimePeriod (UNSIGNED time)
{

    // Since the BD Task only processes time triggers once a cycle, modify
    // the time so that it is processed in the closest interval by subtracting
    // half of the cycle time.   For eg., if the cycle time is 20 ticks and the 
    // time is 104 ticks, the time is set to 94 ticks, which means that
    // it would be processed at time interval 100 ticks.  If left at 104, it 
    // would not be processed until 120 ticks.
    if (time >= BD_CYCLE_TIME_IN_TICKS)
    {
        time-= BD_CYCLE_TIME_IN_TICKS >> 1;
    }

    // Update both the TimePeriod and the BackupTimePeriod.  BackupTimePeriod
    // is a one's complement of the TimePeriod and is used to protect against
    // data corruption.
    TimePeriod = time;
    BackupTimePeriod = ~time;

}   // end SetTimePeriod()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetTimePeriod()
//
//    Processing: 
//      This operation compares the TimePeriod and BackupTimePeriod to check
//      if either attribute has been corrupted.  If not the TimePeriod is
//      returned to the caller.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      UNSIGNED - value of TimePeriod
//
//    Pre-Conditions:
//      Time Period is not corrupted.
//
//    Miscellaneous:
//      If the TimePeriod or BackupTimePeriod are not in agreement, error 
//      handling is performed.
//
//    Requirements:  
//
/******************************************************************************/
UNSIGNED TimeTrigger::GetTimePeriod (void)
{
    const UNSIGNED ALL_FF = ~0;

    // Verify that the values have not been corrupted.
    ASSERTION((TimePeriod ^ BackupTimePeriod) == ALL_FF);

    return (TimePeriod);

}   // GetTimePeriod()
