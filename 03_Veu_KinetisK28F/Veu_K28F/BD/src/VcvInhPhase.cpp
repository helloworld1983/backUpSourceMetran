/******************************************************************************/
//$COMMON.CPP$
//   File Name:  VcvInhPhase.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Volume Control Ventilation
//      mandatory inhalation breath phase.
//
//   Interfaces:
//      Operations in this class interface with the BD Setting Manager 
//      (MgrBdSetting) to retrieve the VCV related settings such as
//      Tidal Volume, Peak Flow and Flow Pattern.  Operations in this class
//      also interface with all of the FlowPattern derived classes.
//      The Start() and End() operations interface with the TriggerMgr object to 
//      enable and disable triggers.  The Start() operation also interfaces with 
//      the MgrBreathData object to notify it of the inhalation start.
//
//      The ProcessBdCycle() operation interfaces with the AirFlowController
//      and the O2FlowController to update the desired flow commands during
//      a VCV inhalation.
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
#include <DeviceInterface.h>
#include "VcvInhPhase.h"
#include "MgrBdSetting.h"
#include "MgrBreathData.h"
#include "Mode.h"
#include "TriggerMgr.h"
#include "RampHighPfFlowPattern.h"
#include "RampLowPfFlowPattern.h"
#include "SquareFlowPattern.h"
#include "AlarmTask.h"
#include "ProxyFlowSensor.h"
#include "MgrController.h"

// Define statics
VcvInhPhase* VcvInhPhase::S_Instance = NULL;

