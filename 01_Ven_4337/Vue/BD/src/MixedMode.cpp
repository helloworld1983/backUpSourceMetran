/******************************************************************************/
//$COMMON.CPP$
//   File Name:  MixedMode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      The operations in this class define the Mixed mode of
//      ventilation.  The Mixed mode is SIMV in Pressure or Volume Ventilation
//      and Spontaneous/Timed mode in Non-Invasive ventilation.  This mode
//      is a mixed mandatory/spontaneous mode of ventilation.
//
//   Interfaces:
//      Operations in this class interface with the TriggerMgr to enable,
//      re-enable and disable triggers via EnableTrigger(), ReEnableTrigger()
//      and DisableTrigger().
//
//      Operations in this class also call PhaseMgr::GetCurrentPhaseId() to
//      determine the current breath phase and PhaseMgr::ProcessNewPhase() to
//      change the breath phase.  
//
//      MixedMode::Enter() calls InhPhase::S_CalcTimeSincePhaseStart() to 
//      determine the time elasped since the start of the last inhalation.
//
//   Restrictions:
//      There is only 1 object instantiated from this class.
/******************************************************************************/
#include <InhPhase.h>
#include <PhaseMgr.h>
#include "MixedMode.h"
#include "TriggerMgr.h"
#include "DebugTrace.h"

