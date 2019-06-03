/******************************************************************************/
//$COMMON.CPP$
//   File Name:  CyclicTimeTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file defines a Cyclic Time Trigger class, which is a class derived
//      from the TimeTrigger base class. A CyclicTimeTrigger provides a means 
//      to implement a cyclic timer associated with a triggering condition. 
//
//   Interfaces:
//      This class is derived from the TimeTrigger class.  Operations in this
//      class invoke methods defined in the base class.
//
//   Restrictions:
//      None
/******************************************************************************/
#include "CyclicTimeTrigger.h"
#include "DebugTrace.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CyclicTimeTrigger()
//
//    Processing: 
//      This operation is the constructor for objects of the CyclicTimeTrigger 
//      class. This operation invokes the constructor for the TimeTrigger 
//      class from which this class is derived.  
//
//    Input Parameters:
//      id - trigger identifier
//      timerName - name of the timer
//      timePeriod - time duration for the timer
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
CyclicTimeTrigger::CyclicTimeTrigger (E_TriggerId id, CHAR* timerName,
                                      UNSIGNED timePeriod) :
        TimeTrigger (id, timerName)
{

    SetTimePeriod (timePeriod);

}   // end CyclicTimeTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This operation is invoked whenever the associated time trigger object
//      is enabled.  This method invokes the Timer::Reset() method of the 
//      base class Timer to start the associated timer as a cyclic timer, 
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
void CyclicTimeTrigger::EnableTrigger (void)
{
//    STATUS status;      // status of RTOS calls
//    UNSIGNED timePeriod = GetTimePeriod();

    // Reset the timer for the TimePeriod.  This is a cyclic timer.
    // Passing TRUE as the last argument indicates that the timer should be
    // enabled now.   Perform error handling if the reset is not successful.
//    status = Reset(timePeriod, timePeriod, TRUE);
//    ASSERTION(status == NU_SUCCESS);
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
//      base class Timer to start the associated timer with the passed time
//      period as the initial time.  This method then calls the base class 
//      EnableTrigger() method.
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
void CyclicTimeTrigger::EnableTrigger (UNSIGNED time)
{
//    STATUS status;      // status of RTOS calls

    // Reset the timer using the time value just calculated for the
    // initial time period.  The second argument indicates the time value for
    // subsequent timeout periods.  Passing TRUE as the last argument 
    // indicates that the timer should be enabled now.
    // Perform error handling if the reset is not successful.
//    status = Reset(time, GetTimePeriod(), TRUE);
//    ASSERTION(status == NU_SUCCESS);
    Reset();

    // Invoke the base class method to perform common processing.
    Trigger::EnableTrigger ();

}  // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//  Operation Name: CheckCondition()
//
//  Processing: 
//      This operations returns the value of the TriggerDidFire attribute
//      and resets the TriggerDidFire attribute to FALSE.
//
//  Input Parameters:
//      None
//
//  Output Parameters:
//      None
//
//  Return Values:
//      eTrue if the trigger did fire; eFalse otherwise
//
//  Pre-Conditions:
//      None
//
//  Miscellaneous:
//      None
//
//  Requirements:    
//
/******************************************************************************/
E_Bool CyclicTimeTrigger::CheckCondition (void)
{
    E_Bool rtnValue = TriggerDidFire;

    // If the trigger fired, reset it.    
    if(eTrue == rtnValue)
    {
        TriggerDidFire = eFalse;
    }

    return (rtnValue);

}   // end CheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PhaseInSettingChange()
//
//    Processing: 
//      This operation was created to override the PhaseInSettingChange()
//      operation of the parent class CyclicTimeTrigger.  Most CyclicTimeTriggers
//      are associated with an operator setting.  When the setting changes
//      the PhaseInSettingChange() operation is called to update the 
//      TimePeriod attribute.  Cyclic timers are fixed and not associated 
//      with a setting.  Therefore, this operation should never be called 
//      for this trigger.  This operation simply performs error handling.
//
//    Input Parameters:
//      not used; included to conform to function prototype of base class
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
void CyclicTimeTrigger::PhaseInSettingChange (LONG)
{
    ASSERTION(eFalse);

}   // end PhaseInSettingChange()

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
void CyclicTimeTrigger::SetTimePeriod (UNSIGNED time)
{

    // Update both the TimePeriod and the BackupTimePeriod.  BackupTimePeriod
    // is a one's complement of the TimePeriod and is used to protect against
    // data corruption.
    TimePeriod = time;
    BackupTimePeriod = ~time;

}   // end SetTimePeriod()

