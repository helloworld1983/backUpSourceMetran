/******************************************************************************/
//$COMMON.H$
//   File Name:  SquareFlowPattern.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the Square Flow Pattern.
//      The SquareFlowPattern class is derived from the base class FlowPattern.
/******************************************************************************/
#ifndef SQUAREFLOWPATTERN_H
#define SQUAREFLOWPATTERN_H
#include "FlowPattern.h"
#include "DebugTrace.h"

class SquareFlowPattern : public FlowPattern
{
public:
    //pointer to the object instantiated from this class
    static SquareFlowPattern* S_GetInstance (void);

    //calculates the target inhalation time
    virtual UNSIGNED CalcTargetInhTime (LONG setTidalVol, LONG setPeakFlow);

    //calculate compensated peak flow based
    virtual void CalcCompensatedPeakFlow (LONG complCompVol);

    //Get desire flow
    virtual void GetDesiredFlows (UNSIGNED time, LONG* desiredAirPtr,
                                  LONG* desiredO2Ptr);

    //Constructor
    SquareFlowPattern (void);

    //Copy constructor
    SquareFlowPattern (const SquareFlowPattern&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static SquareFlowPattern* S_Instance;
};

#endif // SQUAREFLOWPATTERN_H

