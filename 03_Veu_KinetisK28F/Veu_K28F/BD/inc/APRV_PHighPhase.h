/******************************************************************************/
//$COMMON.H$
//   File Name:  APRV_PHighPhase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the APRV_PHighPhase class
/******************************************************************************/
#ifndef APRV_PHIGHPHASE_H
#define APRV_PHIGHPHASE_H

#include <InhPhase.h>
#include "PhaseMgr.h"
#include "util.h"

class APRV_PHighPhase : public InhPhase
{
public:
    //pointer to the object instantiated from this class
    static APRV_PHighPhase* S_GetInstance(void);

    //start phase
    virtual void Start(void);

    //end phase
    virtual void End(void);

    //Process Bd Cycle
    virtual void ProcessBdCycle(void);

    //calculate time elapsed since the start of the exhalation phase
    static UNSIGNED S_CalcTimeSincePhaseStart(void);

    //indicates if the ventilator is at least minExhTime into exhalation phase
    static E_Bool S_IsInUnrestrictedPhase
    (
            UNSIGNED minExhTime = DEFAULT_MIN_EXH_TIME
    );

    //Get time duration
    static UNSIGNED GetTimeDuration(void);

    //Get time elapsed
    static UNSIGNED GetTimeElapsed(void);

    //Reset time duration
    static void ResetTimeDuration(void);

protected:

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
    static UNSIGNED S_InhStartTime;

    //$COMMON.ATTRIBUTE$
    //  Name: S_Time
    //  Description:  Time duration
    //  Units:
    //  Range:
    static UNSIGNED S_Time;

private:
    //Copy constructor
    APRV_PHighPhase(const APRV_PHighPhase&);

    //Constructor
    APRV_PHighPhase(void);

    static APRV_PHighPhase* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: TargetPressure
    //  Description:  Pressure targeted during exhalation; this value is
    //      either the PEEP or EPAP setting.
    //  Units: cmH2O * SCALE
    //  Range: 0 - 3500
    LONG TargetPressure;

    //$COMMON.ATTRIBUTE$
    //  Name: RiseTime
    //  Description:  Rise Time
    //  Units:
    //  Range:
    LONG RiseTime;

};

#endif // APRV_PHighPhase_H