// Define statics
MixedMode* MixedMode::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the MixedMode object, it invokes MixedMode::S_GetInstance(),
//      which returns a pointer to the MixedMode object.
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
//      MixedMode* - pointer to the object instantiated from this class
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
MixedMode* MixedMode::S_GetInstance (void)
{
    // If the pointer to the instance is NULL, create the object.
    if(NULL == S_Instance)
    {
        S_Instance = new MixedMode ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MixedMode()
//
//    Processing: 
//      This operation is the constructor for the MixedMode.  It invokes
//      the base class constructor.
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
//      None
//
/******************************************************************************/
MixedMode::MixedMode (void) :
        NormBreathingMode ()
{
    Window = eMandWindow;
}   // end MixedMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter()
//
//    Processing: 
//      This operation is invoked when the Mixed mode is entered from
//      any other mode in the system.  
//
//      When this operation is called TriggerMgr::EnableTrigger() is called to
//      enable the Mode Change Trigger and the Apnea Alarm Time Trigger.  The 
//      Window attribute is set to eMandWindow.  The remainder of the processing 
//      depends upon the mode being exited in order to enter a Mixed mode.
//
//      If the Mixed mode is being entered from the Start-Up or Non-Vent Mode, 
//      PhaseMgr::ProcessNewPhase() is called to start a Mandatory Inhalation
//      and TriggerMgr::EnableTrigger() is called to start the Breath Period
//      Time Trigger for the operator set breath period.
//
//      If Mixed Mode is being entered from any other valid mode, 
//      InhPhase::S_CalcTimeSincePhaseStart() is invoked to determine the time
//      elapsed since the start of the last inhalation and 
//      TriggerMgr::GetTimePeriod() is called to retrieve the set breath 
//      period from the BpTimeTrigger Object.  If the time elapsed since the
//      start of the last inhalation is greater than the breath period,
//      PhaseMgr::ProcessNewPhase() is called to start a Mandatory Inhalation
//      and TriggerMgr::EnableTrigger() is called to start the Breath Period
//      Time Trigger for the operator set breath period.
//
//      Otherwise, EnableTrigger() is called to enable the Breath Period Time
//      Trigger for the time difference between the breath period and the time
//      since the start of the last inhalation.
//
//    Input Parameters: 
//      fromModeId - ID of the Mode that is being exited so that A/C can be
//          entered
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Miscellaneous:
//      If the fromModeId is not one of the expected modes, error handling
//      is performed.  If the Mixed mode is entered from any mode except 
//      Start-Up, the breath phase is expected to be exhalation; 
//      if not, error handling is performed.
//
//    Requirements:  
//      None
//
/******************************************************************************/
void MixedMode::Enter (E_VentilationMode fromModeId)
{

    UNSIGNED timeSinceInhStart;     // time elaspsed since start of last inh
    UNSIGNED breathPeriod;          // current breath period

    Window = eMandWindow;

    // Initialize pointers to the TriggerMgr object and the PhaseMgr object
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Enable triggers common to all breathing modes such as the Apnea Time
    // trigger and the Mode change trigger.
    UpdateCommonTriggers ();

    // Enable the exhalation hold request trigger.
//    triggerMgrPtr->EnableTrigger (eExhHoldOpReqTrigger);

    // Set the window to mandatory.  Any breath triggered while the Mandatory
    // window is open will be a mandatory breath.
    Window = eMandWindow;

    // Determine processing based upon the mode that is being exited
    switch (fromModeId)
    {
        case eStandbyMode:
        case eOcclusionMode:
        case eNoGasMode:

            // When entering the SIMV or Spont/Timed Mode from Start-Up,
            // go right into a mandatory inhalation.  
            phaseMgrPtr->ProcessNewPhase (eMandInhPhase);

            // Enable the Breath Period time trigger.  Since a time interval
            // is not passed to the EnableTrigger() method, the trigger will 
            // enable the breath period timer based on the current respiratory 
            // rate setting
            triggerMgrPtr->EnableTrigger (eBpTimeTrigger);

            break;

        case eSpontaneousMode:
        case eApneaMode:
        case eMixedMode:

            // The mode change should only occur in exhalation.
            ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

            // Determine the time elasped since the start of the last 
            // inhalation phase
            timeSinceInhStart = InhPhase::S_CalcTimeSincePhaseStart ();

            // The value associated with the Breath Period Time Trigger is
            // the breath period.  Retrieve it.
            breathPeriod = triggerMgrPtr->GetTimePeriod (eBpTimeTrigger);

            // When entering the Mixed Mode from any breathing mode, 
            // determine the time elapsed since last inhalation.  
            if(timeSinceInhStart >= breathPeriod)
            {
                // If the time elapsed exceeds the breath period, transition 
                // to inhalation right away.  
                phaseMgrPtr->ProcessNewPhase (eMandInhPhase);

                // Enable the Breath Period time trigger.  Since no 
                // parameters are being passed, the trigger will enable 
                // the breath period timer based on the current respiratory 
                // rate setting
                triggerMgrPtr->EnableTrigger (eBpTimeTrigger);
            }
            else
            {
                // If the time elapsed since the start of the last 
                // inhalation is less than the breath period, enable the 
                // trigger for the difference between the breath period and
                // the time elapsed since the start of the last inhlation.
                triggerMgrPtr->EnableTrigger (
                        eBpTimeTrigger, (breathPeriod - timeSinceInhStart));
            }

            break;

        default:
            // Illegal mode
            ASSERTION(eFalse);
            break;
    }

}   // end Enter()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessPtInhTrigger()
//
//    Processing: 
//      This operation handles the processing of the Patient Inhalation Trigger
//      for the Mixed Mode.
//
//      This operation retrieves the current phase from the PhaseMgr object.
//      If the current phase is exhalation, the Window attribute is checked.
//      If the mandatory window is open, PhaseMgr::ProcessNewPhase() 
//      is called to transition the ventilator to a mandatory inhalation, and 
//      the mandatory window is closed.  
//
//      If the mandatory window is not open, ProcessNewPhase() is called to 
//      transition the ventilator to a spontaneous inhalation.
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
//      If the breath phase is not exhalation or if the Window attribute 
//      contains an illegal value, error handling is performed.
//
//    Requirements:  
//
/******************************************************************************/
void MixedMode::ProcessPtInhTrigger (void)
{

    // Initialize pointer to the PhaseMgr object
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // A patient trigger should only fire during the exhalation phase.  
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Start a new inhalation.  The type of inhalation (mand or
    // spont) depends upon the window that is currently
    // open.  If the mandatory window is open, deliver a mand
    // breath and close the mandatory window since this breath
    // will meet the mandatory breath criteria for this breath
    // period
    if(eMandWindow == Window)
    {
        // Start a mandatory inhalation
        phaseMgrPtr->ProcessNewPhase (eMandInhPhase);

        // Close the mandatory window; open the spontaneous window.
        Window = eSpontWindow;
    }

    else if(eSpontWindow == Window)
    {
        // If the spontaneous window is open, deliver a spontaneous 
        // inhalation.  The spontaneous window remains open until the 
        // breath period timer elapses.
        phaseMgrPtr->ProcessNewPhase (eSpontInhPhase);
    }
    else
    {
        // Window contains illegal value
        ASSERTION((Window == eMandWindow) || (Window == eSpontWindow));
    }

}   // end ProcessPtInhTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBpTimeTrigger()
//
//    Processing: 
//      This operation handles the processing of the Breath Period Time Trigger
//      for the Mixed Mode.
//
//      This operation first checks the Window attribute to determine if the
//      Mandatory window is open.  If so, the current phase is retrieved from
//      the PhaseMgr object.  If the ventilator is in exhalation,
//      PhaseMgr::ProcessNewPhase() is called to transition 
//      the ventilator to a mandatory inhalation.  
//
//      If the spontaneous window is opened when this operation is called,
//      the Window attribute is set to eMandWindow to open the mandatory 
//      window.
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
//      If the Window attribute contains an illegal value, error handling is
//      performed.  If the mandatory window is open when this operation is
//      called and the breath phase is not exhalation, error handling is also
//      performed.  
//
//    Requirements:  
//
/******************************************************************************/
void MixedMode::ProcessBpTimeTrigger (void)
{

    E_PhaseId phaseId;          // Current Phase ID
    PhaseMgr* phaseMgrPtr;      // Pointer to the PhaseMgr object

    switch (Window)
    {
        case eMandWindow:
            // Initialize pointer to the PhaseMgr object
            phaseMgrPtr = PhaseMgr::S_GetInstance ();

            // Make sure the ventilator is in the exhalation phase.
            phaseId = phaseMgrPtr->GetCurrentPhaseId ();
            ASSERTION(phaseId == eExhPhase);
            phaseMgrPtr->ProcessNewPhase (eMandInhPhase);
            break;

        case eSpontWindow:
            // If the breath period fires while the spontaneous window is open, 
            // the ventilator just re-opens the mandatory window.
            Window = eMandWindow;
            break;

        default:
            // Trigger fired in wrong window
            ASSERTION((Window == eSpontWindow) || (Window == eMandWindow));
            break;
    }

}   // end ProcessBpTimeTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessManualInhOpReqTrigger()
//
//    Processing: 
//      This operation handles the processing of the Manual Inhalation Operator
//      Request Trigger for the Mixed Mode.
//
//      This operation retrieves the current phase from the PhaseMgr object.
//      If the current phase is exhalation, PhaseMgr::ProcessNewPhase() is 
//      called to transition the ventilator to a mandatory inhalation, and the
//      mandatory window is closed.  
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
//      If the breath phase is not exhalation, error handling is performed.
//
//    Requirements:  
//
/******************************************************************************/
void MixedMode::ProcessManualInhOpReqTrigger (void)
{
    // Initialize pointer to the PhaseMgr object
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // This trigger should only fire during the exhalation phase.  
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Start a mandatory inhalation
    phaseMgrPtr->ProcessNewPhase (eMandInhPhase);

    // An operator initiated manual inhalation while the mand
    // window is open causes that window to close and the spont
    // window to open.  If the spont window is already open,
    // it remains open.
    Window = eSpontWindow;

}   // end ProcessManualInhOpReqTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessPtInhTriggerAC()
//
//    Processing: This operation handles the processing of Pt inhalation trigger
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
void MixedMode::ProcessPtInhTriggerAC (void)
{
    // Initialize pointer to the PhaseMgr object
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // A patient trigger should only fire during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);


    // Start a mandatory inhalation
    phaseMgrPtr->ProcessNewPhase(eMandInhPhase);

    // Close the mandatory window; open the spontaneous window.
    Window = eMandWindow;
    TriggerMgr::S_GetInstance()->DisableTrigger(eBpTimeTrigger);
    UNSIGNED breathPeriod = TriggerMgr::S_GetInstance()->GetTimePeriod(eBpTimeTrigger);

    TriggerMgr::S_GetInstance()->EnableTrigger(eBpTimeTrigger,breathPeriod);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessExhHoldTrigger()
//
//    Processing: 
//      This operation handles the processing of the Exhalation Hold Trigger 
//      for the Mixed Mode.  The Exhalation Hold Trigger fires when
//      either the BD receives a request from the GUI to stop the hold
//      (operator released the button) or a timeout occurred.
//  
//      This operation retrieves the current phase ID from the PhaseMgr.  If
//      the breath phase is exhalation hold, this operation invokes the 
//      ProcessNewPhase() operation of the PhaseMgr to transition
//      the ventilator to the Mandatory Inhalation Phase.  This operation
//      also enables the Breath Period Time Trigger since it was disabled when
//      The Exhalation Hold phase started.
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
//      If the phase is not exhalation hold, error handling is performed
//      since this trigger should only fire in that phase.
//
//    Requirements:  
//
/******************************************************************************/
void MixedMode::ProcessExhHoldTrigger (void)
{

    // Initialize pointer to the PhaseMgr object
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // This trigger should only occur in the exhalation hold phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhHoldPhase);

    // Transition to the mandatory inhalation phase since a mandatory 
    // inhalation was being held off to perform the hold.   
    (TriggerMgr::S_GetInstance ())->EnableTrigger (eBpTimeTrigger);
    phaseMgrPtr->ProcessNewPhase (eMandInhPhase);

}   // end ProcessExhHoldTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of MIx Mode
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
void MixedMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
//        case ePtInhTrigger:
//            ProcessPtInhTrigger();
//            break;

        case eBpTimeTrigger:
            ProcessBpTimeTrigger();
            break;

        case eManualInhOpReqTrigger:
            ProcessManualInhOpReqTrigger();
            break;

//        case eExhHoldTrigger:
//            ProcessExhHoldTrigger();
//            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetWindow(void)
//
//    Processing: Return this is main window or not
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
E_WindowType MixedMode::GetWindow(void)
{
    return Window;
}
