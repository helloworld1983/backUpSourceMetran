/******************************************************************************/
//$COMMON.H$
//   File Name:  PressTrigExhPhase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The Pressure Triggering Exhalation Phase class declared in this file 
//      is derived from the ExhPhase class.  The Pressure Triggering Exhalation 
//      Phase is the exhalation phase active when the operator selected Trigger
//      Type is Pressure.
//
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      01    01/17/97  K.A. McGuigan       Morph
//      Description: Initial Revision
//
/******************************************************************************/
#ifndef PRESSTRIGEXHPHASE_H
#define PRESSTRIGEXHPHASE_H

#include "ExhPhase.h"

class PressTrigExhPhase : public ExhPhase
{
public:
    //constructor
    PressTrigExhPhase (void);

    //start phase
    virtual void Start (void);

    //End phase
    virtual void End(void);

    //copy constructor
    PressTrigExhPhase (const PressTrigExhPhase&);

};

#endif // PRESSTRIGEXHPHASE_H

