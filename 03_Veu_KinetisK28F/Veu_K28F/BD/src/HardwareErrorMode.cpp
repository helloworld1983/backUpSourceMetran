/******************************************************************************/
//$COMMON.CPP$
//   File Name:  HardwareErrorMode.cpp
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
#include "HardwareErrorMode.h"
#include "TriggerMgr.h"
#include "PhaseMgr.h"
#include "AlarmTask.h"
#include "AzTest.h"
#include "ModeMgr.h"
#include "DebugTrace.h"
#include "MgrDiscreteOutput.h"
#include "MgrController.h"
#include "HfoServo.h"
// Initialize statics

HardwareErrorMode* HardwareErrorMode::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the HardwareErrorMode object, it invokes HardwareErrorMode::S_GetInstance(),
//      which returns a pointer to the HardwareErrorMode object.
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
//      HardwareErrorMode* - pointer to the object instantiated from this class
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
HardwareErrorMode* HardwareErrorMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new HardwareErrorMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: HardwareErrorMode()
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
HardwareErrorMode::HardwareErrorMode(void):NonVenMode(eTrue)
{
    causeOfAlarm = eNoEventID;
}   // end HardwareErrorMode()

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
void HardwareErrorMode::Enter(E_VentilationMode fromModeId)
{
    MgrDiscreteOutput::SafetyValve->SetDesiredState(eOpen);

    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    triggerMgrPtr->DisableAllTriggers();
    triggerMgrPtr->DisableTrigger(eHardwareErrorAlarmTriger);
    triggerMgrPtr->EnableTrigger(eModeChangeTrigger);
    triggerMgrPtr->EnableTrigger(eHardwareErrorAutoresetTrigger);
    triggerMgrPtr->EnableTrigger(eAlarmResetOpReqTrigger);
    triggerMgrPtr->DisableTrigger(eUpdateStatusTimeTrigger);
    triggerMgrPtr->EnableTrigger(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);

//    BdTask::S_GetInstance()->Send(eSuspendAutoSetRequest);

    //	Suspend auto-zero scheduling.
//    PInhAzTestPtr->SuspendTest();
//    PExhAzTestPtr->SuspendTest();
    MgrController::CMVServoController->Disable();

//    HfoController->Disable();
    HfoServo::S_GetInstance()->Disable();


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
void HardwareErrorMode::Exit(void)
{
    // Update the High Press alarm status
    if(causeOfAlarm != eNoEventID)
    {
        AlarmTask::S_GetInstance()->SetIsActive(causeOfAlarm, eNotActive);
    }
    causeOfAlarm = eNoEventID;
    // Initialize pointer to the TriggerMgr
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable triggers.
    triggerMgrPtr->DisableTrigger(eHardwareErrorAutoresetTrigger);
    triggerMgrPtr->DisableTrigger(eUpdateStatusTimeTrigger);
    triggerMgrPtr->EnableTrigger(eHardwareErrorAlarmTriger);

    // Resume auto-zero scheduling.
//    PInhAzTestPtr->ResumeTest();
//    PExhAzTestPtr->ResumeTest();

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
void HardwareErrorMode::UpdateAlarmStatus(void)
{

    E_ReturnStatus status = eError;
    TriggerMgr* triggerMgrPtr;
    if(causeOfAlarm != eNoEventID)
    {
        status = AlarmTask::S_GetInstance()->SetIsActive(causeOfAlarm, eActive);


        // If the status update was unsuccessful, reschedule the next update
        // for the short interval
        if (status != eSuccess)
        {
            triggerMgrPtr = TriggerMgr::S_GetInstance();
            triggerMgrPtr->DisableTrigger(eUpdateStatusTimeTrigger);
            triggerMgrPtr->EnableTrigger(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);
        }
    }

}   // end UpdateAlarmStatus()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of Hardware Error Mode
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
void HardwareErrorMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eHardwareErrorAutoresetTrigger:
            ProcessHardwareErrorAutoresetTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name:ProcessHardwareErrorAutoresetTrigger()
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
//      LONG PIP
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
void HardwareErrorMode::ProcessHardwareErrorAutoresetTrigger(void)
{
    (ModeMgr::S_GetInstance())->GoToOperatorSetMode();
}
