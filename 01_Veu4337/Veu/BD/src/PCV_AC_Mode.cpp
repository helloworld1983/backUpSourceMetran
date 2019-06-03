/******************************************************************************/
//$COMMON.CPP$
//   File Name:  PCV_CPAP_Mode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:
//      The operations in this class define the PCV_AC_Mode mode
//
//   Interfaces:
//
//   Restrictions:
//      There is only 1 object instantiated from this class.
/******************************************************************************/
#include <InhPhase.h>
#include "PCV_AC_Mode.h"
#include "PhaseMgr.h"
#include "TriggerMgr.h"
#include "Hip2AlarmTrigger.h"
#include "ModeMgr.h"
#include "MgrAlarmStat.h"
#include "Btps.h"
#include "MgrController.h"

PCV_AC_Mode* PCV_AC_Mode::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the PCV_AC_Mode object, it invokes PCV_AC_Mode::S_GetInstance(),
//      which returns a pointer to the PCV_AC_Mode object.
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
//      PCV_AC_Mode* - pointer to the object instantiated from this class
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
PCV_AC_Mode* PCV_AC_Mode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new PCV_AC_Mode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PCV_AC_Mode()
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
PCV_AC_Mode::PCV_AC_Mode(void) : MixedMode()
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
void PCV_AC_Mode::Enter(E_VentilationMode fromModeId)
{
    MixedMode::Enter(fromModeId);

//    PhaseMgr::S_GetInstance()->ProcessNewVentType(ePressureVentilation);
    if(fromModeId != eApneaBackupMode)
    {
//        MgrController::CMVServoController->ResetIMVStaticVariables();
    }

//    MgrController::CMVServoController->Enable();

    PhaseMgr::S_GetInstance()->ProcessNewPhase(eExhPhase);

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessPtInhTrigger()
//
//    Processing:
//      This operation handles the processing of the Patient Inhalation Trigger
//      for the Mixed Mode.
//
//      This operation retrieves the current phase from the PhaseMgr object.
//      If the current phase is exhalation, the Window attribute is checked.
//      If the mandatory window is open, PhaseMgr::ProcessNewPhase()
//      is called to transition the ventilator to a mandatory inhalation, and
//      the mandatory window is closed.
//
//      If the mandatory window is not open, ProcessNewPhase() is called to
//      transition the ventilator to a spontaneous inhalation.
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
//      If the breath phase is not exhalation or if the Window attribute
//      contains an illegal value, error handling is performed.
//
//    Requirements:
//
/******************************************************************************/
void PCV_AC_Mode::ProcessPtInhTrigger(void)
{

    ProcessPtInhTriggerAC();

}   // end ProcessPtInhTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of PCV_AC_Mode
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
void PCV_AC_Mode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
//        case ePtInhTrigger:
//            ProcessPtInhTrigger();
//            break;

        default:
        ASSERTION(eFalse);
            break;
    }
}
