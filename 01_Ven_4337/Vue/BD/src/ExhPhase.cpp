/******************************************************************************/
//$COMMON.CPP$
//   File Name:  ExhPhase.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      ExhPhase is the base class for all Exhalation Phase classes.
//      Operations in this class perform basic functionality common to all
//      exhalation phases.
//
//   Interfaces:
//      At the start of each exhalation, ExhPhase::Start() is invoked which
//      interfaces with the TriggerMgr to enable and disable triggers common
//      to all exhalation phases.  The Start() operation also interfaces with 
//      the MgrBdSetting class to phase in settings at the start of the breath
//      phase.  Start() also invokes the OS kernel service 
//      SystemClock::Retrieve() to snapshot the exhalation start time, and
//      it calls MgrBreathData::ExhalationStart() to update data and notify
//      the Patient Data Task of the start of exhalation.
//
//      ExhPhase::S_IsInUnrestrictedPhase() interfaces with with the PhaseMgr 
//      object to determine the current breath phase by invoking
//      GetCurrentPhaseId().
//
//      ExhPhase::Start() and ExhPhase::ProcessBdCycle() invoke 
//      Exponential::GetExpTerm() to get the exponential term of the desired
//      pressure equation.
/******************************************************************************/
//#include "nucpp.h"
#include <ExhPhase.h>
#include <PhaseMgr.h>
#include "Exponential.h"
#include "TriggerMgr.h"
#include "MgrBdSetting.h"
#include "MgrBreathData.h"
#include "CommonTypes.h"

#include "MgrController.h"
#include "PressureSensor.h"

// Define statics
UNSIGNED ExhPhase::S_ExhStartTime = 0;
UNSIGNED ExhPhase::S_TimeElapsed = 0;

// The FALL_TIME defines the amount of time it takes for the exhalation 
// pressure to fall from the end inhalation pressure to the targeted baseline
// pressure.  This value is passed to Exponential::GetExpTerm() as it 
// is used to calculate the exponential term of the desired pressure equation.
//
// This value is 150 ms in system clock ticks
static const UNSIGNED FALL_TIME = 150 / MS_PER_TICK;

// These adjustment factors are applied to the desired pressure sent
// to the exhalation controller to help prevent under or over-shoot depending 
// upon the operator set baseline pressure and bias flow. 
// The values are scaled by 2**7
static const LONG BASELINE_ADJUST_HIGH = 136;   // 1.06
static const LONG BASELINE_ADJUST_LOW = 125;    // 0.98

