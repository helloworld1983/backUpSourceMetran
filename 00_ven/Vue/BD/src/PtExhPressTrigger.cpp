/******************************************************************************/
//$COMMON.CPP$
//   File Name:  PtExhPressTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Patient Exhalation Pressure
//      Trigger, which is derived from the base Trigger class.  
//
//      The Patient Exhalation Pressure Trigger is enabled at the start of
//      each supported and spontaneous inhalation.  This trigger implements
//      the pressure criteria to cause a normal (non-alarm) end to a 
//      spontaneous or supported inhalation.
//
//   Interfaces:
//      The CheckCondition() operation interfaces with MgrBreathData to 
//      retrieve the latest pressure reading.  CheckCondition() also calls
//      InhPhase::S_CalcTimeSincePhaseStart() to determine the time elapsed 
//      since the start of the Inhalation phase and SystemClock::Retrieve()
//      to snapshot the current system time.
//
//   Restrictions:
//      None
/******************************************************************************/
//#include "nucpp.h"
#include <InhPhase.h>
#include <Util.h>
#include "PtExhPressTrigger.h"
#include "MgrBreathData.h"

#include "DebugTrace.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PtExhPressTrigger()
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
PtExhPressTrigger::PtExhPressTrigger (void) : Trigger (eNoTrigger)
//        Trigger (ePtExhPressTrigger)
{

}   // end PtExhPressTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation is invoked every BD task cycle while the Patient 
//      Exhalation Pressure Trigger is enabled.  This operation calls 
//      MgrBreathData::GetMonitoredPressure() to get the latest 
//      pressure reading.  This operation then compares the value to the
//      PressureLimit attribute.  The PressureLimit attribute contains the 
//      pressure trigger limit and was initialized in the EnableTrigger()
//      operation.  If the current pressure is less than the PressureLimit, 
//      the IsConditionMet attribute is set to FALSE and the current value
//      of TriggerDidFire (FALSE) is returned to the caller.
//
//      If the current pressure is greater than or equal to the PressureLimit,
//      InhPhase::S_CalcTimeSincePhaseStart() is called to determine the time
//      elasped since the start of the inhalation phase.   This value is 
//      compared to INIT_PRESS_CRITERIA_PHASE to determine if the ventilator
//      is in the initial part of the inhalation.  If not, the TriggerDidFire
//      attribute is set to TRUE and returned to the caller.
//
//      If the ventilator is in the INIT_PRESS_CRITERIA_PHASE of the inhalation,
//      IsConditionMet is checked to see if the criteria were met on the 
//      previous cycle.  If not, IsConditionMet is set to TRUE and the system
//      clock is snapshot and stored in the TimeConditionFirstMet attribute.
//
//      If the ventilator is in the INIT_PRESS_CRITERIA_PHASE but the criteria
//      have been met on the previous cycle, the system clock is retrieved and
//      compared to the TimeConditionFirstMet.  During the initial part of
//      inhalation, the pressure criteria must be met for at least
//      the MIN_DURATION_PRESS_COND_MET.
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
E_Bool PtExhPressTrigger::CheckCondition (void)
{
    // Check if the monitored pressure is greater than or equal to the 
    // PressureLimit.  The PressureLimit is initialized in EnableTrigger().
    if((MgrBreathData::S_GetInstance ())->GetMonitoredPressure ()
            >= PressureLimit)
    {
        // Check if the time elapsed since the start of inhalation is greater
        // than or equal to the initial phase.  During the initial part of 
        // inhalation, the criteria that cause this trigger to fire must be
        // met for a minimum amount of time.  But after the initial
        // time period, the trigger criteria is met right away, so set
        // the TriggerDidFire attribute.
        if(InhPhase::S_CalcTimeSincePhaseStart () >= INIT_PRESS_CRITERIA_PHASE)
        {
            TriggerDidFire = eTrue;
        }

        else
        {
            // The initial part of the inhalation is active.  Check if the
            // condition was met on the previous cycle. 
            if(eTrue == IsConditionMet)
            {
                // Since the condition has been met before, see how long the
                // condition has been met.  If this value exceeds the minimum
                // time criteria, set the TriggerDidFire attribute.
                if(CalcElapsedTime (TimeConditionFirstMet)
                        >= MIN_DURATION_PRESS_COND_MET)

                {
                    TriggerDidFire = eTrue;
                }

            }
            else
            {
                // The condition was not met on the previous cycle, so set
                // the IsConditionMet flag and store the time stamp of when
                // the condition was met.
                IsConditionMet = eTrue;
//                TimeConditionFirstMet = SystemClock::Retrieve();

            }

        }
    }
    else
    {
        // Failed the pressure condition check.
        IsConditionMet = eFalse;
    }

    return (TriggerDidFire);

}   // end CheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This operation should never be invoked for this trigger.  Whenever
//      the Patient Exhalation Pressure Trigger is enabled, the EnableTrigger
//      with a pressure argument should be invoked. 
//
//      This operation performs error handling if called.
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
void PtExhPressTrigger::EnableTrigger (void)
{

    // This enable method should not be used, must provide Pressure
    ASSERTION(eFalse);

}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This method is invoked when the PtExhPressTrigger is enabled during
//      CPAP or supported inhalations.  This operation sets the IsConditionMet
//      attribute to FALSE and the PressureLimit to the passed pressure 
//      argument.  It then invokes the base class Trigger::EnableTrigger()
//      method.
//
//    Input Parameters:
//      UNSIGNED pressure - pressure limit that causes the trigger to fire
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
void PtExhPressTrigger::EnableTrigger (UNSIGNED pressure)
{

    // Initialize IsConditionMet.
    IsConditionMet = eFalse;

    // Use the passed in pressure
    PressureLimit = (SHORT) pressure;

    // Invoke Base Class Enable Method
    Trigger::EnableTrigger ();

}   // end EnableTrigger()

