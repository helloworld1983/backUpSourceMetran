/******************************************************************************/
//$COMMON.CPP$
//   File Name:  PCV_SIMV_Mode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:
//      The operations in this class define the PCV_CPAP_Mode mode
//
//   Interfaces:
//
//   Restrictions:
//      There is only 1 object instantiated from this class.
/******************************************************************************/
#include "PCV_SIMV_Mode.h"
#include "PhaseMgr.h"
#include "MgrBdSetting.h"
#include "MgrController.h"
#include "TriggerMgr.h"
#include "AlarmTask.h"
#include "CommonTypes.h"

PCV_SIMV_Mode* PCV_SIMV_Mode::S_Instance = NULL;

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
PCV_SIMV_Mode* PCV_SIMV_Mode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new PCV_SIMV_Mode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PCV_SIMV_Mode()
//
//    Processing:
//
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
PCV_SIMV_Mode::PCV_SIMV_Mode(void): MixedMode()
{
    IsSynOn = false;
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter(E_VentilationMode)
//
//    Processing:
//
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
void PCV_SIMV_Mode::Enter(E_VentilationMode fromModeId)
{
    IsSynOn = false;
    MixedMode::Enter(fromModeId);
    (PhaseMgr::S_GetInstance())->ProcessNewVentType(ePressureVentilation);
    if(fromModeId != eApneaBackupMode)
    {
        MgrController::CMVServoController->ResetIMVStaticVariables();
    }
    MgrController::CMVServoController->Enable();
    PhaseMgr::S_GetInstance()->ProcessNewPhase(eExhPhase);

}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessPtInhTrigger()
//
//    Processing:
//
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
void PCV_SIMV_Mode::ProcessPtInhTrigger(void)
{
    // Initialize pointer to the PhaseMgr object
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();
    MgrBdSetting* mgrBdSetting = MgrBdSetting::S_GetInstance();

    // A patient trigger should only fire during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Start a new inhalation.  The type of inhalation (mand or
    // spont) depends upon the window that is currently
    // open.  If the mandatory window is open, deliver a mand
    // breath and close the mandatory window since this breath
    // will meet the mandatory breath criteria for this breath
    // period
    //    TriggerMgr::S_GetInstance()->DisableTrigger(eBpTimeTrigger);
    if (eMandWindow == Window)
    {
        // Start a mandatory inhalation
        if(eSwitchTypeOn == mgrBdSetting->GetCurrentValue(eSyncPlus))
        {
            phaseMgrPtr->ProcessNewPhase(eSpontInhPhase);
            IsSynOn = true;
            TriggerMgr::S_GetInstance()->DisableTrigger(eBpTimeTrigger);
        }
        else
        {
            phaseMgrPtr->ProcessNewPhase(eMandInhPhase);
        }

        // Close the mandatory window; open the spontaneous window.
        Window = eSpontWindow;
    }

    else if (eSpontWindow == Window)
    {
        // If the spontaneous window is open, deliver a spontaneous
        // inhalation.  The spontaneous window remains open until the
        // breath period timer elapses.
        phaseMgrPtr->ProcessNewPhase(eSpontInhPhase);
        if(eSwitchTypeOn == mgrBdSetting->GetCurrentValue(eSyncPlus))
        {
            IsSynOn = true;
            TriggerMgr::S_GetInstance()->DisableTrigger(eBpTimeTrigger);
        }
    }
    else
    {
        // Window contains illegal value
        ASSERTION((Window == eMandWindow) || (Window == eSpontWindow));
    }

}   // end ProcessPtInhTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessPtExhTrigger()
//
//    Processing:
//
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
void PCV_SIMV_Mode::ProcessPtExhTrigger()
{
    AlarmTask::S_GetInstance()->SetIsActive(eITimeTooLong, eNotActive);
    //    EndSpontInhalation();

    if(IsSynOn)
    {
        UNSIGNED breathPeriod = TriggerMgr::S_GetInstance()->GetTimePeriod(eBpTimeTrigger);
        breathPeriod = breathPeriod - MgrBdSetting::S_GetInstance()->GetCurrentValue(eInhTime)*10;

        TriggerMgr::S_GetInstance()->EnableTrigger(eBpTimeTrigger,breathPeriod);
    }
    IsSynOn = false;
}
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
void PCV_SIMV_Mode::ProcessBpTimeTrigger(void)
{
    MgrBdSetting* mgrBdSetting = MgrBdSetting::S_GetInstance();
    E_PhaseId phaseId;          // Current Phase ID
    PhaseMgr* phaseMgrPtr;      // Pointer to the PhaseMgr object
    phaseMgrPtr = PhaseMgr::S_GetInstance();
    switch (Window)
    {
        case eMandWindow:
            // Make sure the ventilator is in the exhalation phase.
            phaseId = phaseMgrPtr->GetCurrentPhaseId();
            ASSERTION(phaseId == eExhPhase);
            phaseMgrPtr->ProcessNewPhase(eMandInhPhase);
            break;

        case eSpontWindow:
            // If the breath period fires while the spontaneous window is open,
            // the ventilator just re-opens the mandatory window.
            Window = eMandWindow;

            if(eSwitchTypeOn == mgrBdSetting->GetCurrentValue(eSyncPlus))
            {
                phaseMgrPtr->ProcessNewPhase(eMandInhPhase);
            }
            break;

        default:
            // Trigger fired in wrong window
            ASSERTION((Window == eSpontWindow) || (Window == eMandWindow));
            break;
    }

}   // end ProcessBpTimeTrigger()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of PCV_SIMV_Mode
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
void PCV_SIMV_Mode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
//        case ePtInhTrigger:
//            ProcessPtInhTrigger();
//            break;
//
//        case ePtExhTrigger:
//            ProcessPtExhTrigger();
//            break;

        case eBpTimeTrigger:
            ProcessBpTimeTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}
