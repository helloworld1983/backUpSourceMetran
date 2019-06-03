/******************************************************************************/
//$COMMON.H$
//   File Name:  APRV_ExhPhase_InPHigh.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the APRV_ExhPhase_InPHigh class
/******************************************************************************/
#ifndef APRV_EXHPHASE_INPHIGH_H
#define APRV_EXHPHASE_INPHIGH_H

#include <InhPhase.h>
#include "util.h"

class APRV_ExhPhase_InPHigh : public InhPhase
{
public:
    //pointer to the object instantiated from this class
    static APRV_ExhPhase_InPHigh* S_GetInstance(void);

    //Start phase
    virtual void Start(void);

    //End phase
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
    static inline UNSIGNED GetTimeDuration(void){return S_Time;}

    //Reset time duration
    static inline void ResetTimeDuration(void){S_Time = 0;}

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

    static UNSIGNED S_Time;

private:
    //Copy constructor
    APRV_ExhPhase_InPHigh(const APRV_ExhPhase_InPHigh&);

    //Constructor
    APRV_ExhPhase_InPHigh(void);

    static APRV_ExhPhase_InPHigh* S_Instance;

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

#endif // APRV_ExhPhase_InPHigh_H
