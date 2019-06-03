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
#include "PCV_CPAP_Mode.h"
#include "PhaseMgr.h"
#include "MgrController.h"
#include "DebugTrace.h"

PCV_CPAP_Mode* PCV_CPAP_Mode::S_Instance = NULL;

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
PCV_CPAP_Mode* PCV_CPAP_Mode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new PCV_CPAP_Mode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PCV_CPAP_Mode()
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
PCV_CPAP_Mode::PCV_CPAP_Mode(void): SpontMode()
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
void PCV_CPAP_Mode::Enter(E_VentilationMode fromModeId)
{
    (PhaseMgr::S_GetInstance())->ProcessNewVentType(ePressureVentilation);
    if(fromModeId != eApneaBackupMode)
    {
        MgrController::CMVServoController->ResetIMVStaticVariables();
    }

    SpontMode::Enter(fromModeId);
    MgrController::CMVServoController->Enable();

    PhaseMgr::S_GetInstance()->ProcessNewPhase(eExhPhase);
}
