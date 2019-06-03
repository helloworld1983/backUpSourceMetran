/******************************************************************************/
//$COMMON.H$
//   File Name:  AcMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the VA_SIMV_Mode Mode
//      which is derived from the VA_AC_Mode base class.
/******************************************************************************/
#ifndef VA_SIMV_MODE_H
#define VA_SIMV_MODE_H

#include "PhaseMgr.h"
#include "VA_AC_Mode.h"

class VA_SIMV_Mode: public VA_AC_Mode
{
public:
    //Pointer to the object instantiated from this class
    static VA_SIMV_Mode* S_GetInstance(void);

    //Get Inh Phase Type
    E_PhaseId GetInhPhaseType(void);

protected:

private:
    //Copy constructor
    VA_SIMV_Mode(const VA_SIMV_Mode&);

    //Constructor
    VA_SIMV_Mode(void);

    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id);

    //Process doing something when another mode enters this mode
    void Enter(E_VentilationMode fromModeId);

    //Process Pt Inh Trigger
    virtual void ProcessPtInhTrigger(void);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static VA_SIMV_Mode* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: InhPhaseType
    //  Description:  Inh Phase Type
    //  Units: None
    //  Range: n/a
    E_PhaseId InhPhaseType;

};

#endif // VA_SIMV_MODE_H
