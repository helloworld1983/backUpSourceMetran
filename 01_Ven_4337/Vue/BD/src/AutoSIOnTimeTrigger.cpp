/******************************************************************************/
//$COMMON.H$
//   File Name:  AutoSIOnTimeTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the AutoSIOnTimeTrigger class
/******************************************************************************/
#include "AutoSIOnTimeTrigger.h"
#include "AutoSIOffTimeTrigger.h"
#include "MgrBdSetting.h"

#define SECONDS_PER_MINUTE 3600
AutoSIOnTimeTrigger* AutoSIOnTimeTrigger::S_Instance = NULL;

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
AutoSIOnTimeTrigger* AutoSIOnTimeTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new AutoSIOnTimeTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AutoSIOnTimeTrigger()
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
AutoSIOnTimeTrigger::AutoSIOnTimeTrigger()
: TimeTrigger(eAutoSIOnTimeTrigger,(CHAR*)"AutoSI")
{
    currentTime = 0;
    currentSIRate = 1*SCALE;

    currentSITimeData = 0;
    one_hundred_ms = 0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing:
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void AutoSIOnTimeTrigger::EnableTrigger(void)
{
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

        currentSITimeData = 0;
        one_hundred_ms = 0;
    }
}   // end EnableTrigger()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing:
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
E_Bool AutoSIOnTimeTrigger::CheckCondition(void)
{
    E_Bool rtnValue = TriggerDidFire;

    UpdateSITime();

    if(TriggerDidFire)
    {
        TriggerDidFire = eFalse;
    }

    return (rtnValue);

}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ReEnableTrigger()
//
//    Processing:
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void AutoSIOnTimeTrigger::ReEnableTrigger(void)
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
//    Operation Name: PhaseInSettingChange()
//
//    Processing: invoked when a setting tied to the Time Trigger change
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void AutoSIOnTimeTrigger::PhaseInSettingChange(LONG value)
{
    const UNSIGNED BD_CYCLE_TIME_IN_TICKS = 2;

    LONG settingSIRate = MgrBdSetting::S_GetInstance()->GetCurrentValue(eSIRate);

    if(value == 0)
    {
        currentTime = value;
        AutoSIOffTimeTrigger::S_GetInstance()->PhaseInSettingChange(0);
    }
    else if((currentTime != value)||(currentSIRate != settingSIRate))
    {
        // To avoid a race condition if the apnea interval is exactly equal to
        // the breath period, the apnea interval is offset by two times the BD cycle
        const UNSIGNED OFFSET_IN_TICKS =
                (BD_CYCLE_TIME_IN_TICKS << 1);

        UNSIGNED timePeriod;    // new time period in TICKS

        // The Apnea interval is in seconds * SCALE.  Need to convert to TICKS.
        // Also, offset the value.
        timePeriod = ((value * TICKS_PER_SEC) / SCALE) + OFFSET_IN_TICKS;

        // Update the time period; it will be used when the Apnea Timer is enabled
        // at the start of the next inhalation.  Apnea Interval changes are not
        // phased in right away.
        SetTimePeriod(timePeriod);
        currentTime = value;
        currentSIRate = settingSIRate;

        if(settingSIRate != 0)
        {
            LONG timeoff = (SECONDS_PER_MINUTE*SCALE/settingSIRate)*SCALE - value;
            AutoSIOffTimeTrigger::S_GetInstance()->PhaseInSettingChange(timeoff);

        }
    }
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdateSITime()
//
//    Processing:
//      Update Current SI Time in Patient Data Monitor
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//     None
//
//    Pre-Conditions:
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void AutoSIOnTimeTrigger::UpdateSITime()
{
    one_hundred_ms = one_hundred_ms + 2;
    if(one_hundred_ms >= 100)
    {
        one_hundred_ms = 0;
        currentSITimeData = currentSITimeData + 10;
        //        PatientDataGUIEvent SIMsg;
        //        SIMsg.Value = currentSITimeData;
        //        MainHandlerDelegate::S_GetInstance()->postCustomEvent<PatientDataGUIEvent>(eManInspHoldTimePtDataID, SIMsg);
    }
}