// Baseline pressure and bias flow cutoff values used to determine which 
// adjustment factor to apply.
static const LONG BASELINE_CUTOFF = 20 * SCALE;     // 20 cmH2O
static const LONG BIAS_FLOW_CUTOFF = (20 * 1000 * SCALE) / SEC_PER_MIN; // 20 Lpm

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ExhPhase()
//
//    Processing: 
//      This operation is the constructor for the ExhPhase.  It invokes the
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
ExhPhase::ExhPhase (void) :
        Phase ()
{
    TargetPressure = 0;

}   // end ExhPhase()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing: 
//      This operation is invoked at the start of every exhalation phase.
//      It perfroms processing common to all exhalation phases. 
//
//      This operation first invokes MgrBdSetting::ProcessNewPhase() to 
//      phase in pending setting changes.   It then calls 
//      MgrBdSetting::GetCurrentValue() to retrieve the baseline pressure
//      setting.  This value is stored in the TargetPressure attribute since
//      this value is the PEEP or EPAP pressure targeted during the exhalation
//      phase.
//
//      Start() then calls ActivePressureSensor::GetLastReading() to get the 
//      last pressure reading.  This value is stored in the InitialPressure
//      attribute.
//      
//      Next the exponential term of the pressure profile during exhalation
//      is calculated by invoking Exponential::GetExpTerm().  And the initial
//      desired pressure is calculated using the following equation:
//
//      desiredPressure = 
//      
//          TargetPressure + (InitialPressure - TargetPressure) * expTerm
//
//      where:
//                      -3(t-Ti+Tc)/FALL_TIME
//          expTerm =  e
//
//          t-Ti = time elapsed since start of exhalation, 0 at start
//          Tc = cycle time of the Exhalation Pressure controller
//
//      Once the desired pressure is determined, the ExhPressController
//      operation SetDesired() is invoked to set the desired pressure and
//      ExhPressController::Enable() is invoked to enable the exhalation
//      pressure controller, which only runs during the exhalation phase.
//
//      The Start() operation then calls SystemClock::Retrieve() to snapshot
//      the system time at the start of exhalation.  It also calls
//      MgrBreathData::ExhalationStart() to update breath data at the start
//      of the exhalation phase.
//
//      Finally, this operation calls TriggerMgr::DisableTrigger() to disable
//      the HIP Alarm triggers and EnableTrigger() to enable the Manual 
//      Inhalation Operatator Request trigger.
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
void ExhPhase::Start (void)
{
//    VCVServoController->SetSepecificPhase(eNoTrigExhPhase);

    MgrBdSetting* mgrBdSettingPtr;  // pointer to MgrBdSetting

    // Get a handle to the BD Setting Manager
    mgrBdSettingPtr = MgrBdSetting::S_GetInstance();


    SHORT BiasFlow = mgrBdSettingPtr->GetCurrentValue(eBaseFlow);
    MgrController::CMVServoController->SetBiasFlow(BiasFlow/100);
    MgrController::VCVServoController->SetBiasFlow(BiasFlow/100);

    // Save the target pressure so that it does not have to be retrieved
    // every BD cycle.

    TargetPressure = MgrBdSetting::S_GetInstance()->GetCurrentValue(eBaselinePress);

//    CMVServoController->SetExhalationTarget(TargetPressure/10);
//    VCVServoController->SetExhalationTarget(TargetPressure/10);

    // During expiratory this bit should be clear.
    //    InpedanceLine->SetState(eOff);

    // Snapshot the current system time as the start of exhalation and reset
    // the elapsed time
//    S_ExhStartTime = SystemClock::Retrieve();
    S_TimeElapsed = 0;

    mgrBdSettingPtr->ProcessNewPhase();
    // Update Patient Data
    (MgrBreathData::S_GetInstance())->ExhalationStart();

    // Get the pointer to the TriggerMgr instance
   TriggerMgr::S_GetInstance()->EnableExhaleTrigger();

    // Enable the low baseline alarm trigger

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
//      disable the Manual Inhalation Op Request Trigger and the Patient
//      Inhalation Pressure Trigger.  It also interfaces with the Exhalation
//      Pressure Controller to close the exhalation valve and disable the
//      controller.
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
void ExhPhase::End (void)
{
    // Pointer to the Trigger Manager
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable the manual inhalation and the patient inhalation pressure
    // trigger.
    triggerMgrPtr->DisableTrigger(eManualInhOpReqTrigger);
    triggerMgrPtr->DisableTrigger(ePtInhPressTrigger);
    triggerMgrPtr->DisableTrigger(eExtremeHighPEEPTrigger);

    // Disable the Patient Inhalation Flow Trigger; the Patient Inhalation
    // Pressure Trigger was disabled in ExhPhase::End()
    triggerMgrPtr->DisableTrigger(ePtInhFlowTrigger);


    // Switch back to Pexh if the vent wasn't already using it.
    PressureSensor::ActivePressureSensor->SetPrimary(PressureSensor::InhPressSensor);
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
//      Pdes = TargetPressure + (Inital Pressure - TargetPressure) * expTerm
//
//                 -3(t-Ti+Tc)/FALL_TIME
//      expTerm = e
//
//      where:
//          t - Ti = time elapsed since the start of exhalation
//          Tc = control cycle of the exhalation pressure controller.
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
void ExhPhase::ProcessBdCycle (void)
{
    // Calculate the time elapsed since the start of exhalation.

    MgrBdSetting::S_GetInstance()->PhaseInPIPSetting();
    MgrBdSetting::S_GetInstance()->PhaseInAllSettings();


    S_TimeElapsed = ExhPhase::S_CalcTimeSincePhaseStart();

    TargetPressure = MgrBdSetting::S_GetInstance()->GetCurrentValue(eBaselinePress);
//    CMVServoController->SetExhalationTarget(TargetPressure/10);
//    VCVServoController->SetExhalationTarget(TargetPressure/10);

    // If it is time to auto-zero the exhalation pressure transducer,
    // kick start the auto-zero, update the state and switch the active
    // pressure transducer to the inhalation pressure transducer.  The
    // ventilator usually uses Pexh for monitoring and control.  During the
    // phase where Pexh is being auto-zeroed, Pinh is used for monitoring
    // and control.  It is always switched back at the end of exhalation.
//    if (eAzPending == PInhAzTestPtr->GetAzTestState())
//    {
//        if ( ActivePressureSensor->GetLastReading() <= (TargetPressure + 200)  )
//        {
//            RT_TASK* receiveTask1 = NULL;
//            receiveTask1 = (AzTask::S_GetInstance())->S_SetEventFlag(INH_AZ_EVENT_FLAG);
//            ActivePressureSensor->SetPrimary(ExhPressSensor);
//            if((receiveTask1 !=0))
//                PInhAzTestPtr->SetAzTestState(eAzActive);
//        }
//    }
//    else
//    {
//        if (eAzCompleted == PInhAzTestPtr->GetAzTestState())
//        {
//            ActivePressureSensor->SetPrimary(InhPressSensor);
//
//            // Otherwise if it is time to auto-zero Pinh, start the auto-zero and
//            // update the state.
//            if (eAzPending == PExhAzTestPtr->GetAzTestState())
//            {
//
//
//                RT_TASK* receiveTask2 = NULL;
//                receiveTask2 = (AzTask::S_GetInstance())->S_SetEventFlag(EXH_AZ_EVENT_FLAG);
//                if((receiveTask2 !=0))
//                    PExhAzTestPtr->SetAzTestState(eAzActive);
//            }
//        }
//    }

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
E_Bool ExhPhase::S_IsInUnrestrictedPhase (UNSIGNED minExhTime)
{
    UNSIGNED tmp = minExhTime;
    E_Bool rtnValue = eFalse;

    if(tmp >= 1000)
        tmp = 1000;
    else if(tmp < 200)
        tmp = 200;

    // If the breath phase is exhalation and the time elapsed since the start
    // of exhalation is greater than or equal to the minimum exhalation time,
    // return TRUE because the ventilator is in the unrestricted phase.
    if (((PhaseMgr::S_GetInstance())->GetCurrentPhaseId() == eExhPhase) &&
            (S_TimeElapsed >= tmp))
    {
        rtnValue = eTrue;
    }

    return(rtnValue);

}   // end S_IsInUnrestrictedPhase()
