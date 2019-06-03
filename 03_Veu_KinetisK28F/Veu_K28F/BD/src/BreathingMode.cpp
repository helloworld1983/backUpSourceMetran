/******************************************************************************/
//$COMMON.CPP$
//   File Name:  BreathingMode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of methods to process triggers common
//      to all breathing modes.  The BreathingMode class is derived from the
//      mode base class and re-defines several virtual methods that handle
//      trigger processing.
//
//   Interfaces:
//      Operations in this class interface with the PhaseMgr object to 
//      determine the current phase by calling GetCurrentPhaseId().  Operations
//      also update the phase by calling PhaseMgr::ProcessNewPhase().
//      The ProcessManualInhOpReqTrigger() operation also interfaces with the
//      TriggerMgr object to re-enable the breath period trigger when the 
//      a manual inhalation is triggered.
//
//   Restrictions:
//      This class is an abstract base class.  It contains pure virtual 
//      methods and cannot be instantiated.
/******************************************************************************/
#include <ModeMgr.h>
#include <PhaseMgr.h>
#include "BreathingMode.h"
#include "TriggerMgr.h"

#include "DebugTrace.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: BreathingMode()
//
//    Processing: 
//      This operation is the constructor.  It invokes the Mode base class.
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
BreathingMode::BreathingMode (E_Bool doesManualReset) :
        Mode (doesManualReset)
{

}   // end BreathingMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessVcvInhTimeTrigger()
//
//    Processing: 
//      This operation handles the processing of the VCV Inhalation Time
//      Trigger for all Breathing Modes.
//  
//      This operation retrieves the current phase ID from the PhaseMgr.  If
//      the breath phase is the mandatory inhalation phase, this operation
//      invokes the ProcessNewPhase() operation of the PhaseMgr to transition
//      the ventilator to the Plateau Phase.  It is always assumed that the
//      ventilator will transition from a VCV inhalation to a plateau.  
//      The Phase Manager will determine if that is actually the case based
//      upon the operator setting for plateau time.
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
//      If the phase is not a mandatory inhalation phase, error handling is
//      performed since the VCV Inhalation time trigger should only fire in
//      VCV mandatory inhalations.
//
//    Requirements:  
//
/******************************************************************************/
void BreathingMode::ProcessVcvInhTimeTrigger (void)
{
    // Get a handle to the PhaseMgr
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // The trigger should only fire in the mandatory inhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eMandInhPhase);

    // Assume that the ventilator will transition to plateau
    // from a VCV inhalation.  It is possible that the ventilator
    // will not go to plateau but instead will go to exhalation.
    // The phase manager takes care of that.
    phaseMgrPtr->ProcessNewPhase (ePlateauPhase);

}   // end ProcessVcvInhTimeTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessMandPressInhTimeTrigger()
//
//    Processing: 
//      This operation handles the processing of the Mandatory Pressure 
//      Inhalation Time Trigger for all Breathing Modes. 
//      The MandPressInhTimeTrigger is the trigger that fires when the 
//      operator set inhalation time expires during a pressure based 
//      mandatory inhalation.
//  
//      This operation retrieves the current phase ID from the PhaseMgr.  If
//      the breath phase is the mandatory inhalation phase, this operation
//      invokes the ProcessNewPhase() operation of the PhaseMgr to transition
//      the ventilator to the Exhalation Phase.  
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
//      If the phase is not a mandatory inhalation phase, error handling is
//      performed since this trigger trigger should only fire in pressure
//      based mandatory inhalations.
//
//    Requirements:  
//
/******************************************************************************/
void BreathingMode::ProcessMandPressInhTimeTrigger (void)
{
    // Get a pointer to the PhaseMgr object
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // If the breath phase is a mandatory inhalation, transition to exhalation.
    // Otherwise, perform error handling.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eMandInhPhase);
    phaseMgrPtr->ProcessNewPhase (eExhPhase);

}   // end ProcessMandPressInhTimeTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessHipAlarmTrigger()
//
//    Processing: 
//      This operation handles the processing for the High Inhalation Pressure-1
//      and High Inhalation Pressure-2 Alarm Triggers for all Breathing Modes.
//
//      A High Inhalation Pressure Trigger fires when the measured pressure 
//      exceeds the operator set HIP limit during the inhalation or plateau 
//      phase of a breath.
//  
//      This operation retrieves the current phase ID from the PhaseMgr.  If
//      the breath phase is inhalation or plateau, this operation
//      invokes the ProcessNewPhase() operation of the PhaseMgr to transition
//      the ventilator to the Exhalation Phase.  
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
//      If the phase is not inhalation or plateau, error handling is
//      performed since this trigger should only fire in those phases.
//
//    Requirements:  
//
/******************************************************************************/
void BreathingMode::ProcessHipAlarmTrigger (void)
{
    // Get a pointer to the PhaseMgr
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Get the current breath phase to be used to check that triggers only
    // fire when they are supposed to fire
    E_PhaseId breathPhase = phaseMgrPtr->GetCurrentPhaseId ();

    // Make sure the ventilator is in inhalation or plateau
    if((eMandInhPhase == breathPhase) || (eSpontInhPhase == breathPhase)
            || (ePlateauPhase == breathPhase))
    {
        phaseMgrPtr->ProcessNewPhase (eExhPhase);
    }
    else
    {
        // HIP Trigger in wrong phase
        ASSERTION(eFalse);
    }

}   // end ProcessHipAlarmTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessPlateauTimeTrigger()
//
//    Processing: 
//      This operation handles the processing of the Plateau Time Trigger for
//      all Breathing Modes.  A Plateau Time Trigger fires when the timer
//      associated with the plateau period times out.
//  
//      This operation retrieves the current phase ID from the PhaseMgr.  If
//      the breath phase is plateau, this operation invokes the 
//      ProcessNewPhase() operation of the PhaseMgr to transition
//      the ventilator to the Exhalation Phase.  
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
//      If the phase is not plateau, error handling is performed
//      since this trigger should only fire in that phase.
//
//    Requirements:  
//
/******************************************************************************/
void BreathingMode::ProcessPlateauTimeTrigger (void)
{
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Plateau Time Trigger is only valid in the Plateau Phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == ePlateauPhase);

    // Transition to the exhalation phase.
    phaseMgrPtr->ProcessNewPhase (eExhPhase);

}   // end ProcessPlateauTimeTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessManualInhOpReqTrigger()
//
//    Processing: 
//      This operation handles the processing of the Manual Inhalation Operator 
//      Request trigger for all Breahing Modes.  A Manual Inhalation Operator
//      Operator Request Trigger fires when the operator requests a manual 
//      inhalation in the unrestricted phase of exhalation.
//  
//      This operation retrieves the current phase ID from the PhaseMgr.  If
//      the breath phase is exhalation, this operation invokes the 
//      ProcessNewPhase() operation of the PhaseMgr to transition
//      the ventilator to the Mandatory Inhalation Phase.  This operation also
//      interfaces with the TriggerMgr to re-enable the breath period
//      triggers when the manual inhalation trigger fires.
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
//      If the phase is not exhalation, error handling is performed
//      since this trigger should only fire in that phase.
//
//    Requirements:  
//
/******************************************************************************/
void BreathingMode::ProcessManualInhOpReqTrigger (void)
{
}   // end ProcessManualInhOpReqTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTotalGasLossTrigger()
//
//    Processing: 
//      This operation handles the processing of the Total Gas Loss Trigger for
//      all Breathing Modes.   It calls ModeMgr::GoToAlarmMode() to transition 
//      to the non-ventilation mode.
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
void BreathingMode::ProcessTotalGasLossTrigger (void)
{

    // Transition to the No Gas Mode.
    (ModeMgr::S_GetInstance ())->GoToAlarmMode (eNoGasMode);

}   // end ProcessTotalGasLossTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessOcclusionAlarmTrigger()
//
//    Processing: 
//      This operation handles the processing of the Occlusion Alarm Trigger
//      for all Breathing Modes.
//  
//      This operation invokes ModeMgr::GoToAlarmMode() to transition to the 
//      Occlusion Mode.
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
void BreathingMode::ProcessOcclusionAlarmTrigger (void)
{

    // Transition to the occlusion mode.
    (ModeMgr::S_GetInstance ())->GoToAlarmMode (eOcclusionMode);

}   // end ProcessOcclusionAlarmTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of Breathing Mode
//
//    Input Parameters:
//      E_TriggerId id
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
//
//    Requirements:
//
/******************************************************************************/
void BreathingMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eVcvInhTimeTrigger:
            ProcessVcvInhTimeTrigger();
            break;

        case eMandPressInhTimeTrigger:
            ProcessMandPressInhTimeTrigger();
            break;

        case ePlateauTimeTrigger:
            ProcessPlateauTimeTrigger();
            break;

        case eManualInhOpReqTrigger:
            ProcessManualInhOpReqTrigger();
            break;

        case eHipAlarmTrigger:
            ProcessHipAlarmTrigger();
            break;

        case eTotalGasLossTrigger:
            ProcessTotalGasLossTrigger();
            break;

