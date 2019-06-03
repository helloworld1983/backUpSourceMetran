/******************************************************************************/
//$COMMON.CPP$
//   File Name:  FlowPattern.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the FlowPattern class definition.  The FlowPattern
//      class is defined as an abstract base class.  It is only defined so
//      that FlowPatterns (Square, Ramp) can be handled similarly
//      by the VcvInhPhase.  VCV is the only type of breath phase that uses
//      the flow patterns.
//
//      An empty default constructor is defined (instead of relying on the one 
//      provided by the compiler) simply to resolve a compiler error that 
//      results because the copy constructor is declared.
//
//   Interfaces:
//      None
/******************************************************************************/
#include "FlowPattern.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: FlowPattern()
//
//    Processing: 
//      This operation is the constructor for objects of the FlowPattern class.
//      It performs no work.
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
FlowPattern::FlowPattern (void)
{
    TargetInhTime = 0;
    CompensatedPeakFlow = 0;
}   // end FlowPattern()

