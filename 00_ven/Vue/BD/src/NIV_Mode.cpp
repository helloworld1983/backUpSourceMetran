/******************************************************************************/
//$COMMON.CPP$
//   File Name:  NIV_Mode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:
//      The operations in this class define the NIV_Mode mode
//
//   Interfaces:
//
//   Restrictions:
//      There is only 1 object instantiated from this class.
/******************************************************************************/
#include "NIV_Mode.h"
#include "PhaseMgr.h"
#include "TriggerMgr.h"
#include "MgrBdSetting.h"
#include "MgrController.h"

NIV_Mode* NIV_Mode::S_Instance = NULL;

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
NIV_Mode* NIV_Mode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new NIV_Mode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: NIV_Mode()
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
NIV_Mode::NIV_Mode(void): MixedMode()
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
void NIV_Mode::Enter(E_VentilationMode fromModeId)
{
    MixedMode::Enter(fromModeId);

    PhaseMgr::S_GetInstance()->ProcessNewVentType(ePressureVentilation);

    MgrController::CMVServoController->Enable();

    (PhaseMgr::S_GetInstance())->ProcessNewTriggerType(ePressureTrigger);

    PhaseMgr::S_GetInstance()->ProcessNewPhase(eExhPhase);

    TriggerMgr::S_GetInstance()->DisableTrigger(eBpTimeTrigger);

    UNSIGNED breathPeriod =  TriggerMgr::S_GetInstance()->GetTimePeriod(eNIVBpTimeTrigger);

    TriggerMgr::S_GetInstance()->EnableTrigger(eNIVBpTimeTrigger,breathPeriod);

    (TriggerMgr::S_GetInstance())->DisableTrigger(ePtInhFlowTrigger);
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessManualInhOpReqTrigger()
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
void NIV_Mode::ProcessManualInhOpReqTrigger()
{
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // Operator initiated inhalation triggers should only fire
    // during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    TriggerMgr::S_GetInstance()->ReEnableTrigger(eNIVBpTimeTrigger);

    // Start a mandatory inhalation.
    phaseMgrPtr->ProcessNewPhase(eMandInhPhase);

    Window = eMandWindow;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of NIV_Mode
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
void NIV_Mode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eManualInhOpReqTrigger:
            ProcessManualInhOpReqTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}
