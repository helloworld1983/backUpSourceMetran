/******************************************************************************/
//$COMMON.H$
//   File Name:  RampHighPfFlowPattern.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The RampHighPfFlowPattern class is derived from the base class
//      FlowPattern.  It is the descending ramp flow profile used when the
//      operator setting for Flow Pattern is Ramp and the Peak Flow setting
//      is greater than or equal to 10 LPM.  
/******************************************************************************/
#ifndef RAMPHIGHPFFLOWPATTERN_H
#define RAMPHIGHPFFLOWPATTERN_H

#include "FlowPattern.h"

class RampHighPfFlowPattern : public FlowPattern
{
public:
    //pointer to the object instantiated from this class
    static RampHighPfFlowPattern* S_GetInstance (void);

    //calculates the target inhalation time
    virtual UNSIGNED CalcTargetInhTime (LONG setTidalVol, LONG setPeakFlow);

    //calculate compensated peak flow based
    virtual void CalcCompensatedPeakFlow (LONG complCompVol);

    //Get desire flow
    virtual LONG GetDesiredFlows (int );

    //Coonstructor
    RampHighPfFlowPattern (void);

    //Copy constructor
    RampHighPfFlowPattern (const RampHighPfFlowPattern&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static RampHighPfFlowPattern* S_Instance;

};

#endif // RAMPHIGHPFFLOWPATTERN_H

