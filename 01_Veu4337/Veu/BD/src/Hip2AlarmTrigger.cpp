/******************************************************************************/
//$COMMON.CPP$
//   File Name:  Hip2AlarmTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the HIP2 Alarm Trigger.
//      The HIP-2 Alarm Trigger is derived from the HIP Alarm Trigger and
//      is very similar to that trigger.  The difference is that the HIP-2
//      Alarm Trigger never returns TRUE from its CheckCondition() method
//      and therefore does not truncate the inhalation.   It is used in 
//      conjunction with the HIP Alarm Trigger to detect the HIP condition 
//      during Mixed Ventilation Modes.  The breath is still truncated by
//      the HIP-1 alarm.
//
//   Interfaces:
//      The CheckCondition() operation retrieves the current pressure measured 
//      in the exhalation limb by calling MgrBreathData::GetMonitoredPressure()
//      and it retrieves the operator setting for HIP Limit by calling
//      MgrBdSetting::GetCurrentValue().
//
//   Restrictions:
//      None
/******************************************************************************/
#include <DeviceInterface.h>
#include "MgrBdSetting.h"
#include "Hip2AlarmTrigger.h"

#include "AlarmInterface.h"
#include "AlarmTask.h"
#include "DebugTrace.h"

#include "HighPressMode.h"

// Define statics
Hip2AlarmTrigger* Hip2AlarmTrigger::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Hip2AlarmTrigger()
//
//    Processing:
//      This operation is the constructor for the Hip2AlarmTrigger.
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
Hip2AlarmTrigger::Hip2AlarmTrigger()
    : Trigger(eHip2AlarmTrigger)
{
    HighPressAlarm  = 20 * SCALE;
    InactiveTimer = 0;

}   // end Hip2AlarmTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the Hip2AlarmTrigger object, it invokes
//      Hip2AlarmTrigger::S_GetInstance(), which returns a pointer
//      to the Hip2AlarmTrigger object.
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
//      Hip2AlarmTrigger* - pointer to the instance of this class
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
Hip2AlarmTrigger* Hip2AlarmTrigger::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new Hip2AlarmTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end Hip2AlarmTrigger()

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
void Hip2AlarmTrigger::DisableTrigger(void)
{
    // Must check IsEnabled because HIP2 alarm is always disabled at the
    // start of exhalation (end of inhalation), but it is not always enabled.
    if ((IsEnabled == eTrue) && (TriggerDidFire == eFalse))
    {
        // If alarm was active, it no longer is
        AlarmTask::S_GetInstance()->SetIsActive(eExtremeHip, eNotActive);
    }

    // Invoke Base Class Disable Method
    Trigger::DisableTrigger();

}   // end DisableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation retrieves the current pressure measured in the exhalation
//      limb by calling MgrBreathData::GetMonitoredPressure().  It then gets
//      the operator setting for HIP Limit via MgrBdSetting::GetCurrentValue().
//      if the exhalation pressure exceeds the HIP Limit, the TriggerDidFire
//      attribute is set to True and alarm status is updated.
//
//      FALSE is always returned to the caller.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the trigger fired or not; always FALSE
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
E_Bool Hip2AlarmTrigger::CheckCondition (void)
{
    static const USHORT ABOVE15_PRESS_TIME = 15;
    static LONG hip2Count = 0;

    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();
    LONG ExtremeHighInsp_Offset =  mgrBdSettingPtr->GetCurrentValue(eExtremeHighInsp_Autoset);

    if (InactiveTimer > 0)
    {
        InactiveTimer--;
        return (eFalse);
    }

    HighPressAlarm = mgrBdSettingPtr->GetCurrentValue(eHipLimit);

    LONG InhLastPressSensor;
    LONG ExhLastPressSensor;

    DeviceInterface::S_GetInstance()->RequestDevice(eInhPressSensorGetLastReading, &InhLastPressSensor);
    DeviceInterface::S_GetInstance()->RequestDevice(eExhPressSensorGetLastReading, &ExhLastPressSensor);



    if ((InhLastPressSensor > HighPressAlarm + ExtremeHighInsp_Offset)||
        (ExhLastPressSensor
                > HighPressAlarm + ExtremeHighInsp_Offset))
    {
        hip2Count++;
        if(hip2Count >= ABOVE15_PRESS_TIME)
        {
            hip2Count = 0;
            // If the pressure condition is met, set the TriggerDidFire attribute
            // and update the alarm status.

            HighPressMode::S_GetInstance()->SetCauseOfAlarm(eExtremeHip);
            AlarmTask::S_GetInstance()->SetIsActive(eExtremeHip, eActive);
            TriggerDidFire = eTrue;
        }
    }
    else
    {
        hip2Count = 0;
        TriggerDidFire = eFalse;

    }
    return (TriggerDidFire);

}   // end CheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger(Mode* mode)
//
//    Processing: 
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
void Hip2AlarmTrigger::ProcessTrigger(Mode* mode)
{
    // Invoke the operation to handle the HIP trigger in the current mode.
    mode->ProcessTriggerID(eHip2AlarmTrigger);
}   // end ProcessTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetHighPressAlarm()
//
//    Processing: Get high pressure alarm
//
//    Input Parameters:
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
LONG Hip2AlarmTrigger::GetHighPressAlarm (void)
{
    return HighPressAlarm;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetInactiveTimer (USHORT value)
//
//    Processing: set inactive timer value
//
//    Input Parameters: inactive timer value
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
void Hip2AlarmTrigger::SetInactiveTimer (USHORT value)
{
    InactiveTimer = value;
}
