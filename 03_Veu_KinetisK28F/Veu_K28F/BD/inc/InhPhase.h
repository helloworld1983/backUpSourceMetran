/******************************************************************************/
//$COMMON.H$
//   File Name:  InhPhase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The Inhalation Phase class declared in this file is derived from
//      the Phase class.  The Inhalation Phase class is an abstract base
//      class from which all inhalation phase classes are derived.
/******************************************************************************/
#ifndef INHPHASE_H
#define INHPHASE_H

#include <Util.h>
#include "Phase.h"

// 200ms in ticks
const UNSIGNED DEFAULT_MIN_INH_TIME = 200/MS_PER_TICK;

class InhPhase : public Phase
{
public:
    //Constructor
    InhPhase (void);

    //start phase
    virtual void Start (void);

    //calculate time elapsed since the start of the inhalation phase.
    static inline UNSIGNED S_CalcTimeSincePhaseStart (void);

    //minimum exhalation time comparing against
    static E_Bool S_IsInUnrestrictedPhase(UNSIGNED minInhTime = DEFAULT_MIN_INH_TIME);

protected:

private:
    //Copy constructor
    InhPhase (const InhPhase&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_InhStartTime
    //  Description:  System time at the start of the most recent inhalation
    //  Units: TICKS
    //  Range: 0 - maximum UNSIGNED value
    static UNSIGNED S_InhStartTime;

};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_CalcTimeSincePhaseStart()
//
//    Processing: 
//      This operation invokes the CalcElapsedTime() utility to calculate
//      the time elapsed since the start of the inhalation phase.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      UNSIGNED - time in system ticks elapsed since the start of the 
//          inhalation.
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
inline UNSIGNED InhPhase::S_CalcTimeSincePhaseStart (void)
{
    // Return the time elapsed since the start of the inhalation phase.
    return (CalcElapsedTime (S_InhStartTime));

}   // end S_CalcTimeSincePhaseStart()

#endif // INHPHASE_H
