/******************************************************************************/
//$COMMON.H$
//   File Name:  APRV_ExhPhase_InPHigh.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the APRV_ExhPhase_InPHigh class
/******************************************************************************/
#ifndef APRV_EXHPHASE_INPLOW_H
#define APRV_EXHPHASE_INPLOW_H

#include "Phase.h"
#include "util.h"
//#include "PhaseMgr.h"

// DEFAULT_MIN_EXH_TIME defines the default minimum exhalation time.  A
// minimum exhalation time guarantees that the ventilator will remain in the
// exhalation phase for at least the minimum time before a trigger fires and
// causes a transition out of the exhalation phase.
// This value is used as the default by S_IsInUnrestrictedPhase()
//const UNSIGNED DEFAULT_MIN_EXH_TIME = 400/MS_PER_TICK;  // 200ms in ticks

// This value defines the bias flow offset used during the exhalation phase
// while flow triggering is active.   This value is set to 3LPM in ml/sec and
// then scaled.

class APRV_ExhPhase_InPLow : public Phase
{
public:

    //pointer to the object instantiated from this class
    static APRV_ExhPhase_InPLow* S_GetInstance(void);

    //start phase
    virtual void Start(void);
    //stop phase
    virtual void End(void);
    //Process bd cycle
    virtual void ProcessBdCycle(void);

    //time in system ticks elapsed since the start of the exhalation
    static UNSIGNED S_CalcTimeSincePhaseStart(void);

    //Get unrestricted phase
    static E_Bool S_IsInUnrestrictedPhase
    (
            UNSIGNED minExhTime = DEFAULT_MIN_EXH_TIME
    );

    //Get time duration
    static UNSIGNED GetTimeDuration(void);

    //Reset time duration
    static void ResetTimeDuration(void);

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

    //$COMMON.ATTRIBUTE$
    //  Name: S_Time
    //  Description:  Time duration
    //  Units: TICKS
    //  Range: 0 - maximum UNSIGNED value
    static UNSIGNED S_Time;

    APRV_ExhPhase_InPLow(const APRV_ExhPhase_InPLow&);
    APRV_ExhPhase_InPLow(void);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  pointer to class
    //  Units: TICKS
    //  Range: 0 - maximum UNSIGNED value
    static APRV_ExhPhase_InPLow* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: TargetPressure
    //  Description:  Pressure targeted during exhalation; this value is
    //      either the PEEP or EPAP setting.
    //  Units: cmH2O * SCALE
    //  Range: 0 - 3500
    LONG TargetPressure;

};

#endif // APRV_EXHPHASE_INPLOW_H
