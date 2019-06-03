/******************************************************************************/
//$COMMON.CPP$
//   File Name:  HFOHighMAP_2Mode.cpp
//   Purpose: 
//      This file contains the definition of the HFOHighMAP_2 mode.  The HFOHighMAP_2 
//        mode is entered in response to the detection of an occluded circuit.  
//      It is exited when the reset for the High MAP_2 condition occurs.
//
//   Interfaces:
//      The ProcessHighMAP_2AutoResetTrigger() operation in this class calls 
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
#include "HFOHighMAP_2Mode.h"
#include "TriggerMgr.h"
#include "ModeMgr.h"
#include "PhaseMgr.h"
#include "AlarmStat.h"
#include "Stepmoto.h"
#include "AzTest.h"
#include "Hfo_cnt.h"
#include "HfoServo.h"
#include "paneltask.h"
#include "mainhandlerdelegate.h"
// Initialize statics
HFOHighMAP_2Mode* HFOHighMAP_2Mode::S_Instance = NULL;
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: HFOHighMAP_2Mode()
//
//    Processing: 
//      This operation is the constructor for the HFOHighMAP_2 mode.  It invokes 
//      the base class NonVentMode constructor.
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
HFOHighMAP_2Mode::HFOHighMAP_2Mode(void):AlarmBreathingMode(eTrue)
{
}   // end HFOHighMAP_2Mode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter()
//
//    Processing: 
//      This operation is called whenever an High MAP_2 alarm trigger fires.
//      enables the High MAP_2 reset trigger and updates the High MAP_2 alarm
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
void HFOHighMAP_2Mode::Enter(E_VentilationMode fromModeId)
{
    switch (fromModeId)
    {
    case eHFOMode:
        SafetyValve->SetDesiredState(eOpen);

        HfoController->Disable();
        HfoServo::S_GetInstance()->Disable();

        HfoController->ResetHFOStaticVariables();
        HfoServo::S_GetInstance()->ClearStaticVariables();
        BdTask::S_GetInstance()->Send(eSuspendAutoSetRequest);
        TriggerMgr::S_GetInstance()->DisableTrigger(eSI10sTimeTrigger);
        break;
    default:
        ASSERTION (0);
        break;
    }

    // Initialize pointer to the TriggerMgr
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    
    triggerMgrPtr->EnableTrigger(eAlarmResetOpReqTrigger);
    triggerMgrPtr->EnableTrigger(eModeChangeTrigger);
    // Enable the Update Status trigger which causes the update of the
    // status of the alarms associated with the active Disconnected mode.
    triggerMgrPtr->EnableTrigger(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);

    // Update the High MAP_2 alarm status.
    if(causeOfAlarm != eNoEventID)
        Alarm[ALARM_IDX(causeOfAlarm)]->SetIsActive(eActive);

    //	Suspend auto-zero scheduling.
    PInhAzTestPtr->SuspendTest();
    PExhAzTestPtr->SuspendTest();

    AlarmBreathingMode::Enter(fromModeId);

    MainHandlerDelegate::S_GetInstance()->postCustomEvent(eAmpAlarmAutoSetStopEvent);
    PhaseMgr::S_GetInstance()->ProcessNewPhase(eAlarmPhase);

}   // end Enter()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Exit()
//
//    Processing: 
//      This operation is invoked when the occlusion condition is reset and
//      the ventilator is to exit the HFOHighMAP_2 mode.  This operation disables
//      the High MAP_2 reset trigger, and updates the High MAP_2 alarm status.
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
void HFOHighMAP_2Mode::Exit(void)
{

    // Update the HighMAP_2 alarm status
    if(causeOfAlarm != eNoEventID)
        Alarm[ALARM_IDX(causeOfAlarm)]->SetIsActive(eNotActive);

    // Initialize pointer to the TriggerMgr
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable triggers.
    triggerMgrPtr->DisableTrigger(eHighMAP_2AutoResetTrigger);
    triggerMgrPtr->DisableTrigger(eUpdateStatusTimeTrigger);

    // Resume auto-zero scheduling.
    PInhAzTestPtr->ResumeTest();
    PExhAzTestPtr->ResumeTest();

    E_VentilationMode nextMode = ModeMgr::S_GetInstance()->GetNextModeId();

    QMetaObject::invokeMethod(MainHandlerDelegate::S_GetInstance(), "EmitBlinkingModeSignal",
                              Q_ARG(int, (int)nextMode),Q_ARG(bool,false));
    AlarmBreathingMode::Exit();
}   // end Exit()

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
void HFOHighMAP_2Mode::UpdateAlarmStatus(void)
{
    E_ReturnStatus status;
    TriggerMgr* triggerMgrPtr;
    
    status = Alarm[ALARM_IDX(causeOfAlarm)]->SetIsActive(eActive);

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




