/******************************************************************************/
//$COMMON.CPP$
//   File Name:  PCV_CPAP_Mode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:
//      The operations in this class define the VA_SIMV_Mode mode
//
//   Interfaces:
//
//   Restrictions:
//      There is only 1 object instantiated from this class.
/******************************************************************************/
#include "VA_SIMV_Mode.h"
#include "TriggerMgr.h"

#include "ProxyFlowSensor.h"
#include "InhVABreathPhase.h"
#include "MgrController.h"
//#include "mainhandlerdelegate.h"

#include "DebugTrace.h"

VA_SIMV_Mode* VA_SIMV_Mode::S_Instance = NULL;

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
VA_SIMV_Mode* VA_SIMV_Mode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new VA_SIMV_Mode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetInhPhaseType()
//
//    Processing: Get Inh Phase Type
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
//
//    Requirements:
//
/******************************************************************************/
E_PhaseId VA_SIMV_Mode::GetInhPhaseType(void)
{
    return InhPhaseType;
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: VA_SIMV_Mode()
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
VA_SIMV_Mode::VA_SIMV_Mode(void) : VA_AC_Mode()
{
    InhPhaseType = eMandInhVABreathPhase;
}
// end ProcessBpTimeTrigger()

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
void VA_SIMV_Mode::Enter(E_VentilationMode fromModeId)
{
    MixedMode::Enter(fromModeId);
    PhaseMgr::S_GetInstance()->ProcessNewVentType(ePressureVentilation);

    if(fromModeId != eApneaVAMode)
    {
        MgrController::CMVServoController->ResetIMVStaticVariables();
        InhVABreathPhase::S_GetInstance()->ClearPIDParameter();
        mTestBreathCount = 0;
        mBreathType = eTestBreath;
    }
    MgrController::CMVServoController->Enable();

    PhaseMgr::S_GetInstance()->ProcessNewPhase(eExhPhase);

    ProxyStatus Status = ProxyFlowSensor::S_GetInstance()->getLastStatus();
    if((Status ==eCalibrated)||(Status==eResetAndRecovered))
    {
        TriggerMgr::S_GetInstance()->EnableTrigger(eProxyDisconnectTrigger);
        prevProxyStatus = eCalibrated;
    }
    else
    {
        prevProxyStatus = eNotCalibrated;
        TriggerMgr::S_GetInstance()->EnableTrigger(eProxyConnectTrigger);
    }
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessPtInhTrigger()
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
void VA_SIMV_Mode::ProcessPtInhTrigger()
{
    // Initialize pointer to the PhaseMgr object
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // A patient trigger should only fire during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Start a new inhalation.  The type of inhalation (mand or
    // spont) depends upon the window that is currently
    // open.  If the mandatory window is open, deliver a mand
    // breath and close the mandatory window since this breath
    // will meet the mandatory breath criteria for this breath
    // period
    if (eMandWindow == Window)
    {
        // Start a mandatory inhalation
        phaseMgrPtr->ProcessNewPhase(eMandInhVABreathPhase);

        // Close the mandatory window; open the spontaneous window.
        Window = eSpontWindow;

        InhPhaseType = eMandInhVABreathPhase;
    }

    else if (eSpontWindow == Window)
    {
#ifdef VA_WITH_PS //for new VA
        phaseMgrPtr->ProcessNewPhase(ePsvInhVABreathPhase);

        InhPhaseType = ePsvInhVABreathPhase;
#else
        phaseMgrPtr->ProcessNewPhase(eSpontInhPhase);

        InhPhaseType = eSpontInhPhase;
#endif
    }
    else
    {
        // Window contains illegal value
        ASSERTION((Window == eMandWindow) || (Window == eSpontWindow));
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of VA_SIMV_Mode
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
void VA_SIMV_Mode::ProcessTriggerID (E_TriggerId id)
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
