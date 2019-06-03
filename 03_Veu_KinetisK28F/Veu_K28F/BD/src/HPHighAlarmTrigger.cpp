/******************************************************************************/
//$COMMON.CPP$
//   File Name:  HPHighAlarmTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:
//      This file contains the definition of the HIP Alarm Trigger.
//      The HIP Alarm Trigger is used to trigger the High Inhalation Pressure
//      alarm condition and response when the pressure condition is detected.
//
//   Interfaces:
//      The CheckCondition() operation retrieves the current pressure measured
//      in the exhalation limb by calling MgrBreathData::GetMonitoredPressure()
//      and it retrieves the operator setting for HIP Limit by calling
//      MgrBdSetting::GetCurrentValue().
//
//      Operations in this class update HIP alarm status by invoking the
//      AlarmStat::SetIsActive() operation.
/******************************************************************************/
#include "HPHighAlarmTrigger.h"
#include "MgrBdSetting.h"
#include "HPHighAlarmTrigger.h"
#include "MgrBreathData.h"
#include "PressureSensor.h"
#include "AlarmTask.h"

#define DELAY_TIME_50MS 25

HPHighAlarmTrigger* HPHighAlarmTrigger::S_Instance = NULL;
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: HPHighAlarmTrigger()
//
//    Processing:
//      This operation is the constructor for the HPHighAlarmTrigger.
//      This operation invokes the Trigger base class constructor.
//
//    Input Parameters:
//      triggerId - Trigger ID
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
HPHighAlarmTrigger::HPHighAlarmTrigger():Trigger(eHPHighAlarmTrigger)
{
    highAlarmDelay = 0;
    highAlarmClearDelay = 0;

    lowAlarmDelay = 0;
    lowAlarmClearDelay = 0;

    InactiveTimer =0;

    alarmFlag = false;
}   // end HPHighAlarmTrigger()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing:
//      This operation retrieves the current pressure measured in the exhalation
//      limb by calling MgrBreathData::GetMonitoredPressure().  It then gets
//      the operator setting for HIP Limit via MgrBdSetting::GetCurrentValue().
//      if the exhalation pressure exceeds the HIP Limit, the TriggerDidFire
//      attribute is set to True and AlarmStat::SetIsActive() is called
//      to update the HIP alarm status.
//
//      The value of TriggerDidFire is always returned to the caller.
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
//      This operation assumes that the TriggerMgr only invokes it while
//      the trigger is enabled.
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
E_Bool HPHighAlarmTrigger::CheckCondition(void)
{
    static const USHORT ABOVE15_PRESS_TIME = 15;
    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();

    LONG highPressLimit = (LONG) mgrBdSettingPtr->GetCurrentValue(eAPRVHighPHighLimit);;
//    LONG lowPressLimit = (LONG) mgrBdSettingPtr->GetCurrentValue(eAPRVLowPHighLimit);;

    if (InactiveTimer > 0)
    {
        InactiveTimer--;
        return (eFalse);
    }

    //   Check High Press Alarm
    if ((PressureSensor::InhPressSensor->GetLastReading() > highPressLimit)||
            (PressureSensor::ExhPressSensor->GetLastReading() > highPressLimit))
    {

        highAlarmDelay++;

        highAlarmClearDelay = 0;

        if(highAlarmDelay >= ABOVE15_PRESS_TIME) //
        {

            highAlarmDelay = 0;
            alarmFlag = true;
            // If the pressure condition is met, set the TriggerDidFire attribute
            // and update the alarm status.
            TriggerDidFire = eTrue;

            AlarmTask::S_GetInstance()->SetIsActive(eAPRVHigh_PHigh, eActive);
            return TriggerDidFire;
        }
    }

    return (eFalse);
}   // end CheckCondition()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DisableTrigger()
//
//    Processing:
//      This operation is invoked at the end of every inhalation to disable
//      the HIP Alarm Trigger.  If the IsEnabled flag is TRUE and the
//      TriggerDidFire attribute is FALSE, the inhalation is ending due to
//      the occurrence of another trigger  -- i.e. the inhalation is not being
//      terminated due to this HIP condition.  One complete inhalation
//      delivered without tripping the alarm will cause the alarm to auto-reset.
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
void HPHighAlarmTrigger::DisableTrigger(void)
{
    highAlarmDelay = 0;
    highAlarmClearDelay = 0;

    lowAlarmDelay = 0;
    lowAlarmClearDelay = 0;
    // Must check IsEnabled because HIP2 alarm is always disabled at the
    // start of exhalation (end of inhalation), but it is not always enabled.
    if ((IsEnabled == eTrue) && (TriggerDidFire == eFalse))
    {
    }
    TriggerDidFire = eFalse;

    // Invoke Base Class Disable Method
    Trigger::DisableTrigger();

}   // end DisableTrigger()

