/******************************************************************************/
//$COMMON.H$
//   File Name:  SupportedInhPhase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The Supported Inhalation Phase defines the PSV and NIPPV spontaneous
//      supported inhalations.  This class is derived from PressInhPhase.
/******************************************************************************/
#ifndef SUPPORTEDINHPHASE_H
#define SUPPORTEDINHPHASE_H

#include "PressInhPhase.h"

class SupportedInhPhase : public PressInhPhase
{
public:
    //Constructor
    SupportedInhPhase (E_BdSettingId targetPressId,
                       E_BdSettingId baselinePressId);

    //start phase
    virtual void Start (void);

    //end phase
    virtual void End (void);

protected:

private:
    //constructor
    SupportedInhPhase (void);
    //copy constructor
    SupportedInhPhase (const SupportedInhPhase&);

};

#endif // SUPPORTEDINHPHASE_H
