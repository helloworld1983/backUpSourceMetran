/******************************************************************************/
//$COMMON.CPP$
//   File Name:  ApneaAlarmTimeTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Apnea Alarm Time Trigger.
//      The Apnea Alarm Time Trigger is used to trigger the apnea alarm 
//      condition and response when the apnea interval expires.
//
//   Interfaces:
//      Operations in this class invoke methods from the base class TimeTrigger.
//      It also invokes the SetIsActive operation in the AlarmStat class to 
//      report the alarm condition to the Alarms Task.  And, several of the
//      Timer kernel services are called (Disable(), Reset()).
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
#include "ApneaAlarmTimeTrigger.h"
#include "AlarmTask.h"
#include "Tasks.h"
#include "DebugTrace.h"

// Define static attributes.
ApneaAlarmTimeTrigger* ApneaAlarmTimeTrigger::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the ApneaAlarmTimeTrigger object, it invokes
//      ApneaAlarmTimeTrigger::S_GetInstance(), which returns a pointer to
//      the ApneaAlarmTimeTrigger object.
//
//      If the object has not already been created, this method instantiates
//      it and returns a pointer to the object.  If the object has already been
//      instantiated, a pointer to the object is returned.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      ApneaAlarmTimeTrigger* - pointer to the instance of this class
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
ApneaAlarmTimeTrigger* ApneaAlarmTimeTrigger::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new ApneaAlarmTimeTrigger ();
        ASSERTION(S_Instance != NULL);

    }

    return (S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation is invoked every BD Cycle Time while the Apnea Alarm
//      Time Trigger is enabled.  This method checks if the TriggerDidFire
//      attribute has been set to TRUE.  If so, AlarmStat::SetIsActive()
//      is called to report the alarm condition to the Alarms Task.  The
//      value of TriggerDidFire is returned to the caller.  Since the
//      ApneaAlarmTimeTrigger is derived from the TimeTrigger class, the 
//      TriggerDidFire attribute is set to TRUE whenever the timer associated
//      with the trigger fires.  The timer associated with this trigger is
//      the Apnea Interval Timer.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool -- indicates whether or not the apnea interval elapsed
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
E_Bool ApneaAlarmTimeTrigger::CheckCondition (void)
{
    // If the timer has already expired, the TriggerDidFire attribute has been
    // set to TRUE.
    if(eTrue == TriggerDidFire)
    {
        // Update alarm status
        AlarmTask::S_GetInstance ()->SetIsActive (eApnea, eActive);
    }

    return (TriggerDidFire);

}   // end CheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ReEnableTrigger()
//
//    Processing: 
//      This operation is invoked at the start of each inhalation.  If the
//      trigger is enabled (IsEnabled attribute is set), the Timer::Disable()
//      operation is invoked to stop the apnea interval timer.  EnableTrigger()
//      is then called to re-enable the trigger.  Since the apnea alarm is
//      meant to detect the absence of breathing, as long as an inhalation is
//      detected, the apnea timer is restarted, therefore holding off apnea
//      alarm detection.
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
//      None
//
//    Requirements:  
//
/******************************************************************************/
void ApneaAlarmTimeTrigger::ReEnableTrigger (void)
{
    // Only re-enable it if it is enabled.
    if(eTrue == IsEnabled)
    {
        // Disable the apnea interval timer
        Stop();

        // Set the IsEnabled flag to false so that EnableTrigger re-enables
        // the trigger.
        IsEnabled = eFalse;

        // Re-enable the trigger.  This operation will also restart the timer.
        EnableTrigger ();

    }
}   // end ReEnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This operation is invoked from ReEnableTrigger() and from the
//      Enter() operations of the breathing modes. 
//
//      If the trigger is not already enabled, this operations starts the 
//      Apnea Interval Timer and invokes the base class operation 
//      Trigger::EnableTrigger().  It is possible for this operation to be
//      called while it is already enabled (e.g. when the mode changes from
//      A/C to Spont mode).  In this case, the timer is left alone so that
//      it continues to run across the mode change boundaries.
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
//      If anything but NU_SUCCESS is returned from Timer::Reset(), error
//      handling is performed.
//
//    Requirements:  
//
/******************************************************************************/
void ApneaAlarmTimeTrigger::EnableTrigger (void)
{
    // It is possible for this method to be called when the
    // trigger is already enabled (e.g. when changing modes Ac -> Cpap)
    // If it is already enabled, leave the timer alone since the timer should
    // run across mode change boundaries.
    if(eFalse == IsEnabled)
    {

        // Restart the timer for the Apnea Interval time period, which is
        // stored in the TimePeriod attribute.   Since this timer is a one-shot
        // timer, 0 is passed as the reschedule time.
        Reset();

        // Perform error handling if return status is not successful.

        // Invoke base class enable method which updates the IsEnabled and 
        // TriggerDidFire flags
        Trigger::EnableTrigger ();
    }

}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PhaseInSettingChange()
//
//    Processing: 
//      This operation is invoked when the Apnea Interval setting changes.
//      This operation adds 100ms to the value and converts if from seconds to
//      ticks and stores it in the TimePeriod attribute.  The 100 ms is added
//      to avoid a potential race condition if the breath period is exactly
//      the same as the apnea interval.
//
//    Input Parameters:
//      value - new time value in seconds * SCALE
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
void ApneaAlarmTimeTrigger::PhaseInSettingChange (LONG value)
{
    // To avoid a race condition if the apnea interval is exactly equal to 
    // the breath period, the apnea interval is offset by two times the BD cycle
    const UNSIGNED APNEA_INT_OFFSET_IN_TICKS = (BD_CYCLE_TIME_IN_TICKS << 1);

    UNSIGNED timePeriod;    // new time period in TICKS

    // The Apnea interval is in seconds * SCALE.  Need to convert to TICKS.
    // Also, offset the value.
    timePeriod = ((value * TICKS_PER_SEC) / SCALE) + APNEA_INT_OFFSET_IN_TICKS;

    // Update the time period; it will be used when the Apnea Timer is enabled
    // at the start of the next inhalation.  Apnea Interval changes are not 
    // phased in right away.
    SetPeriod(timePeriod);

}   // end PhaseInSettingChange()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ApneaAlarmTimeTrigger()
//
//    Processing: 
//      This operation is the constructor for the ApneaAlarmTimeTrigger.
//      This operation invokes the TimeTrigger base class constructor.
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
//      None
//
//    Requirements:  
//
/******************************************************************************/
ApneaAlarmTimeTrigger::ApneaAlarmTimeTrigger (void) :
        TimeTrigger (eApneaAlarmTimeTrigger, (CHAR*)"AVINTTMR")
{

}   // end ApneaAlarmTimeTrigger()

