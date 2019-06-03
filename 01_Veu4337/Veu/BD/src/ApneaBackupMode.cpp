/******************************************************************************/
//$COMMON.CPP$
//   File Name:  ApneaBackupMode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:
//      The operations in this class define the ApneaBackupMode mode
//
//   Interfaces:
//
//   Restrictions:
//      There is only 1 object instantiated from this class.
/******************************************************************************/
#include "ApneaBackupMode.h"
#include "TriggerMgr.h"
#include "ModeMgr.h"
#include "AlarmTask.h"
#include "MgrBdSetting.h"
#include "MgrBreathData.h"
#include "PhaseMgr.h"
#include "DeviceInterface.h"

ApneaBackupMode* ApneaBackupMode::S_Instance = NULL;

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
ApneaBackupMode* ApneaBackupMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new ApneaBackupMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of ApneaBackupMode
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
void ApneaBackupMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
//        case ePtInhTrigger:
//            ProcessPtInhTrigger();
//            break;
//
//        case eAlarmResetTrigger:
//            ProcessAlarmResetTrigger();
//            break;

        case eManualInhOpReqTrigger:
            ProcessManualInhOpReqTrigger();
            break;

        case eMandPressInhTimeTrigger:
            ProcessMandPressInhTimeTrigger();
            break;


        case eVcvInhTimeTrigger:
            ProcessVcvInhTimeTrigger();
            break;

        case ePlateauTimeTrigger:
            ProcessPlateauTimeTrigger();
            break;

        case eBpTimeTrigger:
            ProcessBpTimeTrigger();
            break;

        case eApneaAlarmTimeTrigger:
            ProcessApneaAlarmTimeTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SpontMode()
