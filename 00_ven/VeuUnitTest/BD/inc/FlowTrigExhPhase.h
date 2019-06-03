/******************************************************************************/
//$COMMON.H$
//   File Name:  FlowTrigExhPhase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The Flow Triggering Exhalation Phase class declared in this file 
//      is derived from the ExhPhase class.  The Flow Triggering Exhalation 
//      Phase is the exhalation phase active when the operator selected Trigger
//      Type is Flow.
/******************************************************************************/
#ifndef FLOWTRIGEXHPHASE_H
#define FLOWTRIGEXHPHASE_H

#include <ExhPhase.h>

#include "DebugTrace.h"

class FlowTrigExhPhase : public ExhPhase
{
public:
    //pointer to the object instantiated from this class
    static inline FlowTrigExhPhase* S_GetInstance (void);

    //Start phase
    virtual void Start (void);

    //end phase
    virtual void End (void);

    //Process Bd cycle
    virtual void ProcessBdCycle (void);

    //Copy constructor
    FlowTrigExhPhase (const FlowTrigExhPhase&);

    //Constructor
    FlowTrigExhPhase (void);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static FlowTrigExhPhase* S_Instance;
};



#endif // FLOWTRIGEXHPHASE_H
