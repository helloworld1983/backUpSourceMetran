/******************************************************************************/
//$COMMON.H$
//   File Name:  FlowTrigExhPhase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The Flow Triggering Exhalation Phase class declared in this file 
//      is derived from the ExhPhase class.  The Flow Triggering Exhalation 
//      Phase is the exhalation phase active when the operator selected Trigger
//      Type is Flow.
/******************************************************************************/
#ifndef FLOWTRIGEXHPHASE_H
#define FLOWTRIGEXHPHASE_H

#include <ExhPhase.h>

#include "DebugTrace.h"

class FlowTrigExhPhase : public ExhPhase
{
public:
    //pointer to the object instantiated from this class
    static inline FlowTrigExhPhase* S_GetInstance (void);

    //Start phase
    virtual void Start (void);

    //end phase
    virtual void End (void);

    //Process Bd cycle
    virtual void ProcessBdCycle (void);

protected:

private:
    //Copy constructor
    FlowTrigExhPhase (const FlowTrigExhPhase&);

    //Constructor
    FlowTrigExhPhase (void);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static FlowTrigExhPhase* S_Instance;
};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing: 
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants to
//      access the FlowTrigExhPhase object, it calls FlowTrigExhPhase::S_GetInstance(),
//      which returns a pointer to the FlowTrigExhPhase object.
//
//      If the object has not already been created, this method instantiates 
//      it and returns a pointer to the object.  If the object has already been 
//      instantiated, a pointer to the object is returned.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      FlowTrigExhPhase* - pointer to the object instantiated from this class
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
inline FlowTrigExhPhase* FlowTrigExhPhase::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new FlowTrigExhPhase ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

#endif // FLOWTRIGEXHPHASE_H
