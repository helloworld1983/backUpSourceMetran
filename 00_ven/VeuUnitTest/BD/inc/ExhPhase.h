/******************************************************************************/
//$COMMON.H$
//   File Name:  ExhPhase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The Exhalation Phase class declared in this file is derived from
//      the Phase class.  The Exhalation Phase class is an abstract base
//      class from which all exhalation phase classes are derived.
/******************************************************************************/
#ifndef EXHPHASE_H
#define EXHPHASE_H

#include <Phase.h>
#include <Util.h>

/******************************************************************************/
//      TYPE DEFINITIONS
/******************************************************************************/
// E_AzSuperState is used to define the current state of an auto-zero test
// from the outside world's point of view.  The auto-zero test also has it's
// own internal state, which is mostly substates of the active state here.
enum E_AzSuperState
{
    eAzIdle, eAzActive, eAzPending
};

/******************************************************************************/
//      CONSTANT DEFINITIONS
/******************************************************************************/
// This value defines the bias flow offset used during the exhalation phase 
// while flow triggering is active.   This value is set to 3LPM in ml/sec and
// then scaled.
static const LONG BIAS_FLOW_OFFSET = 50 * SCALE;

/******************************************************************************/
//      CLASS DECLARATIONS
/******************************************************************************/
class ExhPhase : public Phase
{
public:
    //Constructor
    ExhPhase (void);

    //Start phase
    virtual void Start (void);

    //End phase
    virtual void End (void);

    //Process bd Cycle
    virtual void ProcessBdCycle (void);

    //calculate the time elapsed since the start of the exhalation phase
    static inline UNSIGNED S_CalcTimeSincePhaseStart (void);

    //indicates if the ventilator is at least minExhTime into exhalation phase
    static E_Bool S_IsInUnrestrictedPhase (UNSIGNED minExhTime =
            DEFAULT_MIN_EXH_TIME);

    //$COMMON.ATTRIBUTE$
    //  Name: S_TimeElapsed
    //  Description:  Time elapsed since the start of the last exhalation.
    //  Units: TICKS
    //  Range: 0 - maximum UNSIGNED value
    static UNSIGNED S_TimeElapsed;

    //$COMMON.ATTRIBUTE$
    //  Name: S_ExhStartTime
    //  Description:  System time at the start of the most recent exhalation
    //  Units: TICKS
    //  Range: 0 - maximum UNSIGNED value
    static UNSIGNED S_ExhStartTime;

    //copy constructor
    ExhPhase (const ExhPhase&);

    //$COMMON.ATTRIBUTE$
    //  Name: TargetPressure
    //  Description:  Pressure targeted during exhalation; this value is
    //      either the PEEP or EPAP setting.
    //  Units: cmH2O * SCALE
    //  Range: 0 - 3500
    LONG TargetPressure;

};

#endif // EXHPHASE_H
