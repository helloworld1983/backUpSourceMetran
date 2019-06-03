/******************************************************************************/
//$COMMON.CPP$
//   File Name:  HFOMode.cpp
//
//   Purpose: 
//      The operations in this class define the HFO Mode of
//      ventilation.  
//   Interfaces:
//      Operations in this class interface with the TriggerMgr to enable,
//      re-enable and disable triggers via EnableTrigger(), ReEnableTrigger()
//      and DisableTrigger().
//
//   Restrictions:
//      There is only 1 object instantiated from this class.
//

/******************************************************************************/
#ifdef HFO_SYSTEM
#include "HFOMode.h"
#include "TriggerMgr.h"
#include "PhaseMgr.h"
#include "AlarmStat.h"
#include "Hfo_cnt.h"
#include "ModeMgr.h"
#include "MgrBdSetting.h"
#include "ExtremeLowMapAlarmTrigger.h"
#include "HFOAlarmTrigger.h"
#include "DOutput.h"
#include "Stepmoto.h"
#include "HfoServo.h"
#include "MgrAlarmStat.h"
#include "Cmv_cnt.h"
#include "Vcv_cnt.h"
#include "mainhandlerdelegate.h"
#include "Btps.h"
#include "ampAlarmAutoSet.h"
#include "BdTask.h"
#include "SIOpRequest.h"
#include "hfomap_cnt.h"
// Define statics
#define EXH_MAX_STEP 2300
HFOMode* HFOMode::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: HFOMode()
//
//    Processing: 
//      This operation is the constructor for the HFOMode.  It invokes
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
HFOMode::HFOMode(void):NormBreathingMode()
{
}   // end HFOMode()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter()
//
//    Processing: 
//      This operation is called whenever an HFO trigger fires.
//      This operation calls the base class NonVentMode::Enter() operation,
//      enables the mode change trigger and updates the HFO alarm
//      status.
//
//    Input Parameters:
//      fromModeId - ID of the Mode that is being exited 
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
void HFOMode::Enter(E_VentilationMode fromModeId)
{
    SafetyValve->SetDesiredState(eClose); //eOn

    HfoController->SetConstFlow(eFalse);

    // Enable HFO servo

    //close safety valve

    HfoController->SetExhValveLimit(EXH_MAX_STEP);

    HfoController->Enable();

    HfoServo::S_Instance->Enable();

    HfoController->SetSIRequest(eFalse);
    HfoServo::S_GetInstance()->SetSIRequest(eFalse);


    // Initialize pointer to the PhaseMgr
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();
    // Initialize pointer to the TriggerMgr
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    HfoController->ResetHFOStaticVariables();
    HfoServo::S_GetInstance()->ClearStaticVariables();
    phaseMgrPtr->ProcessNewPhase(eHFOPhase);

    switch(fromModeId)
    {
    case eHardwareErrorMode:
    case eNoGasMode:
    case eHFOExtremeLowMAPMode:
        Alarm[ALARM_IDX(eLowMAP)]->SetIsActive(eNotActive);
    case eHFOHighMAP_2Mode:
    {
        Alarm[ALARM_IDX(eHighMAP)]->SetIsActive(eNotActive);
        Alarm[ALARM_IDX(eHighAmplitude)]->SetIsActive(eNotActive);
        ExtremeLowMapAlarmTrigger::S_GetInstance()->SetInactiveTimer(5000);
        HFOAlarmTrigger::S_GetInstance()->SetInactiveTimer((USHORT)10000);// 2ms servo*10000 =20s

        BdTask::S_GetInstance()->Send(eDoAutoSetRequest);
        break;
    }
    default:

        HfoServo::S_GetInstance()->ResetPIDStatic();
        HfoServo::S_GetInstance()->Reset_20sTimer();

        HFOAlarmTrigger::S_GetInstance()->SetInactiveTimer((USHORT)10000);// 2ms servo*10000 =20s
        ExtremeLowMapAlarmTrigger::S_GetInstance()->SetInactiveTimer(5000);

        BdTask::S_GetInstance()->Send(eStopAutoSetRequest);

        if(eSwitchTypeOn == MgrBdSetting::S_GetInstance()->GetCurrentValue(eAmplitudeControl))
        {
            BdTask::S_GetInstance()->Send(eAmpChangeRequest);
        }
        else
        {
            BdTask::S_GetInstance()->Send(eSVChangeRequest);
        }


        break;
    }

    triggerMgrPtr->DisableAllTriggers();

    triggerMgrPtr->EnableHFOTrigger();

    Btps::S_ComputeBtpsFactors();

}   // end Enter()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Exit()
//
//    Processing: 
//      This operation is invoked when the ventilator is to exit the HFO mode. 
//      This operation disables mode change trigger, invokes the base class 
//        NonVentMode::Exit() operation, and updates the HFO alarm status.
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
void HFOMode::Exit(void)
{
    OilPump->SetDesiredState(eOff); //close OilPump

    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtr->DisableTrigger(eModeChangeTrigger);

    triggerMgrPtr->DisableTrigger(eTotalGasLossTrigger);
    
    triggerMgrPtr->DisableTrigger(eHighMAP_2AlarmTrigger);
    
    triggerMgrPtr->DisableTrigger(eExtremeLowMapAlarmTrigger);
    
    triggerMgrPtr->DisableTrigger(eUpdateStatusTimeTrigger);

    triggerMgrPtr->DisableTrigger(eHardwareErrorAlarmTriger);

    triggerMgrPtr->DisableTrigger(eOcclusionTrigger);

    triggerMgrPtr->DisableTrigger(eAutoSIOnTimeTrigger);

    triggerMgrPtr->DisableTrigger(eAutoSIOffTimeTrigger);

    HfoController->Disable();
    HfoServo::S_GetInstance()->Disable();

    ProcessResetSITimeTrigger();

    SIOpRequest::S_GetInstance()->ResetSIStatus();
    QMetaObject::invokeMethod(MainHandlerDelegate::S_GetInstance(), "EmitSIButtonEvent",
                              Q_ARG(bool, false));
}   // end Exit()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name:ProcessHighMAP_2AlarmTrigger(void)()
//
//    Processing: 
//      This operation handles the processing of the High MAP_2 Alarm Trigger
//      only for HFO mode.
//  
//      This operation invokes ModeMgr::GoToAlarmMode() to transition to the 
//      HFOHighMAP_2 mode.
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
void HFOMode::ProcessHighMAP_2AlarmTrigger(void)
{
    (ModeMgr::S_GetInstance())->GoToAlarmMode(eHFOHighMAP_2Mode);
}   // end ProcessHighMAP_2AlarmTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name:ProcessExtremeLowMapAlarmTrigger(void)()
//
//    Processing: 
//      This operation handles the processing of the ProcessExtremeLowMapAlarmTrigger Trigger
//      only for HFO mode.
//  
//      This operation invokes ModeMgr::GoToAlarmMode() to transition to the 
//      HFOHighMAP_2 mode.
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
void HFOMode::ProcessExtremeLowMapAlarmTrigger(void)
{
    // Transition to the Extreme low map mode.
    (ModeMgr::S_GetInstance())->GoToAlarmMode(eHFOExtremeLowMAPMode);

}   // end ProcessExtremeLowMapAlarmTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdateAlarmStatus()
//
//    Processing: 
//      This operation is invoked each time the update status time trigger
//      fires to update the mode related alarm statuses.  As part of 
//      the built in redundancy, the HighMAP_2 and SVO alarm statuses
//      are updated to indicate that they are not active.
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
void HFOMode::UpdateAlarmStatus(void)
{
    SHORT status = 0;
    TriggerMgr* triggerMgrPtr;
    // If it is time to auto-zero the inhalation pressure transducer,
    // kick start the auto-zero, update the state.

    // Notify AlarmStat objects that SVO,Disconnection and Hip2 are not active
    status+= (SHORT) Alarm[ALARM_IDX(eGasSuppliesLost)]->SetIsActive(eNotActive);
    status+= (SHORT) Alarm[ALARM_IDX(eExtremeHighMAP)]->SetIsActive(eNotActive);

    // If any of the status updates was unsuccessful, reschedule the next update
    // for the short interval
    if (status != (SHORT) eSuccess)
    {
        triggerMgrPtr = TriggerMgr::S_GetInstance();
        triggerMgrPtr->DisableTrigger(eUpdateStatusTimeTrigger);
        triggerMgrPtr->EnableTrigger(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);
    }

}   // UpdateAlarmStatus()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessSI10sTimerTrigger()
//
//    Processing:
//      This operation processes the SI 10s Time Trigger
//      - Disable SI 10s Time Trigger
//      - Enable 1sAfterSITrigger
//      - Send SI Off Command to BdTask
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
void HFOMode::ProcessSI10sTimerTrigger()
{
    BdTask::S_GetInstance()->Send(eManHoldInspKeyUp); // End SI
    TriggerMgr::S_GetInstance()->DisableTrigger(eSI10sTimeTrigger);
    TriggerMgr::S_GetInstance()->EnableTrigger(eResetSITimeTrigger,1000);
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessResetSITimeTrigger()
//
//    Processing:
//      1s After SI stop, Reset SI Time in Patien Data Monitor
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
void HFOMode::ProcessResetSITimeTrigger()
{
    TriggerMgr::S_GetInstance()->DisableTrigger(eResetSITimeTrigger);
    PatientDataGUIEvent SIMsg;
    SIMsg.Value = 0;
    MainHandlerDelegate::S_GetInstance()->postCustomEvent<PatientDataGUIEvent>(eManInspHoldTimePtDataID, SIMsg);
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessAutoSIOnTimeTrigger()
//
//    Processing:
//      This operation processes the Auto SI On Trigger
//      - Disable Auto SI On Trigger
//      - Send Auto SI Off Command to BdTask
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
void HFOMode::ProcessAutoSIOnTimeTrigger(void) // End SI
{
    TriggerMgr::S_GetInstance()->DisableTrigger(eAutoSIOnTimeTrigger);
    BdTask::S_GetInstance()->Send(eAutoSIOff); // End SI
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessAutoSIOffTimeTrigger()
//
//    Processing:
//      This operation processes the Auto SI Off Trigger
//      - Disable Auto SI Off Trigger
//      - Send Auto SI On Command to BdTask
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
void HFOMode::ProcessAutoSIOffTimeTrigger(void) // Start SI
{

    if((MgrBdSetting::S_GetInstance()->GetCurrentValue(eSITime)!=0)
            && (eSwitchTypeOn == MgrBdSetting::S_GetInstance()->GetCurrentValue(eAutoSI)))
    {
        BdTask::S_GetInstance()->Send(eAutoSIOn); // Start SI
    }
    TriggerMgr::S_GetInstance()->DisableTrigger(eAutoSIOffTimeTrigger);

}
#endif // HFO_SYSTEM


