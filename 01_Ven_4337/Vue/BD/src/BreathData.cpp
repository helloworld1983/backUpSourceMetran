/******************************************************************************/
//$COMMON.CPP$
//   File Name:  BreathData.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the BreathData class which contains pressure, 
//      volume and time data for a complete breath.  Data is stored in
//      objects instantiated from this class by the BD Task; it is then
//      used by the Patient Data Task to calcluated monitored data that
//      is displayed on the GUI.  The Patient Data Task also uses this
//      data to determine the status of alarm conditions such as the
//      tidal and minute volume alarms.  The BD Task uses the data for 
//      end inhalation pressure average used to compute compliance compensation
//      for volume delivery.
//
//   Interfaces:
//      Operations in this class are invoked from both the BD and the Patient
//      Data Tasks.  Operations in this class interface with the InhPhase
//      and ExhPhase objects to determine the time elapsed since the start
//      of the phases.  
//
//   Restrictions:
//      None
/******************************************************************************/
#include <ExhPhase.h>
#include <InhPhase.h>
#include "BreathData.h" 
#include "CommonConstants.h"
#include "APRV_PLowPhase.h"
#include "APRV_SupportedInhPhase_InPLow.h"
#include "APRV_ExhPhase_InPLow.h"
#include "Tasks.h"
#include "MgrBdSetting.h"
#include "APRV_PHighPhase.h"
#include "APRV_SupportedInhPhase_InPHigh.h"
#include "APRV_ExhPhase_InPHigh.h"

