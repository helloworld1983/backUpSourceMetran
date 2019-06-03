/******************************************************************************/
//$COMMON.H$
//   File Name:  NIVBpTimeTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the NIVBpTimeTrigger class
/******************************************************************************/
#include <InhPhase.h>
#include "NIVBpTimeTrigger.h"

#include "ExhPhase.h"
#include "AzTest.h"
#include "ModeMgr.h"
#include "MixedMode.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: NIVBpTimeTrigger()
//
//    Processing:
//      This operation is the constructor for the NIVBpTimeTrigger class.  It
//      invokes the TimeTrigger constructor and initializes the IsChangePending
//      attribute to FALSE.
//
//    Input Parameters:
//      id - Trigger identifier
//      timerName - name of the Timer
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
NIVBpTimeTrigger::NIVBpTimeTrigger(E_TriggerId id, CHAR* timerName)
    : CyclicTimeTrigger(id, timerName),
      IsChangePending(eFalse)
{
    breathRateValue = 0;
}   // end NIVBpTimeTrigger()




/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing:
//      This method is called when the breath period time trigger is enabled.
//      It first retrieves the currently set breath period by calling
//      GetTimePeriod().  It then invokes the Timer::Reset() to start the
//      breath period timer.  Finally, it calls the base class method
//      Trigger::EnableTrigger().
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
//      If anything but a successful status is returned from the RTOS
//      routine Timer::Reset(), perform error handling.
//
//    Requirements:
//
/******************************************************************************/
void NIVBpTimeTrigger::EnableTrigger(void)
{
    // Retrieve the currently set breath period
//    UNSIGNED timePeriod = GetTimePeriod();

    // Reset the timer for the Breath Period, which is stored in TimePeriod.
    // Using TimePeriod as the second argument indicates that the timer is
    // continuous.  Passing TRUE as the last argument indicates that the timer
    // should be enabled now.   Perform error handling if the reset is not
    // successful.
//    if (Reset(timePeriod, timePeriod, TRUE) != Q_SUCCESS)
//    {
//        ASSERTION(eFalse);
//    }
    Reset();

    // Invoke base class enable method which updates the IsEnabled
    // and TriggerDidFire flags
    Trigger::EnableTrigger();

    // If a change was pending, it just got phased in
    IsChangePending = eFalse;

}   // end EnableTrigger()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing:
//      This method is called when the breath period time trigger is enabled
//      with a specific value (not the breath period) for the initial time.
//      It invokes the Timer::Reset() to start the breath period timer.  It
//      also invokes the base class method Trigger::EnableTrigger().
//
//    Input Parameters:
//      time - time period to use as the intial time
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
//      If anything but a successful status is returned from the RTOS
//      routine Timer::Reset(), perform error handling.
//
//    Requirements:
//
/******************************************************************************/
void NIVBpTimeTrigger::EnableTrigger(UNSIGNED time)
{
    // Reset the timer using the passed in value for time as the initial
    // period and the Breath Period for every duration after that.  The
    // Breath Period is stored in TimePeriod.  Passing TRUE as the last
    // argument indicates that the timer should be enabled now.
    // Perform error handling if the reset is not successful.
//    UNSIGNED timePeriod = GetTimePeriod();

//    if (Reset(time, timePeriod, TRUE) != Q_SUCCESS)
//    {
//        ASSERTION(eFalse);
//    }
    Reset();

    // Invoke base class enable method which updates the IsEnabled
    // and TriggerDidFire flags
    Trigger::EnableTrigger();

    // If a change was pending, it just got phased in
    IsChangePending = eFalse;

}   // end EnableTrigger()




/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ReEnableTrigger()
//
//    Processing:
//      This method is invoked from the AcMode whenever an operator or patient
//      initiated breath is initiated.  This operation disables the timer
//      then invokes the EnableTrigger() method to restart the timer.  When
//      a patient or operator breath is triggered in A/C, the breath period is
//      re-started.
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
void NIVBpTimeTrigger::ReEnableTrigger(void)
{
    // Check IsEnabled here; only want to re-enable if currently enabled
    if (eTrue == IsEnabled)
    {
        // Disable the timer
        Stop();

        // Re-enable the trigger
        EnableTrigger();
    }

}   // end ReEnableTrigger()





/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing:
//      This operation is invoked every BD Cycle Time while the trigger is
//      enabled.  If the trigger has not already fired and a respiratory rate
//      change is pending, the PhaseIn() operation is invoked to phase in the
//      respiratory rate.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool -- indicates if the timer expired and the trigger fired
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
E_Bool NIVBpTimeTrigger::CheckCondition(void)
{
    static E_Bool isDelayed = eFalse;
    E_Bool rtnValue = TriggerDidFire;
    if(breathRateValue == 0) return eFalse;

    // If the trigger fired, reset it.
    if (eTrue == rtnValue)
    {
        if (PInhAzTestPtr->GetAzTestState() == eAzActive)
        {
            // If an auto-zero is in progress, delay the BP trigger.  This case
            // is only possible for high rates and inverse I:E ratios.
            rtnValue = eFalse;

            // Set a flag indicating that the BP timer was delayed.
            isDelayed = eTrue;
        }

        else
        {
            TriggerDidFire = eFalse;

            // If a change is pending, re-enable the trigger so that the new
            // breath period is phased in.  If the previous trigger was delayed,
            // re-enable the trigger so that BP timer is synched up with the
            // start of the inhalation.
            if ((eTrue == IsChangePending) || (eTrue == isDelayed))
                //            if (eTrue == IsChangePending)
            {
                ReEnableTrigger();
                isDelayed = eFalse;
            }
        }

    }
    else
    {
        // If the trigger hasn't already fired and a respiratory rate change is
        // pending, phase in the change.  The trigger may fire as a result of
        // the new rate being phased in.
        if (eTrue == IsChangePending)
        {
            PhaseIn();
        }
    }

    return (rtnValue);

}   // end CheckCondition()




