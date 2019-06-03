/******************************************************************************/
//$COMMON.OPERATION$
//    Phase Name: APRV_PHighPhase Phase
//
//    Purpose:
//     The APRV_ExhPhase_InPHigh is the phase that occurs only in APRV mode.
//
//
//       __________________________________           ___________________________
//      |                                  |         |                          |
//   ___|                                  |_________|                          |_______
//
//               APRV_PHighPhase            APRV_PLowPhase
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
#include <DeviceInterface.h>
#include "APRV_PHighPhase.h"
#include "Exponential.h"
#include "PhaseMgr.h"
#include "TriggerMgr.h"
#include "MgrBdSetting.h"
#include "MgrBreathData.h"
#include "CommonTypes.h"
#include "MgrController.h"
#include "PressureSensor.h"


APRV_PHighPhase* APRV_PHighPhase::S_Instance = NULL;
UNSIGNED APRV_PHighPhase::S_InhStartTime = 0;
UNSIGNED APRV_PHighPhase::S_TimeElapsed = 0;
UNSIGNED APRV_PHighPhase::S_Time = 0;

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
APRV_PHighPhase* APRV_PHighPhase::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new APRV_PHighPhase();
        ASSERTION(S_Instance != NULL);
    }
    return(S_Instance);

}   // end S_GetInstance()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: APRV_PHighPhase()
//
//    Processing:
//      This operation is the constructor for the APRV_PHighPhase.  It invokes the
//      base class Phase constructor.
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
APRV_PHighPhase::APRV_PHighPhase(void) : InhPhase()
{
    RiseTime = 0;
    TargetPressure = 0;
}   // end ExhPhase()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing:
//      This operation is invoked at the end of every APRV_PLowPhase.
//
//      This operation invokes the TriggerMgr::Enable() operation to
//      enable Inhalation Pressure Trigger In PHigh,Inhalation Flow Trigger and sends the target pressure (PHigh Pressure)
//      to CMVServoController
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
void APRV_PHighPhase::Start(void)
{
    E_BreathPhaseIndicator breathType = eMandatory;

    MgrBdSetting* mgrBdSettingPtr;  // pointer to MgrBdSetting

    // Get a handle to the BD Setting Manager
    mgrBdSettingPtr = MgrBdSetting::S_GetInstance();


    SHORT BiasFlow = mgrBdSettingPtr->GetCurrentValue(eBaseFlow);

    SHORT TempBiasFlow = BiasFlow / 100;
    DeviceInterface::S_GetInstance()->RequestDevice(eCMVServoControllerSetBiasFlow, &TempBiasFlow);

    // Save the target pressure so that it does not have to be retrieved
    // every BD cycle.

    TargetPressure = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVHighInhPress);
    RiseTime = MgrBdSetting::S_GetInstance()->GetCurrentValue(eRiseTime);

    MgrController::CMVServoController->SetInhalationTarget(TargetPressure/10,RiseTime);

    // Snapshot the current system time as the start of exhalation and reset
    // the elapsed time
//    S_InhStartTime = SystemClock::Retrieve();
    S_TimeElapsed = 0;

    mgrBdSettingPtr->ProcessNewPhase();
    // Update Patient Data
    (MgrBreathData::S_GetInstance())->APRV_PHighPhaseStart(breathType);


    TriggerMgr::S_GetInstance()->EnableExhaleTrigger();
    TriggerMgr::S_GetInstance()->EnableTrigger(ePtInhFlowTrigger);
    TriggerMgr::S_GetInstance()->EnableTrigger(eAPRV_PtInhPressTrigger_InPHigh);

    TriggerMgr::S_GetInstance()->ReEnableTrigger(eApneaAlarmTimeTrigger);

    TriggerMgr::S_GetInstance()->EnableTrigger(eHPHighAlarmTrigger);
}   // end Start()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing:
//      This operation is invoked at the end of every APRV_PHighPhase phase.
//      It perfroms processing common to all exhalation phases.
//
//      This operation invokes the TriggerMgr::DisableTrigger() operation to
//      disable the Manual Inhalation Op Request Trigger, the Patient
//      Inhalation Pressure Trigger and Compute the duration time of the APRV_PHighPhase
//      phase.
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
void APRV_PHighPhase::End(void)
{
    // Pointer to the Trigger Manager
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtr->DisableTrigger(eManualInhOpReqTrigger);

    triggerMgrPtr->DisableTrigger(eExtremeHighPEEPTrigger);

    triggerMgrPtr->DisableTrigger(ePtInhFlowTrigger);

    TriggerMgr::S_GetInstance()->DisableTrigger(eAPRV_PtInhPressTrigger_InPHigh);

    PressureSensor::ActivePressureSensor->SetPrimary(PressureSensor::InhPressSensor);

    S_Time += APRV_PHighPhase::S_CalcTimeSincePhaseStart();
}   // end End()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBdCycle()
//
//    Processing:
//      This operation is called every BD task cycle during exhalation.  This
//      operation performs processing common to all exhalation phases.  This
//      operation calculates the desired pressure and sends the command to the
//      CMVServoController.
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
//      The exhalation pressure controller must already be enabled before
//      this operation is called.
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void APRV_PHighPhase::ProcessBdCycle(void)
{
    // Calculate the time elapsed since the start of exhalation.

     MgrBdSetting::S_GetInstance()->PhaseInAllSettings();

     S_TimeElapsed = APRV_PHighPhase::S_CalcTimeSincePhaseStart();

     TargetPressure = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVHighInhPress);
     RiseTime = MgrBdSetting::S_GetInstance()->GetCurrentValue(eRiseTime);

     MgrController::CMVServoController->SetInhalationTarget(TargetPressure/10,RiseTime);


     // If it is time to auto-zero the exhalation pressure transducer,
     // kick start the auto-zero, update the state and switch the active
     // pressure transducer to the inhalation pressure transducer.  The
     // ventilator usually uses Pexh for monitoring and control.  During the
     // phase where Pexh is being auto-zeroed, Pinh is used for monitoring
     // and control.  It is always switched back at the end of exhalation.
