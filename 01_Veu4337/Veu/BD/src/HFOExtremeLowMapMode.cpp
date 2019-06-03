/******************************************************************************/
//$COMMON.CPP$
//   File Name:  HFOExtremeLowMapMode.cpp
//   Purpose: 
//      This file contains the definition of the HFOExtremeLowMap mode.  The HFOExtremLowMap
//        mode is entered in response to the detection of an occluded circuit.  
//      It is exited when the reset for the High MAP_2 condition occurs.
//
//   Interfaces:
//      The ProcessExtremeLowMapAutoResetTrigger() operation in this class calls 
//      ModeMgr::ReturnToLastMode() to transition to the operator set mode or 
//      the mode that was active when the HFOHighMAP_2 mode was entered.
//
//      The Enter() and Exit() operations interface with the TriggerMgr to 
//      enable/disable triggers.  
//
//   Restrictions:
//      There is only 1 instance of this class.
//
//   Revision History:
//
//
/******************************************************************************/
#ifdef HFO_SYSTEM
#include "HFOExtremeLowMapMode.h"
#include "TriggerMgr.h"
#include "ModeMgr.h"
#include "PhaseMgr.h"
#include "AlarmStat.h"
#include "Hfo_cnt.h"
#include "AzTest.h"
#include "HfoServo.h"
#include "mainhandlerdelegate.h"
// Initialize statics
HFOExtremeLowMapMode* HFOExtremeLowMapMode::S_Instance = NULL;
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: HFOExtremeLowMapMode()
//
//    Processing: 
//      This operation is the constructor for the HFOExtremeLowMapMode mode.  It invokes 
//      the base class AlarmBreathingMode constructor.
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
HFOExtremeLowMapMode::HFOExtremeLowMapMode(void):AlarmBreathingMode(eTrue)
{
}   // end HFOExtremeLowMapMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter()
//
//    Processing: 
//      This operation is called whenever an Extreme Low Map alarm trigger fires.
//      enables the Extreme Low Map Auto reset trigger and updates the High MAP_2 alarm
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
void HFOExtremeLowMapMode::Enter(E_VentilationMode fromModeId)
{
    switch (fromModeId)
    {
    case eHFOMode:
        ASSERTION(PhaseMgr::S_GetInstance()->GetCurrentPhaseId() == eHFOPhase);
        HfoController->Enable();
        HfoServo::S_GetInstance()->Disable();
        BdTask::S_GetInstance()->Send(eSuspendAutoSetRequest);
        break;
    default:
        ASSERTION (0);
        break;
    }

    // Initialize pointer to the TriggerMgr
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    
    // Enable the reset trigger
    triggerMgrPtr->EnableTrigger(eExtremeLowMapAutoResetTrigger);
    triggerMgrPtr->EnableTrigger(eAlarmResetOpReqTrigger);
    triggerMgrPtr->EnableTrigger(eHardwareErrorAlarmTriger);
    triggerMgrPtr->EnableTrigger(eModeChangeTrigger);
    triggerMgrPtr->EnableTrigger(eTotalGasLossTrigger);
    triggerMgrPtr->DisableTrigger(eExtremeLowMapAlarmTrigger);

    // Enable the Update Status trigger which causes the update of the
    // status of the alarms associated with the active Disconnected mode.
    triggerMgrPtr->EnableTrigger(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);

    // Update the extreme low map alarm.
    Alarm[ALARM_IDX(eExtremeLowMAP)]->SetIsActive(eActive);

    //	Suspend auto-zero scheduling.
    PInhAzTestPtr->SuspendTest();
    PExhAzTestPtr->SuspendTest();


}   // end Enter()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Exit()
//
//    Processing: 
//      This operation is invoked when the occlusion condition is reset and
//      the ventilator is to exit the Extreme Low Map mode.  This operation disables
//      the High MAP_2 reset trigger, and updates the Extreme Low Map alarm status.
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
void HFOExtremeLowMapMode::Exit(void)
{
    HfoController->Disable();
    HfoServo::S_GetInstance()->Disable();
    // Initialize pointer to the TriggerMgr
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable triggers.
    triggerMgrPtr->DisableTrigger(eExtremeLowMapAutoResetTrigger);
    triggerMgrPtr->DisableTrigger(eUpdateStatusTimeTrigger);
    
    // Update the High MAP_1 alarm status.
    Alarm[ALARM_IDX(eExtremeLowMAP)]->SetIsActive(eNotActive);

    // Resume auto-zero scheduling.
    PInhAzTestPtr->ResumeTest();
    PExhAzTestPtr->ResumeTest();

}   // end Exit()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessExtremeLowMapAutoResetTrigger()
//
//    Processing: 
//      This operation handles the processing of the High MAP_2 alarm Auto-Reset 
//      Trigger for the Extreme Low Map mode. If so, ModeMgr::GoToLastMode() is called
//      to return the ventilator to the previous mode or the operator set mode.
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
void HFOExtremeLowMapMode::ProcessExtremeLowMapAutoResetTrigger(void)
{
    ModeMgr* modeMgrPtr = ModeMgr::S_GetInstance();
    modeMgrPtr->ProcessAutoReset();
    modeMgrPtr->ReturnToLastMode();
}   // end ProcessExtremeLowMapAutoResetTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdateAlarmStatus()
//
//    Processing: 
//      This operation is invoked each time the UpdateStatus TimeTrigger fires.
//      It updates the occlusion alarm status to indicate that occlusion is
//      still active.
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
void HFOExtremeLowMapMode::UpdateAlarmStatus(void)
{
    E_ReturnStatus status;
    TriggerMgr* triggerMgrPtr;
    
    status = Alarm[ALARM_IDX(eExtremeLowMAP)]->SetIsActive(eActive);

    // If the status update was unsuccessful, reschedule the next update
    // for the short interval
    if (status != eSuccess)
    {
        triggerMgrPtr = TriggerMgr::S_GetInstance();
        triggerMgrPtr->DisableTrigger(eUpdateStatusTimeTrigger);
        triggerMgrPtr->EnableTrigger(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);
    }

}   // end UpdateAlarmStatus()
#endif // HFO_SYSTEM




