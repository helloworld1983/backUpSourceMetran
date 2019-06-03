/******************************************************************************/
//$COMMON.H$
//   File Name:  MandPressInhPhase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The MandPressInhPhase is the Mandatory Pressure Inhalation Phase which
//      is used to implement both Pressure Control inhalations and mandatory
//      inhalations in NIPPV.  This class is derived from the PressInhPhase.
/******************************************************************************/
#ifndef MANDPRESSINHPHASE_H
#define MANDPRESSINHPHASE_H

#include "PressInhPhase.h"

class MandPressInhPhase : public PressInhPhase
{
public:
    //Constructor
    MandPressInhPhase (E_BdSettingId targetPressId,
                       E_BdSettingId baselinePressId);

    //Start phase
    virtual void Start (void);

    //End phase
    virtual void End (void);

protected:

private:
    //Constructor
    MandPressInhPhase (void);
    //Copy Constructor
    MandPressInhPhase (const MandPressInhPhase&);

};

#endif // MANDPRESSINHPHASE_H
