/******************************************************************************/
//$COMMON.H$
//   File Name:  FlowPattern.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the Flow Pattern class.
//      The FlowPattern class declares several pure virtual methods to lay out
//      a consistent structure for all Flow Patterns.  Flow Patterns are used
//      during VCV inhalations to define the flow trajectory.
/******************************************************************************/
#ifndef FLOWPATTERN_H
#define FLOWPATTERN_H

#include "CommonConstants.h"

// PF_THRESHOLD_IN_SCALED_MLPS defines the peak flow limit used to determine
// which Ramp Flow Pattern to use.  There are 2 flavors of the Ramp Flow
// shape.  If the operator set peak flow is greater than or equal to 10 Lpm,
// the flow shape starts at the peak flow and follows a descending 
// ramp pattern down to 5 Lpm.  If the operator set peak flow is less than
// 10 Lpm, the flow shape starts at the peak flow and follows a descending
// ramp pattern down to 1/2 of the peak flow.  This constant defines 10 Lpm
// in ml per second, scaled.
static const LONG PF_THRESHOLD_IN_SCALED_MLPS = (10 * 1000 * SCALE) / 60;

// After 20ms, the targeted flow for ramp is increased.
static const UNSIGNED INCREASE_RAMP_TIME = 20 / MS_PER_TICK;

// Flow Target is increased by 5% when ramp is used.
static const LONG RAMP_TARGET_MULT = 269; // 1.05 * 2**8  (for peak flow <  60 lpm)
static const LONG RAMP_TARGET_MULT_HI = 264; // 1.03 * 2**8  (for peak flow >= 60 lpm)
static const LONG RAMP_TARGET_SHIFT = 8;
static const LONG RAMP_PEAK_FLOW_THRESHOLD = 100000;  // 1000 ml/sec * 100

class FlowPattern
{
public:

    //calculate target inhalation Time
    virtual UNSIGNED CalcTargetInhTime (LONG setTidalVol,
                                        LONG setPeakFlow){};

    //Calculate compensated peak flow
    virtual void CalcCompensatedPeakFlow (LONG complCompVol){};

    //Get desired flow
    virtual void GetDesiredFlows (UNSIGNED time, LONG* desiredAirPtr,
                                  LONG* desiredO2Ptr){};

    //Set compensated peak Flow
    inline void SetCompensatedPeakFlow (LONG setPeakFlow);
    inline LONG GetCompensatedPeakFlow(void);

    //Constructor
    FlowPattern (void);

    //$COMMON.ATTRIBUTE$
    //  Name: TargetInhTime
    //  Description:
    //      VCV Inhalation Time calculated based upon operator
    //      settings for peak flow, flow pattern and tidal volume.
    //  Units: TICKS
    //  Range: 0 - maximum VCV Inhalation Time (9 seconds)
    UNSIGNED TargetInhTime;

    //$COMMON.ATTRIBUTE$
    //  Name: CompensatedPeakFlow
    //  Description:  Compliance compensated peak flow
    //  Units: ml/sec * SCALE
    //  Range: 0 - 3333 ml/sec * SCALE; 0 - 200 Lpm
    LONG CompensatedPeakFlow;

    // copy constructor not used
    FlowPattern (const FlowPattern&);


};

#endif // FLOWPATTERN_H
