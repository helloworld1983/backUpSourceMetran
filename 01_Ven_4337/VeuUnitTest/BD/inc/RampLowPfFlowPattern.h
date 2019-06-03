/******************************************************************************/
//$COMMON.H$
//   File Name:  RampLowPfFlowPattern.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The RampLowPfFlowPattern class is derived from the base class
//      FlowPattern.  It is the descending ramp flow profile used when the
//      operator setting for Flow Pattern is Ramp and the Peak Flow setting
//      is less than 10 LPM.  
/******************************************************************************/
#ifndef RAMPLOWPFFLOWPATTERN_H
#define RAMPLOWPFFLOWPATTERN_H

#include "FlowPattern.h"

class RampLowPfFlowPattern : public FlowPattern
{
public:
    //pointer to the object instantiated from this class
    static RampLowPfFlowPattern* S_GetInstance (void);

    //calculates the target inhalation time
    virtual UNSIGNED CalcTargetInhTime (LONG setTidalVol, LONG setPeakFlow);

    //calculate compensated peak flow based
    virtual void CalcCompensatedPeakFlow (LONG complCompVol);

    //Get desire flow
    virtual LONG GetDesiredFlows (int);

    //Constructor
    RampLowPfFlowPattern (void);
    //Copy constructor
    RampLowPfFlowPattern (const RampLowPfFlowPattern&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static RampLowPfFlowPattern* S_Instance;

};

#endif // RAMPLOWPFFLOWPATTERN_H

