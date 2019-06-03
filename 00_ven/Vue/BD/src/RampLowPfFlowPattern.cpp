/******************************************************************************/
//$COMMON.CPP$
//   File Name:  RampLowPfFlowPattern.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Ramp Flow Pattern that
//      is used when the operator set value for Flow Pattern is Ramp, the
//      operator set value for peak flow is less than 10 LPM and
//      Volume Ventilation is active.  This class contains methods to 
//      calculate the target inhalation time based upon the operator settings
//      for tidal volume and peak flow, determine the compliance compensated
//      peak flow, and calculate the desired flow for a given point within
//      the breath.
//
//      If the operator set peak flow is less than 10 LPM, the ramp flow
//      trajectory starts at the peak flow and ramps down to 1/2 the peak flow.
//
//   Interfaces:
//      This class interfaces with O2Setting to determine the division of the
//      total flow command between the air and oxygen sources.
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
#include "RampLowPfFlowPattern.h"
#include "O2Setting.h"

#include "DebugTrace.h"

// define statics
RampLowPfFlowPattern* RampLowPfFlowPattern::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to this object, it invokes RampLowPfFlowPattern::S_GetInstance()
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
//      RampLowPfFlowPattern* - pointer to the object instantiated from this
//                                  class
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
RampLowPfFlowPattern* RampLowPfFlowPattern::S_GetInstance (void)
{
    // If the object has not already been instantiated, instantiated it
    if(NULL == S_Instance)
    {
        S_Instance = new RampLowPfFlowPattern ();
        ASSERTION(S_Instance != NULL);
    }

    // Return a pointer to the object
    return (S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: RampLowPfFlowPattern()
//
//    Processing: 
//      This operation is the constructor for the RampLowPfFlowPattern.  It 
//      invokes the base class FlowPattern constructor.
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
RampLowPfFlowPattern::RampLowPfFlowPattern (void) :
        FlowPattern ()
{

}   // end RampLowPfFlowPattern()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CalcTargetInhTime()
//
//    Processing: 
//      This operation calculates the target inhalation time based upon the
//      operator settings for tidal volume and peak flow.  It returns the
//      inhalation time in UNSIGNED ticks to the caller and it also stores
//      this value in the private attribute TargetInhTime.
//
//    Input Parameters:
//      tidalVolume -- operator set TV in ml * SCALE
//      peakFlow -- operator set peak flow in ml/sec * SCALE
//
//    Output Parameters:
//      None
//
//    Return Values:
//      inhalation time in ticks
//
//    Pre-Conditions:
//      peakFlow > 0
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
UNSIGNED RampLowPfFlowPattern::CalcTargetInhTime (LONG tidalVol, LONG peakFlow)
{
    LONG inhTime;              // calculated inhalation time in msec

    // For set Peak Flows less than 10 Lpm, inhalation time is calculated as
    //
    //      4* V(t) ml|    sec       | 1000 ms 
    //      ----------|--------------|---------
    //                |   3*Qpf mL   |  sec    

    ASSERTION(peakFlow > 0);
    inhTime = (4000 * tidalVol) / (3 * peakFlow);

    // Convert the Inhalation Time to TICKS and store the value in the
    // TargetInhTime attribute
    TargetInhTime = (UNSIGNED) inhTime / MS_PER_TICK;

    // Return TargetInhTime in TICKS to the caller
    return (TargetInhTime);

}   // end CalcTargetInhTime()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CalcCompensatedPeakFlow()
//
//    Processing: 
//      This operation takes the compliance compensated volume as an input and
//      calculates the compliance compensated peak flow based upon the 
//      targeted inhalation time.  This value is stored in the 
//      CompensatedPeakFlow private attribute.
//
//    Input Parameters:
//      complCompVol -- compliance compensated volume in ml * SCALE
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      TargetInhTime > 0
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void RampLowPfFlowPattern::CalcCompensatedPeakFlow (LONG complCompVol)
{
    ASSERTION(TargetInhTime > 0);

    // Calculate the compliance compensated peak flow and store this
    // value in the CompensatedPeakFlow attribute.  CompensatedPeakFlow is
    // stored in ml/sec * SCALE, so the result must be converted 

    CompensatedPeakFlow = (4 * complCompVol * TICKS_PER_SEC)
            / (3 * TargetInhTime);

}   // end CalcCompensatedPeakFlow()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetDesiredFlows()
//
//    Processing: 
//      This method is invoked every BD Cycle of the inhalation phase of a VCV
//      inhalation with a Ramp flow pattern and PF less than 10 LPM.  This
//      operation calls O2Setting::CalculateAirO2Flows() to calculate the 
//      distribution of the desired flow between the air and oxygen sources.
//
//    Input Parameters:
//      time -- time elapsed since the start of inhalation
//
//    Output Parameters:
//      desired Air - desired air flow in ml/sec * SCALE
//      desired O2 - desired O2 flow in ml/sec * SCALE
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
void RampLowPfFlowPattern::GetDesiredFlows (UNSIGNED time, LONG* desiredAirPtr,
                                            LONG* desiredO2Ptr)
{
    LONG adjustedTime;
    LONG totalFlow;     // in ml/sec * SCALE

    // Put a 60ms shelf on the ramp to allow time to reach the peak
    adjustedTime = time - (60 / MS_PER_TICK);

    if(adjustedTime < 0)
        totalFlow = CompensatedPeakFlow;
    else
    {
        totalFlow = (((0 - CompensatedPeakFlow) * (LONG) adjustedTime)
                / (2 * (LONG) TargetInhTime)) + CompensatedPeakFlow;

        if(CompensatedPeakFlow < RAMP_PEAK_FLOW_THRESHOLD)
        {
            totalFlow = (totalFlow * RAMP_TARGET_MULT) >> RAMP_TARGET_SHIFT;
        }
        else
        {
            totalFlow = (totalFlow * RAMP_TARGET_MULT_HI) >> RAMP_TARGET_SHIFT;
        }
    }

    // CalculateAirO2Flows will determine the desired BTPS compensated air
    // and oxygen flows.    
    (O2Setting::S_GetInstance ())->CalculateAirO2Flows (totalFlow,
                                                        desiredAirPtr,
                                                        desiredO2Ptr);

}   // end GetDesiredFlows()
