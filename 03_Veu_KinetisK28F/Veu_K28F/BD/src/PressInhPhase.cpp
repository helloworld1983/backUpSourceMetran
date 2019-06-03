/******************************************************************************/
//$COMMON.CPP$
//   File Name:  PressInhPhase.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Pressure Inhalation Phase
//      base class.  All mandatory and supported pressure based inhalation
//      phases are derived from this class.  Pressure based mandatory and
//      supported inhalations include:  PCV, PSV, mandatory and spontaneous
//      breaths in NIPPV.
//
//   Interfaces:
//      The Start() and End() operations in this class interface with the
//      Inhalation Pressure Controller to enable and disable the controller
//      task via the operations Enable() and Disable().  The Start() operation
//      also interfaces with MgrBdSetting to retrieve the current settings
//      for inhalation pressure, baseline pressure and rise time by calling
//      GetCurrentValue() with the proper setting ID.
//
//      ExhPhase::Start() and ExhPhase::ProcessBdCycle() invoke 
//      Exponential::GetExpTerm() to get the exponential term of the desired
//      pressure equation.
//
//   Restrictions:
//      This class is currently not instantiated; it is used only as a base
//      class for the SupportedInhPhase and the MandPressInhPhase.  There is
//      nothing to prevent it from being instantiated.  
/******************************************************************************/
#include "PressInhPhase.h"
#include "MgrBdSetting.h"
#include "MgrBreathData.h"
#include "Exponential.h"
#include "PressureSensor.h"
#include "ModeMgr.h"


// Rise Time cutoff.  For Rise Times less than 150ms, the reference does not
// follow an exponential rise to the target; instead it just goes there directly
// to avoid delays.  Values of 150ms and greater, use an exponential rise to
// the target.
static const UNSIGNED RT_CUTOFF = 150 / MS_PER_TICK;     // 150 ms in ticks

