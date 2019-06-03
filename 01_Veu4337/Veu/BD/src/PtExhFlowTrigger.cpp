/******************************************************************************/
//$COMMON.CPP$
//   File Name:  PtExhFlowTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Patient Exhalation Flow
//      Trigger, which is derived from the base Trigger class.  
//
//      The Patient Exhalation Flow Trigger is enabled at the start of
//      each spontaneous or supported inhalation.  This trigger implements the 
//      flow criteria to cause a normal (non-alarm) end to a spontaneous or
//      supported inhalation.
//
//   Interfaces:
//      The CheckCondition() operation interfaces with MgrBreathData to 
//      retrieve the latest delivered flow reading and the peak delivered 
//      flow reading for the current breath.  CheckCondition() also calls
//      InhPhase::S_CalcTimeSincePhaseStart() to determine the time elapsed 
//      since the start of the Inhalation phase and SystemClock::Retrieve()
//      to snapshot the current system time.
//
//      The EnableTrigger() operation invokes MgrBdSetting::GetCurrentValue()
//      to retrieve the current setting for the exhalation sensitivity when
//      the EnableTrigger() operation with no input parameters is invoked.
//
//   Restrictions:
//      None
/******************************************************************************/
//#include "nucpp.h"
#include <InhPhase.h>
#include <Util.h>
#include "PtExhFlowTrigger.h"
#include "MgrBreathData.h"
#include "MgrBdSetting.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PtExhFlowTrigger()
//
//    Processing: 
//      This operation is the constructor for the Patient Exhalation Flow
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
PtExhFlowTrigger::PtExhFlowTrigger (void) : Trigger(eNoTrigger)
//        Trigger (ePtExhFlowTrigger)
{

}   // end PtExhFlowTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation is invoked every BD task cycle while the Patient 
//      Exhalation Flow Trigger is enabled.  This operation calls the
//      GetDeliveredFlow() and GetPeakDeliveredFlow() operations to get the 
//      latest delivered flow and the peak delivered flow for the current 
//      breath.  It then checks if the delivered flow is less than or equal
//      to the ExhSens percentage of the peak delivered flow.  If the flow
//      condition is not met, the IsConditionMet attribute is set to FALSE 
//      and the current value of TriggerDidFire (FALSE) is returned to the 
//      caller.
//
//      If the flow condition is met, InhPhase::S_CalcTimeSincePhaseStart() 
//      is called to determine the time elasped since the start of the 
//      inhalation phase.   This value is compared to INIT_FLOW_CRITERIA_PHASE 
//      to determine if the ventilator is in the initial part of the inhalation.  
//      If not, the TriggerDidFire attribute is set to TRUE and returned to the
//      caller.
//
//      If the ventilator is in the INIT_FLOW_CRITERIA_PHASE of the inhalation,
//      IsConditionMet is checked to see if the criteria were met on the 
//      previous cycle.  If not, IsConditionMet is set to TRUE and the system
//      clock is snapshot and stored in the TimeConditionFirstMet attribute.
//
//      If the ventilator is in the INIT_FLOW_CRITERIA_PHASE but the criteria
//      have been met on the previous cycle, the system clock is retrieved and
//      compared to the TimeConditionFirstMet.  During the initial part of
//      inhalation, the Flow criteria must be met for at least
//      the MIN_DURATION_FLOW_COND_MET.
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
E_Bool PtExhFlowTrigger::CheckCondition (void)
{
    LONG filteredDelFlow;

    // Get ta pointer to the MgrBreathData object
    MgrBreathData* mgrBreathDataPtr = MgrBreathData::S_GetInstance ();

    // Filter the flow value using the equation:
    // y(n) = 0.5 * x(n) + 0.5 * y(n-1)    
    filteredDelFlow = (PrevFilteredDelFlow >> 1)
            + (mgrBreathDataPtr->GetDeliveredFlow () >> 1);

    // Update the Previous value to prepare for the next cycle
    PrevFilteredDelFlow = filteredDelFlow;

    // Compare the delivered flow for the current cycle to the exhalation
    // sensitivity (percentage) of the peak delivered flow.
    // The divide by 100 is needed since ExhSens is a percentage.
    if(filteredDelFlow
            <= ((ExhSens * mgrBreathDataPtr->GetPeakDeliveredFlow ()) / 100))
    {

        // Check if the time elapsed since the start of inhalation is greater
        // than or equal to the initial phase.  During the initial part of 
        // inhalation, the criteria that cause this trigger to fire must be
        // met for a minimum amount of time.  But after the initial
        // time period, the trigger criteria is met right away, so set
        // the TriggerDidFire attribute.
        if(InhPhase::S_CalcTimeSincePhaseStart () >= INIT_FLOW_CRITERIA_PHASE)
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
                        >= MIN_DURATION_FLOW_COND_MET)

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
        // Failed the flow condition check.
        IsConditionMet = eFalse;
    }

    return (TriggerDidFire);

}   // end CheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This method is invoked when the PtExhFlowTrigger is enabled during
//      supported inhalations.  During supported inhalations, the operator set 
//      exhalation sensitivity is used.
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
void PtExhFlowTrigger::EnableTrigger (void)
{

    // Initialize IsConditionMet.
    IsConditionMet = eFalse;

    // Get the exhalation sensitivity; remove scaling and store the value in %
    ExhSens = (MgrBdSetting::S_GetInstance ())->GetCurrentValue (eExhSens)
            / SCALE;

    // Initialize the previous filtered flow value
    PrevFilteredDelFlow = 0;

    // Invoke Base Class Enable Method
    Trigger::EnableTrigger ();

}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This method is invoked when the PtExhFlowTrigger is enabled during
//      CPAP inhalations.  During CPAP, the operator set exhalation sensitivity
//      is not used like it is during PSV or NIPPV spontaneous inhalations.
//
//    Input Parameters:
//      sensitivity - the exhalation sensitivity in % 
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
void PtExhFlowTrigger::EnableTrigger (UNSIGNED sensitivity)
{

    // Initialize IsConditionMet.
    IsConditionMet = eFalse;

    // Use the passed in sensitivity
    ExhSens = (LONG) sensitivity;

    // Invoke Base Class Enable Method
    Trigger::EnableTrigger ();

}   // end EnableTrigger()

