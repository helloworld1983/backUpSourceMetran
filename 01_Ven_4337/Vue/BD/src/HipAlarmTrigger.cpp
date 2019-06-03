/******************************************************************************/
//$COMMON.CPP$
//   File Name:  HipAlarmTrigger.cpp
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
//
//   Restrictions:
//      None
/******************************************************************************/
#include <DeviceInterface.h>
#include "MgrBdSetting.h"
#include "HipAlarmTrigger.h"
#include "AlarmTask.h"
#include "DebugTrace.h"

HipAlarmTrigger* HipAlarmTrigger::S_Instance = NULL;
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the HipAlarmTrigger object, it invokes
//      HipAlarmTrigger::S_GetInstance(), which returns a pointer
//      to the HipAlarmTrigger object.
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
//      HipAlarmTrigger* - pointer to the instance of this class
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
HipAlarmTrigger* HipAlarmTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new HipAlarmTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end Hip2AlarmTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: HipAlarmTrigger()
//
//    Processing: 
//      This operation is the constructor for the HipAlarmTrigger.
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
HipAlarmTrigger::HipAlarmTrigger (E_TriggerId triggerId, SHORT alarmIx) :
        Trigger (triggerId), AlarmIx (alarmIx)
{

}   // end HipAlarmTrigger()

HipAlarmTrigger::HipAlarmTrigger() : Trigger()
{

}
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
E_Bool HipAlarmTrigger::CheckCondition (void)
{
    const LONG DELAY_50S = 15;
    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();

    LONG inhPressLimit;

    if (InactiveTimer > 0)
    {
        InactiveTimer--;
        return (eFalse);
    }

    inhPressLimit = (LONG) mgrBdSettingPtr->GetCurrentValue(eHipLimit);

    LONG InhLastPressSensor;
    LONG ExhLastPressSensor;

    DeviceInterface::S_GetInstance()->RequestDevice(eInhPressSensorGetLastReading, &InhLastPressSensor);
    DeviceInterface::S_GetInstance()->RequestDevice(eExhPressSensorGetLastReading, &ExhLastPressSensor);


    if ((InhLastPressSensor > inhPressLimit)||
            (ExhLastPressSensor > inhPressLimit))
    {
        hip1Count++;
        if(hip1Count >= DELAY_50S) //delay 50ms
        {
            hip1Count = 0;
            // If the pressure condition is met, set the TriggerDidFire attribute
            // and update the alarm status.
            TriggerDidFire = eTrue;

            if(eHigh != AlarmTask::S_GetInstance()->GetLevel(eHip))
            {
                AlarmTask::S_GetInstance()->SetIsActive(eHip, eActive);
            }
        }
    }
    else
    {
        hip1Count = 0;
    }

    return (TriggerDidFire);

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
void HipAlarmTrigger::DisableTrigger (void)
{
    // Must check IsEnabled because HIP2 alarm is always disabled at the 
    // start of exhalation (end of inhalation), but it is not always enabled.
    if((IsEnabled == eTrue) && (TriggerDidFire == eFalse))
    {
        // If alarm was active, it no longer is
        AlarmTask::S_GetInstance()->SetIsActive(AlarmIx, eNotActive);
    }

    // Invoke Base Class Disable Method
    Trigger::DisableTrigger ();

}   // end DisableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetInactiveTimer (USHORT value)
//
//    Processing:
//      set inactive timer
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
void HipAlarmTrigger::SetInactiveTimer (USHORT value)
{
    InactiveTimer = value;
}
