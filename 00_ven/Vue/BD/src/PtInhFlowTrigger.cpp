/******************************************************************************/
//$COMMON.CPP$
//   File Name:  PtInhFlowTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Patient Inhalation Flow
//      Trigger, which is derived from the base Trigger class.  
//
//      The Patient Inhalation Flow Trigger is enabled at the start of the
//      exhalation phase if the operator set Trigger Type is Flow.
//
//   Interfaces:
//      The CheckCondition() and PreCheckCondition() operations interface
//      with MgrBreathData to retrieve the latest delivered and exhaled flow
//      readings.  They also call ExhPhase::S_IsInUnrestrictedPhase() to 
//      determine if the ventilator is in the unrestricted phase of exhalation. 
//      The EnableTrigger() and PhaseInSetting() operations invoke 
//      MgrBdSetting::GetCurrentValue() to retrieve the flow sensitivity
//      setting.
//
//   Restrictions:
//      None
/******************************************************************************/
#include <DeviceInterface.h>
#include <ExhPhase.h>
#include "PtInhFlowTrigger.h"
#include "MgrBreathData.h"
#include "MgrBdSetting.h"
#include "FlowTrigExhPhase.h"
#include "ModeMgr.h"
#include "ProxyFlowSensor.h"
#include "PtData.h"
#include "APRV_PHighPhase.h"
#include "APRV_SupportedInhPhase_InPHigh.h"
#include "APRV_ExhPhase_InPHigh.h"
#include "APRV_PLowPhase.h"
#include "APRV_SupportedInhPhase_InPLow.h"
#include "APRV_ExhPhase_InPLow.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PtInhFlowTrigger()
//
//    Processing: 
//      This operation is the constructor for the Patient Inhalation Flow
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
PtInhFlowTrigger::PtInhFlowTrigger (void) :
        Trigger (ePtInhFlowTrigger)
{
    PrevState = CurState = eOff;
    InUnrestrictedPhase = eFalse;
    timerCnt = 0;

}   // end PtInhFlowTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation is invoked every BD task cycle while the Patient 
//      Inhalation Flow Trigger is enabled.  If the InUnrestrictedPhase
//      attribute is TRUE, indicating that it was already determined that the
//      ventilator is in the unrestricted phase of exhalation, the
//      GetExhaledFlow() and GetDeliveredFlow() operations of the MgrBreathData
//      object are called to retrieve the latest delivered and exhaled flow
//      readings.  This operation then compares the value of (Exhaled flow -
//      delivered flow) to the FlowLimit attribute, which contains the 
//      flow sensitivity.  If (exhaled - delivered) <= (0 - flow sensitivity), 
//      the TriggerDidFire attribute is set to TRUE and returned to the caller.
//
//      If the IsInUnrestrictedPhase attribute is FALSE, 
//      ExhPhase::S_IsInUnrestrictedPhase() is called to determine if the 
//      ventilator is in the unrestricted phase.  If so, IsInUnrestrictedPhase
//      is set TRUE so that this ExhPhase::S_IsInUnrestrictedPhase() does
//      not have to be called every BD cycle after it has already been 
//      determined that the unrestricted part of exhalation is active.  If
//      the unrestricted phase is active, the GetExhaledFlow() and 
//      GetDeliveredFlow() operations of the MgrBreathData object
//      are called to retrieve the latest delivered and exhaled flow
//      readings.  This operation then compares the value of (Exhaled flow -
//      delivered flow) to the FlowLimit attribute as described in the previous
//      paragraph.  If the trigger condition is met, the TriggerDidFire 
//      attribute is set to TRUE and returned to the caller.
//
//      If the unrestricted phase is not active or if the flow condition
//      is not met, TriggerDidFire is not updated and its current value (FALSE)
//      is returned to the caller.
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
E_Bool PtInhFlowTrigger::CheckCondition (void)
{
    if(MgrBdSetting::S_GetInstance()->GetFlowTriggOn())
    {
        ProxyStatus Status = ProxyFlowSensor::S_GetInstance()->getLastStatus();

        if((Status == eCalibrated)||(Status == eResetAndRecovered))
        {
            if(ModeMgr::S_GetInstance()->GetCurrentModeId() == eAPRVMode)
            {
                TriggerDidFire = APRV_CheckCondition();
            }
            else
            {
                TriggerDidFire = IMV_CheckCondition();
            }
        }
    }
    return(TriggerDidFire);

}   // end CheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PreCheckCondition()
//
//    Processing: 
//      This operation is invoked by the ModeChangeTrigger::CheckCondition()
//      operation when the mode change trigger is armed but is waiting
//      for the unrestricted phase of exhalation.   PreCheckCondition() looks
//      ahead to determine if the patient trigger is going to fire on the
//      current BD cycle.  The ModeChangeTrigger uses this information to
//      determine whether the mode change should be phased in early.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the trigger condition is met
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
E_Bool PtInhFlowTrigger::PreCheckCondition (void)
{
    E_Bool rtnValue = eFalse;       // return value; indicates if trigger fired

    return(rtnValue);

}   // end PreCheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This operation is invoked to enable the PtInhFlowTrigger using the
//      operator set flow sensitivity as the trigger sensitivity.  This
//      operation sets the private attribute IsInUnrestrictedPhase to FALSE.
//      It then invokes the GetCurrentValue() operation of MgrBdSetting to 
//      retrieve the operator setting for flow sensitivity.  It updates the
//      FlowLimit attribute by subtracting the flow sensitivity from 0 and
//      invokes the base class Trigger::EnableTrigger() method.
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
void PtInhFlowTrigger::EnableTrigger (void)
{
    // When the trigger is first enabled, the restricted phase of exhalation
    // is active.  
    InUnrestrictedPhase = eFalse;

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
void PtInhFlowTrigger::PhaseInSettingChange (LONG value)
{
    // If the trigger is enabled, update the trigger sensitivity and the
    // bias flow
    if(eTrue == IsEnabled)
    {
        FlowLimit = 0 - value;

    }

}   // end PhaseInSettingChange()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: IMV_CheckCondition()
//
//    Processing: IMV checks condition
//
//
//    Input Parameters:
//
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
E_Bool PtInhFlowTrigger::IMV_CheckCondition()
{
    const unsigned int NINE_SEC = 9000;
    const unsigned int THREE_SEC = 3000;
    const unsigned int SIX_SEC = 6000;
    const unsigned int STEP = 2;
    LONG triggerBaseLine = 0;

    if(timerCnt < NINE_SEC)//9 seconds
    {
        timerCnt += STEP;
    }


    if(timerCnt <= THREE_SEC)//3 seconds
    {
        triggerBaseLine = (LONG)((LONG)Ptdata::S_GetInstance()->GetExhLeak()*SCALE/2);
    }
    else if(timerCnt <= SIX_SEC)//6 seconds
    {
        triggerBaseLine = (LONG)((LONG)Ptdata::S_GetInstance()->GetExhLeak()*SCALE/4);
    }
    else
    {
        triggerBaseLine = 0;
    }


    E_Bool triggerDidFire = eFalse;

    UNSIGNED timeInh =  MgrBreathData::S_GetInstance()->GetBdBreathDataPtr()->GetTimeInInh();
    if (ExhPhase::S_IsInUnrestrictedPhase(timeInh) == eTrue)
    {
        LONG flow = ProxyFlowSensor::S_GetInstance()->GetLastReading();
        if(flow > MgrBdSetting::S_GetInstance()->GetCurrentValue(eFlowSens)+triggerBaseLine)
        {
            triggerDidFire = eTrue;
        }
    }
    return triggerDidFire;
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: APRV_CheckCondition()
//
//    Processing: APRV checks condition
//
//
//    Input Parameters:
//
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
E_Bool PtInhFlowTrigger::APRV_CheckCondition()
{
    E_Bool triggerDidFire = eFalse;
    E_PhaseId currentPhase = PhaseMgr::S_GetInstance()->GetCurrentPhaseId();

    UNSIGNED timeInh = 0;
    E_Bool IsInUnrestrictedPhase = eFalse;

    if(currentPhase == eAPRVHighPhase)
    {
        timeInh = MgrBreathData::S_GetInstance()->GetBdBreathDataPtr()->GetTimeInExh();
        IsInUnrestrictedPhase = APRV_PHighPhase::S_IsInUnrestrictedPhase(timeInh);
    }
    else if(currentPhase == eAPRVExhPhase_InPHigh)
    {
        timeInh = APRV_SupportedInhPhase_InPHigh::GetTimeElapsed();
        IsInUnrestrictedPhase = APRV_ExhPhase_InPHigh::S_IsInUnrestrictedPhase(timeInh);
    }
    else if(currentPhase == eAPRVLowPhase)
    {
        timeInh = 500;
        IsInUnrestrictedPhase = APRV_PLowPhase::S_IsInUnrestrictedPhase(timeInh);
    }
    else if(currentPhase == eAPRVExhPhase_InPLow)
    {
        timeInh = APRV_SupportedInhPhase_InPLow::GetTimeElapsed();
        IsInUnrestrictedPhase = APRV_ExhPhase_InPLow::S_IsInUnrestrictedPhase(timeInh);
    }


    LONG triggerBaseLine = 0;
    if(timerCnt<9000)//9 seconds
    {
        timerCnt += 2;
    }


    if(timerCnt<=3000)//3 seconds
    {
        triggerBaseLine = (LONG)((LONG)Ptdata::S_GetInstance()->GetExhLeak()*SCALE/2);
    }
    else if(timerCnt<=6000)//6 seconds
    {
        triggerBaseLine = (LONG)((LONG)Ptdata::S_GetInstance()->GetExhLeak()*SCALE/4);
    }
    else
    {
        triggerBaseLine = 0;
    }

    if (IsInUnrestrictedPhase == eTrue)
    {
        if(ProxyFlowSensor::S_GetInstance()->GetLastReading() > MgrBdSetting::S_GetInstance()->GetCurrentValue(eFlowSens)+triggerBaseLine)
        {
            triggerDidFire = eTrue;
        }
    }
    return triggerDidFire;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DisableTrigger()
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
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void PtInhFlowTrigger::DisableTrigger(void)
{

    // When the trigger is first enabled, the restricted phase of exhalation
    // is active.
    InUnrestrictedPhase = eFalse;

    // Invoke Base Class Enable Method
    Trigger::DisableTrigger();

    timerCnt = 0;
}   // end EnableTrigger()