//        case eOcclusionAlarmTrigger:
//            ProcessOcclusionAlarmTrigger();
//            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessDisconnectionAlarmTrigger()
//
//    Processing:
//      This operation handles the processing of the Disconnect Alarm Trigger
//      for all Breathing Modes.
//
//      This operation invokes TriggerMgr::DisableTrigger to disable the
//        DisconnectAlarmTrigger and ModeMgr::GoToAlarmMode() to transition to the
//        Disconnect Mode.
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
//
//    Requirements:
//
/******************************************************************************/
void BreathingMode::ProcessDisconnectionAlarmTrigger(void)
{
    // Transition to Disconnect Mode
    (ModeMgr::S_GetInstance())->GoToAlarmMode(eDisconnectionMode);

}   // end ProcessDisconnectionAlarmTrigger()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessHardwareErrorAlarmTrigger()
//
//    Processing:   Process doing something when hardware is error
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
//
//    Requirements:
//
/******************************************************************************/
void BreathingMode::ProcessHardwareErrorAlarmTrigger(void)
{
    (ModeMgr::S_GetInstance())->GoToAlarmMode(eHardwareErrorMode);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter()
//
//    Processing:   Process doing something when exit from this mode
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
//
//
//    Requirements:
//
/******************************************************************************/
//void BreathingMode::Exit (void)
//{
//}
