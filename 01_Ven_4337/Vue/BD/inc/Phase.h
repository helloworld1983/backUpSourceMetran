/******************************************************************************/
//$COMMON.H$
//   File Name:  Phase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The Phase class declared in this file is an abstract base
//      class from which all breath phase classes are derived.  This class 
//      was created to define the high level structure of all phases.  
/******************************************************************************/
#ifndef PHASE_H
#define PHASE_H

#include "CommonConstants.h"

// DEFAULT_MIN_EXH_TIME defines the default minimum exhalation time.  A
// minimum exhalation time guarantees that the ventilator will remain in the
// exhalation phase for at least the minimum time before a trigger fires and
// causes a transition out of the exhalation phase.
// This value is used as the default by S_IsInUnrestrictedPhase()
const UNSIGNED DEFAULT_MIN_EXH_TIME = 1000/MS_PER_TICK;  // 200ms in ticks

class Phase
{
public:
    //Start phase
    virtual void Start (void) = 0;

    //End phase
    virtual void End (void) = 0;

    //Process Bd cycle
    virtual void ProcessBdCycle (void) = 0;

protected:
    //Constructor
    Phase (void);

    //Destructor
    virtual ~Phase();

private:
    //Copy constructor
    Phase (const Phase&);
};

#endif // PHASE_H
