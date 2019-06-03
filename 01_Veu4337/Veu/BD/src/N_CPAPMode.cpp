/******************************************************************************/
//$COMMON.CPP$
//   File Name:  N_CPAPMode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:
//      This file contains the definition of the N_CPAPMode.
//
//   Interfaces:
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
#include "N_CPAPMode.h"
#include "TriggerMgr.h"
#include "PhaseMgr.h"

#include "MgrDiscreteOutput.h"
#include "NCPAPController.h"
#include "MgrFlowController.h"

#include "DebugTrace.h"

N_CPAPMode* N_CPAPMode::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the N_CPAPMode object, it calls N_CPAPMode::S_GetInstance(),
//      which returns a pointer to the N_CPAPMode object.
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
//      NoGasMode* - pointer to the object instantiated from this class
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
N_CPAPMode* N_CPAPMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new N_CPAPMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: NormBreathingMode()
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
N_CPAPMode::N_CPAPMode():NormBreathingMode()
{
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
void N_CPAPMode::Enter(E_VentilationMode fromModeId)
{
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    //enable trigger
    triggerMgrPtr->EnableTrigger(eModeChangeTrigger);
    triggerMgrPtr->EnableTrigger(eAlarmResetOpReqTrigger);
    triggerMgrPtr->EnableTrigger(eTotalGasLossTrigger);
    triggerMgrPtr->EnableTrigger(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);
    triggerMgrPtr->EnableTrigger(eHardwareErrorAlarmTriger);

    PhaseMgr::S_GetInstance()->ProcessNewPhase(eNCPAPPhase);

    NCPAPController::S_GetInstance()->ResetIMVStaticVariables();
    MgrFlowController::AirFlowController->Enable();
    MgrFlowController::O2FlowController->Enable();
    NCPAPController::S_GetInstance()->Enable();

    //Close safe valve
    MgrDiscreteOutput::SafetyValve->SetDesiredState(eClose);
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Exit()
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
void N_CPAPMode::Exit()
{
    MgrFlowController::AirFlowController->Disable();
    MgrFlowController::O2FlowController->Disable();
    NCPAPController::S_GetInstance()->Disable();
    MgrDiscreteOutput::SafetyValve->SetDesiredState(eOpen); //eOn
}
