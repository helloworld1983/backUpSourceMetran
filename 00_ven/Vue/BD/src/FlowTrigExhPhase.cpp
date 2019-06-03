/******************************************************************************/
//$COMMON.CPP$
//   File Name:  FlowTrigExhPhase.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file defines the Exhalation Phase with Flow Triggering class.
//      The Flow Triggering Exhalation Phase is active during exhalation
//      when the operator setting for Trigger Type is Flow.
//
//      This class is derived from the ExhPhase base class.
//
//   Interfaces:
//      The Start() operation in this class interfaces with the TriggerMgr
//      to enable triggers specific to a flow triggering exhalation phase.
//      The Start() operation also calls MgrBdSetting::GetCurrentValue() to
//      retrieve the current flow sensitivity setting value.
//
//      The ProcessBdCycle() operation calls Exponential::GetExpTerm() and
//      O2Setting::CalculateAirO2Flows() to determine the desired bias flow
//      and the split of that flow between the gas sources for the current
//      BD Cycle.  The AirFlowController and O2FlowController are notified of
//      the desired flows via the operation SetDesired() every BD Control cycle.
//
//      Operations in this class also invoke methods in the base class ExhPhase
//      to perform processing common to all exhalation phases.
//
//   Restrictions:
//      There is only one instance of this class.
/******************************************************************************/
#include <DeviceInterface.h>
#include "ProxyFlowSensor.h"
#include "FlowTrigExhPhase.h"
#include "TriggerMgr.h"
#include "MgrBdSetting.h"
#include "Exponential.h"
#include "O2Setting.h"
#include "ModeMgr.h"
#include "MgrFlowController.h"

// Define statics
FlowTrigExhPhase* FlowTrigExhPhase::S_Instance = NULL;

// Rise time to the targeted bias flow.  At the start of exhalation the
// flow is 0.  Flow then exponentially rises to the targeted Bias flow
// using the BIAS_FLOW_RISE_TIME which is 200ms.
static const UNSIGNED BIAS_FLOW_RISE_TIME = 200 / MS_PER_TICK;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: FlowTrigExhPhase()
//
//    Processing: 
//      This operation is the constructor for the FlowTrigExhPhase.  It invokes 
//      the base class ExhPhase constructor.
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
FlowTrigExhPhase::FlowTrigExhPhase (void) :
        ExhPhase ()
{

}   // end FlowTrigExhPhase()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing: 
//      This operation is invoked at the start of each exhalation phase while
//      flow triggering is active.  This operation invokes the base class
//      ExhPhase::Start() to perform processing common to all exhalation 
//      phases.  It then invokes the EnableTrigger() operation of the 
//      TriggerMgr object to enable the Patient Inhalation Flow Trigger and
//      the Patient Inhalation Pressure Trigger.  The pressure trigger is
//      enabled while flow triggering is active as a backup triggering
//      mechanism.
//
//      Finally, this operation updates the BiasFlow attribute by retrieving
//      the flow sensitvity by calling MgrBdSetting::GetCurrentValue() and 
//      adding the bias flow offset to the flow sensitivity.
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
void FlowTrigExhPhase::Start (void)
{
    // Get a pointer to the TriggerMgr object
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    ExhPhase::Start();

    E_VentilationMode currentMode = ModeMgr::S_GetInstance()->GetCurrentModeId();
    if(currentMode == eNivMode)
    {
        triggerMgrPtr->DisableTrigger(ePtInhFlowTrigger);
        return;
    }

    ProxyStatus Status = ProxyFlowSensor::S_GetInstance()->getLastStatus();

    if((Status==eCalibrated)||(Status==eResetAndRecovered))
    {
        triggerMgrPtr->DisableTrigger(ePtInhPressTrigger);
        triggerMgrPtr->EnableTrigger(ePtInhFlowTrigger);
    }
    else
        triggerMgrPtr->EnableTrigger(ePtInhPressTrigger);

}   // end Start()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing: 
//      This operation is invoked at the end of an exhalation phase when 
//      flow triggering is active.  This operation calls the base class
//      ExhPhase::End() operation to perform processing common to all
//      exhalation phases.  This operation then calls the DisableTrigger()
//      operation of the TriggerMgr object to disable the Patient Inhalation 
//      Flow Trigger.
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
void FlowTrigExhPhase::End (void)
{
    // Invoke the base class method to perform processing common to all
    // exhalation phases.
    ExhPhase::End();
    // Get a pointer to the TriggerMgr object
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    // Disable the Patient Inhalation Flow Trigger; the Patient Inhalation
    // Pressure Trigger was disabled in ExhPhase::End()
    triggerMgrPtr->DisableTrigger(ePtInhFlowTrigger);
    triggerMgrPtr->DisableTrigger(ePtInhPressTrigger);

}   // end End()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBdCycle()
//
//    Processing: 
//      This operation is invoked every BD Cycle during an exhalation phase
//      with flow triggering active.  This operation first calls the base
//      class ExhPhase::ProcessBdCycle() operation.  It then uses the 
//      ExhPhase::S_TimeElapsed value in a call to Exponential::GetExpTerm() 
//      to determine the exponential part of the equation shown below. 
//
//      Next this operation determines the desired flow for this cycle
//      using the following equation:
//
//      biasFlow = BiasFlow * (1-expTerm),
//  
//      where the exponential term is defined as
//
//                 -3(t-Ti+Tc)/BIAS_FLOW_RISE_TIME
//      expTerm = e
//
//      where:
//          t - Ti = time elapsed since the start of exhalation
//          Tc = control cycle of the flow controller.
//
//      Once the total desired flow has been determined, O2Setting::
//      CalculateAirO2Flows() is called to determine the split between the 
//      Air and O2 sources.  The SetDesired() operations of AirFlowController
//      and O2FlowController are then called to update the flow commands. 
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
void FlowTrigExhPhase::ProcessBdCycle (void)
{

    LONG desiredAir = 0;    // desired air flow
    LONG desiredO2 = 0;     // desired O2 flow
//    LONG biasFlow ;      // desired bias flow
//    UNSIGNED timeElapsed;   // time elapsed since start of exhalation in ticks
//    LONG expTerm;       // exponential term

    // invoke the base class method
//    ExhPhase::ProcessBdCycle ();

    // Determine the time elapsed since the start of the exhalation phase;
    // offset this value by 1 flow control cycle.   Air and O2 use the same
    // control cycle so it does not matter which one is used.
//    timeElapsed = ExhPhase::S_TimeElapsed + AirO2Task->GetSamplePeriod();

    // Calculate the exponential term of the equation based upont the rise
    // time and the time elapsed since the start of exhalation.
//    expTerm = Exponential::GetExpTerm (BIAS_FLOW_RISE_TIME, timeElapsed);

    // biasFlow starts at 0 at the start of the exhalation phase and 
    // exponentially rises to the targeted bias flow.  
//    biasFlow = (BiasFlow * (EXP_DATA_SF - expTerm)) >> EXP_DATA_SHIFT;

    // Determine the air/oxygen split of the bias flow for this control cycle
//    (O2Setting::S_GetInstance ())->CalculateAirO2Flows (biasFlow, &desiredAir,
//                                                        &desiredO2);

    // Update the air and O2 flow controllers
    MgrFlowController::AirFlowController->SetDesired(desiredAir);
    MgrFlowController::O2FlowController->SetDesired(desiredO2);

}   // end ProcessBdCycle()

