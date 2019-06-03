/******************************************************************************/
//$COMMON.CPP$
//   File Name:  LowBaselineAlarmTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Low Baseline Pressure Alarm 
//      Trigger.  The Low Baseline Pressure Alarm Trigger is used to trigger 
//      the Low PEEP alarm (in Pressure and Volume Ventilation) or the Low EPAP 
//      alarm (in Non-Invasive Ventilation) when the low pressure condition is 
//      detected during exhalation.
//
//   Interfaces:
//      Operations in this class interface with BdSetting objects to obtain
//      current setting values for the Low Baseline limit and the ventilation
//      type.  They also interface with the AlarmStat objects for the 
//      Low PEEP alarm and the Low EPAP alarm and with MgrBreathData to 
//      retrieve monitored pressure. 
//
//   Restrictions:
//      None
/******************************************************************************/
//#include "nucpp.h"
#include <ExhPhase.h>
#include <Util.h>
#include "LowBaselineAlarmTrigger.h"
#include "MgrBreathData.h"
#include "MgrBdSetting.h"
#include "AlarmTask.h"
/******************************************************************************/
//    CONSTANT Definitions
/******************************************************************************/
// Amount of time the Low Baseline Pressure condition must be met/ not met
// before the alarm is triggerred or reset.  
static const UNSIGNED MIN_LOP_DURATION = 1 * TICKS_PER_SEC;   // 1 sec

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: LowBaselineAlarmTrigger()
//
//    Processing: 
//      This operation is the constructor for the Patient Exhalation Pressure
//      Trigger.  It invokes the base class Trigger constructor with the 
//      Trigger ID.
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
LowBaselineAlarmTrigger::LowBaselineAlarmTrigger (void) : Trigger(eNoTrigger)
//        Trigger (eLowBaselineAlarmTrigger)
{

}   // end LowBaselineAlarmTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation is invoked every BD task cycle while the Low Baseline
//      Alarm Trigger is enabled.  This operation calls 
//      MgrBreathData::GetMonitoredPressure() to get the latest 
//      pressure reading.  This operation then compares the value to the
//      PressureLimit attribute.  The PressureLimit attribute contains the 
//      low baseline limit and was initialized in the EnableTrigger()
//      operation.  
//
//      If the monitored pressure is less than the pressure limit and has been
//      for MIN_LOP_DURATION, this operation calls AlarmStat::SetIsActive()
//      to annunciate the Low Peep alarm if conventional ventilation is active
//      or the Low EPAP alarm if Non-Invasive Ventilation is active.
//
//      If the alarm condition is not met, this operation resets the appropriate 
//      alarm if the condition has not been met for at least MIN_LOP_DURATION.
//
//      False is always returned to the caller since this trigger never fires.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      eFalse
//
//    Pre-Conditions:
//      This operation assumes that the TriggerMgr only invokes it while
//      the trigger is enabled.
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
E_Bool LowBaselineAlarmTrigger::CheckCondition (void)
{
    // Check if the monitored pressure is less than the pressure limit.
    // The PressureLimit is initialized in EnableTrigger().

     if ((MgrBreathData::S_GetInstance())->GetMonitoredPressure() < PressureLimit)
     {
         // Check if the condition was met on the previous cycle.
         if (eTrue == IsConditionMet)
         {
             // Since the condition has been met before, see how long the
             // condition has been met.  If this value exceeds the minimum
             // time criteria, annunciate the alarm.
             if (CalcElapsedTime(TimeConditionFirstMet)>= MIN_LOP_DURATION)
             {
                 AlarmTask::S_GetInstance()->SetIsActive(eLowPeep, eActive);
             }

         }
         else
         {
             // The condition was not met on the previous cycle, so set
             // the IsConditionMet flag and store the time stamp of when
             // the condition was met.
             IsConditionMet = eTrue;
//             TimeConditionFirstMet= SystemClock::Retrieve();
         }
     }
     else
     {
         // At least one sample did not meet the alarm criteria.
         IsCondMetEveryCycle = eFalse;

         // Check for auto-reset
         if(eActive == AlarmTask::S_GetInstance()->GetIsActive(eLowPeep))
         {
             // Check if the condition was not met on the previous cycle.
             // The alarm condition not being met is the same as the
             // auto-reset condition being met
             if (eFalse == IsConditionMet)
             {
                 // See how long the auto-reset condition has been present.
                 if (CalcElapsedTime(TimeConditionFirstMet) >= MIN_LOP_DURATION)
                 {
                     AlarmTask::S_GetInstance()->SetIsActive(eLowPeep, eNotActive);
                 }

             }
             else
             {
                 // The condition was not met on the previous cycle, so reset
                 // the IsConditionMet flag and store the time stamp of when
                 // the auto-reset condition was met.
                 IsConditionMet = eFalse;
//                 TimeConditionFirstMet= SystemClock::Retrieve();
             }
         }

         else
         {
             // Failed the pressure condition check.
             IsConditionMet = eFalse;
         }

     }

     // This trigger never returns eTrue to the caller since it does not cause
     // a mode or phase change.  Instead, this trigger handled its own trigger
     // by updating the alarm status.
     return(eFalse);

}   // end CheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This operation is invoked at the start of each exhalation phase to
//      enable the check for the Low Peep (Volume or Pressure Ventilation) or
//      the Low EPAP (Non-Invasive Ventilation).  This operation updates
//      the PressureLimit attribute with the operator set value for the
//      alarm limit.  It also retrieves the Ventilation Type currently in
//      effect.    It auto-resets the Low EPAP alarm if Volume or Pressure
//      Ventilation is active and it auto-resets the Low PEEP alarm if 
//      Non-Invasive Ventilation is active.
//
//      This routine also initializes IsConditionMet and TimeConditionFirstMet
//      and calls the base class Trigger::EnableTrigger() operation.
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
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void LowBaselineAlarmTrigger::EnableTrigger (void)
{
    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();

    PressureLimit = (SHORT) mgrBdSettingPtr->GetCurrentValue(eLowBaselineLimit);

    // What we want to do is put a dead band of 0.2 cm H2O around the Low PEEP
    // alarm when the alarm limit is set to zero (0).  This means that when the
    // Low PEEP alarm is set to zero we really want to use -0.2 cm H2O as the
    // level to trip the alarm rather than 0.0 cm H2O.  This change also effects
    // the Low EPAP alarm but it should never be a problem due to the lowest
    // EPAP limit is 3 cmH2O
    if (0 == PressureLimit)
        PressureLimit = -20;

    // Initialize IsConditionMet.
    IsConditionMet = eFalse;

    // Initialize TimeConditionFirstMet.  
//    TimeConditionFirstMet= SystemClock::Retrieve();

    // Assume the condition is met every cycle.  If at least one pressure
    // cycle does not meet the condition, this flag will be set to FALSE.
    // It is used when the exhalation time is less than MIN_LOP_DURATION
    // to reset the alarm if the condition is not met every cycle.
    IsCondMetEveryCycle = eTrue;

    // Invoke Base Class Enable
    Trigger::EnableTrigger();

}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DisableTrigger()
//
//    Processing: 
//      This operation is invoked at the end of each exhalation phase to
//      disable the check for the Low Peep (Volume or Pressure Ventilation) or
//      the Low EPAP (Non-Invasive Ventilation).  This operation retrieves 
//      the Ventilation Type currently in effect.  It auto-resets the Low EPAP 
//      alarm if Volume or Pressure Ventilation is active and it auto-resets 
//      the Low PEEP alarm if Non-Invasive Ventilation is active.
//
//      This routine also calls the base class Trigger::DisableTrigger() 
//      operation.
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
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void LowBaselineAlarmTrigger::DisableTrigger (void)
{
#ifdef HFO_SYSTEM
    if((ModeMgr::S_GetInstance()->GetCurrentModeId() >= eFirstHFOMode) &&
        (ModeMgr::S_GetInstance()->GetCurrentModeId() <= eLastHFOMode));
    else
#endif
    {
        // Check if the exhalation lasted less than MIN_LOP_DURATION.  If so,
        // the alarm is tripped if the condition is met every cycle.  Otherwise,
        // the alarm is reset.
        if (MIN_LOP_DURATION > ExhPhase::S_CalcTimeSincePhaseStart())
        {
            if (eTrue == IsCondMetEveryCycle)
            {
                AlarmTask::S_GetInstance()->SetIsActive(eLowPeep, eActive);
            }
            else
            {
                AlarmTask::S_GetInstance()->SetIsActive(eLowPeep, eNotActive);

            }
        }
    }

    Trigger::DisableTrigger();

}   // end DisableTrigger()

