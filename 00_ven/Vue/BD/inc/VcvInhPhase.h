/******************************************************************************/
//$COMMON.H$
//   File Name:  VcvInhPhase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The VcvInhPhase is the Volume Control Ventilation Inhalation
//      Phase.  This class is derived from the base class InhPhase.
/******************************************************************************/
#ifndef VCVINHPHASE_H
#define VCVINHPHASE_H

#include <InhPhase.h>
#include "FlowPattern.h"

#include "DebugTrace.h"

//  NUM_INH_PRESS_SAMPLES is the number of end inhalation pressure samples used
//  to compliance compensate delivered volume breaths.
const SHORT NUM_INH_PRESS_SAMPLES = 8;

class VcvInhPhase : public InhPhase
{
public:
    //pointer to the object instantiated from this class
    static VcvInhPhase* S_GetInstance (void);

    //Start phase
    virtual void Start (void);

    //end phase
    virtual void End (void);

    //process Bd Cycle
    virtual void ProcessBdCycle (void);

    //Reset end inhalation pressure
    void ResetEndInhPress (void);

    //initialization for error vt
    void InitErrorCompensate(void);

    //Get compute volume
    LONG GetComplVol(void);

    //return active FlowPattern object
    FlowPattern* GetFlowPatternPtr(void);

    //Reset volume adjustmet
    void ResetVolumeAdjustment(void);

    //Set phase start
    void SetPhaseStarted(E_Bool value);

    //Get phase start
    E_Bool GetPhaseStarted(void);

protected:

private:
    //Copy constructor
    VcvInhPhase (const VcvInhPhase&);

    //Constructor
    VcvInhPhase (void);

    //operator set Tidal Volume in ml * SCALE
    LONG CalcComplCompVol (LONG setTidalVol);

    //
    LONG DoP(void);

    UNSIGNED InhTime;

    //$COMMON.ATTRIBUTE$
    //  Name: VolumeAdjustment
    //  Description:  store volume adjustment value use in P control
    //  Units: None
    //  Range: n/a
    LONG VolumeAdjustment;

    //$COMMON.ATTRIBUTE$
    //  Name: IsVCVInhPhaseStarted
    //  Description:
    //  Units: None
    //  Range: n/a
    E_Bool IsVCVInhPhaseStarted;

    //$COMMON.ATTRIBUTE$
    //  Name: EndInhPress
    //  Description:  Array of the last NUM_INH_PRESS_SAMPLES end inhalation
    //      pressure readings; these values are averaged to calculate
    //      compliance compensation
    //  Units: None
    //  Range: n/a
    SHORT EndInhPress[NUM_INH_PRESS_SAMPLES];

    //$COMMON.ATTRIBUTE$
    //  Name: EndInhPressIx
    //  Description:  Index into the EndInhPress array; indicates where
    //      the next end inhalation pressure will be stored
    //  Units: None
    //  Range: 0 - (NUM_INH_PRESS_SAMPLES-1)
    SHORT EndInhPressIx;

    //$COMMON.ATTRIBUTE$
    //  Name: NumEndInhPressSamples
    //  Description:  Number of EndInhPress samples in the array.
    //  Units: None
    //  Range: 0 - NUM_INH_PRESS_SAMPLES
    SHORT NumEndInhPressSamples;

    //$COMMON.ATTRIBUTE$
    //  Name: FlowPatternPtr
    //  Description:  Pointer to the active FlowPattern object
    //  Units: None
    //  Range: n/a
    FlowPattern* FlowPatternPtr;

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static VcvInhPhase* S_Instance;


    //$COMMON.ATTRIBUTE$
    //  Name: ComplVol
    //  Description:
    //  Units: None
    //  Range: n/a
    LONG ComplVol;

    //$COMMON.ATTRIBUTE$
    //  Name: PrevcomplCompVol
    //  Description:
    //  Units: None
    //  Range: n/a
    LONG PrevcomplCompVol;

    //$COMMON.ATTRIBUTE$
    //  Name: PrevsetTidalVol
    //  Description:
    //  Units: None
    //  Range: n/a
    LONG PrevsetTidalVol;

    //$COMMON.ATTRIBUTE$
    //  Name:
    //  Description:
    //  Units: None
    //  Range: n/a
    double errorvt;

    //$COMMON.ATTRIBUTE$
    //  Name:
    //  Description:
    //  Units: None
    //  Range: n/a
    LONG prev_errorvt;

    //$COMMON.ATTRIBUTE$
    //  Name:
    //  Description:
    //  Units: None
    //  Range: n/a
    LONG VolumeErrBuf[NUM_INH_PRESS_SAMPLES];

    //$COMMON.ATTRIBUTE$
    //  Name:
    //  Description:
    //  Units: None
    //  Range: n/a
    SHORT VolumeErrIx;

    //$COMMON.ATTRIBUTE$
    //  Name:
    //  Description:
    //  Units: None
    //  Range: n/a
    SHORT VolumeBrCnt;

};

#endif // VCVINHPHASE_H