static const UNSIGNED CONTROL_CUTOFF = 17 * SCALE;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PressInhPhase()
//
//    Processing: 
//      This operation is the constructor for the PressInhPhase.  It initializes
//      the InhPressId and the BaselinePressId, which are used to determine
//      the target inhalation pressure.  This operation also invokes the
//      InhPhase base class constructor.
//
//    Input Parameters:
//      inhPressId -- setting ID of the inhalation pressure; this is
//          the inhalation Pressure in PCV, the support pressure in PSV and
//          the IPAP value in NIPPV
//
//      baselinePressId -- setting ID of the baseline pressure; this is the
//          PEEP value for PCV or PSV breaths and it is a dummy 0 value for
//          NIPPV breaths.  Target pressure in NIPPV is not offset by EPAP; 
//          target pressure in PCV and PSV is offset by PEEP.
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
PressInhPhase::PressInhPhase (E_BdSettingId inhPressId,
                              E_BdSettingId baselinePressId) :
        InhPhase (), InhPressId (inhPressId), BaselinePressId (baselinePressId)
{
    BaseLine = -100;
    TargetPressure = 2;
    RiseTime = 0;

}   // end PressInhPhase()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing: 
//      This operation is invoked at the start of every mandatory or supported
//      pressure based inhalation phase.  It performs processing common to 
//      the PSV, PCV and NIPPV inhalation phases.
//
//      This operation invokes the base class InhPhase::Start() operation to
//      perform processing common to all inhalation phases.  It then calls
//      MgrBdSetting::GetCurrentValue() to retrieve the baseline pressure and
//      the inhalation pressure based upon the BaselinePressureId and 
//      InhPressId attributes.  The sum of these values is stored in the 
//      TargetPressure attribute.  This operation also retrieves the Rise Time 
//      setting value, converts it into system clock tick units and stores it
//      in the RiseTime attribute.  
//
//      Start() then calls ActivePressureSensor::GetLastReading() to get the 
//      last pressure reading.  This value is stored in the InitialPressure
//      attribute.
//      
//      Next the exponential term of the pressure profile during inhalation
//      is calculated by invoking Exponential::GetExpTerm().  And the initial
//      desired pressure is calculated using the following equation:
//
//      desiredPressure = (TargetPressure - InitalPressure)*(1 - expTerm) + 
//                          InitialPressure
//
//                 -3(t+Tc)/RiseTime
//      expTerm = e
//
//      where:
//          t = time elapsed since the start of inhalation
//          Tc = control cycle of the inhalation pressure controller.
//
//      Once the desired pressure is determined, the InhPressController
//      operation SetDesired() is invoked to set the desired pressure and
//      InhPressController::Enable() is invoked to enable the inhalation
//      pressure controller, which only runs during pressure based inhalation
//      phases.
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
void PressInhPhase::Start (void)
{
    // Set up a pointer to the BD Setting Manager
    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();

    mgrBdSettingPtr->PhaseInAllSettings();

    // Retrieve the baseline pressure setting for the exhalation phase that
    // has just ended (before phasing in the settings).  This is the starting
    // point for the breath about to start.  This value is used to determine
    // the pressure command given to the pressure controller during inhalation
    //    InitialPressure = mgrBdSettingPtr->GetCurrentValue(eBaselinePress);

    // Invoke base class method to perform processing common to all inhalation
    // phases.  This call must be made before accessing setting values since
    // it phases in the settings at the start of the phase.
    InhPhase::Start();

    // Get the operator set pressure control or support level, the offset
    // pressure and the baseline pressure.  Offset pressure is 0 for NPPV and
    // PEEP otherwise.  Baseline pressure is EPAP for NPPV and PEEP otherwise.
    // The calls to GetCurrentValue() must be made AFTER the base class Start()
    // is called to phase in any new settings.

    if((ModeMgr::S_GetInstance()->GetCurrentModeId() == ePcvMixedMode)
            ||((mgrBdSettingPtr->GetPendingValue(eMode) == ePcvMixedMode)
               &&(ModeMgr::S_GetInstance()->GetCurrentModeId() == eApneaBackupMode)))
    {
        if(eSwitchTypeOn == mgrBdSettingPtr->GetCurrentValue(eSyncPlus))
        {
            TargetPressure = mgrBdSettingPtr->GetCurrentValue(eSupportPress)
                    + mgrBdSettingPtr->GetCurrentValue(OffsetPressId);
        }
        else
            TargetPressure = mgrBdSettingPtr->GetCurrentValue(InhPressId);
    }
    else
        TargetPressure = mgrBdSettingPtr->GetCurrentValue(InhPressId);
    BaseLine = mgrBdSettingPtr->GetCurrentValue(OffsetPressId);

    // Store the Rise Time in seconds, scaled by 100.
    RiseTime = MgrBdSetting::S_GetInstance()->GetCurrentValue(eRiseTime);

}   // Start()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing: 
//      This operation is invoked at the end of each pressure based inhalation.
//      This operation disables the Inhalation Pressure Controller.
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
void PressInhPhase::End (void)
{

}   // end End()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBdCycle()
//
//    Processing: 
//      This operation is called every BD task cycle during a mandatory or
//      supported pressure based inhalation exhalation.  This operation 
//      performs processing common to all pressure based inhalation phases.  
//      This operation calculates the desired pressure and sends the command 
//      to the Inhalation Pressure controller.
//
//      Pdes = (TargetPressure - InitalPressure)*(1 - expTerm) + InitialPressure
//
//                 -3(t+Tc)/RiseTime
//      expTerm = e
//
//      where:
//          t = time elapsed since the start of inhalation
//          Tc = control cycle of the inhalation pressure controller.
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
//      The inhalation pressure controller must already be enabled before
//      this operation is called.
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void PressInhPhase::ProcessBdCycle (void)
{
    UNSIGNED timeElapsed;   // time elapsed since start of inhalation in ticks
    LONG expTerm;           // exponential term
    LONG desiredPress;      // desired pressure sent to the controllers
    E_BreathPhaseIndicator breathType;
    E_PatientType patientType;

    // Set up a pointer to the BD Setting Manager
    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance ();

    // Set up a pointer to the Breath Data Manager
    MgrBreathData* mgrBreathDataPtr = MgrBreathData::S_GetInstance ();

    // Get patient type and breath type
//    patientType = (E_PatientType) mgrBdSettingPtr->GetCurrentValue (ePatientType);
    breathType = mgrBreathDataPtr->GetBdBreathDataPtr ()->GetInhType ();

    if((breathType == eAssisted) || (breathType == eMandatory))
    {
        // PCV:  if rise time < 0.15 AND control level is < 17cmH20
        // go straight to the desired pressure level  
        // (don't use the exponetial curve)
        if((RiseTime < RT_CUTOFF)
                && (TargetPressure - InitialPressure < CONTROL_CUTOFF)
                && (patientType == eAdult))
        {
            desiredPress = TargetPressure;
        }
        else
        {
            // Calculate the time elapsed since the start of inhalation.
            timeElapsed = InhPhase::S_CalcTimeSincePhaseStart ();

            // Offset the time elapsed by the amount of 1 inhalation pressure control 
            // cycle time.
//            timeElapsed = timeElapsed + InhController->GetSamplePeriod();

            // Get the exponential term based on the rise time and elapsed time
            expTerm = Exponential::GetExpTerm (RiseTime, timeElapsed);

            // Calculate the desired pressure            
            desiredPress = (((TargetPressure - InitialPressure)
                    * (EXP_DATA_SF - expTerm)) >> EXP_DATA_SHIFT)
                    + InitialPressure;

        }
    }
    else
    {
        // PSV: if rise time < .015 and PatientType == Adult
        // go straight to the desired pressure level  
        // (don't use the exponetial curve)
        if((RiseTime < RT_CUTOFF) && (patientType == eAdult))
        {
            desiredPress = TargetPressure;
        }
        else
        {
            // Calculate the time elapsed since the start of inhalation.
            timeElapsed = InhPhase::S_CalcTimeSincePhaseStart ();

            // Offset the time elapsed by the amount of 1 inhalation pressure control 
            // cycle time.
//            timeElapsed = timeElapsed + InhController->GetSamplePeriod();

            // Get the exponential term based on the rise time and elapsed time
            expTerm = Exponential::GetExpTerm (RiseTime, timeElapsed);

            // Calculate the desired pressure            
            desiredPress = (((TargetPressure - InitialPressure)
                    * (EXP_DATA_SF - expTerm)) >> EXP_DATA_SHIFT)
                    + InitialPressure;

        }

    }

}   // end ProcessBdCycle()