//     if (eAzPending == PInhAzTestPtr->GetAzTestState())
//     {
//         if ( ActivePressureSensor->GetLastReading() <= (TargetPressure + 200)  )
//         {
//             RT_TASK* receiveTask1 = NULL;
//             receiveTask1 = (AzTask::S_GetInstance())->S_SetEventFlag(INH_AZ_EVENT_FLAG);
//             ActivePressureSensor->SetPrimary(ExhPressSensor);
//             if((receiveTask1 !=0))
//                 PInhAzTestPtr->SetAzTestState(eAzActive);
//         }
//     }
//     else
//     {
//         if (eAzCompleted == PInhAzTestPtr->GetAzTestState())
//         {
//             ActivePressureSensor->SetPrimary(InhPressSensor);
//
//             // Otherwise if it is time to auto-zero Pinh, start the auto-zero and
//             // update the state.
//             if (eAzPending == PExhAzTestPtr->GetAzTestState())
//             {
//
//
//                 RT_TASK* receiveTask2 = NULL;
//                 receiveTask2 = (AzTask::S_GetInstance())->S_SetEventFlag(EXH_AZ_EVENT_FLAG);
//                 if((receiveTask2 !=0))
//                     PExhAzTestPtr->SetAzTestState(eAzActive);
//             }
//         }
//     }
}   // end ProcessBdCycle()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_IsInUnrestrictedPhase()
//
//    Processing:
//      This operation determines whether the ventilator is in the unrestricted
//      phase of exhalation.  The unrestricted phase of exhalation is defined
//      as the part of the exhalation phase after a minimum exhalation period
//      has elapsed.  The minimum exhalation time is defined by an input
//      argument.
//
//    Input Parameters:
//      minExhTime - minimum exhalation time comparing against
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the ventilator is at least minExhTime into
//          the exhalation phase
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
E_Bool APRV_PHighPhase::S_IsInUnrestrictedPhase(UNSIGNED minExhTime)
{
    UNSIGNED tmp = minExhTime;
    E_Bool rtnValue = eFalse;
    if(tmp >= 1000)
    {
        tmp = 1000;
    }
    else if(tmp < 200)
    {
        tmp = 200;
    }

    // If the breath phase is exhalation and the time elapsed since the start
    // of exhalation is greater than or equal to the minimum exhalation time,
    // return TRUE because the ventilator is in the unrestricted phase.
    if (((PhaseMgr::S_GetInstance())->GetCurrentPhaseId() == eAPRVHighPhase) &&
            (S_TimeElapsed >= tmp))
    {
        rtnValue = eTrue;
    }

    return(rtnValue);

}   // end S_IsInUnrestrictedPhase()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_CalcTimeSincePhaseStart()
//
//    Processing:
//      This operation invokes the CalcElapsedTime() utility to calculate
//      the time elapsed since the start of the exhalation phase.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      UNSIGNED - time in system ticks elapsed since the start of the
//          exhalation.
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
UNSIGNED APRV_PHighPhase::S_CalcTimeSincePhaseStart(void)
{
    // Return the time elapsed since the start of the exhalation phase.
    return(CalcElapsedTime(S_InhStartTime));

}   // end S_CalcTimeSincePhaseStart()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetTimeDuration()
//
//    Processing: get time duration
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      UNSIGNED - time duration.
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
UNSIGNED APRV_PHighPhase::GetTimeDuration(void)
{
    return S_Time;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetTimeElapsed()
//
//    Processing: get time elapsed
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      UNSIGNED - time elapsed.
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
UNSIGNED APRV_PHighPhase::GetTimeElapsed(void)
{
    return S_TimeElapsed;
}


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ResetTimeDuration()
//
//    Processing: Reset time duration
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
void APRV_PHighPhase::ResetTimeDuration(void)
{
    S_Time = 0;
}
