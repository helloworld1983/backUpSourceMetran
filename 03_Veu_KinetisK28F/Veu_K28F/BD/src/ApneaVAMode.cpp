/******************************************************************************/
//$COMMON.CPP$
//   File Name:  PCV_CPAP_Mode.cpp
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
#include "ApneaVAMode.h"

#include "AlarmTask.h"

#include "TriggerMgr.h"
#include "PhaseMgr.h"
#include "MgrBdSetting.h"
#include "ModeMgr.h"
#include "MgrBreathData.h"

#include "DeviceInterface.h"
#include "ProxyFlowSensor.h"
#include "MgrController.h"

#include "DebugTrace.h"

ApneaVAMode* ApneaVAMode::S_Instance = NULL;

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
ApneaVAMode* ApneaVAMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new ApneaVAMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of ApneaVAMode
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
void ApneaVAMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eProxyDisconnectTrigger:
            ProcessProxyDisconnectTrigger();
            break;

        case eProxyConnectTrigger:
            ProcessProxyConnectTrigger();
            break;

//        case ePtInhTrigger:
//            ProcessPtInhTrigger();
//            break;

        case eMandPressInhTimeTrigger:
            ProcessMandPressInhTimeTrigger();
            break;

//        case eAlarmResetTrigger:
//            ProcessAlarmResetTrigger();
//            break;

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
//    Operation Name: Exit()
//
//    Processing: Constructor of ApneaVAMode
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
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
ApneaVAMode::ApneaVAMode()
{
    fromMode = eStandbyMode;
    IsPtInhTrigger = false;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter(E_VentilationMode fromModeId)
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
void ApneaVAMode::Enter(E_VentilationMode fromModeId)
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


        TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
        triggerMgrPtr->DisableAllTriggers();

        triggerMgrPtr->EnableIMVTrigger();
        triggerMgrPtr->DisableTrigger(eApneaAlarmTimeTrigger);

        UNSIGNED breathPeriod = TriggerMgr::S_GetInstance()->GetTimePeriod(eBpBackupTimeTrigger);
        TriggerMgr::S_GetInstance()->EnableTrigger(eBpBackupTimeTrigger,breathPeriod);

        DeviceInterface::S_GetInstance()->RequestDevice(eCMVServoControllerEnable, &TempState);

        ProxyStatus Status = ProxyFlowSensor::S_GetInstance()->getLastStatus();

        if((Status ==eCalibrated)||(Status==eResetAndRecovered))
        {
            TriggerMgr::S_GetInstance()->EnableTrigger(eProxyDisconnectTrigger);
            prevProxyStatus = eCalibrated;
        }
        else
        {
            prevProxyStatus = eNotCalibrated;
            TriggerMgr::S_GetInstance()->EnableTrigger(eProxyConnectTrigger);
        }
        AlarmTask::S_GetInstance()->SetIsActive(eApnea, eActive);
        ProcessInhPhase();
        break;
    }
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
void ApneaVAMode::Exit()
{
    AlarmTask::S_GetInstance()->SetIsActive(eApnea, eNotActive);

    MgrBreathData::S_GetInstance()->InitializeBreathDataPtr();
    TriggerMgr::S_GetInstance()->DisableTrigger(eBpBackupTimeTrigger);

    MgrController::CMVServoController->Disable();

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessPtInhTrigger()
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
void ApneaVAMode::ProcessPtInhTrigger(void)
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
        // Start a mandatory inhalation
        phaseMgrPtr->ProcessNewPhase(eMandInhVABreathPhase);

        // Close the mandatory window; open the spontaneous window.
        Window = eSpontWindow;
    }

    else if (eSpontWindow == Window)
    {
        // If the spontaneous window is open, deliver a spontaneous
        // inhalation.  The spontaneous window remains open until the
        // breath period timer elapses.
        phaseMgrPtr->ProcessNewPhase(eMandInhVABreathPhase);
    }
    else
    {
        // Window contains illegal value
        ASSERTION((Window == eMandWindow) || (Window == eSpontWindow));
    }
    IsPtInhTrigger = true;

}   // end ProcessPtInhTrigger()
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
void ApneaVAMode::ProcessMandPressInhTimeTrigger()
{
    if(!IsPtInhTrigger)
    {
        // Get a pointer to the PhaseMgr object
        PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

        phaseMgrPtr->ProcessNewPhase(eExhPhase);
    }
    else
    {
        (ModeMgr::S_GetInstance())->GoToOperatorSetMode();
    }
}
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
void ApneaVAMode::ProcessAlarmResetTrigger()
{
//    MgrBreathData::S_GetInstance()->SetFirstBreath();

    (ModeMgr::S_GetInstance())->GoToOperatorSetMode();
}
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
void ApneaVAMode::ProcessApneaAlarmTimeTrigger()
{
    //Do nothing
}
