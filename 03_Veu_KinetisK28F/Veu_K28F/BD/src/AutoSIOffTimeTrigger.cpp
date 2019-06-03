/******************************************************************************/
//$COMMON.H$
//   File Name:  AutoSIOffTimeTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the AutoSIOffTimeTrigger class
/******************************************************************************/
#include "AutoSIOffTimeTrigger.h"
#include "DebugTrace.h"

AutoSIOffTimeTrigger* AutoSIOffTimeTrigger::S_Instance = NULL;

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
AutoSIOffTimeTrigger* AutoSIOffTimeTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new AutoSIOffTimeTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AutoSIOffTimeTrigger()
//
//    Processing: Constructor for class
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
AutoSIOffTimeTrigger::AutoSIOffTimeTrigger()
    : TimeTrigger(eAutoSIOffTimeTrigger,(CHAR*)"AutoSI")
{
    currentTime = 0;
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing:
//
//    Input Parameters:
//      E_VentilationMode fromModeId
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
E_Bool AutoSIOffTimeTrigger::CheckCondition(void)
{
    E_Bool rtnValue = TriggerDidFire;
    if(TriggerDidFire)
    {
        TriggerDidFire = eFalse;
        DisableTrigger();
    }
    return (rtnValue);
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing:
//
//    Input Parameters:
//      E_VentilationMode fromModeId
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
void AutoSIOffTimeTrigger::EnableTrigger(void)
{
    const UNSIGNED BD_CYCLE_TIME_IN_TICKS = 2;

    const UNSIGNED OFFSET_IN_TICKS =
            (BD_CYCLE_TIME_IN_TICKS << 1);

    UNSIGNED timePeriod;

//    STATUS status;

    if(currentTime == 0)
    {
        return;
    }
    else
    {
        timePeriod = ((currentTime * TICKS_PER_SEC) / SCALE) + OFFSET_IN_TICKS;

        SetTimePeriod(timePeriod);
//        status = Reset(GetTimePeriod(), 0, TRUE);
        Reset();

        Trigger::EnableTrigger();
    }
}   // end EnableTrigger()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PhaseInSettingChange()
//
//    Processing:
//
//    Input Parameters:
//      E_VentilationMode fromModeId
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
void AutoSIOffTimeTrigger::PhaseInSettingChange(LONG value)
{
    const UNSIGNED BD_CYCLE_TIME_IN_TICKS = 2;

    if(currentTime == value)
        return;
    if(value == 0)
    {
        DisableTrigger();
        currentTime = value;
    }
    else if(currentTime != value)
    {
        DisableTrigger();
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
        EnableTrigger();
    }
}
