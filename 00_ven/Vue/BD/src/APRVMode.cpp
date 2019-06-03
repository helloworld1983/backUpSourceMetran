/******************************************************************************/
//$COMMON.CPP$
//   File Name:  PCV_CPAP_Mode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:
//      The operations in this class define the APRVMode mode
//
//   Interfaces:
//
//   Restrictions:
//      There is only 1 object instantiated from this class.
/******************************************************************************/
#include "APRVMode.h"
#include "PhaseMgr.h"
#include "TriggerMgr.h"
#include "PtInhPressTrigger.h"
#include "SupportedInhPhase.h"
#include "pdtask.h"
#include "MgrBdSetting.h"
#include "MgrBreathData.h"
#include "AlarmTask.h"
#include "MgrController.h"

APRVMode* APRVMode::S_Instance = NULL;

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
APRVMode* APRVMode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new APRVMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: APRVMode()
//
//    Processing:
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
APRVMode::APRVMode() : SpontMode(),Timer("TimerOfAPRVMode", 1000, pdTRUE)
{
    mTimeWindow = eNot_ReleaseWindow;
    IsInReleaseWindow = false;
    IsPtInhTrigger = false;
    mPressureWindow = ePLowWindow;
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
//
//
//    Requirements:
//
/******************************************************************************/
void APRVMode::Enter(E_VentilationMode fromModeId)
{
    const int APRV_UPDATE_BRTRIGGER = 60000;
    mTimeWindow = eNot_ReleaseWindow;
    mPressureWindow = ePLowWindow;

    IsInReleaseWindow = false;
    IsPtInhTrigger = false;

    (PhaseMgr::S_GetInstance())->ProcessNewVentType(ePressureVentilation);
    MgrController::CMVServoController->ResetIMVStaticVariables();
    MgrController::CMVServoController->Enable();

    SpontMode::Enter(fromModeId);

    //Start PLow phase
    ProcessAPRVPHighTimeTrigger();

    TriggerMgr::S_GetInstance()->EnableTrigger(eAPRVUpdateBRTrigger,APRV_UPDATE_BRTRIGGER);
    TriggerMgr::S_GetInstance()->EnableTrigger(eExtremeHigh_PHigh_Trigger);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessPtInhTrigger()
//
//    Processing:
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
//
//
//    Requirements:
//
/******************************************************************************/
void APRVMode::ProcessPtInhTrigger()
{
    if(mPressureWindow == ePLowWindow)
    {
        PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();
        phaseMgrPtr->ProcessNewPhase(eSpontInhPhase_InAPRV_PLow);
        IsPtInhTrigger = false;
    }
    else
    {
        TriggerMgr::S_GetInstance()->DisableTrigger(eHPHighAlarmTrigger);
        IsPtInhTrigger = true;
        switch(mTimeWindow)
        {
            case eNot_ReleaseWindow:
                IsInReleaseWindow = false;
                break;
            case eReleaseWindow:
                IsInReleaseWindow = true;
                TriggerMgr::S_GetInstance()->DisableTrigger(eAPRVHighPressTimeTrigger);
                mPressureWindow = ePLowWindow;
                break;
        }

        //Initialize pointer to the PhaseMgr object
        PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

        phaseMgrPtr->ProcessNewPhase(eSpontInhPhase_InAPRV_PHigh);
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessHipAlarmTrigger()
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
void APRVMode::ProcessHipAlarmTrigger(void)
{
    ProcessAPRVPHighTimeTrigger();
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessItLongAlarmTimeTrigger()
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
void APRVMode::ProcessItLongAlarmTimeTrigger()
{
    //End Inhalation
    if(IsInReleaseWindow)
    {
        EndSpontInhalation_InReleaseWindow();
    }
    else
    {
        EndSpontInhalation();
    }
    //Active ITimeTooLong
    AlarmTask::S_GetInstance()->SetIsActive(eITimeTooLong, eActive);
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessAPRVPtInhTrigger_InPHigh()
//
//    Processing:
//      This operation handles the processing of the Patient Exhalation Trigger for the APRV Mode.
//
//      This operation checks the IsInReleaseWindow flag then invokes EndSpontInhalation_InReleaseWindow() function
//      or EndSpontInhalation() function.And deactive ITimeTooLong Alarm
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
void APRVMode::ProcessPtExhTrigger()
{
    //Clear ITimeTooLong
    AlarmTask::S_GetInstance()->SetIsActive(eITimeTooLong, eNotActive);

    if(IsInReleaseWindow)
    {
        EndSpontInhalation_InReleaseWindow();
    }
    else
    {
        EndSpontInhalation();
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
void APRVMode::Exit()
{
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    triggerMgrPtr->DisableTrigger(eAPRVLowPressTimeTrigger);
    triggerMgrPtr->DisableTrigger(eAPRVHighPressTimeTrigger);
    triggerMgrPtr->DisableTrigger(eExtremeHigh_PHigh_Trigger);
    triggerMgrPtr->DisableTrigger(eHPHighAlarmTrigger);

    mTimeWindow = eNot_ReleaseWindow;
    IsInReleaseWindow = false;
    //    CMVServoController->Disable();
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessAPRVPLowTimeTrigger()
//
//    Processing:
//      This operation handles the processing of the APRV PLow Time Trigger for the APRV Mode.
//      - ProcessNewPhase  APRV PHigh Phase
//      - Disable APRV Low Press Time Trigger
//      - Enable APRV High Press Time Trigger
//      - Reset The Timer
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
void APRVMode::ProcessAPRVPLowTimeTrigger(void)
{
    mPressureWindow = ePHighWindow;
    PhaseMgr::S_GetInstance()->ProcessNewPhase(eAPRVHighPhase);
    TriggerMgr::S_GetInstance()->DisableTrigger(eAPRVLowPressTimeTrigger);
    TriggerMgr::S_GetInstance()->EnableTrigger(eAPRVHighPressTimeTrigger);

    //    LONG time = CalculateReleaseWindowTime()*10;
    //    Disable();
    //    Reset(time,time,true);
    //    Enable();
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessAPRVPHighTimeTrigger()
//
//    Processing:
//      This operation handles the processing of the APRV PHigh Time Trigger for the APRV Mode.
//      - ProcessNewPhase  APRV PLow Phase
//      - Disable APRV High Press Time Trigger
//      - Enable APRV Low Press Time Trigger
//      - Disable The Timer
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
void APRVMode::ProcessAPRVPHighTimeTrigger(void)
{
    if(PhaseMgr::S_GetInstance()->GetCurrentPhaseId() == eSpontInhPhase_InAPRV_PHigh)
    {
        AlarmTask::S_GetInstance()->SetIsActive(eITimeTooLong, eNotActive);
    }
    mPressureWindow = ePLowWindow;
    PhaseMgr::S_GetInstance()->ProcessNewPhase(eAPRVLowPhase);
    TriggerMgr::S_GetInstance()->DisableTrigger(eAPRVHighPressTimeTrigger);
    TriggerMgr::S_GetInstance()->EnableTrigger(eAPRVLowPressTimeTrigger);

    //Disable timer
    Stop();
    mTimeWindow = eNot_ReleaseWindow;
    IsInReleaseWindow = false;
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ExpirationRoutine()
//
//    Processing:
//      This operation handles the Timer Expiration.
//      It just open Release Window
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
void APRVMode::ExpirationRoutine()
{
    //Disable timer
    Stop();
    mTimeWindow = eReleaseWindow;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of APRV Mode
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
void APRVMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case ePtInhPressTrigger:
            ProcessPtInhTrigger();
            break;

//        case ePtExhTrigger:
//            ProcessPtExhTrigger();
//            break;

        case eAPRVLowPressTimeTrigger:
            ProcessAPRVPLowTimeTrigger();
            break;

        case eAPRVHighPressTimeTrigger:
            ProcessAPRVPHighTimeTrigger();
            break;

        case eItLongAlarmTimeTrigger:
            ProcessItLongAlarmTimeTrigger();
            break;

        case eHipAlarmTrigger:
            ProcessHipAlarmTrigger();
            break;

        case eApneaAlarmTimeTrigger:
            ProcessApneaAlarmTimeTrigger();
            break;

        case eAPRVUpdateBRTrigger:
            ProcessAPRVUpdateBRTrigger();
            break;

        case eHPHighAlarmTrigger:
            ProcessHPHighAlarmTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CalculateReleaseWindowTime()
//
//    Processing:
//      This operation computes the time before opening Release Window form starting APRV PHigh Phase
//      If Ti_Limit Setting > 25%(Time of PHigh), then time  = 25%(Time of PHigh)
//      Otherwise , time = Ti_Limit
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      LONG time
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
LONG APRVMode::CalculateReleaseWindowTime()
{
    LONG T_HighSetting = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVTimeHigh);
    LONG percent25_THigh = T_HighSetting*25/100;
    LONG Ti_Limit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eTiLimit);

    LONG ReleaseWindowTime;

    if(Ti_Limit > percent25_THigh)
    {
        ReleaseWindowTime = percent25_THigh;
    }
    else
    {
        ReleaseWindowTime = Ti_Limit;
    }

    LONG time = (T_HighSetting - ReleaseWindowTime);

    return (time);
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EndSpontInhalation()
//
//    Processing:
//      This operation ends the Spont Inhalation Phase then Processes a new phase
//      The New Phase Processing is depended on Pressure Window
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      LONG time
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
void APRVMode::EndSpontInhalation()
{
    // Initialize pointer to the PhaseMgr
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    if(mPressureWindow == ePLowWindow)
    {
        //start APRV Low Pressure Phase
        phaseMgrPtr->ProcessNewPhase(eAPRVExhPhase_InPLow);
    }
    else
    {
        phaseMgrPtr->ProcessNewPhase(eAPRVExhPhase_InPHigh);
    }
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EndSpontInhalation_InReleaseWindow()
//
//    Processing:
//      This operation ends the Spont Inhalation Phase then Processes a new phase
//      The New Phase Processing always APRV PLow Phase
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      LONG time
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
void APRVMode::EndSpontInhalation_InReleaseWindow()
{
    ProcessAPRVPHighTimeTrigger();
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessApneaAlarmTimeTrigger()
//
//    Processing:
//      This operation handles the processing of the Apnea Alarm Time Trigger for the APRV Mode.
//      - Active Apnea Alarm
//      - Disable Apnea Alarm Time Trigger
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      LONG time
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
void APRVMode::ProcessApneaAlarmTimeTrigger()
{
    AlarmTask::S_GetInstance()->SetIsActive(eApnea, eActive);

    TriggerMgr::S_GetInstance()->DisableTrigger(eApneaAlarmTimeTrigger);
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessAPRVUpdateBRTrigger()
//
//    Processing:
//      This operation handles the processing of the APRV Update Breath Rate Trigger for the APRV Mode.
//      - Disable APRV Update Breath Rate Trigger
//      - Send UPDATE_APRV_BR_FLAG to Ptdata to update the data
//      - ReEnable APRV Update Breath Rate Trigger
//      APRV Update Breath Rate Trigger is a time trigger with time interval = 1 minute
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      LONG time
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
void APRVMode::ProcessAPRVUpdateBRTrigger()
{
    const int APRV_UPDATE_BRTRIGGER = 60000;

    TriggerMgr::S_GetInstance()->DisableTrigger(eAPRVUpdateBRTrigger);
    PdTask::S_GetInstance()->S_SetEventFlag(UPDATE_APRV_BR_FLAG);
    TriggerMgr::S_GetInstance()->EnableTrigger(eAPRVUpdateBRTrigger,APRV_UPDATE_BRTRIGGER);
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessHPHighAlarmTrigger()
//
//    Processing:
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      LONG time
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
void APRVMode::ProcessHPHighAlarmTrigger()
{
    ProcessAPRVPHighTimeTrigger();
}
