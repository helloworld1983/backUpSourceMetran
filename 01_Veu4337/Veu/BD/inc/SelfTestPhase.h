/******************************************************************************/
//$COMMON.H$
//   File Name:  SelfTestPhase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: The SelfTestPhase class is derived from the Phase class.
//      It serves the specific purpose of self test .
/******************************************************************************/
#ifndef SELFTESTPHASE_H
#define SELFTESTPHASE_H

#include "Phase.h"

class SelfTestPhase : public Phase
{
public:
    //constructor
    SelfTestPhase(void);

    //start, end, process Phase
    virtual void Start(void);
    virtual void End(void);
    virtual void ProcessBdCycle(void);

private:
    //copy constructor
    SelfTestPhase(const SelfTestPhase&);
};

#endif // SELFTESTPHASE_H
