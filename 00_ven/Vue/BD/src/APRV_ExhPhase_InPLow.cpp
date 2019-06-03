/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: APRV_ExhPhase_InPLow()
//
//    Purpose:
//     The APRV_ExhPhase_InPLow is the phase that occurs only in APRV mode after
//     an Spont Inh in PLow Phase
//
//
//       _____________________________________                                                   ___________________________
//      |                                     |                   ________                      |                          |
//   ___|                                     |__________________|        |_____________________|                          |_______
//             APRV_ExhPhase_InPHigh
//
//                                                             Supported Inh
//                                                                         APRV_ExhPhase_InPLow
//        APRV_PHighPhase                       APRV_PLowPhase                                         APRV_PHighPhase
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
#include "APRV_ExhPhase_InPLow.h"
#include "PhaseMgr.h"
#include "TriggerMgr.h"
#include "MgrBdSetting.h"
#include "MgrBreathData.h"
#include "CommonTypes.h"
#include "ModeMgr.h"
#include "InhPhase.h"
#include "MgrController.h"
#include "PressureSensor.h"
#include "DebugTrace.h"

// Define statics
UNSIGNED APRV_ExhPhase_InPLow::S_ExhStartTime = 0;
UNSIGNED APRV_ExhPhase_InPLow::S_TimeElapsed = 0;
UNSIGNED APRV_ExhPhase_InPLow::S_Time = 0;
APRV_ExhPhase_InPLow* APRV_ExhPhase_InPLow::S_Instance = NULL;

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
APRV_ExhPhase_InPLow* APRV_ExhPhase_InPLow::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new APRV_ExhPhase_InPLow();
        ASSERTION(S_Instance != NULL);
    }
    return(S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetTimeDuration()
//
//    Processing:   Get time duration
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//       time duration
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
UNSIGNED APRV_ExhPhase_InPLow::GetTimeDuration (void)
{
    return S_Time;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ResetTimeDuration()
//
//    Processing:   Reset time duration
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//       time duration
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
void APRV_ExhPhase_InPLow::ResetTimeDuration (void)
{
    S_Time = 0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: APRV_ExhPhase_InPLow()
//
//    Processing:
//      This operation is the constructor for the APRV_ExhPhase_InPLow.  It invokes the
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
APRV_ExhPhase_InPLow::APRV_ExhPhase_InPLow(void) : Phase()
{
    TargetPressure = 0;
}   // end ExhPhase()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing:
//      This operation is invoked at the end of every APRV_SupportedInhPhase_InPLow.
//      It perfroms processing common to all exhalation phases.
//
//      This operation invokes the TriggerMgr::Enable() operation to
//      enable Inhalation Pressure Trigger In PLow,Inhalation Flow Trigger and sends the target pressure (PLow Pressure)
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
void APRV_ExhPhase_InPLow::Start(void)
{
    MgrBdSetting* mgrBdSettingPtr;  // pointer to MgrBdSetting

    // Get a handle to the BD Setting Manager
    mgrBdSettingPtr = MgrBdSetting::S_GetInstance();


    SHORT BiasFlow = mgrBdSettingPtr->GetCurrentValue(eBaseFlow);
    SHORT TempBiasFlow = BiasFlow / 100;
    DeviceInterface::S_GetInstance()->RequestDevice(eCMVServoControllerSetBiasFlow, &TempBiasFlow);

    // Save the target pressure so that it does not have to be retrieved
    // every BD cycle.

    TargetPressure = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVLowInhPress);

    MgrController::CMVServoController->SetExhalationTarget(TargetPressure/10);

    // Snapshot the current system time as the start of exhalation and reset
    // the elapsed time
//    S_ExhStartTime = SystemClock::Retrieve();
    S_TimeElapsed = 0;

    mgrBdSettingPtr->ProcessNewPhase();
    // Update Patient Data
    (MgrBreathData::S_GetInstance())->APRV_ExhPhaseStart_InPLow(eExhalation);


    TriggerMgr::S_GetInstance()->EnableExhaleTrigger();

    TriggerMgr::S_GetInstance()->EnableTrigger(ePtInhFlowTrigger);

    TriggerMgr::S_GetInstance()->EnableTrigger(eAPRV_PtInhPressTrigger_InPLow);
}   // end Start()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing:
//      This operation is invoked at the end of every exhalation phase.
//      It perfroms processing common to all exhalation phases.
//
//      This operation invokes the TriggerMgr::DisableTrigger() operation to
//      disable the Manual Inhalation Op Request Trigger, the Patient
//      Inhalation Pressure Trigger In PLow and Compute the duration time of all the APRV_ExhPhase_InPLow
//      phases in a APRV PLow Phase.
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
void APRV_ExhPhase_InPLow::End(void)
{
    // Pointer to the Trigger Manager
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtr->DisableTrigger(eManualInhOpReqTrigger);

    triggerMgrPtr->DisableTrigger(ePtInhFlowTrigger);

    triggerMgrPtr->DisableTrigger(eAPRV_PtInhPressTrigger_InPLow);

    PressureSensor::ActivePressureSensor->SetPrimary(PressureSensor::InhPressSensor);

    S_Time += APRV_ExhPhase_InPLow::S_CalcTimeSincePhaseStart();

}   // end End()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBdCycle()
//
//    Processing:
//      This operation is called every BD task cycle during exhalation.  This
//      operation performs processing common to all exhalation phases.  This
//      operation calculates the desired pressure and sends the command to the
//      Exhalation Pressure controller.
//
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
void APRV_ExhPhase_InPLow::ProcessBdCycle(void)
{
    // Calculate the time elapsed since the start of exhalation.
     MgrBdSetting::S_GetInstance()->PhaseInAllSettings();

//     S_TimeElapsed = APRV_PLowPhase::S_CalcTimeSincePhaseStart();

     TargetPressure = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVLowInhPress);

     MgrController::CMVServoController->SetExhalationTarget(TargetPressure/10);

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
E_Bool APRV_ExhPhase_InPLow::S_IsInUnrestrictedPhase(UNSIGNED minExhTime)
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
    if (((PhaseMgr::S_GetInstance())->GetCurrentPhaseId() == eAPRVExhPhase_InPLow) &&
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
UNSIGNED APRV_ExhPhase_InPLow::S_CalcTimeSincePhaseStart(void)
{

    // Return the time elapsed since the start of the exhalation phase.
    return(CalcElapsedTime(S_ExhStartTime));

}   // end S_CalcTimeSincePhaseStart()
