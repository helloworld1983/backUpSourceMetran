/******************************************************************************/
//$COMMON.CPP$
//   File Name:  DisconnectionMode.cpp
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
#include "DisconnectionMode.h"
#include "TriggerMgr.h"
#include "AlarmTask.h"
#include "ModeMgr.h"
#include "PhaseMgr.h"
#include "AzTest.h"
#include "MgrController.h"

#include "DebugTrace.h"
// Initialize statics

DisconnectionMode* DisconnectionMode::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the DisconnectionMode object, it invokes DisconnectionMode::S_GetInstance(),
//      which returns a pointer to the DisconnectionMode object.
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
//      DisconnectionMode* - pointer to the object instantiated from this class
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
DisconnectionMode* DisconnectionMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new DisconnectionMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DisconnectionMode()
//
//    Processing: 
//      This operation is the constructor for the disconnection mode.  It invokes 
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
DisconnectionMode::DisconnectionMode(void):AlarmBreathingMode(eTrue)
{

}   // end DisconnectionMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter()
//
//    Processing: 
//      This operation is called whenever an Disconected trigger fires,enables 
//		the Reconected trigger, AlarmResetOpReq trigger and updates the Disconected 
//		Circuit alarm status.
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
void DisconnectionMode::Enter(E_VentilationMode fromModeId)
{
    // Initialize pointer to the TriggerMgr
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // Put the ventilator into the eExhPhase phase.
    phaseMgrPtr->ProcessNewPhase(eExhPhase);
    
    // Disable some triggers.  These call must be made after forcing the vent
    // into the exhalaion phase above because depending on the phase
    // transitioned from, there may be some triggers left enabled.
    triggerMgrPtr->DisableTrigger(eManualInhOpReqTrigger);
    triggerMgrPtr->DisableTrigger(ePtInhPressTrigger);
    triggerMgrPtr->DisableTrigger(ePtInhFlowTrigger);

    // Enable the Update Status trigger which causes the update of the
    // status of the alarms associated with the active Disconnected mode.
    triggerMgrPtr->EnableTrigger(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);

    // Enable the Reconected trigger.
    triggerMgrPtr->EnableTrigger(eReconnectionTrigger);

    // Enable the reset trigger
    triggerMgrPtr->EnableTrigger(eAlarmResetOpReqTrigger);

    // Update the alarm status to indicate that the total gas loss alarm is
    // not active.

    if(AlarmTask::S_GetInstance()->GetIsActive(eGasSuppliesLost) == eActive)
    {
        AlarmTask::S_GetInstance()->SetIsActive(eGasSuppliesLost, eNotActive);
    }

    // Update the Disconnected Circuit alarm status.
    AlarmTask::S_GetInstance()->SetIsActive(eDisconnectedCircuit, eActive);

    MgrController::CMVServoController->Enable();

    // Suspend auto-zero scheduling.
//    PInhAzTestPtr->SuspendTest();
//    PExhAzTestPtr->SuspendTest();


}   // end Enter()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Exit()
//
//    Processing: 
//      This operation is invoked when the reconnection condition is reset and
//      the ventilator is to exit the Disconnection mode. This operation disables
//      the Reconnection trigger, and updates the Disconnected circuit alarm status.
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
void DisconnectionMode::Exit(void)
{

    // Initialize pointer to the TriggerMgr
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable triggers.
    triggerMgrPtr->DisableTrigger(eReconnectionTrigger);
    triggerMgrPtr->DisableTrigger(eUpdateStatusTimeTrigger);

    // Update the Disconnected Circuit alarm status
    AlarmTask::S_GetInstance()->SetIsActive(eDisconnectedCircuit, eNotActive);

    MgrController::CMVServoController->Disable();


    // Resume auto-zero scheduling.    
//    PInhAzTestPtr->ResumeTest();
//    PExhAzTestPtr->ResumeTest();

}   // end Exit()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessReconectedTrigger()
//
//    Processing: 
//      This operation handles the processing of the Reconected Trigger for the 
//		Disconected mode. If so, ModeMgr::GoToLastMode() is called to return the
//		ventilator to the previous mode or the operator set mode.
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
void DisconnectionMode::ProcessReconnectionTrigger(void)
{
    ModeMgr* modeMgrPtr = ModeMgr::S_GetInstance();
    modeMgrPtr->ProcessAutoReset();
    ASSERTION((PhaseMgr::S_GetInstance())->GetCurrentPhaseId() == eExhPhase);
    modeMgrPtr->ReturnToLastMode();
    
}   // end ProcessReconectedTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of Disconnection Mode
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
void DisconnectionMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eReconnectionTrigger:
            ProcessReconnectionTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdateAlarmStatus()
//
//    Processing: 
//      This operation is invoked each time the UpdateStatus TimeTrigger fires.
//      It updates the Disconnected Circuit alarm status to indicate that 
//		ET disconnection is still active.
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
void DisconnectionMode::UpdateAlarmStatus(void)
{
    E_ReturnStatus status;
    TriggerMgr* triggerMgrPtr;
    
    // Update the alarm status to indicate that the disconnected circuit alarm
    // is active.
    status = AlarmTask::S_GetInstance()->SetIsActive(eDisconnectedCircuit, eActive);

    // If the status update was unsuccessful, reschedule the next update 
    // for the short interval
    if (status != eSuccess)
    {
        triggerMgrPtr = TriggerMgr::S_GetInstance();
        triggerMgrPtr->DisableTrigger(eUpdateStatusTimeTrigger);
        triggerMgrPtr->EnableTrigger(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);
    }

}   // end UpdateAlarmStatus()
