/******************************************************************************/
//$COMMON.CPP$
//   File Name:  PtExhFlowSpontTrigger.cpp
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
//      retrieve the latest pressure and flow readings.  CheckCondition() also 
//      calls InhPhase::S_CalcTimeSincePhaseStart() to determine the time 
//      elapsed since the start of the Inhalation phase and 
//      SystemClock::Retrieve() to snapshot the current system time.
//
/******************************************************************************/
#include <InhPhase.h>
#include "PtExhFlowSpontTrigger.h"
#include "MgrBreathData.h"
#include "util.h"

static const SHORT SPON_PEAKFLOW_TERM    = 10; // 1/10%
static const SHORT SPON_FLOWRATE_TERM    = 5*SCALE; // 5ml/s,scaled

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PtExhFlowSpontTrigger()
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
PtExhFlowSpontTrigger::PtExhFlowSpontTrigger(void):Trigger(ePtExhFlowSpontTrigger)
{
    PrevFilteredDelFlow = 0;
}   // end PtExhFlowSpontTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation is invoked every BD task cycle while the Patient 
//      Exhalation Pressure Trigger is enabled.  This operation retrieves the
//      latest pressure and flow readings from MgrBreathData.  It then checks
//      if the following criteria are met:
//              
//          Measured Pressure >= Pressure Limit &&
//          Delivered Flow < .60 * Peak Delivered Flow
// 
//
//      If the criteria are not met, the IsConditionMet attribute is set to 
//      FALSE and the current value of TriggerDidFire (FALSE) is returned to 
//      the caller.
//
//      If the criteria are met, InhPhase::S_CalcTimeSincePhaseStart() is called 
//      to determine the time elapsed since the start of the inhalation phase.   
//      This value is compared to INIT_PRESS_CRITERIA_PHASE to determine if the 
//      ventilator is in the initial part of the inhalation.  If not, the 
//      TriggerDidFire attribute is set to TRUE and returned to the caller.
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
E_Bool PtExhFlowSpontTrigger::CheckCondition(void)
{
    const UNSIGNED INIT_FLOW_CRITERIA_PHASE = 200/MS_PER_TICK;  // 200 ms in TICKS

    LONG filteredDelFlow;
    
    // Get ta pointer to the MgrBreathData object
    MgrBreathData* mgrBreathDataPtr = MgrBreathData::S_GetInstance();

    // Filter the flow value using the equation:
    // y(n) = 0.5 * x(n) + 0.5 * y(n-1)    
    filteredDelFlow = (PrevFilteredDelFlow >> 1) + 
                      (mgrBreathDataPtr->GetDeliveredFlow() >> 1); 

    // Update the Previous value to prepare for the next cycle
    PrevFilteredDelFlow = filteredDelFlow;

    // Check if the time elapsed since the start of inhalation is greater
    // than or equal to the initial phase.  During the initial part of
    // inhalation, the criteria that cause this trigger to fire must be
    // met for a minimum amount of time.  But after the initial
    // time period, the trigger criteria is met right away, so set
    // the TriggerDidFire attribute.
    if (InhPhase::S_CalcTimeSincePhaseStart() >= INIT_FLOW_CRITERIA_PHASE)
    {
        // Compare the delivered flow for the current cycle to the exhalation
        // sensitivity (percentage) of the peak delivered flow.
        // The divide by 100 is needed since ExhSens is a percentage.
        if ((filteredDelFlow < (mgrBreathDataPtr->GetPeakDeliveredFlow()/ SPON_PEAKFLOW_TERM))
            ||(filteredDelFlow < SPON_FLOWRATE_TERM))
        {
            TriggerDidFire = eTrue;
        }
    }

    return(TriggerDidFire);

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
void PtExhFlowSpontTrigger::EnableTrigger(void)
{
    // Initialize the previous filtered flow value
    PrevFilteredDelFlow = 0;

    // Invoke Base Class Enable Method
    Trigger::EnableTrigger();


}   // end EnableTrigger()
