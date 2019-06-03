/******************************************************************************/
//$COMMON.CPP$
//   File Name:  PCV_CPAP_Mode.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:
//      The operations in this class define the VA_AC_Mode mode
//
//   Interfaces:
//
//   Restrictions:
//      There is only 1 object instantiated from this class.
/******************************************************************************/
#include "VA_AC_Mode.h"
#include "TriggerMgr.h"
#include "PhaseMgr.h"

#include "ptdata.h"
#include "ModeMgr.h"
#include "MgrController.h"
#include "ProxyFlowSensor.h"
#include "MgrDiscreteOutput.h"
#include "InhVABreathPhase.h"

#include "DebugTrace.h"

#define NUM_TEST_BREATH 3

VA_AC_Mode* VA_AC_Mode::S_Instance = NULL;
BreathType VA_AC_Mode::mBreathType = eTestBreath;
USHORT VA_AC_Mode::mTestBreathCount = 0;

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
VA_AC_Mode* VA_AC_Mode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new VA_AC_Mode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: VA_AC_Mode()
//
//    Processing: COnstructor
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
VA_AC_Mode::VA_AC_Mode(void) : MixedMode()
{
    prevProxyStatus = eNotCalibrated;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter()
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
void VA_AC_Mode::Enter(E_VentilationMode fromModeId)
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
    if((Status == eCalibrated)||(Status == eResetAndRecovered))
    {
        TriggerMgr::S_GetInstance()->EnableTrigger(eProxyDisconnectTrigger);

        prevProxyStatus = eCalibrated;
    }
    else
    {
        TriggerMgr::S_GetInstance()->EnableTrigger(eProxyConnectTrigger);

        prevProxyStatus = eNotCalibrated;

    }
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
void VA_AC_Mode::Exit(void)
{
//    QMetaObject::invokeMethod(MainHandlerDelegate::S_GetInstance(), "EmitAlarmModeSignal",
//                             Q_ARG(int, (int)ePcvACMode),Q_ARG(bool,false));

    TriggerMgr::S_GetInstance()->DisableTrigger(eProxyConnectTrigger);
    TriggerMgr::S_GetInstance()->DisableTrigger(eProxyDisconnectTrigger);
    MgrDiscreteOutput::SafetyValve->SetDesiredState(eOpen);
    MgrController::CMVServoController->Disable();
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: StartTestBreath()
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
void VA_AC_Mode::StartTestBreath(void)
{
    mTestBreathCount = 0;
    mBreathType = eTestBreath;
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBpTimeTrigger()
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
void VA_AC_Mode::ProcessBpTimeTrigger(void)
{
    E_PhaseId phaseId;          // Current Phase ID
    PhaseMgr* phaseMgrPtr;      // Pointer to the PhaseMgr object
    switch (Window)
    {
    case eMandWindow:
        // Initialize pointer to the PhaseMgr object
        phaseMgrPtr = PhaseMgr::S_GetInstance();

        // Make sure the ventilator is in the exhalation phase.
        phaseId = phaseMgrPtr->GetCurrentPhaseId();
        ASSERTION(phaseId == eExhPhase);

        ProcessInhPhase();
        break;

    case eSpontWindow:
        // If the breath period fires while the spontaneous window is open,
        // the ventilator just re-opens the mandatory window.
        Window = eMandWindow;
        break;

    default:
        // Trigger fired in wrong window
        ASSERTION((Window == eSpontWindow) || (Window == eMandWindow));
        break;
    }

}   // end ProcessBpTimeTrigger()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessInhPhase()
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
void VA_AC_Mode::ProcessInhPhase(void)
{
    PhaseMgr* phaseMgrPtr;      // Pointer to the PhaseMgr object

//     Initialize pointer to the PhaseMgr object
    phaseMgrPtr = PhaseMgr::S_GetInstance();

    ProxyStatus Status = ProxyFlowSensor::S_GetInstance()->getLastStatus();
    if((Status ==eCalibrated)||(Status==eResetAndRecovered))
    {
        if(prevProxyStatus == eNotCalibrated)
        {
            double Compliance = 0;//MainHandlerDelegate::S_GetInstance()->GetNumModel(eCompliancePtDataID)->GetValue();
            if(Compliance < 0.1*SCALE)
            {
                mTestBreathCount = 1;
                phaseMgrPtr->ProcessNewPhase(eMandInhTestBreathPhase);
                InhVABreathPhase::S_GetInstance()->ClearPIDParameter();
            }
            else
            {
                prevProxyStatus = eCalibrated;
            }

        }
        else
        {
            if(NUM_TEST_BREATH > mTestBreathCount)
            {
                mTestBreathCount++;
                phaseMgrPtr->ProcessNewPhase(eMandInhTestBreathPhase);
            }
            else
            {
                phaseMgrPtr->ProcessNewPhase(eMandInhVABreathPhase);
            }
        }

    }
    else
    {
        prevProxyStatus = eNotCalibrated;

        phaseMgrPtr->ProcessNewPhase(eMandInhPhase);
        mTestBreathCount = 0;
        mBreathType = eTestBreath;
        InhVABreathPhase::S_GetInstance()->ClearPIDParameter();
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
void VA_AC_Mode::ProcessPtInhTrigger(void)
{
    // Initialize pointer to the PhaseMgr object
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // A patient trigger should only fire during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Start a mandatory inhalation
    phaseMgrPtr->ProcessNewPhase(eMandInhVABreathPhase);

    // Close the mandatory window; open the spontaneous window.
    Window = eMandWindow;

    TriggerMgr::S_GetInstance()->DisableTrigger(eBpTimeTrigger);
    UNSIGNED breathPeriod = TriggerMgr::S_GetInstance()->GetTimePeriod(eBpTimeTrigger);

    TriggerMgr::S_GetInstance()->EnableTrigger(eBpTimeTrigger,breathPeriod);


}   // end ProcessPtInhTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessProxyDisconnectTrigger()
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
void VA_AC_Mode::ProcessProxyDisconnectTrigger()
{
    prevProxyStatus = eNotCalibrated;

    UpdatePIPSetting();

    E_VentilationMode currentMode = ModeMgr::S_GetInstance()->GetCurrentModeId();
    if(currentMode == eVaACMode)
    {
//        QMetaObject::invokeMethod(MainHandlerDelegate::S_GetInstance(), "EmitAlarmModeSignal",
//                                 Q_ARG(int, (int)ePcvACMode),Q_ARG(bool,true));
    }
    else if(currentMode == eVaMixedMode)
    {
//        QMetaObject::invokeMethod(MainHandlerDelegate::S_GetInstance(), "EmitAlarmModeSignal",
//                                 Q_ARG(int, (int)ePcvMixedMode),Q_ARG(bool,true));
    }

}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessProxyConnectTrigger()
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
void VA_AC_Mode::ProcessProxyConnectTrigger()
{
//    QMetaObject::invokeMethod(MainHandlerDelegate::S_GetInstance(), "EmitAlarmModeSignal",
//                             Q_ARG(int, (int)ePcvACMode),Q_ARG(bool,false));
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessApneaAlarmTimeTrigger()
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
void VA_AC_Mode::ProcessApneaAlarmTimeTrigger(void)
{
//    QMetaObject::invokeMethod(MainHandlerDelegate::S_GetInstance(), "EmitAlarmModeSignal",
//                             Q_ARG(int, (int)ePcvACMode),Q_ARG(bool,false));
    (ModeMgr::S_GetInstance())->GoToAlarmMode(eApneaVAMode);
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdatePIPSetting()
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
void VA_AC_Mode::UpdatePIPSetting()
{
    LONG pipTarget = InhVABreathPhase::S_GetInstance()->GetPIPTarget();

    LONG tmp = pipTarget % 100;

    if(tmp > 50)
    {
        pipTarget = pipTarget + (100 - tmp);//  round up
    }
    else if(tmp < 50)
    {
        pipTarget = pipTarget - tmp;//  round down
    }

//    MainHandlerDelegate::S_GetInstance()->GetNumSetting(ePCVPressureSettingID)->SetValue(pipTarget);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of VA_AC_Mode
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
void VA_AC_Mode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eBpTimeTrigger:
            ProcessBpTimeTrigger();
            break;

//        case ePtInhTrigger:
//            ProcessPtInhTrigger();
//            break;

        case eProxyDisconnectTrigger:
            ProcessProxyDisconnectTrigger();
            break;

        case eProxyConnectTrigger:
            ProcessProxyConnectTrigger();
            break;

        case eApneaAlarmTimeTrigger:
            ProcessApneaAlarmTimeTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}
