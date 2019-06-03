/******************************************************************************/
//$COMMON.CPP$
//   File Name:  PtExhFlowSuppTrigger.cpp
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
/******************************************************************************/
#include <DeviceInterface.h>
#include <InhPhase.h>
#include "PtExhFlowSuppTrigger.h"
#include "MgrBreathData.h"
#include "MgrBdSetting.h"
#include "util.h"
#include "ptdata.h"
#include "ModeMgr.h"
#include "ProxyFlowSensor.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PtExhFlowSuppTrigger()
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
PtExhFlowSuppTrigger::PtExhFlowSuppTrigger(void):Trigger(ePtExhFlowSuppTrigger)
{
    BiasFlow = 10;
    IsConditionMet = eFalse;
    TimeConditionFirstMet = 0;
    PrevFilteredDelFlow = 0;
    PrevFilteredProxFlow = 0;
}   // end PtExhFlowSuppTrigger()

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
E_Bool PtExhFlowSuppTrigger::CheckCondition(void)
{
    const UNSIGNED INIT_FLOW_CRITERIA_PHASE = 200/MS_PER_TICK;  // 200 ms in TICKS

    LONG filteredDelFlow;
    LONG filteredProxFlow;
    USHORT exhSens;
    
    // Get ta pointer to the MgrBreathData object
    MgrBreathData* mgrBreathDataPtr = MgrBreathData::S_GetInstance();

    //biasFlow (ml/s * SCALE)
    exhSens = MgrBdSetting::S_GetInstance()->GetCurrentValue(eExhSens)/100;
    BiasFlow = MgrBdSetting::S_GetInstance()->GetCurrentValue(eBaseFlow);

    // Filter the flow value using the equation:
    // y(n) = 0.5 * x(n) + 0.5 * y(n-1)
    LONG DeliveredFlow = mgrBreathDataPtr->GetDeliveredFlow();
    LONG ProxyFlow = mgrBreathDataPtr->GetProxyFlow();
    filteredDelFlow = (PrevFilteredDelFlow >> 1) + (DeliveredFlow >> 1);
    filteredProxFlow = (PrevFilteredProxFlow >> 1) + (ProxyFlow >> 1);

    // Update the Previous value to prepare for the next cycle
    PrevFilteredDelFlow = filteredDelFlow;
    PrevFilteredProxFlow = filteredProxFlow;

    // Check if the time elapsed since the start of inhalation is greater
    // than or equal to the initial phase.  During the initial part of
    // inhalation, the criteria that cause this trigger to fire must be
    // met for a minimum amount of time.  But after the initial
    // time period, the trigger criteria is met right away, so set
    // the TriggerDidFire attribute
    if (InhPhase::S_CalcTimeSincePhaseStart() >= INIT_FLOW_CRITERIA_PHASE)
    {
        // Compare the delivered flow for the current cycle to the exhalation
        // sensitivity (percentage) of the peak delivered flow.
        // The divide by 100 is needed since ExhSens is a percentage.
        LONG limit=0;
        LONG PeakFlow = mgrBreathDataPtr->GetPeakDeliveredFlow();

        E_VentilationMode currenMode = ModeMgr::S_GetInstance()->GetCurrentModeId();

        if (currenMode == eNivMode)
        {
            limit = ((exhSens * (PeakFlow - BiasFlow)) / 100) + BiasFlow;

            if (filteredDelFlow < limit)
            {
                TriggerDidFire = eTrue;
            }
        }
        else
        {
            ProxyStatus Status = ProxyFlowSensor::S_GetInstance()->getLastStatus();

            if((Status == eCalibrated) || (Status == eResetAndRecovered))
            {
                LONG leak = Ptdata::S_GetInstance()->GetExhLeak()*SCALE;
                PeakFlow = mgrBreathDataPtr->GetPeakProxyFlow();
                limit = ((exhSens * (PeakFlow - leak)) / 100) + leak;

                if (filteredProxFlow < limit)
                {
                    TriggerDidFire = eTrue;
                }
            }
            else
            {
                limit = ((exhSens * (PeakFlow - BiasFlow)) / 100) + BiasFlow;

                if (filteredDelFlow < limit)
                {
                    TriggerDidFire = eTrue;
                }
            }
        }
    }
    return(TriggerDidFire);

}   // end CheckCondition()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This method is invoked when the PtExhFlowSuppTrigger is enabled during CPAP
//      and supported inhalations.  The ExhSens attribute is initialized with
//      the operator set exhalation sensitivity.  Other attributes are 
//      initialized to clear any information from the previous time this trigger
//      was enabled.
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
void PtExhFlowSuppTrigger::EnableTrigger(void)
{    
    // Initialize the previous filtered flow value
    PrevFilteredDelFlow = 0;

    // Invoke Base Class Enable Method
    Trigger::EnableTrigger();

}   // end EnableTrigger()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PhaseInSettingChange()
//
//    Processing: 
//      This operation is invoked by the Flow Sensitvity setting object
//      whenever a flow sensitivity change request is received.
//      If the trigger is enabled, this operation updates the FlowLimit and
//      calls FlowTrigExhPhase::SetBiasFlow() to update the bias flow based
//      upon the new sensitivity.  Sensitivity changes are phased in right
//      away -- i.e. not at the start of the next breath phase.
//
//    Input Parameters:
//      value - new flow sensitivity setting
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
void PtExhFlowSuppTrigger::PhaseInSettingChange(LONG value)
{
    // Update the trigger sensitivity
    BiasFlow = (value * 100) / 6;
    LONG BiasValue = value / 10;
    DeviceInterface::S_GetInstance()->RequestDevice(eCMVServoControllerSetBiasFlow, &BiasValue);

}// end PhaseInSettingChange()



