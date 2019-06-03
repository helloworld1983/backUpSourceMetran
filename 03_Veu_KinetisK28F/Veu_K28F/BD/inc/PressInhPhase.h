/******************************************************************************/
//$COMMON.H$
//   File Name:  PressInhPhase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The PressInhPhase class is derived from the InhPhase class and serves
//      as a base class for all pressure based mandatory and supported
//      inhalation phases.
/******************************************************************************/
#ifndef PRESSINHPHASE_H
#define PRESSINHPHASE_H

#include <InhPhase.h>
#include "BdSetting.h"

class PressInhPhase : public InhPhase
{
public:
    //Constructor
    PressInhPhase (E_BdSettingId inhPressId, E_BdSettingId baselinePressId);

    //start phase
    virtual void Start (void);

    //end phase
    virtual void End (void);

    //process Bd cycle
    virtual void ProcessBdCycle (void);

protected:

    //$COMMON.ATTRIBUTE$
    //  Name: InhPressId
    //  Description:
    //      ID of the setting used as the inhalation pressure.  During
    //      PSV, this value is the Support Pressure Setting ID.  During
    //      PCV, it is the Inhalation Pressure Setting ID.  And, during
    //      NIPPV, this value is the IPAP ID.
    //  Units: None
    //  Range: eSupportPress, ePcvInhPress, eNippvIpap
    E_BdSettingId InhPressId;

    //$COMMON.ATTRIBUTE$
    //  Name: InhPressId
    //  Description:
    //      ID of the setting used as the inhalation pressure.  During
    //      PSV, this value is the Support Pressure Setting ID.  During
    //      PCV, it is the Inhalation Pressure Setting ID.  And, during
    //      NIPPV, this value is the IPAP ID.
    //  Units: None
    //  Range: eBaselinePress, eDummyBaselinePress (0)
    E_BdSettingId BaselinePressId;      // dummy peep (0) or peep

    //$COMMON.ATTRIBUTE$
    //  Name: InitialPressure
    //  Description:
    //      Pressure read in the exhalation limb at the start of
    //      the inhalation phase
    //  Units: cmH2O * SCALE
    //  Range: -100 to 200  cmH2O, scaled
    LONG InitialPressure;

    //$COMMON.ATTRIBUTE$
    //  Name: TargetPressure
    //  Description:
    //      Pressure targeted during inhalation; this value is
    //      the InhPress + BaselinePress.  Therefore, in PSV, the target
    //      pressure is SupportPressure + PEEP.  In PCV, it is PCV InhPress
    //      + PEEP.  And, in NIPPV, it is IPAP + 0.
    //  Units: cmH2O * SCALE
    //  Range: 2 - 135 cmH2O, scaled
    LONG TargetPressure;

    //$COMMON.ATTRIBUTE$
    //  Name: RiseTime
    //  Description:  Time to rise from InitialPressure to TargetPressure.
    //  Units: System Clock ticks
    //  Range: 100 - 900 ms in TICKS
    UNSIGNED RiseTime;

    //$COMMON.ATTRIBUTE$
    //  Name: OffsetPressId
    //  Description:
    //      ID of the setting used as the offset pressure.  During PCV &
    //      PSV, this value is the Baseline Pressure (PEEP) Setting ID.
    //      During NIPPV, this value is the Dummy Baseline Pressure (0).
    //  Units: None
    //  Range: eBaselinePress, eDummyBaselinePress (0)
    E_BdSettingId OffsetPressId;      // dummy peep (0) or peep

    //$COMMON.ATTRIBUTE$
    //  Name: BaseLine
    //  Description:
    //      Pressure read in the exhalation limb at the start of
    //      the inhalation phase
    //  Units: cmH2O * SCALE
    //  Range: -100 to 200  cmH2O, scaled
    LONG BaseLine;

private:
    //constructor
    PressInhPhase (void);
    //Copy constructor
    PressInhPhase (const PressInhPhase&);

};

#endif // PRESSINHPHASE_H
