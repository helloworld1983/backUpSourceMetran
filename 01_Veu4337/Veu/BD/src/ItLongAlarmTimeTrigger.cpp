/******************************************************************************/
    //$COMMON.CPP$
//   File Name:  ItLongAlarmTimeTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the implementation of the Inhalation Time Too
//      Long Alarm Time Trigger.  The ItLongAlarmTimeTrigger class is derived
//      from the TimeTrigger class.
//
//   Interfaces:
//      This operation invokes operations in its parent class TimeTrigger.
//      It also interfaces with the Inhalation Too Long Alarm Status to 
//      update the alarm status.
//
//   Restrictions:
//      None
/******************************************************************************/
#include "ItLongAlarmTimeTrigger.h"
#include "TriggerMgr.h"
#include "MgrBdSetting.h"
#include "AlarmTask.h"
#include "tasks.h"

#include "DebugTrace.h"

// IT_LONG_TIME_PERIOD_IN_MS is the maximum allowed duration of a spontaneous
// or supported inhalation.  This value is fixed; it is not user settable like
// other time triggers.  The value is fixed at 3.5 seconds.
const UNSIGNED IT_LONG_TIME_PERIOD_IN_TICKS = 3500 / MS_PER_TICK;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ItLongAlarmTimeTrigger()
//
//    Processing: 
//      This operation is the constructor for the Inhalation Time Too Long
//      Alarm Time Trigger.  It initializes the TimePeriod attribute and
//      invokes the TimeTrigger constructor.
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
ItLongAlarmTimeTrigger::ItLongAlarmTimeTrigger (void) :
        TimeTrigger (eItLongAlarmTimeTrigger,(CHAR*) "ITLNGTMR")
{

    // Store the Time Period in System Clock Ticks.
    SetTimePeriod (IT_LONG_TIME_PERIOD_IN_TICKS);

}   // end ItLongAlarmTimeTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This operation is invoked at the start of a spontaneous or supported
//      inhalation to enable the Inhalation Time Too Long Alarm Trigger.
//      This operation retrieves the current mode setting and breath period.
//      If the mode is Mixed and the breath period is shorter than the 
//      default IT_LONG_TIME_PERIOD_IN_TICKS, the breath period is used as
//      the timeout value.  Otherwise, the default value is used.  In either
//      case the timer is started as a one shot timer and the trigger is put
//      into the enabled state.
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
void ItLongAlarmTimeTrigger::EnableTrigger (void)
{
    STATUS status;              // status of RTOS calls
    E_VentilationMode mode;     // current mode
    UNSIGNED breathPeriod;      // BP based on current Resp Rate setting

    mode = (E_VentilationMode)(MgrBdSetting::S_GetInstance())->GetCurrentValue(eMode);
    breathPeriod = (TriggerMgr::S_GetInstance())->GetTimePeriod(eBpTimeTrigger);

    if ((eMixedMode == mode) &&
            (breathPeriod < IT_LONG_TIME_PERIOD_IN_TICKS))
    {
        // Reset the timer for one BD cycle less than the Breath Period.  Using
        // 1 BD cycle less avoids a potential race condition where both triggers
        // fire at about the same time but the BP trigger gets processed first.
        // Using 0 as the second argument indicates that the timer only goes off
        // 1 time (it's a 1 shot timer).  Passing TRUE as the last argument
        // indicates that the timer should be enabled now.
        // Perform error handling if the reset is not successful.
        Reset(breathPeriod - BD_CYCLE_TIME_IN_TICKS);
        ASSERTION(status == NU_SUCCESS);

        // Invoke the base class method to perform common processing.
        Trigger::EnableTrigger();
    }
    else
    {

        // Use the default time period.
        TimeTrigger::EnableTrigger();
    }

}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PhaseInSettingChange()
//
//    Processing: 
//      This operation was created to override the PhaseInSettingChange()
//      operation of the parent class TimeTrigger.  Most TimeTriggers
//      are associated with an operator setting.  When the setting changes
//      the PhaseInSettingChange() operation is called to update the 
//      TimePeriod attribute.  The Inhalation Time Too Long time is fixed
//      and not associated with a setting.  Therefore, this operation
//      should never be called for this trigger.  This operation simply
//      performs error handling.
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
void ItLongAlarmTimeTrigger::PhaseInSettingChange (LONG)
{

    // This operation should never be called; just assert.
    ASSERTION(eFalse);

}   // end PhaseInSettingChange()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation is invoked every BD Cycle Time while the trigger is
//      enabled.  This operation checks if the IT Too Long Timer expired by
//      checking the TriggerDidFire attribute.  If TriggerDidFire is TRUE,
//      this operation invokes AlarmStat::SetIsActive() to update the alarm
//      status.  The value of TriggerDidFire is returned to the caller.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the trigger fired or not
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
E_Bool ItLongAlarmTimeTrigger::CheckCondition (void)
{

    // If the TriggerDidFire attribute is TRUE, it indicates that the timer
    // expired, in which case the alarm status is updated to active.
    if(eTrue == TriggerDidFire)
    {
        AlarmTask::S_GetInstance()->SetIsActive(eITimeTooLong, eActive);
    }

    return (TriggerDidFire);

}   // end CheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DisableTrigger()
//
//    Processing: 
//      This operation is invoked at the end of a spontaneous or supported
//      inhalation to disable the Inhalation Too Long Alarm Time Trigger.
//      This operation invokes the Timer::Disable() operation to stop the
//      IT Too Long timer.  It then checks the TriggerDidFire attribute.
//      If this flag is FALSE, the inhalation is ending due to the occurrence
//      of another trigger  -- i.e. the inhalation is not being terminated
//      due to the IT Too Long Alarm condition.  One complete inhalation 
//      delivered without tripping the alarm will cause the alarm to auto-reset.
//      AlarmStat::SetIsActive() is called to update the alarm status.
//  
//      Finally, this operation invokes the base class Trigger::DisableTrigger()
//      operation to perform some processing common to disabling all triggers.
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
void ItLongAlarmTimeTrigger::DisableTrigger (void)
{
    // Disable the Timer; this invokes the RTOS Timer::Disable() method.
    Stop();

    if(eFalse == TriggerDidFire)
    {
        // Update Alarm Status
        AlarmTask::S_GetInstance()->SetIsActive(eITimeTooLong, eNotActive);
    }

    // Invoke base class disable method
    Trigger::DisableTrigger ();

}   // end DisableTrigger()