//
//    Processing:
//      This operation is the constructor for the SpontMode.  It invokes
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
ApneaBackupMode::ApneaBackupMode(void):MixedMode()
{
    fromMode = eStandbyMode;
    IsPtInhTrigger = false;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter()
//
//    Processing:
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void ApneaBackupMode::Enter(E_VentilationMode fromModeId)
{
    switch(fromModeId)
    {
        case eHighPressMode:
        case eHardwareErrorMode:
            ModeMgr::S_GetInstance()->GoToOperatorSetMode();
            break;
        default:
            fromMode = fromModeId;
            IsPtInhTrigger = false;
            Window = eMandWindow;

            E_State TempState;
            TempState = eOn;
            DeviceInterface::S_GetInstance()->RequestDevice(eInpedanceLineSetStateeOn, &TempState);
            TempState = eOff;
            DeviceInterface::S_GetInstance()->RequestDevice(eInpedanceLineSetDesiredStateeOff, &TempState);
            TempState = eClose;
            DeviceInterface::S_GetInstance()->RequestDevice(eInpedanceLineSetDesiredStateeOff, &TempState);

            //Disable all trigger
            TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
            triggerMgrPtr->DisableAllTriggers();

            //Enable IMV trigger
            triggerMgrPtr->EnableIMVTrigger();

            //Disable eApneaAlarmTimeTrigger
            triggerMgrPtr->DisableTrigger(eApneaAlarmTimeTrigger);

            UNSIGNED breathPeriod = TriggerMgr::S_GetInstance()->GetTimePeriod(eBpBackupTimeTrigger);
            TriggerMgr::S_GetInstance()->EnableTrigger(eBpBackupTimeTrigger,breathPeriod);


            PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

            E_VentilationType currentVentType = phaseMgrPtr->GetCurrentVentType();

            if(ePressureVentilation == currentVentType)
            {
                DeviceInterface::S_GetInstance()->RequestDevice(eCMVServoControllerEnable, NULL);
            }
            else
            {
                DeviceInterface::S_GetInstance()->RequestDevice(eVCVServoControllerEnable, NULL);
            }

            phaseMgrPtr->ProcessNewPhase(eMandInhPhase);

            AlarmTask::S_GetInstance()->SetIsActive(eApnea, eActive);

            break;
    }
    //    AlarmSettingLogEntry entry;
    //    entry.strID = eSTR_LOG_BackupModeStart;
    //    entry.dateTimeStr = QDateTime::currentDateTime().toString(LOG_DATE_TIME_FORMAT);
    //    LogFileHandler::S_GetInstance()->postCustomEvent<AlarmSettingLogEntry>(LogFileHandler::eLogHandlerAlarmSettingLogFileType, entry);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Exit()
//
//    Processing:
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void ApneaBackupMode::Exit()
{
    AlarmTask::S_GetInstance()->SetIsActive(eApnea, eNotActive);

    MgrBreathData::S_GetInstance()->InitializeBreathDataPtr();
    DeviceInterface::S_GetInstance()->RequestDevice(eCMVServoControllerDisable, NULL);
    DeviceInterface::S_GetInstance()->RequestDevice(eVCVServoControllerDisable, NULL);

    //    AlarmSettingLogEntry entry;
    //    entry.strID = eSTR_LOG_BackupModeStop;
    //    entry.dateTimeStr = QDateTime::currentDateTime().toString(LOG_DATE_TIME_FORMAT);
    //    LogFileHandler::S_GetInstance()->postCustomEvent<AlarmSettingLogEntry>(LogFileHandler::eLogHandlerAlarmSettingLogFileType, entry);
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessPtInhTrigger()
//
//    Processing:
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void ApneaBackupMode::ProcessPtInhTrigger(void)
{
    // Initialize pointer to the PhaseMgr object
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // A patient trigger should only fire during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Start a new inhalation.  The type of inhalation (mand or
    // spont) depends upon the window that is currently
    // open.  If the mandatory window is open, deliver a mand
    // breath and close the mandatory window since this breath
    // will meet the mandatory breath criteria for this breath
    // period
    if (eMandWindow == Window)
    {        
        phaseMgrPtr->ProcessNewPhase(eMandInhPhase);

        // Close the mandatory window; open the spontaneous window.
        Window = eSpontWindow;
    }

    else if (eSpontWindow == Window)
    {
        // If the spontaneous window is open, deliver a spontaneous
        // inhalation.  The spontaneous window remains open until the
        // breath period timer elapses.
        phaseMgrPtr->ProcessNewPhase(eMandInhPhase);
    }
    else
    {
        // Window contains illegal value
        ASSERTION((Window == eMandWindow) || (Window == eSpontWindow));
    }


    IsPtInhTrigger = true;

    //    E_VentilationMode nextMode = (E_VentilationMode)MgrBdSetting::S_GetInstance()->GetCurrentValue(eMode);

}   // end ProcessPtInhTrigger()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessAlarmResetTrigger()
//
//    Processing:
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void ApneaBackupMode::ProcessAlarmResetTrigger()
{
    //    E_VentilationMode nextMode = (E_VentilationMode)MgrBdSetting::S_GetInstance()->GetCurrentValue(eMode);

    MgrBreathData::S_GetInstance()->SetFirstBreath();

    (ModeMgr::S_GetInstance())->GoToOperatorSetMode();
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessManualInhOpReqTrigger()
//
//    Processing:
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void ApneaBackupMode::ProcessManualInhOpReqTrigger()
{
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // Operator initiated inhalation triggers should only fire
    // during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);


    TriggerMgr::S_GetInstance()->ReEnableTrigger(eBpBackupTimeTrigger);

    // Start a mandatory inhalation.
    phaseMgrPtr->ProcessNewPhase(eMandInhPhase);
    Window = eMandWindow;

}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessMandPressInhTimeTrigger()
//
//    Processing:
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void ApneaBackupMode::ProcessMandPressInhTimeTrigger()
{

    if(!IsPtInhTrigger)
    {
        // Get a pointer to the PhaseMgr object
        PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

        // If the breath phase is a mandatory inhalation, transition to exhalation.
        // Otherwise, perform error handling.
        ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eMandInhPhase);
        phaseMgrPtr->ProcessNewPhase(eExhPhase);
    }
    else
    {
        (ModeMgr::S_GetInstance())->GoToOperatorSetMode();
    }
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessVcvInhTimeTrigger()
//
//    Processing:
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void ApneaBackupMode::ProcessVcvInhTimeTrigger()
{
    if(!IsPtInhTrigger)
    {
        // Get a handle to the PhaseMgr
        PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

        // The trigger should only fire in the mandatory inhalation phase.
        ASSERTION (phaseMgrPtr->GetCurrentPhaseId() == eMandInhPhase);

        // Assume that the ventilator will transition to plateau
        // from a VCV inhalation.  It is possible that the ventilator
        // will not go to plateau but instead will go to exhalation.
        // The phase manager takes care of that.
        phaseMgrPtr->ProcessNewPhase(ePlateauPhase);

    }
    else
    {
        (ModeMgr::S_GetInstance())->GoToOperatorSetMode();
    }
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessPlateauTimeTrigger()
//
//    Processing:
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void ApneaBackupMode::ProcessPlateauTimeTrigger()
{
    if(!IsPtInhTrigger)
    {
        PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

        // Plateau Time Trigger is only valid in the Plateau Phase.
        ASSERTION (phaseMgrPtr->GetCurrentPhaseId() == ePlateauPhase);

        // Transition to the exhalation phase.
        phaseMgrPtr->ProcessNewPhase(eExhPhase);

    }
    else
    {
        (ModeMgr::S_GetInstance())->GoToOperatorSetMode();
    }
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBpTimeTrigger()
//
//    Processing:
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void ApneaBackupMode::ProcessBpTimeTrigger(void)
{
    E_PhaseId phaseId;          // Current Phase ID
    PhaseMgr* phaseMgrPtr;      // Pointer to the PhaseMgr object
    switch (Window)
    {
        case eMandWindow:
            // Initialize pointer to the PhaseMgr object
            phaseMgrPtr = PhaseMgr::S_GetInstance();

            // Make sure the ventilator is in the exhalation phase.
            phaseId = phaseMgrPtr->GetCurrentPhaseId();
            ASSERTION(phaseId == eExhPhase);
            phaseMgrPtr->ProcessNewPhase(eMandInhPhase);
            break;

        case eSpontWindow:
            // If the breath period fires while the spontaneous window is open,
            // the ventilator just re-opens the mandatory window.
            phaseMgrPtr = PhaseMgr::S_GetInstance();

            //Make sure the ventilator is in the exhalation phase.
            phaseId = phaseMgrPtr->GetCurrentPhaseId();
            ASSERTION(phaseId == eExhPhase);
            if(phaseId == eExhPhase)
            {
                phaseMgrPtr->ProcessNewPhase(eMandInhPhase);
            }

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
//    Operation Name: ProcessApneaAlarmTimeTrigger()
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
void ApneaBackupMode::ProcessApneaAlarmTimeTrigger()
{
    //Do nothing
}