// MIN_NET_FLOW is used to determine if a flow sample is included in the 
// flow sum, which is integrated to calculate the delivered or exhaled volume.
// If the flow sum is less than this value, it is not integrated.
// 
// This value is ml/sec, SCALED.    
static const LONG MIN_NET_FLOW = 17 * SCALE;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: BreathData()
//
//    Processing: 
//      This operation is the constructor for BreathData objects.  It 
//      initializes all of the private attributes.
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
BreathData::BreathData (void)
{
    ExhFix = 0;
    ExhFlowCnt = 0;

    timer200ms=0;
    Compliance = 0;
    PeakInhPress = MIN_PRESSURE_READING;
    NetFlowSumInInh = 0;
    NetFlowSumInExh = 0;
    NetFlowSum = 0;
    Compensated_NetFlowSumInInh = 0;
    Compensated_NetFlowSumInExh = 0;

    VteInPhase = 0;
    ExhVolume = 0;
    InhVolume = 0;

    Compensated_ExhVolume = 0;
    Compensated_InhVolume = 0;

    InhPix = 0;
    ExhPix = 0;
    InhFix = 0;

    mPLowIndex = 0;
    mPHighIndex = 0;

    NumInhSamples = 0;
    NumExhSamples = 0;
    PressureSum = 0;
    BaselinePress = 0;
    PdDone = eTrue;
    TimeInInh = 0;
    TimeInExh = 0;
    InhType = eNoPhase;

    // Zero out the pressure averages.  
    for (SHORT ii = 0; ii < NUM_END_P_SAMPLES; ii++)
    {
        InhPress[ii] = 0;
        ExhPress[ii] = 0;
        PHigh[ii] = 0;
        PLow[ii] = 0;
        InhFlow[ii] = 0;
        ExhFlow[ii] = 0;
    }
    for (SHORT ii = 0; ii < NUM_EXH_FLOW_SAMPLES; ii++)
    {
        ExhFlowData[ii] = 0;
    }
    ExhFDataix = 0;

    InhFlowSum = 0;
    MoniInhVolume = 0;
    ExhFlowSum = 0;
    MoniExhVolume = 0;

}   // end BreathData()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: InhalationStart()
//
//    Processing: 
//      This operation is invoked from MgrBreathData::InhalationStart() in the
//      BD Task's thread of control.  It is invoked at the start of every 
//      inhalation.  All attributes are initialized to 0 or a value that
//      insures they will be updated or contain the correct value on the
//      next BD Cycle.
//
//    Input Parameters:
//      InhType - inhalation type
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      PatientData is finished using the breath data.
//
//    Miscellaneous:
//      If the PdDone flag is FALSE, the Patient Data Task is still using the
//      breath data.  But, the BD Task wants to use the object.  Since there
//      are 2 BreathData objects, this situation should never occur since 
//      Patient Data should at most 1 breath behind BD.  If Patient Data is 
//      not done, error handling is performed.
//
//    Requirements:  
//
/******************************************************************************/
void BreathData::InhalationStart (E_BreathPhaseIndicator inhType)
{
    // BD is ready to use this object again.  Make sure that the Patient
    // Data Task is done with it.  If not, it's an error.
    //    ASSERTION(PdDone == eTrue);

    // Update the inhalation type with the new inh type.  Inhalation type
    // is used by patient data to determine if the breath is mandatory
    // or spontaneous so that the exhaled volumes can be properly updated.
    InhType = inhType;
    VteInPhase = 0;

    // Initialize all of the attributes
    PeakInhPress = MIN_PRESSURE_READING;
    NetFlowSumInInh = 0;
    NetFlowSumInExh = 0;
    NetFlowSum = 0;

    InhFlowSum = 0;
    ExhFlowSum = 0;
    MoniInhVolume = 0;
    MoniExhVolume = 0;

    Compensated_NetFlowSumInInh = 0;
    Compensated_NetFlowSumInExh = 0;

    ExhVolume = 0;
    InhVolume = 0;

    Compensated_ExhVolume = 0;
    Compensated_InhVolume = 0;

    InhPix = 0;
    ExhPix = 0;
    InhFix = 0;
    NumInhSamples = 0;
    NumExhSamples = 0;
    PressureSum = 0;

    // Zero out the pressure arrays.        
    for (SHORT ii = 0; ii < NUM_END_P_SAMPLES; ii++)
    {
        InhPress[ii] = 0;
        ExhPress[ii] = 0;
        InhFlow[ii] = 0;
        ExhFlow[ii] = 0;
        PHigh[ii] = 0;

    }
    for (SHORT ii = 0; ii < NUM_EXH_FLOW_SAMPLES; ii++)
    {
        ExhFlowData[ii] = 0;
    }
    ExhFDataix = 0;
    timer200ms = 0;

    PdDone = eFalse;
}   // end InhalationStart()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ExhalationStart()
//
//    Processing: 
//      This operation invokes InhPhase::S_CalcTimeSincePhaseStart() to update
//      the InhTime for the data contained in this BreathData object.
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
void BreathData::ExhalationStart (void)
{
    TimeInInh = InhPhase::S_CalcTimeSincePhaseStart();

    // Retreive the baseline pressure that will be used during the current
    // exhalation.  This value is used to adjust end inspiratory pressure.
    BaselinePress = (MgrBdSetting::S_GetInstance())->GetCurrentValue(eBaselinePress);

#ifdef BILEVEL_CPAP
    if(eBiLevelMode == ModeMgr::S_GetInstance()->GetCurrentModeId())
    {
        BaselinePress = BiLevelMode::S_GetInstance()->GetBaseline();
    }
#endif

}   // end ExhalationStart()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdateTimeInExhalation()
//
//    Processing:   
//      This operation calls ExhPhase::S_CalcTimeSincePhaseStart() to calculate
//      the exhalation time.  This operation is called from 
//      MgrBreathData::InhalationStart() before MgrBreathData switches its
//      BreathData pointers.  The update of the exhalation time cannot be
//      done in the InhalationStart() operation of this class because by
//      the time the InhalationStart() method is called, the BreathData object
//      is being used for the current breath, but the exhalation time applies
//      to the previous breath.
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
void BreathData::UpdateTimeInExhalation (void)
{
#ifdef BILEVEL_CPAP
    TimeInExh = ExhPhase::S_CalcTotalTimeSincePhaseStart();
#else
    TimeInExh = ExhPhase::S_CalcTimeSincePhaseStart();
#endif

}   // end UpdateTimeInExhalation()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBdCycleInInh()
//
//    Processing: 
//      This operation is invoked every BD Cycle Time during inhalation or
//      plateau.  It is called from MgrBreathData::ProcessBdCycle().  This
//      operation updates the PeakInhPress, NetFlowSumInInh,
//      InhPress and PressureSum averages based upon the current readings
//      for exhalation pressure and net flow.
//
//    Input Parameters:
//      exhPressure - pressure measured in the expiratory limb (cmH20 * SCALE)
//      netFlow -- deliverd flow - exhaled flow (ml/sec * SCALE)
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
LONG BreathData::ProcessBdCycleInInh (SHORT inhPressure, LONG netFlow)
{
    // The NetFlowSumInExh and ExhVolume calculations depend on the
    // BD_CYCLE_TIME_IN_MS being no less than 5ms.  They also assume that the
    // cycle time is a multiple of 5.  If this is no longer the case, the
    // NetFlowSumInExh and ExhVolume computations must be re-visited.

    // If the current pressure reading exceeds the current peak, update the
    // peak
    if (inhPressure > PeakInhPress)
    {
        PeakInhPress = inhPressure;
    }

//    InhFlowSum = AddSat(InhFlowSum, netFlow);
    // calculate inhalation volume that is truntcast in mL, scaled.
    MoniInhVolume = (InhFlowSum*2.0)/60.0;

    if(netFlow < 0)
    {
        netFlow = 0;
    }

//    NetFlowSumInInh = AddSat(NetFlowSumInInh, netFlow);

//    LONG compensate = (LONG)Ptdata::S_GetInstance()->GetExhLeak()*SCALE;
//    netFlow = netFlow + abs(compensate);
//    Compensated_NetFlowSumInInh = AddSat(Compensated_NetFlowSumInInh, netFlow);

    // Update the pressure sum and the number of samples in inhalation.
    PressureSum += (LONG) inhPressure;
    NumInhSamples++;

    // Update the InhPress buffer.  This buffer maintains the last 
    // NUM_END_P_SAMPLES which are averaged together to determine the end
    // inhalation pressre.
    InhPress[InhPix] = inhPressure;
    InhPix++;
    InhPix = InhPix % NUM_END_P_SAMPLES;

    // calculate inhalation volume in mL, scaled.
    InhVolume = NetFlowSumInInh*2.0/60.0;
    Compensated_InhVolume = (Compensated_NetFlowSumInInh)*2.0/60.0;

    InhFlow[InhFix] = netFlow;
    InhFix++;
    InhFix = InhFix % NUM_END_P_SAMPLES;

    // return the inhalation volume
    return(InhVolume);

}   // end ProcessBdCycleInInh()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBdCycleInExh()
//
//    Processing: 
//
//    Input Parameters:
//      exhPressure - pressure measured in the expiratory limb (cmH20 * SCALE)
//      netFlow -- exhaled flow - delivered flow (ml/sec * SCALE)
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
LONG BreathData::ProcessBdCycleInExh (SHORT exhPressure, LONG netFlow)
{
    // The NetFlowSumInExh and ExhVolume calculations depend on the
    // BD_CYCLE_TIME_IN_MS being no less than 5ms.  They also assume that the
    // cycle time is a multiple of 5.  If this is no longer the case, the
    // NetFlowSumInExh and ExhVolume computations must be re-visited.

//    ExhFlowSum = AddSat(ExhFlowSum, netFlow);
    // calculate exhaled volume in mL, scaled.
    MoniExhVolume = (ExhFlowSum*2.0)/60.0;

    // Make sure the netFlow is not negative
    if(netFlow < 0)
    {
        netFlow = 0;
    }

//    NetFlowSum = AddSat(NetFlowSum, netFlow);
//    NetFlowSumInExh = AddSat(NetFlowSumInExh, netFlow);
    ExhFlow[ExhPix] = netFlow;

    if(timer200ms > 200)
    {
        ExhFlowData[ExhFDataix] = netFlow;
        ExhFDataix++;
        ExhFDataix = ExhFDataix % NUM_EXH_FLOW_SAMPLES;
    }
    else
    {
        timer200ms += 2;
    }

//    Compensated_NetFlowSumInExh = AddSat(Compensated_NetFlowSumInExh, netFlow);

    // Update the pressure sum and the number of samples in the exhalation.
    PressureSum+= exhPressure;
    NumExhSamples++;

    // Update the ExhPress buffer.  This buffer maintains the last 
    // NUM_END_P_SAMPLES which are averaged together to determine the end
    // exhalation pressre.
    ExhPress[ExhPix] = exhPressure;
    ExhPix++;
    ExhPix = ExhPix % NUM_END_P_SAMPLES;

    // calculate exhaled volume in mL, scaled.
    ExhVolume = (NetFlowSumInExh)*2.0/60.0;
    Compensated_ExhVolume = (Compensated_NetFlowSumInExh)*2.0/60.0;

    // return the exhaled volume 
    return((NetFlowSum)*2.0/60.0);

}   // end ProcessBdCycleInExh()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetEndInhPress()
//
//    Processing: 
//      This method averages the elements of the InhPress array and returns
//      the averaged value to the caller.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      SHORT avg - end inhalation pressure average
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
SHORT BreathData::GetEndInhPress (E_Bool compensated) const
{
    LONG avg = 0;

    for (SHORT j = 0; j < NUM_END_P_SAMPLES; j++)
    {
        avg += InhPress[j];
    }

    avg /= NUM_END_P_SAMPLES;

    if (eTrue == compensated)
    {
        // Compensate for baseline pressure but limit to 0.
        avg -= BaselinePress;

        if(avg < 0)
        {
            avg = 0;
        }
    }

    return((SHORT) avg);

}   // end GetEndInhPress()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetEndExhPress()
//
//    Processing: 
//      This method averages the elements of the ExhPress array and returns
//      the averaged value to the caller.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      SHORT avg - end exhalation pressure average
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
SHORT BreathData::GetEndExhPress (void) const
{

    LONG avg = 0;

    // Sum the values in the ExhPress array
    for (SHORT j = 0; j < NUM_END_P_SAMPLES; j++)
    {
        avg += ExhPress[j];
    }

    avg /= NUM_END_P_SAMPLES;

    return ((SHORT) avg);

}   // end GetEndExhPress()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ComputeVteInPhase()
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
void BreathData::ComputeVteInPhase(LONG netFlow)
{
//    LONG compensate = (LONG)Ptdata::S_GetInstance()->GetExhLeak()*SCALE;
//    compensate = 0;//not aplly yet
//    //COMPENSATE
//    netFlow = netFlow - compensate;
//    VteInPhase = AddSat(VteInPhase, netFlow);
}


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetAPRV_PHigh()
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
//
//
//    Requirements:
//
/******************************************************************************/
SHORT BreathData::GetAPRV_PHigh(void)
{
    LONG avg = 0;
    // Sum the values in the ExhPress array
    for (SHORT j = 0; j < NUM_END_P_SAMPLES; j++)
    {
         avg += PHigh[j];
    }

    avg /= NUM_END_P_SAMPLES;

    return((SHORT)avg);
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetAPRV_PLow()
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
//
//
//    Requirements:
//
/******************************************************************************/
SHORT BreathData::GetAPRV_PLow(void)
{
    LONG avg = 0;
    // Sum the values in the ExhPress array
    for (SHORT j = 0; j < NUM_END_P_SAMPLES; j++)
    {
        avg += PLow[j];
    }

    avg /= NUM_END_P_SAMPLES;

    return((SHORT)avg);
}


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetEndInhFlow()
//
//    Processing:
//      This method averages the elements of the InhFlow array and returns
//      the averaged value to the caller.
//
//    Input Parameters:
//      E_Bool compensated - indicates if the end inhalation pressure should be
//          adjusted for baseline pressure
//
//    Output Parameters:
//      None
//
//    Return Values:
//      SHORT avg - end inhalation pressure average
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
LONG BreathData::GetEndInhFlow()
{
    LONG avg = 0;

    for (SHORT j = 0; j < NUM_END_P_SAMPLES; j++)
    {
        avg += InhFlow[j];
    }

    avg /= NUM_END_P_SAMPLES;

    return(avg);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetInhVolume()
//
//    Processing: Get inhalation volume
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      InhVolume - inhalation volume
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
LONG BreathData::GetInhVolume (void)
{
    return InhVolume;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetCompensatedExhVolume()
//
//    Processing: Get compensated exhalation volume
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      Compensated_ExhVolume - compensated exhalation volume
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
LONG BreathData::GetCompensatedExhVolume (void)
{
    return Compensated_ExhVolume;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetCompensatedInhVolume()
//
//    Processing: Get compensated inhalation volume
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      Compensated_InhVolume - compensated inhalation volume
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
LONG BreathData::GetCompensatedInhVolume (void)
{
    return Compensated_InhVolume;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetMoniInhVolume()
//
//    Processing: Get monitor inhalation volume
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      MoniInhVolume - monitor inhalation volume
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
LONG BreathData::GetMoniInhVolume (void)
{
    return MoniInhVolume;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetMoniExhVolume()
//
//    Processing: Get monitor exhalation volume
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      MoniInhVolume - monitor exhalation volume
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
LONG BreathData::GetMoniExhVolume (void)
{
    return MoniExhVolume;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetTimeExhInFirstBreath (UNSIGNED value)
//
//    Processing: Set time exhalation in first breath
//
//    Input Parameters:
//      UNSIGNED value - time exhalation in first breath
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
void BreathData::SetTimeExhInFirstBreath (UNSIGNED value)
{
    TimeInExh = value;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetTimeExhInFirstBreath (void)
//
//    Processing: Get sum of flow in inhalation
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      NetFlowSumInInh - sum of flow in inhalation
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
LONG BreathData::GetNetFlowSumInInh (void) const
{
    return NetFlowSumInInh;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetNetFlowSumInExh (void)
//
//    Processing: Get sum of flow in exhalation
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      NetFlowSumInExh-sum of flow in exhalation
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
LONG BreathData::GetNetFlowSumInExh (void) const
{
    return NetFlowSumInExh;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetNetFlowSumInExh (void)
//
//    Processing: Get peak inhalation pressure
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      PeakInhPress - peak inhalation pressure
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
SHORT BreathData::GetPeakInhPress (void) const
{
    return PeakInhPress;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetNumInhSamples (void)
//
//    Processing: Get number of inhalation sample
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      NumInhSamples - number of inhalation sample
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
SHORT BreathData::GetNumInhSamples (void) const
{
    return NumInhSamples;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetNumExhSamples (void)
//
//    Processing: Get number of exhalation sample
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      NumExhSamples - number of exhalation sample
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
SHORT BreathData::GetNumExhSamples (void) const
{
    return NumExhSamples;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetNumExhSamples (void)
//
//    Processing: Get sum of pressure
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      PressureSum - sum of pressure
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
LONG BreathData::GetPressureSum (void) const
{
    return PressureSum;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetExhVolume (void)
//
//    Processing: Get exhalation volume
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      ExhVolume - exhalation volume
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
LONG BreathData::GetExhVolume (void) const
{
    return ExhVolume;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetTimeInInh (void)
//
//    Processing: Get time in inhalation
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      TimeInInh - time in inhalation
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
UNSIGNED BreathData::GetTimeInInh (void) const
{
    return TimeInInh;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetTimeInExh (void)
//
//    Processing: Get time in exhalation
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      TimeInExh - time in exhaltion
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
UNSIGNED BreathData::GetTimeInExh (void) const
{
    return TimeInExh;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetInhType (void)
//
//    Processing: Get inhalation type
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      InhType - inhalation type
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
E_BreathPhaseIndicator BreathData::GetInhType (void) const
{
    return InhType;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetPdDone (E_Bool flag)
//
//    Processing:Set pd Done with value
//
//    Input Parameters:
//      E_Bool flag
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
void BreathData::SetPdDone (E_Bool flag)
{
    PdDone = flag;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ResetVte (void)
//
//    Processing: Reset Vte
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
//
//    Requirements:
//
/******************************************************************************/
void BreathData::ResetVte (void)
{
    VteInPhase = 0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetVte (void)
//
//    Processing: Get Vte
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      VteInPhase*2.0/60.0
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
LONG BreathData::GetVte (void)
{
    return (VteInPhase*2.0/60.0);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetCompensatedNetFlowSumInInh (void)
//
//    Processing: Get sum of compensate net flow in inhalation
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      Compensated_NetFlowSumInInh-sum of compensate net flow in inhalation
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
LONG BreathData::GetCompensatedNetFlowSumInInh (void) const
{
    return Compensated_NetFlowSumInInh;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetCompensatedNetFlowSumInExh (void)
//
//    Processing: Get sum of compensate net flow in exhalation
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      Compensated_NetFlowSumInExh-sum of compensate net flow in exhalation
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
LONG BreathData::GetCompensatedNetFlowSumInExh (void) const
{
    return Compensated_NetFlowSumInExh;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ResetPeakInhPress (void)
//
//    Processing: Reset Peak Inhalation pressure
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
//
//    Requirements:
//
/******************************************************************************/
void BreathData::ResetPeakInhPress (void)
{
    PeakInhPress = MIN_PRESSURE_READING;
}


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetPdDoneFlag (void)
//
//    Processing: Get Pd Done Flag
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      PdDone - Pd Done Flag
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
E_Bool BreathData::GetPdDoneFlag (void)
{
    return PdDone;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetEndExhFlow()
//
//    Processing:
//      This method averages the elements of the InhFlow array and returns
//      the averaged value to the caller.
//
//    Input Parameters:
//      E_Bool compensated - indicates if the end inhalation pressure should be
//          adjusted for baseline pressure
//
//    Output Parameters:
//      None
//
//    Return Values:
//      SHORT avg - end inhalation pressure average
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
LONG BreathData::GetEndExhFlow()
{
    LONG avg = 0;

    for (SHORT j = 0; j < NUM_END_P_SAMPLES; j++)
    {
        avg += ExhFlow[j];
    }

    avg /= NUM_END_P_SAMPLES;
    return(avg);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: clearInhPressData()
//
//    Processing: clean all inhalation pressure data
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
//
//    Requirements:
//
/******************************************************************************/
void BreathData::clearInhPressData()
{
    for (SHORT i = 0; i < NUM_END_P_SAMPLES; i++)
    {
       InhPress[i] = 0;
    }
    InhPix = 0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name:UpdateTimeIn_APRVPLowPhase()
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
//
//
//    Requirements:
//
/******************************************************************************/
void BreathData::UpdateTimeIn_APRVPLowPhase(void)
{
    TimeInExh = APRV_PLowPhase::GetTimeDuration()
            + APRV_SupportedInhPhase_InPLow::GetTimeDuration()
            + APRV_ExhPhase_InPLow::GetTimeDuration();

    APRV_PLowPhase::ResetTimeDuration();
    APRV_SupportedInhPhase_InPLow::ResetTimeDuration();
    APRV_ExhPhase_InPLow::ResetTimeDuration();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name:APRVTriggerInhStart(E_BreathPhaseIndicator inhType)
//
//    Processing: APRV Trigger Inhalation start
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
//
//    Requirements:
//
/******************************************************************************/
void BreathData::APRVTriggerInhStart(E_BreathPhaseIndicator inhType)
{
    InhType = inhType;
    Compensated_NetFlowSumInInh = 0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name:PlateauStart(E_BreathPhaseIndicator inhType)
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
//
//
//    Requirements:
//
/******************************************************************************/
void BreathData::PlateauStart(E_BreathPhaseIndicator inhType)
{
    InhType = inhType;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation UpdateTimeIn_APRVPHighPhase(void)
//
//    Processing: Update Time in APRV  Phigh Phase
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
//
//    Requirements:
//
/******************************************************************************/
void BreathData::UpdateTimeIn_APRVPHighPhase (void)
{
    TimeInInh = APRV_PHighPhase::GetTimeDuration()
            + APRV_SupportedInhPhase_InPHigh::GetTimeDuration()
            + APRV_ExhPhase_InPHigh::GetTimeDuration();

    APRV_PHighPhase::ResetTimeDuration();
    APRV_SupportedInhPhase_InPHigh::ResetTimeDuration();
    APRV_ExhPhase_InPHigh::ResetTimeDuration();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation GetAgvExhFlow(void)
//
//    Processing:Get average exhalation flow
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      avg - average exhalation flow
//
//    Pre-Conditions:
//
//
//    Requirements:
//
/******************************************************************************/
LONG BreathData::GetAgvExhFlow (void)
{
    LONG avg = 0;

    if(ExhFDataix < 180)
    {
        for (SHORT j = ExhFDataix+1; j < ExhFDataix+20; j++)
        {
            avg += ExhFlowData[j];
        }

        avg /= 20;

    }
    else
    {
        for (SHORT j = ExhFDataix+1; j < NUM_EXH_FLOW_SAMPLES; j++)
        {
            avg += ExhFlowData[j];
        }
        for (SHORT j = 0; j < ExhFDataix - 180; j++)
        {
            avg += ExhFlowData[j];
        }
        avg /= 20;

    }
    return avg;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name:ProcessBdCycleAPRV_PHigh(SHORT pressure)
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
//
//
//    Requirements:
//
/******************************************************************************/
void BreathData::ProcessBdCycleAPRV_PHigh(SHORT pressure)
{
    PHigh[mPHighIndex] = pressure;
    mPHighIndex++;
    mPHighIndex = mPHighIndex % NUM_END_P_SAMPLES;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name:ProcessBdCycleAPRV_PLow(SHORT pressure)
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
//
//
//    Requirements:
//
/******************************************************************************/
void BreathData::ProcessBdCycleAPRV_PLow(SHORT pressure)
{
    PLow[mPLowIndex] = pressure;
    mPLowIndex++;
    mPLowIndex = mPLowIndex % NUM_END_P_SAMPLES;
}
