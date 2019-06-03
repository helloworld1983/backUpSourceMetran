/******************************************************************************/
//$COMMON.CPP$
//   File Name:  SquareFlowPattern.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Square Flow Pattern that
//      is used when the operator set value for Flow Pattern is Square and
//      Volume Ventilation is active.  This class contains methods to 
//      calculate the target inhalation time based upon the operator settings
//      for tidal volume and peak flow, determine the compliance compensated
//      peak flow, and calculate the desired flow for a given point within
//      the inhalation.
//
//   Interfaces:
//      This class interfaces with O2Setting to determine the division of the
//      total flow command between the air and oxygen sources.
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
#include "SquareFlowPattern.h"
#include "O2Setting.h"
#include "MgrFlowController.h"

#include "DebugTrace.h"

// VCV PropRefWeight constants; scaled by 2**7
static const LONG VCV_SQUARE_REF_WEIGHT = 122;   // .95 scaled by 2**7
static const LONG VCV_RAMP_REF_WEIGHT   = 125;   // .98 scaled by 2**7
static const LONG VCV_O2_REF_WEIGHT     = 128;   // 1.0 scaled by 2**7

// define statics
SquareFlowPattern* SquareFlowPattern::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SquareFlowPattern()
//
//    Processing: 
//      This operation is the constructor for the SquareFlowPattern.  It 
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
SquareFlowPattern::SquareFlowPattern (void) :
        FlowPattern ()
{

}   // end SquareFlowPattern()

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
UNSIGNED SquareFlowPattern::CalcTargetInhTime (LONG tidalVol, LONG peakFlow)
{
    LONG inhTime;              // calculated inhalation time in msec

    ASSERTION(peakFlow > 0);

    // inhalation time is calculated as
    //
    //        V(t) ml |  sec    |  1000 ms 
    //      ----------|---------|---------
    //                |  Qpf mL |  sec    
    //
    // Note that both tidalVol and peakFlow are scaled, but these
    // scale factors cancel each other out in the division.
    //
    inhTime = (tidalVol*MS_PER_SEC) / (peakFlow); //in ms

    // Convert the Inhalation Time to TICKS and store the value in the
    // TargetInhTime attribute.
    TargetInhTime = (UNSIGNED) inhTime / MS_PER_TICK;

    // Return TargetInhTime in TICKS to the caller
    return(TargetInhTime);

}   // end CalcTargetInhTime()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CalcCompensatedPeakFlow()
//
//    Processing: 
//      This operation takes the compliance volume as an input and calculates
//      the compensated peak flow based upon the targeted inhalation time.
//      This value is stored in the CompensatedPeakFlow private attribute.
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
void SquareFlowPattern::CalcCompensatedPeakFlow (LONG complCompVol)
{
    ASSERTION(TargetInhTime > 0);

    // Calculate the compliance compensated peak flow and store this
    // value in the CompensatedPeakFlow attribute.  CompensatedPeakFlow is
    // stored in ml/sec * SCALE, so the result must be converted.
    CompensatedPeakFlow = (complCompVol * TICKS_PER_SEC) / TargetInhTime;

}   // end CalcCompensatedPeakFlow()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetDesiredFlows()
//
//    Processing: 
//      This method is invoked every BD Cycle Time while the inhalation phase
//      of a VCV breath with a square flow pattern is being delivered.  This
//      operation calls O2Setting::CalculateAirO2Flows() to calculate the 
//      distribution of the desired flow between the air and oxygen sources.
//
//    Input Parameters:
//      time -- time elapsed since the start of inhalation; not used here
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
void SquareFlowPattern::GetDesiredFlows (UNSIGNED, // not used by this flow pattern
        LONG* desiredAirPtr, LONG* desiredO2Ptr)
{

    // CalculateAirO2Flows will determine the desired BTPS compensated air
    // and oxygen flows.    
    (O2Setting::S_GetInstance ())->CalculateAirO2Flows (CompensatedPeakFlow,
                                                        desiredAirPtr,
                                                        desiredO2Ptr);

}   // end GetDesiredFlows()

/******************************************************************************/

//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the SquareFlowPattern object, it invokes
//      SquareFlowPattern::S_GetInstance(), which returns a pointer to the
//      SquareFlowPattern object.
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
//      SquareFlowPattern* - pointer to the object instantiated from this class
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
SquareFlowPattern* SquareFlowPattern::S_GetInstance (void)
{

    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new SquareFlowPattern ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()
