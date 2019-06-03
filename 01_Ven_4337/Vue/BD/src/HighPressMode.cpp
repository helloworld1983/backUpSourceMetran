/******************************************************************************/
//$COMMON.CPP$
//   File Name:  HighPressMode.cpp
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
/******************************************************************************/
#include "HighPressMode.h"
#include "TriggerMgr.h"
#include "PhaseMgr.h"
#include "AlarmTask.h"
#include "AzTest.h"
#include "pdtask.h"
#include "MgrController.h"

// Initialize statics
HighPressMode* HighPressMode::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the HighPressMode object, it invokes HighPressMode::S_GetInstance(),
//      which returns a pointer to the HighPressMode object.
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
//      HighPressMode* - pointer to the object instantiated from this class
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
HighPressMode* HighPressMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new HighPressMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: HighPressMode()
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
HighPressMode::HighPressMode(void):AlarmBreathingMode(eTrue)
{
    causeOfAlarm = eNoEventID;
    FromModeId = eStandbyMode;
}   // end HighPressMode()

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
void HighPressMode::Enter(E_VentilationMode fromModeId)
{
    FromModeId = fromModeId;

    //    SafetyValve->SetDesiredState(eOpen);

    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    triggerMgrPtr->DisableAllTriggers();
    triggerMgrPtr->EnableTrigger(eModeChangeTrigger);
    triggerMgrPtr->EnableTrigger(eAlarmResetOpReqTrigger);
    triggerMgrPtr->DisableTrigger(eUpdateStatusTimeTrigger);
    triggerMgrPtr->EnableTrigger(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);    

    MgrController::CMVServoController->Disable();
    MgrController::VCVServoController->Disable();
    //    HfoController->Disable();

    //	Suspend auto-zero scheduling.
    //    PInhAzTestPtr->SuspendTest();
    //    PExhAzTestPtr->SuspendTest();
    (PdTask::S_GetInstance())->S_SetEventFlag(NONBREATHING_MODE_EVENT_FLAG);

    AlarmBreathingMode::Enter(fromModeId);
    PhaseMgr::S_GetInstance()->ProcessNewPhase(eAlarmPhase);
}  // end Enter()

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
void HighPressMode::Exit(void)
{
    causeOfAlarm = eNoEventID;
    // Initialize pointer to the TriggerMgr
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable triggers.
    triggerMgrPtr->DisableTrigger(eHighMAP_2AutoResetTrigger);
    triggerMgrPtr->DisableTrigger(eUpdateStatusTimeTrigger);

    // Resume auto-zero scheduling.
    //    PInhAzTestPtr->ResumeTest();
    //    PExhAzTestPtr->ResumeTest();

    //    QMetaObject::invokeMethod(MainHandlerDelegate::S_GetInstance(), "EmitBlinkingModeSignal",
    //                              Q_ARG(int, (int)FromModeId),Q_ARG(bool,false));
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
void HighPressMode::UpdateAlarmStatus(void)
{
    TriggerMgr* triggerMgrPtr;
    if(causeOfAlarm != eNoEventID)
    {
        AlarmTask::S_GetInstance()->SetIsActive(causeOfAlarm, eActive);
        {
            triggerMgrPtr = TriggerMgr::S_GetInstance();
            triggerMgrPtr->DisableTrigger(eUpdateStatusTimeTrigger);
            triggerMgrPtr->EnableTrigger(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);
        }
    }
}   // end UpdateAlarmStatus()
