/******************************************************************************/
//$COMMON.H$
//   File Name:  AutoSIStartTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the AutoSIStartTrigger class
/******************************************************************************/
#include "AutoSIStartTrigger.h"
#include "AutoSIStopTrigger.h"
#include "MgrBdSetting.h"

AutoSIStartTrigger* AutoSIStartTrigger::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AutoSIStartTrigger()
//
//    Processing: Constructor for class
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
//
//    Requirements:
//
/******************************************************************************/
AutoSIStartTrigger::AutoSIStartTrigger()
: TimeTrigger(eAutoSIOnTimeTrigger,(CHAR*)"AutoSI")
{
    currentTime = 0;
    currentSIRate = 1*SCALE;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the object, it invokes S_GetInstance(),
//      which returns a pointer to the object.
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
//      * - pointer to the object instantiated from this class
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
AutoSIStartTrigger* AutoSIStartTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new AutoSIStartTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: Enable trigger
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
//
//    Requirements:
//
/******************************************************************************/
void AutoSIStartTrigger::EnableTrigger(void)
{
    if(currentTime == 0)
        return;
    //    STATUS status;

    // It is possible for this method to be called when the
    // trigger is already enabled (e.g. when changing modes Ac -> Cpap)
    // If it is already enabled, leave the timer alone since the timer should
    // run across mode change boundaries.
    if (eFalse == IsEnabled)
    {

        // Restart the timer for the Apnea Interval time period, which is
        // stored in the TimePeriod attribute.   Since this timer is a one-shot
        // timer, 0 is passed as the reschedule time.
        //        status = Reset(GetTimePeriod(), 0, TRUE);
        Reset();

        // Invoke base class enable method which updates the IsEnabled and
        // TriggerDidFire flags
        Trigger::EnableTrigger();
    }
}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: Check condition and process action
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
//
//    Requirements:
//
/******************************************************************************/
E_Bool AutoSIStartTrigger::CheckCondition(void)
{
    E_Bool rtnValue = TriggerDidFire;
    if(TriggerDidFire)
    {
        TriggerDidFire = eFalse;
        ReEnableTrigger();
    }

    return (rtnValue);

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ReEnableTrigger()
//
//    Processing: Re-enable trigger
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
//
//    Requirements:
//
/******************************************************************************/
void AutoSIStartTrigger::ReEnableTrigger(void)
{
    // Only re-enable it if it is enabled.
    if (eTrue == IsEnabled)
    {
        //Disable timer
        Stop();
        IsEnabled = eFalse;
        EnableTrigger();

    }
}   // end ReEnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PhaseInSettingChange(LONG value)
//
//    Processing: Process phase when setting change
//
//    Input Parameters:
//      LONG value
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//
//    Requirements:
//
/******************************************************************************/
void AutoSIStartTrigger::PhaseInSettingChange(LONG value)
{
    static const UNSIGNED BD_CYCLE_TIME_IN_TICKS = 2;

    LONG settingSIRate = MgrBdSetting::S_GetInstance()->GetCurrentValue(eSIRate);

    if(value == 0)
    {
        DisableTrigger();
        currentTime = value;

    }
    else if((currentTime != value)||(currentSIRate != settingSIRate))
    {
        //        DisableTrigger();
        // To avoid a race condition if the apnea interval is exactly equal to
        // the breath period, the apnea interval is offset by two times the BD cycle
        const UNSIGNED APNEA_INT_OFFSET_IN_TICKS =
                (BD_CYCLE_TIME_IN_TICKS << 1);

        UNSIGNED timePeriod;    // new time period in TICKS

        // The Apnea interval is in seconds * SCALE.  Need to convert to TICKS.
        // Also, offset the value.
        timePeriod = ((value * TICKS_PER_SEC) / SCALE) + APNEA_INT_OFFSET_IN_TICKS;

        // Update the time period; it will be used when the Apnea Timer is enabled
        // at the start of the next inhalation.  Apnea Interval changes are not
        // phased in right away.
        SetTimePeriod(timePeriod);
        EnableTrigger();
        currentTime = value;
        currentSIRate = settingSIRate;


        //calculate SI Time Off
        //        LONG SIrate = MgrBdSetting::S_GetInstance()->GetCurrentValue(eSIRate);
        if(settingSIRate != 0)
        {
            LONG timeoff = (3600*SCALE/settingSIRate)*SCALE - value;
            AutoSIStopTrigger::S_GetInstance()->PhaseInSettingChange(timeoff);

        }
    }
}