/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PhaseInSettingChange()
//
//    Processing:
//      This method is invoked whenever the RR is changed.  This operation
//      calculates a new breath period based upon the respiratory rate and
//      stores the value in the TimePeriod attribute.  If the trigger is
//      enabled, the private PhaseIn() operation is called to update the
//      timer and trigger status based upon the new respiratory rate.
//
//    Input Parameters:
//      newValue - new respiratory rate in BPM * SCALE
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
void NIVBpTimeTrigger::PhaseInSettingChange(LONG newValue)
{
    UNSIGNED breathPeriod = 0;      // new breath period calculated from new RR

    // Always update the Breath period; will only phase in the change if the
    // trigger is currently active.  The new RR passed to this method is
    // scaled.
    if(newValue != 0)
        breathPeriod = (TICKS_PER_MIN * SCALE) / newValue;
    breathRateValue  = newValue;

    // Store the breath period in the TimePeriod attribute
    SetTimePeriod(breathPeriod);

    // Only need to worry about phasing in the time change if the trigger is
    // enabled and a change is not already pending
    if ((eTrue == IsEnabled) && (eFalse == IsChangePending))
    {
        PhaseIn();
    }

}   // end PhaseInSetting()




/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PhaseIn()
//
//    Processing:
//      This method is invoked to phase in a new respiratory rate if the
//      NIVBpTimeTrigger is enabled.  This method first checks if a new rate
//      can be phased in.  Rate changes can only be phased in after a minimum
//      amount of time has elapsed since the start of exhalation.  If the
//      ventilator is in Inhalation or if this minimum time has not elapsed,
//      the the attribute IsChangePending is set so that the CheckCondition
//      method will call this method every BD Cycle time until the rate can
//      be phased in.
//
//      If it is okay for the rate to be phased in, the Breath Period Timer
//      is disabled by calling Timer::Disable().  Then
//      InhPhase::S_CalcTimeSincePhaseStart() is called to determine
//      the amount of the total breath period time that has already elapsed.
//      If the new breath period has already elapsed, the trigger
//      fires right away; otherwise, the timer is re-enabled for the time
//      remaining in the new breath period.
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
//      The operation assumes that the new Breath Period has already been
//      placed in the TimePeriod attribute.
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void NIVBpTimeTrigger::PhaseIn(void)
{
    UNSIGNED bpElapsedTime;     // time elapsed since the start of the last inh

    // See if the ventilator is in the unrestricted phase of exhalation
    // By not passing any parameters, the default minimum exhalation time
    // is used.   The ventilator waits until the minimum exhalation time has
    // elapsed since the start of exhalation before phasing in a rate change.
    if (ExhPhase::S_IsInUnrestrictedPhase() == eTrue)
    {

        // Disable the Timer
//        Disable();
        Stop();

        // Determine the amount of the total breath period time that
        // has already elapsed.
        bpElapsedTime = InhPhase::S_CalcTimeSincePhaseStart();

        // If the new breath period has already elapsed, the trigger
        // fires right away; otherwise, the timer is re-enabled
        // accounting for the new breath period
        if (GetTimePeriod() <= bpElapsedTime)
        {
            // Since this is a continuous timer, enable it right away
            EnableTrigger();

            // Set the TriggerDidFire flag since the trigger has already
            // fired.   This must be done after the call to EnableTrigger()
            // since EnableTrigger() resets the flag.
            TriggerDidFire = eTrue;

        }
        else
        {
            // Enable the trigger for the amount of time remaining
            EnableTrigger(GetTimePeriod() - bpElapsedTime);
        }

        // If a change was pending, it just got phased in; reset the flag.
        IsChangePending = eFalse;

    }   // in unrestricted phase
    else
    {
        // Phase it in later; can't do it now
        IsChangePending = eTrue;
    }
}   // end PhaseIn()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PreCheckCondition()
//
//    Processing:
//      This operation is invoked by the ModeChangeTrigger::CheckCondition()
//      operation when the mode change trigger is armed but is waiting
//      for the unrestricted phase of exhalation.   PreCheckCondition() looks
//      ahead to determine if the BP Time trigger has already fired or is
//      going to fire within the next BD cycle and cause a mandatory inhalation
//      to be delivered.  The ModeChangeTrigger uses this information to
//      determine whether the mode change should be phased in early.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool -- indicates if the timer is about to expire and cause a
//          transition to inhalation.
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
E_Bool NIVBpTimeTrigger::PreCheckCondition(void)
{
    E_Bool rtnValue = eFalse;

    // In order for the pre-check to pass,
    //      the trigger must be enabled
    //      an auto-zero of the exhalation pressure transducer cannot be active
    //      triggering of the BP timer must result in delivery of a mandatory
    //          inhalation (this is the case if the trigger fires in any mode
    //          except Mixed or if the trigger fires in Mixed mode and the
    //          Mandatory window is open)

    if ( eTrue == ExhPhase::S_IsInUnrestrictedPhase() )
    {
        if ((eTrue == IsEnabled) &&
                ((ModeMgr::S_GetInstance()->GetCurrentModeId() != eMixedMode) ||
                        (MixedMode::S_GetInstance()->GetWindow() == eMandWindow)))
        {
            // If the timer has already expired or will expire in less than one BD
            // control cycle, the pre-check condition is met.  The time check is
            // made to avoid a possible race condition if the timer hasn't expired
            // when the mode trigger is checked but expires before the BP timer is
            // checked.
            if (TriggerDidFire == eTrue)
            {
                rtnValue = eTrue;
            }
        }
    }

    return (rtnValue);

}   // end PreCheckCondition()