static const LONG STEP_EXP_THRESHOLD = 150 * SCALE;  // 9.00 lpm = 150 ml/sec
static const LONG LPM_24  = 400 * SCALE;  // 24  lpm =  400 ml/sec
static const LONG ML_7   = 7 * SCALE;     // 7 ml
static const LONG ADJUSTED_VT_SETTING   = 3 * SCALE;     // 4 ml
static const LONG ML_150 = 150 * SCALE;   // 150 ml
const SHORT PURGE_FLOW_IN_MLPM = 200;
const int Kp = 1;
const int TIDAL_VOLUME_ERROR = 50;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants to
//      access the VcvInhPhase object, it calls VcvInhPhase::S_GetInstance(),
//      which returns a pointer to the VcvInhPhase object.
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
//      VcvInhPhase* - pointer to the object instantiated from this class
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
VcvInhPhase* VcvInhPhase::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new VcvInhPhase ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing: 
//      This operation is invoked at the start of each VCV inhalation.  This
//      operation retrieves the current settings for Tidal Volume, Peak Flow
//      and Flow Pattern.  It also retrieves the setting values for compliance
//      compensation ON/OFF and compliance factor.
//
//      Based on the flow pattern and peak flow settings, this operation then
//      sets the FlowPatternPtr for the current inhalation and invokes the 
//      CalcTargetInhTime() operation for the active flow pattern.
//      This inhalation time is used to enable the VCV inhalation trigger.
//
//      If compliance compensation is on, the private operation 
//      CalcComplCompVol() is called to compute the compliance compensated
//      tidal volume and the CalcCompensatedPeakFlow() operation of the 
//      active flow pattern is invoked to calculate the compliance compensated
//      peak flow.  If compliance compensation is not on, the 
//      SetCompensatedPeakFlow() operation of the active flow pattern is
//      invoked to set the compensated peak flow to the set peak flow.  The
//      flow profile is dictated by the compensated peak flow, the inhalation
//      time and the active flow pattern.
//
//      Finally, the VCV inhalation time trigger is enabled and the 
//      MgrBreathData::InhalationStart() operation is invoked to initialize
//      the breath data for a new inhalation.
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
//      This operation assumes that the ComplianceCompOn attribute was already
//      set in the constructor and does not change during operation.
//
//    Miscellaneous:
//      If the flow pattern is an unexpected value, error handling is 
//      performed.
//
//    Requirements:  
//
/******************************************************************************/
void VcvInhPhase::Start (void)
{
    E_FlowPattern flowPattern;      // operator set flow pattern
    LONG setTidalVol;               // operator set tidal volume
    LONG compTidalVol;              // compliance compensated tidal volume
    LONG setPeakFlow;               // operator set peak flow
    UNSIGNED inhTime;               // calculated inhalation time in TICKS
    LONG desiredAirFlow;
    LONG desiredO2Flow;
    static bool firstChange = false;

    // Get a handle to the BD Setting Manager and the TriggerMgr
    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Invoke base class method to do processing common to all inhalation
    // phases
    mgrBdSettingPtr->PhaseInAllSettings();

    IsVCVInhPhaseStarted = eTrue;
    // Retrieve current setting values for TV, PF and Flow Pattern.  These
    // calls must be made AFTER the base class Start() is called since any
    // pending setting values are phased in the base class operation.
    ProxyStatus Status = ProxyFlowSensor::S_GetInstance()->getLastStatus();

    if((Status == eCalibrated)||(Status == eResetAndRecovered))
    {
        setTidalVol = DoP();
        firstChange = true;
    }
    else
    {
        if(firstChange == true)
        {
            firstChange = false;
            InitErrorCompensate();
            MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->clearInhPressData();
        }
        setTidalVol = mgrBdSettingPtr->GetCurrentValue(eTidalVolume); // ml*SCALE
    }

    setPeakFlow = mgrBdSettingPtr->GetCurrentValue(eInspFlow)*1000/60; // convert from LPM to mL/sec


    flowPattern = (E_FlowPattern) mgrBdSettingPtr->GetCurrentValue(eWaveForm);

    // Set the FlowPatternPtr based upon the operator set flow pattern
    switch (flowPattern)
    {
    case eSquare:
        FlowPatternPtr = SquareFlowPattern::S_GetInstance();
        break;

    case eRamp:

        // There are 2 versions of the Ramp Flow Pattern.  If the
        // operator set peak flow is greater than or equal to
        // PF_THRESHOLD_IN_SCALED_MLPS, the flow profile starts at the
        // operator set peak flow and decelerates to
        // PF_THRESHOLD_IN_SCALED_MLPS.  If the operator set peak flow is
        // less than this threshold, the flow profile starts at the
        // operator set peak flow and decelerates to 1/2 of the operator
        // set peak flow.
        if (setPeakFlow >= PF_THRESHOLD_IN_SCALED_MLPS*SCALE)
        {
            FlowPatternPtr = RampHighPfFlowPattern::S_GetInstance();
        }
        else
        {
            FlowPatternPtr = RampLowPfFlowPattern::S_GetInstance();
        }
        break;

    default:
        // Illegal flow pattern
        ASSERTION((flowPattern == eSquare) || (flowPattern == eRamp));
        break;
    }

    // Calculate the target inhalation time based upon flow pattern, set TV
    // and set PF.  Note that the inhalation time is calculated based upon
    // operator settings, not compliance compensated values.
    if ((flowPattern == eSquare) &&
            (setTidalVol <= ML_150)  &&
            (setPeakFlow <= LPM_24))
        {
            setTidalVol += ADJUSTED_VT_SETTING;
        }

    setPeakFlow = setPeakFlow + PURGE_FLOW_IN_MLPM * SCALE / SEC_PER_MIN;
    inhTime = FlowPatternPtr->CalcTargetInhTime(setTidalVol, setPeakFlow);
    InhTime = inhTime;
    // Reset error buffer if high ins pressure occur
    if(eActive == AlarmTask::S_GetInstance()->GetIsActive(eHip))
    {
        InitErrorCompensate();
    }
    // compliance compensation is on, invoke the private method
    // CalcComplCompVolume to calculate the volume
    compTidalVol = CalcComplCompVol(setTidalVol);
    // Then, calculate the compliance compensated peak flow based upon
    // flow pattern, compliance compensated volume and inhalation time.
    FlowPatternPtr->CalcCompensatedPeakFlow(compTidalVol);

    // Enable the VCV Inhalation Time for the inhalation time calculated by
    // the active flow pattern and based upon the TV, PF and flow pattern
    triggerMgrPtr->EnableTrigger(eVcvInhTimeTrigger, inhTime);

    // Enable the Hip2 trigger; Hip1 was enabled in the InhPhase base class.
    triggerMgrPtr->EnableTrigger(eHip2AlarmTrigger);

    // Update patient data;  need to know how the breath was triggered
    // to determine whether or not it is a mandatory or assisted breath
    if (Mode::S_IsInhPtTriggered() == eTrue)
    {
        (MgrBreathData::S_GetInstance())->InhalationStart(eAssisted);
    }
    else
    {
        (MgrBreathData::S_GetInstance())->InhalationStart(eMandatory);
    }

    // Get the initial desired air and o2 flows for this breath
    InhPhase::Start();
    FlowPatternPtr->GetDesiredFlows(InhPhase::S_CalcTimeSincePhaseStart(),
                                    &desiredAirFlow, &desiredO2Flow);

    MgrController::VCVServoController->SetSepecificPhase(eVcvInhPhase);
    MgrController::VCVServoController->SetDesiredFlow(desiredAirFlow*60/1000,desiredO2Flow*60/1000);

}   // end Start()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing: 
//      This operation is invoked whenever a VCV inhalation ends.  It disables 
//      the VCV Inhalation Time Trigger.  And, it interfaces with MgrBreathData
//      to retrieve the end inhalation pressure reading which is stored in
//      the EndInhPress array and used to calculate compliance compensation.
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
void VcvInhPhase::End (void)
{
    // Disable the VCV inhalation time trigger
    (TriggerMgr::S_GetInstance())->DisableTrigger(eVcvInhTimeTrigger);

}   // end End()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBdCycle()
//
//    Processing: 
//      This operation is invoked every BD Cycle Time during a VCV inhalation.
//      This operation calls the base class operation
//      InhPhase::S_CalcTimeSincePhaseStart() to determine the time elapsed
//      since the start of the inhalation.  This value is then passed to 
//      the GetDesiredFlows() operation of the currently active flow pattern,
//      which updates the desired air and oxygen flows.
//
//      The SetDesired() operations of the Air and O2 Flow Controllers are
//      then invoked to notify the controllers of the desired flow commands.
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
void VcvInhPhase::ProcessBdCycle (void)
{
    LONG desiredAirFlow;
    LONG desiredO2Flow;

    // Get the desired air and o2 flows for this cycle
    FlowPatternPtr->GetDesiredFlows(InhPhase::S_CalcTimeSincePhaseStart(),
                                    &desiredAirFlow, &desiredO2Flow);


    MgrController::VCVServoController->SetDesiredFlow(desiredAirFlow*60/1000,desiredO2Flow*60/1000);

}   // end ProcessBdCycle()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ResetEndInhPress()
//
//    Processing: 
//      This operation is invoked each time the ventilator enters the Volume
//      Ventilation Type from any other ventilation type.  This operation
//      zeroes out the pressure readings in the EndInhPress array and sets the
//      EndInhPressIx to 0 and NumInhPressSamples to 0.  Resetting the buffer 
//      effectively means that the first in the Volume Ventilation Type is
//      not compliance compensated.
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
void VcvInhPhase::ResetEndInhPress (void)
{
    // An average of the End Inhalation Pressure samples from previous breaths
    // is used to determine compliance compensation for breath delivery.  
    // Initialize the pressure samples to 0 and the index to 0.  By setting
    // the pressure to 0, the first VCV breath will not be compensated.
    for (SHORT ii = 0; ii < NUM_INH_PRESS_SAMPLES; ii++)
    {
        EndInhPress[ii] = 0;
    }

    // Reset the index and the number of samples.
    EndInhPressIx = 0;
    NumEndInhPressSamples = 0;

}   // end ResetEndInhPress()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: VcvInhPhase()
//
//    Processing: 
//      This operation is the constructor for the VCV Inhalation Phase.  It 
//      calls the base class constructor and creates all of the flow patterns.
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
VcvInhPhase::VcvInhPhase (void) :
        InhPhase ()
{
    // Instantiate all of the flow patterns.
    RampHighPfFlowPattern::S_GetInstance ();
    RampLowPfFlowPattern::S_GetInstance ();
    SquareFlowPattern::S_GetInstance ();

}   // end VcvInhPhase()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CalcComplCompVol()
//
//    Processing: 
//      This operation calculates 
//
//    Input Parameters:
//      setTidalVol -- operator set Tidal Volume in ml * SCALE
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG - compliance compensated tidal volume in ml * SCALE
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
/******************************************************************************/
LONG VcvInhPhase::CalcComplCompVol (LONG setTidalVol)
{
    LONG avg = 0;           // average end inhalation pressure
    LONG complCompVol;      // compliance compensated volume
    LONG complCompFactor;   // compliance compensated volume

    // use end Ip buffer to average end IP for volume breaths...
    for (SHORT jj = 0; jj < NUM_INH_PRESS_SAMPLES; jj++)
    {
        avg += EndInhPress[jj];
    }

    // Average the value
    avg = avg / NUM_INH_PRESS_SAMPLES;

    // Don't compensate if the value is less than 0.    
    if(avg < 0)
    {
        avg = 0;
    }

    complCompFactor = (MgrBdSetting::S_GetInstance ())->GetCurrentValue (
            eComplFactor);

    // Add the compliance volume to the set tidal volume to determine the
    // compliance compensated tidal volume.  The divide by the scale factor
    // is needed since both the compliance factor and the avg are scaled.
    complCompVol = setTidalVol + ((complCompFactor * avg) / SCALE);

    return (complCompVol);

}   // end CalcComplCompVol()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: InitErrorCompensate()
//
//    Processing:
//      This operation is used to initialization for error vt
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
void VcvInhPhase::InitErrorCompensate (void)
{
    PrevcomplCompVol = 0;
    PrevsetTidalVol = 0;
    errorvt = 0;
    prev_errorvt = 0;
    VolumeBrCnt = 0;
    VolumeErrIx = 0;
    for(int i = 0; i < NUM_INH_PRESS_SAMPLES;i++)
    {
        VolumeErrBuf[i] = 0;
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetComplVol()
//
//    Processing:
//      Get compute volume
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
LONG VcvInhPhase::GetComplVol (void)
{
    return ComplVol;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetFlowPatternPtr()
//
//    Processing:
//      return active FlowPattern object
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      return active FlowPattern object
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
FlowPattern* VcvInhPhase::GetFlowPatternPtr (void)
{
    return FlowPatternPtr;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ResetVolumeAdjustment()
//
//    Processing:
//      Reset volume adjustmet
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
void VcvInhPhase::ResetVolumeAdjustment (void)
{
    VolumeAdjustment = 0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetPhaseStarted (E_Bool value)
//
//    Processing:
//      Set phase start
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
void VcvInhPhase::SetPhaseStarted (E_Bool value)
{
    IsVCVInhPhaseStarted = value;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetPhaseStarted ()
//
//    Processing:
//      Get phase start
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
E_Bool VcvInhPhase::GetPhaseStarted (void)
{
    //Get phase start
    return IsVCVInhPhaseStarted;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DoP ()
//
//    Processing:
//      Get phase start
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
LONG VcvInhPhase::DoP (void)
{
    LONG tidalVolSetting = MgrBdSetting::S_GetInstance()->GetCurrentValue(eTidalVolume);
    LONG currentTidalVolume = MgrBreathData::S_GetInstance()->GetTidalVolumeEndInh();

    if((tidalVolSetting != 0) && (currentTidalVolume * 100/tidalVolSetting < 25))
    {
        return tidalVolSetting;
    }
    LONG error =  tidalVolSetting - currentTidalVolume;
    if(error > 50)
    {
        VolumeAdjustment += (Kp * TIDAL_VOLUME_ERROR);
    }
    else if(error < -50)
    {
        VolumeAdjustment -= (Kp * TIDAL_VOLUME_ERROR);
    }
    else
    {
        //Do nothing
    }
    return tidalVolSetting + VolumeAdjustment;
}
