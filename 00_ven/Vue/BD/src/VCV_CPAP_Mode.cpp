#include "VCV_CPAP_Mode.h"
#include "TriggerMgr.h"
#include "PhaseMgr.h"
#include "MgrController.h"

#include "DebugTrace.h"

VCV_CPAP_Mode* VCV_CPAP_Mode::S_Instance = NULL;

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
VCV_CPAP_Mode* VCV_CPAP_Mode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new VCV_CPAP_Mode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: VCV_CPAP_Mode()
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
VCV_CPAP_Mode::VCV_CPAP_Mode(void) : SpontMode()
{
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter(E_VentilationMode fromModeId)
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
void VCV_CPAP_Mode::Enter(E_VentilationMode fromModeId)
{
    (PhaseMgr::S_GetInstance())->ProcessNewVentType(eVolumeVentilation);
    SpontMode::Enter(fromModeId);

    if(fromModeId != eApneaBackupMode)
    {
        MgrController::VCVServoController->ResetIMVStaticVariables();
        PhaseMgr::S_GetInstance()->ResetVolumeAdjustmentOfVCVInhPhase();
    }

    MgrController::VCVServoController->Enable();
    PhaseMgr::S_GetInstance()->ProcessNewPhase(eExhPhase);
}
