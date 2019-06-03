/******************************************************************************/
//$COMMON.H$
//   File Name:  NCPAP_Phase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
/******************************************************************************/
#ifndef NCPAP_PHASE_H
#define NCPAP_PHASE_H

#include "Phase.h"

class NCPAP_Phase : public Phase
{
public:
    //pointer to the object instantiated from this class
    static NCPAP_Phase* S_GetInstance(void);

    //Start, end , process phase
    virtual void Start(void);
    virtual void End(void);
    virtual void ProcessBdCycle(void);

private:
    //Constructor
    NCPAP_Phase(void);

    //copy constructor
    NCPAP_Phase(const NCPAP_Phase&);


    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of Alarm task
    //
    //    Units: None
    //
    //    Range: N/A
    static NCPAP_Phase* S_Instance;


    //$COMMON.ATTRIBUTE$
    //    Name: timer_1s
    //
    //    Description: One sec timer
    //
    //    Units: None
    //
    //    Range: N/A
    int timer_1s;
};

#endif // NCPAP_PHASE_H
