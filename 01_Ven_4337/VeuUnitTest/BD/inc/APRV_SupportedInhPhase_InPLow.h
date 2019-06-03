/******************************************************************************/
//$COMMON.H$
//   File Name:  APRV_SupportedInhPhase_InPLow.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for
//the APRV_SupportedInhPhase_InPLow mode
/******************************************************************************/
#ifndef APRV_SUPPORTEDINHPHASE_INPLOW_H
#define APRV_SUPPORTEDINHPHASE_INPLOW_H

//#include "PhaseMgr.h"
//#include "PressInhPhase.h"

class APRV_SupportedInhPhase_InPLow //: public PressInhPhase
{
public:
    //Constructor
    APRV_SupportedInhPhase_InPLow
    (
            E_BdSettingId targetPressId,
            E_BdSettingId offsetPressId
    );

    //start phase
    virtual void Start(void);

    //end phase
    virtual void End(void);

    //Process Bd cycle
    virtual void ProcessBdCycle(void);

    //time elapsed since the start of the exhalation phase
    static UNSIGNED S_CalcTimeSincePhaseStart(void);

    //Get time duration
    static UNSIGNED GetTimeDuration(void);

    //Reset time duration
    static void ResetTimeDuration(void);

    //Get time elapse
    static UNSIGNED GetTimeElapsed(void);

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
    //  Description:  TIme duration
    //  Units: TICKS
    //  Range: 0 - maximum UNSIGNED value
    static UNSIGNED S_Time;

    //Constructor
    APRV_SupportedInhPhase_InPLow(void){};
    //Copy constructor
    APRV_SupportedInhPhase_InPLow(const APRV_SupportedInhPhase_InPLow&);
};


#endif // APRV_SUPPORTEDINHPHASE_INPLOW_H
