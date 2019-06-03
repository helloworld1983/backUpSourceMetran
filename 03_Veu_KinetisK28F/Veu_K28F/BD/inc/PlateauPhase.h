/******************************************************************************/
//$COMMON.H$
//   File Name:  PlateauPhase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The Plateau Phase class declared in this file is derived from
//      the Phase class.  The Plateau Phase is an extension of a VCV inhalation
//      when the operator setting for Plateau Time is greater than 0.
/******************************************************************************/
#ifndef PLATEAUPHASE_H
#define PLATEAUPHASE_H

#include "Phase.h"

class PlateauPhase : public Phase
{
public:
    //pointer to the object instantiated from this class
    static PlateauPhase* S_GetInstance (void);

    //Start phase
    virtual void Start (void);

    //end phase
    virtual void End (void);

    //process Bd Cycle
    virtual void ProcessBdCycle (void);

protected:

private:
    //Constructor
    PlateauPhase (void);

    //Copy constructor
    PlateauPhase (const PlateauPhase&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static PlateauPhase* S_Instance;
};

#endif // PLATEAUPHASE_H
