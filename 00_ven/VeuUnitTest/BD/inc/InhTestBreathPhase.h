/******************************************************************************/
//$COMMON.CPP$
//   File Name:  InhTestBreathPhase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: InhTestBreathPhase is the phase at the beginning of VA mode
//            At the beginning of VA mode, The vent starts 3 test breath to compute
//            the averaged Tidal Volume.The Insp Pressure in the test breath:
//                      Target Pressure = 0.7*(PIP setting)
//
//
/******************************************************************************/
#ifndef INHTESTBREATHPHASE_H
#define INHTESTBREATHPHASE_H

//#include "PressInhPhase.h"

class InhTestBreathPhase //: public PressInhPhase
{
public:
    //Constructor
    InhTestBreathPhase
    (
            E_BdSettingId targetPressId,
            E_BdSettingId offsetPressId
    );

    //Start phase
    virtual void Start(void);

    //end phase
    virtual void End(void);

    //Constructor
    InhTestBreathPhase(void){};
    //copy constructor
    InhTestBreathPhase(const InhTestBreathPhase&);

    int TargetPressure;
};

#endif // INHTESTBREATHPHASE_H
